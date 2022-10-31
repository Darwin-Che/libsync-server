#include "util.h"
#include <iostream>
#include <unistd.h>

namespace Util {

using namespace std;

StrCmp str_cmp;

void errExit(const char *msg) {
  cout << msg << endl;
  exit(1);
}

void errExit() {
  cout << endl;
  exit(1);
}

/* Return values:
 * -1 : fail
 * 1 : success
 */
int get_word(char *buffer, ssize_t sz, ssize_t &idx, ssize_t &len) {
  idx += len;
  while (idx < sz and buffer[idx] != '\0' and isspace(buffer[idx]))
    idx += 1;
  if (idx >= sz or buffer[idx] == '\0')
    return -1;
  len = 1;
  while (idx + len < sz and buffer[idx + len] != '\0' and
         !isspace(buffer[idx + len]))
    len += 1;
  return 1;
}

/* Return values:
 * -1 : fails
 * 0  : nothing is read, no newline appended;
 * >0 : number of bytes read, including the endding '\n'
 */
ssize_t read_line(int fd, void *buffer, size_t n, bool rstrip) {
  ssize_t numRead;
  size_t totRead;
  char *buf;
  char ch;

  if (n <= 0 || buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  buf = (char *)buffer;
  totRead = 0;
  for (;;) {
    numRead = read(fd, &ch, 1);

    if (numRead == -1) {
      if (errno == EINTR)
        continue;
      else
        return -1;

    } else if (numRead == 0) {
      if (totRead == 0)
        return 0;
      else
        break;

    } else {
      totRead++;
      if (totRead < n) {
        *buf++ = ch;
      }
      if (ch == '\n') {
        break;
      }
    }
  }
  *buf = '\0';
  if (rstrip && *(buf - 1) == '\n')
    *(buf - 1) = '\0';
  return totRead;
}

/* Return values:
 * -1 : fails
 * >=0  : number of bytes written
 */
ssize_t bi_writen(int fd, const void *buffer, size_t sz, size_t perlimit) {
  char *buf = (char *)buffer;
  ssize_t numWritten = 0;
  ssize_t numWrite;

  if (buf == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (perlimit <= 0)
    perlimit = sz;

  while (sz > 0) {
    numWrite = write(fd, buf, std::min(sz, perlimit));

    if (numWrite == -1) {
      if (errno == EINTR)
        continue;
      else
        return -1;

    } else if (numWrite == 0) {
      break;

    } else {
      numWritten += numWrite;
      buf += numWrite;
      sz -= numWrite;
    }
  }

  return numWritten;
}

ssize_t write_str(int fd, const char *buffer, size_t perlimit) {
  if (buffer == nullptr) {
    errno = EINVAL;
    return -1;
  }
  size_t sz = strlen(buffer);
  return bi_writen(fd, buffer, sz, perlimit);
}

char *copy_str(const char *str) {
  if (str == nullptr)
    return nullptr;
  size_t len = strlen(str);
  char *str_cp = new char[len + 1];
  strcpy(str_cp, str);
  return str_cp;
}

} // namespace Util