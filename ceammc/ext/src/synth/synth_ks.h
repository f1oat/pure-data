/* ------------------------------------------------------------
name: "synth.ks"
Code generated with Faust 2.18.7 (https://faust.grame.fr)
Compilation options: -lang cpp -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __synth_ks_H__
#define  __synth_ks_H__

// FAUST Architecture File for ceammc::SoundExternal class
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

/************************** BEGIN dsp.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>
#include <vector>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'addBtton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Returns the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param sample_rate - the sampling rate in Hertz
         */
        virtual void init(int sample_rate) = 0;

        /**
         * Init instance state
         *
         * @param sample_rate - the sampling rate in Hertz
         */
        virtual void instanceInit(int sample_rate) = 0;

        /**
         * Init instance constant state
         *
         * @param sample_rate - the sampling rate in Hertz
         */
        virtual void instanceConstants(int sample_rate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (delay lines...) */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with successive in/out audio buffers.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         *
         */
        virtual void compute(double /*date_usec*/, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = nullptr):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int sample_rate) { fDSP->init(sample_rate); }
        virtual void instanceInit(int sample_rate) { fDSP->instanceInit(sample_rate); }
        virtual void instanceConstants(int sample_rate) { fDSP->instanceConstants(sample_rate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
        virtual std::string getCompileOptions() = 0;
        virtual std::vector<std::string> getLibraryList() = 0;
        virtual std::vector<std::string> getIncludePathnames() = 0;
    
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/**************************  END  dsp.h **************************/
/************************** BEGIN UI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __UI_H__
#define __UI_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

template <typename REAL>
class UIReal
{
    
    public:
        
        UIReal() {}
        virtual ~UIReal() {}
        
        // -- widget's layouts
        
        virtual void openTabBox(const char* label) = 0;
        virtual void openHorizontalBox(const char* label) = 0;
        virtual void openVerticalBox(const char* label) = 0;
        virtual void closeBox() = 0;
        
        // -- active widgets
        
        virtual void addButton(const char* label, REAL* zone) = 0;
        virtual void addCheckButton(const char* label, REAL* zone) = 0;
        virtual void addVerticalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
        virtual void addHorizontalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
        virtual void addNumEntry(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
        
        // -- passive widgets
        
        virtual void addHorizontalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
        virtual void addVerticalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
        
        // -- soundfiles
        
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;
        
        // -- metadata declarations
        
        virtual void declare(REAL* zone, const char* key, const char* val) {}
};

class UI : public UIReal<FAUSTFLOAT>
{

    public:

        UI() {}
        virtual ~UI() {}
};

#endif
/**************************  END  UI.h **************************/
/************************** BEGIN meta.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __meta__
#define __meta__

struct Meta
{
    virtual ~Meta() {};
    virtual void declare(const char* key, const char* value) = 0;
    
};

#endif
/**************************  END  meta.h **************************/
/************************** BEGIN misc.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/
 
#ifndef __misc__
#define __misc__

#include <algorithm>
#include <map>
#include <cstdlib>
#include <string.h>
#include <fstream>
#include <string>


using std::max;
using std::min;

struct XXXX_Meta : std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key] = value; }
};

struct MY_Meta : Meta, std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key] = value; }
};

static int lsr(int x, int n) { return int(((unsigned int)x) >> n); }

static int int2pow2(int x) { int r = 0; while ((1<<r) < x) r++; return r; }

static long lopt(char* argv[], const char* name, long def)
{
	int	i;
    for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return std::atoi(argv[i+1]);
	return def;
}

static bool isopt(char* argv[], const char* name)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return true;
	return false;
}

static const char* lopts(char* argv[], const char* name, const char* def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return argv[i+1];
	return def;
}

static std::string pathToContent(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ifstream::binary);
    
    file.seekg(0, file.end);
    int size = int(file.tellg());
    file.seekg(0, file.beg);
    
    // And allocate buffer to that a single line can be read...
    char* buffer = new char[size + 1];
    file.read(buffer, size);
    
    // Terminate the string
    buffer[size] = 0;
    std::string result = buffer;
    file.close();
    delete [] buffer;
    return result;
}

#endif

/**************************  END  misc.h **************************/

#include "ceammc_faust.h"

using namespace ceammc::faust;

#ifdef FAUST_MACRO
// clang-format off
// clang-format on
#endif

#define sym(name) xsym(name)
#define xsym(name) #name

// clang-format off
#ifndef FAUST_MACRO
struct synth_ks : public dsp {
};
#endif
// clang-format on

#ifdef FAUST_MACRO
// clang-format off
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <math.h>

static float synth_ks_faustpower2_f(float value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS synth_ks
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class synth_ks : public dsp {
	
 private:
	
	FAUSTFLOAT fHslider0;
	int IOTA;
	float fRec7[2048];
	int fSampleRate;
	float fConst0;
	float fConst1;
	FAUSTFLOAT fHslider1;
	float fConst2;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fHslider3;
	float fConst3;
	FAUSTFLOAT fHslider4;
	int iRec12[2];
	float fRec11[3];
	float fConst4;
	FAUSTFLOAT fHslider5;
	FAUSTFLOAT fButton0;
	float fVec0[2];
	int iRec13[2];
	int iRec14[2];
	float fConst5;
	FAUSTFLOAT fHslider6;
	float fRec16[3];
	float fRec15[3];
	int iRec17[2];
	float fVec1[2048];
	float fRec3[2];
	float fRec0[3];
	float fRec1[3];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("envelopes.lib/ar:author", "Stéphane Letz");
		m->declare("envelopes.lib/author", "GRAME");
		m->declare("envelopes.lib/copyright", "GRAME");
		m->declare("envelopes.lib/license", "LGPL with exception");
		m->declare("envelopes.lib/name", "Faust Envelope Library");
		m->declare("envelopes.lib/version", "0.0");
		m->declare("filename", "synth_ks.dsp");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/highpass:author", "Julius O. Smith III");
		m->declare("filters.lib/highpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "synth.ks");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "0.0");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "0.1");
		m->declare("spn.lib/name", "Standart Pitch Notation constants");
		m->declare("spn.lib/version", "0.1");
	}

	virtual int getNumInputs() {
		return 0;
		
	}
	virtual int getNumOutputs() {
		return 1;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	
	static void classInit(int sample_rate) {
		
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = (0.00147058826f * fConst0);
		fConst2 = (0.00882352982f * fConst0);
		fConst3 = (6911.50391f / fConst0);
		fConst4 = (0.00200000009f * fConst0);
		fConst5 = (3.14159274f / fConst0);
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.0f);
		fHslider1 = FAUSTFLOAT(48.0f);
		fHslider2 = FAUSTFLOAT(0.0f);
		fHslider3 = FAUSTFLOAT(1.0f);
		fHslider4 = FAUSTFLOAT(1.0f);
		fHslider5 = FAUSTFLOAT(0.25f);
		fButton0 = FAUSTFLOAT(0.0f);
		fHslider6 = FAUSTFLOAT(0.69999999999999996f);
		
	}
	
	virtual void instanceClear() {
		IOTA = 0;
		for (int l0 = 0; (l0 < 2048); l0 = (l0 + 1)) {
			fRec7[l0] = 0.0f;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			iRec12[l1] = 0;
			
		}
		for (int l2 = 0; (l2 < 3); l2 = (l2 + 1)) {
			fRec11[l2] = 0.0f;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fVec0[l3] = 0.0f;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			iRec13[l4] = 0;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			iRec14[l5] = 0;
			
		}
		for (int l6 = 0; (l6 < 3); l6 = (l6 + 1)) {
			fRec16[l6] = 0.0f;
			
		}
		for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) {
			fRec15[l7] = 0.0f;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			iRec17[l8] = 0;
			
		}
		for (int l9 = 0; (l9 < 2048); l9 = (l9 + 1)) {
			fVec1[l9] = 0.0f;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec3[l10] = 0.0f;
			
		}
		for (int l11 = 0; (l11 < 3); l11 = (l11 + 1)) {
			fRec0[l11] = 0.0f;
			
		}
		for (int l12 = 0; (l12 < 3); l12 = (l12 + 1)) {
			fRec1[l12] = 0.0f;
			
		}
		
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual synth_ks* clone() {
		return new synth_ks();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
		
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("synth.ks");
		ui_interface->addHorizontalSlider("cutoff", &fHslider4, 1.0f, 0.100000001f, 1.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("gain", &fHslider3, 1.0f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->addButton("gate", &fButton0);
		ui_interface->addHorizontalSlider("mode", &fHslider2, 0.0f, 0.0f, 1.0f, 1.0f);
		ui_interface->addHorizontalSlider("mute", &fHslider0, 0.0f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("pitch", &fHslider1, 48.0f, 36.0f, 84.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("pos", &fHslider6, 0.699999988f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("sharp", &fHslider5, 0.25f, 0.00999999978f, 1.0f, 0.00100000005f);
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = (0.5f * ((0.200000003f * (1.0f - float(fHslider0))) + 0.800000012f));
		float fSlow1 = std::pow(2.0f, (0.0833333358f * (float(fHslider1) + -69.0f)));
		float fSlow2 = (fConst1 * ((0.772727251f / fSlow1) + -0.0500000007f));
		float fSlow3 = (fSlow2 + -1.49999499f);
		float fSlow4 = std::floor(fSlow3);
		float fSlow5 = (fSlow2 + (-1.0f - fSlow4));
		float fSlow6 = (fSlow2 + (-2.0f - fSlow4));
		float fSlow7 = (fSlow2 + (-3.0f - fSlow4));
		float fSlow8 = (fSlow2 + (-4.0f - fSlow4));
		float fSlow9 = ((((0.0f - fSlow5) * (0.0f - (0.5f * fSlow6))) * (0.0f - (0.333333343f * fSlow7))) * (0.0f - (0.25f * fSlow8)));
		int iSlow10 = int(fSlow3);
		int iSlow11 = (int(std::min<float>(fConst2, float(std::max<int>(0, iSlow10)))) + 1);
		float fSlow12 = (fSlow2 - fSlow4);
		float fSlow13 = (((0.0f - fSlow6) * (0.0f - (0.5f * fSlow7))) * (0.0f - (0.333333343f * fSlow8)));
		int iSlow14 = (int(std::min<float>(fConst2, float(std::max<int>(0, (iSlow10 + 1))))) + 1);
		float fSlow15 = (0.5f * ((fSlow5 * (0.0f - fSlow7)) * (0.0f - (0.5f * fSlow8))));
		int iSlow16 = (int(std::min<float>(fConst2, float(std::max<int>(0, (iSlow10 + 2))))) + 1);
		float fSlow17 = (fSlow5 * fSlow6);
		float fSlow18 = (0.166666672f * (fSlow17 * (0.0f - fSlow8)));
		int iSlow19 = (int(std::min<float>(fConst2, float(std::max<int>(0, (iSlow10 + 3))))) + 1);
		float fSlow20 = (0.0416666679f * (fSlow17 * fSlow7));
		int iSlow21 = (int(std::min<float>(fConst2, float(std::max<int>(0, (iSlow10 + 4))))) + 1);
		int iSlow22 = (int(float(fHslider2)) >= 1);
		float fSlow23 = float(fHslider3);
		float fSlow24 = std::tan((fConst3 * (fSlow1 * float(fHslider4))));
		float fSlow25 = (1.0f / fSlow24);
		float fSlow26 = (((fSlow25 + 1.41421354f) / fSlow24) + 1.0f);
		float fSlow27 = (fSlow23 / fSlow26);
		float fSlow28 = (1.0f / fSlow26);
		float fSlow29 = (((fSlow25 + -1.41421354f) / fSlow24) + 1.0f);
		float fSlow30 = (2.0f * (1.0f - (1.0f / synth_ks_faustpower2_f(fSlow24))));
		float fSlow31 = float(fHslider5);
		float fSlow32 = (1.0f / std::max<float>(1.0f, (fConst4 * (fSlow31 * synth_ks_faustpower2_f((1.0f - (0.219999999f * fSlow1)))))));
		float fSlow33 = float(fButton0);
		float fSlow34 = float(fHslider6);
		float fSlow35 = std::tan((fConst5 * ((15000.0f * fSlow34) + 500.0f)));
		float fSlow36 = (1.0f / fSlow35);
		float fSlow37 = (((fSlow36 + 1.41421354f) / fSlow35) + 1.0f);
		float fSlow38 = (fSlow23 / fSlow37);
		float fSlow39 = std::tan((fConst5 * ((500.0f * fSlow34) + 40.0f)));
		float fSlow40 = (1.0f / fSlow39);
		float fSlow41 = (1.0f / (((fSlow40 + 1.41421354f) / fSlow39) + 1.0f));
		float fSlow42 = synth_ks_faustpower2_f(fSlow39);
		float fSlow43 = (1.0f / fSlow42);
		float fSlow44 = (((fSlow40 + -1.41421354f) / fSlow39) + 1.0f);
		float fSlow45 = (2.0f * (1.0f - fSlow43));
		float fSlow46 = (0.0f - (2.0f / fSlow42));
		float fSlow47 = (1.0f / fSlow37);
		float fSlow48 = (((fSlow36 + -1.41421354f) / fSlow35) + 1.0f);
		float fSlow49 = (2.0f * (1.0f - (1.0f / synth_ks_faustpower2_f(fSlow35))));
		float fSlow50 = (1.0f / std::max<float>(1.0f, (fConst4 * fSlow31)));
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = (fSlow0 * (fRec1[1] + fRec1[2]));
			fRec7[(IOTA & 2047)] = fTemp0;
			float fRec8 = ((fSlow9 * fRec7[((IOTA - iSlow11) & 2047)]) + (fSlow12 * ((((fSlow13 * fRec7[((IOTA - iSlow14) & 2047)]) + (fSlow15 * fRec7[((IOTA - iSlow16) & 2047)])) + (fSlow18 * fRec7[((IOTA - iSlow19) & 2047)])) + (fSlow20 * fRec7[((IOTA - iSlow21) & 2047)]))));
			iRec12[0] = ((1103515245 * iRec12[1]) + 12345);
			float fTemp1 = (4.65661287e-10f * float(iRec12[0]));
			fRec11[0] = (fTemp1 - (fSlow28 * ((fSlow29 * fRec11[2]) + (fSlow30 * fRec11[1]))));
			fVec0[0] = fSlow33;
			iRec13[0] = (((fVec0[1] >= fSlow33) * iRec13[1]) + 1);
			float fTemp2 = float(iRec13[0]);
			float fTemp3 = std::min<float>((fSlow32 * fTemp2), 1.0f);
			iRec14[0] = ((iRec14[1] + 1) * (fTemp3 >= 1.0f));
			fRec16[0] = (fTemp1 - (fSlow41 * ((fSlow44 * fRec16[2]) + (fSlow45 * fRec16[1]))));
			fRec15[0] = ((fSlow41 * (((fSlow43 * fRec16[0]) + (fSlow46 * fRec16[1])) + (fSlow43 * fRec16[2]))) - (fSlow47 * ((fSlow48 * fRec15[2]) + (fSlow49 * fRec15[1]))));
			float fTemp4 = std::min<float>((fSlow50 * fTemp2), 1.0f);
			iRec17[0] = ((iRec17[1] + 1) * (fTemp4 >= 1.0f));
			float fTemp5 = (iSlow22 ? (fSlow38 * ((fRec15[2] + (fRec15[0] + (2.0f * fRec15[1]))) * std::max<float>(0.0f, (fTemp4 - (fSlow50 * float(iRec17[0])))))) : (fSlow27 * ((fRec11[2] + (fRec11[0] + (2.0f * fRec11[1]))) * std::max<float>(0.0f, (fTemp3 - (fSlow32 * float(iRec14[0])))))));
			float fTemp6 = (fRec0[2] + fTemp5);
			fVec1[(IOTA & 2047)] = fTemp6;
			float fTemp7 = (fSlow9 * fVec1[((IOTA - iSlow11) & 2047)]);
			float fTemp8 = (fSlow12 * ((((fSlow13 * fVec1[((IOTA - iSlow14) & 2047)]) + (fSlow15 * fVec1[((IOTA - iSlow16) & 2047)])) + (fSlow18 * fVec1[((IOTA - iSlow19) & 2047)])) + (fSlow20 * fVec1[((IOTA - iSlow21) & 2047)])));
			float fRec9 = (fTemp7 + fTemp8);
			float fRec10 = (fTemp8 + (fTemp0 + fTemp7));
			fRec3[0] = fRec8;
			float fRec4 = (fTemp5 + fRec3[1]);
			float fRec5 = fRec9;
			float fRec6 = fRec10;
			fRec0[0] = fRec4;
			fRec1[0] = fRec5;
			float fRec2 = fRec6;
			output0[i] = FAUSTFLOAT(fRec2);
			IOTA = (IOTA + 1);
			iRec12[1] = iRec12[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fVec0[1] = fVec0[0];
			iRec13[1] = iRec13[0];
			iRec14[1] = iRec14[0];
			fRec16[2] = fRec16[1];
			fRec16[1] = fRec16[0];
			fRec15[2] = fRec15[1];
			fRec15[1] = fRec15[0];
			iRec17[1] = iRec17[0];
			fRec3[1] = fRec3[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			
		}
		
	}

};
// clang-format on
#endif

    template <class T>
    struct _synth_ks_UI : public UI {
    static std::string name;
};

template <class T>
std::string _synth_ks_UI<T>::name(sym(synth_ks));

typedef _synth_ks_UI<synth_ks> synth_ks_UI;

class faust_synth_ks_tilde : public FaustExternal<synth_ks, synth_ks_UI> {
public:
    faust_synth_ks_tilde(const ceammc::PdArgs& args)
        : FaustExternal(args)
    {
    }
};

#endif
