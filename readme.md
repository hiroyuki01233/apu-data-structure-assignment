# Financial Transaction Analysis System

This project is a C++ console application designed for managing and analyzing financial transaction data. It reads transaction data from large CSV files, stores it in custom-implemented data structures (array-based and linked list-based), and performs various data operations such as sorting, searching, filtering, and JSON output. The focus is on evaluating efficient data processing and algorithms as a foundation for a fraud detection system.

## Table of Contents

1.  [Project Overview](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#1-project-overview)
2.  [Features](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#2-features)
3.  [Data Structures and Algorithms](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#3-data-structures-and-algorithms)
4.  [Prerequisites](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#4-prerequisites)
5.  [Repository Layout](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#5-repository-layout)
6.  [Installation & Build](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#6-installation--build)
7.  [Usage](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#7-usage)
8.  [CSV Format Expectations](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#8-csv-format-expectations)
9.  [Performance Tips](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#9-performance-tips)
10. [License](https://github.com/hiroyuki01233/apu-data-structure-assignment/tree/main?tab=readme-ov-file#10-license)

-----

## 1\. Project Overview

This application is developed in response to the need for efficient transaction data management and analysis, prompted by a recent large-scale financial fraud incident at a digital bank[cite: 1]. It aims to efficiently manage and process vast amounts of financial transaction data[cite: 4]. The ability to search for specific transactions and sort large datasets efficiently is crucial for optimizing business operations[cite: 5]. Using C++, the project avoids reliance on built-in containers like `std::vector` or `std::list`[cite: 22], instead implementing custom data structures (arrays and linked lists) and algorithms to evaluate data processing performance and memory efficiency[cite: 7].

## 2\. Features

The system offers the following functionalities via a menu-driven interface:

  * **1. Load & Store Transactions by Payment Channel:** This feature loads financial transactions from a CSV file and organizes them into custom data structures, separated according to payment channel[cite: 10, 11].
  * **2. Sort Transactions by Location:** Sorts the stored financial transactions by `location` in ascending order[cite: 10, 11]. An efficient sorting algorithm suitable for the chosen data structure will be implemented[cite: 10, 16].
  * **3. Search Transactions by Type:** Allows searching for transactions of a specific type[cite: 10, 11]. An efficient searching algorithm based on the data structure will be selected and implemented[cite: 10, 17].
  * **4. Generate JSON from Specific Data Collection:** Exports a specific collection of transaction data (e.g., sorted data, data for a particular payment channel) from the internal custom data structures into JSON format[cite: 10, 11]. A third-party library like nlohmann/json or RapidJSON can be used for JSON export[cite: 11].
  * **5. Display All Stored Transactions:** Displays all financial transactions currently stored in memory to the console output[cite: 10].
  * **6. Perform Full CSV to JSON Conversion (Bulk):** Converts an entire specified CSV file to JSON format using a multithreaded producer-consumer pipeline. This feature is optimized for large files without loading the entire dataset into memory, providing high performance.

## 3\. Data Structures and Algorithms

This project focuses on deepening the understanding of data structures and algorithms, implemented according to the following requirements:

  * **Custom Data Structures:**
      * **Array-based:** An `TransactionArray` class is implemented to store transaction data using a dynamically allocated C-style array. It dynamically expands its capacity as needed[cite: 13].
      * **Linked List-based:** A `TransactionLinkedList` class is implemented to store transaction data using pointer-linked nodes[cite: 15].
      * The use of C++ built-in containers like `<vector>` or `<list>` is strictly prohibited[cite: 22].
  * **Sorting Algorithms:** An efficient sorting algorithm suitable for the chosen custom data structures will be implemented for sorting transactions by location[cite: 10, 16].
  * **Search Algorithms:** Efficient search algorithms will be implemented to filter and retrieve relevant transactions based on specific conditions, such as transaction type[cite: 10, 17].
  * **Performance Comparison:** The project will compare the time and memory efficiency of search and sorting algorithms for Arrays vs. Linked Lists[cite: 18].

## 4\. Prerequisites

  * A C++11-compatible compiler (e.g., `g++`, `clang++`). The project utilizes C++11 features such as `std::thread`.
  * [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) (header-only library)
  * [RapidJSON](https://github.com/Tencent/rapidjson) (header-only library)

## 5\. Repository Layout

```
├─ Assignemnt/
│  ├─ .vscode/
│  ├─ Data_Files/
│  │  └─ financial_fraud_detection_dataset.csv  # Input dataset [cite: 9]
│  │  └─ result.json                            # Example JSON output file
│  ├─ Program_Files/
│  │  ├─ third_party/                           # External libraries
│  │  │  ├─ fast-cpp-csv-parser/
│  │  │  │  └─ csv.h                           # fast-cpp-csv-parser header
│  │  │  └─ rapidjson/
│  │  │     └─ include/
│  │  │        └─ rapidjson/                    # RapidJSON headers
│  │  ├─ csv_json_processing.cpp                 # Implementation for thread-safe queue and worker functions
│  │  ├─ csv_json_processing.h                   # Declarations for thread-safe queue and worker functions
│  │  ├─ csv_to_json_conversion.cpp              # Implementation for bulk CSV to JSON conversion feature
│  │  ├─ custom_data_structures.cpp              # Implementation for custom data structures (TransactionArray, TransactionLinkedList)
│  │  ├─ custom_data_structures.h                # Declarations for custom data structures
│  │  ├─ load_store.cpp                          # (Deprecated - to be integrated into TransactionManager::loadTransactionsFromCsv)
│  │  ├─ main.cpp                                # Entry point; menu-driven interface [cite: 11]
│  │  ├─ tm_display_all.cpp                      # Implementation of TransactionManager::displayAll()
│  │  ├─ tm_generate_json.cpp                    # Implementation of TransactionManager::generateJson()
│  │  ├─ tm_sort_transactions.cpp                # Implementation of TransactionManager::sortTransactions()
│  │  ├─ tm_search_transactions.cpp              # Implementation of TransactionManager::searchTransactions()
│  │  ├─ tm_store_by_payment_channel.cpp         # Implementation of TransactionManager::storeByPaymentChannel()
│  │  ├─ transaction_features.h                  # Declarations for all main feature functions
│  │  ├─ transaction_manager.cpp                 # Core implementation of TransactionManager and CSV loading
│  │  ├─ transaction_manager.h                   # Declaration of TransactionManager class
│  │  └─ transaction.h                           # Definition of Transaction struct
│  ├─ Tmp Files/
│  └─ README.md                                 # This file
```

## 6\. Installation & Build

1.  Clone this repository or copy the files to your project directory.

2.  Ensure that the header-only libraries (`fast-cpp-csv-parser` and `RapidJSON`) are correctly placed under `Program_Files/third_party/`:

      * `Program_Files/third_party/fast-cpp-csv-parser/csv.h`
      * `Program_Files/third_party/rapidjson/include/rapidjson/...`

3.  Navigate to the `Program_Files` directory and compile using the following command:

    ```bash
    g++ main.cpp \
        transaction_manager.cpp \
        tm_store_by_payment_channel.cpp \
        tm_sort_transactions.cpp \
        tm_search_transactions.cpp \
        tm_generate_json.cpp \
        tm_display_all.cpp \
        custom_data_structures.cpp \
        csv_json_processing.cpp \
        csv_to_json_conversion.cpp \
        -o main \
        -I. \
        -Ithird_party/fast-cpp-csv-parser \
        -Ithird_party/rapidjson/include \
        -std=c++17 -pthread
    ```

4.  Grant executable permissions to the compiled binary:

    ```bash
    chmod +x main
    ```

## 7\. Usage

After compilation, run the program from the `Program_Files` directory using the following command. You must specify the CSV file path upon initial launch.

```bash
./main <input_csv_file_path>
```

  * `<input_csv_file_path>`: Path to your CSV file to be processed (e.g., `../Data_Files/financial_fraud_detection_dataset.csv`).

**Example:**

```bash
./main ../Data_Files/financial_fraud_detection_dataset.csv
```

Upon launching, an interactive menu will be displayed, allowing you to select various functionalities. For JSON output features (menu options 4 and 6), you will be prompted to enter the output file name.

## 8\. CSV Format Expectations

The input CSV file must include a header row with **exactly** these 18 fields (in any order)[cite: 9]:

```
transaction_id, timestamp, sender_account, receiver_account,
amount, transaction_type, merchant_category, location,
device_used, is_fraud, fraud_type, time_since_last_transaction,
spending_deviation_score, velocity_score, geo_anomaly_score,
payment_channel, ip_address, device_hash
```

Each subsequent row should match these columns.

## 9\. Performance Tips

  * **Multicore Optimization:** For large CSV files, the bulk CSV to JSON conversion feature (menu option 6) leverages multithreading, allowing for high throughput on multicore machines.
  * **Buffer Size:** The output buffer size configured in `writerWorker` within `csv_json_processing.cpp` (`1<<20`, i.e., 1 MiB) can be adjusted to optimize performance for your specific I/O subsystem.

## 10\. License

This project is released under the MIT License. See the `LICENSE` file for details.

-----

Enjoy fast, scalable financial transaction data analysis\!