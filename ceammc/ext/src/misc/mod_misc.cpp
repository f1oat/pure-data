#include "mod_misc.h"
#include "ceammc_config.h"
#include "speech_flite.h"

#ifdef CEAMMC_HAVE_MODPLUG
#include "misc_modplug.h"
#endif

extern "C" void setup_live0x2ecapture_tilde();

void ceammc_misc_setup()
{
    setup_live0x2ecapture_tilde();
    setup_misc_speech_flite();

#ifdef CEAMMC_HAVE_MODPLUG
    setup_misc0x2emodplug_tilde();
#endif
}
