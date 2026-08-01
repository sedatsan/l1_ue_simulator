#pragma once
#include <vector>
#include <cstdint>
#include <memory>

struct UE {
    uint32_t id;
    uint32_t buffer_size; // in bytes
    uint32_t priority;

    UE(uint32_t id, uint32_t buffer_size, uint32_t priority)
        : id(id), buffer_size(buffer_size), priority(priority) {}
};

class UEManager {
public:
    void addUE(uint32_t id, uint32_t buffer_size, uint32_t priority) {
        ues_.push_back(std::make_shared<UE>(id, buffer_size, priority));
    }

    const std::vector<std::shared_ptr<UE>>& getUEs() const {
        return ues_;
    }

private:
    std::vector<std::shared_ptr<UE>> ues_;
};
