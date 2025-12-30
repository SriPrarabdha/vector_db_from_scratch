#pragma once
#include "vector.h"
#include "simd.h"

using namespace std;

namespace vdb{

    dist_t l2_distance(const Vector& a , const Vector& b);
    dist_t cosine_distance(const Vector& a , const Vector& b);
    
    inline dist_t l2_dispatch(const Vector& a , const Vector& b , DistanceType type){
        if(type == DistanceType::L2_AVX2){
            return l2_avx2(a.raw() , b.raw() , a.dim);
        }

        return l2_distance(a , b);
    }
}