#include "kd_tree.h"
#include <algorithm>
#include <cmath>
#include <limits>

#include "../core/distance.h"
using namespace std;

namespace vdb{
    KDTree::KDTree(size_t dim) : dim_(dim) , data_(nullptr) {}

    void KDTree::build(const vector<Vector>& data){
        data_ = &data;
        vector<size_t> indices(data.size());
        for(size_t i = 0; i<data.size() ; i++){
            indices[i] = i;
        }

        root_ = build_recursive(indices , 0);
    }

    unique_ptr<KDTree::Node> KDTree::build_recursive(vector<size_t>& indices , size_t depth){
        if(indices.empty()) return nullptr;

        size_t axis = depth%dim_;
        size_t mid = indices.size() / 2;

        nth_element(indices.begin(),
                    indices.begin() + mid , 
                    indices.end(),
                    [&](size_t a , size_t b){
                        return (*data_)[a].data[axis] < (*data_)[b].data[axis];
                });

        auto node = make_unique<Node>();
        node -> index = indices[mid];
        node -> axis = axis;
        node -> split = (*data_)[node->index].data[axis];

        vector<size_t> left(indices.begin() , indices.begin() + mid);
        vector<size_t> right(indices.begin() + mid + 1 , indices.end());

        node -> left = build_recursive(left , depth + 1);
        node -> right = build_recursive(right , depth + 1);

        return node;
    }

    void KDTree::search(
        const Vector& query , 
        size_t k,
        vector<size_t>& out_indices,
        vector<size_t>& out_distances,
        KDTreeStats* stats
    )const {
        
        priority_queue<pair<float, size_t>> heap;

        if(stats) *stats = KDTreeStats{};

        search_recursive(root_.get() , query , k , heap , stats);

        size_t n = heap.size();
        out_distances.resize(n);
        out_indices.resize(n);

        for(size_t i = 0 ; i<n ; i++){
            out_indices[n-1-i] = heap.top().second;
            out_distances[n-1-i] = heap.top().first;
            heap.pop();
        }
    }

    void KDTree::search_recursive(
        const Node* node,
        const Vector& query,
        size_t k,
        priority_queue<pair<float , size_t>>& heap ,
        KDTreeStats* stats 
    ) const {
        if(!node) return ;

        if(stats) stats->visited_nodes++;

        float dist = l2_distance((*data_)[node->index] , query);

        if(heap.size() < k) heap.emplace(dist , node->index);
        else if(heap.top().first > dist) {heap.pop() ; heap.emplace(dist , node->index);}

        float diff = query.data[node->axis] - node->split;
        const Node* near = diff <= 0 ? node->left.get() : node->right.get();
        const Node* far = diff<=0 ? node->right.get() : node->left.get();

        search_recursive(near , query , k , heap , stats);

        float worst = heap.size() < k ? numeric_limits<float>::infinity()
                                      : heap.top().first; 
        
        if(diff*diff < worst) search_recursive(far , query , k , heap , stats);

        else{
            if(stats) stats->pruned_branches++;
        }
    }
}