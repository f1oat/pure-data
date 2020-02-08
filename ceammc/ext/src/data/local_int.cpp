#include "local_int.h"

LocalInt::LocalInt(const PdArgs& a)
    : LocalIntBase(a)
{
    property("@value")->info().setType(PropertyInfoType::INTEGER);
}

void setup_local_int()
{
    NumericIFaceFactory<LocalInt> obj("local.int");
}
