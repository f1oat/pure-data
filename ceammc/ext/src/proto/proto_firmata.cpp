#include "proto_firmata.h"
#include "ceammc_factory.h"
#include "ceammc_log.h"
#include "firmata/firmata_proto.h"
#include "firmata/firmata_proto_request.h"

#include <algorithm>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/static_assert.hpp>
#include <cmath>
#include <cstdio>

typedef unsigned char byte;

using namespace firmata;

static t_symbol* F_QUERY_FIRMWARE = gensym("firmware?");
static t_symbol* F_QUERY_VERSION = gensym("version?");
static t_symbol* F_QUERY_ANALOG_MAPPING = gensym("apin_mapping?");
static t_symbol* F_QUERY_PIN_STATE = gensym("pin_state?");
static t_symbol* F_QUERY_CAPABILITIES = gensym("capabilities?");

static t_symbol* F_RESPONSE_VERSION = gensym("version");
static t_symbol* F_RESPONSE_FIRMWARE = gensym("firmware");
static t_symbol* F_RESPONSE_ANALOG_MAPPING = gensym("apin_mapping");
static t_symbol* F_RESPONSE_PIN_STATE = gensym("pin_state");
static t_symbol* F_RESPONSE_PIN_INFO = gensym("pin_info");
static t_symbol* F_RESPONSE_ANALOG_PIN = gensym("apin");
static t_symbol* F_RESPONSE_DIGITAL_PORT = gensym("dport");
static t_symbol* F_RESPONSE_STRING = gensym("string");

static t_symbol* F_SET_SAMPLING_INTERVAL = gensym("samp_interval");
static t_symbol* F_SET_PIN_MODE = gensym("pin_mode");

static t_symbol* F_WRITE_DIGITAL_PIN = gensym("write_dpin");
static t_symbol* F_WRITE_PWM_PIN = gensym("write_pwm");

static t_symbol* F_REPORT_ANALOG_PIN = gensym("report_apin");
static t_symbol* F_REPORT_DIGITAL_PORT = gensym("report_dport");

// clang-format off
static t_symbol* PIN_MODE_INPUT   = gensym("INPUT");
static t_symbol* PIN_MODE_OUTPUT  = gensym("OUTPUT");
static t_symbol* PIN_MODE_ANALOG  = gensym("ANALOG");
static t_symbol* PIN_MODE_PWM     = gensym("PWM");
static t_symbol* PIN_MODE_SERVO   = gensym("SERVO");
static t_symbol* PIN_MODE_SHIFT   = gensym("SHIFT");
static t_symbol* PIN_MODE_I2C     = gensym("I2C");
static t_symbol* PIN_MODE_ONEWIRE = gensym("ONEWIRE");
static t_symbol* PIN_MODE_STEPPER = gensym("STEPPER");
static t_symbol* PIN_MODE_ENCODER = gensym("ENCODER");
static t_symbol* PIN_MODE_SERIAL  = gensym("SERIAL");
static t_symbol* PIN_MODE_PULLUP  = gensym("PULLUP");
// clang-format on

static t_symbol* pin_modes[] = {
    PIN_MODE_INPUT,
    PIN_MODE_OUTPUT,
    PIN_MODE_ANALOG,
    PIN_MODE_PWM,
    PIN_MODE_SERVO,
    PIN_MODE_SHIFT,
    PIN_MODE_I2C,
    PIN_MODE_ONEWIRE,
    PIN_MODE_STEPPER,
    PIN_MODE_ENCODER,
    PIN_MODE_SERIAL,
    PIN_MODE_PULLUP
};

BOOST_STATIC_ASSERT(sizeof(pin_modes) / sizeof(pin_modes[0]) == 12);

Firmata::Firmata(const PdArgs& a)
    : BaseObject(a)
{
    createOutlet();
}

void Firmata::onFloat(t_float f)
{
    byte v = static_cast<byte>(roundf(f));

    parser_ << v;

    if (parser_.isDone()) {
        processFirmataInput(parser_.msg().command);
        parser_.reset();
    }

    if (parser_.isError()) {
        OBJ_ERR << "parsing error";
        parser_.reset();
    }
}

