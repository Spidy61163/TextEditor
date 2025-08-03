#pragma once
#include <iostream>

// Node structure defined outside the class
template <typename T>
struct Node {
    T data;
    Node* next;
    Node* prev;
    bool highlight;

    Node(const T& val) : data(val), next(nullptr), prev(nullptr), highlight (false) { }
};

// LinkList class
template <typename T>
class LinkList {
private:
    Node<T>* head;  // Use Node<T> to refer to the Node structure
    Node<T>* tail;  // Use Node<T> to refer to the Node structure
    size_t size;

public:
    class Iterator {
    private:
        Node<T>* current;  // Use Node<T> to refer to the Node structure

        friend class LinkList;
        friend class TextEditor;
        friend class SearchEngine;
    public:
        Iterator(Node<T>* node = nullptr) : current(node) {}

        T& operator*();
        T* operator->();

        // Pre-increment
        Iterator& operator++();

        // Post-increment
        Iterator operator++(int);

        // Pre-decrement
        Iterator& operator--();

        // Post-decrement
        Iterator operator--(int);

        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
    };

    friend TextEditor;

    LinkList();
    ~LinkList();

    void clear();
    size_t getSize() const;
    bool isEmpty() const;

    Iterator begin();
    Iterator end();

    void push_front(const T& value);
    void push_back(const T& value);
    void pop_front();
    void pop_back();
    void insert(Iterator& pos, const T& value);
    void erase(Iterator& pos);
    T get_pos(Iterator pos);
    void print() const;
};

// Iterator member functions
template <typename T>
T& LinkList<T>::Iterator::operator*() {
    if (this != nullptr)
    {
        return current->data;
    }
}

template <typename T>
T* LinkList<T>::Iterator::operator->() {
    return &(current->data);
}

template <typename T>
typename LinkList<T>::Iterator& LinkList<T>::Iterator::operator++() {
    if (current) current = current->next;
    return *this;
}

template <typename T>
typename LinkList<T>::Iterator LinkList<T>::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

template <typename T>
typename LinkList<T>::Iterator& LinkList<T>::Iterator::operator--() {
    if (current) current = current->prev;
    return *this;
}

template <typename T>
typename LinkList<T>::Iterator LinkList<T>::Iterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

template <typename T>
bool LinkList<T>::Iterator::operator==(const Iterator& other) const {
    return current == other.current;
}

template <typename T>
bool LinkList<T>::Iterator::operator!=(const Iterator& other) const {
    return current != other.current;
}

// LinkList member functions
template <typename T>
LinkList<T>::LinkList() : head(nullptr), tail(nullptr), size(0) {
}

template <typename T>
T LinkList<T>::get_pos(Iterator pos)
{
    int position = 0;
    Node<char>* temp = head;
    while (temp and temp != pos.current and temp->next)
    {
        position++;
        temp = temp->next;
    }
    return position;
}

template <typename T>
LinkList<T>::~LinkList() {
    clear();
}

template <typename T>
void LinkList<T>::clear() {
    while (head) {
        Node<T>* temp = head;  // Use Node<T>
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    size = 0;
}

template <typename T>
size_t LinkList<T>::getSize() const {
    return size;
}

template <typename T>
bool LinkList<T>::isEmpty() const {
    return size == 0;
}

template <typename T>
typename LinkList<T>::Iterator LinkList<T>::begin() {
    return Iterator(head);
}

template <typename T>
typename LinkList<T>::Iterator LinkList<T>::end() {
    return Iterator(nullptr);
}

template <typename T>
void LinkList<T>::push_front(const T& value) {
    Node<T>* newNode = new Node<T>(value);  // Use Node<T>
    if (head) {
        newNode->next = head;
        head->prev = newNode;
    }
    else {
        tail = newNode;
    }
    head = newNode;
    ++size;
}

template <typename T>
void LinkList<T>::push_back(const T& value) {
    Node<T>* newNode = new Node<T>(value);  // Use Node<T>
    if (tail) {
        tail->next = newNode;
        newNode->prev = tail;
    }
    else {
        head = newNode;
    }
    tail = newNode;
    ++size;
}

template <typename T>
void LinkList<T>::pop_front() {
    if (!head) return;
    Node<T>* temp = head;  // Use Node<T>
    head = head->next;
    if (head) {
        head->prev = nullptr;
    }
    else {
        tail = nullptr;
    }
    delete temp;
    --size;
}

template <typename T>
void LinkList<T>::pop_back() {
    if (!tail) return;
    Node<T>* temp = tail;  // Use Node<T>
    tail = tail->prev;
    if (tail) {
        tail->next = nullptr;
    }
    else {
        head = nullptr;
    }
    delete temp;
    --size;
}

template <typename T>
void LinkList<T>::insert(Iterator& pos, const T& value) {
    if (pos == begin()) {
        push_front(value);
        pos++;
        return;
    }
    if (pos == end()) {
        push_back(value);
        return;
    }

    Node<T>* current = pos.current;  // Use Node<T>
    Node<T>* newNode = new Node<T>(value);  // Use Node<T>

    newNode->prev = current->prev;
    newNode->next = current;
    current->prev->next = newNode;
    current->prev = newNode;
    //pos++;
    ++size;
}

template <typename T>
void LinkList<T>::erase(Iterator& pos) {
    if (pos == end() || pos.current == nullptr)
    {
        pos--;
        pop_back();
        return;
    }

    Node<T>* current = pos.current;  // Use Node<T>
    pos.current = pos.current->prev;

    // If the current node is the head
    if (current == head) {
        pop_front();
        return;
    }

    // If the current node is the tail
    if (current == tail) {
        pop_back();
        return;
    }

    // If the current node is in the middle
    current->prev->next = current->next;
    if (current->next)
    {  // Check if current->next is not null
        current->next->prev = current->prev;
    }

    delete current;  // Delete the current node
    --size;  // Decrease the size of the linked list
}


template <typename T>
void LinkList<T>::print() const {
    Node<T>* current = head;  // Use Node<T>
    while (current) {
        std::cout << current->data << " ";
        current = current->next;
    }
    std::cout << std::endl;
}