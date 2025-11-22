#include <iostream>

template <class T> class Node {
  T data;
  Node *next;
  Node *prev;
  Node(T _d, Node _n = nullptr, Node _p = nullptr) {
    data = _d;
    next = _n;
    prev = _p;
  }
};

template <class T> class CQueue {
public:
  Node<T> *root;
  Node<T> *tail;

public:
  CQueue();
  void push(T data);
};

template <class T> CQueue<T>::CQueue() {
  root = nullptr;
  tail = nullptr;
};

template <class T> void CQueue<T>::push(T data) {
  Node<T> node = new Node(data);

  if (root == nullptr) {
    root = node;
    tail = node;
    return;
  }

  Node<T> temp;
  temp = root;
};
