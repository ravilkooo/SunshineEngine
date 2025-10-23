#pragma once

#include <xhash>
#include <EASTL/hash_map.h>
#include <EASTL/hash_map.h>

namespace Sunshine
{
    class UUID
    {
    public:
        UUID();
        explicit UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const;

        bool operator==(const UUID& other) const noexcept;
        bool operator!=(const UUID& other) const noexcept;

        size_t operator()(const UUID& uuid) const
        {
            return std::hash<uint64_t>()((uint64_t)uuid);
        }

        uint64_t m_UUID;
    };
}

// возможно не пригодитс€

namespace std {
    template<>
    struct hash<Sunshine::UUID>
    {
        std::size_t operator()(const Sunshine::UUID& uuid) const
        {
            return std::hash<uint64_t>()((uint64_t)uuid);
        }
    };  
}

/*
namespace eastl {
    template <>
    struct hash<Sunshine::UUID>
    {
        size_t operator()(const Sunshine::UUID& uuid) const noexcept
        {
            // ƒл€ 64-бит UUID Ч просто возвращаем value
            return static_cast<size_t>(uuid.m_UUID);
        }
    };
}
*/