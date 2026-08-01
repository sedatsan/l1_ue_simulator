#pragma once
#include "l1_ue_simulator/ue_manager.hpp"
#include <iostream>
#include <map>

class RoundRobinScheduler {
public:
    RoundRobinScheduler(UEManager& ue_manager) : ue_manager_(ue_manager), current_ue_idx_(0) {}

    void schedule(uint32_t available_resource_blocks) {
        auto& ues = ue_manager_.getUEs();
        if (ues.empty()) return;

        const uint32_t bytes_per_rb = 1000;

        for (uint32_t rb = 0; rb < available_resource_blocks; ++rb) {
            // Find next UE with data in buffer
            bool found_ue = false;
            for (size_t i = 0; i < ues.size(); ++i) {
                size_t ue_idx = (current_ue_idx_ + i) % ues.size();
                if (ues[ue_idx]->buffer_size > 0) {
                    // Transmit data
                    uint32_t to_transmit = std::min(ues[ue_idx]->buffer_size, bytes_per_rb);
                    ues[ue_idx]->buffer_size -= to_transmit;
                    
                    // Track RB allocation
                    allocated_rbs_[ues[ue_idx]->id]++;
                    
                    // Move to next UE for next RB
                    current_ue_idx_ = (ue_idx + 1) % ues.size();
                    found_ue = true;
                    break;
                }
            }
            if (!found_ue) break; // No more data to transmit
        }
    }

    uint32_t getAndResetUEAllocation(uint32_t ue_id) {
        uint32_t rbs = allocated_rbs_[ue_id];
        allocated_rbs_[ue_id] = 0;
        return rbs;
    }

private:
    UEManager& ue_manager_;
    size_t current_ue_idx_;
    std::map<uint32_t, uint32_t> allocated_rbs_;
};
