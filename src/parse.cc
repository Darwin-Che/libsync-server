#include "parse.h"
#include "util.h"
#include <iostream>
#include <string>

using namespace std;

Word::Word(char *_word, size_t _len) : word(_word), len(_len) {}

const char *Word::get_word() const { return word; }

size_t Word::get_len() const { return len; }

string Word::get_str() const { return string(word, len); }

int Word::icmp(const char *other) const {
  return strncasecmp(word, other, len);
}

int Word::cmp(const char *other) const { return strncmp(word, other, len); }

int Line::parse_words() {
  words.clear();
  int ret = 0;
  int tmp_res = 0;
  ssize_t idx = 0, len = 0;
  while (tmp_res >= 0) {
    tmp_res = Util::get_word(line, sz, idx, len);
    if (tmp_res < 0)
      break;
    words.emplace_back(line + idx, len);
    ret += 1;
  }
  // for (auto &word : words) {
  //   word.word[word.len] = '\0';
  // }
  reset_itr();
  return ret;
}

Line::OutOfRange::OutOfRange(const char *_msg) : msg(_msg) {}

const char *Line::OutOfRange::what() const noexcept {
  if (msg == nullptr)
    return "\n**** Line::OutOfRange ****\n";
  return msg;
}

Line::Line(char *_line, size_t _sz)
    : line(_line), sz(_sz), words(), cur_itr_pos(0) {}

Line::Line(size_t _sz) : sz(_sz), words(), cur_itr_pos(0) {
  line = new char[sz];
}

Line::~Line() { delete line; }

char *Line::get_line() { return line; }

size_t Line::get_sz() { return sz; }

size_t Line::get_words_num() { return words.size(); }

const vector<Word> &Line::get_words() { return words; }

const Word &Line::get_word(int pos) {
  if (pos >= words.size())
    throw OutOfRange();
  return words[pos];
}

const Word &Line::get_word() { return get_word(cur_itr_pos++); }

void Line::reset_itr() { cur_itr_pos = 0; }

Op OpParse::parse(const Word &word) {
  if (word.icmp("chnlset") == 0) {
    return Op::CHNL_SET;
  } else if (word.icmp("chnlcreate") == 0) {
    return Op::CHNL_CREATE;
  } else if (word.icmp("chnldel") == 0) {
    return Op::CHNL_DEL;
  } else if (word.icmp("hset") == 0) {
    return Op::HSET;
  } else if (word.icmp("hget") == 0) {
    return Op::HGET;
  } else if (word.icmp("quit") == 0) {
    return Op::QUIT;
  }
  return Op::NOOP;
}