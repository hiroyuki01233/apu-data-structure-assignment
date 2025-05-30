// File: main.cpp
#include <iostream>
#include <string>
#include <limits>
#include "transaction_features.h" // 既存の機能関数宣言
#include "transaction_manager.h"  // 新しいデータ管理クラス
#include "csv_json_processing.h"  // bulk conversion のために必要 (parserWorker/writerWorker)

// Function to display the menu
void showMenu() {
    std::cout << "\n## Financial Transaction Analysis System ##\n";
    std::cout << "=========================================\n";
    std::cout << "1. Load & Store Transactions by Payment Channel\n";
    std::cout << "2. Sort Transactions by Location\n";
    std::cout << "3. Search Transactions by Type\n";
    std::cout << "4. Generate JSON from Specific Data Collection\n";
    std::cout << "5. Display All Stored Transactions\n";
    std::cout << "6. Perform Full CSV to JSON Conversion (Bulk)\n";
    std::cout << "0. Exit\n";
    std::cout << "-----------------------------------------\n";
    std::cout << "Enter your choice: ";
}

int main(int argc, char* argv[]) { // 引数を受け取るように変更
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_csv_file_path>\n";
        return 1;
    }
    std::string initialCsvFile = argv[1]; // 起動時にCSVファイルパスを受け取る

    // TransactionManager のインスタンスを作成
    // このクラスがすべてのトランザクションデータを管理します
    TransactionManager transactionManager;

    // 起動時にデータを読み込む (またはメニューの最初の選択肢で読み込ませる)
    // ここでは起動時に必須とします
    if (!transactionManager.loadTransactionsFromCsv(initialCsvFile)) {
        std::cerr << "Failed to load initial CSV data. Exiting.\n";
        return 1;
    }

    int choice;

    do {
        showMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cout << "\n!!! Invalid input. Please enter a number corresponding to a menu option. !!!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        std::cout << "\n";

        switch (choice) {
            case 1:
                transactionManager.storeByPaymentChannel(); // データの処理はmanagerに任せる
                break;
            case 2:
                transactionManager.sortTransactions();
                break;
            case 3:
                transactionManager.searchTransactions();
                break;
            case 4: {
                std::string outputJsonFile;
                std::cout << "Enter output JSON file path for specific collection: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::getline(std::cin, outputJsonFile);
                transactionManager.generateJson(outputJsonFile); // managerに任せる
                break;
            }
            case 5:
                transactionManager.displayAll();
                break;
            case 6: {
                // Bulk conversion は、TransactionManagerの内部データとは独立して処理
                std::string outputBulkJsonFile;
                std::cout << "Enter output JSON file path for bulk conversion: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::getline(std::cin, outputBulkJsonFile);
                // transaction_features.cpp で定義された performFullCsvToJsonConversion を呼び出す
                performFullCsvToJsonConversion(initialCsvFile, outputBulkJsonFile);
                break;
            }
            case 0:
                std::cout << "Exiting program. Goodbye!\n";
                break;
            default:
                if (choice != -1) {
                    std::cout << "!!! Invalid choice. Please try again. !!!\n";
                }
        }
        if (choice != 0) {
            std::cout << "\n(Press Enter to continue to the menu...)";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }

    } while (choice != 0);

    return 0;
}