#ifndef HEADER_H
#define HEADER_H

#include <arpa/inet.h>
#include <cstring>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __unix__
#include <limits.h>
#include <sys/sendfile.h>

#elif __APPLE__
#include <sys/syslimits.h>

#endif

#define PORT_NUM_GET 6378
#define PORT_NUM_UPDATE 6377

#define NIL_CHAR '_'
#define NIL_STR "_"

#endif