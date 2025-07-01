// Complete TRUE Structure Search - ALL options use active structure
#include "transaction_manager.h"
#include "csv_json_processing.h"
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <sstream>
#include <iomanip>

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

// UNIFIED TRUE STRUCTURE SEARCH - Works for ALL search types
void performUnifiedTrueSearch(TransactionManager* manager, const SearchCriteria& criteria, const std::string& searchTitle) {
    std::cout << "\n" << Color::CYAN << "=== " << searchTitle << " ===" << Color::RESET << "\n";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool found = false;
    int matchCount = 0;
    const int MAX_DISPLAY = 15;
    
    std::cout << "Searching through " << Color::CYAN << manager->getTransactionCount() << Color::RESET << " transactions...\n";
    std::cout << Color::BRIGHT_YELLOW << "🔥 ACTIVE STRUCTURE: " << manager->getCurrentDataStructureName() << Color::RESET << "\n";
    std::cout << Color::CYAN << "---------------------------------------------------" << Color::RESET << "\n";

    // TRUE DUAL STRUCTURE: Use active structure for BOTH search AND storage
    if (manager->isUsingArray()) {
        std::cout << Color::GREEN << "📊 Using Array for BOTH search AND result storage" << Color::RESET << "\n";
        
        TransactionArray searchResults;  // Array storage for Array search
        
        for (int i = 0; i < manager->getTransactionCount(); ++i) {
            try {
                Transaction* txPtr = manager->getTransactionPtrAt(i);
                if (txPtr == nullptr) continue;
                
                bool matches = true;
                
                // Apply all search criteria
                if (matches && !criteria.transactionType.empty()) {
                    if (toLowercase(txPtr->transaction_type) != toLowercase(criteria.transactionType)) {
                        matches = false;
                    }
                }
                if (matches && !criteria.location.empty()) {
                    if (toLowercase(txPtr->location).find(toLowercase(criteria.location)) == std::string::npos) {
                        matches = false;
                    }
                }
                if (matches && !criteria.paymentChannel.empty()) {
                    if (toLowercase(txPtr->payment_channel) != toLowercase(criteria.paymentChannel)) {
                        matches = false;
                    }
                }
                if (matches && criteria.hasAmountRange) {
                    if (txPtr->amount < criteria.minAmount || txPtr->amount > criteria.maxAmount) {
                        matches = false;
                    }
                }
                if (matches && criteria.isFraudOnly) {
                    if (!txPtr->is_fraud) {
                        matches = false;
                    }
                }
                
                if (matches) {
                    found = true;
                    matchCount++;
                    
                    // Store result in Array structure
                    searchResults.addTransaction(*txPtr);

                    if (searchResults.getSize() <= MAX_DISPLAY) {
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
        
        // Save Array results
        if (found) {
            try {
                std::string filename = generateUniqueFilename() + "_array.json";
                std::string filepath = "results/" + filename;
                std::cout << Color::CYAN << "[ARRAY SAVE]" << Color::RESET 
                        << " Saving " << searchResults.getSize() << " results to " << Color::YELLOW << filepath << Color::RESET << "..." << std::endl;
                saveTransactionsToJson(searchResults, filepath);
                std::cout << Color::BRIGHT_GREEN << "[SUCCESS]" << Color::RESET 
                          << " Array results saved!" << Color::RESET << "\n";
            } catch (const std::exception& e) {
                std::cout << Color::RED << "[ERROR]" << Color::RESET 
                          << " Failed to save results: " << e.what() << "\n";
            }
        }
        
    } else {
        std::cout << Color::BLUE << "📊 Using LinkedList for BOTH search AND result storage" << Color::RESET << "\n";
        
        TransactionLinkedList searchResults;  // LinkedList storage for LinkedList search
        
        for (int i = 0; i < manager->getTransactionCount(); ++i) {
            try {
                Transaction* txPtr = manager->getTransactionPtrAt(i);
                if (txPtr == nullptr) continue;
                
                bool matches = true;
                
                // Apply all search criteria (same logic as Array)
                if (matches && !criteria.transactionType.empty()) {
                    if (toLowercase(txPtr->transaction_type) != toLowercase(criteria.transactionType)) {
                        matches = false;
                    }
                }
                if (matches && !criteria.location.empty()) {
                    if (toLowercase(txPtr->location).find(toLowercase(criteria.location)) == std::string::npos) {
                        matches = false;
                    }
                }
                if (matches && !criteria.paymentChannel.empty()) {
                    if (toLowercase(txPtr->payment_channel) != toLowercase(criteria.paymentChannel)) {
                        matches = false;
                    }
                }
                if (matches && criteria.hasAmountRange) {
                    if (txPtr->amount < criteria.minAmount || txPtr->amount > criteria.maxAmount) {
                        matches = false;
                    }
                }
                if (matches && criteria.isFraudOnly) {
                    if (!txPtr->is_fraud) {
                        matches = false;
                    }
                }
                
                if (matches) {
                    found = true;
                    matchCount++;
                    
                    // Store result in LinkedList structure
                    searchResults.addTransaction(*txPtr);

                    if (searchResults.getSize() <= MAX_DISPLAY) {
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
        
        // Convert LinkedList results to Array for saving
        if (found) {
            try {
                TransactionArray arrayForSaving;
                for (int i = 0; i < searchResults.getSize(); ++i) {
                    Transaction* tx = searchResults.getTransaction(i);
                    if (tx) {
                        arrayForSaving.addTransaction(*tx);
                    }
                }
                
                std::string filename = generateUniqueFilename() + "_linkedlist.json";
                std::string filepath = "results/" + filename;
                std::cout << Color::CYAN << "[LINKEDLIST SAVE]" << Color::RESET 
                        << " Saving " << arrayForSaving.getSize() << " results to " << Color::YELLOW << filepath << Color::RESET << "..." << std::endl;
                saveTransactionsToJson(arrayForSaving, filepath);
                std::cout << Color::BRIGHT_GREEN << "[SUCCESS]" << Color::RESET 
                          << " LinkedList results saved!" << Color::RESET << "\n";
            } catch (const std::exception& e) {
                std::cout << Color::RED << "[ERROR]" << Color::RESET 
                          << " Failed to save results: " << e.what() << "\n";
            }
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << Color::CYAN << "---------------------------------------------------" << Color::RESET << "\n";
    if (found) {
        std::cout << Color::GREEN << Color::BOLD << "[SUCCESS]" << Color::RESET 
                  << " TRUE Structure Search Completed!\n";
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
        
        std::cout << Color::BLUE << "[MEMORY]" << Color::RESET 
                  << " Used " << (manager->isUsingArray() ? Color::GREEN + "Array" : Color::BLUE + "LinkedList") 
                  << Color::RESET << " for both search AND result storage\n";
        
        std::cout << Color::BRIGHT_YELLOW << "[TRUE COMPARISON]" << Color::RESET 
                  << " This reflects genuine " << manager->getCurrentDataStructureName() 
                  << " performance!" << Color::RESET << "\n";
        
    } else {
        std::cout << Color::RED << Color::BOLD << "[ERROR]" << Color::RESET 
                  << " No transactions found matching criteria\n";
        std::cout << Color::YELLOW << "[TIP]" << Color::RESET 
                  << " Try relaxing some search criteria\n";
        std::cout << Color::MAGENTA << "[TIME]" << Color::RESET 
                  << " Search time: " << Color::YELLOW << duration.count() 
                  << Color::RESET << " milliseconds\n";
    }
    std::cout << Color::CYAN << "===================================" << Color::RESET << "\n";
}

// Performance comparison between structures for any search type
void performFullStructureComparison(TransactionManager* manager, const SearchCriteria& criteria, const std::string& searchTitle) {
    std::cout << "\n" << Color::BRIGHT_YELLOW << "⚡ FULL STRUCTURE COMPARISON ⚡" << Color::RESET << "\n";
    std::cout << Color::CYAN << "Testing: " << searchTitle << " on both structures" << Color::RESET << "\n\n";
    
    // Store original structure
    bool originalStructure = manager->isUsingArray();
    
    // Test Array Structure
    std::cout << Color::GREEN << "🔵 TESTING ARRAY STRUCTURE:" << Color::RESET << "\n";
    manager->setActiveDataStructure(true);
    auto arrayStart = std::chrono::high_resolution_clock::now();
    performUnifiedTrueSearch(manager, criteria, searchTitle + " (ARRAY)");
    auto arrayEnd = std::chrono::high_resolution_clock::now();
    auto arrayDuration = std::chrono::duration_cast<std::chrono::milliseconds>(arrayEnd - arrayStart);
    
    std::cout << "\n" << Color::BLUE << "🔵 TESTING LINKEDLIST STRUCTURE:" << Color::RESET << "\n";
    manager->setActiveDataStructure(false);
    auto listStart = std::chrono::high_resolution_clock::now();
    performUnifiedTrueSearch(manager, criteria, searchTitle + " (LINKEDLIST)");
    auto listEnd = std::chrono::high_resolution_clock::now();
    auto listDuration = std::chrono::duration_cast<std::chrono::milliseconds>(listEnd - listStart);
    
    // Restore original structure
    manager->setActiveDataStructure(originalStructure);
    
    // Display comparison results
    std::cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << "📊 " << searchTitle << " COMPARISON:" << Color::RESET << "\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << std::left << std::setw(25) << Color::GREEN + "Array Structure:" + Color::RESET
              << std::right << std::setw(15) << arrayDuration.count() << " ms" << "\n";
    std::cout << std::left << std::setw(25) << Color::BLUE + "LinkedList Structure:" + Color::RESET
              << std::right << std::setw(15) << listDuration.count() << " ms" << "\n";
    
    std::cout << std::string(60, '-') << "\n";
    
    if (arrayDuration.count() > 0 && listDuration.count() > 0) {
        if (arrayDuration < listDuration) {
            double speedup = (double)listDuration.count() / arrayDuration.count();
            std::cout << Color::BRIGHT_GREEN << "🏆 Array is " << std::fixed << std::setprecision(2) 
                      << speedup << "x faster for " << searchTitle << Color::RESET << "\n";
        } else {
            double speedup = (double)arrayDuration.count() / listDuration.count();
            std::cout << Color::BRIGHT_BLUE << "🏆 LinkedList is " << std::fixed << std::setprecision(2) 
                      << speedup << "x faster for " << searchTitle << Color::RESET << "\n";
        }
    }
    
    std::cout << std::string(60, '=') << "\n";
}

void TransactionManager::searchTransactions() {
    std::cout << Color::BRIGHT_MAGENTA << ">>> Feature 3: TRUE Dual-Structure Search System" << Color::RESET << "\n";
    std::cout << Color::CYAN << "    ALL search options now use genuine structure comparison!" << Color::RESET << "\n\n";
    
    while (true) {
        std::cout << Color::BOLD << Color::CYAN << "==========================================\n";
        std::cout << "         TRUE STRUCTURE SEARCH MENU       \n";
        std::cout << "==========================================" << Color::RESET << "\n";
        std::cout << Color::GREEN << " 1. Simple Transaction Type Search" << Color::RESET << " (TRUE structure)\n";
        std::cout << Color::BLUE << " 2. Advanced Multi-Criteria Search" << Color::RESET << " (TRUE structure)\n";
        std::cout << Color::YELLOW << " 3. Fraud Transaction Search" << Color::RESET << " (TRUE structure)\n";
        std::cout << Color::MAGENTA << " 4. Amount Range Search" << Color::RESET << " (TRUE structure)\n";
        std::cout << Color::CYAN << " 5. Location-based Search" << Color::RESET << " (TRUE structure)\n";
        std::cout << Color::WHITE << " 6. Performance Test (Current Structure)" << Color::RESET << "\n"; 
        std::cout << Color::BRIGHT_MAGENTA << " 7. Switch Active Data Structure" << Color::RESET << "\n";
        std::cout << Color::BRIGHT_YELLOW << " 8. Compare Structures (Any Search)" << Color::RESET << "\n";
        std::cout << Color::RED << " 0. Return to Main Menu" << Color::RESET << "\n";
        std::cout << Color::BOLD << Color::CYAN << "==========================================" << Color::RESET << "\n";
        
        std::cout << Color::YELLOW << "Current Active Structure: " << this->getCurrentDataStructureName() << Color::RESET << "\n";
        
        int choice;
        std::cout << "Enter your choice (0-8): ";
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
                std::cout << "\nAvailable types: " << Color::YELLOW << "withdrawal, deposit, transfer, payment" << Color::RESET << "\n";
                std::cout << "Enter transaction type: ";
                std::getline(std::cin, input);
                criteria.transactionType = input;
                performUnifiedTrueSearch(this, criteria, "SIMPLE TYPE SEARCH");
                break;
            }
            
            case 2: {
                std::cout << "\nTransaction type (or press Enter to skip): ";
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

                performUnifiedTrueSearch(this, criteria, "ADVANCED MULTI-CRITERIA SEARCH");
                break;
            }
            
            case 3: {
                criteria.isFraudOnly = true;
                performUnifiedTrueSearch(this, criteria, "FRAUD TRANSACTION SEARCH");
                break;
            }
            
            case 4: {
                criteria.hasAmountRange = true;
                std::cout << "Enter minimum amount: $";
                std::cin >> criteria.minAmount;
                std::cout << "Enter maximum amount: $";
                std::cin >> criteria.maxAmount;
                performUnifiedTrueSearch(this, criteria, "AMOUNT RANGE SEARCH");
                break;
            }
            
            case 5: {
                std::cout << "Enter location (partial match): ";
                std::getline(std::cin, criteria.location);
                performUnifiedTrueSearch(this, criteria, "LOCATION-BASED SEARCH");
                break;
            }
            
            case 6: {
                std::cout << "Enter transaction type for performance test: ";
                std::getline(std::cin, input);
                criteria.transactionType = input;
                
                auto start = std::chrono::high_resolution_clock::now();
                performUnifiedTrueSearch(this, criteria, "PERFORMANCE TEST");
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                
                std::cout << "\n" << Color::BRIGHT_YELLOW << "📊 DETAILED PERFORMANCE ANALYSIS:" << Color::RESET << "\n";
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
            
            case 8: {
                std::cout << "\n" << Color::BRIGHT_YELLOW << "=== CHOOSE SEARCH TYPE FOR COMPARISON ===" << Color::RESET << "\n";
                std::cout << "1. Simple Type Search\n";
                std::cout << "2. Fraud Search\n";
                std::cout << "3. Amount Range Search\n";
                std::cout << "4. Location Search\n";
                std::cout << "Enter choice (1-4): ";
                int compChoice;
                std::cin >> compChoice;
                std::cin.ignore();
                
                switch(compChoice) {
                    case 1:
                        std::cout << "Enter transaction type: ";
                        std::getline(std::cin, input);
                        criteria.transactionType = input;
                        performFullStructureComparison(this, criteria, "SIMPLE TYPE SEARCH");
                        break;
                    case 2:
                        criteria.isFraudOnly = true;
                        performFullStructureComparison(this, criteria, "FRAUD SEARCH");
                        break;
                    case 3:
                        criteria.hasAmountRange = true;
                        std::cout << "Minimum amount: $";
                        std::cin >> criteria.minAmount;
                        std::cout << "Maximum amount: $";
                        std::cin >> criteria.maxAmount;
                        performFullStructureComparison(this, criteria, "AMOUNT RANGE SEARCH");
                        break;
                    case 4:
                        std::cout << "Enter location: ";
                        std::getline(std::cin, criteria.location);
                        performFullStructureComparison(this, criteria, "LOCATION SEARCH");
                        break;
                    default:
                        std::cout << Color::RED << "Invalid choice!" << Color::RESET << "\n";
                }
                break;
            }
            
            default:
                std::cout << Color::RED << "Invalid choice! Please try again." << Color::RESET << "\n";
        }
        
        std::cout << "\n" << Color::CYAN << "Press Enter to continue..." << Color::RESET;
        std::cin.get();
    }
}