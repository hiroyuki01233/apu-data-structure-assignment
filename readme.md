# CSV → JSON Converter

A high-performance, multithreaded command-line tool for converting large CSV files of financial transactions into newline-delimited JSON. Built with:

* [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) for blazing-fast CSV parsing
* [RapidJSON](https://rapidjson.org) for zero-allocation, streaming JSON output
* C++17 threading (producer–consumer) for parallel parsing and serialization

---

## Features

* **Streaming conversion** of arbitrarily large CSV files without loading the entire dataset into memory
* **Multithreaded pipeline**: separate parser and writer threads for maximum throughput
* **Buffered I/O**: 1 MiB user-space buffers reduce system call overhead
* **JSON output**: emits one JSON object per line for easy downstream processing

## Prerequisites

* A C++17-compatible compiler (e.g., `g++`, `clang++`)
* [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) (header-only)
* [RapidJSON](https://github.com/Tencent/rapidjson) (header-only)

## Repository Layout

```
├─ main.cpp              # Entry point; launches parser and writer threads
├─ writer.h              # JSON writer logic (uses RapidJSON)
├─ third_party/
│  ├─ fast-cpp-csv-parser/
│  │   └─ csv.h           # CSVReader
│  └─ rapidjson/
│      └─ include/
│          └─ rapidjson/   # RapidJSON headers
└─ README.md             # This file
```

## Installation & Build

1. **Clone** this repo (or copy files) to your project directory.
2. Make sure the header-only libraries are placed under `third_party/`:

   * `third_party/fast-cpp-csv-parser/csv.h`
   * `third_party/rapidjson/include/rapidjson/...`
3. **Compile** with:

   ```bash
   g++ main.cpp -o csv2json_converter
   chmod +x csv2json_converter
   ```

## Usage

```bash
./csv2json_converter <input.csv> <output.json>
```

* `<input.csv>`: path to your CSV file (must have the correct 18-column header)
* `<output.json>`: path where the JSON will be written

Example:

```bash
./csv2json_converter transactions.csv transactions.json
```

Then you can preview:

```bash
head -n 5 transactions.json
```

## CSV Format Expectations

The CSV must include a header row with exactly these 18 fields (in any order):

```
transaction_id, timestamp, sender_account, receiver_account,
amount, transaction_type, merchant_category, location,
device_used, is_fraud, fraud_type, time_since_last_transaction,
spending_deviation_score, velocity_score, geo_anomaly_score,
payment_channel, ip_address, device_hash
```

Each subsequent row should match those columns.

## Performance Tips

* Splitting a 700 MB CSV into multiple stripes and running multiple parser threads can yield near-linear speedups on multicore machines.
* Adjust the buffer size in `writerWorker` (`1<<20`) for your I/O subsystem.
* For maximum compactness, pipe the JSON output through `gzip -c > out.json.gz`.

## License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.

---

Enjoy fast, scalable CSV → JSON conversion!
