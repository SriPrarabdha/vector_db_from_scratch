#pragma once
#include "vector.h"

using namespace std;

namespace vdb{

    dist_t l2_distance(const Vector& a , const Vector& b);
    dist_t cosine_distance(const Vector& a , const Vector& b);
}