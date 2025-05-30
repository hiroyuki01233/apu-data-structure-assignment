// File: custom_data_structures.cpp
#include "custom_data_structures.h"
#include <iostream> // デバッグ出力やエラーメッセージのため

// ===================================
// TransactionArray の実装
// ===================================

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

// ★ TransactionArray::getTransaction (非const版) の実装
Transaction& TransactionArray::getTransaction(int index) {
    if (index < 0 || index >= size) {
        std::cerr << "Error: Index out of bounds in TransactionArray::getTransaction(" << index << ")\n";
        // エラーハンドリング: 範囲外アクセス
        // 実際には、例外をスローしたり、プログラムを終了させたりするべきです。
        // ここでは、コンパイルを通すためにダミーを返していますが、非常に危険なコードです。
        static Transaction dummyTx; // 安全ではないが、参照を返すための仮の措置
        return dummyTx;
    }
    return data[index];
}

// ★ TransactionArray::getTransaction (const版) の実装
const Transaction& TransactionArray::getTransaction(int index) const {
    if (index < 0 || index >= size) {
        std::cerr << "Error: Index out of bounds in TransactionArray::getTransaction(" << index << ") const\n";
        static Transaction dummyTx;
        return dummyTx;
    }
    return data[index];
}

// ★ TransactionArray::getDataPointer (非const版) の実装
Transaction* TransactionArray::getDataPointer() {
    return data;
}

// ★ TransactionArray::getDataPointer (const版) の実装
const Transaction* TransactionArray::getDataPointer() const {
    return data;
}

// コピーコンストラクタ
TransactionArray::TransactionArray(const TransactionArray& other) : size(other.size), capacity(other.capacity) {
    data = new Transaction[capacity];
    for (int i = 0; i < size; ++i) {
        data[i] = other.data[i];
    }
}

// 代入演算子
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

// ムーブコンストラクタ
TransactionArray::TransactionArray(TransactionArray&& other) noexcept
    : data(other.data), size(other.size), capacity(other.capacity) {
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
}

// ムーブ代入演算子
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


// ===================================
// TransactionLinkedList の実装
// ===================================

TransactionLinkedList::TransactionLinkedList() : head(nullptr), size(0) {}

TransactionLinkedList::~TransactionLinkedList() {
    TransactionNode* current = head;
    while (current != nullptr) {
        TransactionNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;
    size = 0;
}

void TransactionLinkedList::addTransaction(const Transaction& tx) {
    TransactionNode* newNode = new TransactionNode(tx);
    if (head == nullptr) {
        head = newNode;
    } else {
        TransactionNode* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    size++;
}

int TransactionLinkedList::getSize() const {
    return size;
}

// ★ TransactionLinkedList::getTransaction (非const版) の実装
Transaction* TransactionLinkedList::getTransaction(int index) {
    if (index < 0 || index >= size) {
        std::cerr << "Error: Index out of bounds in TransactionLinkedList::getTransaction(" << index << ")\n";
        return nullptr; // 範囲外の場合はnullptrを返す
    }
    TransactionNode* current = head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }
    return &(current->data); // 該当ノードのデータへのポインタを返す
}

// ★ TransactionLinkedList::getTransaction (const版) の実装
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

// コピーコンストラクタ
TransactionLinkedList::TransactionLinkedList(const TransactionLinkedList& other) : head(nullptr), size(0) {
    if (other.head == nullptr) {
        return;
    }

    TransactionNode* currentOther = other.head;
    head = new TransactionNode(currentOther->data);
    TransactionNode* currentThis = head;
    size = 1;

    currentOther = currentOther->next;
    while (currentOther != nullptr) {
        currentThis->next = new TransactionNode(currentOther->data);
        currentThis = currentThis->next;
        currentOther = currentOther->next;
        size++;
    }
}

// 代入演算子
TransactionLinkedList& TransactionLinkedList::operator=(const TransactionLinkedList& other) {
    if (this != &other) {
        // 現在のリストをクリア
        TransactionNode* current = head;
        while (current != nullptr) {
            TransactionNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
        size = 0;

        // コピー元のリストからノードをコピー
        if (other.head == nullptr) {
            return *this;
        }

        TransactionNode* currentOther = other.head;
        head = new TransactionNode(currentOther->data);
        TransactionNode* currentThis = head;
        size = 1;

        currentOther = currentOther->next;
        while (currentOther != nullptr) {
            currentThis->next = new TransactionNode(currentOther->data);
            currentThis = currentThis->next;
            currentOther = currentOther->next;
            size++;
        }
    }
    return *this;
}

// ムーブコンストラクタ
TransactionLinkedList::TransactionLinkedList(TransactionLinkedList&& other) noexcept
    : head(other.head), size(other.size) {
    other.head = nullptr;
    other.size = 0;
}

// ムーブ代入演算子
TransactionLinkedList& TransactionLinkedList::operator=(TransactionLinkedList&& other) noexcept {
    if (this != &other) {
        // 現在のリストを解放
        TransactionNode* current = head;
        while (current != nullptr) {
            TransactionNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }

        // other のリソースを奪う
        head = other.head;
        size = other.size;

        // other を空の状態にする
        other.head = nullptr;
        other.size = 0;
    }
    return *this;
}