#ifndef RANDOM_LINEAR_H
#define RANDOM_LINEAR_H

#include "ceammc_object.h"

using namespace ceammc;

class RandomLinear : public BaseObject {
    FloatProperty* v0_;
    FloatProperty* v1_;
    FloatProperty* p0_;
    FloatProperty* p1_;

public:
    RandomLinear(const PdArgs& a);
    void onBang() override;
};

void setup_random_linear();

#endif // RANDOM_LINEAR_H
