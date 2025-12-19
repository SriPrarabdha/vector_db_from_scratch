#include "distance.h"
#include <cassert>

using namespace std;

namespace vdb {
    dist_t l2_distance (const Vector& a, const Vector& b){
        assert (a.dim == b.dim);

        float sum = 0.0f;
        for(dim_t i = 0; i<a.dim ; ++i){
            float d = (a.data[i] - b.data[i]);

            sum += d*d;
        }

        return sum;
    }

    dist_t cosine_distance(const Vector& a, const Vector& b){
        assert (a.dim == b.dim);

        float dot = 0.0f;
        float na = 0.0f;
        float nb = 0.0f;

        for(dim_t i = 0 ; i<a.dim ; ++i){
            dot += a.data[i] * b.data[i];
            na += a.data[i] * a.data[i];
            nb += b.data[i] * b.data[i];
        }

        if (na == 0.0f || nb == 0.0f) return 1.0f;

        return 1.0f - (dot / (sqrt(na) * sqrt(nb)));
    }
}