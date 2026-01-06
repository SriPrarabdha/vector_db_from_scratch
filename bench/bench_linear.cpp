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

using namespace std;
using namespace vdb;

static constexpr dim_t DIM = 4;
static constexpr size_t DATASET_SIZE = 100000;
static constexpr size_t NUM_QUERIES = 1000;
static constexpr size_t K = 10;

struct CLIArgs {
    int threads = 1;
    string structure = "aos";
    string type = "scalar";
    bool show_help = false;
};

void print_usage(const char* prog_name) {
    cout << "Usage: " << prog_name << " [OPTIONS]\n\n"
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
        string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.show_help = true;
            return args;
        }
        else if (arg == "--threads" && i + 1 < argc) {
            stoi(argv[++i]);
        }
        else if (arg == "--structure" && i + 1 < argc) {
            args.structure = argv[++i];
            if (args.structure != "aos" && args.structure != "soa") {
                cerr << "Error: structure must be 'aos' or 'soa'\n";
                exit(1);
            }
        }
        else if (arg == "--type" && i + 1 < argc) {
            args.type = argv[++i];
            if (args.type != "scalar" && args.type != "avx2") {
                cerr << "Error: type must be 'scalar' or 'avx2'\n";
                exit(1);
            }
        }
        else {
            cerr << "Error: Unknown argument '" << arg << "'\n";
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

Vector random_vector(dim_t d, mt19937& rng) {
    normal_distribution<float> dist(0.0f, 1.0f);
    Vector v(d);
    for (auto& x : v.data) x = dist(rng);
    return v;
}

vector<uint32_t> extract_ids(
    const vector<pair<uint32_t, float>>& res
) {
    vector<uint32_t> ids;
    for (auto& p : res) ids.push_back(p.first);
    return ids;
}

struct BenchResult {
    double build_ms;
    double search_ms;
    float recall;
};

BenchResult run_benchmark(
    const string& name,
    const SearchConfig& cfg,
    const vector<Vector>& dataset,
    const vector<Vector>& queries,
    const vector<vector<uint32_t>>& gt_ids
) {
    cout << "\n[RUN] " << name << endl;

    /* Build index */
    Timer t_build;
    LinearScanIndex index(DIM, cfg);
    for (const auto& v : dataset) index.add(v);
    double build_ms = t_build.elapsed_ms();

    /* Search */
    Timer t_search;
    vector<vector<uint32_t>> results;
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

    cout << "  Build time   : " << build_ms  << " ms\n";
    cout << "  Search time  : " << search_ms << " ms\n";
    cout << "  QPS          : " 
              << (queries.size() * 1000.0 / search_ms) << "\n";
    cout << "  Recall@K     : " << recall << "\n";

    return {build_ms, search_ms, recall};
}

/* ---------------- Main ---------------- */

int main(int argc, char* argv[]) {
    CLIArgs args = parse_args(argc, argv);
    
    if (args.show_help) {
        print_usage(argv[0]);
        return 0;
    }
    
    mt19937 rng(42);

    cout << "\n===== VectorDB Linear Scan Benchmark =====\n";
    cout << "Dataset size : " << DATASET_SIZE << "\n";
    cout << "Dimension    : " << DIM << "\n";
    cout << "Queries      : " << NUM_QUERIES << "\n";
    cout << "Top-K        : " << K << "\n";
    cout << "\nConfiguration:\n";
    cout << "  Threads      : " << (args.threads == 0 ? "auto" : to_string(args.threads)) << "\n";
    cout << "  Structure    : " << args.structure << "\n";
    cout << "  Type         : " << args.type << "\n";

    /* Generate dataset */
    vector<Vector> dataset;
    dataset.reserve(DATASET_SIZE);
    for (size_t i = 0; i < DATASET_SIZE; ++i) {
        dataset.push_back(random_vector(DIM, rng));
    }

    /* Generate queries */
    vector<Vector> queries;
    queries.reserve(NUM_QUERIES);
    for (size_t i = 0; i < NUM_QUERIES; ++i) {
        queries.push_back(random_vector(DIM, rng));
    }

    /* Ground truth (scalar baseline) */
    cout << "\n[INFO] Computing ground truth...\n";
    SearchConfig gt_cfg;
    gt_cfg.distance = DistanceType::L2_SCALAR;
    gt_cfg.exec = ExecPolicy::SINGLE_THREAD;
    gt_cfg.layout = LayoutType::AOS;

    LinearScanIndex gt_index(DIM, gt_cfg);
    for (const auto& v : dataset) gt_index.add(v);

    vector<vector<uint32_t>> gt_ids;
    for (const auto& q : queries) {
        auto r = gt_index.search(q, K);
        gt_ids.push_back(extract_ids(r));
    }

    /* Create configuration from CLI args */
    SearchConfig test_cfg = create_config(args);
    
    /* Build name for the test */
    string test_name = args.type + " (" + args.structure + ", ";
    if (args.threads == 1) {
        test_name += "single thread)";
    } else {
        test_name += std::to_string(args.threads) + " threads)";
    }

    /* Run benchmark */
    BenchResult result = run_benchmark(test_name, test_cfg, dataset, queries, gt_ids);

    cout << "\n===== SUMMARY =====\n";
    cout << left
              << setw(40) << "Configuration"
              << setw(15) << "Search(ms)"
              << setw(10) << "Recall"
              << "\n";

    cout << left
              << setw(40) << test_name
              << setw(15) << result.search_ms
              << setw(10) << result.recall
              << "\n";

    if (result.recall < 0.99f) {
        cerr << "\n✗ Warning: Recall is below expected threshold!\n";
        return 1;
    }

    cout << "\n✔ Benchmark completed successfully\n";
    return 0;
}