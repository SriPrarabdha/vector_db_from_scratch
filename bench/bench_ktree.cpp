#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <string>

#include "../core/vector.h"
#include "../indexes/kd_tree.h"
#include "metrics.h"
#include "../core/distance.h"

using namespace std;
using namespace vdb;

/* -------------------------------
   Simple CLI parsing
--------------------------------*/
size_t get_arg(int argc, char** argv, const string& name, size_t default_val) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == name) {
            return static_cast<size_t>(std::stoul(argv[i + 1]));
        }
    }
    return default_val;
}

int main(int argc, char** argv) {
    const size_t N   = get_arg(argc, argv, "--N",   100000);
    const size_t D   = get_arg(argc, argv, "--dim", 1024);
    const size_t K   = get_arg(argc, argv, "--K",   10);

    cout << "KD-tree benchmark (with Recall@K)\n";
    cout << "N=" << N << "  dim=" << D << "  K=" << K << "\n\n";

    /* -------------------------------
       Random data
    --------------------------------*/
    mt19937 rng(123);
    normal_distribution<float> dist(0.0f, 1.0f);

    vector<Vector> dataset;
    dataset.reserve(N);

    for (size_t i = 0; i < N; ++i) {
        Vector v(D);
        for (auto& x : v.data) x = dist(rng);
        dataset.push_back(move(v));
    }

    /* -------------------------------
       Build KD-tree
    --------------------------------*/
    KDTree kd_tree(D);
    KDTreeStats stats;

    Timer build_timer;
    kd_tree.build(dataset);
    double build_ms = build_timer.elapsed_ms();

    /* -------------------------------
       Query
    --------------------------------*/
    Vector query(D);
    for (auto& x : query.data) x = dist(rng);

    vector<size_t> kd_indices;
    vector<size_t> kd_dists;

    Timer search_timer;
    kd_tree.search(query, K, kd_indices, kd_dists , &stats);
    double search_ms = search_timer.elapsed_ms();

    /* -------------------------------
       Ground truth (brute force)
    --------------------------------*/
    vector<pair<size_t, float>> all;

    all.reserve(N);
    for (size_t i = 0; i < N; ++i) {
        float d = l2_dispatch(query, dataset[i] , DistanceType::L2_AVX2);
        all.emplace_back(i, d);
    }

    nth_element(
        all.begin(),
        all.begin() + K,
        all.end(),
        [](auto& a, auto& b) { return a.second < b.second; }
    );

    sort(
        all.begin(),
        all.begin() + K,
        [](auto& a, auto& b) { return a.second < b.second; }
    );

    vector<uint32_t> gt;
    for (size_t i = 0; i < K; ++i) {
        gt.push_back(all[i].first);
    }

    /* -------------------------------
       Recall@K
    --------------------------------*/
    vector<uint32_t> out;
    for (auto idx : kd_indices) {
        out.push_back(static_cast<uint32_t>(idx));
    }

    float recall = recall_at_k(gt, out);

    /* -------------------------------
       Results
    --------------------------------*/
    cout << "Build time:   " << build_ms  << " ms\n";
    cout << "Search time:  " << search_ms << " ms\n";
    cout << "Recall@K:     " << recall << "\n";
    cout << "Visited Nodes : " << stats.visited_nodes << "\n";
    cout << "Pruned Branches : " << stats.pruned_branches << "\n";


    return 0;
}
