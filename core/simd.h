#pragma once
#include <immintrin.h>
#include<cstddef>

namespace vdb{

    float l2_avx2 (const float* a , const float* b , size_t dim);
    // FLOAT l2_avx2_soa (const float* const* soa,   // soa[dim][num_vectors]
    // const float* query,        // AoS query vector
    // size_t dim,
    // size_t vec_base,            // starting vector index
    // float* out_distances)
}