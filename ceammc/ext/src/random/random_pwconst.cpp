#include <boost/algorithm/cxx11/is_sorted.hpp>

#include "ceammc_factory.h"
#include "random_pwconst.h"

static AtomList vector2list(const std::vector<t_float>& v)
{
    AtomList res;
    res.reserve(v.size());
    for (auto x : v)
        res.append(x);

    return res;
}

RandomPWConst::RandomPWConst(const PdArgs& a)
    : BaseObject(a)
    , seed_(nullptr)
{
    createOutlet();

    createCbListProperty(
        "@v",
        [this]() -> AtomList { return values_; },
        [this](const AtomList& l) -> bool { return set(l); })
        ->setArgIndex(0);

    createCbListProperty("@bounds", [this]() { return vector2list(bounds_); });
    createCbListProperty("@weights", [this]() { return vector2list(weights_); });

    seed_ = new SizeTProperty("@seed", 0);
    seed_->setSuccessFn([this](Property* p) { gen_.setSeed(seed_->value()); });
    addProperty(seed_);
}

void RandomPWConst::onBang()
{
    if (bounds_.size() < 3) {
        OBJ_ERR << "too few values..." << values_;
        return;
    }

    std::piecewise_constant_distribution<t_float> dist(
        bounds_.begin(), bounds_.end(), weights_.begin());

    floatTo(0, dist(gen_.get()));
}

void RandomPWConst::onList(const AtomList& v)
{
    if (set(v))
        onBang();
}

bool RandomPWConst::set(const AtomList& data)
{
    if (data.size() % 2 == 0) {
        OBJ_ERR << "expected odd number of arguments: boundary0, weight0, boundary1 etc...";
        return false;
    }

    if (data.size() < 5) {
        OBJ_ERR << "too few values..." << data;
        return false;
    }

    std::vector<t_float> b;
    std::vector<t_float> w;
    for (size_t i = 0; i < data.size(); i++) {
        b.push_back(data[i].asFloat());
        if (++i < data.size()) {
            w.push_back(data[i].asFloat());
        }
    }

    if (!boost::algorithm::is_strictly_increasing(b.begin(), b.end())) {
        OBJ_ERR << "boundary values shoud be in strictly increasing order: " << b;
        return false;
    }

    if (std::count_if(w.begin(), w.end(), std::bind2nd(std::less<t_float>(), 0))) {
        OBJ_ERR << "negative weights are found: " << w;
        return false;
    }

    values_ = data;
    bounds_ = b;
    weights_ = w;
    return true;
}

void setup_random_pw_const()
{
    ObjectFactory<RandomPWConst> obj("random.pw_const");

    obj.setDescription("piecewise constant random distribution");
    obj.addAuthor("Serge Poltavsky");
    obj.setKeywords({ "linear", "random", "piecewise", "const" });
    obj.setCategory("random");
    obj.setSinceVersion(0, 4);

    RandomPWConst::setInletsInfo(obj.classPointer(), { "bang: output new random\n"
                                                       "list: set new distribution values\n"
                                                       "args: b0 w0 b1 w1 b2..." });
    RandomPWConst::setOutletsInfo(obj.classPointer(), { "float: piecewise constant distributed random" });
}
