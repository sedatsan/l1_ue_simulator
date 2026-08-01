#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include <random>
#include <nlohmann/json.hpp>
#include "l1_ue_simulator/ue_manager.hpp"
#include "l1_ue_simulator/scheduler.hpp"
#include "l1_ue_simulator/telemetry_server.hpp"

std::atomic<bool> keep_running(true);

void signal_handler(int signal) {
    if (signal == SIGINT) {
        keep_running = false;
    }
}

class SimulationLoop {
public:
    void start() {
        UEManager ue_manager;
        // Instantiate UEManager with 5 dummy UEs
        for (uint32_t i = 1; i <= 5; ++i) {
            ue_manager.addUE(i, 10000, 1); // 10KB initial buffer
        }

        RoundRobinScheduler scheduler(ue_manager);
        TelemetryServer telemetry("/tmp/mac_sim.sock");
        telemetry.start();

        auto next_slot = std::chrono::steady_clock::now();
        const auto slot_duration = std::chrono::microseconds(125);
        uint64_t slot_count = 0;

        std::cout << "Starting 6G MAC Simulator (125us slots)..." << std::endl;

        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> ue_dist(0, 4);
        std::uniform_int_distribution<> data_dist(50000, 200000); // More data: 50KB to 200KB

        while (keep_running) {
            next_slot += slot_duration;
            
            // Traffic generation: every 10 slots, add random data to one UE's buffer
            if (slot_count % 10 == 0) {
                auto& ues = ue_manager.getUEs();
                size_t target_ue = ue_dist(gen);
                uint32_t new_data = data_dist(gen);
                ues[target_ue]->buffer_size += new_data;
            }

            // In each 125us slot, call scheduler.schedule
            const uint32_t rbs_per_slot = 100;
            scheduler.schedule(rbs_per_slot);

            // Telemetry: Every 800 slots (100ms)
            if (slot_count % 800 == 0) {
                nlohmann::json stats;
                stats["slot"] = slot_count;
                stats["ues"] = nlohmann::json::array();
                
                for (const auto& ue : ue_manager.getUEs()) {
                    nlohmann::json ue_json;
                    ue_json["id"] = ue->id;
                    ue_json["buffer"] = ue->buffer_size;
                    ue_json["rbs"] = scheduler.getAndResetUEAllocation(ue->id);
                    stats["ues"].push_back(ue_json);
                }
                telemetry.pushStats(stats);
            }

            // Simulation logging
            if (slot_count % 8000 == 0) { // Every 1 second
                std::cout << "Slot: " << slot_count << " (elapsed: " 
                          << (slot_count * 0.125) << " ms) | Telemetry pushed." << std::endl;
            }

            std::this_thread::sleep_until(next_slot);
            slot_count++;
        }
        telemetry.stop();
        std::cout << "Ticker stopped." << std::endl;
    }
};

int main() {
    std::signal(SIGINT, signal_handler);
    SimulationLoop loop;
    loop.start();
    return 0;
}
