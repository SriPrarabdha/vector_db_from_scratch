#include "vector.h"
using namespace std;
namespace vdb {

    float Vector::norm() const {
        float sum = 0.0f;
        for(float v: data) {
            sum += v*v;
        }

        return sqrt(sum);
    }

    void Vector::normalize() {
        float n = norm();

        if(n > 0.0f) {
            for (float& v : data){
                v /= n;
            }
        }
    }
}