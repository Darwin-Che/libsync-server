import socket
import subprocess
import time
from unittest import (
    TestCase,
    main,
)

LOG_FILE = "./itest-log.txt"
log_file = open(LOG_FILE, "w")

HOST, PORT_CONTROL, PORT_UPDATE = "localhost", 6378, 6377
print("HOST=%s, PORT_CONTROL=%s, PORT_UPDATE=%s\n" % (HOST, PORT_CONTROL, PORT_UPDATE))

PROGRAM, ARGS = "./sync", ""
print("PROGRAM=%s, ARGS=%s\n" % (PROGRAM, ARGS))

TIMEOUT = 3  # 3 seconds

CHNL_NAME = "c1"


class BaseTest(TestCase):
    def setUp(self):
        print(f"Starting {self.__class__.__name__}")
        self.process = subprocess.Popen(
            [PROGRAM, *ARGS], stdout=log_file, stderr=log_file
        )
        print("Setup the Main Socket")
        self.main_socket = self.connect(HOST, PORT_CONTROL)  # On channel default
        print("Setup the Update Socket")
        self.update_socket = self.connect(HOST, PORT_UPDATE)
        print("Setup the Control Socket")
        self.control_socket = self.connect(HOST, PORT_CONTROL)

    def connect(self, host, port):
        ret = None
        while True:
            try:
                ret = socket.create_connection((host, port), TIMEOUT)
                break
            except socket.error:
                print("Connection Failed, Retrying..")
                time.sleep(1.0)
        print("Connection Success")
        print()
        return ret

    def tearDown(self):
        self.update_socket.close()
        self.control_socket.close()
        self.main_socket.close()

        self.process.terminate()
        self.process.wait()

    def send(self, socket, msg):
        msg += "\n"
        socket.sendall(msg.encode("ascii"))

    def assertRecv(self, socket, msg=None):
        text = ""
        while True:
            data = socket.recv(1).decode("ascii")
            if data == "\n":
                break
            text += data
        if msg is None:
            return
        self.assertEqual(text, msg)

    def setUpChannel(self):
        self.send(self.control_socket, f"chnlcreate {CHNL_NAME}")
        self.assertRecv(self.control_socket, "CHANNEL CREATED")
        self.send(self.control_socket, f"chnlset {CHNL_NAME}")
        self.assertRecv(self.control_socket, "CHANNEL IS SET")
        self.send(self.update_socket, f"chnlset {CHNL_NAME}")
        self.assertRecv(self.update_socket, "CHANNEL IS SET")

    def closeSocket(self, socket_name):
        old_socket = getattr(self, socket_name)
        old_socket.close()

    def startSocket(self, socket_name):
        port = None

        if socket_name == "control_socket" or socket_name == "main_socket":
            port = PORT_CONTROL
        elif socket_name == "update_socket":
            port = PORT_UPDATE

        if port is None:
            print("restart socket #{socket_name} fails")
            return

        new_socket = self.connect(HOST, port)
        setattr(self, socket_name, new_socket)

    def restartSocket(self, socket_name):
        self.closeSocket(socket_name)
        self.startSocket(socket_name)


class TestControlBasic(BaseTest):
    def test(self):
        self.send(self.control_socket, "hget k1 f1")
        self.assertRecv(self.control_socket, "_")
        self.send(self.control_socket, "hset k1 f1 v1")
        self.assertRecv(self.control_socket, "v1")
        self.send(self.control_socket, "hget k1 f1")
        self.assertRecv(self.control_socket, "v1")


class TestUpdateBasic(BaseTest):
    def test(self):
        self.setUpChannel()
        self.send(self.control_socket, "hget k1 f1")
        self.assertRecv(self.control_socket, "_")
        self.send(self.control_socket, "hset k1 f1 v1")
        self.assertRecv(self.control_socket, "v1")
        self.assertRecv(self.update_socket, "hset k1 f1 v1")
        self.send(self.control_socket, "hget k1 f1")
        self.assertRecv(self.control_socket, "v1")


class TestUpdateFromMain(BaseTest):
    def test(self):
        self.setUpChannel()
        self.send(self.control_socket, "hget k1 f1")
        self.assertRecv(self.control_socket, "_")
        self.send(self.main_socket, "hset k1 f1 v1")
        self.assertRecv(self.main_socket, "v1")
        self.assertRecv(self.update_socket, "hset k1 f1 v1")
        self.send(self.control_socket, "hget k1 f1")
        self.assertRecv(self.control_socket, "v1")


class TestControlClosed(BaseTest):
    def test(self):
        self.setUpChannel()
        self.send(self.control_socket, "hget k1 f1")
        self.assertRecv(self.control_socket, "_")
        self.send(self.control_socket, "quit")
        self.assertRecv(self.control_socket, "QUIT")
        self.send(self.main_socket, "hset k1 f1 v1")
        self.assertRecv(self.main_socket, "v1")
        self.assertRecv(self.update_socket, "hset k1 f1 v1")


class TestUpdateRestarted(BaseTest):
    def test(self):
        self.setUpChannel()
        self.send(self.control_socket, "hget k1 f1")
        self.assertRecv(self.control_socket, "_")
        self.closeSocket("update_socket")
        self.send(self.main_socket, "hset k1 f1 v1")
        self.assertRecv(self.main_socket, "v1")

        self.startSocket("update_socket")
        self.send(self.update_socket, f"chnlset {CHNL_NAME}")
        self.assertRecv(self.update_socket, "CHANNEL IS SET")
        # TODO: currently only receive the pending updates until a new update is sent
        self.send(self.main_socket, "hset k1 f1 v2")
        self.assertRecv(self.update_socket, "hset k1 f1 v1")
        self.assertRecv(self.update_socket, "hset k1 f1 v2")


if __name__ == "__main__":
    main()
