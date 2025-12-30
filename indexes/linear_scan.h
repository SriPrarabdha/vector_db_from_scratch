#pragma once 
#include "../bench/dataset_loader.h"
#include <vector>
#include <utility>
#include <algorithm>

#include "../core/vector.h"
#include "../core/distance.h"
#include "../core/vector_block.h"

using namespace std;
namespace vdb {
    class LinearScanIndex{
        public:
            explicit LinearScanIndex(dim_t dim , SearchConfig cfg = {}) ;

            void add(const Vector& v);

            vector<pair<idx_t , dist_t>> search(const Vector& query , size_t k) const;

            vector<vector<pair<idx_t , dist_t>>> batch_search(const vector<Vector>& queries , size_t k) const;

            size_t size() const {return aos_.size();}

        private:
            dim_t dim_;
            // vector<Vector> data_;
            SearchConfig cfg_;
            vector<Vector> aos_;
            VectorBlock soa_;
    };
}