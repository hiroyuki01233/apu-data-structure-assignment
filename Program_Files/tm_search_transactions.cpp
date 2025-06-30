#include "transaction_manager.h"
#include "csv_json_processing.h"
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>
#include <chrono>


namespace Color {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_BLUE = "\033[44m";
}

std::string toLowercase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

struct SearchCriteria {
    std::string transactionType;
    std::string location;
    std::string paymentChannel;
    double minAmount;
    double maxAmount;
    bool hasAmountRange;
    bool isFraudOnly;
    
    SearchCriteria() : minAmount(0), maxAmount(0), hasAmountRange(false), isFraudOnly(false) {}
};

std::string generateUniqueFilename() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << "search_results_";
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

void performSimpleSearch(TransactionManager* manager, const std::string& searchType) {
    std::cout << "\n" << Color::CYAN << "=== SIMPLE SEARCH RESULTS ===" << Color::RESET << "\n";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool found = false;
    int matchCount = 0;
    const int MAX_DISPLAY = 15;

    TransactionArray searchResults;
    
    std::cout << "Search Criteria: " << Color::YELLOW << Color::BOLD << searchType << Color::RESET << "\n";
    std::cout << "Searching through " << Color::CYAN << manager->getTransactionCount() << Color::RESET << " transactions...\n";
    
    std::cout << "Using: " << Color::BLUE << manager->getCurrentDataStructureName() << Color::RESET << "\n";
    std::cout << Color::CYAN << "---------------------------------------------------" << Color::RESET << "\n";
    
    std::string searchTypeLower = toLowercase(searchType);

    for (int i = 0; i < manager->getTransactionCount(); ++i) {
        try {
            Transaction* txPtr = manager->getTransactionPtrAt(i);
            if (txPtr == nullptr) continue;
            
            std::string txTypeLower = toLowercase(txPtr->transaction_type);
            
            if (txTypeLower == searchTypeLower) {
                found = true;
                matchCount++;
                
                searchResults.addTransaction(*txPtr);

                if (searchResults.getSize() <= 15) {
                    std::cout << Color::GREEN << "  [" << matchCount << "]" << Color::RESET 
                              << " ID: " << Color::YELLOW << txPtr->transaction_id << Color::RESET
                              << " | Type: " << Color::BRIGHT_CYAN << txPtr->transaction_type << Color::RESET
                              << " | Amount: " << Color::BRIGHT_GREEN << "$" << txPtr->amount << Color::RESET
                              << " | Location: " << Color::MAGENTA << txPtr->location << Color::RESET
                              << " | Channel: " << Color::BLUE << txPtr->payment_channel << Color::RESET
                              << " | Fraud: " << (txPtr->is_fraud ? Color::RED + "YES" : Color::GREEN + "NO") << Color::RESET
                              << std::endl;
                }
                
                if (matchCount > 0 && matchCount % 5000 == 0) {
                    std::cout << Color::YELLOW << "  ... found " << matchCount << " matches so far ..." << Color::RESET << "\n";
                }
            }
        } catch (const std::exception& e) {
            std::cerr << Color::RED << "Error accessing transaction: " << e.what() << Color::RESET << std::endl;
            break;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << Color::CYAN << "---------------------------------------------------" << Color::RESET << "\n";
    if (found) {
        std::cout << Color::GREEN << Color::BOLD << "[SUCCESS]" << Color::RESET 
                  << " Search Completed Successfully!\n";
        std::cout << Color::BLUE << "[STATS]" << Color::RESET 
                  << " Total matches found: " << Color::YELLOW << Color::BOLD << matchCount 
                  << Color::RESET << " transactions\n";
        if (matchCount > MAX_DISPLAY) {
            std::cout << Color::MAGENTA << "[INFO]" << Color::RESET 
                      << " Displayed: First " << Color::YELLOW << MAX_DISPLAY 
                      << Color::RESET << " results\n";
        }

        double percentage = (manager->getTransactionCount() > 0) ? (double)matchCount / manager->getTransactionCount() * 100.0 : 0.0;
        std::cout << Color::CYAN << "[PERCENT]" << Color::RESET 
                  << " Percentage of total: " << Color::YELLOW << Color::BOLD 
                  << percentage << "%" << Color::RESET << "\n";
        std::cout << Color::MAGENTA << "[TIME]" << Color::RESET 
                  << " Search time: " << Color::YELLOW << duration.count() 
                  << Color::RESET << " milliseconds\n";
        
        std::cout << Color::BLUE << "[TECH]" << Color::RESET 
                  << " Data Structure: " << Color::GREEN << manager->getCurrentDataStructureName() + " (O(n) linear search)"
                  << Color::RESET << "\n";

        std::string filename = generateUniqueFilename() + ".json";
        std::string filepath = "results/" + filename;
        std::cout << Color::CYAN << "[INFO]" << Color::RESET 
                << " Saving " << searchResults.getSize() << " results to " << Color::YELLOW << filepath << Color::RESET << "..." << std::endl;
        saveTransactionsToJson(searchResults, filepath);
        std::cout << Color::BRIGHT_GREEN << "[SAVE]" << Color::RESET 
                  << " Results saved to " << Color::YELLOW << filepath << Color::RESET << "\n";
    } else {
        std::cout << Color::RED << Color::BOLD << "[ERROR]" << Color::RESET 
                  << " No transactions found for type: '" << Color::YELLOW << searchType << Color::RESET << "'\n";
        std::cout << Color::YELLOW << "[TIP]" << Color::RESET 
                  << " Available types: " << Color::CYAN << "withdrawal, deposit, transfer, payment" << Color::RESET << "\n";
        std::cout << Color::MAGENTA << "[TIME]" << Color::RESET 
                  << " Search time: " << Color::YELLOW << duration.count() 
                  << Color::RESET << " milliseconds\n";
    }
    std::cout << Color::CYAN << "===================================" << Color::RESET << "\n";
}

void performAdvancedSearch(TransactionManager* manager, const SearchCriteria& criteria) {
    std::cout << "\n" << Color::CYAN << "=== ADVANCED SEARCH RESULTS ===" << Color::RESET << "\n";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool found = false;
    int matchCount = 0;
    const int MAX_DISPLAY = 15;

    TransactionArray searchResults;
    
    std::cout << "Using: " << Color::BLUE << manager->getCurrentDataStructureName() << Color::RESET << "\n";
    std::cout << "\nSearching through " << Color::CYAN << manager->getTransactionCount() << Color::RESET << " transactions...\n";
    std::cout << Color::CYAN << "---------------------------------------------------" << Color::RESET << "\n";

    for (int i = 0; i < manager->getTransactionCount(); ++i) {
        try {
            Transaction* txPtr = manager->getTransactionPtrAt(i);
            if (txPtr == nullptr) continue;
            
            bool matches = true;
            
            if (matches && !criteria.transactionType.empty()) {
                if (toLowercase(txPtr->transaction_type) != toLowercase(criteria.transactionType)) matches = false;
            }
            if (matches && !criteria.location.empty()) {
                if (toLowercase(txPtr->location).find(toLowercase(criteria.location)) == std::string::npos) matches = false;
            }
            if (matches && !criteria.paymentChannel.empty()) {
                if (toLowercase(txPtr->payment_channel) != toLowercase(criteria.paymentChannel)) matches = false;
            }
            if (matches && criteria.hasAmountRange) {
                if (txPtr->amount < criteria.minAmount || txPtr->amount > criteria.maxAmount) matches = false;
            }
            if (matches && criteria.isFraudOnly) {
                if (!txPtr->is_fraud) matches = false;
            }
            
            if (matches) {
                found = true;
                matchCount++;
                
                searchResults.addTransaction(*txPtr);

                if (searchResults.getSize() <= 15) {
                    std::cout << Color::GREEN << "  [" << matchCount << "]" << Color::RESET 
                              << " ID: " << Color::YELLOW << txPtr->transaction_id << Color::RESET
                              << " | Type: " << Color::BRIGHT_CYAN << txPtr->transaction_type << Color::RESET
                              << " | Amount: " << Color::BRIGHT_GREEN << "$" << txPtr->amount << Color::RESET
                              << " | Location: " << Color::MAGENTA << txPtr->location << Color::RESET
                              << " | Channel: " << Color::BLUE << txPtr->payment_channel << Color::RESET
                              << " | Fraud: " << (txPtr->is_fraud ? Color::RED + "YES" : Color::GREEN + "NO") << Color::RESET
                              << std::endl;
                }
                
                if (matchCount > 0 && matchCount % 5000 == 0) {
                    std::cout << Color::YELLOW << "  ... found " << matchCount << " matches so far ..." << Color::RESET << "\n";
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << Color::RED << "Error accessing transaction: " << e.what() << Color::RESET << std::endl;
            break;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << Color::CYAN << "---------------------------------------------------" << Color::RESET << "\n";
    if (found) {
        double percentage = (manager->getTransactionCount() > 0) ? (double)matchCount / manager->getTransactionCount() * 100.0 : 0.0;

        std::cout << Color::BLUE << "[TECH]" << Color::RESET 
                  << " Data Structure: " << Color::GREEN << manager->getCurrentDataStructureName() + " (O(n) linear search)"
                  << Color::RESET << "\n";

        std::string filename = generateUniqueFilename() + ".json";
        std::string filepath = "results/" + filename;
        std::cout << Color::CYAN << "[INFO]" << Color::RESET 
            << " Saving " << searchResults.getSize() << " results to " << Color::YELLOW << filepath << Color::RESET << "..." << std::endl;
        saveTransactionsToJson(searchResults, filepath);
        std::cout << Color::BRIGHT_GREEN << "[SAVE]" << Color::RESET 
                  << " Results saved to " << Color::YELLOW << filepath << Color::RESET << "\n";
    } else {
        std::cout << Color::RED << Color::BOLD << "[ERROR]" << Color::RESET 
                  << " No transactions found matching all criteria\n";
        std::cout << Color::YELLOW << "[TIP]" << Color::RESET 
                  << " Try relaxing some search criteria\n";
        std::cout << Color::MAGENTA << "[TIME]" << Color::RESET 
                  << " Search time: " << Color::YELLOW << duration.count() 
                  << Color::RESET << " milliseconds\n";
    }
    std::cout << Color::CYAN << "===================================" << Color::RESET << "\n";
}

void TransactionManager::searchTransactions() {
    std::cout << Color::BRIGHT_MAGENTA << ">>> Feature 3: Advanced Multi-Criteria Search System" << Color::RESET << "\n";
    std::cout << Color::CYAN << "    Search transactions using multiple criteria for precise filtering." << Color::RESET << "\n\n";
    
    while (true) {
        std::cout << Color::BOLD << Color::CYAN << "==========================================\n";
        std::cout << "         SEARCH OPTIONS MENU              \n";
        std::cout << "==========================================" << Color::RESET << "\n";
        std::cout << Color::GREEN << " 1. Simple Transaction Type Search" << Color::RESET << "\n";
        std::cout << Color::BLUE << " 2. Advanced Multi-Criteria Search" << Color::RESET << "\n";
        std::cout << Color::YELLOW << " 3. Fraud Transaction Search" << Color::RESET << "\n";
        std::cout << Color::MAGENTA << " 4. Amount Range Search" << Color::RESET << "\n";
        std::cout << Color::CYAN << " 5. Location-based Search" << Color::RESET << "\n";

        std::cout << Color::WHITE << " 6. Performance Test on Current Structure" << Color::RESET << "\n"; 
        std::cout << Color::BRIGHT_MAGENTA << " 7. Switch Active Data Structure" << Color::RESET << "\n";
        std::cout << Color::RED << " 0. Return to Main Menu" << Color::RESET << "\n";
        std::cout << Color::BOLD << Color::CYAN << "==========================================" << Color::RESET << "\n";
        
        std::cout << Color::YELLOW << "Current Active Structure: " << this->getCurrentDataStructureName() << Color::RESET << "\n";
        
        int choice;
        std::cout << "Enter your choice (0-7): ";
        std::cin >> choice;

        if(std::cin.fail()) {
            std::cout << Color::RED << "Invalid input. Please enter a number." << Color::RESET << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        if (choice == 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        
        SearchCriteria criteria;
        std::string input;
        
        if (choice != 4 && choice != 7) {
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1: {
                std::cout << "\n" << Color::BRIGHT_GREEN << "=== SIMPLE TRANSACTION TYPE SEARCH ===" << Color::RESET << "\n";
                std::cout << "Available types: " << Color::YELLOW << "withdrawal, deposit, transfer, payment" << Color::RESET << "\n";
                std::cout << "Enter transaction type: ";
                std::getline(std::cin, input);
                performSimpleSearch(this, input);
                break;
            }
            
            case 2: {
                std::cin.ignore();
                std::cout << "\n" << Color::BRIGHT_BLUE << "=== ADVANCED MULTI-CRITERIA SEARCH ===" << Color::RESET << "\n";
                
                std::cout << "Transaction type (or press Enter to skip): ";
                std::getline(std::cin, criteria.transactionType);
                
                std::cout << "Location contains (or press Enter to skip): ";
                std::getline(std::cin, criteria.location);
                
                std::cout << "Payment channel (or press Enter to skip): ";
                std::getline(std::cin, criteria.paymentChannel);
                
                std::cout << "Include amount range? (y/n): ";
                std::getline(std::cin, input);
                if (toLowercase(trim(input)) == "y" || toLowercase(trim(input)) == "yes") {
                    criteria.hasAmountRange = true;
                    std::cout << "Minimum amount: $";
                    std::cin >> criteria.minAmount;
                    std::cout << "Maximum amount: $";
                    std::cin >> criteria.maxAmount;
                }

                performAdvancedSearch(this, criteria);
                break;
            }
            
            case 3: {
                criteria.isFraudOnly = true;
                std::cout << "\n" << Color::RED << "=== FRAUD TRANSACTION SEARCH ===" << Color::RESET << "\n";
                performAdvancedSearch(this, criteria);
                break;
            }
            
            case 4: {
                criteria.hasAmountRange = true;
                std::cout << "\n" << Color::GREEN << "=== AMOUNT RANGE SEARCH ===" << Color::RESET << "\n";
                std::cout << "Enter minimum amount: $";
                std::cin >> criteria.minAmount;
                std::cout << "Enter maximum amount: $";
                std::cin >> criteria.maxAmount;
                performAdvancedSearch(this, criteria);
                break;
            }
            
            case 5: {
                std::cout << "\n" << Color::MAGENTA << "=== LOCATION-BASED SEARCH ===" << Color::RESET << "\n";
                std::cout << "Enter location (partial match): ";
                std::getline(std::cin, criteria.location);
                performAdvancedSearch(this, criteria);
                break;
            }
            
            case 6: {
                std::cout << "\n" << Color::BRIGHT_CYAN << "=== PERFORMANCE TEST ===" << Color::RESET << "\n";
                criteria.transactionType = "deposit";
                
                auto start = std::chrono::high_resolution_clock::now();
                performAdvancedSearch(this, criteria);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                
                std::cout << "\n" << Color::BRIGHT_YELLOW << "📊 PERFORMANCE ANALYSIS:" << Color::RESET << "\n";
                std::cout << "   Active Structure: " << this->getCurrentDataStructureName() << "\n";
                std::cout << "   Time Complexity: " << Color::YELLOW << "O(n) linear search" << Color::RESET << "\n";
                std::cout << "   Execution Time: " << Color::BRIGHT_GREEN << duration.count() << Color::RESET << " microseconds\n";
                std::cout << "   Records Processed: " << Color::BRIGHT_BLUE << getTransactionCount() << Color::RESET << "\n";
                break;
            }
            
            case 7: {
                this->switchDataStructure();
                break;
            }
            
            default:
                std::cout << Color::RED << "Invalid choice! Please try again." << Color::RESET << "\n";
        }
        
        std::cout << "\n" << Color::CYAN << "Press Enter to continue..." << Color::RESET;
        std::cin.get();
    }
}