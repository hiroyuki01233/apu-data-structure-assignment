#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "transaction.h"
#include "custom_data_structures.h"

namespace Color {
    extern const std::string RESET;
    extern const std::string RED;
    extern const std::string GREEN;
    extern const std::string YELLOW;
    extern const std::string BLUE;
    extern const std::string MAGENTA;
    extern const std::string CYAN;
    extern const std::string WHITE;
    extern const std::string BOLD;
    extern const std::string BRIGHT_RED;
    extern const std::string BRIGHT_GREEN;
    extern const std::string BRIGHT_YELLOW;
    extern const std::string BRIGHT_BLUE;
    extern const std::string BRIGHT_MAGENTA;
    extern const std::string BRIGHT_CYAN;
    extern const std::string BG_RED;
    extern const std::string BG_GREEN;
    extern const std::string BG_BLUE;
}


class TransactionManager {
public:
    TransactionArray transactionsArray;
    TransactionLinkedList transactionsLinkedList;
    
    bool useArrayDataStructure;

public:
    TransactionManager();

    ~TransactionManager() = default;

    bool loadTransactionsFromCsv(const std::string& filePath);

    void storeByPaymentChannel();
    void sortTransactions();
    void searchTransactions();
    void generateJson(const std::string& outputFilePath);
    void displayAll();

    int getTransactionCount() const;
    Transaction* getTransactionPtrAt(int index);
    std::string getCurrentDataStructureName() const;
    void switchDataStructure();

    void displayCurrentStructureInfo() const;

    void setActiveDataStructure(bool useArray) { 
        useArrayDataStructure = useArray; 
        std::cout << Color::CYAN << "🔄 Active data structure switched to: " 
                  << getCurrentDataStructureName() << Color::RESET << "\n";
    }
    
    bool isUsingArray() const { 
        return useArrayDataStructure; 
    }
    
    std::string getActiveDataStructureName() const {
        if (useArrayDataStructure) {
            return Color::GREEN + "Array-Based (TransactionArray)" + Color::RESET;
        } else {
            return Color::BLUE + "LinkedList-Based (TransactionLinkedList)" + Color::RESET;
        }
    }
    
    void displayDataStructureInfo() const {
        std::cout << "\n" << Color::BOLD << Color::CYAN << "🔍 DATA STRUCTURE INFO" << Color::RESET << "\n";
        std::cout << std::string(40, '=') << "\n";
        
        std::cout << Color::YELLOW << "Current Active Structure: " << Color::RESET 
                  << getActiveDataStructureName() << "\n";
        
        std::cout << Color::YELLOW << "Total Transactions: " << Color::RESET 
                  << getTransactionCount() << "\n";
        
        if (useArrayDataStructure) {
            std::cout << "\n" << Color::GREEN << "📈 Array Benefits:" << Color::RESET << "\n";
            std::cout << "  • O(1) random access time\n";
            std::cout << "  • Cache-friendly memory access pattern\n";
        } else {
            std::cout << "\n" << Color::GREEN << "📈 LinkedList Benefits:" << Color::RESET << "\n";
            std::cout << "  • Dynamic memory allocation\n";
            std::cout << "  • Efficient additions/deletions (at ends)\n";
        }
        std::cout << std::string(40, '=') << "\n";
    }

    void performFullStructureSortComparison(); 
};

#endif