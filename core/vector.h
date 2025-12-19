#pragma once
#include <vector>
#include <cassert>
#include <cmath>

#include "types.h"

using namespace std;

namespace vdb{

    struct Vector{
        dim_t dim;
        vector<float> data;

        Vector() : dim(0) {}
        explicit Vector(dim_t d) : dim(d) , data(d , 0.0f) {}

        float* raw() {return data.data();}
        const float* raw() const { return data.data();}

        float norm() const;

        void normalize();
    };
}