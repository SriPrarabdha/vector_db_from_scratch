#include "linear_scan.h"

#include <bits/stdc++.h>
#include <cassert>
#include <omp.h>

using namespace std;

namespace vdb {
    LinearScanIndex::LinearScanIndex(dim_t dim , SearchConfig cfg) : dim_(dim), cfg_(cfg) , soa_(0, dim) {}

    void LinearScanIndex::add(const Vector& v){
        assert (v.dim == dim_);

        aos_.push_back(v);

        if(cfg_.layout == LayoutType::SOA) {
            if(soa_.size == 0) soa_ = VectorBlock(1 , dim_);
            else{
                soa_.data.resize((soa_.size + 1) * dim_);
                soa_.size++;
            }

            copy(v.raw() , v.raw() + dim_ , soa_.row(soa_.size - 1));
        }
    }

    vector<pair<idx_t , dist_t>> LinearScanIndex::search(const Vector& query , size_t k) const {
        assert (query.dim == dim_);

        vector<pair<idx_t, dist_t>> results(aos_.size());
        
        auto compute = [&] (idx_t i) {
            dist_t d = l2_dispatch(query , aos_[i] , cfg_.distance);
            results[i] = {i , d};
        };

        if(cfg_.exec == ExecPolicy::OPENMP){
            #pragma omp parralel for schedule(static)
            for(idx_t i = 0 ; i<aos_.size() ; i++)compute(i);
        }else{
            for(idx_t i = 0 ; i<aos_.size() ; i++) compute(i);
        }

        if(results.size() > k){
            nth_element(results.begin() , 
                        results.begin() + k,
                        results.end() , 
                        [](auto& a , auto& b) {return a.second < b.second ;}
                    );
            results.resize(k);
        }

        sort(results.begin() , results.end() , 
            [](auto& a , auto& b) {return a.second < b.second ;}
        );

        return results;
    }

    vector<vector<pair<idx_t, dist_t>>> LinearScanIndex::batch_search(const vector<Vector>& queries , size_t k) const {
        vector<vector<pair<idx_t, dist_t>>> all;

        for(const auto& q : queries){
            all.emplace_back(search(q, k));
        }

        return all;
    }
}

