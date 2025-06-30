#ifndef CSV_JSON_PROCESSING_H
#define CSV_JSON_PROCESSING_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "transaction.h"
#include "custom_data_structures.h"


template<typename T>
class ThreadSafeQueue {
    std::queue<T>           _q;
    std::mutex              _m;
    std::condition_variable _cv;
    bool                    _finished = false;
public:
    void push(T&& v) {
        {
            std::lock_guard<std::mutex> lk(_m);
            _q.push(std::move(v));
        }
        _cv.notify_one();
    }
    bool pop(T& out) {
        std::unique_lock<std::mutex> lk(_m);
        _cv.wait(lk, [&]{ return !_q.empty() || _finished; });
        if (_q.empty()) return false;
        out = std::move(_q.front());
        _q.pop();
        return true;
    }
    void finish() {
        {
            std::lock_guard<std::mutex> lk(_m);
            _finished = true;
        }
        _cv.notify_all();
    }
};

extern ThreadSafeQueue<Transaction> txQueue;

void parserWorker(const std::string& inFile);

void writerWorker(const std::string& outFile);

void saveTransactionsToJson(const TransactionArray& transactions, const std::string& outFile);


#endif