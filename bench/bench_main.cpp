#include <iostream>
#include <random>

#include "../core/vector.h"
#include "../indexes/linear_scan.h"
#include "metrics.h"

using namespace vdb;

int main() {
    constexpr dim_t D = 1024;
    constexpr size_t N = 100000;
    constexpr size_t K = 10;

    std::mt19937 rng(123);
    std::normal_distribution<float> dist(0.0f, 1.0f);

    LinearScanIndex index(D);
    std::vector<Vector> dataset;

    for (size_t i = 0; i < N; ++i) {
        Vector v(D);
        for (auto& x : v.data) x = dist(rng);
        dataset.push_back(v);
        index.add(v);
    }

    Vector query(D);
    for (auto& x : query.data) x = dist(rng);

    Timer t;
    auto res = index.search(query, K);
    double time_ms = t.elapsed_ms();

    // Ground truth = same linear scan
    std::vector<uint32_t> gt, out;
    for (auto& p : res) out.push_back(p.first);
    gt = out;

    float recall = recall_at_k(gt, out);

    std::cout << "Linear scan time: " << time_ms << " ms\n";
    std::cout << "Recall@K: " << recall << "\n";

    return 0;
}


