/* ------------------------------------------------------------
author: "brummer"
copyright: "(c)brummer 2008"
license: "BSD"
name: "fx.distortion1"
version: "0.01"
Code generated with Faust 2.17.3 (https://faust.grame.fr)
Compilation options: cpp, -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __fx_distortion1_H__
#define  __fx_distortion1_H__

// FAUST Architecture File for ceammc::SoundExternal class
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

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

        decorator_dsp(dsp* dsp = 0):fDSP(dsp) {}
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

#endif


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
struct fx_distortion1 : public dsp {
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

static float fx_distortion1_faustpower2_f(float value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS fx_distortion1
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class fx_distortion1 : public dsp {
	
 private:
	
	FAUSTFLOAT fCheckbox0;
	int fSampleRate;
	float fConst0;
	float fConst1;
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fVslider1;
	int iVec0[2];
	float fRec4[2];
	float fConst2;
	float fConst3;
	float fConst4;
	float fConst5;
	float fConst6;
	float fConst7;
	float fConst8;
	float fConst9;
	FAUSTFLOAT fVslider2;
	float fRec6[2];
	FAUSTFLOAT fVslider3;
	float fRec7[2];
	FAUSTFLOAT fVslider4;
	FAUSTFLOAT fVslider5;
	FAUSTFLOAT fVslider6;
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fCheckbox1;
	FAUSTFLOAT fEntry1;
	FAUSTFLOAT fEntry2;
	FAUSTFLOAT fCheckbox2;
	FAUSTFLOAT fVslider7;
	float fVec1[2];
	FAUSTFLOAT fVslider8;
	float fRec17[2];
	float fVec2[2];
	float fRec16[2];
	float fRec15[2];
	float fVec3[2];
	float fRec14[2];
	float fVec4[2];
	float fRec13[2];
	float fRec12[3];
	float fRec11[3];
	float fConst10;
	float fConst11;
	float fRec10[3];
	float fConst12;
	float fConst13;
	float fVec5[2];
	float fRec9[2];
	float fRec8[3];
	FAUSTFLOAT fVslider9;
	float fRec18[2];
	FAUSTFLOAT fVslider10;
	FAUSTFLOAT fEntry3;
	float fRec22[2];
	float fRec21[3];
	float fVec6[2];
	float fRec20[2];
	float fRec19[3];
	FAUSTFLOAT fVslider11;
	float fRec23[2];
	FAUSTFLOAT fVslider12;
	FAUSTFLOAT fEntry4;
	float fRec27[2];
	float fRec26[3];
	float fVec7[2];
	float fRec25[2];
	float fRec24[3];
	FAUSTFLOAT fVslider13;
	float fRec28[2];
	FAUSTFLOAT fVslider14;
	float fRec30[2];
	float fRec29[3];
	float fRec5[3];
	float fVec8[2];
	float fRec3[2];
	float fVec9[2];
	float fRec2[2];
	float fRec1[3];
	float fRec0[3];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("HighShelf.dsp/id", "HighShelf");
		m->declare("author", "brummer");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("copyright", "(c)brummer 2008");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("filename", "fx_distortion1");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("license", "BSD");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("maxmsp.lib/author", "GRAME");
		m->declare("maxmsp.lib/copyright", "GRAME");
		m->declare("maxmsp.lib/license", "LGPL");
		m->declare("maxmsp.lib/name", "MaxMSP compatibility Library");
		m->declare("maxmsp.lib/version", "1.1");
		m->declare("misceffects.lib/name", "Faust Math Library");
		m->declare("misceffects.lib/version", "2.0");
		m->declare("name", "fx.distortion1");
		m->declare("version", "0.01");
	}

	virtual int getNumInputs() {
		return 1;
		
	}
	virtual int getNumOutputs() {
		return 1;
		
	}
	virtual int getInputRate(int channel) {
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
		fConst1 = (3.14159274f / fConst0);
		fConst2 = (6.28318548f * (std::max<float>(0.0f, ((0.5f * fConst0) + -100.0f)) / fConst0));
		fConst3 = std::cos(fConst2);
		fConst4 = (0.683772206f * fConst3);
		fConst5 = (0.00562341325f * std::sin(fConst2));
		fConst6 = (fConst4 + fConst5);
		fConst7 = (1.0f / (fConst6 + 1.31622779f));
		fConst8 = (1.31622779f * fConst3);
		fConst9 = (0.0f - (0.632455528f * (fConst8 + -0.683772206f)));
		fConst10 = (fConst4 + (1.31622779f - fConst5));
		fConst11 = (2.0f * (-0.683772206f - fConst8));
		fConst12 = (0.316227764f * (fConst5 + (1.31622779f - fConst4)));
		fConst13 = (0.316227764f * (1.31622779f - fConst6));
		
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0f);
		fVslider0 = FAUSTFLOAT(5000.0f);
		fVslider1 = FAUSTFLOAT(130.0f);
		fVslider2 = FAUSTFLOAT(2.0f);
		fVslider3 = FAUSTFLOAT(10.0f);
		fVslider4 = FAUSTFLOAT(0.01f);
		fVslider5 = FAUSTFLOAT(1.0f);
		fVslider6 = FAUSTFLOAT(0.64000000000000001f);
		fEntry0 = FAUSTFLOAT(1250.0f);
		fCheckbox1 = FAUSTFLOAT(0.0f);
		fEntry1 = FAUSTFLOAT(130.0f);
		fEntry2 = FAUSTFLOAT(5000.0f);
		fCheckbox2 = FAUSTFLOAT(0.0f);
		fVslider7 = FAUSTFLOAT(0.12f);
		fVslider8 = FAUSTFLOAT(1.0f);
		fVslider9 = FAUSTFLOAT(10.0f);
		fVslider10 = FAUSTFLOAT(1.0f);
		fEntry3 = FAUSTFLOAT(650.0f);
		fVslider11 = FAUSTFLOAT(10.0f);
		fVslider12 = FAUSTFLOAT(1.0f);
		fEntry4 = FAUSTFLOAT(250.0f);
		fVslider13 = FAUSTFLOAT(10.0f);
		fVslider14 = FAUSTFLOAT(1.0f);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			iVec0[l0] = 0;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec4[l1] = 0.0f;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec6[l2] = 0.0f;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec7[l3] = 0.0f;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fVec1[l4] = 0.0f;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec17[l5] = 0.0f;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fVec2[l6] = 0.0f;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec16[l7] = 0.0f;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec15[l8] = 0.0f;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fVec3[l9] = 0.0f;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec14[l10] = 0.0f;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fVec4[l11] = 0.0f;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec13[l12] = 0.0f;
			
		}
		for (int l13 = 0; (l13 < 3); l13 = (l13 + 1)) {
			fRec12[l13] = 0.0f;
			
		}
		for (int l14 = 0; (l14 < 3); l14 = (l14 + 1)) {
			fRec11[l14] = 0.0f;
			
		}
		for (int l15 = 0; (l15 < 3); l15 = (l15 + 1)) {
			fRec10[l15] = 0.0f;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fVec5[l16] = 0.0f;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec9[l17] = 0.0f;
			
		}
		for (int l18 = 0; (l18 < 3); l18 = (l18 + 1)) {
			fRec8[l18] = 0.0f;
			
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec18[l19] = 0.0f;
			
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec22[l20] = 0.0f;
			
		}
		for (int l21 = 0; (l21 < 3); l21 = (l21 + 1)) {
			fRec21[l21] = 0.0f;
			
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fVec6[l22] = 0.0f;
			
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fRec20[l23] = 0.0f;
			
		}
		for (int l24 = 0; (l24 < 3); l24 = (l24 + 1)) {
			fRec19[l24] = 0.0f;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec23[l25] = 0.0f;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec27[l26] = 0.0f;
			
		}
		for (int l27 = 0; (l27 < 3); l27 = (l27 + 1)) {
			fRec26[l27] = 0.0f;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fVec7[l28] = 0.0f;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec25[l29] = 0.0f;
			
		}
		for (int l30 = 0; (l30 < 3); l30 = (l30 + 1)) {
			fRec24[l30] = 0.0f;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec28[l31] = 0.0f;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec30[l32] = 0.0f;
			
		}
		for (int l33 = 0; (l33 < 3); l33 = (l33 + 1)) {
			fRec29[l33] = 0.0f;
			
		}
		for (int l34 = 0; (l34 < 3); l34 = (l34 + 1)) {
			fRec5[l34] = 0.0f;
			
		}
		for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
			fVec8[l35] = 0.0f;
			
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec3[l36] = 0.0f;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fVec9[l37] = 0.0f;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec2[l38] = 0.0f;
			
		}
		for (int l39 = 0; (l39 < 3); l39 = (l39 + 1)) {
			fRec1[l39] = 0.0f;
			
		}
		for (int l40 = 0; (l40 < 3); l40 = (l40 + 1)) {
			fRec0[l40] = 0.0f;
			
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
	
	virtual fx_distortion1* clone() {
		return new fx_distortion1();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
		
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("fx.distortion1");
		ui_interface->addVerticalSlider("drive", &fVslider6, 0.639999986f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->declare(&fVslider2, "unit", "db");
		ui_interface->addVerticalSlider("gain", &fVslider2, 2.0f, -10.0f, 10.0f, 0.100000001f);
		ui_interface->addVerticalSlider("high_drive", &fVslider5, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->declare(&fVslider3, "unit", "db");
		ui_interface->addVerticalSlider("high_gain", &fVslider3, 10.0f, -10.0f, 20.0f, 0.100000001f);
		ui_interface->addVerticalSlider("level", &fVslider4, 0.00999999978f, 0.0f, 0.5f, 0.00999999978f);
		ui_interface->addVerticalSlider("low_drive", &fVslider14, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->declare(&fVslider13, "unit", "db");
		ui_interface->addVerticalSlider("low_gain", &fVslider13, 10.0f, -10.0f, 20.0f, 0.100000001f);
		ui_interface->openHorizontalBox("low_highcutoff");
		ui_interface->declare(&fVslider0, "unit", "Hz");
		ui_interface->addVerticalSlider("sp_high_freq", &fVslider0, 5000.0f, 1000.0f, 12000.0f, 10.0f);
		ui_interface->declare(&fVslider1, "unit", "Hz");
		ui_interface->addVerticalSlider("sp_low_freq", &fVslider1, 130.0f, 20.0f, 1000.0f, 10.0f);
		ui_interface->addCheckButton("sp_on_off", &fCheckbox0);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("low_highpass");
		ui_interface->addCheckButton("flt_on_off", &fCheckbox1);
		ui_interface->declare(&fEntry1, "unit", "Hz");
		ui_interface->addNumEntry("high_freq", &fEntry1, 130.0f, 20.0f, 7040.0f, 10.0f);
		ui_interface->declare(&fEntry2, "unit", "Hz");
		ui_interface->addNumEntry("low_freq", &fEntry2, 5000.0f, 20.0f, 12000.0f, 10.0f);
		ui_interface->closeBox();
		ui_interface->addVerticalSlider("middle_h_drive", &fVslider10, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->declare(&fVslider9, "unit", "db");
		ui_interface->addVerticalSlider("middle_h_gain", &fVslider9, 10.0f, -10.0f, 20.0f, 0.100000001f);
		ui_interface->addVerticalSlider("middle_l_drive", &fVslider12, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->declare(&fVslider11, "unit", "db");
		ui_interface->addVerticalSlider("middle_l_gain", &fVslider11, 10.0f, -10.0f, 20.0f, 0.100000001f);
		ui_interface->addCheckButton("res_on_off", &fCheckbox2);
		ui_interface->declare(&fEntry0, "unit", "Hz");
		ui_interface->addNumEntry("split_high_freq", &fEntry0, 1250.0f, 1250.0f, 12000.0f, 10.0f);
		ui_interface->declare(&fEntry4, "unit", "Hz");
		ui_interface->addNumEntry("split_low_freq", &fEntry4, 250.0f, 20.0f, 600.0f, 10.0f);
		ui_interface->declare(&fEntry3, "unit", "Hz");
		ui_interface->addNumEntry("split_middle_freq", &fEntry3, 650.0f, 600.0f, 1250.0f, 10.0f);
		ui_interface->addVerticalSlider("trigger", &fVslider7, 0.119999997f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->addVerticalSlider("vibrato", &fVslider8, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		int iSlow0 = int(float(fCheckbox0));
		float fSlow1 = std::tan((fConst1 * float(fVslider0)));
		float fSlow2 = (1.0f / fSlow1);
		float fSlow3 = (1.0f / (((fSlow2 + 0.765366852f) / fSlow1) + 1.0f));
		float fSlow4 = (1.0f / (((fSlow2 + 1.84775901f) / fSlow1) + 1.0f));
		float fSlow5 = (fConst1 * float(fVslider1));
		float fSlow6 = (1.0f / (fSlow5 + 1.0f));
		float fSlow7 = (0.00100000005f * std::pow(10.0f, (0.0500000007f * (float(fVslider2) + -10.0f))));
		float fSlow8 = (0.00100000005f * std::pow(10.0f, (0.0500000007f * (float(fVslider3) + -10.0f))));
		float fSlow9 = float(fVslider4);
		float fSlow10 = float(fVslider6);
		float fSlow11 = std::tan((fConst1 * float(fEntry0)));
		float fSlow12 = (1.0f / fSlow11);
		float fSlow13 = (((fSlow12 + 1.0f) / fSlow11) + 1.0f);
		float fSlow14 = (std::pow(10.0f, (2.0f * (float(fVslider5) * fSlow10))) / fSlow13);
		float fSlow15 = fx_distortion1_faustpower2_f(fSlow11);
		float fSlow16 = (1.0f / fSlow15);
		float fSlow17 = (fSlow12 + 1.0f);
		float fSlow18 = (1.0f / (fSlow11 * fSlow17));
		int iSlow19 = int(float(fCheckbox1));
		float fSlow20 = std::tan((fConst1 * float(fEntry1)));
		float fSlow21 = (1.0f / fSlow20);
		float fSlow22 = (fSlow21 + 1.0f);
		float fSlow23 = (0.0f - (1.0f / (fSlow20 * fSlow22)));
		float fSlow24 = (1.0f / std::tan((fConst1 * float(fEntry2))));
		float fSlow25 = (1.0f / (fSlow24 + 1.0f));
		float fSlow26 = (1.0f - fSlow24);
		int iSlow27 = int(float(fCheckbox2));
		float fSlow28 = (1.0f - float(fVslider7));
		float fSlow29 = float(fVslider8);
		int iSlow30 = int(std::min<float>(4096.0f, std::max<float>(0.0f, (fSlow29 + -1.0f))));
		int iSlow31 = int(std::min<float>(4096.0f, std::max<float>(0.0f, fSlow29)));
		float fSlow32 = (1.0f / fSlow22);
		float fSlow33 = (1.0f - fSlow21);
		float fSlow34 = (1.0f - fSlow5);
		float fSlow35 = (((fSlow2 + -1.84775901f) / fSlow1) + 1.0f);
		float fSlow36 = (2.0f * (1.0f - (1.0f / fx_distortion1_faustpower2_f(fSlow1))));
		float fSlow37 = (((fSlow2 + -0.765366852f) / fSlow1) + 1.0f);
		float fSlow38 = (0.0f - fSlow18);
		float fSlow39 = (1.0f - fSlow12);
		float fSlow40 = (fSlow39 / fSlow17);
		float fSlow41 = (1.0f / fSlow13);
		float fSlow42 = (((fSlow12 + -1.0f) / fSlow11) + 1.0f);
		float fSlow43 = (2.0f * (1.0f - fSlow16));
		float fSlow44 = (0.0f - (2.0f / fSlow15));
		float fSlow45 = (0.00100000005f * std::pow(10.0f, (0.0500000007f * (float(fVslider9) + -10.0f))));
		float fSlow46 = std::tan((fConst1 * float(fEntry3)));
		float fSlow47 = (1.0f / fSlow46);
		float fSlow48 = (((fSlow47 + 1.0f) / fSlow46) + 1.0f);
		float fSlow49 = (std::pow(10.0f, (2.0f * (fSlow10 * float(fVslider10)))) / fSlow48);
		float fSlow50 = fx_distortion1_faustpower2_f(fSlow46);
		float fSlow51 = (1.0f / fSlow50);
		float fSlow52 = (fSlow47 + 1.0f);
		float fSlow53 = (0.0f - (1.0f / (fSlow46 * fSlow52)));
		float fSlow54 = (1.0f / fSlow17);
		float fSlow55 = (1.0f / fSlow52);
		float fSlow56 = (1.0f - fSlow47);
		float fSlow57 = (1.0f / (fSlow46 * fSlow13));
		float fSlow58 = (1.0f / fSlow48);
		float fSlow59 = (((fSlow47 + -1.0f) / fSlow46) + 1.0f);
		float fSlow60 = (2.0f * (1.0f - fSlow51));
		float fSlow61 = (0.0f - (2.0f / fSlow50));
		float fSlow62 = (0.00100000005f * std::pow(10.0f, (0.0500000007f * (float(fVslider11) + -10.0f))));
		float fSlow63 = std::tan((fConst1 * float(fEntry4)));
		float fSlow64 = (1.0f / fSlow63);
		float fSlow65 = (((fSlow64 + 1.0f) / fSlow63) + 1.0f);
		float fSlow66 = (std::pow(10.0f, (2.0f * (fSlow10 * float(fVslider12)))) / fSlow65);
		float fSlow67 = fx_distortion1_faustpower2_f(fSlow63);
		float fSlow68 = (1.0f / fSlow67);
		float fSlow69 = (fSlow64 + 1.0f);
		float fSlow70 = (0.0f - (1.0f / (fSlow63 * fSlow69)));
		float fSlow71 = (1.0f / fSlow69);
		float fSlow72 = (1.0f - fSlow64);
		float fSlow73 = (1.0f / (fSlow63 * fSlow48));
		float fSlow74 = (1.0f / fSlow65);
		float fSlow75 = (((fSlow64 + -1.0f) / fSlow63) + 1.0f);
		float fSlow76 = (2.0f * (1.0f - fSlow68));
		float fSlow77 = (0.0f - (2.0f / fSlow67));
		float fSlow78 = (0.00100000005f * std::pow(10.0f, (0.0500000007f * (float(fVslider13) + -10.0f))));
		float fSlow79 = (std::pow(10.0f, (2.0f * (fSlow10 * float(fVslider14)))) / fSlow65);
		for (int i = 0; (i < count); i = (i + 1)) {
			iVec0[0] = 1;
			fRec4[0] = ((9.99999968e-21f * float((1 - iVec0[1]))) - fRec4[1]);
			fRec6[0] = (fSlow7 + (0.999000013f * fRec6[1]));
			fRec7[0] = (fSlow8 + (0.999000013f * fRec7[1]));
			float fTemp0 = float(input0[i]);
			float fTemp1 = (fTemp0 + (fSlow28 * fRec17[1]));
			fVec1[0] = fTemp1;
			fRec17[0] = (0.5f * (fVec1[iSlow30] + fVec1[iSlow31]));
			float fTemp2 = ((iSlow27 ? fRec17[0] : fTemp0) + fRec4[0]);
			float fTemp3 = (fRec4[0] + (iSlow19 ? 0.0f : fTemp2));
			fVec2[0] = fTemp3;
			fRec16[0] = (0.0f - (fSlow25 * ((fSlow26 * fRec16[1]) - (fTemp3 + fVec2[1]))));
			fRec15[0] = ((fSlow23 * fRec16[1]) - (fSlow32 * ((fSlow33 * fRec15[1]) - (fSlow21 * fRec16[0]))));
			float fTemp4 = (iSlow19 ? fTemp2 : fRec15[0]);
			float fTemp5 = (fRec4[0] + (iSlow0 ? 0.0f : fTemp4));
			fVec3[0] = (fSlow6 * fTemp5);
			fRec14[0] = ((fSlow6 * (fTemp5 + (fSlow34 * fRec14[1]))) - fVec3[1]);
			fVec4[0] = (fSlow6 * fRec14[0]);
			fRec13[0] = ((fSlow6 * (fRec14[0] + (fSlow34 * fRec13[1]))) - fVec4[1]);
			fRec12[0] = (fRec13[0] - (fSlow4 * ((fSlow35 * fRec12[2]) + (fSlow36 * fRec12[1]))));
			fRec11[0] = ((fSlow4 * (fRec12[2] + (fRec12[0] + (2.0f * fRec12[1])))) - (fSlow3 * ((fSlow37 * fRec11[2]) + (fSlow36 * fRec11[1]))));
			fRec10[0] = ((iSlow0 ? fTemp4 : (fSlow3 * (fRec11[2] + (fRec11[0] + (2.0f * fRec11[1]))))) - (fConst7 * ((fConst10 * fRec10[2]) + (fConst11 * fRec10[1]))));
			float fTemp6 = (((fConst9 * fRec10[1]) + (fConst12 * fRec10[0])) + (fConst13 * fRec10[2]));
			fVec5[0] = fTemp6;
			fRec9[0] = ((fConst7 * ((fSlow18 * fTemp6) + (fSlow38 * fVec5[1]))) - (fSlow40 * fRec9[1]));
			fRec8[0] = (fRec9[0] - (fSlow41 * ((fSlow42 * fRec8[2]) + (fSlow43 * fRec8[1]))));
			float fTemp7 = std::max<float>(-1.0f, std::min<float>(1.0f, (fSlow9 + (fSlow14 * (((fSlow16 * fRec8[0]) + (fSlow44 * fRec8[1])) + (fSlow16 * fRec8[2]))))));
			fRec18[0] = (fSlow45 + (0.999000013f * fRec18[1]));
			fRec22[0] = (0.0f - (fSlow54 * ((fSlow39 * fRec22[1]) - (fConst7 * (fTemp6 + fVec5[1])))));
			fRec21[0] = (fRec22[0] - (fSlow41 * ((fSlow42 * fRec21[2]) + (fSlow43 * fRec21[1]))));
			float fTemp8 = (fRec21[2] + (fRec21[0] + (2.0f * fRec21[1])));
			float fTemp9 = (fSlow41 * fTemp8);
			fVec6[0] = fTemp9;
			fRec20[0] = ((fSlow53 * fVec6[1]) - (fSlow55 * ((fSlow56 * fRec20[1]) - (fSlow57 * fTemp8))));
			fRec19[0] = (fRec20[0] - (fSlow58 * ((fSlow59 * fRec19[2]) + (fSlow60 * fRec19[1]))));
			float fTemp10 = std::max<float>(-1.0f, std::min<float>(1.0f, (fSlow9 + (fSlow49 * (((fSlow51 * fRec19[0]) + (fSlow61 * fRec19[1])) + (fSlow51 * fRec19[2]))))));
			fRec23[0] = (fSlow62 + (0.999000013f * fRec23[1]));
			fRec27[0] = (0.0f - (fSlow55 * ((fSlow56 * fRec27[1]) - (fTemp9 + fVec6[1]))));
			fRec26[0] = (fRec27[0] - (fSlow58 * ((fSlow59 * fRec26[2]) + (fSlow60 * fRec26[1]))));
			float fTemp11 = (fRec26[2] + (fRec26[0] + (2.0f * fRec26[1])));
			float fTemp12 = (fSlow58 * fTemp11);
			fVec7[0] = fTemp12;
			fRec25[0] = ((fSlow70 * fVec7[1]) - (fSlow71 * ((fSlow72 * fRec25[1]) - (fSlow73 * fTemp11))));
			fRec24[0] = (fRec25[0] - (fSlow74 * ((fSlow75 * fRec24[2]) + (fSlow76 * fRec24[1]))));
			float fTemp13 = std::max<float>(-1.0f, std::min<float>(1.0f, (fSlow9 + (fSlow66 * (((fSlow68 * fRec24[0]) + (fSlow77 * fRec24[1])) + (fSlow68 * fRec24[2]))))));
			fRec28[0] = (fSlow78 + (0.999000013f * fRec28[1]));
			fRec30[0] = (0.0f - (fSlow71 * ((fSlow72 * fRec30[1]) - (fTemp12 + fVec7[1]))));
			fRec29[0] = (fRec30[0] - (fSlow74 * ((fSlow75 * fRec29[2]) + (fSlow76 * fRec29[1]))));
			float fTemp14 = std::max<float>(-1.0f, std::min<float>(1.0f, (fSlow9 + (fSlow79 * (fRec29[2] + (fRec29[0] + (2.0f * fRec29[1])))))));
			fRec5[0] = ((fRec6[0] * (((((fRec7[0] * fTemp7) * (1.0f - (0.333333343f * fx_distortion1_faustpower2_f(fTemp7)))) + ((fRec18[0] * fTemp10) * (1.0f - (0.333333343f * fx_distortion1_faustpower2_f(fTemp10))))) + ((fRec23[0] * fTemp13) * (1.0f - (0.333333343f * fx_distortion1_faustpower2_f(fTemp13))))) + ((fRec28[0] * fTemp14) * (1.0f - (0.333333343f * fx_distortion1_faustpower2_f(fTemp14)))))) - (fConst7 * ((fConst10 * fRec5[2]) + (fConst11 * fRec5[1]))));
			float fTemp15 = (fConst7 * (((fConst9 * fRec5[1]) + (fConst12 * fRec5[0])) + (fConst13 * fRec5[2])));
			float fTemp16 = (fRec4[0] + (iSlow0 ? 0.0f : fTemp15));
			fVec8[0] = (fSlow6 * fTemp16);
			fRec3[0] = ((fSlow6 * (fTemp16 + (fSlow34 * fRec3[1]))) - fVec8[1]);
			fVec9[0] = (fSlow6 * fRec3[0]);
			fRec2[0] = ((fSlow6 * (fRec3[0] + (fSlow34 * fRec2[1]))) - fVec9[1]);
			fRec1[0] = (fRec2[0] - (fSlow4 * ((fSlow35 * fRec1[2]) + (fSlow36 * fRec1[1]))));
			fRec0[0] = ((fSlow4 * (fRec1[2] + (fRec1[0] + (2.0f * fRec1[1])))) - (fSlow3 * ((fSlow37 * fRec0[2]) + (fSlow36 * fRec0[1]))));
			output0[i] = FAUSTFLOAT((iSlow0 ? fTemp15 : (fSlow3 * (fRec0[2] + (fRec0[0] + (2.0f * fRec0[1]))))));
			iVec0[1] = iVec0[0];
			fRec4[1] = fRec4[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fVec1[1] = fVec1[0];
			fRec17[1] = fRec17[0];
			fVec2[1] = fVec2[0];
			fRec16[1] = fRec16[0];
			fRec15[1] = fRec15[0];
			fVec3[1] = fVec3[0];
			fRec14[1] = fRec14[0];
			fVec4[1] = fVec4[0];
			fRec13[1] = fRec13[0];
			fRec12[2] = fRec12[1];
			fRec12[1] = fRec12[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			fVec5[1] = fVec5[0];
			fRec9[1] = fRec9[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec18[1] = fRec18[0];
			fRec22[1] = fRec22[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fVec6[1] = fVec6[0];
			fRec20[1] = fRec20[0];
			fRec19[2] = fRec19[1];
			fRec19[1] = fRec19[0];
			fRec23[1] = fRec23[0];
			fRec27[1] = fRec27[0];
			fRec26[2] = fRec26[1];
			fRec26[1] = fRec26[0];
			fVec7[1] = fVec7[0];
			fRec25[1] = fRec25[0];
			fRec24[2] = fRec24[1];
			fRec24[1] = fRec24[0];
			fRec28[1] = fRec28[0];
			fRec30[1] = fRec30[0];
			fRec29[2] = fRec29[1];
			fRec29[1] = fRec29[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fVec8[1] = fVec8[0];
			fRec3[1] = fRec3[0];
			fVec9[1] = fVec9[0];
			fRec2[1] = fRec2[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			
		}
		
	}

};
// clang-format on
#endif

    template <class T>
    struct _fx_distortion1_UI : public UI {
    static std::string name;
};

template <class T>
std::string _fx_distortion1_UI<T>::name(sym(fx_distortion1));

typedef _fx_distortion1_UI<fx_distortion1> fx_distortion1_UI;

class faust_fx_distortion1_tilde : public FaustExternal<fx_distortion1, fx_distortion1_UI> {
public:
    faust_fx_distortion1_tilde(const ceammc::PdArgs& args)
        : FaustExternal(args)
    {
    }
};

#endif
