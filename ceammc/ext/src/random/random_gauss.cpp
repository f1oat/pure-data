#include <ctime>
#include <random>

#include "ceammc_factory.h"
#include "random_gauss.h"

RandomGauss::RandomGauss(const PdArgs& a)
    : BaseObject(a)
    , mu_(nullptr)
    , sigma_(nullptr)
    , seed_(nullptr)
{
    createOutlet();

    mu_ = new FloatProperty("@mu", 0);
    mu_->setArgIndex(0);
    sigma_ = new FloatProperty("@sigma", 0);
    sigma_->setArgIndexNext(mu_);
    sigma_->checkNonNegative();

    addProperty(mu_);
    addProperty(sigma_);

    seed_ = new SizeTProperty("@seed", 0);
    seed_->setSuccessFn([this](Property* p) { gen_.setSeed(seed_->value()); });
    addProperty(seed_);
}

void RandomGauss::onBang()
{
    std::normal_distribution<t_float> dist(mu_->value(), sigma_->value());
    floatTo(0, dist(gen_.get()));
}

void setup_random_gauss()
{
    ObjectFactory<RandomGauss> obj("random.gauss");

    obj.setDescription("gaussian random distribution");
    obj.addAuthor("Serge Poltavsky");
    obj.setKeywords({ "gauss", "random" });
    obj.setCategory("random");
    obj.setSinceVersion(0, 1);
}
