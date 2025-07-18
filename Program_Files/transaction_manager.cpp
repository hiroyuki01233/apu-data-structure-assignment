#include "transaction_manager.h"
#include "csv_json_processing.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "third_party/fast-cpp-csv-parser/csv.h"
#include <chrono>
#include <sys/resource.h>
#include <cstdio>
#include <cstring>

static long getMemoryUsageKB() {
    long rss = 0L;
    FILE* fp = fopen("/proc/self/status", "r");
    if (fp != NULL) {
        char line[128];
        while (fgets(line, 128, fp) != NULL) {
            if (strncmp(line, "VmRSS:", 6) == 0) {
                char* p = line;
                while (*p && !isdigit(*p)) p++;
                rss = atol(p);
                break;
            }
        }
        fclose(fp);
    }
    return rss;
}

TransactionManager::TransactionManager()
    : transactionsArray(500000),
      transactionsLinkedList(),
      useArrayDataStructure(true)
{
    std::cout << "🔧 TransactionManager initialized. Both data structures are ready.\n";
    std::cout << "Current active data structure: " << getCurrentDataStructureName() << "\n";
}


bool TransactionManager::loadTransactionsFromCsv(const std::string& filePath) {
    std::cout << "Loading transactions from " << filePath << " into BOTH Array and LinkedList...\n";

    long memoryBefore = getMemoryUsageKB();
    auto totalStartTime = std::chrono::high_resolution_clock::now();

    io::CSVReader<18, io::trim_chars<' '>, io::no_quote_escape<','>> in(filePath);
    
    in.read_header(io::ignore_no_column, "transaction_id", "timestamp", "sender_account", "receiver_account", "amount", 
                   "transaction_type", "merchant_category", "location", "device_used", "is_fraud", "fraud_type", 
                   "time_since_last_transaction", "spending_deviation_score", "velocity_score", "geo_anomaly_score", 
                   "payment_channel", "ip_address", "device_hash");

    std::string id, ts, sa, ra, amt_s, tt, mc, loc, du,
                fraud_s, ft, tsl, sds_s, vs_s, gas_s, pc, ip, dh;

    int count = 0;
    std::chrono::duration<double, std::milli> array_load_time(0);
    std::chrono::duration<double, std::milli> list_load_time(0);

    while (in.read_row(id, ts, sa, ra, amt_s, tt, mc, loc, du, fraud_s, ft, tsl, sds_s, vs_s, gas_s, pc, ip, dh)) {
        if (count > 50000){
            break;
        }

        Transaction tx;
        tx.transaction_id = std::move(id);
        tx.timestamp = std::move(ts);
        tx.sender_account = std::move(sa);
        tx.receiver_account = std::move(ra);
        tx.amount = std::strtod(amt_s.c_str(), nullptr);
        tx.transaction_type = std::move(tt);
        tx.merchant_category = std::move(mc);
        tx.location = std::move(loc);
        tx.device_used = std::move(du);
        tx.is_fraud = (fraud_s == "True" || fraud_s == "true");
        tx.fraud_type = std::move(ft);
        tx.time_since_last_transaction = std::move(tsl);
        tx.spending_deviation_score = std::strtod(sds_s.c_str(), nullptr);
        tx.velocity_score = std::strtod(vs_s.c_str(), nullptr);
        tx.geo_anomaly_score = std::strtod(gas_s.c_str(), nullptr);
        tx.payment_channel = std::move(pc);
        tx.ip_address = std::move(ip);
        tx.device_hash = std::move(dh);

        auto start_array = std::chrono::high_resolution_clock::now();
        transactionsArray.addTransaction(tx);
        auto end_array = std::chrono::high_resolution_clock::now();
        array_load_time += (end_array - start_array);

        auto start_list = std::chrono::high_resolution_clock::now();
        transactionsLinkedList.addTransaction(tx);
        auto end_list = std::chrono::high_resolution_clock::now();
        list_load_time += (end_list - start_list);

        count++;
    }

    auto totalEndTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(totalEndTime - totalStartTime);
    long memoryAfter = getMemoryUsageKB();
    long memoryUsed = memoryAfter - memoryBefore;

    std::cout << "Transactions loaded successfully. Total: " << count << " transactions in each data structure.\n";
    
    std::cout << "-----------------------------------------\n";
    std::cout << "Time to populate TransactionArray:      " << array_load_time.count() << " ms\n";
    std::cout << "Time to populate TransactionLinkedList: " << list_load_time.count() << " ms\n";
    std::cout << "Total loading time:                     " << totalDuration.count() << " ms\n";
    std::cout << "Memory usage:                           " << memoryUsed << " KB (current total: " << memoryAfter << " KB)\n";
    std::cout << "-----------------------------------------\n";

    return true;
}

int TransactionManager::getTransactionCount() const {
    if (useArrayDataStructure) {
        return transactionsArray.getSize();
    } else {
        return transactionsLinkedList.getSize();
    }
}

Transaction* TransactionManager::getTransactionPtrAt(int index) {
    if (useArrayDataStructure) {
        return transactionsArray.getTransaction(index);
    } else {
        return transactionsLinkedList.getTransaction(index);
    }
}

std::string TransactionManager::getCurrentDataStructureName() const {
    if (useArrayDataStructure) {
        return "Array-Based (TransactionArray)";
    } else {
        return "LinkedList-Based (TransactionLinkedList)";
    }
}

void TransactionManager::displayCurrentStructureInfo() const {
     std::cout << "-----------------------------------------\n";
    std::cout << "Active Data Structure: " << getCurrentDataStructureName() << "\n";
    std::cout << "Total Transactions: " << getTransactionCount() << "\n";
    std::cout << "-----------------------------------------\n";
}

void TransactionManager::switchDataStructure() {
    std::cout << "\n## Switch Active Data Structure ##\n";
    std::cout << "=========================================\n";
    std::cout << "1. Use Array-Based Structure\n";
    std::cout << "2. Use LinkedList-Based Structure\n";
    std::cout << "-----------------------------------------\n";
    std::cout << "Current selection: " << getCurrentDataStructureName() << "\n";
    std::cout << "Enter your choice (1 or 2): ";

    int choice;
    std::cin >> choice;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\n❌ Invalid input. No changes made.\n";
        return;
    }

    if (choice == 1) {
        useArrayDataStructure = true;
        std::cout << "\n✅ Switched to Array-Based data structure.\n";
    } else if (choice == 2) {
        useArrayDataStructure = false;
        std::cout << "\n✅ Switched to LinkedList-Based data structure.\n";
    } else {
        std::cout << "\n❌ Invalid choice. No changes made.\n";
    }
    displayCurrentStructureInfo();
}