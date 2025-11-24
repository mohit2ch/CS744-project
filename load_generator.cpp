#include "httplib.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <cstring>
#include <mutex>
#include <iomanip>

using namespace std::chrono;

struct Config {
    std::string host = "127.0.0.1";
    int port = 8080;
    int threads = 4;
    int duration_sec = 30;
    std::string workload = "mix"; 
    int keys = 10000;      
    int popular = 10;      
    int think_ms = 0;      
};

std::atomic<uint64_t> total_attempts{0};
std::atomic<uint64_t> total_success{0};
std::atomic<uint64_t> total_failures{0};
// Sum of latencies in nanoseconds
std::atomic<long long> sum_latency_ns{0};

void print_usage(const char *prog) {
    std::cout << "Usage: " << prog << " [options]\n"
              << "Options:\n"
              << "  --host <host>            (default 127.0.0.1)\n"
              << "  --port <port>            (default 8080)\n"
              << "  --threads <n>            (default 4)\n"
              << "  --duration <sec>         (default 30)\n"
              << "  --workload <type>        put_all|get_all|get_popular|mix (default mix)\n"
              << "  --keys <n>               (unique keys count for get_all/put_all/mix; default 10000)\n"
              << "  --popular <n>            (popular keys count for get_popular; default 10)\n"
              << "  --think <ms>             (think time between requests in ms; default 0)\n"
              << "  --help\n";
}


inline int thread_unique_key(int thread_id, uint64_t counter, int keys) {
   
    return (int)((thread_id * 1315423911u + counter) % (uint32_t)keys);
}

void worker_thread(int tid, const Config &cfg, steady_clock::time_point end_time) {
    
    httplib::Client cli(cfg.host.c_str(), cfg.port);
    
    cli.set_connection_timeout(5); // seconds
    cli.set_read_timeout(10); // seconds
    cli.set_write_timeout(5); // seconds

    std::mt19937_64 rng((uint64_t)std::hash<std::thread::id>{}(std::this_thread::get_id()) ^ (uint64_t)tid ^ (uint64_t)steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> op_dist(0, 99); // for mix selection
    std::uniform_int_distribution<int> key_dist(0, cfg.keys - 1);
    // If popular workload, prepare small set of popular keys
    std::vector<int> popular_keys;
    popular_keys.reserve(cfg.popular);
    for (int i = 0; i < cfg.popular; ++i) popular_keys.push_back(i);

    uint64_t local_counter = 0;

    while (steady_clock::now() < end_time) {
        
        enum Op { PUT, GET, DELETE } op;
        int key = 0;

        if (cfg.workload == "put_all") {
            op = PUT;
            key = thread_unique_key(tid, local_counter++, cfg.keys);
        } else if (cfg.workload == "get_all") {
            op = GET;
            key = thread_unique_key(tid, local_counter++, cfg.keys);
        }  else { 
            int r = op_dist(rng);
            if (r < 40) op = GET;         // 40% reads
            else if (r < 75) op = PUT;    // 35% puts
            else op = DELETE;             // 25% deletes
            key = thread_unique_key(tid, local_counter++, cfg.keys);
        }

        
        std::string path = "/kv/" + std::to_string(key);

        
        std::string body;
        if (op == PUT) {
            std::ostringstream oss;
            oss << "val_tid" << tid << "_cnt" << local_counter;
            body = oss.str();
        }

        total_attempts.fetch_add(1, std::memory_order_relaxed);
        auto t0 = high_resolution_clock::now();

        bool success = false;
        try {
            // auto res;
            httplib::Result res;
            if (op == PUT) {
                res = cli.Put(path.c_str(), body, "text/plain");
            } else if (op == GET) {
                res = cli.Get(path.c_str());
            } else { 
                res = cli.Delete(path.c_str());
            }

            auto t1 = high_resolution_clock::now();
            auto latency = duration_cast<nanoseconds>(t1 - t0).count();
            sum_latency_ns.fetch_add(latency, std::memory_order_relaxed);

            if (res && (res->status >= 200 && res->status < 300)) {
                total_success.fetch_add(1, std::memory_order_relaxed);
                success = true;
            } else {
                total_failures.fetch_add(1, std::memory_order_relaxed);
                
            }
        } catch (const std::exception &ex) {
           
            auto t1 = high_resolution_clock::now();
            auto latency = duration_cast<nanoseconds>(t1 - t0).count();
            sum_latency_ns.fetch_add(latency, std::memory_order_relaxed);
            total_failures.fetch_add(1, std::memory_order_relaxed);
            
        }

        if (cfg.think_ms > 0) {
            std::this_thread::sleep_for(milliseconds(cfg.think_ms));
        }
    }
}

int main(int argc, char* argv[]) {
    Config cfg;
    
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--host" && i+1 < argc) { cfg.host = argv[++i]; }
        else if (a == "--port" && i+1 < argc) { cfg.port = std::stoi(argv[++i]); }
        else if (a == "--threads" && i+1 < argc) { cfg.threads = std::stoi(argv[++i]); }
        else if (a == "--duration" && i+1 < argc) { cfg.duration_sec = std::stoi(argv[++i]); }
        else if (a == "--workload" && i+1 < argc) { cfg.workload = argv[++i]; }
        else if (a == "--keys" && i+1 < argc) { cfg.keys = std::stoi(argv[++i]); if (cfg.keys<1) cfg.keys=1; }
        
        else if (a == "--think" && i+1 < argc) { cfg.think_ms = std::stoi(argv[++i]); }
        else if (a == "--help") { print_usage(argv[0]); return 0; }
        else {
            std::cerr << "Unknown arg: " << a << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    if (cfg.workload != "put_all" && cfg.workload != "get_all" &&
        cfg.workload != "get_popular" && cfg.workload != "mix") {
        std::cerr << "Invalid workload: " << cfg.workload << "\n";
        return 1;
    }

    std::cout << "Load generator config:\n"
              << " host=" << cfg.host << " port=" << cfg.port << "\n"
              << " threads=" << cfg.threads << " duration=" << cfg.duration_sec << "s\n"
              << " workload=" << cfg.workload << " keys=" << cfg.keys
             ;

    // compute end time
    auto start_time = steady_clock::now();
    auto end_time = start_time + seconds(cfg.duration_sec);

    // spawn threads
    std::vector<std::thread> threads;
    threads.reserve(cfg.threads);
    for (int t = 0; t < cfg.threads; ++t) {
        threads.emplace_back(worker_thread, t, std::ref(cfg), end_time);
    }

    // join threads
    for (auto &th : threads) th.join();

    // compute metrics
    uint64_t succ = total_success.load();
    uint64_t fail = total_failures.load();
    uint64_t attempts = total_attempts.load();
    long long sum_ns = sum_latency_ns.load();

    double elapsed_s = duration_cast<duration<double>>(steady_clock::now() - start_time).count();
    if (elapsed_s <= 0.0) elapsed_s = (double)cfg.duration_sec;

    double throughput = (double)succ / elapsed_s;
    double avg_latency_ms = succ > 0 ? (double)sum_ns / (double)succ / 1e6 : 0.0;

    std::cout << "\n=== Results ===\n"
              << "Duration (s): " << std::fixed << std::setprecision(2) << elapsed_s << "\n"
              << "Attempts: " << attempts << "\n"
              << "Successful: " << succ << "\n"
              << "Failed: " << fail << "\n"
              << "Throughput (successful req/sec): " << std::fixed << std::setprecision(2) << throughput << "\n"
              << "Average response time (ms): " << std::fixed << std::setprecision(3) << avg_latency_ms << "\n";

    return 0;
}
