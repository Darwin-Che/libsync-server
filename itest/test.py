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


class BaseTest(TestCase):
    def setUp(self):
        # Start the under-test program
        self.process = subprocess.Popen(
            [PROGRAM, *ARGS], stdout=log_file, stderr=log_file
        )
        self.main_socket = self.connect(HOST, PORT_CONTROL)  # On channel default
        self.update_socket = self.connect(HOST, PORT_UPDATE)
        self.control_socket = self.connect(HOST, PORT_CONTROL)

    def connect(self, host, port):
        ret = None
        # Connect to the program
        while True:
            try:
                ret = socket.create_connection((host, port), TIMEOUT)
                break
            except socket.error:
                print("Connection Failed, Retrying..")
                time.sleep(0.5)
        return ret

    def tearDown(self):
        # self.update_socket.sendall(b"quit")
        self.update_socket.close()
        # self.control_socket.sendall(b"quit")
        self.control_socket.close()
        # self.main_socket.sendall(b"quit")
        self.main_socket.close()

        self.process.terminate()
        self.process.wait()

    def send(self, socket, msg):
        msg += "\n"
        socket.sendall(msg.encode("ascii"))

    def assertRecv(self, socket, msg=None):
        ret = socket.recv(1024)
        if msg is None:
            return
        ret = ret.decode("ascii")
        self.assertEqual(ret[-1], "\n")
        self.assertEqual(ret[:-1], msg)

    def setUpChannel(self):
        self.send(self.control_socket, "chnlcreate c1")
        self.assertRecv(self.control_socket, "CHANNEL CREATED")
        self.send(self.control_socket, "chnlset c1")
        self.assertRecv(self.control_socket, "CHANNEL IS SET")
        self.send(self.update_socket, "chnlset c1")
        self.assertRecv(self.update_socket, "CHANNEL IS SET")


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


if __name__ == "__main__":
    main()
