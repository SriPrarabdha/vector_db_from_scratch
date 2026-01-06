// #include <iostream>
// #include <vector>
// #include <random>
// #include <iomanip>
// #include <cassert>

// #include "../core/vector.h"
// #include "../core/types.h"
// #include "../indexes/linear_scan.h"
// #include "metrics.h"

// using namespace vdb;

// /* ---------------- Configuration ---------------- */

// static constexpr dim_t DIM = 1024;
// static constexpr size_t DATASET_SIZE = 100000;
// static constexpr size_t NUM_QUERIES = 100;
// static constexpr size_t K = 10;

// /* ---------------- Utilities ---------------- */

// Vector random_vector(dim_t d, std::mt19937& rng) {
//     std::normal_distribution<float> dist(0.0f, 1.0f);
//     Vector v(d);
//     for (auto& x : v.data) x = dist(rng);
//     return v;
// }

// std::vector<uint32_t> extract_ids(
//     const std::vector<std::pair<uint32_t, float>>& res
// ) {
//     std::vector<uint32_t> ids;
//     for (auto& p : res) ids.push_back(p.first);
//     return ids;
// }

// /* ---------------- Benchmark Case ---------------- */

// struct BenchResult {
//     double build_ms;
//     double search_ms;
//     float recall;
// };

// BenchResult run_benchmark(
//     const std::string& name,
//     const SearchConfig& cfg,
//     const std::vector<Vector>& dataset,
//     const std::vector<Vector>& queries,
//     const std::vector<std::vector<uint32_t>>& gt_ids
// ) {
//     std::cout << "\n[RUN] " << name << std::endl;

//     /* Build index */
//     Timer t_build;
//     LinearScanIndex index(DIM, cfg);
//     for (const auto& v : dataset) index.add(v);
//     double build_ms = t_build.elapsed_ms();

//     /* Search */
//     Timer t_search;
//     std::vector<std::vector<uint32_t>> results;
//     results.reserve(queries.size());

//     for (const auto& q : queries) {
//         auto r = index.search(q, K);
//         results.push_back(extract_ids(r));
//     }
//     double search_ms = t_search.elapsed_ms();

//     /* Recall */
//     float recall_sum = 0.0f;
//     for (size_t i = 0; i < queries.size(); ++i) {
//         recall_sum += recall_at_k(gt_ids[i], results[i]);
//     }
//     float recall = recall_sum / queries.size();

//     std::cout << "  Build time   : " << build_ms  << " ms\n";
//     std::cout << "  Search time  : " << search_ms << " ms\n";
//     std::cout << "  QPS          : " 
//               << (queries.size() * 1000.0 / search_ms) << "\n";
//     std::cout << "  Recall@K     : " << recall << "\n";

//     return {build_ms, search_ms, recall};
// }

// /* ---------------- Main ---------------- */

// int main() {
//     std::mt19937 rng(42);

//     std::cout << "\n===== VectorDB Linear Scan Benchmark =====\n";
//     std::cout << "Dataset size : " << DATASET_SIZE << "\n";
//     std::cout << "Dimension    : " << DIM << "\n";
//     std::cout << "Queries      : " << NUM_QUERIES << "\n";
//     std::cout << "Top-K        : " << K << "\n";

//     /* Generate dataset */
//     std::vector<Vector> dataset;
//     dataset.reserve(DATASET_SIZE);
//     for (size_t i = 0; i < DATASET_SIZE; ++i) {
//         dataset.push_back(random_vector(DIM, rng));
//     }

//     /* Generate queries */
//     std::vector<Vector> queries;
//     queries.reserve(NUM_QUERIES);
//     for (size_t i = 0; i < NUM_QUERIES; ++i) {
//         queries.push_back(random_vector(DIM, rng));
//     }

//     /* Ground truth (scalar baseline) */
//     std::cout << "\n[INFO] Computing ground truth...\n";
//     SearchConfig gt_cfg;
//     gt_cfg.distance = DistanceType::L2_SCALAR;
//     gt_cfg.exec = ExecPolicy::SINGLE_THREAD;
//     gt_cfg.layout = LayoutType::AOS;

//     LinearScanIndex gt_index(DIM, gt_cfg);
//     for (const auto& v : dataset) gt_index.add(v);

//     std::vector<std::vector<uint32_t>> gt_ids;
//     for (const auto& q : queries) {
//         auto r = gt_index.search(q, K);
//         gt_ids.push_back(extract_ids(r));
//     }

//     std::vector<std::pair<std::string, SearchConfig>> cases = {
//         {
//             "Scalar (Single Thread)",
//             {DistanceType::L2_SCALAR, ExecPolicy::SINGLE_THREAD, LayoutType::AOS}
//         },
//         {
//             "AVX2 (Single Thread)",
//             {DistanceType::L2_AVX2, ExecPolicy::SINGLE_THREAD, LayoutType::AOS}
//         },
//         {
//             "OpenMP",
//             {DistanceType::L2_SCALAR, ExecPolicy::OPENMP, LayoutType::AOS}
//         }
//     };

//     std::vector<BenchResult> results;

//     for (auto& c : cases) {
//         results.push_back(
//             run_benchmark(c.first, c.second, dataset, queries, gt_ids)
//         );
//     }

