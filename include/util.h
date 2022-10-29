#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

// const char * log_path = "log.txt";
// FILE * log_fptr = fopen(log_path, "w");

namespace Util {

struct StrCmp {
  bool operator()(char const *a, char const *b) const {
    return std::strcmp(a, b) < 0;
  }
};

extern StrCmp str_cmp;

void errExit(const char *msg);

void errExit();

template <typename T, typename... Types> void errExit(T var1, Types... var2) {
  std::cout << var1 << ' ';
  errExit(var2...);
}

int get_word(char *buffer, ssize_t sz, ssize_t &idx, ssize_t &len);

ssize_t read_line(int fd, void *buffer, size_t n, bool rstrip);

ssize_t bi_writen(int fd, const void *buffer, size_t sz, size_t perlimit = 0);

ssize_t write_str(int fd, const char *buffer, size_t perlimit = 0);

char *copy_str(const char *str);

} // namespace Util

#endif