#include "ivf.h"
#include <random>
#include <limits>
#include <cassert>
#include <iostream>

using namespace std;

namespace vdb{

    IVFIndex::IVFIndex(dim_t dim , size_t nlist , SearchConfig cfg):
        dim_(dim) , nlist_(nlist) , cfg_(cfg){
            centroids_.reserve(nlist_);
            lists_.reserve(nlist_);
        }
    
    void IVFIndex::train(const vector<Vector>& data , size_t max_iters){
        assert(data.size() >=nlist_);

        mt19937 rng(64);
        uniform_int_distribution<size_t> uni(0 , data.size() - 1);

        centroids_.clear();

        // randomly pick k data points to be centroid
        for(size_t i = 0 ; i<nlist_ ; ++i) centroids_.emplace_back(data[uni(rng)]);

        vector<size_t> assignments(data.size());
        for(size_t iter = 0 ; iter<max_iters ; ++iter){
            bool changed = false;
            
        }

        
    }
}