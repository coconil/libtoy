#pragma once

namespace ctoy {

struct ListNode {
  ListNode *next; /**< point to next node. */
  ListNode *prev; /**< point to prev node. */
  ListNode():next(this),prev(this){}
  void init() {
    next = this;
    prev = this;
  }

  void insert_after(ListNode *n) {
    next->prev = n;
    n->next = next;

    next = n;
    n->prev = this;
  }

  void insert_before(ListNode *n) {
    prev->next = n;
    n->prev = prev;

    prev = n;
    n->next = this;
  }

  void remove() {
    this->next->prev = this->prev;
    this->prev->next = this->next;

    this->next = this->prev = this;
  }

  int isempty() const {
    return next == this;
  }

  unsigned int length() const {
    unsigned int len = 0;
    const ListNode *p = this;
    while (p->next != this) {
      p = p->next;
      len++;
    }

    return len;
  }
};

} // namespace ctoy