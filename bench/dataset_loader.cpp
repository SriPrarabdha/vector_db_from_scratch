#include "dataset_loader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

Dataset load_fvecs(const string &path){
    Dataset ds;
    ifstream ifs(path , ios::binary);
    if (!ifs) throw runtime_error("failed to open " + path);

    while(ifs) {
        u_int32_t dim = 0;
        ifs.read(reinterpret_cast<char*>(&dim) , sizeof(dim));
        if(!ifs) break;

        vector<float> vec(dim);
        ifs.read(reinterpret_cast<char*>(vec.data()) , sizeof(float)*dim);
        if(!ifs) break;

        if(ds.dim = 0) ds.dim = dim;
        if(dim != ds.dim) throw runtime_error("inconsistent dim in fvecs");

        ds.data.insert(ds.data.end() , vec.begin() , vec.end());
        ds.n++;
    }

    return ds;
}

Dataset load_bin(const string &path){
    Dataset ds;

    ifstream ifs(path , ios::binary);
    if(!ifs) throw runtime_error("failed to open " + path);

    u_int32_t n , dim;
    ifs.read(reinterpret_cast<char*>(&n) , sizeof(n));
    ifs.read(reinterpret_cast<char*>(&dim) , sizeof(dim));

    ds.n = n;
    ds.dim = dim;

    ds.data.resize(static_cast<size_t>(n) * dim);
    ifs.read(reinterpret_cast<char*>(ds.data.data()) , sizeof(float)*ds.data.size());
    return ds;
}