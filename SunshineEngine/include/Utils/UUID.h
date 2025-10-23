#pragma once

#include <xhash>

// Класс-обёртка для UUID
class UUID
{
public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID&) = default;

    operator uint64_t() const;

    bool operator==(const UUID& other) const;
    bool operator!=(const UUID& other) const;

private:
    uint64_t m_UUID;
};

namespace std {
    template<>
    struct hash<UUID>
    {
        std::size_t operator()(const UUID& uuid) const
        {
            return std::hash<uint64_t>()((uint64_t)uuid);
        }
    };
    
}