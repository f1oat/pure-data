/*****************************************************************************
 * Copyright 2019 Serge Poltavsky. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#include "hoa_decoder.h"
#include "ceammc_convert.h"
#include "ceammc_factory.h"

static t_symbol* SYM_REGULAR;
static t_symbol* SYM_IRREGULAR;
static t_symbol* SYM_BINAURAL;
static t_symbol* SYM_PROP_MODE;

template <typename T>
constexpr bool is_in(T t, T v)
{
    return t == v;
}

template <typename T, typename... Args>
constexpr bool is_in(T t, T v, Args... args)
{
    return t == v || is_in(t, args...);
}

HoaDecoder::HoaDecoder(const PdArgs& args)
    : HoaBase(args)
    , mode_(nullptr)
    , plane_waves_(nullptr)
    , crop_size_(0)
{
    mode_ = new SymbolEnumProperty("@mode", { SYM_REGULAR, SYM_IRREGULAR, SYM_BINAURAL });
    addProperty(mode_);
    addProperty(new SymbolEnumAlias("@regular", mode_, SYM_REGULAR));
    addProperty(new SymbolEnumAlias("@irregular", mode_, SYM_IRREGULAR));
    addProperty(new SymbolEnumAlias("@binaural", mode_, SYM_BINAURAL));

    plane_waves_ = new IntProperty("@nwaves", 0);
    plane_waves_->setInitOnly();
    createProperty(plane_waves_);

    Property* pcrop = createCbIntProperty(
        "@crop",
        [this]() -> int { return propCropSize(); },
        [this](int v) -> bool { return propSetCropSize(v); });

    pcrop->setUnits(PropValueUnits::SAMP);
    pcrop->setIntCheck(PropValueConstraints::CLOSED_RANGE, 0, 512);

    createCbIntProperty("@nharm", [this]() -> int { return decoder_->getNumberOfHarmonics(); });
    createCbListProperty("@pw_x", [this]() -> AtomList { return propPlaneWavesX(); })
        ->setUnits(PropValueUnits::RAD);
    createCbListProperty("@pw_y", [this]() -> AtomList { return propPlaneWavesY(); })
        ->setUnits(PropValueUnits::RAD);
    createCbListProperty("@pw_z", [this]() -> AtomList { return propPlaneWavesZ(); })
        ->setUnits(PropValueUnits::RAD);

    createCbListProperty(
        "@angles",
        [this]() -> AtomList { return propAngles(); },
        [this](const AtomList& l) -> bool { return propSetAngles(l); })
        ->setUnits(PropValueUnits::DEG);

    createCbFloatProperty(
        "@offset",
        [this]() -> t_float { return convert::rad2degree(decoder_->getPlanewavesRotationZ()); },
        [this](t_float f) -> bool { return propSetOffset(f); })
        ->setUnits(PropValueUnits::DEG);
}

void HoaDecoder::parseMode()
{
    if (positionalArguments().size() > 1 && positionalArguments()[1].isSymbol())
        mode_->setValue(positionalArguments()[1].asSymbol());
}

void HoaDecoder::parsePlainWavesNum()
{
    const int NWAVES_ARG_IDX = 2;

    // num of plane waves ignored in binaural mode
    if (mode_->value() == SYM_BINAURAL) {
        plane_waves_->setValue(2);
    } else if (mode_->value() == SYM_REGULAR) {
        const int MIN_PW_COUNT = 2 * order() + 1;
        const int DEFAULT = 2 * order() + 2;

        // property was not specified, try positional arg
        if (plane_waves_->value() == 0)
            plane_waves_->setValue(positionalFloatArgumentT(NWAVES_ARG_IDX, DEFAULT));

        const auto N = plane_waves_->value();

        if (N >= MIN_PW_COUNT) {
            plane_waves_->setValue(N);
        } else {
            OBJ_ERR << "minimal number of plane waves for regular mode should be >= "
                    << MIN_PW_COUNT << ", setting to this value";
            plane_waves_->setValue(MIN_PW_COUNT);
        }
    } else if (mode_->value() == SYM_IRREGULAR) {
        const int MIN_PW_COUNT = 1;
        const int DEFAULT = 5;

        // property was not specified, try positional arg
        if (plane_waves_->value() == 0)
            plane_waves_->setValue(positionalFloatArgumentT(NWAVES_ARG_IDX, DEFAULT));

        const auto N = plane_waves_->value();

        if (N >= MIN_PW_COUNT) {
            plane_waves_->setValue(N);
        } else {
            OBJ_ERR << "minimal number of plane waves for irregular mode should be >= "
                    << MIN_PW_COUNT << ", setting to this value";
            plane_waves_->setValue(MIN_PW_COUNT);
        }
    } else {
        OBJ_ERR << "unknown mode: " << mode_->value();
    }
}

void HoaDecoder::parseProperties()
{
    HoaBase::parseProperties();

    parseMode();
    parsePlainWavesNum();

    initDecoder();

    propSetOffset(init_offset_);

    if (!init_angles_.empty())
        propSetAngles(init_angles_);

    createSignalInlets(decoder_->getNumberOfHarmonics());
    createSignalOutlets(decoder_->getNumberOfPlanewaves());

    in_buf_.resize(numInputChannels() * HOA_DEFAULT_BLOCK_SIZE);
    out_buf_.resize(numOutputChannels() * HOA_DEFAULT_BLOCK_SIZE);
}

void HoaDecoder::processBlock(const t_sample** in, t_sample** out)
{
    // not used
}

void HoaDecoder::processCommon()
{
    const size_t NINS = numInputChannels();
    const size_t NOUTS = numOutputChannels();
    const size_t BS = blockSize();

    t_sample** in = inputBlocks();
    t_sample** out = outputBlocks();

    for (size_t i = 0; i < NINS; i++) {
        Signal::copy(BS, &in[i][0], 1, &in_buf_[i], NINS);
    }

    for (size_t i = 0; i < BS; i++) {
        decoder_->process(&in_buf_[NINS * i], &out_buf_[NOUTS * i]);
    }

    for (size_t i = 0; i < NOUTS; i++) {
        Signal::copy(BS, &out_buf_[i], NOUTS, &out[i][0], 1);
    }
}

void HoaDecoder::processBinaural()
{
    t_sample** in_blocks = inputBlocks();
    t_sample** out_blocks = outputBlocks();
    DecoderBinaural2d* p = static_cast<DecoderBinaural2d*>(decoder_.get());
    p->processBlock((const t_sample**)in_blocks, out_blocks);
}

void HoaDecoder::setupDSP(t_signal** sp)
{
    SoundExternal::signalInit(sp);

    decoder_->prepare(blockSize());

    if (mode_->value() == SYM_BINAURAL) {
        dsp_add(dspPerformBinaural, 1, static_cast<void*>(this));
    } else {
        dsp_add(dspPerformCommon, 1, static_cast<void*>(this));
    }
}

void HoaDecoder::blockSizeChanged(size_t bs)
{
    in_buf_.resize(numInputChannels() * bs);
    out_buf_.resize(numOutputChannels() * bs);
}

void HoaDecoder::initDecoder()
{
    t_symbol* mode = mode_->value();
    if (mode == SYM_REGULAR) {
        decoder_.reset(new DecoderRegular2d(order(), plane_waves_->value()));
        decoder_->setPlanewavesRotation(0, 0, 0);
    } else if (mode == SYM_IRREGULAR) {
        decoder_.reset(new DecoderIrregular2d(order(), plane_waves_->value()));
    } else if (mode == SYM_BINAURAL) {
        DecoderBinaural2d* x = new DecoderBinaural2d(order());
        x->setCropSize(crop_size_);
        // to assure limits
        crop_size_ = x->getCropSize();
        decoder_.reset(x);
    } else {
        OBJ_ERR << "unknown mode: " << mode;
    }
}

int HoaDecoder::propCropSize() const
{
    if (mode_->value() == SYM_BINAURAL) {
        DecoderBinaural2d* p = static_cast<DecoderBinaural2d*>(decoder_.get());
        return p->getCropSize();
    } else
        return crop_size_;
}

bool HoaDecoder::propSetCropSize(int lst)
{
    crop_size_ = lst;

    if (mode_->value() == SYM_BINAURAL) {
        DecoderBinaural2d* p = static_cast<DecoderBinaural2d*>(decoder_.get());
        p->setCropSize(crop_size_);
        // to assure limits
        crop_size_ = p->getCropSize();
    }

    return true;
}

AtomList HoaDecoder::propPlaneWavesX() const
{
    auto N = decoder_->getNumberOfPlanewaves();
    AtomList res;
    res.reserve(N);
    for (size_t i = 0; i < N; i++)
        res.append(decoder_->getPlanewaveAbscissa(i, true));

    return res;
}

AtomList HoaDecoder::propPlaneWavesY() const
{
    auto N = decoder_->getNumberOfPlanewaves();
    AtomList res;
    res.reserve(N);
    for (size_t i = 0; i < N; i++)
        res.append(decoder_->getPlanewaveOrdinate(i, true));

    return res;
}

AtomList HoaDecoder::propPlaneWavesZ() const
{
    auto N = decoder_->getNumberOfPlanewaves();
    AtomList res;
    res.reserve(N);
    for (size_t i = 0; i < N; i++)
        res.append(decoder_->getPlanewaveHeight(i, true));

    return res;
}

AtomList HoaDecoder::propAngles() const
{
    auto N = decoder_->getNumberOfPlanewaves();
    AtomList res;
    res.reserve(N);
    for (size_t i = 0; i < N; i++)
        res.append(Atom(convert::rad2degree(decoder_->getPlanewaveAzimuth(i, true))));

    return res;
}

bool HoaDecoder::propSetAngles(const AtomList& lst)
{
    if (lst.empty()) {
        OBJ_ERR << "angle list in degrees expected";
        return false;
    }

    if (decoder_) {
        if (mode_->value() != SYM_IRREGULAR) {
            OBJ_ERR << "not in irregular mode: can not set angles";
            return false;
        }

        auto N = std::min<size_t>(decoder_->getNumberOfPlanewaves(), lst.size());
        for (size_t i = 0; i < N; i++)
            decoder_->setPlanewaveAzimuth(i, convert::degree2rad(lst[i].asFloat()));
    } else {
        // on init stage
        init_angles_ = lst;
    }

    return true;
}

bool HoaDecoder::propSetOffset(t_float f)
{
    if (decoder_) {
        if (mode_->value() == SYM_BINAURAL) {
            OBJ_ERR << "can not set offset in binaural mode";
            return false;
        }

        decoder_->setPlanewavesRotation(0, 0, convert::degree2rad(f));
    } else {
        // on init stage
        init_offset_ = f;
    }

    return true;
}

void setup_spat_hoa_decoder()
{
    SYM_REGULAR = gensym("regular");
    SYM_IRREGULAR = gensym("irregular");
    SYM_BINAURAL = gensym("binaural");
    SYM_PROP_MODE = gensym("@mode");

    SoundExternalFactory<HoaDecoder> obj("hoa.2d.decoder~");
    obj.addAlias("hoa.decoder~");
}
