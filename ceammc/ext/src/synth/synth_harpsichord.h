/* ------------------------------------------------------------
author: "Romain Michon (rmichon@ccrma.stanford.edu)"
copyright: "Romain Michon"
name: "synth.harpsichord"
version: "1.0"
Code generated with Faust 2.53.1 (https://faust.grame.fr)
Compilation options: -a /Users/serge/work/music/pure-data/ceammc/faust/faust_arch_ceammc.cpp -lang cpp -i -cn synth_harpsichord -scn synth_harpsichord_dsp -es 1 -mcd 16 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __synth_harpsichord_H__
#define  __synth_harpsichord_H__

// FAUST Architecture File for ceammc::SoundExternal class
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

/************************** BEGIN synth_harpsichord_dsp.h ********************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>
#include <vector>

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ***************************************************************************/

#ifndef __export__
#define __export__

#define FAUSTVERSION "2.53.1"

// Use FAUST_API for code that is part of the external API but is also compiled in faust and libfaust
// Use LIBFAUST_API for code that is compiled in faust and libfaust

#ifdef _WIN32
    #pragma warning (disable: 4251)
    #ifdef FAUST_EXE
        #define FAUST_API
        #define LIBFAUST_API
    #elif FAUST_LIB
        #define FAUST_API __declspec(dllexport)
        #define LIBFAUST_API __declspec(dllexport)
    #else
        #define FAUST_API
        #define LIBFAUST_API 
    #endif
#else
    #ifdef FAUST_EXE
        #define FAUST_API
        #define LIBFAUST_API
    #else
        #define FAUST_API __attribute__((visibility("default")))
        #define LIBFAUST_API __attribute__((visibility("default")))
    #endif
#endif

#endif

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct FAUST_API UI;
struct FAUST_API Meta;

/**
 * DSP memory manager.
 */

