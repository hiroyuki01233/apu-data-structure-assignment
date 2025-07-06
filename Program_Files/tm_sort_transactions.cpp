#include "transaction_manager.h"
#include "custom_data_structures.h"
#include <iostream>
#include <functional>
#include <limits>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "csv_json_processing.h"
#include <sys/resource.h>

static long getMemoryUsageKB() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

enum class SortField {
    LOCATION,
    AMOUNT,
    TIMESTAMP
};

std::string getFieldName(SortField field) {
    switch (field) {
        case SortField::LOCATION: return "Location";
        case SortField::AMOUNT: return "Amount";
        case SortField::TIMESTAMP: return "Timestamp";
        default: return "Unknown";
    }
}

void quickSortArray(Transaction* arr, int left, int right, std::function<bool(const Transaction&, const Transaction&)> comp) {
    if (left >= right) return;
    Transaction pivot = arr[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (comp(arr[i], pivot)) i++;
        while (comp(pivot, arr[j])) j--;
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++; j--;
        }
    }
    quickSortArray(arr, left, j, comp);
    quickSortArray(arr, i, right, comp);
}

void merge(Transaction* arr, int left, int mid, int right, std::function<bool(const Transaction&, const Transaction&)> comp) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    Transaction* L = new Transaction[n1];
    Transaction* R = new Transaction[n2];
    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) arr[k++] = comp(L[i], R[j]) ? L[i++] : R[j++];
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    delete[] L;
    delete[] R;
}

void mergeSortArray(Transaction* arr, int left, int right, std::function<bool(const Transaction&, const Transaction&)> comp) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortArray(arr, left, mid, comp);
    mergeSortArray(arr, mid + 1, right, comp);
    merge(arr, left, mid, right, comp);
}

TransactionNode* mergeSortedLists(TransactionNode* a, TransactionNode* b, 
    std::function<bool(const Transaction&, const Transaction&)> comp) {
    
    if (!a) return b;
    if (!b) return a;
    
    TransactionNode* result = nullptr;
    TransactionNode* tail = nullptr;
    
    while (a && b) {
        if (comp(a->data, b->data)) {
            if (!result) {
                result = tail = a;
            } else {
                tail->next = a;
                tail = a;
            }
            a = a->next;
        } else {
            if (!result) {
                result = tail = b;
            } else {
                tail->next = b;
                tail = b;
            }
            b = b->next;
        }
    }
    
    if (tail) {
        tail->next = a ? a : b;
    } else {
        result = a ? a : b;
    }
    
    return result;
}

void splitList(TransactionNode* source, TransactionNode** front, TransactionNode** back) {
    TransactionNode* slow = source;
    TransactionNode* fast = source->next;
    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *front = source;
    *back = slow->next;
    slow->next = nullptr;
}

TransactionNode* mergeSortLinkedList(TransactionNode* head, std::function<bool(const Transaction&, const Transaction&)> comp) {
    if (!head || !head->next) return head;
    TransactionNode* a;
    TransactionNode* b;
    splitList(head, &a, &b);
    a = mergeSortLinkedList(a, comp);
    b = mergeSortLinkedList(b, comp);
    return mergeSortedLists(a, b, comp);
}

TransactionNode* mergeSortLinkedListIterative(TransactionNode* head, 
    std::function<bool(const Transaction&, const Transaction&)> comp) {
    
    if (!head || !head->next) return head;
    
    int length = 0;
    TransactionNode* current = head;
    while (current) {
        length++;
        current = current->next;
    }
    
    for (int size = 1; size < length; size *= 2) {
        TransactionNode* newHead = nullptr;
        TransactionNode* tail = nullptr;
        TransactionNode* curr = head;
        
        while (curr) {
            TransactionNode* first = curr;
            int firstSize = 0;
            
            while (curr && firstSize < size - 1) {
                curr = curr->next;
                firstSize++;
            }
            
            if (!curr || !curr->next) {
                if (!newHead) {
                    newHead = first;
                } else {
                    tail->next = first;
                }
                break;
            }
            
            TransactionNode* secondStart = curr->next;
            curr->next = nullptr;
            curr = secondStart;
            
            TransactionNode* second = secondStart;
            int secondSize = 0;
            
            while (curr && secondSize < size - 1) {
                curr = curr->next;
                secondSize++;
            }
            
            TransactionNode* nextStart = nullptr;
            if (curr) {
                nextStart = curr->next;
                curr->next = nullptr;
            }
            
            TransactionNode* merged = mergeSortedLists(first, second, comp);
            
            if (!newHead) {
                newHead = merged;
            } else {
                tail->next = merged;
            }
            
            tail = merged;
            while (tail->next) {
                tail = tail->next;
            }
            
            curr = nextStart;
        }
        
        head = newHead;
    }
    
    return head;
}

