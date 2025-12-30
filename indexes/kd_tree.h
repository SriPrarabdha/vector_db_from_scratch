#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <cstddef>

#include "../core/vector.h"
using namespace std;

namespace vdb {

    struct KDTreeStats{
        size_t visited_nodes;
        size_t pruned_branches;
    };

    class KDTree{
        public:
            explicit KDTree(size_t dim);

            void build(const vector<Vector>& data);

            void search(const Vector& query,
                        size_t k,
                        vector<size_t>& out_indices,
                        vector<size_t>& out_distances,
                        KDTreeStats* stats = nullptr
                    )const;

        private:
            struct Node{
                size_t index;
                size_t axis;
                float split;
                unique_ptr<Node> left;
                unique_ptr<Node> right;
            };

            unique_ptr<Node> build_recursive(
                vector<size_t>& indices,
                size_t depth
            );

            void search_recursive(
                const Node* node, 
                const Vector& query,
                size_t k,
                priority_queue<pair<float , size_t>>& heap ,
                KDTreeStats* stats
            ) const;

        private:
            size_t dim_;
            const vector<Vector>* data_;
            unique_ptr<Node> root_;
    };
}