struct FAUST_API dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    /**
     * Inform the Memory Manager with the number of expected memory zones.
     * @param count - the number of expected memory zones
     */
    virtual void begin(size_t /*count*/) {}
    
    /**
     * Give the Memory Manager information on a given memory zone.
     * @param size - the size in bytes of the memory zone
     * @param reads - the number of Read access to the zone used to compute one frame
     * @param writes - the number of Write access to the zone used to compute one frame
     */
    virtual void info(size_t /*size*/, size_t /*reads*/, size_t /*writes*/) {}

    /**
     * Inform the Memory Manager that all memory zones have been described,
     * to possibly start a 'compute the best allocation strategy' step.
     */
    virtual void end() {}
    
    /**
     * Allocate a memory zone.
     * @param size - the memory zone size in bytes
     */
    virtual void* allocate(size_t size) = 0;
    
    /**
     * Destroy a memory zone.
     * @param ptr - the memory zone pointer to be deallocated
     */
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class FAUST_API synth_harpsichord_dsp {

    public:

        synth_harpsichord_dsp() {}
        virtual ~synth_harpsichord_dsp() {}

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
        virtual synth_harpsichord_dsp* clone() = 0;
    
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

class FAUST_API decorator_dsp : public synth_harpsichord_dsp {

    protected:

        synth_harpsichord_dsp* fDSP;

    public:

        decorator_dsp(synth_harpsichord_dsp* synth_harpsichord_dsp = nullptr):fDSP(synth_harpsichord_dsp) {}
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

class FAUST_API dsp_factory {
    
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
    
        virtual synth_harpsichord_dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

// Denormal handling

#if defined (__SSE__)
#include <xmmintrin.h>
#endif

class FAUST_API ScopedNoDenormals {
    
    private:
    
        intptr_t fpsr = 0;
        
        void setFpStatusRegister(intptr_t fpsr_aux) noexcept
        {
        #if defined (__arm64__) || defined (__aarch64__)
            asm volatile("msr fpcr, %0" : : "ri" (fpsr_aux));
        #elif defined (__SSE__)
            // The volatile keyword here is needed to workaround a bug in AppleClang 13.0
            // which aggressively optimises away the variable otherwise
            volatile uint32_t fpsr_w = static_cast<uint32_t>(fpsr_aux);
            _mm_setcsr(fpsr_w);
        #endif
        }
        
        void getFpStatusRegister() noexcept
        {
        #if defined (__arm64__) || defined (__aarch64__)
            asm volatile("mrs %0, fpcr" : "=r" (fpsr));
        #elif defined (__SSE__)
            fpsr = static_cast<intptr_t>(_mm_getcsr());
        #endif
        }
    
    public:
    
        ScopedNoDenormals() noexcept
        {
        #if defined (__arm64__) || defined (__aarch64__)
            intptr_t mask = (1 << 24 /* FZ */);
        #elif defined (__SSE__)
        #if defined (__SSE2__)
            intptr_t mask = 0x8040;
        #else
            intptr_t mask = 0x8000;
        #endif
        #else
            intptr_t mask = 0x0000;
        #endif
            getFpStatusRegister();
            setFpStatusRegister(fpsr | mask);
        }
        
        ~ScopedNoDenormals() noexcept
        {
            setFpStatusRegister(fpsr);
        }

};

#define AVOIDDENORMALS ScopedNoDenormals ftz_scope;

#endif

/************************** END synth_harpsichord_dsp.h **************************/
/************************** BEGIN misc.h *******************************
FAUST Architecture File
Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
---------------------------------------------------------------------
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

EXCEPTION : As a special exception, you may create a larger work
that contains this FAUST architecture section and distribute
that work under terms of your choice, so long as this FAUST
architecture section is not modified.
***************************************************************************/

#ifndef __misc__
#define __misc__

#include <algorithm>
#include <map>
#include <cstdlib>
#include <string.h>
#include <fstream>
#include <string>

/************************** BEGIN meta.h *******************************
 FAUST Architecture File
 Copyright (C) 2003-2022 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __meta__
#define __meta__


/**
 The base class of Meta handler to be used in synth_harpsichord_dsp::metadata(Meta* m) method to retrieve (key, value) metadata.
 */
struct FAUST_API Meta {
    virtual ~Meta() {}
    virtual void declare(const char* key, const char* value) = 0;
};

#endif
/**************************  END  meta.h **************************/

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
struct synth_harpsichord : public synth_harpsichord_dsp {
};
#endif
// clang-format on

#ifdef FAUST_MACRO
// clang-format off
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

/* link with : "" */
#include "harpsichord.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS synth_harpsichord
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

class synth_harpsichordSIG0 {
	
  private:
	
	int iVec1[2];
	int iRec17[2];
	
  public:
	
	int getNumInputssynth_harpsichordSIG0() {
		return 0;
	}
	int getNumOutputssynth_harpsichordSIG0() {
		return 1;
	}
	
	void instanceInitsynth_harpsichordSIG0(int sample_rate) {
		for (int l6 = 0; l6 < 2; l6 = l6 + 1) {
			iVec1[l6] = 0;
		}
		for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
			iRec17[l7] = 0;
		}
	}
	
	void fillsynth_harpsichordSIG0(int count, float* table) {
		for (int i1 = 0; i1 < count; i1 = i1 + 1) {
			iVec1[0] = 1;
			iRec17[0] = (iVec1[1] + iRec17[1]) % 65536;
			table[i1] = std::sin(9.58738e-05f * float(iRec17[0]));
			iVec1[1] = iVec1[0];
			iRec17[1] = iRec17[0];
		}
	}

};

static synth_harpsichordSIG0* newsynth_harpsichordSIG0() { return (synth_harpsichordSIG0*)new synth_harpsichordSIG0(); }
static void deletesynth_harpsichordSIG0(synth_harpsichordSIG0* dsp) { delete dsp; }

static float synth_harpsichord_faustpower2_f(float value) {
	return value * value;
}
static float ftbl0synth_harpsichordSIG0[65536];

class synth_harpsichord : public synth_harpsichord_dsp {
	
 private:
	
	int fSampleRate;
	float fConst3;
	FAUSTFLOAT fHslider0;
	float fConst4;
	float fConst5;
	float fConst7;
	float fConst8;
	float fRec11[2];
	float fRec10[2];
	int IOTA0;
	float fVec0[32768];
	int iConst10;
	FAUSTFLOAT fHslider1;
	float fConst11;
	FAUSTFLOAT fHslider2;
	float fConst12;
	float fRec12[2];
	FAUSTFLOAT fHslider3;
	FAUSTFLOAT fCheckbox0;
	float fRec15[2];
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fHslider4;
	float fRec16[2];
	float fConst13;
	FAUSTFLOAT fHslider5;
	float fRec19[2];
	float fRec18[2];
	float fRec25[2];
	float fRec24[2];
	float fRec23[2];
	float fRec22[2];
	float fRec21[2];
	float fRec20[2];
	float fRec31[2];
	float fRec30[2];
	float fRec29[2];
	float fRec28[2];
	float fRec27[2];
	float fRec26[2];
	int iRec33[2];
	float fConst14;
	float fRec34[2];
	float fRec32[2];
	int iRec35[2];
	float fVec2[8192];
	float fConst15;
	float fRec14[3];
	float fConst16;
	float fConst17;
	float fConst18;
	float fConst19;
	float fRec37[2];
	float fRec36[2];
	float fRec13[8192];
	float fConst20;
	FAUSTFLOAT fHslider6;
	float fVec3[4096];
	int iConst21;
	float fVec4[2048];
	int iConst22;
	float fRec8[2];
	float fConst25;
	float fConst26;
	float fRec41[2];
	float fRec40[2];
	float fVec5[32768];
	int iConst28;
	float fVec6[4096];
	int iConst29;
	float fRec38[2];
	float fConst32;
	float fConst33;
	float fRec45[2];
	float fRec44[2];
	float fVec7[16384];
	int iConst35;
	float fVec8[4096];
	float fVec9[2048];
	int iConst36;
	float fRec42[2];
	float fConst39;
	float fConst40;
	float fRec49[2];
	float fRec48[2];
	float fVec10[32768];
	int iConst42;
	float fVec11[4096];
	int iConst43;
	float fRec46[2];
	float fConst46;
	float fConst47;
	float fRec53[2];
	float fRec52[2];
	float fVec12[16384];
	int iConst49;
	float fVec13[4096];
	int iConst50;
	float fRec50[2];
	float fConst53;
	float fConst54;
	float fRec57[2];
	float fRec56[2];
	float fVec14[16384];
	int iConst56;
	float fVec15[2048];
	int iConst57;
	float fRec54[2];
	float fConst60;
	float fConst61;
	float fRec61[2];
	float fRec60[2];
	float fVec16[16384];
	int iConst63;
	float fVec17[4096];
	int iConst64;
	float fRec58[2];
	float fConst67;
	float fConst68;
	float fRec65[2];
	float fRec64[2];
	float fVec18[16384];
	int iConst70;
	float fVec19[4096];
	int iConst71;
	float fRec62[2];
	float fRec0[3];
	float fRec1[3];
	float fRec2[3];
	float fRec3[3];
	float fRec4[3];
	float fRec5[3];
	float fRec6[3];
	float fRec7[3];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("analyzers.lib/amp_follower_ar:author", "Jonatan Liljedahl, revised by Romain Michon");
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "0.2");
		m->declare("author", "Romain Michon (rmichon@ccrma.stanford.edu)");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.8");
		m->declare("ceammc.lib/name", "Ceammc PureData misc utils");
		m->declare("ceammc.lib/version", "0.1.4");
		m->declare("ceammc_instruments.lib/name", "CEAMMC replacement for intruments.lib");
		m->declare("ceammc_instruments.lib/version", "0.3");
		m->declare("ceammc_ui.lib/name", "CEAMMC faust default UI elements");
		m->declare("ceammc_ui.lib/version", "0.1.2");
		m->declare("compile_options", "-a /Users/serge/work/music/pure-data/ceammc/faust/faust_arch_ceammc.cpp -lang cpp -i -cn synth_harpsichord -scn synth_harpsichord_dsp -es 1 -mcd 16 -single -ftz 0");
		m->declare("compressors.lib/compression_gain_mono:author", "Julius O. Smith III");
		m->declare("compressors.lib/compression_gain_mono:copyright", "Copyright (C) 2014-2020 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("compressors.lib/compression_gain_mono:license", "MIT-style STK-4.3 license");
		m->declare("compressors.lib/compressor_lad_mono:author", "Julius O. Smith III");
		m->declare("compressors.lib/compressor_lad_mono:copyright", "Copyright (C) 2014-2020 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("compressors.lib/compressor_lad_mono:license", "MIT-style STK-4.3 license");
		m->declare("compressors.lib/compressor_mono:author", "Julius O. Smith III");
		m->declare("compressors.lib/compressor_mono:copyright", "Copyright (C) 2014-2020 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("compressors.lib/compressor_mono:license", "MIT-style STK-4.3 license");
		m->declare("compressors.lib/name", "Faust Compressor Effect Library");
		m->declare("compressors.lib/version", "0.4");
		m->declare("copyright", "Romain Michon");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("description", "Nonlinear WaveGuide Commuted Harpsichord");
		m->declare("filename", "synth_harpsichord.dsp");
		m->declare("filters.lib/allpass_comb:author", "Julius O. Smith III");
		m->declare("filters.lib/allpass_comb:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/allpass_comb:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/allpassnn:author", "Julius O. Smith III");
		m->declare("filters.lib/allpassnn:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/allpassnn:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/tf1:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf1s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "0.3");
		m->declare("instruments.lib/author", "Romain Michon (rmichon@ccrma.stanford.edu)");
		m->declare("instruments.lib/copyright", "Romain Michon");
		m->declare("instruments.lib/licence", "STK-4.3");
		m->declare("instruments.lib/name", "Faust-STK Tools Library");
		m->declare("instruments.lib/version", "1.0");
		m->declare("licence", "STK-4.3");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.5");
		m->declare("name", "synth.harpsichord");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "0.4");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.3");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.2");
		m->declare("reverbs.lib/name", "Faust Reverb Library");
		m->declare("reverbs.lib/version", "0.2");
		m->declare("routes.lib/hadamard:author", "Remy Muller, revised by Romain Michon");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "0.2");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/onePoleSwitching:author", "Jonatan Liljedahl, revised by Dario Sanfilippo");
		m->declare("signals.lib/onePoleSwitching:licence", "STK-4.3");
		m->declare("signals.lib/version", "0.3");
		m->declare("spn.lib/name", "Standart Pitch Notation constants");
		m->declare("spn.lib/version", "0.2");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
		synth_harpsichordSIG0* sig0 = newsynth_harpsichordSIG0();
		sig0->instanceInitsynth_harpsichordSIG0(sample_rate);
		sig0->fillsynth_harpsichordSIG0(65536, ftbl0synth_harpsichordSIG0);
		deletesynth_harpsichordSIG0(sig0);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		float fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
		float fConst1 = std::floor(0.219991f * fConst0 + 0.5f);
		float fConst2 = (0.0f - 6.9077554f * fConst1) / fConst0;
		fConst3 = 0.25f * fConst2;
		fConst4 = std::cos(37699.113f / fConst0);
		fConst5 = 0.16666667f * fConst2;
		float fConst6 = 1.0f / std::tan(628.31854f / fConst0);
		fConst7 = 1.0f / (fConst6 + 1.0f);
		fConst8 = 1.0f - fConst6;
		float fConst9 = std::floor(0.019123f * fConst0 + 0.5f);
		iConst10 = int(std::min<float>(16384.0f, std::max<float>(0.0f, fConst1 - fConst9)));
		fConst11 = 44.1f / fConst0;
		fConst12 = 1.0f - fConst11;
		fConst13 = 1.0f / fConst0;
		fConst14 = 7.0f / fConst0;
		fConst15 = 0.0022727272f * fConst0;
		fConst16 = std::exp(0.0f - 0.2f / fConst0);
		fConst17 = 1.0f - fConst16;
		fConst18 = std::exp(0.0f - 0.02f / fConst0);
		fConst19 = std::exp(0.0f - 0.1f / fConst0);
		fConst20 = 0.0011363636f * fConst0;
		iConst21 = int(std::min<float>(8192.0f, std::max<float>(0.0f, 0.02f * fConst0)));
		iConst22 = int(std::min<float>(1024.0f, std::max<float>(0.0f, fConst9 + -1.0f)));
		float fConst23 = std::floor(0.210389f * fConst0 + 0.5f);
		float fConst24 = (0.0f - 6.9077554f * fConst23) / fConst0;
		fConst25 = 0.25f * fConst24;
		fConst26 = 0.16666667f * fConst24;
		float fConst27 = std::floor(0.024421f * fConst0 + 0.5f);
		iConst28 = int(std::min<float>(16384.0f, std::max<float>(0.0f, fConst23 - fConst27)));
		iConst29 = int(std::min<float>(2048.0f, std::max<float>(0.0f, fConst27 + -1.0f)));
		float fConst30 = std::floor(0.125f * fConst0 + 0.5f);
		float fConst31 = (0.0f - 6.9077554f * fConst30) / fConst0;
		fConst32 = 0.25f * fConst31;
		fConst33 = 0.16666667f * fConst31;
		float fConst34 = std::floor(0.013458f * fConst0 + 0.5f);
		iConst35 = int(std::min<float>(8192.0f, std::max<float>(0.0f, fConst30 - fConst34)));
		iConst36 = int(std::min<float>(1024.0f, std::max<float>(0.0f, fConst34 + -1.0f)));
		float fConst37 = std::floor(0.256891f * fConst0 + 0.5f);
		float fConst38 = (0.0f - 6.9077554f * fConst37) / fConst0;
		fConst39 = 0.25f * fConst38;
		fConst40 = 0.16666667f * fConst38;
		float fConst41 = std::floor(0.027333f * fConst0 + 0.5f);
		iConst42 = int(std::min<float>(16384.0f, std::max<float>(0.0f, fConst37 - fConst41)));
		iConst43 = int(std::min<float>(2048.0f, std::max<float>(0.0f, fConst41 + -1.0f)));
		float fConst44 = std::floor(0.192303f * fConst0 + 0.5f);
		float fConst45 = (0.0f - 6.9077554f * fConst44) / fConst0;
		fConst46 = 0.25f * fConst45;
		fConst47 = 0.16666667f * fConst45;
		float fConst48 = std::floor(0.029291f * fConst0 + 0.5f);
		iConst49 = int(std::min<float>(8192.0f, std::max<float>(0.0f, fConst44 - fConst48)));
		iConst50 = int(std::min<float>(2048.0f, std::max<float>(0.0f, fConst48 + -1.0f)));
		float fConst51 = std::floor(0.153129f * fConst0 + 0.5f);
		float fConst52 = (0.0f - 6.9077554f * fConst51) / fConst0;
		fConst53 = 0.25f * fConst52;
		fConst54 = 0.16666667f * fConst52;
		float fConst55 = std::floor(0.020346f * fConst0 + 0.5f);
		iConst56 = int(std::min<float>(8192.0f, std::max<float>(0.0f, fConst51 - fConst55)));
		iConst57 = int(std::min<float>(1024.0f, std::max<float>(0.0f, fConst55 + -1.0f)));
		float fConst58 = std::floor(0.127837f * fConst0 + 0.5f);
		float fConst59 = (0.0f - 6.9077554f * fConst58) / fConst0;
		fConst60 = 0.25f * fConst59;
		fConst61 = 0.16666667f * fConst59;
		float fConst62 = std::floor(0.031604f * fConst0 + 0.5f);
		iConst63 = int(std::min<float>(8192.0f, std::max<float>(0.0f, fConst58 - fConst62)));
		iConst64 = int(std::min<float>(2048.0f, std::max<float>(0.0f, fConst62 + -1.0f)));
		float fConst65 = std::floor(0.174713f * fConst0 + 0.5f);
		float fConst66 = (0.0f - 6.9077554f * fConst65) / fConst0;
		fConst67 = 0.25f * fConst66;
		fConst68 = 0.16666667f * fConst66;
		float fConst69 = std::floor(0.022904f * fConst0 + 0.5f);
		iConst70 = int(std::min<float>(8192.0f, std::max<float>(0.0f, fConst65 - fConst69)));
		iConst71 = int(std::min<float>(2048.0f, std::max<float>(0.0f, fConst69 + -1.0f)));
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.36f);
		fHslider1 = FAUSTFLOAT(0.6f);
		fHslider2 = FAUSTFLOAT(0.137f);
		fHslider3 = FAUSTFLOAT(48.0f);
		fCheckbox0 = FAUSTFLOAT(0.0f);
		fEntry0 = FAUSTFLOAT(0.0f);
		fHslider4 = FAUSTFLOAT(0.0f);
		fHslider5 = FAUSTFLOAT(2.2e+02f);
		fHslider6 = FAUSTFLOAT(0.5f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			fRec11[l0] = 0.0f;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			fRec10[l1] = 0.0f;
		}
		IOTA0 = 0;
		for (int l2 = 0; l2 < 32768; l2 = l2 + 1) {
			fVec0[l2] = 0.0f;
		}
		for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
			fRec12[l3] = 0.0f;
		}
		for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
			fRec15[l4] = 0.0f;
		}
		for (int l5 = 0; l5 < 2; l5 = l5 + 1) {
			fRec16[l5] = 0.0f;
		}
		for (int l8 = 0; l8 < 2; l8 = l8 + 1) {
			fRec19[l8] = 0.0f;
		}
		for (int l9 = 0; l9 < 2; l9 = l9 + 1) {
			fRec18[l9] = 0.0f;
		}
		for (int l10 = 0; l10 < 2; l10 = l10 + 1) {
			fRec25[l10] = 0.0f;
		}
		for (int l11 = 0; l11 < 2; l11 = l11 + 1) {
			fRec24[l11] = 0.0f;
		}
		for (int l12 = 0; l12 < 2; l12 = l12 + 1) {
			fRec23[l12] = 0.0f;
		}
		for (int l13 = 0; l13 < 2; l13 = l13 + 1) {
			fRec22[l13] = 0.0f;
		}
		for (int l14 = 0; l14 < 2; l14 = l14 + 1) {
			fRec21[l14] = 0.0f;
		}
		for (int l15 = 0; l15 < 2; l15 = l15 + 1) {
			fRec20[l15] = 0.0f;
		}
		for (int l16 = 0; l16 < 2; l16 = l16 + 1) {
			fRec31[l16] = 0.0f;
		}
		for (int l17 = 0; l17 < 2; l17 = l17 + 1) {
			fRec30[l17] = 0.0f;
		}
		for (int l18 = 0; l18 < 2; l18 = l18 + 1) {
			fRec29[l18] = 0.0f;
		}
		for (int l19 = 0; l19 < 2; l19 = l19 + 1) {
			fRec28[l19] = 0.0f;
		}
		for (int l20 = 0; l20 < 2; l20 = l20 + 1) {
			fRec27[l20] = 0.0f;
		}
		for (int l21 = 0; l21 < 2; l21 = l21 + 1) {
			fRec26[l21] = 0.0f;
		}
		for (int l22 = 0; l22 < 2; l22 = l22 + 1) {
			iRec33[l22] = 0;
		}
		for (int l23 = 0; l23 < 2; l23 = l23 + 1) {
			fRec34[l23] = 0.0f;
		}
		for (int l24 = 0; l24 < 2; l24 = l24 + 1) {
			fRec32[l24] = 0.0f;
		}
		for (int l25 = 0; l25 < 2; l25 = l25 + 1) {
			iRec35[l25] = 0;
		}
		for (int l26 = 0; l26 < 8192; l26 = l26 + 1) {
			fVec2[l26] = 0.0f;
		}
		for (int l27 = 0; l27 < 3; l27 = l27 + 1) {
			fRec14[l27] = 0.0f;
		}
		for (int l28 = 0; l28 < 2; l28 = l28 + 1) {
			fRec37[l28] = 0.0f;
		}
		for (int l29 = 0; l29 < 2; l29 = l29 + 1) {
			fRec36[l29] = 0.0f;
		}
		for (int l30 = 0; l30 < 8192; l30 = l30 + 1) {
			fRec13[l30] = 0.0f;
		}
		for (int l31 = 0; l31 < 4096; l31 = l31 + 1) {
			fVec3[l31] = 0.0f;
		}
		for (int l32 = 0; l32 < 2048; l32 = l32 + 1) {
			fVec4[l32] = 0.0f;
		}
		for (int l33 = 0; l33 < 2; l33 = l33 + 1) {
			fRec8[l33] = 0.0f;
		}
		for (int l34 = 0; l34 < 2; l34 = l34 + 1) {
			fRec41[l34] = 0.0f;
		}
		for (int l35 = 0; l35 < 2; l35 = l35 + 1) {
			fRec40[l35] = 0.0f;
		}
		for (int l36 = 0; l36 < 32768; l36 = l36 + 1) {
			fVec5[l36] = 0.0f;
		}
		for (int l37 = 0; l37 < 4096; l37 = l37 + 1) {
			fVec6[l37] = 0.0f;
		}
		for (int l38 = 0; l38 < 2; l38 = l38 + 1) {
			fRec38[l38] = 0.0f;
		}
		for (int l39 = 0; l39 < 2; l39 = l39 + 1) {
			fRec45[l39] = 0.0f;
		}
		for (int l40 = 0; l40 < 2; l40 = l40 + 1) {
			fRec44[l40] = 0.0f;
		}
		for (int l41 = 0; l41 < 16384; l41 = l41 + 1) {
			fVec7[l41] = 0.0f;
		}
		for (int l42 = 0; l42 < 4096; l42 = l42 + 1) {
			fVec8[l42] = 0.0f;
		}
		for (int l43 = 0; l43 < 2048; l43 = l43 + 1) {
			fVec9[l43] = 0.0f;
		}
		for (int l44 = 0; l44 < 2; l44 = l44 + 1) {
			fRec42[l44] = 0.0f;
		}
		for (int l45 = 0; l45 < 2; l45 = l45 + 1) {
			fRec49[l45] = 0.0f;
		}
		for (int l46 = 0; l46 < 2; l46 = l46 + 1) {
			fRec48[l46] = 0.0f;
		}
		for (int l47 = 0; l47 < 32768; l47 = l47 + 1) {
			fVec10[l47] = 0.0f;
		}
		for (int l48 = 0; l48 < 4096; l48 = l48 + 1) {
			fVec11[l48] = 0.0f;
		}
		for (int l49 = 0; l49 < 2; l49 = l49 + 1) {
			fRec46[l49] = 0.0f;
		}
		for (int l50 = 0; l50 < 2; l50 = l50 + 1) {
			fRec53[l50] = 0.0f;
		}
		for (int l51 = 0; l51 < 2; l51 = l51 + 1) {
			fRec52[l51] = 0.0f;
		}
		for (int l52 = 0; l52 < 16384; l52 = l52 + 1) {
			fVec12[l52] = 0.0f;
		}
		for (int l53 = 0; l53 < 4096; l53 = l53 + 1) {
			fVec13[l53] = 0.0f;
		}
		for (int l54 = 0; l54 < 2; l54 = l54 + 1) {
			fRec50[l54] = 0.0f;
		}
		for (int l55 = 0; l55 < 2; l55 = l55 + 1) {
			fRec57[l55] = 0.0f;
		}
		for (int l56 = 0; l56 < 2; l56 = l56 + 1) {
			fRec56[l56] = 0.0f;
		}
		for (int l57 = 0; l57 < 16384; l57 = l57 + 1) {
			fVec14[l57] = 0.0f;
		}
		for (int l58 = 0; l58 < 2048; l58 = l58 + 1) {
			fVec15[l58] = 0.0f;
		}
		for (int l59 = 0; l59 < 2; l59 = l59 + 1) {
			fRec54[l59] = 0.0f;
		}
		for (int l60 = 0; l60 < 2; l60 = l60 + 1) {
			fRec61[l60] = 0.0f;
		}
		for (int l61 = 0; l61 < 2; l61 = l61 + 1) {
			fRec60[l61] = 0.0f;
		}
		for (int l62 = 0; l62 < 16384; l62 = l62 + 1) {
			fVec16[l62] = 0.0f;
		}
		for (int l63 = 0; l63 < 4096; l63 = l63 + 1) {
			fVec17[l63] = 0.0f;
		}
		for (int l64 = 0; l64 < 2; l64 = l64 + 1) {
			fRec58[l64] = 0.0f;
		}
		for (int l65 = 0; l65 < 2; l65 = l65 + 1) {
			fRec65[l65] = 0.0f;
		}
		for (int l66 = 0; l66 < 2; l66 = l66 + 1) {
			fRec64[l66] = 0.0f;
		}
		for (int l67 = 0; l67 < 16384; l67 = l67 + 1) {
			fVec18[l67] = 0.0f;
		}
		for (int l68 = 0; l68 < 4096; l68 = l68 + 1) {
			fVec19[l68] = 0.0f;
		}
		for (int l69 = 0; l69 < 2; l69 = l69 + 1) {
			fRec62[l69] = 0.0f;
		}
		for (int l70 = 0; l70 < 3; l70 = l70 + 1) {
			fRec0[l70] = 0.0f;
		}
		for (int l71 = 0; l71 < 3; l71 = l71 + 1) {
			fRec1[l71] = 0.0f;
		}
		for (int l72 = 0; l72 < 3; l72 = l72 + 1) {
			fRec2[l72] = 0.0f;
		}
		for (int l73 = 0; l73 < 3; l73 = l73 + 1) {
			fRec3[l73] = 0.0f;
		}
		for (int l74 = 0; l74 < 3; l74 = l74 + 1) {
			fRec4[l74] = 0.0f;
		}
		for (int l75 = 0; l75 < 3; l75 = l75 + 1) {
			fRec5[l75] = 0.0f;
		}
		for (int l76 = 0; l76 < 3; l76 = l76 + 1) {
			fRec6[l76] = 0.0f;
		}
		for (int l77 = 0; l77 < 3; l77 = l77 + 1) {
			fRec7[l77] = 0.0f;
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
	
	virtual synth_harpsichord* clone() {
		return new synth_harpsichord();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("synth.harpsichord");
		ui_interface->declare(&fCheckbox0, "type", "float");
		ui_interface->addCheckButton("gate", &fCheckbox0);
		ui_interface->declare(&fHslider5, "tooltip", "Frequency of the sine wave for the modulation of theta (works if Modulation Type=3)");
		ui_interface->declare(&fHslider5, "unit", "Hz");
		ui_interface->addHorizontalSlider("modfreq", &fHslider5, FAUSTFLOAT(2.2e+02f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(1e+03f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fEntry0, "tooltip", "0=theta is modulated by the incoming signal; 1=theta is modulated by the averaged incoming signal; 2=theta is modulated by the squared incoming signal; 3=theta is modulated by a sine wave of frequency freqMod; 4=theta is modulated by a sine wave of frequency freq;");
		ui_interface->declare(&fEntry0, "type", "int");
		ui_interface->addNumEntry("modtype", &fEntry0, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(4.0f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider4, "tooltip", "Nonlinearity factor (value between 0 and 1)");
		ui_interface->addHorizontalSlider("nonlin", &fHslider4, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("pan", &fHslider1, FAUSTFLOAT(0.6f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("pitch", &fHslider3, FAUSTFLOAT(48.0f), FAUSTFLOAT(12.0f), FAUSTFLOAT(96.0f), FAUSTFLOAT(0.001f));
		ui_interface->addHorizontalSlider("reverb_gain", &fHslider2, FAUSTFLOAT(0.137f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("room_size", &fHslider0, FAUSTFLOAT(0.36f), FAUSTFLOAT(0.005f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->addHorizontalSlider("width", &fHslider6, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = float(fHslider0);
		float fSlow1 = std::exp(fConst3 / fSlow0);
		float fSlow2 = synth_harpsichord_faustpower2_f(fSlow1);
		float fSlow3 = 1.0f - fConst4 * fSlow2;
		float fSlow4 = 1.0f - fSlow2;
		float fSlow5 = std::sqrt(std::max<float>(0.0f, synth_harpsichord_faustpower2_f(fSlow3) / synth_harpsichord_faustpower2_f(fSlow4) + -1.0f));
		float fSlow6 = fSlow3 / fSlow4;
		float fSlow7 = fSlow1 * (fSlow5 + (1.0f - fSlow6));
		float fSlow8 = std::exp(fConst5 / fSlow0) / fSlow1 + -1.0f;
		float fSlow9 = fSlow6 - fSlow5;
		float fSlow10 = float(fHslider1);
		float fSlow11 = 4.0f * fSlow10;
		float fSlow12 = fConst11 * float(fHslider2);
		float fSlow13 = std::pow(2.0f, 0.083333336f * (float(fHslider3) + -69.0f));
		float fSlow14 = 4.4e+02f * fSlow13;
		float fSlow15 = getValueLoopFilterb0(fSlow14);
		float fSlow16 = float(fCheckbox0);
		int iSlow17 = fSlow16 > 0.0f;
		int iSlow18 = iSlow17 < 1;
		float fSlow19 = fConst11 * (0.9996f * float(iSlow17) + 0.9f * float(iSlow18) * getValueReleaseLoopGain(fSlow14));
		int iSlow20 = int(float(fEntry0));
		float fSlow21 = float(iSlow20 >= 3);
		float fSlow22 = fConst11 * float(fHslider4);
		float fSlow23 = 4.4e+02f * fSlow13 * float(iSlow20 == 4);
		float fSlow24 = float(iSlow20 != 4);
		float fSlow25 = fConst11 * float(fHslider5);
		float fSlow26 = float(iSlow20 < 3);
		float fSlow27 = 3.1415927f * float(iSlow20 == 0);
		float fSlow28 = 1.5707964f * float(iSlow20 == 1);
		float fSlow29 = 3.1415927f * float(iSlow20 == 2);
		int iSlow30 = iSlow17 > 0;
		float fSlow31 = fConst14 / getValueDryTapAmpT60(fSlow14);
		float fSlow32 = fConst11 * std::min<float>(1.0f, std::max<float>(0.0f, fSlow16));
		int iSlow33 = int(std::min<float>(4096.0f, std::max<float>(0.0f, fConst15 / fSlow13)));
		float fSlow34 = getValueLoopFiltera1(fSlow14);
		float fSlow35 = getValueLoopFiltera2(fSlow14);
		float fSlow36 = getValueLoopFilterb1(fSlow14);
		float fSlow37 = getValueLoopFilterb2(fSlow14);
		int iSlow38 = int(std::min<float>(4096.0f, std::max<float>(0.0f, fConst20 * (float(fHslider6) / fSlow13))));
		float fSlow39 = std::exp(fConst25 / fSlow0);
		float fSlow40 = synth_harpsichord_faustpower2_f(fSlow39);
		float fSlow41 = 1.0f - fConst4 * fSlow40;
		float fSlow42 = 1.0f - fSlow40;
		float fSlow43 = std::sqrt(std::max<float>(0.0f, synth_harpsichord_faustpower2_f(fSlow41) / synth_harpsichord_faustpower2_f(fSlow42) + -1.0f));
		float fSlow44 = fSlow41 / fSlow42;
		float fSlow45 = fSlow39 * (fSlow43 + (1.0f - fSlow44));
		float fSlow46 = std::exp(fConst26 / fSlow0) / fSlow39 + -1.0f;
		float fSlow47 = fSlow44 - fSlow43;
		float fSlow48 = std::exp(fConst32 / fSlow0);
		float fSlow49 = synth_harpsichord_faustpower2_f(fSlow48);
		float fSlow50 = 1.0f - fConst4 * fSlow49;
		float fSlow51 = 1.0f - fSlow49;
		float fSlow52 = std::sqrt(std::max<float>(0.0f, synth_harpsichord_faustpower2_f(fSlow50) / synth_harpsichord_faustpower2_f(fSlow51) + -1.0f));
		float fSlow53 = fSlow50 / fSlow51;
		float fSlow54 = fSlow48 * (fSlow52 + (1.0f - fSlow53));
		float fSlow55 = std::exp(fConst33 / fSlow0) / fSlow48 + -1.0f;
		float fSlow56 = fSlow53 - fSlow52;
		float fSlow57 = 4.0f * (1.0f - fSlow10);
		float fSlow58 = std::exp(fConst39 / fSlow0);
		float fSlow59 = synth_harpsichord_faustpower2_f(fSlow58);
		float fSlow60 = 1.0f - fConst4 * fSlow59;
		float fSlow61 = 1.0f - fSlow59;
		float fSlow62 = std::sqrt(std::max<float>(0.0f, synth_harpsichord_faustpower2_f(fSlow60) / synth_harpsichord_faustpower2_f(fSlow61) + -1.0f));
		float fSlow63 = fSlow60 / fSlow61;
		float fSlow64 = fSlow58 * (fSlow62 + (1.0f - fSlow63));
		float fSlow65 = std::exp(fConst40 / fSlow0) / fSlow58 + -1.0f;
		float fSlow66 = fSlow63 - fSlow62;
		float fSlow67 = std::exp(fConst46 / fSlow0);
		float fSlow68 = synth_harpsichord_faustpower2_f(fSlow67);
		float fSlow69 = 1.0f - fConst4 * fSlow68;
		float fSlow70 = 1.0f - fSlow68;
		float fSlow71 = std::sqrt(std::max<float>(0.0f, synth_harpsichord_faustpower2_f(fSlow69) / synth_harpsichord_faustpower2_f(fSlow70) + -1.0f));
		float fSlow72 = fSlow69 / fSlow70;
		float fSlow73 = fSlow67 * (fSlow71 + (1.0f - fSlow72));
		float fSlow74 = std::exp(fConst47 / fSlow0) / fSlow67 + -1.0f;
		float fSlow75 = fSlow72 - fSlow71;
		float fSlow76 = std::exp(fConst53 / fSlow0);
		float fSlow77 = synth_harpsichord_faustpower2_f(fSlow76);
		float fSlow78 = 1.0f - fConst4 * fSlow77;
		float fSlow79 = 1.0f - fSlow77;
		float fSlow80 = std::sqrt(std::max<float>(0.0f, synth_harpsichord_faustpower2_f(fSlow78) / synth_harpsichord_faustpower2_f(fSlow79) + -1.0f));
		float fSlow81 = fSlow78 / fSlow79;
		float fSlow82 = fSlow76 * (fSlow80 + (1.0f - fSlow81));
		float fSlow83 = std::exp(fConst54 / fSlow0) / fSlow76 + -1.0f;
		float fSlow84 = fSlow81 - fSlow80;
		float fSlow85 = std::exp(fConst60 / fSlow0);
		float fSlow86 = synth_harpsichord_faustpower2_f(fSlow85);
		float fSlow87 = 1.0f - fConst4 * fSlow86;
		float fSlow88 = 1.0f - fSlow86;
		float fSlow89 = std::sqrt(std::max<float>(0.0f, synth_harpsichord_faustpower2_f(fSlow87) / synth_harpsichord_faustpower2_f(fSlow88) + -1.0f));
		float fSlow90 = fSlow87 / fSlow88;
		float fSlow91 = fSlow85 * (fSlow89 + (1.0f - fSlow90));
		float fSlow92 = std::exp(fConst61 / fSlow0) / fSlow85 + -1.0f;
		float fSlow93 = fSlow90 - fSlow89;
		float fSlow94 = std::exp(fConst67 / fSlow0);
		float fSlow95 = synth_harpsichord_faustpower2_f(fSlow94);
		float fSlow96 = 1.0f - fConst4 * fSlow95;
		float fSlow97 = 1.0f - fSlow95;
		float fSlow98 = std::sqrt(std::max<float>(0.0f, synth_harpsichord_faustpower2_f(fSlow96) / synth_harpsichord_faustpower2_f(fSlow97) + -1.0f));
		float fSlow99 = fSlow96 / fSlow97;
		float fSlow100 = fSlow94 * (fSlow98 + (1.0f - fSlow99));
		float fSlow101 = std::exp(fConst68 / fSlow0) / fSlow94 + -1.0f;
		float fSlow102 = fSlow99 - fSlow98;
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			fRec11[0] = 0.0f - fConst7 * (fConst8 * fRec11[1] - (fRec7[1] + fRec7[2]));
			fRec10[0] = fSlow7 * (fRec7[1] + fSlow8 * fRec11[0]) + fSlow9 * fRec10[1];
			fVec0[IOTA0 & 32767] = 0.35355338f * fRec10[0] + 1e-20f;
			fRec12[0] = fSlow12 + fConst12 * fRec12[1];
			fRec15[0] = fSlow19 + fConst12 * fRec15[1];
			float fTemp0 = fRec13[(IOTA0 - 1) & 8191];
			fRec16[0] = fSlow22 + fConst12 * fRec16[1];
			fRec19[0] = fSlow25 + fConst12 * fRec19[1];
			float fTemp1 = fRec18[1] + fConst13 * (fSlow23 + fSlow24 * fRec19[0]);
			fRec18[0] = fTemp1 - std::floor(fTemp1);
			float fTemp2 = 3.1415927f * fRec16[0] * ftbl0synth_harpsichordSIG0[int(65536.0f * fRec18[0])];
			float fTemp3 = std::sin(fTemp2);
			float fTemp4 = std::cos(fTemp2);
			float fTemp5 = fTemp0 * fTemp4 - fTemp3 * fRec20[1];
			float fTemp6 = fTemp4 * fTemp5 - fTemp3 * fRec21[1];
			float fTemp7 = fTemp4 * fTemp6 - fTemp3 * fRec22[1];
			float fTemp8 = fTemp4 * fTemp7 - fTemp3 * fRec23[1];
			float fTemp9 = fTemp4 * fTemp8 - fTemp3 * fRec24[1];
			fRec25[0] = fTemp4 * fTemp9 - fTemp3 * fRec25[1];
			fRec24[0] = fTemp3 * fTemp9 + fTemp4 * fRec25[1];
			fRec23[0] = fTemp3 * fTemp8 + fTemp4 * fRec24[1];
			fRec22[0] = fTemp3 * fTemp7 + fTemp4 * fRec23[1];
			fRec21[0] = fTemp3 * fTemp6 + fTemp4 * fRec22[1];
			fRec20[0] = fTemp3 * fTemp5 + fTemp4 * fRec21[1];
			float fTemp10 = fRec16[0] * (fSlow27 * fTemp0 + fSlow28 * (fTemp0 + fRec13[(IOTA0 - 2) & 8191]) + fSlow29 * synth_harpsichord_faustpower2_f(fTemp0));
			float fTemp11 = std::sin(fTemp10);
			float fTemp12 = std::cos(fTemp10);
			float fTemp13 = fTemp0 * fTemp12 - fTemp11 * fRec26[1];
			float fTemp14 = fTemp12 * fTemp13 - fTemp11 * fRec27[1];
			float fTemp15 = fTemp12 * fTemp14 - fTemp11 * fRec28[1];
			float fTemp16 = fTemp12 * fTemp15 - fTemp11 * fRec29[1];
			float fTemp17 = fTemp12 * fTemp16 - fTemp11 * fRec30[1];
			fRec31[0] = fTemp12 * fTemp17 - fTemp11 * fRec31[1];
			fRec30[0] = fTemp11 * fTemp17 + fTemp12 * fRec31[1];
			fRec29[0] = fTemp11 * fTemp16 + fTemp12 * fRec30[1];
			fRec28[0] = fTemp11 * fTemp15 + fTemp12 * fRec29[1];
			fRec27[0] = fTemp11 * fTemp14 + fTemp12 * fRec28[1];
			fRec26[0] = fTemp11 * fTemp13 + fTemp12 * fRec27[1];
			iRec33[0] = iSlow17 * iRec33[1] + 1;
			float fTemp18 = float(iRec33[0] + -1);
			float fTemp19 = float((fTemp18 < 2.0f) & iSlow30);
			fRec34[0] = fSlow32 + fConst12 * fRec34[1];
			float fTemp20 = 0.030197384f * fTemp19 + float((fTemp18 >= 2.0f) | iSlow18) * std::exp(0.0f - fSlow31 / fRec34[0]);
			fRec32[0] = fRec32[1] * fTemp20 + 0.15f * fTemp19 * (1.0f - fTemp20);
			iRec35[0] = 1103515245 * iRec35[1] + 12345;
			fVec2[IOTA0 & 8191] = fRec15[0] * (fSlow21 * (fTemp0 * fTemp3 + fRec20[1] * fTemp4) + fSlow26 * (fRec16[0] * (fTemp0 * fTemp11 + fRec26[1] * fTemp12) + (1.0f - fRec16[0]) * fTemp0)) + 4.656613e-10f * fRec32[0] * float(iRec35[0]);
			fRec14[0] = fVec2[(IOTA0 - iSlow33) & 8191] - (fSlow34 * fRec14[1] + fSlow35 * fRec14[2]);
			float fTemp21 = fSlow15 * fRec14[0] + fSlow36 * fRec14[1] + fSlow37 * fRec14[2];
			float fTemp22 = std::fabs(fTemp21);
			float fTemp23 = ((fTemp22 > fRec37[1]) ? fConst19 : fConst18);
			fRec37[0] = fTemp22 * (1.0f - fTemp23) + fRec37[1] * fTemp23;
			fRec36[0] = fConst17 * (0.0f - 0.5f * std::max<float>(2e+01f * std::log10(std::max<float>(1.1754944e-38f, fRec37[0])) + -1e+02f, 0.0f)) + fConst16 * fRec36[1];
			fRec13[IOTA0 & 8191] = fTemp21 * std::pow(1e+01f, 0.05f * fRec36[0]);
			float fTemp24 = fRec13[(IOTA0 - iSlow38) & 8191];
			fVec3[IOTA0 & 4095] = fSlow11 * fRec12[0] * fTemp24;
			float fTemp25 = 0.3f * fVec3[(IOTA0 - iConst21) & 4095];
			float fTemp26 = 0.6f * fRec8[1] + fVec0[(IOTA0 - iConst10) & 32767] - fTemp25;
			fVec4[IOTA0 & 2047] = fTemp26;
			fRec8[0] = fVec4[(IOTA0 - iConst22) & 2047];
			float fRec9 = 0.0f - 0.6f * fTemp26;
			fRec41[0] = 0.0f - fConst7 * (fConst8 * fRec41[1] - (fRec1[1] + fRec1[2]));
			fRec40[0] = fSlow45 * (fRec1[1] + fSlow46 * fRec41[0]) + fSlow47 * fRec40[1];
			fVec5[IOTA0 & 32767] = 0.35355338f * fRec40[0] + 1e-20f;
			float fTemp27 = fVec5[(IOTA0 - iConst28) & 32767] + 0.6f * fRec38[1] + fTemp25;
			fVec6[IOTA0 & 4095] = fTemp27;
			fRec38[0] = fVec6[(IOTA0 - iConst29) & 4095];
			float fRec39 = 0.0f - 0.6f * fTemp27;
			fRec45[0] = 0.0f - fConst7 * (fConst8 * fRec45[1] - (fRec6[1] + fRec6[2]));
			fRec44[0] = fSlow54 * (fRec6[1] + fSlow55 * fRec45[0]) + fSlow56 * fRec44[1];
			fVec7[IOTA0 & 16383] = 0.35355338f * fRec44[0] + 1e-20f;
			float fTemp28 = fRec13[IOTA0 & 8191];
			fVec8[IOTA0 & 4095] = fSlow57 * fTemp28 * fRec12[0];
			float fTemp29 = 0.3f * fVec8[(IOTA0 - iConst21) & 4095];
			float fTemp30 = fVec7[(IOTA0 - iConst35) & 16383] - (fTemp29 + 0.6f * fRec42[1]);
			fVec9[IOTA0 & 2047] = fTemp30;
			fRec42[0] = fVec9[(IOTA0 - iConst36) & 2047];
			float fRec43 = 0.6f * fTemp30;
			fRec49[0] = 0.0f - fConst7 * (fConst8 * fRec49[1] - (fRec3[1] + fRec3[2]));
			fRec48[0] = fSlow64 * (fRec3[1] + fSlow65 * fRec49[0]) + fSlow66 * fRec48[1];
			fVec10[IOTA0 & 32767] = 0.35355338f * fRec48[0] + 1e-20f;
			float fTemp31 = 0.6f * fRec46[1] + fVec10[(IOTA0 - iConst42) & 32767] - fTemp25;
			fVec11[IOTA0 & 4095] = fTemp31;
			fRec46[0] = fVec11[(IOTA0 - iConst43) & 4095];
			float fRec47 = 0.0f - 0.6f * fTemp31;
			fRec53[0] = 0.0f - fConst7 * (fConst8 * fRec53[1] - (fRec5[1] + fRec5[2]));
			fRec52[0] = fSlow73 * (fRec5[1] + fSlow74 * fRec53[0]) + fSlow75 * fRec52[1];
			fVec12[IOTA0 & 16383] = 0.35355338f * fRec52[0] + 1e-20f;
			float fTemp32 = fVec12[(IOTA0 - iConst49) & 16383] + fTemp25 + 0.6f * fRec50[1];
			fVec13[IOTA0 & 4095] = fTemp32;
			fRec50[0] = fVec13[(IOTA0 - iConst50) & 4095];
			float fRec51 = 0.0f - 0.6f * fTemp32;
			fRec57[0] = 0.0f - fConst7 * (fConst8 * fRec57[1] - (fRec0[1] + fRec0[2]));
			fRec56[0] = fSlow82 * (fRec0[1] + fSlow83 * fRec57[0]) + fSlow84 * fRec56[1];
			fVec14[IOTA0 & 16383] = 0.35355338f * fRec56[0] + 1e-20f;
			float fTemp33 = fVec14[(IOTA0 - iConst56) & 16383] + fTemp29 - 0.6f * fRec54[1];
			fVec15[IOTA0 & 2047] = fTemp33;
			fRec54[0] = fVec15[(IOTA0 - iConst57) & 2047];
			float fRec55 = 0.6f * fTemp33;
			fRec61[0] = 0.0f - fConst7 * (fConst8 * fRec61[1] - (fRec2[1] + fRec2[2]));
			fRec60[0] = fSlow91 * (fRec2[1] + fSlow92 * fRec61[0]) + fSlow93 * fRec60[1];
			fVec16[IOTA0 & 16383] = 0.35355338f * fRec60[0] + 1e-20f;
			float fTemp34 = fVec16[(IOTA0 - iConst63) & 16383] - (fTemp29 + 0.6f * fRec58[1]);
			fVec17[IOTA0 & 4095] = fTemp34;
			fRec58[0] = fVec17[(IOTA0 - iConst64) & 4095];
			float fRec59 = 0.6f * fTemp34;
			fRec65[0] = 0.0f - fConst7 * (fConst8 * fRec65[1] - (fRec4[1] + fRec4[2]));
			fRec64[0] = fSlow100 * (fRec4[1] + fSlow101 * fRec65[0]) + fSlow102 * fRec64[1];
			fVec18[IOTA0 & 16383] = 0.35355338f * fRec64[0] + 1e-20f;
			float fTemp35 = fTemp29 + fVec18[(IOTA0 - iConst70) & 16383] - 0.6f * fRec62[1];
			fVec19[IOTA0 & 4095] = fTemp35;
			fRec62[0] = fVec19[(IOTA0 - iConst71) & 4095];
			float fRec63 = 0.6f * fTemp35;
			float fTemp36 = fRec54[1] + fRec62[1];
			float fTemp37 = fRec43 + fRec59 + fRec63 + fTemp36 + fRec58[1];
			fRec0[0] = fRec8[1] + fRec38[1] + fRec42[1] + fRec46[1] + fRec50[1] + fRec9 + fRec47 + fRec51 + fRec55 + fRec39 + fTemp37;
			fRec1[0] = fRec42[1] + fRec55 + fTemp37 - (fRec8[1] + fRec38[1] + fRec46[1] + fRec50[1] + fRec9 + fRec47 + fRec39 + fRec51);
			float fTemp38 = fRec63 + fTemp36;
			float fTemp39 = fRec43 + fRec59 + fRec58[1];
			fRec2[0] = fRec38[1] + fRec50[1] + fRec51 + fRec55 + fRec39 + fTemp38 - (fRec8[1] + fRec42[1] + fRec46[1] + fRec9 + fRec47 + fTemp39);
			fRec3[0] = fRec8[1] + fRec46[1] + fRec9 + fRec47 + fRec55 + fTemp38 - (fRec38[1] + fRec42[1] + fRec50[1] + fRec51 + fRec39 + fTemp39);
			float fTemp40 = fRec59 + fRec54[1] + fRec58[1];
			float fTemp41 = fRec43 + fRec63 + fRec62[1];
			fRec4[0] = fRec38[1] + fRec46[1] + fRec47 + fRec55 + fRec39 + fTemp40 - (fRec8[1] + fRec42[1] + fRec50[1] + fRec9 + fRec51 + fTemp41);
			fRec5[0] = fRec8[1] + fRec50[1] + fRec9 + fRec51 + fRec55 + fTemp40 - (fRec38[1] + fRec42[1] + fRec46[1] + fRec47 + fRec39 + fTemp41);
			float fTemp42 = fRec43 + fRec54[1];
			float fTemp43 = fRec59 + fRec63 + fRec62[1] + fRec58[1];
			fRec6[0] = fRec8[1] + fRec38[1] + fRec42[1] + fRec9 + fRec55 + fRec39 + fTemp42 - (fRec46[1] + fRec50[1] + fRec47 + fRec51 + fTemp43);
			fRec7[0] = fRec42[1] + fRec46[1] + fRec50[1] + fRec47 + fRec51 + fRec55 + fTemp42 - (fRec8[1] + fRec38[1] + fRec9 + fRec39 + fTemp43);
			float fTemp44 = 1.0f - fRec12[0];
			output0[i0] = FAUSTFLOAT(0.37f * (fRec1[0] + fRec2[0]) + fSlow57 * fTemp28 * fTemp44);
			output1[i0] = FAUSTFLOAT(0.37f * (fRec1[0] - fRec2[0]) + fSlow11 * fTemp44 * fTemp24);
			fRec11[1] = fRec11[0];
			fRec10[1] = fRec10[0];
			IOTA0 = IOTA0 + 1;
			fRec12[1] = fRec12[0];
			fRec15[1] = fRec15[0];
			fRec16[1] = fRec16[0];
			fRec19[1] = fRec19[0];
			fRec18[1] = fRec18[0];
			fRec25[1] = fRec25[0];
			fRec24[1] = fRec24[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec26[1] = fRec26[0];
			iRec33[1] = iRec33[0];
			fRec34[1] = fRec34[0];
			fRec32[1] = fRec32[0];
			iRec35[1] = iRec35[0];
			fRec14[2] = fRec14[1];
			fRec14[1] = fRec14[0];
			fRec37[1] = fRec37[0];
			fRec36[1] = fRec36[0];
			fRec8[1] = fRec8[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec38[1] = fRec38[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec42[1] = fRec42[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec46[1] = fRec46[0];
			fRec53[1] = fRec53[0];
			fRec52[1] = fRec52[0];
			fRec50[1] = fRec50[0];
			fRec57[1] = fRec57[0];
			fRec56[1] = fRec56[0];
			fRec54[1] = fRec54[0];
			fRec61[1] = fRec61[0];
			fRec60[1] = fRec60[0];
			fRec58[1] = fRec58[0];
			fRec65[1] = fRec65[0];
			fRec64[1] = fRec64[0];
			fRec62[1] = fRec62[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			fRec2[2] = fRec2[1];
			fRec2[1] = fRec2[0];
			fRec3[2] = fRec3[1];
			fRec3[1] = fRec3[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
		}
	}

};
#endif

class faust_synth_harpsichord_tilde : public FaustExternal<synth_harpsichord> {
public:
    faust_synth_harpsichord_tilde(const ceammc::PdArgs& args)
        : FaustExternal(args, sym(synth_harpsichord))
    {
    }
};

#endif
