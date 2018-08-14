declare name "DX7";

ui = library("ceammc_ui.lib");
spn = library("spn.lib");
pm = library("physmodels.lib");
ba = library("basics.lib");
dx = library("dx7.lib");
ma = library("maths.lib");


dx7 = par(i, 2, dx.dx7_algo(i, egR1, egR2, egR3, egR4,
            egL1, egL2, egL3, egL4,
            outLevel, keyVelSens, ampModSens,
            opMode, opFreq, opDetune, opRateScale,
            feedback,
            lfoDelay, lfoDepth, lfoSpeed,
            freq, gain, gate) : control(algorithm == i)) :> _
with{
    algorithm = nentry("algorithm", 0, 0, 31, 1) : int;
    feedback  = nentry("feedback", 0, 0, 99, 1) : dx.dx7_fdbkscalef/(2 * ma.PI);
    lfoDelay  = nentry("lfoDelay", 0, 0, 99, 1);
    lfoDepth  = nentry("lfoDepth", 0, 0, 99, 1);
    lfoSpeed  = nentry("lfoSpeed", 0, 0, 99, 1);
    freq = hslider("freq", 400, 50, 1000, 0.01);
    gain = hslider("gain", 0.8, 0, 1, 0.01);
    gate = button("gate");
    egR1UI = par(i, 6, nentry("op%i:egR1",90,0,99,1));
    egR1(n) = ba.take(n+1,egR1UI);
    egR2UI = par(i, 6, nentry("op%i:egR2",90,0,99,1));
    egR2(n) = ba.take(n+1,egR2UI);
    egR3UI = par(i,6,nentry("op%i:egR3",90,0,99,1));
    egR3(n) = ba.take(n+1,egR3UI);
    egR4UI = par(i,6,nentry("op%i:egR4",90,0,99,1));
    egR4(n) = ba.take(n+1,egR4UI);
    egL1UI = par(i,6,nentry("op%i:egL1",0,0,99,1));
    egL1(n) = ba.take(n+1,egL1UI);
    egL2UI = par(i,6,nentry("op%i:egL2",90,0,99,1));
    egL2(n) = ba.take(n+1,egL2UI);
    egL3UI = par(i,6,nentry("op%i:egL3",90,0,99,1));
    egL3(n) = ba.take(n+1,egL3UI);
    egL4UI = par(i,6,nentry("op%i:egL4",0,0,99,1));
    egL4(n) = ba.take(n+1,egL4UI);
    outLevelUI = par(i,6,nentry("op%i:level",95,0,99,1));
    outLevel(n) = ba.take(n+1,outLevelUI);
    keyVelSensUI = par(i,6,nentry("op%i:keyVelSens",1,0,8,1));
    keyVelSens(n) = ba.take(n+1,keyVelSensUI);
    ampModSensUI = par(i,6,nentry("op%i:ampModSens",0,0,99,1));
    ampModSens(n) = ba.take(n+1,ampModSensUI);
    opModeUI = par(i,6,nentry("op%i:opMode",0,0,1,1));
    opMode(n) = ba.take(n+1,opModeUI);
    opFreqUI = par(i,6,nentry("op%i:opFreq",1.0,0.0,2.0,0.01));
    opFreq(n) = ba.take(n+1,opFreqUI);
    opDetuneUI = par(i,6,nentry("op%i:opDetune",1,-10,10,1));
    opDetune(n) = ba.take(n+1,opDetuneUI);
    opRateScaleUI = par(i,6,nentry("op%i:opRateScale",0,0,10,1));
    opRateScale(n) = ba.take(n+1,opRateScaleUI);
};


process = dx7;
