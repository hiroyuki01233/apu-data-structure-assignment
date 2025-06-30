#include "csv_json_processing.h"
#include <iostream>
#include <cstdlib>
#include "third_party/fast-cpp-csv-parser/csv.h"
#include "third_party/rapidjson/include/rapidjson/prettywriter.h" 
#include "third_party/rapidjson/include/rapidjson/filewritestream.h"

ThreadSafeQueue<Transaction> txQueue;

void parserWorker(const std::string& inFile) {
    io::CSVReader<18, io::trim_chars<' '>, io::no_quote_escape<','>> in(inFile);
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

        txQueue.push(std::move(tx));
    }
    txQueue.finish();
}

void writerWorker(const std::string& outFile) {
    FILE* fp = std::fopen(outFile.c_str(), "wb");
    if (!fp) {
        std::perror("fopen");
        return;
    }
    static char writeBuf[1<<20];
    std::setvbuf(fp, writeBuf, _IOFBF, sizeof(writeBuf));
    rapidjson::FileWriteStream os(fp, writeBuf, sizeof(writeBuf));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

    writer.StartArray();
    Transaction tx;
    while (txQueue.pop(tx)) {
        writer.StartObject();
        writer.Key("transaction_id");           writer.String(tx.transaction_id.c_str());
        writer.Key("timestamp");                writer.String(tx.timestamp.c_str());
        writer.Key("sender_account");           writer.String(tx.sender_account.c_str());
        writer.Key("receiver_account");         writer.String(tx.receiver_account.c_str());
        writer.Key("amount");                   writer.Double(tx.amount);
        writer.Key("transaction_type");         writer.String(tx.transaction_type.c_str());
        writer.Key("merchant_category");        writer.String(tx.merchant_category.c_str());
        writer.Key("location");                 writer.String(tx.location.c_str());
        writer.Key("device_used");              writer.String(tx.device_used.c_str());
        writer.Key("is_fraud");                 writer.Bool(tx.is_fraud);
        writer.Key("fraud_type");               writer.String(tx.fraud_type.c_str());
        writer.Key("time_since_last_transaction"); writer.String(tx.time_since_last_transaction.c_str());
        writer.Key("spending_deviation_score"); writer.Double(tx.spending_deviation_score);
        writer.Key("velocity_score");           writer.Double(tx.velocity_score);
        writer.Key("geo_anomaly_score");        writer.Double(tx.geo_anomaly_score);
        writer.Key("payment_channel");          writer.String(tx.payment_channel.c_str());
        writer.Key("ip_address");               writer.String(tx.ip_address.c_str());
        writer.Key("device_hash");              writer.String(tx.device_hash.c_str());
        writer.EndObject();

        os.Put('\n');
    }
    writer.EndArray();
    std::fclose(fp);
}

void saveTransactionsToJson(const TransactionArray& transactions, const std::string& outFile) {
    FILE* fp = std::fopen(outFile.c_str(), "wb");
    if (!fp) {
        std::perror(("Error opening file for writing: " + outFile).c_str());
        return;
    }

    char writeBuf[1 << 20];
    rapidjson::FileWriteStream os(fp, writeBuf, sizeof(writeBuf));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

    writer.StartArray();
    for (int i = 0; i < transactions.getSize(); ++i) {
        const Transaction* tx = transactions.getTransaction(i);
        if (tx == nullptr) continue;

        writer.StartObject();
        writer.Key("transaction_id");           writer.String(tx->transaction_id.c_str());
        writer.Key("timestamp");                writer.String(tx->timestamp.c_str());
        writer.Key("sender_account");           writer.String(tx->sender_account.c_str());
        writer.Key("receiver_account");         writer.String(tx->receiver_account.c_str());
        writer.Key("amount");                   writer.Double(tx->amount);
        writer.Key("transaction_type");         writer.String(tx->transaction_type.c_str());
        writer.Key("merchant_category");        writer.String(tx->merchant_category.c_str());
        writer.Key("location");                 writer.String(tx->location.c_str());
        writer.Key("device_used");              writer.String(tx->device_used.c_str());
        writer.Key("is_fraud");                 writer.Bool(tx->is_fraud);
        writer.Key("fraud_type");               writer.String(tx->fraud_type.c_str());
        writer.Key("time_since_last_transaction"); writer.String(tx->time_since_last_transaction.c_str());
        writer.Key("spending_deviation_score"); writer.Double(tx->spending_deviation_score);
        writer.Key("velocity_score");           writer.Double(tx->velocity_score);
        writer.Key("geo_anomaly_score");        writer.Double(tx->geo_anomaly_score);
        writer.Key("payment_channel");          writer.String(tx->payment_channel.c_str());
        writer.Key("ip_address");               writer.String(tx->ip_address.c_str());
        writer.Key("device_hash");              writer.String(tx->device_hash.c_str());
        writer.EndObject();
    }
    writer.EndArray();
    std::fclose(fp);
}