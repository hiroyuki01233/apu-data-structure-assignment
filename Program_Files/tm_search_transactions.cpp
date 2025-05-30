#include "transaction_manager.h" // TransactionManager クラス定義のため
#include <iostream>
#include <string> // ユーザーからの入力やTransactionのフィールド比較のため
#include <limits>

// searchTransactions()関数の実装
void TransactionManager::searchTransactions() {
    std::cout << ">>> Feature 3: Search by Transaction Type\n";
    std::cout << "    This feature allows searching for transactions of a specific type.\n";

    std::string searchType;
    std::cout << "Enter the transaction type to search for (e.g., 'Debit', 'Credit', 'Transfer'): ";
    // std::cin.ignore() と std::getline() を使用して、スペースを含む文字列も正確に読み込む
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 前の入力で残った改行をクリア
    std::getline(std::cin, searchType);

    std::cout << "\nSearching for transactions of type: '" << searchType << "'\n";
    std::cout << "---------------------------------------------------\n";

    bool found = false;

    // TransactionManagerが保持するカスタムデータ構造（transactionsCollection）にアクセス
    // 例としてTransactionArrayを使用していますが、TransactionLinkedListの場合も同様にイテレートします。
    for (int i = 0; i < transactionsCollection.getSize(); ++i) {
        // getSize() と getTransaction() は custom_data_structures.h/.cpp で実装済みと仮定
        Transaction& tx = transactionsCollection.getTransaction(i);

        // トランザクションタイプが検索条件と一致するかチェック
        if (tx.transaction_type == searchType) {
            found = true;
            // マッチしたトランザクションの情報を表示
            std::cout << "  ID: " << tx.transaction_id
                      << ", Type: " << tx.transaction_type
                      << ", Amount: " << tx.amount
                      << ", Location: " << tx.location
                      << ", Channel: " << tx.payment_channel
                      << std::endl;
        }
    }

    if (!found) {
        std::cout << "No transactions found for type: '" << searchType << "'\n";
    }
    std::cout << "---------------------------------------------------\n";
}