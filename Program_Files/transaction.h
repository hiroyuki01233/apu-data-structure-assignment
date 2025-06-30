#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

struct Transaction {
    std::string transaction_id, timestamp,
                sender_account, receiver_account,
                transaction_type, merchant_category,
                location, device_used,
                fraud_type, time_since_last_transaction,
                payment_channel, ip_address, device_hash;
    double amount, spending_deviation_score,
           velocity_score, geo_anomaly_score;
    bool is_fraud;
};

#endif