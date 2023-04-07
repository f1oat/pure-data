declare name "dyn.comp_peak_ff";

import("stdfaust.lib");
cm = library("ceammc.lib");
ui = library("ceammc_ui.lib");

process = _ : cm.bypass_mono(ui.bypass, comp) with {
    comp = co.FFcompressor_N_chan(strength, thresh, att, rel, knee, pre, link, meter, 1);

    pre = 1;
    link = 0;
    strength = hslider("strength", 0, 0, 2, 0.01);
    knee     = hslider("knee [unit:db]", 0, 0, 12, 0.01);
    thresh   = hslider("threshold [unit:db]", -10, -60, 0, 0.1);
    att      = hslider("attack [unit:ms]", 10, 0.1, 100, 0.1) * 0.001;
    rel      = hslider("release [unit:ms]", 50, 1, 500, 0.1) * 0.001;


    meter = _ <: attach(_, ba.db2linear : (1-_) : hbargraph("level", 0, 1));
};

