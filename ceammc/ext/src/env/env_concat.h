#ifndef ENV_CONCAT_H
#define ENV_CONCAT_H

#include "ceammc_object.h"
#include "datatype_env.h"

using namespace ceammc;

class EnvConcat : public BaseObject {
    DataTypeEnv res_env_;
    DataTPtr<DataTypeEnv> right_;

public:
    EnvConcat(const PdArgs& args);

    void onBang() override;
    void onDataT(const DataTPtr<DataTypeEnv>& dptr);
    void onInlet(size_t n, const AtomList& lst) override;
};

void setup_env_concat();

#endif // ENV_CONCAT_H
