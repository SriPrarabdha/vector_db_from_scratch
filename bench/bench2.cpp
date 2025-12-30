#include <iostream>
#include <random>

#include "../core/vector.h"
#include "../indexes/linear_scan.h"
#include "metrics.h"
#include "../indexes/kd_tree.h"

using namespace vdb;

int main() {
    constexpr dim_t D = 1024;
    constexpr size_t N = 100000;
    constexpr size_t K = 10;

    std::mt19937 rng(123);
    std::normal_distribution<float> dist(0.0f, 1.0f);

    LinearScanIndex knn_index(D);
    KDTree kd_tree(D);
    std::vector<Vector> dataset;

    for (size_t i = 0; i < N; ++i) {
        Vector v(D);
        for (auto& x : v.data) x = dist(rng);
        dataset.push_back(v);
        knn_index.add(v);
    }
    kd_tree.build(dataset);

    Vector query(D);
    for (auto& x : query.data) x = dist(rng);
    vector<size_t> out_indices;
    vector<size_t> out_dist;

    Timer t;
    auto knn_res = knn_index.search(query, K);
    kd_tree.search(query , K , out_indices , out_dist);
    double time_ms = t.elapsed_ms();

    // Ground truth = same linear scan
    std::vector<uint32_t> gt, out;
    for (auto& p : knn_res) out.push_back(p.first);
    gt = out;

    float recall = recall_at_k(gt, out);

    std::cout << "Linear scan time: " << time_ms << " ms\n";
    std::cout << "Recall@K: " << recall << "\n";

    return 0;
}