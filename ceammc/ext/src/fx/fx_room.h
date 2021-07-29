/* ------------------------------------------------------------
author: "thedrgreenthumb"
license: "MIT"
name: "fx_room"
Code generated with Faust 2.30.12 (https://faust.grame.fr)
Compilation options: -lang cpp -es 1 -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __fx_room_H__
#define  __fx_room_H__

// FAUST Architecture File for ceammc::SoundExternal class
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

/************************** BEGIN fx_room_dsp.h **************************/
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

struct UI;
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

class fx_room_dsp {

    public:

        fx_room_dsp() {}
        virtual ~fx_room_dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'openTabBox', 'addButton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Return the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void init(int sample_rate) = 0;

        /**
         * Init instance state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceInit(int sample_rate) = 0;
    
        /**
         * Init instance constant state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceConstants(int sample_rate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (like delay lines...) but keep the control parameter values */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual fx_room_dsp* clone() = 0;
    
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

class decorator_dsp : public fx_room_dsp {

    protected:

        fx_room_dsp* fDSP;

    public:

        decorator_dsp(fx_room_dsp* fx_room_dsp = nullptr):fDSP(fx_room_dsp) {}
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
 * DSP factory class, used with LLVM and Interpreter backends
 * to create DSP instances from a compiled DSP program.
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
    
