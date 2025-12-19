#include "linear_scan.h"
#include <bits/stdc++.h>
#include <cassert>

using namespace std;

namespace vdb {
    LinearScanIndex::LinearScanIndex(dim_t dim) : dim_(dim) {}

    void LinearScanIndex::add(const Vector& v){
        assert (v.dim == dim_);

        data_.push_back(v);
    }

    vector<pair<idx_t , dist_t>> LinearScanIndex::search(const Vector& query , size_t k) const {
        assert (query.dim == dim_);

        vector<pair<idx_t, dist_t>> results;

        results.reserve(data_.size());

        for(idx_t i = 0; i<data_.size() ; ++i){
            dist_t d = l2_distance(query , data_[i]);
            results.emplace_back(i, d);
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
}

