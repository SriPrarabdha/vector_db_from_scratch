#pragma once
#include <vector>
#include <unordered_set>
#include <chrono>

using namespace std;
namespace vdb{

    class Timer {
        public :
            Timer() { reset(); }

            void reset() {start_ = chrono::high_resolution_clock::now();}

            double elapsed_ms() const {
                auto end = chrono::high_resolution_clock::now();
                return chrono::duration<double , milli> (end - start_).count();

            }

        private:
            chrono::high_resolution_clock::time_point start_;
    };

    inline float recall_at_k(
        const std::vector<uint32_t>& gt,
        const std::vector<uint32_t>& res
    ) {
        std::unordered_set<uint32_t> gt_set(gt.begin(), gt.end());
        size_t hit = 0;
        for (auto id : res) {
            if (gt_set.count(id)) hit++;
        }
        return static_cast<float>(hit) / gt.size();
    }
}