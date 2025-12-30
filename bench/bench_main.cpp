#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <cassert>

#include "../core/vector.h"
#include "../core/types.h"
#include "../indexes/linear_scan.h"
#include "metrics.h"

using namespace vdb;

/* ---------------- Configuration ---------------- */

static constexpr dim_t DIM = 1024;
static constexpr size_t DATASET_SIZE = 100000;
static constexpr size_t NUM_QUERIES = 100;
static constexpr size_t K = 10;

/* ---------------- Utilities ---------------- */

Vector random_vector(dim_t d, std::mt19937& rng) {
    std::normal_distribution<float> dist(0.0f, 1.0f);
    Vector v(d);
    for (auto& x : v.data) x = dist(rng);
    return v;
}

std::vector<uint32_t> extract_ids(
    const std::vector<std::pair<uint32_t, float>>& res
) {
    std::vector<uint32_t> ids;
    for (auto& p : res) ids.push_back(p.first);
    return ids;
}

/* ---------------- Benchmark Case ---------------- */

struct BenchResult {
    double build_ms;
    double search_ms;
    float recall;
};

BenchResult run_benchmark(
    const std::string& name,
    const SearchConfig& cfg,
    const std::vector<Vector>& dataset,
    const std::vector<Vector>& queries,
    const std::vector<std::vector<uint32_t>>& gt_ids
) {
    std::cout << "\n[RUN] " << name << std::endl;

    /* Build index */
    Timer t_build;
    LinearScanIndex index(DIM, cfg);
    for (const auto& v : dataset) index.add(v);
    double build_ms = t_build.elapsed_ms();

    /* Search */
    Timer t_search;
    std::vector<std::vector<uint32_t>> results;
    results.reserve(queries.size());

    for (const auto& q : queries) {
        auto r = index.search(q, K);
        results.push_back(extract_ids(r));
    }
    double search_ms = t_search.elapsed_ms();

    /* Recall */
    float recall_sum = 0.0f;
    for (size_t i = 0; i < queries.size(); ++i) {
        recall_sum += recall_at_k(gt_ids[i], results[i]);
    }
    float recall = recall_sum / queries.size();

    std::cout << "  Build time   : " << build_ms  << " ms\n";
    std::cout << "  Search time  : " << search_ms << " ms\n";
    std::cout << "  QPS          : " 
              << (queries.size() * 1000.0 / search_ms) << "\n";
    std::cout << "  Recall@K     : " << recall << "\n";

    return {build_ms, search_ms, recall};
}

/* ---------------- Main ---------------- */

int main() {
    std::mt19937 rng(42);

    std::cout << "\n===== VectorDB Linear Scan Benchmark =====\n";
    std::cout << "Dataset size : " << DATASET_SIZE << "\n";
    std::cout << "Dimension    : " << DIM << "\n";
    std::cout << "Queries      : " << NUM_QUERIES << "\n";
    std::cout << "Top-K        : " << K << "\n";

    /* Generate dataset */
    std::vector<Vector> dataset;
    dataset.reserve(DATASET_SIZE);
    for (size_t i = 0; i < DATASET_SIZE; ++i) {
        dataset.push_back(random_vector(DIM, rng));
    }

    /* Generate queries */
    std::vector<Vector> queries;
    queries.reserve(NUM_QUERIES);
    for (size_t i = 0; i < NUM_QUERIES; ++i) {
        queries.push_back(random_vector(DIM, rng));
    }

    /* Ground truth (scalar baseline) */
    std::cout << "\n[INFO] Computing ground truth...\n";
    SearchConfig gt_cfg;
    gt_cfg.distance = DistanceType::L2_SCALAR;
    gt_cfg.exec = ExecPolicy::SINGLE_THREAD;
    gt_cfg.layout = LayoutType::AOS;

    LinearScanIndex gt_index(DIM, gt_cfg);
    for (const auto& v : dataset) gt_index.add(v);

    std::vector<std::vector<uint32_t>> gt_ids;
    for (const auto& q : queries) {
        auto r = gt_index.search(q, K);
        gt_ids.push_back(extract_ids(r));
    }

    std::vector<std::pair<std::string, SearchConfig>> cases = {
        {
            "Scalar (Single Thread)",
            {DistanceType::L2_SCALAR, ExecPolicy::SINGLE_THREAD, LayoutType::AOS}
        },
        {
            "AVX2 (Single Thread)",
            {DistanceType::L2_AVX2, ExecPolicy::SINGLE_THREAD, LayoutType::AOS}
        },
        {
            "AVX2 + OpenMP",
            {DistanceType::L2_AVX2, ExecPolicy::OPENMP, LayoutType::AOS}
        }
    };

    std::vector<BenchResult> results;

    for (auto& c : cases) {
        results.push_back(
            run_benchmark(c.first, c.second, dataset, queries, gt_ids)
        );
    }

    std::cout << "\n===== SUMMARY =====\n";
    std::cout << std::left
              << std::setw(30) << "Variant"
              << std::setw(15) << "Search(ms)"
              << std::setw(15) << "Speedup"
              << std::setw(10) << "Recall"
              << "\n";

    double base_time = results[0].search_ms;

    for (size_t i = 0; i < cases.size(); ++i) {
        std::cout << std::left
                  << std::setw(30) << cases[i].first
                  << std::setw(15) << results[i].search_ms
                  << std::setw(15) << (base_time / results[i].search_ms)
                  << std::setw(10) << results[i].recall
                  << "\n";

        assert(results[i].recall == 1.0f && "Recall regression detected!");
    }

    std::cout << "\n✔ All benchmarks passed correctness checks\n";
    return 0;
}
