#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <nlohmann/json.hpp>

class TelemetryServer {
public:
    TelemetryServer(const std::string& socket_path);
    ~TelemetryServer();
    void start();
    void stop();
    void pushStats(const nlohmann::json& stats);

private:
    void run();
    std::string socket_path_;
    std::thread worker_thread_;
    std::atomic<bool> running_;
    std::mutex queue_mutex_;
    std::queue<nlohmann::json> stats_queue_;
    int server_fd_;
    int client_fd_;
};
