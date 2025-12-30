#pragma once
#include <immintrin.h>
#include<cstddef>

namespace vdb{

    float l2_avx2 (const float* a , const float* b , size_t dim);
}