void Firmata::onAny(t_symbol* s, const AtomList& lst)
{
    if (s == F_QUERY_VERSION) { // query firmata protocol version (answer is list like: [2 5])
        return floatTo(0, PROTO_PROTOCOL_VERSION);
    } else if (s == F_WRITE_DIGITAL_PIN) {
        if (!checkArgs(lst, ARG_BYTE, ARG_BOOL, s))
            return;

        return toArduino(ProtoRequest::setDigitalPinValue(lst[0].asInt(), lst[1].asInt()));

    } else if (s == F_WRITE_PWM_PIN) {
        if (!checkArgs(lst, ARG_BYTE, ARG_NATURAL, s))
            return;

        return toArduino(ProtoRequest::setAnalogPinValue(lst[0].asInt(), lst[1].asInt()));
    } else if (s == F_SET_PIN_MODE) { // set pin mode
        if (!checkArgs(lst, ARG_BYTE, ARG_SYMBOL, s))
            return;

        int mode = symToPinMode(lst[1].asSymbol());
        if (mode < 0) {
            OBJ_ERR << "invalid pin mode: " << lst[1];
            OBJ_DBG << "supported values are: ";
            BOOST_FOREACH (t_symbol* s, pin_modes) {
                OBJ_DBG << "    " << s->s_name;
            }
            return;
        }

        return toArduino(ProtoRequest::setDigitalPinMode(lst[0].asInt(), static_cast<FirmataPinMode>(mode)));
    } else if (s == F_REPORT_ANALOG_PIN) {
        if (!checkArgs(lst, ARG_BYTE, ARG_BOOL, s))
            return;

        return toArduino(ProtoRequest::enableAnalogReporting(lst[0].asInt(), lst[1].asInt()));
    } else if (s == F_REPORT_DIGITAL_PORT) {
        if (!checkArgs(lst, ARG_BYTE, ARG_BOOL, s))
            return;

        return toArduino(ProtoRequest::enableDigitalReporting(lst[0].asInt(), lst[1].asInt()));
    } else if (s == F_QUERY_ANALOG_MAPPING) {
        return toArduino(ProtoRequest::queryAnalogMapping());
    } else if (s == F_QUERY_CAPABILITIES) {
        return toArduino(ProtoRequest::queryCapabilities());
    } else if (s == F_QUERY_FIRMWARE) {
        // query firmata firmware version like list [StandartFirmata.ino 2 5]
        return toArduino(ProtoRequest::queryFirmware());
    } else if (s == F_SET_SAMPLING_INTERVAL) {
        if (!checkArgs(lst, ARG_FLOAT, s))
            return;

        int ms = lst[0].asInt();
        if (ms < 10 || ms > 10000) {
            OBJ_ERR << "sampling interval should be in range: [10-10000]";
            return;
        }

        return toArduino(ProtoRequest::setSamplingInterval(ms));
    } else if (s == F_QUERY_PIN_STATE) { // query pin state
        if (!checkArgs(lst, ARG_BYTE, s))
            return;

        return toArduino(ProtoRequest::queryPinState(lst[0].asInt()));
    } else {
        OBJ_ERR << "unknown message: " << s->s_name;
    }
}

void Firmata::processFirmataInput(int cmd)
{
    switch (cmd) {
    case PROTO_PROTOCOL_VERSION:
        anyTo(0, F_RESPONSE_VERSION, AtomList(parser_.versionMajor(), parser_.versionMinor()));
        break;
    case PROTO_ANALOG_IO_MESSAGE:
        anyTo(0, F_RESPONSE_ANALOG_PIN, AtomList(parser_.msg().pin, parser_.value()));
        break;
    case PROTO_DIGITAL_IO_MESSAGE:
        anyTo(0, F_RESPONSE_DIGITAL_PORT, AtomList(parser_.msg().pin, parser_.value()));
        break;
    case PROTO_START_SYSEX: {
        switch (parser_.msg().sysex_id) {
        case PROTO_SYSEX_ANALOG_MAPPING_RESPONSE: {

            for (int i = 0; i < parser_.msg().cur_pin_map_idx; i++) {
                int pin = parser_.msg().analog_pin_map[i];
                if (pin < 0)
                    continue;

                char buf[6];
                snprintf(buf, 5, "A%i", pin);
                anyTo(0, F_RESPONSE_ANALOG_MAPPING, AtomList(gensym(buf), Atom(i)));
            }
        } break;
        case PROTO_SYSEX_CAPABILITY_RESPONSE: {
            for (int pin = 0; pin < parser_.msg().pin_cap_idx; pin++) {
                const FirmataPinCapabilities& caps = parser_.msg().capabilities[pin];

                AtomList res;
                for (int i = 0; i < caps.num; i++) {
                    const FirmataPinCapability& c = caps.list[i];

                    // [pinInfo $PIN OUTPUT $resolution(
                    res.append(Atom(pin));
                    res.append(Atom(pinModeToSym(c.mode)));
                    res.append(Atom(c.resolution));
                    anyTo(0, F_RESPONSE_PIN_INFO, res);
                    res.clear();
                }
            }
        } break;
        case PROTO_SYSEX_REPORT_FIRMWARE: {
            AtomList res;
            res.append(gensym(parser_.str().c_str()));
            res.append(parser_.versionMajor());
            res.append(parser_.versionMinor());
            anyTo(0, F_RESPONSE_FIRMWARE, res);
        } break;
        case PROTO_SYSEX_PIN_STATE_RESPONSE: {
            AtomList res;
            res.append(parser_.msg().pin);
            res.append(pinModeToSym(parser_.msg().pin_mode));
            res.append(parser_.msg().pin_state.value);
            anyTo(0, F_RESPONSE_PIN_STATE, res);
        } break;
        case PROTO_SYSEX_STRING_DATA: {
            OBJ_ERR << parser_.str();
        } break;
        default:
            OBJ_ERR << "unsupported sysex: " << parser_.msg().sysex_id;
            break;
        }
    } break;
    default:
        OBJ_ERR << "unsupported action: " << cmd;
        break;
    }
}

void Firmata::toArduino(const std::string& cmd)
{
    for (size_t i = 0; i < cmd.size(); i++) {
        boost::uint8_t ch = static_cast<boost::uint8_t>(cmd[i]);
        floatTo(0, ch);
    }
}

t_symbol* Firmata::pinModeToSym(int mode)
{
    if (mode < 0 || mode >= boost::size(pin_modes))
        return &s_;

    return pin_modes[mode];
}

int Firmata::symToPinMode(t_symbol* s)
{
    t_symbol** begin = &pin_modes[0];
    t_symbol** end = begin + boost::size(pin_modes);
    t_symbol** pos = std::find(begin, end, s);

    if (pos == end)
        return -1;

    return std::distance(begin, pos);
}

void proto_firmata_setup()
{
    ObjectFactory<Firmata> obj("proto.firmata");
}
