declare name "dubdub";

ui = library("ceammc_ui.lib");
spn = library("spn.lib");
sy = library("synths.lib");
ba = library("basics.lib");


process = sy.dubDub(freq, cutoff, ui.q(0.1), gate) with {
    freq = hslider("pitch", spn.C3, spn.C1, spn.C6, 0.001) : ba.midikey2hz;
    cutoff = hslider("cutoff", 3000, 20, 20000, 1);
    gate = button("gate");
};
