#include <iostream>
#include <string>
#include <thread>
#include "csv_json_processing.h"
#include "transaction_manager.h"

void performFullCsvToJsonConversion(const std::string& inFile, const std::string& outFile) {
    std::cout << ">>> Feature: Perform Full CSV to JSON Conversion (Bulk)\n";
    std::cout << "    This feature converts the entire CSV file to JSON using multithreading.\n";
    std::cout << "    Input CSV: " << inFile << "\n";
    std::cout << "    Output JSON: " << outFile << "\n";

    std::thread parser(parserWorker, inFile);
    std::thread writer(writerWorker, outFile);

    parser.join();
    writer.join();
    std::cout << "    CSV to JSON conversion complete.\n";
}