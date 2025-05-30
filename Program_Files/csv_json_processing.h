#ifndef CSV_JSON_PROCESSING_H
#define CSV_JSON_PROCESSING_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue> // std::queue を使用するため
#include "transaction.h" // Transaction 構造体を使用するため

// A simple thread-safe queue for Transactions (ここに移す)
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

// グローバルキューはここではなく、.cppファイルで定義するか、
// クラスのメンバーとして渡すことを検討すべきですが、
// 今回はシンプルにするためにここで定義
extern ThreadSafeQueue<Transaction> txQueue; // 外部リンケージを宣言

// Parser worker: reads CSV and enqueues Transactions
void parserWorker(const std::string& inFile);

// Writer worker: dequeues Transactions and streams them out as JSON
void writerWorker(const std::string& outFile);

#endif // CSV_JSON_PROCESSING_H