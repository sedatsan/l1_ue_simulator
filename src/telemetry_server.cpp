#include "l1_ue_simulator/telemetry_server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

TelemetryServer::TelemetryServer(const std::string& socket_path) 
    : socket_path_(socket_path), running_(false), server_fd_(-1), client_fd_(-1) {}

TelemetryServer::~TelemetryServer() {
    stop();
}

void TelemetryServer::start() {
    running_ = true;
    worker_thread_ = std::thread(&TelemetryServer::run, this);
}

void TelemetryServer::stop() {
    running_ = false;
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    if (client_fd_ != -1) {
        close(client_fd_);
        client_fd_ = -1;
    }
    if (server_fd_ != -1) {
        close(server_fd_);
        server_fd_ = -1;
    }
    unlink(socket_path_.c_str());
}

void TelemetryServer::pushStats(const nlohmann::json& stats) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if (stats_queue_.size() < 100) {
        stats_queue_.push(stats);
    }
}

void TelemetryServer::run() {
    server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1);

    unlink(socket_path_.c_str());
    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        return;
    }

    if (listen(server_fd_, 1) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return;
    }

    // Set non-blocking for accept to check running_
    int flags = fcntl(server_fd_, F_GETFL, 0);
    fcntl(server_fd_, F_SETFL, flags | O_NONBLOCK);

    while (running_) {
        if (client_fd_ == -1) {
            client_fd_ = accept(server_fd_, NULL, NULL);
            if (client_fd_ == -1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            std::cout << "Telemetry client connected." << std::endl;
            // Set client socket to non-blocking as well if needed, 
            // but here we use it in a dedicated thread.
        }

        nlohmann::json stats;
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (!stats_queue_.empty()) {
                stats = stats_queue_.front();
                stats_queue_.pop();
            }
        }

        if (!stats.empty()) {
            std::string data = stats.dump() + "\n";
            ssize_t sent = send(client_fd_, data.c_str(), data.size(), MSG_NOSIGNAL);
            if (sent == -1) {
                close(client_fd_);
                client_fd_ = -1;
                std::cout << "Telemetry client disconnected." << std::endl;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
