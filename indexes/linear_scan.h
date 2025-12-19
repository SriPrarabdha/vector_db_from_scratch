#pragma once 
#include "../bench/dataset_loader.h"
#include <vector>
#include <utility>
#include <algorithm>

#include "../core/vector.h"
#include "../core/distance.h"

using namespace std;

namespace vdb {
    class LinearScanIndex{
        public:
            explicit LinearScanIndex(dim_t dim);

            void add(const Vector& v);

            vector<pair<idx_t , dist_t>> search(const Vector& query , size_t k) const;

            size_t size() const {return data_.size();}
        private:
            dim_t dim_;
            vector<Vector> data_;
    };
}