//     std::cout << "\n===== SUMMARY =====\n";
//     std::cout << std::left
//               << std::setw(30) << "Variant"
//               << std::setw(15) << "Search(ms)"
//               << std::setw(15) << "Speedup"
//               << std::setw(10) << "Recall"
//               << "\n";

//     double base_time = results[0].search_ms;

//     for (size_t i = 0; i < cases.size(); ++i) {
//         std::cout << std::left
//                   << std::setw(30) << cases[i].first
//                   << std::setw(15) << results[i].search_ms
//                   << std::setw(15) << (base_time / results[i].search_ms)
//                   << std::setw(10) << results[i].recall
//                   << "\n";

//         assert(results[i].recall == 1.0f && "Recall regression detected!");
//     }

//     std::cout << "\n✔ All benchmarks passed correctness checks\n";
//     return 0;
// }
#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <cassert>
#include <string>
#include <cstring>
#include <omp.h>

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

/* ---------------- CLI Arguments ---------------- */

struct CLIArgs {
    int threads = 1;
    std::string structure = "aos";
    std::string type = "scalar";
    bool show_help = false;
};

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [OPTIONS]\n\n"
              << "Options:\n"
              << "  --threads <N>        Number of threads (default: 1, use 0 for auto)\n"
              << "  --structure <TYPE>   Memory layout: aos or soa (default: aos)\n"
              << "  --type <TYPE>        Distance computation: scalar or avx2 (default: scalar)\n"
              << "  --help               Show this help message\n\n"
              << "Examples:\n"
              << "  " << prog_name << " --threads 10 --structure aos --type avx2\n"
              << "  " << prog_name << " --threads 4 --type scalar\n";
}

CLIArgs parse_args(int argc, char* argv[]) {
    CLIArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.show_help = true;
            return args;
        }
        else if (arg == "--threads" && i + 1 < argc) {
            args.threads = std::stoi(argv[++i]);
        }
        else if (arg == "--structure" && i + 1 < argc) {
            args.structure = argv[++i];
            if (args.structure != "aos" && args.structure != "soa") {
                std::cerr << "Error: structure must be 'aos' or 'soa'\n";
                exit(1);
            }
        }
        else if (arg == "--type" && i + 1 < argc) {
            args.type = argv[++i];
            if (args.type != "scalar" && args.type != "avx2") {
                std::cerr << "Error: type must be 'scalar' or 'avx2'\n";
                exit(1);
            }
        }
        else {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            print_usage(argv[0]);
            exit(1);
        }
    }
    
    return args;
}

SearchConfig create_config(const CLIArgs& args) {
    SearchConfig cfg;
    
    // Set distance type
    if (args.type == "scalar") {
        cfg.distance = DistanceType::L2_SCALAR;
    } else if (args.type == "avx2") {
        cfg.distance = DistanceType::L2_AVX2;
    }
    
    // Set execution policy
    if (args.threads == 1) {
        cfg.exec = ExecPolicy::SINGLE_THREAD;
    } else {
        cfg.exec = ExecPolicy::OPENMP;
        // Set number of threads for OpenMP if needed
        #ifdef _OPENMP
        if (args.threads > 0) {
            omp_set_num_threads(args.threads);
        }
        #endif
    }
    
    // Set layout type
    if (args.structure == "aos") {
        cfg.layout = LayoutType::AOS;
    } else if (args.structure == "soa") {
        cfg.layout = LayoutType::SOA;
    }
    
    return cfg;
}

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

int main(int argc, char* argv[]) {
    CLIArgs args = parse_args(argc, argv);
    
    if (args.show_help) {
        print_usage(argv[0]);
        return 0;
    }
    
    std::mt19937 rng(42);

    std::cout << "\n===== VectorDB Linear Scan Benchmark =====\n";
    std::cout << "Dataset size : " << DATASET_SIZE << "\n";
    std::cout << "Dimension    : " << DIM << "\n";
    std::cout << "Queries      : " << NUM_QUERIES << "\n";
    std::cout << "Top-K        : " << K << "\n";
    std::cout << "\nConfiguration:\n";
    std::cout << "  Threads      : " << (args.threads == 0 ? "auto" : std::to_string(args.threads)) << "\n";
    std::cout << "  Structure    : " << args.structure << "\n";
    std::cout << "  Type         : " << args.type << "\n";

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

    /* Create configuration from CLI args */
    SearchConfig test_cfg = create_config(args);
    
    /* Build name for the test */
    std::string test_name = args.type + " (" + args.structure + ", ";
    if (args.threads == 1) {
        test_name += "single thread)";
    } else {
        test_name += std::to_string(args.threads) + " threads)";
    }

    /* Run benchmark */
    BenchResult result = run_benchmark(test_name, test_cfg, dataset, queries, gt_ids);

    std::cout << "\n===== SUMMARY =====\n";
    std::cout << std::left
              << std::setw(40) << "Configuration"
              << std::setw(15) << "Search(ms)"
              << std::setw(10) << "Recall"
              << "\n";

    std::cout << std::left
              << std::setw(40) << test_name
              << std::setw(15) << result.search_ms
              << std::setw(10) << result.recall
              << "\n";

    if (result.recall < 0.99f) {
        std::cerr << "\n✗ Warning: Recall is below expected threshold!\n";
        return 1;
    }

    std::cout << "\n✔ Benchmark completed successfully\n";
    return 0;
}