void saveSortedResultsToJson(TransactionManager* manager, SortField field, const std::string& algoName) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "sorted_" << getFieldName(field) << "_" << algoName << "_";
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    ss << (manager->isUsingArray() ? "_array.json" : "_linkedlist.json");
    
    std::string filename = ss.str();
    std::string filepath = "results/" + filename;
    
    std::cout << Color::CYAN << "💾 Saving sorted results to " << Color::YELLOW << filepath << Color::RESET << "...\n";
    
    if (manager->isUsingArray()) {
        saveTransactionsToJson(manager->transactionsArray, filepath);
    } else {
        TransactionArray temp;
        for (int i = 0; i < manager->transactionsLinkedList.getSize(); ++i) {
            Transaction* tx = manager->transactionsLinkedList.getTransaction(i);
            if (tx) temp.addTransaction(*tx);
        }
        saveTransactionsToJson(temp, filepath);
    }
    std::cout << Color::BRIGHT_GREEN << "✅ Saved successfully!" << Color::RESET << "\n";
}

void performSort(TransactionManager* manager) {
    std::cout << "\n" << Color::CYAN << "Choose field to sort by:" << Color::RESET << "\n";
    std::cout << "1. Location\n";
    std::cout << "2. Amount\n";
    std::cout << "3. Timestamp\n";
    std::cout << Color::RED << "0. Go Back" << Color::RESET << "\n";
    std::cout << "Enter choice (0-3): ";
    
    int fieldChoice;
    std::cin >> fieldChoice;
    
    if (fieldChoice == 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    if (fieldChoice < 1 || fieldChoice > 3) {
        std::cout << Color::RED << "Invalid choice!" << Color::RESET << "\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    SortField field = SortField::LOCATION;
    if (fieldChoice == 2) field = SortField::AMOUNT;
    else if (fieldChoice == 3) field = SortField::TIMESTAMP;

    std::cout << "\n" << Color::CYAN << "Choose sorting algorithm:" << Color::RESET << "\n";
    std::cout << "1. Quick Sort\n";
    std::cout << "2. Merge Sort\n";
    std::cout << Color::RED << "0. Go Back" << Color::RESET << "\n";
    std::cout << "Enter choice (0-2): ";
    
    int algoChoice;
    std::cin >> algoChoice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (algoChoice == 0) return;
    
    if (algoChoice < 1 || algoChoice > 2) {
        std::cout << Color::RED << "Invalid choice!" << Color::RESET << "\n";
        return;
    }

    auto comparator = [&](const Transaction& a, const Transaction& b) {
        switch (field) {
            case SortField::LOCATION: return a.location < b.location;
            case SortField::AMOUNT: return a.amount < b.amount;
            case SortField::TIMESTAMP: return a.timestamp < b.timestamp;
            default: return false;
        }
    };

    std::cout << "\n" << Color::BRIGHT_YELLOW << "🔍 Sorting by " << getFieldName(field) 
              << " using " << (algoChoice == 1 ? "QuickSort" : "MergeSort") 
              << " on " << manager->getCurrentDataStructureName() << "..." << Color::RESET << "\n";
    
    long memoryBefore = getMemoryUsageKB();
    auto start = std::chrono::high_resolution_clock::now();

    if (manager->isUsingArray()) {
        Transaction* arr = manager->transactionsArray.getDataPointer();
        int size = manager->transactionsArray.getSize();
        if (algoChoice == 1)
            quickSortArray(arr, 0, size - 1, comparator);
        else
            mergeSortArray(arr, 0, size - 1, comparator);
    } else {
        TransactionNode* sorted = nullptr;
        
        int nodeCount = 0;
        TransactionNode* temp = manager->transactionsLinkedList.getHead();
        while (temp && nodeCount < 5000) {
            nodeCount++;
            temp = temp->next;
        }
        
        if (algoChoice == 1) {
            std::cout << Color::YELLOW << "Note: QuickSort is not efficient for LinkedList. Using MergeSort instead.\n" << Color::RESET;
        }
        
        if (nodeCount >= 5000) {
            std::cout << Color::YELLOW << "Large dataset detected. Using iterative approach...\n" << Color::RESET;
            sorted = mergeSortLinkedListIterative(
                manager->transactionsLinkedList.getHead(), comparator);
        } else {
            sorted = mergeSortLinkedList(
                manager->transactionsLinkedList.getHead(), comparator);
        }
        
        TransactionLinkedList newList;
        TransactionNode* current = sorted;
        while (current) {
            newList.addTransaction(current->data);
            current = current->next;
        }
        manager->transactionsLinkedList = newList;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    long memoryAfter = getMemoryUsageKB();
    long memoryUsed = memoryAfter - memoryBefore;
    
    std::cout << Color::GREEN << "⏱️  Time Taken: " << Color::YELLOW << duration.count() << " ms" << Color::RESET << "\n";
    std::cout << Color::BLUE << "💾 Memory Usage: " << Color::YELLOW << memoryUsed 
              << " KB (current total: " << memoryAfter << " KB)" << Color::RESET << "\n";
    std::cout << Color::BRIGHT_GREEN << "✅ Sort complete!" << Color::RESET << "\n\n";

    std::cout << Color::CYAN << "First 10 sorted results:" << Color::RESET << "\n";
    std::cout << std::string(80, '-') << "\n";
    
    int displayCount = std::min(10, manager->getTransactionCount());
    for (int i = 0; i < displayCount; ++i) {
        Transaction* tx = manager->getTransactionPtrAt(i);
        if (tx) {
            std::cout << Color::GREEN << "[" << i+1 << "] " << Color::RESET;
            switch (field) {
                case SortField::LOCATION:
                    std::cout << "Location: " << Color::YELLOW << tx->location << Color::RESET
                              << " | ID: " << tx->transaction_id << "\n";
                    break;
                case SortField::AMOUNT:
                    std::cout << "Amount: " << Color::YELLOW << "$" << tx->amount << Color::RESET
                              << " | ID: " << tx->transaction_id << "\n";
                    break;
                case SortField::TIMESTAMP:
                    std::cout << "Timestamp: " << Color::YELLOW << tx->timestamp << Color::RESET
                              << " | ID: " << tx->transaction_id << "\n";
                    break;
            }
        }
    }
    std::cout << std::string(80, '-') << "\n";

    std::cout << "\n" << Color::BRIGHT_YELLOW << "Would you like to save the sorted results? (y/n): " << Color::RESET;
    char saveChoice;
    std::cin >> saveChoice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (saveChoice == 'y' || saveChoice == 'Y') {
        saveSortedResultsToJson(manager, field, algoChoice == 1 ? "quick" : "merge");
    }
}

void TransactionManager::performFullStructureSortComparison() {
    std::cout << "\n" << Color::BRIGHT_MAGENTA << ">>> ⚡ Sorting Structure Comparison" << Color::RESET << "\n";
    
    std::cout << "\n" << Color::CYAN << "Choose field to sort by:" << Color::RESET << "\n";
    std::cout << "1. Location\n";
    std::cout << "2. Amount\n";
    std::cout << "3. Timestamp\n";
    std::cout << Color::RED << "0. Go Back" << Color::RESET << "\n";
    std::cout << "Enter choice (0-3): ";
    
    int fieldChoice;
    std::cin >> fieldChoice;
    
    if (fieldChoice == 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    if (fieldChoice < 1 || fieldChoice > 3) {
        std::cout << Color::RED << "Invalid choice!" << Color::RESET << "\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    SortField field = fieldChoice == 2 ? SortField::AMOUNT : (fieldChoice == 3 ? SortField::TIMESTAMP : SortField::LOCATION);

    std::cout << "\n" << Color::CYAN << "Choose algorithm:" << Color::RESET << "\n";
    std::cout << "1. Quick Sort\n";
    std::cout << "2. Merge Sort\n";
    std::cout << Color::RED << "0. Go Back" << Color::RESET << "\n";
    std::cout << "Enter choice (0-2): ";
    
    int algoChoice;
    std::cin >> algoChoice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (algoChoice == 0) return;
    
    if (algoChoice < 1 || algoChoice > 2) {
        std::cout << Color::RED << "Invalid choice!" << Color::RESET << "\n";
        return;
    }
    
    auto comparator = [&](const Transaction& a, const Transaction& b) {
        switch (field) {
            case SortField::LOCATION: return a.location < b.location;
            case SortField::AMOUNT: return a.amount < b.amount;
            case SortField::TIMESTAMP: return a.timestamp < b.timestamp;
            default: return false;
        }
    };

    bool original = isUsingArray();

    std::cout << "\n" << Color::GREEN << "🔵 Testing ARRAY structure..." << Color::RESET << "\n";
    setActiveDataStructure(true);
    long arrayMemBefore = getMemoryUsageKB();
    auto startA = std::chrono::high_resolution_clock::now();
    if (algoChoice == 1)
        quickSortArray(transactionsArray.getDataPointer(), 0, transactionsArray.getSize() - 1, comparator);
    else
        mergeSortArray(transactionsArray.getDataPointer(), 0, transactionsArray.getSize() - 1, comparator);
    auto endA = std::chrono::high_resolution_clock::now();
    auto arrayDuration = std::chrono::duration<double, std::milli>(endA - startA).count();
    long arrayMemAfter = getMemoryUsageKB();
    long arrayMemUsed = arrayMemAfter - arrayMemBefore;

    std::cout << Color::BLUE << "🔵 Testing LINKEDLIST structure..." << Color::RESET << "\n";
    setActiveDataStructure(false);
    long listMemBefore = getMemoryUsageKB();
    auto startL = std::chrono::high_resolution_clock::now();
    
    int nodeCount = 0;
    TransactionNode* temp = transactionsLinkedList.getHead();
    while (temp && nodeCount < 5000) {
        nodeCount++;
        temp = temp->next;
    }
    
    TransactionNode* sorted;
    if (nodeCount >= 5000) {
        sorted = mergeSortLinkedListIterative(transactionsLinkedList.getHead(), comparator);
    } else {
        sorted = mergeSortLinkedList(transactionsLinkedList.getHead(), comparator);
    }
    
    TransactionLinkedList newList;
    for (TransactionNode* n = sorted; n; n = n->next)
        newList.addTransaction(n->data);
    transactionsLinkedList = newList;
    auto endL = std::chrono::high_resolution_clock::now();
    auto listDuration = std::chrono::duration<double, std::milli>(endL - startL).count();
    long listMemAfter = getMemoryUsageKB();
    long listMemUsed = listMemAfter - listMemBefore;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN 
              << "📊 Sorting Comparison (" << getFieldName(field) << ", " 
              << (algoChoice == 1 ? "QuickSort" : "MergeSort") << ")" << Color::RESET << "\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << std::left << std::setw(25) << Color::GREEN + "Array Time:" + Color::RESET
              << std::right << std::setw(15) << arrayDuration << " ms\n";
    std::cout << std::left << std::setw(25) << Color::GREEN + "Array Memory:" + Color::RESET
              << std::right << std::setw(15) << arrayMemUsed << " KB\n";
    std::cout << std::left << std::setw(25) << Color::BLUE + "LinkedList Time:" + Color::RESET
              << std::right << std::setw(15) << listDuration << " ms\n";
    std::cout << std::left << std::setw(25) << Color::BLUE + "LinkedList Memory:" + Color::RESET
              << std::right << std::setw(15) << listMemUsed << " KB\n";
    std::cout << std::string(60, '-') << "\n";
    
    if (arrayDuration > 0 && listDuration > 0) {
        if (arrayDuration < listDuration) {
            double speedup = listDuration / arrayDuration;
            std::cout << Color::BRIGHT_GREEN << "🏆 Array is " << speedup << "x faster!" << Color::RESET << "\n";
        } else {
            double speedup = arrayDuration / listDuration;
            std::cout << Color::BRIGHT_BLUE << "🏆 LinkedList is " << speedup << "x faster!" << Color::RESET << "\n";
        }
    }
    std::cout << std::string(60, '=') << "\n";

    setActiveDataStructure(original);
    std::cout << "\n" << Color::YELLOW << "📌 Active structure restored to: " 
              << getCurrentDataStructureName() << Color::RESET << "\n";
}

void TransactionManager::sortTransactions() {
    while (true) {
        std::cout << "\n" << Color::BOLD << Color::CYAN << "==========================================\n";
        std::cout << "           SORT TRANSACTIONS MENU          \n";
        std::cout << "==========================================" << Color::RESET << "\n";
        std::cout << Color::GREEN << " 1. Perform Sorting" << Color::RESET << "\n";
        std::cout << Color::BLUE << " 2. Compare Structures (Benchmark)" << Color::RESET << "\n";
        std::cout << Color::BRIGHT_MAGENTA << " 3. Switch Active Data Structure" << Color::RESET << "\n";
        std::cout << Color::RED << " 0. Return to Main Menu" << Color::RESET << "\n";
        std::cout << Color::BOLD << Color::CYAN << "==========================================" << Color::RESET << "\n";
        
        std::cout << Color::YELLOW << "Current Active Structure: " << Color::BRIGHT_YELLOW 
                  << getCurrentDataStructureName() << Color::RESET << "\n";
        std::cout << Color::YELLOW << "Total Transactions: " << Color::BRIGHT_BLUE 
                  << getTransactionCount() << Color::RESET << "\n\n";
        
        std::cout << "Enter your choice (0-3): ";
        
        int choice;
        std::cin >> choice;
        
        if (std::cin.fail()) {
            std::cout << Color::RED << "Invalid input. Please enter a number." << Color::RESET << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        switch (choice) {
            case 0:
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
                
            case 1:
                performSort(this);
                break;
                
            case 2:
                performFullStructureSortComparison();
                break;
                
            case 3:
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                switchDataStructure();
                break;
                
            default:
                std::cout << Color::RED << "Invalid choice! Please try again." << Color::RESET << "\n";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        
        if (choice != 0) {
            std::cout << "\n" << Color::CYAN << "Press Enter to continue..." << Color::RESET;
            std::cin.get();
        }
    }
}