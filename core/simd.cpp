#include "simd.h"

namespace vdb {

float l2_avx2(const float* a, const float* b, size_t dim) {
    __m256 sum = _mm256_setzero_ps();
    size_t i = 0;

    for (; i + 8 <= dim; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 diff = _mm256_sub_ps(va, vb);
        sum = _mm256_fmadd_ps(diff, diff, sum);
    }

    float tmp[8];
    _mm256_storeu_ps(tmp, sum);

    float res = tmp[0] + tmp[1] + tmp[2] + tmp[3]
              + tmp[4] + tmp[5] + tmp[6] + tmp[7];

    for (; i < dim; ++i) {
        float d = a[i] - b[i];
        res += d * d;
    }

    return res;
}

}
