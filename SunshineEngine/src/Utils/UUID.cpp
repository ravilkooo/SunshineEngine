#include "Utils/UUID.h"
#include <random>

namespace Sunshine {
    static std::random_device s_randomDevice;
    static std::mt19937_64 s_rng{ s_randomDevice() };
    static std::uniform_int_distribution<uint64_t> s_dist;


    UUID::UUID()
        : m_UUID(s_dist(s_rng))
    {
    }

    UUID::UUID(uint64_t uuid)
        : m_UUID(uuid)
    {
    }

    UUIDhilo UUID::GetHilo()
    {
        return UUIDhilo{
            (uint32_t)(m_UUID >> 32),
            (uint32_t)(m_UUID & 0xFFFFFFFF)
        };
    }

    UUID::operator uint64_t() const { return m_UUID; }

    bool UUID::operator==(const UUID& other) const noexcept { return m_UUID == other.m_UUID; }
    bool UUID::operator!=(const UUID& other) const noexcept { return !(*this == other); }
}