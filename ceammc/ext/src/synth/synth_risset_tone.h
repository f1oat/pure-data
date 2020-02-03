/* ------------------------------------------------------------
author: "Oli Larkin (contact@olilarkin.co.uk)"
copyright: "Oliver Larkin"
name: "synth.risset_tone"
version: "0.1"
Code generated with Faust 2.22.1 (https://faust.grame.fr)
Compilation options: -lang cpp -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __synth_risset_tone_H__
#define  __synth_risset_tone_H__

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

struct UI : public UIReal<FAUSTFLOAT>
{

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
    for (int i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return std::atoi(argv[i+1]);
    return def;
}

static long lopt1(int argc, char* argv[], const char* longname, const char* shortname, long def)
{
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i-1], shortname) == 0 || strcmp(argv[i-1], longname) == 0) {
            return atoi(argv[i]);
        }
    }
    return def;
}

static const char* lopts(char* argv[], const char* name, const char* def)
{
    for (int i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return argv[i+1];
    return def;
}

static const char* lopts1(int argc, char* argv[], const char* longname, const char* shortname, const char* def)
{
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i-1], shortname) == 0 || strcmp(argv[i-1], longname) == 0) {
            return argv[i];
        }
    }
    return def;
}

static bool isopt(char* argv[], const char* name)
{
    for (int i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return true;
    return false;
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
struct synth_risset_tone : public dsp {
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

class synth_risset_toneSIG0 {
	
  private:
	
	int iRec0[2];
	
  public:
	
	int getNumInputssynth_risset_toneSIG0() {
		return 0;
	}
	int getNumOutputssynth_risset_toneSIG0() {
		return 1;
	}
	int getInputRatesynth_risset_toneSIG0(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	int getOutputRatesynth_risset_toneSIG0(int channel) {
		int rate;
		switch ((channel)) {
			case 0: {
				rate = 0;
				break;
			}
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	
	void instanceInitsynth_risset_toneSIG0(int sample_rate) {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			iRec0[l0] = 0;
		}
	}
	
	void fillsynth_risset_toneSIG0(int count, float* table) {
		for (int i = 0; (i < count); i = (i + 1)) {
			iRec0[0] = (iRec0[1] + 1);
			table[i] = std::sin((9.58738019e-05f * float((iRec0[0] + -1))));
			iRec0[1] = iRec0[0];
		}
	}

};

static synth_risset_toneSIG0* newsynth_risset_toneSIG0() { return (synth_risset_toneSIG0*)new synth_risset_toneSIG0(); }
static void deletesynth_risset_toneSIG0(synth_risset_toneSIG0* dsp) { delete dsp; }

class synth_risset_toneSIG1 {
	
  private:
	
	int iRec3[2];
	
  public:
	
	int getNumInputssynth_risset_toneSIG1() {
		return 0;
	}
	int getNumOutputssynth_risset_toneSIG1() {
		return 1;
	}
	int getInputRatesynth_risset_toneSIG1(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	int getOutputRatesynth_risset_toneSIG1(int channel) {
		int rate;
		switch ((channel)) {
			case 0: {
				rate = 0;
				break;
			}
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	
	void instanceInitsynth_risset_toneSIG1(int sample_rate) {
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			iRec3[l3] = 0;
		}
	}
	
	void fillsynth_risset_toneSIG1(int count, float* table) {
		for (int i = 0; (i < count); i = (i + 1)) {
			iRec3[0] = (iRec3[1] + 1);
			table[i] = std::exp((0.0f - (4.8283f * (1.0f - std::cos((9.58738019e-05f * (float((iRec3[0] + -1)) + -32768.0f)))))));
			iRec3[1] = iRec3[0];
		}
	}

};

static synth_risset_toneSIG1* newsynth_risset_toneSIG1() { return (synth_risset_toneSIG1*)new synth_risset_toneSIG1(); }
static void deletesynth_risset_toneSIG1(synth_risset_toneSIG1* dsp) { delete dsp; }

static float ftbl0synth_risset_toneSIG0[65536];
static float ftbl1synth_risset_toneSIG1[65537];

#ifndef FAUSTCLASS 
#define FAUSTCLASS synth_risset_tone
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class synth_risset_tone : public dsp {
	
 private:
	
	int fSampleRate;
	float fConst0;
	float fConst1;
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	float fConst2;
	FAUSTFLOAT fHslider2;
	float fRec2[2];
	float fRec1[2];
	float fRec4[2];
	float fRec5[2];
	float fRec6[2];
	float fRec7[2];
	float fRec8[2];
	float fRec9[2];
	float fRec10[2];
	float fRec11[2];
	float fRec12[2];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "Oli Larkin (contact@olilarkin.co.uk)");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("ceammc.lib/name", "Ceammc PureData misc utils");
		m->declare("ceammc.lib/version", "0.1.1");
		m->declare("copyright", "Oliver Larkin");
		m->declare("description", "Jean Claude Risset's endless glissando");
		m->declare("filename", "synth_risset_tone.dsp");
		m->declare("licence", "GPL");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "synth.risset_tone");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.0");
		m->declare("version", "0.1");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 1;
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch ((channel)) {
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
		synth_risset_toneSIG0* sig0 = newsynth_risset_toneSIG0();
		sig0->instanceInitsynth_risset_toneSIG0(sample_rate);
		sig0->fillsynth_risset_toneSIG0(65536, ftbl0synth_risset_toneSIG0);
		synth_risset_toneSIG1* sig1 = newsynth_risset_toneSIG1();
		sig1->instanceInitsynth_risset_toneSIG1(sample_rate);
		sig1->fillsynth_risset_toneSIG1(65537, ftbl1synth_risset_toneSIG1);
		deletesynth_risset_toneSIG0(sig0);
		deletesynth_risset_toneSIG1(sig1);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = (440.0f / fConst0);
		fConst2 = (1.0f / fConst0);
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(60.0f);
		fHslider1 = FAUSTFLOAT(120.0f);
		fHslider2 = FAUSTFLOAT(0.01f);
	}
	
	virtual void instanceClear() {
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec2[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec1[l2] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec4[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec5[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec6[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec7[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec8[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec9[l9] = 0.0f;
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec10[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec11[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec12[l12] = 0.0f;
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
	
	virtual synth_risset_tone* clone() {
		return new synth_risset_tone();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("synth.risset_tone");
		ui_interface->addHorizontalSlider("centroid", &fHslider0, 60.0f, 20.0f, 120.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("range", &fHslider1, 120.0f, 1.0f, 120.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("rate", &fHslider2, 0.00999999978f, -2.0f, 2.0f, 0.00100000005f);
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = float(fHslider0);
		float fSlow1 = (0.5f * float(fHslider1));
		float fSlow2 = (fConst2 * float(fHslider2));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec2[0] = (fSlow2 + (fRec2[1] - std::floor((fSlow2 + fRec2[1]))));
			float fTemp0 = std::fmod((fRec2[0] + 0.699999988f), 1.0f);
			float fTemp1 = (fRec1[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp0) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec1[0] = (fTemp1 - std::floor(fTemp1));
			float fTemp2 = (65536.0f * fTemp0);
			int iTemp3 = int(fTemp2);
			float fTemp4 = ftbl1synth_risset_toneSIG1[iTemp3];
			float fTemp5 = std::fmod((fRec2[0] + 0.600000024f), 1.0f);
			float fTemp6 = (fRec4[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp5) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec4[0] = (fTemp6 - std::floor(fTemp6));
			float fTemp7 = (65536.0f * fTemp5);
			int iTemp8 = int(fTemp7);
			float fTemp9 = ftbl1synth_risset_toneSIG1[iTemp8];
			float fTemp10 = std::fmod((fRec2[0] + 0.5f), 1.0f);
			float fTemp11 = (fRec5[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp10) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec5[0] = (fTemp11 - std::floor(fTemp11));
			float fTemp12 = (65536.0f * fTemp10);
			int iTemp13 = int(fTemp12);
			float fTemp14 = ftbl1synth_risset_toneSIG1[iTemp13];
			float fTemp15 = std::fmod((fRec2[0] + 0.400000006f), 1.0f);
			float fTemp16 = (fRec6[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp15) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec6[0] = (fTemp16 - std::floor(fTemp16));
			float fTemp17 = (65536.0f * fTemp15);
			int iTemp18 = int(fTemp17);
			float fTemp19 = ftbl1synth_risset_toneSIG1[iTemp18];
			float fTemp20 = std::fmod((fRec2[0] + 0.300000012f), 1.0f);
			float fTemp21 = (fRec7[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp20) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec7[0] = (fTemp21 - std::floor(fTemp21));
			float fTemp22 = (65536.0f * fTemp20);
			int iTemp23 = int(fTemp22);
			float fTemp24 = ftbl1synth_risset_toneSIG1[iTemp23];
			float fTemp25 = std::fmod((fRec2[0] + 0.200000003f), 1.0f);
			float fTemp26 = (fRec8[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp25) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec8[0] = (fTemp26 - std::floor(fTemp26));
			float fTemp27 = (65536.0f * fTemp25);
			int iTemp28 = int(fTemp27);
			float fTemp29 = ftbl1synth_risset_toneSIG1[iTemp28];
			float fTemp30 = std::fmod((fRec2[0] + 0.100000001f), 1.0f);
			float fTemp31 = (fRec9[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp30) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec9[0] = (fTemp31 - std::floor(fTemp31));
			float fTemp32 = (65536.0f * fTemp30);
			int iTemp33 = int(fTemp32);
			float fTemp34 = ftbl1synth_risset_toneSIG1[iTemp33];
			float fTemp35 = std::fmod(fRec2[0], 1.0f);
			float fTemp36 = (fRec10[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp35) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec10[0] = (fTemp36 - std::floor(fTemp36));
			float fTemp37 = (65536.0f * fTemp35);
			int iTemp38 = int(fTemp37);
			float fTemp39 = ftbl1synth_risset_toneSIG1[iTemp38];
			float fTemp40 = std::fmod((fRec2[0] + 0.800000012f), 1.0f);
			float fTemp41 = (fRec11[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp40) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec11[0] = (fTemp41 - std::floor(fTemp41));
			float fTemp42 = (65536.0f * fTemp40);
			int iTemp43 = int(fTemp42);
			float fTemp44 = ftbl1synth_risset_toneSIG1[iTemp43];
			float fTemp45 = std::fmod((fRec2[0] + 0.899999976f), 1.0f);
			float fTemp46 = (fRec12[1] + (fConst1 * std::pow(2.0f, (0.0833333358f * (std::min<float>(120.0f, std::max<float>(20.0f, (fSlow0 + ((fSlow1 * ((2.0f * fTemp45) + -1.0f)) + 7.0f)))) + -69.0f)))));
			fRec12[0] = (fTemp46 - std::floor(fTemp46));
			float fTemp47 = (65536.0f * fTemp45);
			int iTemp48 = int(fTemp47);
			float fTemp49 = ftbl1synth_risset_toneSIG1[iTemp48];
			output0[i] = FAUSTFLOAT((0.100000001f * ((ftbl0synth_risset_toneSIG0[int((65536.0f * fRec1[0]))] * (fTemp4 + ((fTemp2 - std::floor(fTemp2)) * (ftbl1synth_risset_toneSIG1[(iTemp3 + 1)] - fTemp4)))) + ((ftbl0synth_risset_toneSIG0[int((65536.0f * fRec4[0]))] * (fTemp9 + ((fTemp7 - std::floor(fTemp7)) * (ftbl1synth_risset_toneSIG1[(iTemp8 + 1)] - fTemp9)))) + ((ftbl0synth_risset_toneSIG0[int((65536.0f * fRec5[0]))] * (fTemp14 + ((fTemp12 - std::floor(fTemp12)) * (ftbl1synth_risset_toneSIG1[(iTemp13 + 1)] - fTemp14)))) + ((ftbl0synth_risset_toneSIG0[int((65536.0f * fRec6[0]))] * (fTemp19 + ((fTemp17 - std::floor(fTemp17)) * (ftbl1synth_risset_toneSIG1[(iTemp18 + 1)] - fTemp19)))) + ((ftbl0synth_risset_toneSIG0[int((65536.0f * fRec7[0]))] * (fTemp24 + ((fTemp22 - std::floor(fTemp22)) * (ftbl1synth_risset_toneSIG1[(iTemp23 + 1)] - fTemp24)))) + ((ftbl0synth_risset_toneSIG0[int((65536.0f * fRec8[0]))] * (fTemp29 + ((fTemp27 - std::floor(fTemp27)) * (ftbl1synth_risset_toneSIG1[(iTemp28 + 1)] - fTemp29)))) + ((ftbl0synth_risset_toneSIG0[int((65536.0f * fRec9[0]))] * (fTemp34 + ((fTemp32 - std::floor(fTemp32)) * (ftbl1synth_risset_toneSIG1[(iTemp33 + 1)] - fTemp34)))) + ((ftbl0synth_risset_toneSIG0[int((65536.0f * fRec10[0]))] * (fTemp39 + ((fTemp37 - std::floor(fTemp37)) * (ftbl1synth_risset_toneSIG1[(iTemp38 + 1)] - fTemp39)))) + ((ftbl0synth_risset_toneSIG0[int((65536.0f * fRec11[0]))] * (fTemp44 + ((fTemp42 - std::floor(fTemp42)) * (ftbl1synth_risset_toneSIG1[(iTemp43 + 1)] - fTemp44)))) + (ftbl0synth_risset_toneSIG0[int((65536.0f * fRec12[0]))] * (fTemp49 + ((fTemp47 - std::floor(fTemp47)) * (ftbl1synth_risset_toneSIG1[(iTemp48 + 1)] - fTemp49)))))))))))))));
			fRec2[1] = fRec2[0];
			fRec1[1] = fRec1[0];
			fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec12[1] = fRec12[0];
		}
	}

};
// clang-format on
#endif

    template <class T>
    struct _synth_risset_tone_UI : public UI {
    static std::string name;
};

template <class T>
std::string _synth_risset_tone_UI<T>::name(sym(synth_risset_tone));

typedef _synth_risset_tone_UI<synth_risset_tone> synth_risset_tone_UI;

class faust_synth_risset_tone_tilde : public FaustExternal<synth_risset_tone, synth_risset_tone_UI> {
public:
    faust_synth_risset_tone_tilde(const ceammc::PdArgs& args)
        : FaustExternal(args)
    {
    }
};

#endif
