#pragma once
#include <vector>
#include <cstdint>

#include "../core/vector.h"
#include "../core/distance.h"
#include "linear_scan.h"

namespace vdb {
    class IVFIndex{
        public:

            IVFIndex(dim_t dim , size_t nlist , SearchConfig cfg = {});

            void train(const vector<Vector>& data , size_t max_iters = 20);

            void add(const Vector& v);

            vector<pair<idx_t , dist_t>> search(const Vector& query , size_t k , size_t nprobe) const;

            size_t size() const {return ntotal_;}

        private:

            dim_t dim_;
            size_t nlist_; //no. of centroids we want
            SearchConfig cfg_;
            size_t ntotal_ = 0;

            vector<Vector> centroids_;
            vector<vector<Vector>> lists_;

            size_t assign_centroid(const Vector& v) const;

    };
}