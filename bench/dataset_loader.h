#pragma once
using namespace std;

#include <string>
#include <vector>
#include <cstddef>

struct Dataset {
    size_t n = 0;
    size_t dim = 0;
    vector<float> data;
};

Dataset load_fvecs(const string &path);
Dataset load_bin(const string &path);
// Dataset load_csv(const string &path);