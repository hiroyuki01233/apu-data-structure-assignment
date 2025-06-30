#include <iostream>
#include <string>
#include <limits>
#include <filesystem>
#include "transaction_features.h"
#include "transaction_manager.h"
#include "csv_json_processing.h"
#include <iomanip>
#include <sstream>

void showMenu() {
    std::cout << "\n## Financial Transaction Analysis System ##\n";
    std::cout << "=========================================\n";
    std::cout << "1. Load & Store Transactions by Payment Channel\n";
    std::cout << "2. Sort Transactions by Location\n";
    std::cout << "3. Search Transactions by Type\n";
    std::cout << "4. Generate JSON from Active Data\n";
    std::cout << "5. Display All Stored Transactions\n";
    std::cout << "6. Perform Full CSV to JSON Conversion (Bulk)\n";
    std::cout << "7. Switch Active Data Structure\n";
    std::cout << "0. Exit\n";
    std::cout << "-----------------------------------------\n";
    std::cout << "Enter your choice: ";
}

int main(int argc, char* argv[]) {
    try {
        if (!std::filesystem::exists("results")) {
            std::filesystem::create_directory("results");
            std::cout << "Created 'results' directory for saving search outputs.\n";
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
    }

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_csv_file_path>\n";
        return 1;
    }
    std::string initialCsvFile = argv[1];
    TransactionManager transactionManager;

    if (!transactionManager.loadTransactionsFromCsv(initialCsvFile)) {
        std::cerr << "Failed to load initial CSV data. Exiting.\n";
        return 1;
    }

    int choice;
    do {
        showMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cout << "\n!!! Invalid input. Please enter a number. !!!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\n";

        switch (choice) {
            case 1:
                transactionManager.storeByPaymentChannel();
                break;
            case 2:
                transactionManager.sortTransactions();
                break;
            case 3:
                transactionManager.searchTransactions();
                break;
            case 4: {
                std::string outputJsonFile;
                std::cout << "Enter output JSON file path: ";
                std::getline(std::cin, outputJsonFile);
                transactionManager.generateJson(outputJsonFile);
                break;
            }
            case 5:
                transactionManager.displayAll();
                break;
            case 6: {
                std::string outputBulkJsonFile;
                std::cout << "Enter output JSON file path for bulk conversion: ";
                std::getline(std::cin, outputBulkJsonFile);
                // ★ 修正: bulk conversionの呼び出し方を修正
                std::thread parser(parserWorker, initialCsvFile);
                std::thread writer(writerWorker, outputBulkJsonFile);
                parser.join();
                writer.join();
                std::cout << "Bulk conversion completed.\n";
                break;
            }
            case 7:
                transactionManager.switchDataStructure();
                break;
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
            std::cin.get();
        }

    } while (choice != 0);

    return 0;
}