        virtual fx_room_dsp* createDSPInstance() = 0;
    
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
/**************************  END  fx_room_dsp.h **************************/
/************************** BEGIN UI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2020 GRAME, Centre National de Creation Musicale
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
struct UIReal
{
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

/**
 The base class of Meta handler to be used in fx_room_dsp::metadata(Meta* m) method to retrieve (key, value) metadata.
 */
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
struct fx_room : public fx_room_dsp {
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
#include <cstdint>
#include <math.h>

static float fx_room_faustpower2_f(float value) {
	return (value * value);
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS fx_room
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class fx_room : public fx_room_dsp {
	
 private:
	
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fHslider0;
	float fRec0[2];
	int IOTA;
	float fVec0[65536];
	FAUSTFLOAT fHslider1;
	int fSampleRate;
	float fConst4;
	float fConst7;
	float fConst9;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fHslider3;
	float fRec19[2];
	float fConst10;
	float fConst11;
	float fRec18[3];
	float fConst12;
	float fConst13;
	float fConst14;
	float fConst15;
	float fRec17[3];
	float fConst18;
	float fConst19;
	float fConst20;
	FAUSTFLOAT fHslider4;
	float fConst21;
	float fConst22;
	float fRec21[3];
	float fConst23;
	float fRec20[3];
	float fVec1[2048];
	int iConst24;
	float fRec15[2];
	float fVec2[4096];
	int iConst25;
	float fRec13[2];
	float fVec3[4096];
	int iConst26;
	int iConst27;
	float fVec4[16384];
	int iConst28;
	float fVec5[16384];
	int iConst29;
	float fRec11[2];
	float fRec9[2];
	float fVec6[8192];
	int iConst30;
	int iConst31;
	float fVec7[16384];
	int iConst32;
	float fVec8[16384];
	int iConst33;
	float fVec9[8192];
	int iConst34;
	float fRec7[2];
	float fRec5[2];
	float fRec3[2];
	float fRec1[2];
	float fRec38[3];
	float fRec37[3];
	float fRec36[2];
	float fConst37;
	float fConst38;
	float fConst39;
	float fConst40;
	float fRec40[3];
	float fConst41;
	float fRec39[3];
	float fVec10[8192];
	int iConst42;
	float fVec11[2048];
	int iConst43;
	float fVec12[8192];
	int iConst44;
	float fRec34[2];
	float fRec32[2];
	float fRec30[2];
	float fVec13[1024];
	int iConst45;
	float fVec14[8192];
	float fRec28[2];
	float fVec15[16384];
	int iConst46;
	int iConst47;
	float fVec16[8192];
	int iConst48;
	float fVec17[2048];
	int iConst49;
	float fRec26[2];
	float fRec24[2];
	float fRec22[2];
	float fConst52;
	float fConst55;
	float fConst57;
	float fConst58;
	float fConst59;
	float fRec55[3];
	float fConst60;
	float fConst61;
	float fConst62;
	float fRec54[3];
	float fRec53[2];
	float fVec18[8192];
	int iConst63;
	float fVec19[8192];
	float fVec20[8192];
	float fVec21[2048];
	float fRec51[2];
	float fRec49[2];
	float fRec47[2];
	float fVec22[16384];
	int iConst64;
	float fVec23[8192];
	float fRec45[2];
	float fRec43[2];
	float fRec41[2];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("author", "thedrgreenthumb");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("ceammc_ui.lib/name", "CEAMMC faust default UI elements");
		m->declare("ceammc_ui.lib/version", "0.1.2");
		m->declare("compile_options", "-lang cpp -es 1 -scal -ftz 0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("filename", "fx_room.dsp");
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
		m->declare("filters.lib/pole:author", "Julius O. Smith III");
		m->declare("filters.lib/pole:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/pole:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "0.3");
		m->declare("id", "fx.room");
		m->declare("license", "MIT");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.3");
		m->declare("name", "fx_room");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.1");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
	}

	virtual int getNumInputs() {
		return 1;
	}
	virtual int getNumOutputs() {
		return 1;
	}
	virtual int getInputRate(int channel) {
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
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		float fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		float fConst1 = std::tan((3141.59277f / fConst0));
		float fConst2 = (1.0f / fConst1);
		float fConst3 = (((fConst2 + 1.41421354f) / fConst1) + 1.0f);
		fConst4 = (1.29900002f / fConst3);
		float fConst5 = std::tan((1570.79639f / fConst0));
		float fConst6 = (1.0f / fConst5);
		fConst7 = (1.0f / (((fConst6 + 1.41421354f) / fConst5) + 1.0f));
		float fConst8 = fx_room_faustpower2_f(fConst5);
		fConst9 = (1.0f / fConst8);
		fConst10 = (((fConst6 + -1.41421354f) / fConst5) + 1.0f);
		fConst11 = (2.0f * (1.0f - fConst9));
		fConst12 = (0.0f - (2.0f / fConst8));
		fConst13 = (1.0f / fConst3);
		fConst14 = (((fConst2 + -1.41421354f) / fConst1) + 1.0f);
		fConst15 = (2.0f * (1.0f - (1.0f / fx_room_faustpower2_f(fConst1))));
		float fConst16 = std::tan((12566.3711f / fConst0));
		float fConst17 = (1.0f / fConst16);
		fConst18 = (1.0f / (((fConst17 + 0.765366852f) / fConst16) + 1.0f));
		fConst19 = (1.0f / (((fConst17 + 1.84775901f) / fConst16) + 1.0f));
		fConst20 = (0.00100000005f * fConst0);
		fConst21 = (((fConst17 + -1.84775901f) / fConst16) + 1.0f);
		fConst22 = (2.0f * (1.0f - (1.0f / fx_room_faustpower2_f(fConst16))));
		fConst23 = (((fConst17 + -0.765366852f) / fConst16) + 1.0f);
		iConst24 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.00800000038f * fConst0) + -1.0f))));
		iConst25 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.0120000001f * fConst0) + -1.0f))));
		iConst26 = int((0.00400000019f * fConst0));
		iConst27 = (iConst26 + int((0.0170000009f * fConst0)));
		iConst28 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.0869999975f * fConst0) + -1.0f))));
		iConst29 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.061999999f * fConst0) + -1.0f))));
		iConst30 = int((0.0309999995f * fConst0));
		iConst31 = (iConst30 + int((0.00300000003f * fConst0)));
		iConst32 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.119999997f * fConst0) + -1.0f))));
		iConst33 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.0759999976f * fConst0) + -1.0f))));
		iConst34 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.0299999993f * fConst0) + -1.0f))));
		float fConst35 = std::tan((18849.5566f / fConst0));
		float fConst36 = (1.0f / fConst35);
		fConst37 = (1.0f / (((fConst36 + 0.765366852f) / fConst35) + 1.0f));
		fConst38 = (1.0f / (((fConst36 + 1.84775901f) / fConst35) + 1.0f));
		fConst39 = (((fConst36 + -1.84775901f) / fConst35) + 1.0f);
		fConst40 = (2.0f * (1.0f - (1.0f / fx_room_faustpower2_f(fConst35))));
		fConst41 = (((fConst36 + -0.765366852f) / fConst35) + 1.0f);
		iConst42 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.0350000001f * fConst0) + -1.0f))));
		iConst43 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.00829999987f * fConst0) + -1.0f))));
		iConst44 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.0219999999f * fConst0) + -1.0f))));
		iConst45 = int((0.00499999989f * fConst0));
		iConst46 = int((0.0670000017f * fConst0));
		iConst47 = (iConst46 + int((0.0149999997f * fConst0)));
		iConst48 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.0390000008f * fConst0) + -1.0f))));
		iConst49 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.00980000012f * fConst0) + -1.0f))));
		float fConst50 = std::tan((5026.54834f / fConst0));
		float fConst51 = (1.0f / fConst50);
		fConst52 = (1.0f / (((fConst51 + 1.41421354f) / fConst50) + 1.0f));
		float fConst53 = std::tan((2513.27417f / fConst0));
		float fConst54 = (1.0f / fConst53);
		fConst55 = (1.0f / (((fConst54 + 1.41421354f) / fConst53) + 1.0f));
		float fConst56 = fx_room_faustpower2_f(fConst53);
		fConst57 = (1.0f / fConst56);
		fConst58 = (((fConst54 + -1.41421354f) / fConst53) + 1.0f);
		fConst59 = (2.0f * (1.0f - fConst57));
		fConst60 = (0.0f - (2.0f / fConst56));
		fConst61 = (((fConst51 + -1.41421354f) / fConst50) + 1.0f);
		fConst62 = (2.0f * (1.0f - (1.0f / fx_room_faustpower2_f(fConst50))));
		iConst63 = int((0.0240000002f * fConst0));
		iConst64 = int(std::min<float>(8192.0f, std::max<float>(0.0f, ((0.0659999996f * fConst0) + -1.0f))));
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0f);
		fHslider0 = FAUSTFLOAT(0.5f);
		fHslider1 = FAUSTFLOAT(1.0f);
		fHslider2 = FAUSTFLOAT(1.0f);
		fHslider3 = FAUSTFLOAT(0.29999999999999999f);
		fHslider4 = FAUSTFLOAT(20.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec0[l0] = 0.0f;
		}
		IOTA = 0;
		for (int l1 = 0; (l1 < 65536); l1 = (l1 + 1)) {
			fVec0[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec19[l2] = 0.0f;
		}
		for (int l3 = 0; (l3 < 3); l3 = (l3 + 1)) {
			fRec18[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 3); l4 = (l4 + 1)) {
			fRec17[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 3); l5 = (l5 + 1)) {
			fRec21[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 3); l6 = (l6 + 1)) {
			fRec20[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 2048); l7 = (l7 + 1)) {
			fVec1[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec15[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 4096); l9 = (l9 + 1)) {
			fVec2[l9] = 0.0f;
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec13[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 4096); l11 = (l11 + 1)) {
			fVec3[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 16384); l12 = (l12 + 1)) {
			fVec4[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 16384); l13 = (l13 + 1)) {
			fVec5[l13] = 0.0f;
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec11[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec9[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 8192); l16 = (l16 + 1)) {
			fVec6[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 16384); l17 = (l17 + 1)) {
			fVec7[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 16384); l18 = (l18 + 1)) {
			fVec8[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 8192); l19 = (l19 + 1)) {
			fVec9[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec7[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec5[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec3[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec1[l23] = 0.0f;
		}
		for (int l24 = 0; (l24 < 3); l24 = (l24 + 1)) {
			fRec38[l24] = 0.0f;
		}
		for (int l25 = 0; (l25 < 3); l25 = (l25 + 1)) {
			fRec37[l25] = 0.0f;
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec36[l26] = 0.0f;
		}
		for (int l27 = 0; (l27 < 3); l27 = (l27 + 1)) {
			fRec40[l27] = 0.0f;
		}
		for (int l28 = 0; (l28 < 3); l28 = (l28 + 1)) {
			fRec39[l28] = 0.0f;
		}
		for (int l29 = 0; (l29 < 8192); l29 = (l29 + 1)) {
			fVec10[l29] = 0.0f;
		}
		for (int l30 = 0; (l30 < 2048); l30 = (l30 + 1)) {
			fVec11[l30] = 0.0f;
		}
		for (int l31 = 0; (l31 < 8192); l31 = (l31 + 1)) {
			fVec12[l31] = 0.0f;
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec34[l32] = 0.0f;
		}
		for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
			fRec32[l33] = 0.0f;
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec30[l34] = 0.0f;
		}
		for (int l35 = 0; (l35 < 1024); l35 = (l35 + 1)) {
			fVec13[l35] = 0.0f;
		}
		for (int l36 = 0; (l36 < 8192); l36 = (l36 + 1)) {
			fVec14[l36] = 0.0f;
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec28[l37] = 0.0f;
		}
		for (int l38 = 0; (l38 < 16384); l38 = (l38 + 1)) {
			fVec15[l38] = 0.0f;
		}
		for (int l39 = 0; (l39 < 8192); l39 = (l39 + 1)) {
			fVec16[l39] = 0.0f;
		}
		for (int l40 = 0; (l40 < 2048); l40 = (l40 + 1)) {
			fVec17[l40] = 0.0f;
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec26[l41] = 0.0f;
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec24[l42] = 0.0f;
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec22[l43] = 0.0f;
		}
		for (int l44 = 0; (l44 < 3); l44 = (l44 + 1)) {
			fRec55[l44] = 0.0f;
		}
		for (int l45 = 0; (l45 < 3); l45 = (l45 + 1)) {
			fRec54[l45] = 0.0f;
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fRec53[l46] = 0.0f;
		}
		for (int l47 = 0; (l47 < 8192); l47 = (l47 + 1)) {
			fVec18[l47] = 0.0f;
		}
		for (int l48 = 0; (l48 < 8192); l48 = (l48 + 1)) {
			fVec19[l48] = 0.0f;
		}
		for (int l49 = 0; (l49 < 8192); l49 = (l49 + 1)) {
			fVec20[l49] = 0.0f;
		}
		for (int l50 = 0; (l50 < 2048); l50 = (l50 + 1)) {
			fVec21[l50] = 0.0f;
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			fRec51[l51] = 0.0f;
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec49[l52] = 0.0f;
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec47[l53] = 0.0f;
		}
		for (int l54 = 0; (l54 < 16384); l54 = (l54 + 1)) {
			fVec22[l54] = 0.0f;
		}
		for (int l55 = 0; (l55 < 8192); l55 = (l55 + 1)) {
			fVec23[l55] = 0.0f;
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fRec45[l56] = 0.0f;
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec43[l57] = 0.0f;
		}
		for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
			fRec41[l58] = 0.0f;
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
	
	virtual fx_room* clone() {
		return new fx_room();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("fx_room");
		ui_interface->addCheckButton("bypass", &fCheckbox0);
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->addHorizontalSlider("drywet", &fHslider0, 0.5f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("fb", &fHslider3, 0.300000012f, 0.0f, 1.0f, 0.0500000007f);
		ui_interface->addHorizontalSlider("hfdamp", &fHslider2, 1.0f, 0.0f, 1.0f, 0.0500000007f);
		ui_interface->declare(&fHslider4, "unit", "ms");
		ui_interface->addHorizontalSlider("predelay", &fHslider4, 20.0f, 1.0f, 200.0f, 1.0f);
		ui_interface->addHorizontalSlider("size", &fHslider1, 1.0f, 0.0f, 3.0f, 0.0500000007f);
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		int iSlow0 = int(float(fCheckbox0));
		float fSlow1 = (0.00100000005f * float(fHslider0));
		float fSlow2 = float(fHslider1);
		float fSlow3 = std::floor(fSlow2);
		float fSlow4 = (fSlow3 + (1.0f - fSlow2));
		int iSlow5 = int(fSlow2);
		int iSlow6 = (iSlow5 == 0);
		int iSlow7 = (iSlow5 == 1);
		float fSlow8 = (1.0f - float(fHslider2));
		float fSlow9 = float(fHslider3);
		float fSlow10 = (0.5f * fSlow9);
		float fSlow11 = (fConst20 * float(fHslider4));
		float fSlow12 = std::floor(fSlow11);
		float fSlow13 = (fSlow12 + (1.0f - fSlow11));
		int iSlow14 = int(fSlow11);
		int iSlow15 = std::min<int>(65537, std::max<int>(0, iSlow14));
		float fSlow16 = (fSlow11 - fSlow12);
		int iSlow17 = std::min<int>(65537, std::max<int>(0, (iSlow14 + 1)));
		float fSlow18 = (0.400000006f * fSlow9);
		float fSlow19 = (1.29900002f * fSlow9);
		float fSlow20 = (fConst52 * fSlow9);
		float fSlow21 = (fSlow2 - fSlow3);
		int iSlow22 = int((fSlow2 + 1.0f));
		int iSlow23 = (iSlow22 == 0);
		int iSlow24 = (iSlow22 == 1);
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec0[0] = (fSlow1 + (0.999000013f * fRec0[1]));
			float fTemp0 = float(input0[i]);
			float fTemp1 = (iSlow0 ? 0.0f : fTemp0);
			fVec0[(IOTA & 65535)] = fTemp1;
			fRec19[0] = ((fSlow8 * fRec19[1]) + (fSlow10 * fRec1[1]));
			fRec18[0] = (fRec19[0] - (fConst7 * ((fConst10 * fRec18[2]) + (fConst11 * fRec18[1]))));
			fRec17[0] = ((fConst7 * (((fConst9 * fRec18[0]) + (fConst12 * fRec18[1])) + (fConst9 * fRec18[2]))) - (fConst13 * ((fConst14 * fRec17[2]) + (fConst15 * fRec17[1]))));
			float fTemp2 = ((fSlow13 * fVec0[((IOTA - iSlow15) & 65535)]) + (fSlow16 * fVec0[((IOTA - iSlow17) & 65535)]));
			fRec21[0] = (fTemp2 - (fConst19 * ((fConst21 * fRec21[2]) + (fConst22 * fRec21[1]))));
			fRec20[0] = ((fConst19 * (fRec21[2] + (fRec21[0] + (2.0f * fRec21[1])))) - (fConst18 * ((fConst23 * fRec20[2]) + (fConst22 * fRec20[1]))));
			float fTemp3 = (((fConst4 * (fRec17[2] + (fRec17[0] + (2.0f * fRec17[1])))) + (fConst18 * (fRec20[2] + (fRec20[0] + (2.0f * fRec20[1]))))) - (0.300000012f * fRec15[1]));
			fVec1[(IOTA & 2047)] = fTemp3;
			fRec15[0] = fVec1[((IOTA - iConst24) & 2047)];
			float fRec16 = (0.300000012f * fTemp3);
			float fTemp4 = ((fRec16 + fRec15[1]) - (0.300000012f * fRec13[1]));
			fVec2[(IOTA & 4095)] = fTemp4;
			fRec13[0] = fVec2[((IOTA - iConst25) & 4095)];
			float fRec14 = (0.300000012f * fTemp4);
			float fTemp5 = (fRec14 + fRec13[1]);
			fVec3[(IOTA & 4095)] = fTemp5;
			float fTemp6 = (fVec3[((IOTA - iConst27) & 4095)] - (0.5f * fRec9[1]));
			fVec4[(IOTA & 16383)] = fTemp6;
			float fTemp7 = (fVec4[((IOTA - iConst28) & 16383)] - (0.25f * fRec11[1]));
			fVec5[(IOTA & 16383)] = fTemp7;
			fRec11[0] = fVec5[((IOTA - iConst29) & 16383)];
			float fRec12 = (0.25f * fTemp7);
			fRec9[0] = (fRec12 + fRec11[1]);
			float fRec10 = (0.5f * fTemp6);
			float fTemp8 = (fRec10 + fRec9[1]);
			fVec6[(IOTA & 8191)] = fTemp8;
			float fTemp9 = (fVec6[((IOTA - iConst31) & 8191)] - (0.5f * fRec3[1]));
			fVec7[(IOTA & 16383)] = fTemp9;
			float fTemp10 = (fVec7[((IOTA - iConst32) & 16383)] - (0.25f * fRec5[1]));
			fVec8[(IOTA & 16383)] = fTemp10;
			float fTemp11 = (fVec8[((IOTA - iConst33) & 16383)] - (0.25f * fRec7[1]));
			fVec9[(IOTA & 8191)] = fTemp11;
			fRec7[0] = fVec9[((IOTA - iConst34) & 8191)];
			float fRec8 = (0.25f * fTemp11);
			fRec5[0] = (fRec8 + fRec7[1]);
			float fRec6 = (0.25f * fTemp10);
			fRec3[0] = (fRec6 + fRec5[1]);
			float fRec4 = (0.5f * fTemp9);
			fRec1[0] = (fRec4 + fRec3[1]);
			float fRec2 = ((0.800000012f * fVec6[((IOTA - iConst30) & 8191)]) + (1.5f * fVec3[((IOTA - iConst26) & 4095)]));
			float fTemp12 = (fRec1[0] + (0.800000012f * fRec2));
			fRec38[0] = (fRec22[1] - (fConst7 * ((fConst10 * fRec38[2]) + (fConst11 * fRec38[1]))));
			fRec37[0] = ((fConst7 * (((fConst9 * fRec38[0]) + (fConst12 * fRec38[1])) + (fConst9 * fRec38[2]))) - (fConst13 * ((fConst14 * fRec37[2]) + (fConst15 * fRec37[1]))));
			fRec36[0] = ((fSlow8 * fRec36[1]) + (fConst13 * (fRec37[2] + (fRec37[0] + (2.0f * fRec37[1])))));
			fRec40[0] = (fTemp2 - (fConst38 * ((fConst39 * fRec40[2]) + (fConst40 * fRec40[1]))));
			fRec39[0] = ((fConst38 * (fRec40[2] + (fRec40[0] + (2.0f * fRec40[1])))) - (fConst37 * ((fConst41 * fRec39[2]) + (fConst40 * fRec39[1]))));
			float fTemp13 = (fConst37 * (fRec39[2] + (fRec39[0] + (2.0f * fRec39[1]))));
			float fTemp14 = (((fSlow19 * fRec36[0]) + fTemp13) - (0.25f * fRec30[1]));
			fVec10[(IOTA & 8191)] = fTemp14;
			float fTemp15 = (fVec10[((IOTA - iConst42) & 8191)] - (0.349999994f * fRec32[1]));
			fVec11[(IOTA & 2047)] = fTemp15;
			float fTemp16 = (fVec11[((IOTA - iConst43) & 2047)] - (0.449999988f * fRec34[1]));
			fVec12[(IOTA & 8191)] = fTemp16;
			fRec34[0] = fVec12[((IOTA - iConst44) & 8191)];
			float fRec35 = (0.449999988f * fTemp16);
			fRec32[0] = (fRec35 + fRec34[1]);
			float fRec33 = (0.349999994f * fTemp15);
			fRec30[0] = (fRec33 + fRec32[1]);
			float fRec31 = (0.25f * fTemp14);
			float fTemp17 = (fRec31 + fRec30[1]);
			fVec13[(IOTA & 1023)] = fTemp17;
			float fTemp18 = (fVec13[((IOTA - iConst45) & 1023)] - (0.449999988f * fRec28[1]));
			fVec14[(IOTA & 8191)] = fTemp18;
			fRec28[0] = fVec14[((IOTA - iConst34) & 8191)];
			float fRec29 = (0.449999988f * fTemp18);
			float fTemp19 = (fRec29 + fRec28[1]);
			fVec15[(IOTA & 16383)] = fTemp19;
			float fTemp20 = ((fTemp2 + (fSlow18 * fVec15[((IOTA - iConst47) & 16383)])) - (0.25f * fRec24[1]));
			fVec16[(IOTA & 8191)] = fTemp20;
			float fTemp21 = (fVec16[((IOTA - iConst48) & 8191)] - (0.349999994f * fRec26[1]));
			fVec17[(IOTA & 2047)] = fTemp21;
			fRec26[0] = fVec17[((IOTA - iConst49) & 2047)];
			float fRec27 = (0.349999994f * fTemp21);
			fRec24[0] = (fRec27 + fRec26[1]);
			float fRec25 = (0.25f * fTemp20);
			fRec22[0] = (fRec25 + fRec24[1]);
			float fRec23 = (0.5f * (fTemp17 + fVec15[((IOTA - iConst46) & 16383)]));
			float fTemp22 = (fRec23 + (0.5f * fRec22[0]));
			fRec55[0] = ((0.99000001f * fRec41[1]) - (fConst55 * ((fConst58 * fRec55[2]) + (fConst59 * fRec55[1]))));
			fRec54[0] = ((fConst55 * (((fConst57 * fRec55[0]) + (fConst60 * fRec55[1])) + (fConst57 * fRec55[2]))) - (fConst52 * ((fConst61 * fRec54[2]) + (fConst62 * fRec54[1]))));
			fRec53[0] = ((fSlow8 * fRec53[1]) + (fSlow20 * (fRec54[2] + (fRec54[0] + (2.0f * fRec54[1])))));
			fVec18[(IOTA & 8191)] = (fRec53[0] + fTemp13);
			float fTemp23 = (fVec18[((IOTA - iConst63) & 8191)] - (0.150000006f * fRec47[1]));
			fVec19[(IOTA & 8191)] = fTemp23;
			float fTemp24 = (fVec19[((IOTA - iConst42) & 8191)] - (0.25f * fRec49[1]));
			fVec20[(IOTA & 8191)] = fTemp24;
			float fTemp25 = (fVec20[((IOTA - iConst44) & 8191)] - (0.300000012f * fRec51[1]));
			fVec21[(IOTA & 2047)] = fTemp25;
			fRec51[0] = fVec21[((IOTA - iConst43) & 2047)];
			float fRec52 = (0.300000012f * fTemp25);
			fRec49[0] = (fRec52 + fRec51[1]);
			float fRec50 = (0.25f * fTemp24);
			fRec47[0] = (fRec50 + fRec49[1]);
			float fRec48 = (0.150000006f * fTemp23);
			float fTemp26 = (fRec48 + fRec47[1]);
			float fTemp27 = (fTemp26 - (0.0799999982f * fRec43[1]));
			fVec22[(IOTA & 16383)] = fTemp27;
			float fTemp28 = (fVec22[((IOTA - iConst64) & 16383)] - (0.300000012f * fRec45[1]));
			fVec23[(IOTA & 8191)] = fTemp28;
			fRec45[0] = fVec23[((IOTA - iConst34) & 8191)];
			float fRec46 = (0.300000012f * fTemp28);
			fRec43[0] = (fRec46 + fRec45[1]);
			float fRec44 = (0.0799999982f * fTemp27);
			fRec41[0] = (fRec44 + fRec43[1]);
			float fRec42 = fTemp26;
			float fTemp29 = (0.5f * (fRec41[0] + fRec42));
			output0[i] = FAUSTFLOAT((iSlow0 ? fTemp0 : (((1.0f - fRec0[0]) * fTemp1) + (fRec0[0] * ((fSlow4 * (iSlow6 ? fTemp29 : (iSlow7 ? fTemp22 : fTemp12))) + (fSlow21 * (iSlow23 ? fTemp29 : (iSlow24 ? fTemp22 : fTemp12))))))));
			fRec0[1] = fRec0[0];
			IOTA = (IOTA + 1);
			fRec19[1] = fRec19[0];
			fRec18[2] = fRec18[1];
			fRec18[1] = fRec18[0];
			fRec17[2] = fRec17[1];
			fRec17[1] = fRec17[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fRec20[2] = fRec20[1];
			fRec20[1] = fRec20[0];
			fRec15[1] = fRec15[0];
			fRec13[1] = fRec13[0];
			fRec11[1] = fRec11[0];
			fRec9[1] = fRec9[0];
			fRec7[1] = fRec7[0];
			fRec5[1] = fRec5[0];
			fRec3[1] = fRec3[0];
			fRec1[1] = fRec1[0];
			fRec38[2] = fRec38[1];
			fRec38[1] = fRec38[0];
			fRec37[2] = fRec37[1];
			fRec37[1] = fRec37[0];
			fRec36[1] = fRec36[0];
			fRec40[2] = fRec40[1];
			fRec40[1] = fRec40[0];
			fRec39[2] = fRec39[1];
			fRec39[1] = fRec39[0];
			fRec34[1] = fRec34[0];
			fRec32[1] = fRec32[0];
			fRec30[1] = fRec30[0];
			fRec28[1] = fRec28[0];
			fRec26[1] = fRec26[0];
			fRec24[1] = fRec24[0];
			fRec22[1] = fRec22[0];
			fRec55[2] = fRec55[1];
			fRec55[1] = fRec55[0];
			fRec54[2] = fRec54[1];
			fRec54[1] = fRec54[0];
			fRec53[1] = fRec53[0];
			fRec51[1] = fRec51[0];
			fRec49[1] = fRec49[0];
			fRec47[1] = fRec47[0];
			fRec45[1] = fRec45[0];
			fRec43[1] = fRec43[0];
			fRec41[1] = fRec41[0];
		}
	}

};
// clang-format on
#endif

    template <class T>
    struct _fx_room_UI : public UI {
    static std::string name;
};

template <class T>
std::string _fx_room_UI<T>::name(sym(fx_room));

typedef _fx_room_UI<fx_room> fx_room_UI;

class faust_fx_room_tilde : public FaustExternal<fx_room, fx_room_UI> {
public:
    faust_fx_room_tilde(const ceammc::PdArgs& args)
        : FaustExternal(args)
    {
    }
};

#endif