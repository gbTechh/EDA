#include "lista.h"

using namespace std;

Node::~Node() {}

Node::Node(Data _data, Node *n) : data(_data), next(n) {}

CList::CList() : size(0), total(0), root(nullptr) {};
CList::~CList() {
  Node *current = root;
  while (current != nullptr) {
    Node *next = current->next;
    delete current;
    current = next;
  }
  root = nullptr;
  size = 0;
  total = 0;
}
void CList::Ins(Data &_data) {
  Node *newNode = new Node(_data);
  Node *searchPtr = newNode;
  if (root == nullptr) {
    root = searchPtr;
    size++;
    total++;
    return;
  }
  if (Find(searchPtr)) {
    searchPtr->data.frq++;
    _data = searchPtr->data;
    delete newNode;
  } else {
    newNode->next = root;
    root = newNode;
    size++;
  }
  total++;
}

bool CList::Find(Node *&n) {
  Node *current = root;

  while (current != nullptr) {
    if (current->data.topic == n->data.topic) {
      n = current;
      return true;
    }
    current = current->next;
  }
  return false;
}

bool CList::Search(Data &d) {
  Node *current = root;

  while (current != nullptr) {
    if (current->data.topic == d.topic) {
      d = current->data;
      return true;
    }
    current = current->next;
  }
  return false;
}

bool CList::RemFreq(Data &d) {
  Node *current = root;
  while (current != nullptr) {
    if (current->data.topic == d.topic) {
      current->data.frq--;
      d.frq = current->data.frq;
      d.topic = current->data.topic;
      d.error = current->data.error;
      return true;
    }
    current = current->next;
  }
  return false;
}

bool CList::Rem(Data &data) {

  if (root == nullptr) {
    return false;
  }

  Node *current = root;
  Node *previous = nullptr;
  int iteration = 0;

  while (current != nullptr) {

    if (current->data.topic == data.topic) {

      data.topic = current->data.topic;
      data.frq = current->data.frq;
      data.error = current->data.error;

      // Reorganizar punteros
      if (previous == nullptr) {

        root = current->next;

      } else {
        previous->next = current->next;
      }

      delete current;
      size--;
      return true;
    }

    previous = current;
    current = current->next;
    iteration++;
  }

  return false;
}

void CList::PrintList() const {
  Node *current = root;
  if (current == nullptr) {
    cout << "(vacio)";
    return;
  }

  while (current != nullptr) {
    cout << "[" << current->data.topic << " (frq:" << current->data.frq << ")]"
         << " (err:" << current->data.error << ")]";
    if (current->next != nullptr) {
      cout << " -> ";
    }
    current = current->next;
  }
}

void CList::limpiar_lista() {
  Node *current = root;
  while (current != nullptr) {
    Node *next = current->next;
    delete current;
    current = next;
  }
  root = nullptr;
  size = 0;
  total = 0;
}