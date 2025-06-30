#include "custom_data_structures.h"
#include <iostream>

TransactionArray::TransactionArray(int initialCapacity) : size(0) {
    if (initialCapacity <= 0) {
        initialCapacity = 10;
    }
    capacity = initialCapacity;
    data = new Transaction[capacity];
}

TransactionArray::~TransactionArray() {
    delete[] data;
    data = nullptr;
    size = 0;
    capacity = 0;
}

void TransactionArray::expandCapacity() {
    int newCapacity = capacity * 2;
    Transaction* newData = new Transaction[newCapacity];
    for (int i = 0; i < size; ++i) {
        newData[i] = std::move(data[i]);
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
    std::cout << "Debug: TransactionArray capacity expanded to " << capacity << std::endl;
}

void TransactionArray::addTransaction(const Transaction& tx) {
    if (size == capacity) {
        expandCapacity();
    }
    data[size++] = tx;
}

int TransactionArray::getSize() const {
    return size;
}

Transaction* TransactionArray::getTransaction(int index) {
    if (index < 0 || index >= size) {
        std::cerr << "Error: Index out of bounds in TransactionArray::getTransaction(" << index << ")\n";
        return nullptr;
    }
    return &data[index];
}

const Transaction* TransactionArray::getTransaction(int index) const {
    if (index < 0 || index >= size) {
        std::cerr << "Error: Index out of bounds in TransactionArray::getTransaction(" << index << ") const\n";
        return nullptr;
    }
    return &data[index];
}
Transaction* TransactionArray::getDataPointer() {
    return data;
}

const Transaction* TransactionArray::getDataPointer() const {
    return data;
}

TransactionArray::TransactionArray(const TransactionArray& other) : size(other.size), capacity(other.capacity) {
    data = new Transaction[capacity];
    for (int i = 0; i < size; ++i) {
        data[i] = other.data[i];
    }
}

TransactionArray& TransactionArray::operator=(const TransactionArray& other) {
    if (this != &other) {
        delete[] data;

        size = other.size;
        capacity = other.capacity;
        data = new Transaction[capacity];
        for (int i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

TransactionArray::TransactionArray(TransactionArray&& other) noexcept
    : data(other.data), size(other.size), capacity(other.capacity) {
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
}

TransactionArray& TransactionArray::operator=(TransactionArray&& other) noexcept {
    if (this != &other) {
        delete[] data;

        data = other.data;
        size = other.size;
        capacity = other.capacity;

        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }
    return *this;
}


TransactionLinkedList::TransactionLinkedList() : head(nullptr), tail(nullptr), size(0) {}

TransactionLinkedList::~TransactionLinkedList() {
    TransactionNode* current = head;
    while (current != nullptr) {
        TransactionNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

void TransactionLinkedList::addTransaction(const Transaction& tx) {
    TransactionNode* newNode = new TransactionNode(tx);
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    size++;
}

int TransactionLinkedList::getSize() const {
    return size;
}

Transaction* TransactionLinkedList::getTransaction(int index) {
    if (index < 0 || index >= size) {
        std::cerr << "Error: Index out of bounds in TransactionLinkedList::getTransaction(" << index << ")\n";
        return nullptr;
    }
    TransactionNode* current = head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }
    return &(current->data);
}

const Transaction* TransactionLinkedList::getTransaction(int index) const {
    if (index < 0 || index >= size) {
        std::cerr << "Error: Index out of bounds in TransactionLinkedList::getTransaction(" << index << ") const\n";
        return nullptr;
    }
    TransactionNode* current = head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }
    return &(current->data);
}

TransactionLinkedList::TransactionLinkedList(const TransactionLinkedList& other) : head(nullptr), tail(nullptr), size(0) {
    if (other.head == nullptr) {
        return;
    }

    TransactionNode* currentOther = other.head;
    head = new TransactionNode(currentOther->data);
    tail = head;
    size = 1;

    currentOther = currentOther->next;
    while (currentOther != nullptr) {
        tail->next = new TransactionNode(currentOther->data);
        tail = tail->next;
        currentOther = currentOther->next;
        size++;
    }
}

TransactionLinkedList& TransactionLinkedList::operator=(const TransactionLinkedList& other) {
    if (this != &other) {
        TransactionNode* current = head;
        while (current != nullptr) {
            TransactionNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
        tail = nullptr;
        size = 0;

        if (other.head != nullptr) {
            TransactionNode* currentOther = other.head;
            head = new TransactionNode(currentOther->data);
            tail = head;
            size = 1;

            currentOther = currentOther->next;
            while (currentOther != nullptr) {
                tail->next = new TransactionNode(currentOther->data);
                tail = tail->next;
                currentOther = currentOther->next;
                size++;
            }
        }
    }
    return *this;
}

TransactionLinkedList::TransactionLinkedList(TransactionLinkedList&& other) noexcept
    : head(other.head), tail(other.tail), size(other.size) {
    other.head = nullptr;
    other.tail = nullptr;
    other.size = 0;
}

TransactionLinkedList& TransactionLinkedList::operator=(TransactionLinkedList&& other) noexcept {
    if (this != &other) {
        TransactionNode* current = head;
        while (current != nullptr) {
            TransactionNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }

        head = other.head;
        tail = other.tail;
        size = other.size;

        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }
    return *this;
}