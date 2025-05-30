// File: custom_data_structures.h
#ifndef CUSTOM_DATA_STRUCTURES_H
#define CUSTOM_DATA_STRUCTURES_H

#include "transaction.h" // Transaction 構造体を使用するため

// ここにアレイベースのデータ構造の宣言
class TransactionArray {
private:
    Transaction* data; // トランザクションを格納する動的配列
    int size;          // 現在格納されているトランザクションの数
    int capacity;      // 配列の最大容量

    void expandCapacity(); // 容量を増やすためのヘルパー関数

public:
    TransactionArray(int initialCapacity = 10);
    ~TransactionArray();

    // コピーコンストラクタと代入演算子 (深いコピーのために必須)
    TransactionArray(const TransactionArray& other);
    TransactionArray& operator=(const TransactionArray& other);

    // ムーブコンストラクタとムーブ代入演算子 (効率的なムーブのため)
    TransactionArray(TransactionArray&& other) noexcept;
    TransactionArray& operator=(TransactionArray&& other) noexcept;

    void addTransaction(const Transaction& tx);
    int getSize() const;

    // ★ getTransaction 関数の宣言 (非const版とconst版)
    Transaction& getTransaction(int index);
    const Transaction& getTransaction(int index) const;

    // データポインタを直接取得する関数 (ソートなどに便利)
    Transaction* getDataPointer(); // 非const版
    const Transaction* getDataPointer() const; // const版
};


// ここにリンクトリストベースのデータ構造の宣言
struct TransactionNode {
    Transaction data;
    TransactionNode* next;

    TransactionNode(const Transaction& t) : data(t), next(nullptr) {}
    TransactionNode(Transaction&& t) noexcept : data(std::move(t)), next(nullptr) {} // ムーブコンストラクタ
};

class TransactionLinkedList {
private:
    TransactionNode* head; // リストの先頭ノードへのポインタ
    int size;              // リスト内のトランザクションの数

public:
    TransactionLinkedList();
    ~TransactionLinkedList();

    // コピーコンストラクタと代入演算子
    TransactionLinkedList(const TransactionLinkedList& other);
    TransactionLinkedList& operator=(const TransactionLinkedList& other);

    // ムーブコンストラクタとムーブ代入演算子
    TransactionLinkedList(TransactionLinkedList&& other) noexcept;
    TransactionLinkedList& operator=(TransactionLinkedList&& other) noexcept;

    void addTransaction(const Transaction& tx);
    int getSize() const;

    // ★ getTransaction 関数の宣言 (非const版とconst版)
    // リンクトリストの場合、ポインタを返す方が一般的で安全です。
    // 値渡しや参照渡しだと、ノードそのものへのアクセスがしにくくなるため。
    Transaction* getTransaction(int index);
    const Transaction* getTransaction(int index) const;
};

#endif // CUSTOM_DATA_STRUCTURES_H