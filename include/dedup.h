#ifndef DEDUP_H
#define DEDUP_H

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>

template <class T> class DedupSet;

template <class T> class DedupSetContent;

template <class T> class DedupSetMgr {
  friend class DedupSetContent<T>;
  friend class DedupSet<T>;

private:
  std::map<std::string, DedupSetContent<T> *> tbl;
  void remove(DedupSetContent<T> *dds) {
    tbl.erase(dds->key);
    delete dds;
  }

public:
  static std::string to_string(std::set<T> s) {
    std::stringstream ss;
    for (auto t : s) {
      ss << t << ',';
    }
    return ss.str();
  }
  DedupSet<T> get(const std::set<T> &s) {
    std::string key = to_string(s);
    auto it = tbl.find(key);
    if (it == tbl.end()) {
      auto ddsc = new DedupSetContent<T>(*this, s, key);
      tbl[key] = ddsc;
      return ddsc->get();
    }
    return (*it).second->get();
  }
};

template <class T> class DedupSetContent {
  friend class DedupSetMgr<T>;
  friend class DedupSet<T>;

private:
  DedupSetMgr<T> &mgr;
  std::set<T> s;
  std::string key;
  unsigned refcnt;

private:
  DedupSetContent(DedupSetMgr<T> &_mgr, std::set<T> _s, const std::string &_key)
      : mgr(_mgr), s(_s), key(_key) {}
  DedupSet<T> get() {
    refcnt += 1;
    return DedupSet<T>(this);
  }
  // return: The refcnt left
  unsigned put() {
    refcnt -= 1;
    return refcnt;
  }
  ~DedupSetContent() {
    std::cout << "Destroy DedupSetContent " << this << std::endl;
  }
};

template <class T> class DedupSet {
  friend class DedupSetContent<T>;
  friend class DedupSetMgr<T>;

private:
  DedupSetContent<T> *content;

private:
  DedupSet(DedupSetContent<T> *_content) : content(_content) {
    std::cout << "Created DedupSet " << content << ", str=[" << content->key
              << "]" << std::endl;
  };

public:
  ~DedupSet() {
    auto refcnt = content->put();
    if (refcnt == 0) {
      content->mgr.remove(content);
    }
  }
  // DedupSet(DedupSet<T> && other) {
  //     swap(content, other.content);
  // }
  DedupSet<T> &operator=(DedupSet<T> &&other) {
    if (this != &other) {
      std::swap(content, other.content);
    }
    return *this;
  }
  DedupSet(const DedupSet<T> &) = delete;
  DedupSet<T> &operator=(const DedupSet<T> &) = delete;

  std::set<T> &get_set() { return content->s; }

  bool add(T t) {
    if (content->s.find(t) != content->s.end())
      return false;
    // optimization if this is the last reference
    if (content->refcnt == 1) {
      content->mgr.tbl.erase(content->key);
      content->s.insert(t);
      std::string newkey = DedupSetMgr<T>::to_string(content->s);
      auto it = content->mgr.tbl.find(newkey);
      if (it == content->mgr.tbl.end()) {
        // optimization to move the content over
        content->mgr.tbl[newkey] = content;
        content->key = newkey;
      } else {
        it->second->refcnt += 1;
        content = it->second;
      }
    } else {
      std::set<T> newset = content->s;
      newset.insert(t);
      *this = content->mgr.get(newset);
    }
    return true;
  }
};

#endif