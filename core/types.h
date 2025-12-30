#pragma once
#include <cstddef>
#include <cstdint>

namespace vdb{
        
    using dim_t = size_t;
    using idx_t = uint32_t;
    using dist_t = float;

    enum class DistanceType {
        L2_SCALAR,
        L2_AVX2
    };

    enum class ExecPolicy{
        SINGLE_THREAD,
        OPENMP
    };

    enum class LayoutType{
        AOS, // array of structures
        SOA  // structures of array
    };

    struct SearchConfig {
        DistanceType distance = DistanceType::L2_SCALAR;
        ExecPolicy exec = ExecPolicy::SINGLE_THREAD;
        LayoutType layout = LayoutType::AOS;
    };
}