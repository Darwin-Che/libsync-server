#ifndef PARSE_H
#define PARSE_H

#include <cstdlib>
#include <string>
#include <vector>

class Line;

class Word {
  friend class Line;
  char *word;
  size_t len;

public:
  Word(char *_word, size_t _len);
  const char *get_word() const;
  size_t get_len() const;
  std::string get_str() const;
  int icmp(const char *other) const;
  int cmp(const char *other) const;
};

class Line {
  char *line;
  size_t sz;
  std::vector<Word> words;
  int cur_itr_pos;

public:
  class OutOfRange : public std::exception {
  private:
    const char *msg;

  public:
    OutOfRange(const char *_msg = nullptr);
    const char *what() const noexcept override;
  };

  Line(char *_line, size_t _sz);
  Line(size_t _sz);
  ~Line();
  int parse_words();
  char *get_line();
  size_t get_sz();
  size_t get_words_num();
  const std::vector<Word> &get_words();
  const Word &get_word(int pos);
  const Word &get_word();
  void reset_itr();
};

enum class Op {
  NOOP,
  HGET,
  HSET,
  CHNL_SET,
  CHNL_CREATE,
  CHNL_DEL,
  QUIT,
};

class OpParse {
public:
  static Op parse(const Word &word);
};

#endif