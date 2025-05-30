// File: transaction_manager.h
#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <string>
#include "transaction.h"
#include "custom_data_structures.h" // あなたが作成するカスタムデータ構造のヘッダー

// 仮のカスタムデータ構造の選択（プロジェクトの要件に合わせて選択）
#define USE_ARRAY_DATA_STRUCTURE
// #define USE_LINKED_LIST_DATA_STRUCTURE

class TransactionManager {
private:
    // PDF要件に従い、std::vectorではなく、
    // あなたが実装するカスタムのアレイベースまたはリンクトリストベースのデータ構造を使用します。
#ifdef USE_ARRAY_DATA_STRUCTURE
    TransactionArray transactionsCollection; // TransactionArrayのインスタンス
#elif USE_LINKED_LIST_DATA_STRUCTURE
    TransactionLinkedList transactionsCollection; // TransactionLinkedListのインスタンス
#else
    // std::vector<Transaction> transactions; // 仮のプレースホルダ。これは課題では使えません。
#endif

public:
    TransactionManager(); // コンストラクタ

    // CSVからデータを読み込み、内部のカスタムデータ構造に格納する
    bool loadTransactionsFromCsv(const std::string& filePath);

    // 以降、PDFの要件に対応する機能の宣言 (これらを個別の.cppファイルに分割)
    void storeByPaymentChannel();
    void sortTransactions();
    void searchTransactions();
    void generateJson(const std::string& outputFilePath);
    void displayAll();
};

#endif // TRANSACTION_MANAGER_H