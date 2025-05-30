// File: transaction_manager.cpp
#include "transaction_manager.h"
#include "csv_json_processing.h" // parserWorker はここに残さないが、CSV読み込みロジックはここ
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib> // std::strtod のため
#include "third_party/fast-cpp-csv-parser/csv.h" // CSVパースに必要

TransactionManager::TransactionManager()
#ifdef USE_ARRAY_DATA_STRUCTURE
    : transactionsCollection(100) // アレイベースの初期容量を指定
#elif USE_LINKED_LIST_DATA_STRUCTURE
    : transactionsCollection() // リンクトリストの初期化
#endif
{
    // コンストラクタで、独自のデータ構造を初期化します
    // メンバー初期化リストで既に初期化済み
}

// デストラクタもここに記述 (もし必要なら)
// TransactionManager::~TransactionManager() {
//     // クリーンアップロジック (例: ポインタベースのデータ構造の場合)
// }


bool TransactionManager::loadTransactionsFromCsv(const std::string& filePath) {
    std::cout << "Loading transactions from " << filePath << "...\n";

    // io::CSVReaderを直接使用してカスタムデータ構造に格納
    io::CSVReader<18, io::trim_chars<' '>, io::no_quote_escape<','>> in(filePath);
    in.read_header(io::ignore_no_column,
        "transaction_id","timestamp","sender_account","receiver_account",
        "amount","transaction_type","merchant_category","location",
        "device_used","is_fraud","fraud_type","time_since_last_transaction",
        "spending_deviation_score","velocity_score","geo_anomaly_score",
        "payment_channel","ip_address","device_hash"
    );

    std::string id, ts, sa, ra, amt_s, tt, mc, loc, du,
                fraud_s, ft, tsl, sds_s, vs_s, gas_s, pc, ip, dh;

    while (in.read_row(
        id, ts, sa, ra, amt_s, tt, mc, loc, du,
        fraud_s, ft, tsl, sds_s, vs_s, gas_s, pc, ip, dh
    )) {
        Transaction tx;
        tx.transaction_id               = std::move(id);
        tx.timestamp                    = std::move(ts);
        tx.sender_account               = std::move(sa);
        tx.receiver_account             = std::move(ra);
        tx.amount                       = std::strtod(amt_s.c_str(), nullptr);
        tx.transaction_type             = std::move(tt);
        tx.merchant_category            = std::move(mc);
        tx.location                     = std::move(loc);
        tx.device_used                  = std::move(du);
        tx.is_fraud                     = (fraud_s == "True" || fraud_s == "true");
        tx.fraud_type                   = std::move(ft);
        tx.time_since_last_transaction  = std::move(tsl);
        tx.spending_deviation_score     = std::strtod(sds_s.c_str(), nullptr);
        tx.velocity_score               = std::strtod(vs_s.c_str(), nullptr);
        tx.geo_anomaly_score            = std::strtod(gas_s.c_str(), nullptr);
        tx.payment_channel              = std::move(pc);
        tx.ip_address                   = std::move(ip);
        tx.device_hash                  = std::move(dh);

        // 読み込んだTransactionオブジェクトをカスタムデータ構造に追加
        transactionsCollection.addTransaction(tx);
    }
    std::cout << "Transactions loaded successfully. Total: " << transactionsCollection.getSize() << " transactions.\n";
    return true;
}

// その他のメソッドは個別のファイルに移す