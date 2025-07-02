#ifndef CUSTOM_DATA_STRUCTURES_H
#define CUSTOM_DATA_STRUCTURES_H

#include "transaction.h"

class TransactionArray {
private:
    Transaction* data;
    int size;
    int capacity;
    void expandCapacity();

public:
    TransactionArray(int initialCapacity = 10);
    ~TransactionArray();
    TransactionArray(const TransactionArray& other);
    TransactionArray& operator=(const TransactionArray& other);
    TransactionArray(TransactionArray&& other) noexcept;
    TransactionArray& operator=(TransactionArray&& other) noexcept;

    void addTransaction(const Transaction& tx);
    int getSize() const;

    Transaction* getTransaction(int index);
    const Transaction* getTransaction(int index) const;

    Transaction* getDataPointer();
    const Transaction* getDataPointer() const;
};

struct TransactionNode {
    Transaction data;
    TransactionNode* next;

    TransactionNode(const Transaction& t) : data(t), next(nullptr) {}
    TransactionNode(Transaction&& t) noexcept : data(std::move(t)), next(nullptr) {}
};

class TransactionLinkedList {
private:
    TransactionNode* head;
    TransactionNode* tail;
    int size;

public:
    TransactionLinkedList();
    ~TransactionLinkedList();
    TransactionLinkedList(const TransactionLinkedList& other);
    TransactionLinkedList& operator=(const TransactionLinkedList& other);
    TransactionLinkedList(TransactionLinkedList&& other) noexcept;
    TransactionLinkedList& operator=(TransactionLinkedList&& other) noexcept;

    void addTransaction(const Transaction& tx);
    int getSize() const;

    TransactionNode* getHead() const { return head; };
    void setHead(TransactionNode* newHead) { head = newHead; };

    Transaction* getTransaction(int index);
    const Transaction* getTransaction(int index) const;
};

#endif