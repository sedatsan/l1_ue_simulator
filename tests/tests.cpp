#include <cassert>
#include <iostream>
#include "l1_ue_simulator/ue_manager.hpp"
#include "l1_ue_simulator/scheduler.hpp"

void test_round_robin_basic() {
    UEManager ue_manager;
    ue_manager.addUE(1, 5000, 1); // 5 RBs worth
    ue_manager.addUE(2, 2000, 1); // 2 RBs worth
    
    RoundRobinScheduler scheduler(ue_manager);
    
    // Each RB = 1000 bytes.
    // Allocate 3 RBs.
    // RR should give RBs to UE 1, then UE 2, then UE 1 (if rotating per RB).
    // Or it could give some to UE 1, then some to UE 2.
    // Let's implement rotating per RB for "pure" Round Robin.
    
    scheduler.schedule(3);
    
    auto ues = ue_manager.getUEs();
    // UE 1 gets RB 1 and RB 3 -> 2000 bytes transmitted -> 3000 left
    // UE 2 gets RB 2 -> 1000 bytes transmitted -> 1000 left
    
    assert(ues[0]->buffer_size == 3000);
    assert(ues[1]->buffer_size == 1000);
    
    std::cout << "test_round_robin_basic passed!" << std::endl;
}

int main() {
    std::cout << "Running tests..." << std::endl;
    test_round_robin_basic();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
