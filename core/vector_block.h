#pragma once 
#include <vector>
#include "types.h"

using namespace std;
namespace vdb {

    struct VectorBlock{
        dim_t dim;
        size_t size;
        vector<float> data;

        VectorBlock(size_t n, dim_t d) : dim(d) , size(n) , data(n*d) {}

        float* row(size_t i){
            return data.data() + i*dim;
        }
    };
}