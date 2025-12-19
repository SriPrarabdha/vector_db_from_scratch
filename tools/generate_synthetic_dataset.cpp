#include <iostream>
#include <fstream>
#include <random>

int main() {
    constexpr int N = 10000;
    constexpr int D = 128;

    std::ofstream out("synthetic_10k_128.fvecs", std::ios::binary);

    std::mt19937 rng(42);
    std::normal_distribution<float> dist(0.0f, 1.0f);

    for (int i = 0; i < N; ++i) {
        out.write(reinterpret_cast<const char*>(&D), sizeof(int));
        for (int j = 0; j < D; ++j) {
            float v = dist(rng);
            out.write(reinterpret_cast<const char*>(&v), sizeof(float));
        }
    }

    std::cout << "Generated synthetic_10k_128.fvecs\n";
}
