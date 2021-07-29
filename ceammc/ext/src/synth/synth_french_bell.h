/* ------------------------------------------------------------
name: "synth.french_bell"
Code generated with Faust 2.30.12 (https://faust.grame.fr)
Compilation options: -lang cpp -es 1 -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __synth_french_bell_H__
#define  __synth_french_bell_H__

// FAUST Architecture File for ceammc::SoundExternal class
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

/************************** BEGIN synth_french_bell_dsp.h **************************/
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

class synth_french_bell_dsp {

    public:

        synth_french_bell_dsp() {}
        virtual ~synth_french_bell_dsp() {}

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
        virtual synth_french_bell_dsp* clone() = 0;
    
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

class decorator_dsp : public synth_french_bell_dsp {

    protected:

        synth_french_bell_dsp* fDSP;

    public:

        decorator_dsp(synth_french_bell_dsp* synth_french_bell_dsp = nullptr):fDSP(synth_french_bell_dsp) {}
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
    
        virtual synth_french_bell_dsp* createDSPInstance() = 0;
    
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
/**************************  END  synth_french_bell_dsp.h **************************/
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
 The base class of Meta handler to be used in synth_french_bell_dsp::metadata(Meta* m) method to retrieve (key, value) metadata.
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
struct synth_french_bell : public synth_french_bell_dsp {
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

const static float fsynth_french_bellSIG0Wave0[350] = {0.972890019f,0.542393029f,0.495831996f,0.897966027f,0.552366972f,0.557895005f,0.61421299f,0.353693992f,0.436039001f,0.441024005f,0.674912989f,0.566753983f,0.755007982f,0.699029982f,0.164398f,0.910040021f,0.628373027f,0.201718003f,0.429517001f,0.503714979f,0.871173978f,0.106885999f,0.76117301f,0.673601985f,0.291936994f,0.588590026f,0.31527999f,0.41308099f,0.274464011f,0.494062006f,0.696120977f,0.612010002f,0.382757008f,0.995113015f,0.228806004f,0.198449001f,0.595847011f,0.306263f,0.252397001f,0.0883567035f,0.236085996f,1.0f,0.245278001f,0.379388005f,0.198824003f,0.548892021f,0.492763996f,0.42087099f,0.794637024f,0.605633974f,1.0f,0.604158998f,0.399841011f,0.799346983f,0.507187009f,0.509809971f,0.477382988f,0.310225993f,0.426975995f,0.437622994f,0.735711992f,0.630728006f,0.625784993f,0.651167989f,0.277738005f,0.850857973f,0.578078985f,0.645353973f,0.383700013f,0.370597988f,0.78254199f,0.181325004f,0.614391029f,0.740683973f,0.342440993f,0.586185992f,0.286909014f,0.405196995f,0.259214997f,0.566982985f,0.748219013f,0.655897975f,0.368259996f,0.940814018f,0.336156994f,0.413702011f,0.561556995f,0.402175993f,0.117697999f,0.329369009f,0.254570991f,0.870706022f,0.260980994f,0.274122f,0.206247002f,0.645299971f,0.400757998f,0.36362201f,0.636834025f,0.584565997f,0.975602984f,0.616936982f,0.295401007f,0.650447011f,0.464221001f,0.465056986f,0.312467009f,0.238358006f,0.383695006f,0.399673998f,0.753911972f,0.662011981f,0.504267991f,0.599247992f,0.378665f,0.725363016f,0.493214011f,1.0f,0.332836002f,0.265888989f,0.674072981f,0.320401013f,0.440017998f,0.769782007f,0.316419005f,0.529968023f,0.380508989f,0.578675985f,0.249013007f,0.591383994f,0.761717021f,0.687057018f,0.324436992f,0.818863988f,0.505369008f,0.672484994f,0.461782992f,0.426198006f,0.0678874999f,0.43532899f,0.347954988f,0.708393991f,0.293321997f,0.328985989f,0.258253992f,0.893512011f,0.320131004f,0.433553994f,0.459302008f,0.542213023f,0.817241013f,0.516229987f,0.205302f,0.467354f,0.388682991f,0.388215989f,0.171261996f,0.150865003f,0.291720003f,0.311414003f,0.658876002f,0.570647001f,0.38361901f,0.502454996f,0.364113986f,0.532312989f,0.352988988f,1.0f,0.261983991f,0.219591007f,0.544031024f,0.337199003f,0.279172987f,0.668303013f,0.208439007f,0.399230003f,0.418673992f,0.648617983f,0.234133005f,0.504728973f,0.645346999f,0.572851002f,0.232828006f,0.614292026f,0.48527199f,0.666263998f,0.316570014f,0.320354998f,0.191421002f,0.340131015f,0.342069f,0.538371027f,0.281130999f,0.393115014f,0.251394004f,0.890725017f,0.310644001f,0.503700018f,0.299089998f,0.442478001f,0.733128011f,0.455217004f,0.199322f,0.315699011f,0.375856012f,0.376489997f,0.0291450005f,0.0200283006f,0.279578f,0.316799998f,0.655956984f,0.546842992f,0.349665999f,0.470248997f,0.353765011f,0.286794007f,0.180185005f,1.0f,0.210831001f,0.280133009f,0.535853028f,0.376488f,0.153669998f,0.634745002f,0.0510449f,0.485574991f,0.593110979f,0.917883992f,0.380477011f,0.422924995f,0.599372983f,0.311421007f,0.135654002f,0.359954f,0.295747995f,0.474438995f,0.353338987f,0.116742998f,0.45431301f,0.112857997f,0.359310001f,0.483897001f,0.301607996f,0.577341974f,0.262663007f,0.79498601f,0.549579978f,0.808085978f,0.152511001f,0.439590991f,0.535941005f,0.308017999f,0.419836998f,0.579191029f,0.250427991f,0.252119988f,0.102860004f,0.288331985f,0.599678993f,0.665108025f,0.636285007f,0.495234013f,0.251612991f,0.208526999f,0.0939071998f,0.458415002f,0.318951994f,0.235349998f,0.215188995f,0.313412011f,0.0472786985f,0.0386893004f,0.0863358974f,0.222345993f,0.361510992f,0.997036994f,0.238508999f,0.382140011f,1.0f,0.203554004f,0.472086996f,0.509015024f,0.206370994f,0.441013008f,0.794008017f,0.971623003f,0.796383977f,0.55043f,0.687780023f,0.554297984f,0.0436545014f,0.0595576987f,0.214684993f,0.581470013f,0.277319998f,0.36846599f,0.121381998f,0.152739003f,0.0782243982f,0.190496996f,0.31626901f,0.180455998f,0.405196011f,0.518715978f,0.159364998f,0.165808007f,0.194371998f,0.614763975f,0.553415f,0.644793987f,0.441780001f,0.441489995f,0.221588001f,1.0f,0.579370975f,0.606338978f,0.529166996f,0.0214475002f,0.41704601f,0.663151979f,0.894841015f,0.475751996f,0.168289006f,0.468589991f,0.304603994f,0.666369021f,0.308337003f,0.295091003f,0.768944979f,0.350910991f,0.490680009f,0.516583979f,0.40011701f,0.820686996f,0.545485973f,0.709357023f,0.509759009f,0.472005993f,0.152455002f,0.981265008f,0.459134012f,0.698665977f,0.247153997f,0.623252988f,0.255508006f,0.830815017f,0.0887089968f,0.126082003f,0.0770623013f,0.263327986f};
class synth_french_bellSIG0 {
	
  private:
	
	int fsynth_french_bellSIG0Wave0_idx;
	
  public:
	
	int getNumInputssynth_french_bellSIG0() {
		return 0;
	}
	int getNumOutputssynth_french_bellSIG0() {
		return 1;
	}
	int getInputRatesynth_french_bellSIG0(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	int getOutputRatesynth_french_bellSIG0(int channel) {
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
	
	void instanceInitsynth_french_bellSIG0(int sample_rate) {
		fsynth_french_bellSIG0Wave0_idx = 0;
	}
	
	void fillsynth_french_bellSIG0(int count, float* table) {
		for (int i = 0; (i < count); i = (i + 1)) {
			table[i] = fsynth_french_bellSIG0Wave0[fsynth_french_bellSIG0Wave0_idx];
			fsynth_french_bellSIG0Wave0_idx = ((1 + fsynth_french_bellSIG0Wave0_idx) % 350);
		}
	}

};

static synth_french_bellSIG0* newsynth_french_bellSIG0() { return (synth_french_bellSIG0*)new synth_french_bellSIG0(); }
static void deletesynth_french_bellSIG0(synth_french_bellSIG0* dsp) { delete dsp; }

static float synth_french_bell_faustpower2_f(float value) {
	return (value * value);
}
static float ftbl0synth_french_bellSIG0[350];

#ifndef FAUSTCLASS 
#define FAUSTCLASS synth_french_bell
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class synth_french_bell : public synth_french_bell_dsp {
	
 private:
	
	FAUSTFLOAT fHslider0;
	int fSampleRate;
	float fConst1;
	FAUSTFLOAT fHslider1;
	float fConst4;
	float fConst6;
	int iRec3[2];
	float fConst7;
	float fConst8;
	float fRec2[3];
	float fConst9;
	float fRec1[3];
	float fConst10;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fButton0;
	float fVec0[2];
	int iRec4[2];
	float fConst11;
	float fConst12;
	FAUSTFLOAT fHslider3;
	FAUSTFLOAT fHslider4;
	FAUSTFLOAT fHslider5;
	float fRec0[3];
	FAUSTFLOAT fEntry0;
	float fConst13;
	float fRec5[3];
	float fConst14;
	float fRec6[3];
	float fConst15;
	float fRec7[3];
	float fConst16;
	float fRec8[3];
	float fConst17;
	float fRec9[3];
	float fConst18;
	float fRec10[3];
	float fConst19;
	float fRec11[3];
	float fConst20;
	float fRec12[3];
	float fConst21;
	float fRec13[3];
	float fConst22;
	float fRec14[3];
	float fConst23;
	float fRec15[3];
	float fConst24;
	float fRec16[3];
	float fConst25;
	float fRec17[3];
	float fConst26;
	float fRec18[3];
	float fConst27;
	float fRec19[3];
	float fConst28;
	float fRec20[3];
	float fConst29;
	float fRec21[3];
	float fConst30;
	float fRec22[3];
	float fConst31;
	float fRec23[3];
	float fConst32;
	float fRec24[3];
	float fConst33;
	float fRec25[3];
	float fConst34;
	float fRec26[3];
	float fConst35;
	float fRec27[3];
	float fConst36;
	float fRec28[3];
	float fConst37;
	float fRec29[3];
	float fConst38;
	float fRec30[3];
	float fConst39;
	float fRec31[3];
	float fConst40;
	float fRec32[3];
	float fConst41;
	float fRec33[3];
	float fConst42;
	float fRec34[3];
	float fConst43;
	float fRec35[3];
	float fConst44;
	float fRec36[3];
	float fConst45;
	float fRec37[3];
	float fConst46;
	float fRec38[3];
	float fConst47;
	float fRec39[3];
	float fConst48;
	float fRec40[3];
	float fConst49;
	float fRec41[3];
	float fConst50;
	float fRec42[3];
	float fConst51;
	float fRec43[3];
	float fConst52;
	float fRec44[3];
	float fConst53;
	float fRec45[3];
	float fConst54;
	float fRec46[3];
	float fConst55;
	float fRec47[3];
	float fConst56;
	float fRec48[3];
	float fConst57;
	float fRec49[3];
	float fConst58;
	float fRec50[3];
	float fConst59;
	float fRec51[3];
	float fConst60;
	float fRec52[3];
	float fConst61;
	float fRec53[3];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("compile_options", "-lang cpp -es 1 -scal -ftz 0");
		m->declare("envelopes.lib/ar:author", "Yann Orlarey, Stéphane Letz");
		m->declare("envelopes.lib/author", "GRAME");
		m->declare("envelopes.lib/copyright", "GRAME");
		m->declare("envelopes.lib/license", "LGPL with exception");
		m->declare("envelopes.lib/name", "Faust Envelope Library");
		m->declare("envelopes.lib/version", "0.1");
		m->declare("filename", "synth_french_bell.dsp");
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
		m->declare("filters.lib/version", "0.3");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.3");
		m->declare("name", "synth.french_bell");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "0.0");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.1");
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
		synth_french_bellSIG0* sig0 = newsynth_french_bellSIG0();
		sig0->instanceInitsynth_french_bellSIG0(sample_rate);
		sig0->fillsynth_french_bellSIG0(350, ftbl0synth_french_bellSIG0);
		deletesynth_french_bellSIG0(sig0);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		float fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = (3.14159274f / fConst0);
		float fConst2 = std::tan((31.415926f / fConst0));
		float fConst3 = (1.0f / fConst2);
		fConst4 = (1.0f / (((fConst3 + 1.41421354f) / fConst2) + 1.0f));
		float fConst5 = synth_french_bell_faustpower2_f(fConst2);
		fConst6 = (1.0f / fConst5);
		fConst7 = (((fConst3 + -1.41421354f) / fConst2) + 1.0f);
		fConst8 = (2.0f * (1.0f - fConst6));
		fConst9 = (0.0f - (2.0f / fConst5));
		fConst10 = (0.00200000009f * fConst0);
		fConst11 = std::cos((13060.7949f / fConst0));
		fConst12 = (1.0f / fConst0);
		fConst13 = std::cos((12916.1553f / fConst0));
		fConst14 = std::cos((12909.8096f / fConst0));
		fConst15 = std::cos((12809.7188f / fConst0));
		fConst16 = std::cos((6750.84277f / fConst0));
		fConst17 = std::cos((6742.04639f / fConst0));
		fConst18 = std::cos((6690.02148f / fConst0));
		fConst19 = std::cos((6687.63379f / fConst0));
		fConst20 = std::cos((5648.90381f / fConst0));
		fConst21 = std::cos((5384.47607f / fConst0));
		fConst22 = std::cos((4460.28857f / fConst0));
		fConst23 = std::cos((4458.65527f / fConst0));
		fConst24 = std::cos((3843.90186f / fConst0));
		fConst25 = std::cos((3810.4502f / fConst0));
		fConst26 = std::cos((11605.9229f / fConst0));
		fConst27 = std::cos((11435.8369f / fConst0));
		fConst28 = std::cos((11365.2773f / fConst0));
		fConst29 = std::cos((11340.0811f / fConst0));
		fConst30 = std::cos((10642.2705f / fConst0));
		fConst31 = std::cos((10626.877f / fConst0));
		fConst32 = std::cos((2758.80225f / fConst0));
		fConst33 = std::cos((2766.51782f / fConst0));
		fConst34 = std::cos((6764.35156f / fConst0));
		fConst35 = std::cos((7196.88623f / fConst0));
		fConst36 = std::cos((7218.75146f / fConst0));
		fConst37 = std::cos((7521.53809f / fConst0));
		fConst38 = std::cos((7539.25684f / fConst0));
		fConst39 = std::cos((8719.17578f / fConst0));
		fConst40 = std::cos((8797.46484f / fConst0));
		fConst41 = std::cos((8811.35059f / fConst0));
		fConst42 = std::cos((9500.55273f / fConst0));
		fConst43 = std::cos((9506.83594f / fConst0));
		fConst44 = std::cos((10005.7842f / fConst0));
		fConst45 = std::cos((10011.3135f / fConst0));
		fConst46 = std::cos((10451.7646f / fConst0));
		fConst47 = std::cos((10470.7402f / fConst0));
		fConst48 = std::cos((12005.4707f / fConst0));
		fConst49 = std::cos((12112.0967f / fConst0));
		fConst50 = std::cos((12786.7217f / fConst0));
		fConst51 = std::cos((13142.916f / fConst0));
		fConst52 = std::cos((13293.8379f / fConst0));
		fConst53 = std::cos((13364.3984f / fConst0));
		fConst54 = std::cos((13884.96f / fConst0));
		fConst55 = std::cos((13911.2861f / fConst0));
		fConst56 = std::cos((14427.1357f / fConst0));
		fConst57 = std::cos((14439.2627f / fConst0));
		fConst58 = std::cos((14444.8545f / fConst0));
		fConst59 = std::cos((14499.2041f / fConst0));
		fConst60 = std::cos((15107.542f / fConst0));
		fConst61 = std::cos((15136.1309f / fConst0));
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(1.0f);
		fHslider1 = FAUSTFLOAT(4000.0f);
		fHslider2 = FAUSTFLOAT(0.25f);
		fButton0 = FAUSTFLOAT(0.0f);
		fHslider3 = FAUSTFLOAT(20.0f);
		fHslider4 = FAUSTFLOAT(1.0f);
		fHslider5 = FAUSTFLOAT(3.0f);
		fEntry0 = FAUSTFLOAT(0.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			iRec3[l0] = 0;
		}
		for (int l1 = 0; (l1 < 3); l1 = (l1 + 1)) {
			fRec2[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 3); l2 = (l2 + 1)) {
			fRec1[l2] = 0.0f;
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fVec0[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			iRec4[l4] = 0;
		}
		for (int l5 = 0; (l5 < 3); l5 = (l5 + 1)) {
			fRec0[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 3); l6 = (l6 + 1)) {
			fRec5[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) {
			fRec6[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 3); l8 = (l8 + 1)) {
			fRec7[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 3); l9 = (l9 + 1)) {
			fRec8[l9] = 0.0f;
		}
		for (int l10 = 0; (l10 < 3); l10 = (l10 + 1)) {
			fRec9[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 3); l11 = (l11 + 1)) {
			fRec10[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 3); l12 = (l12 + 1)) {
			fRec11[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 3); l13 = (l13 + 1)) {
			fRec12[l13] = 0.0f;
		}
		for (int l14 = 0; (l14 < 3); l14 = (l14 + 1)) {
			fRec13[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 3); l15 = (l15 + 1)) {
			fRec14[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 3); l16 = (l16 + 1)) {
			fRec15[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 3); l17 = (l17 + 1)) {
			fRec16[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 3); l18 = (l18 + 1)) {
			fRec17[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 3); l19 = (l19 + 1)) {
			fRec18[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 3); l20 = (l20 + 1)) {
			fRec19[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 3); l21 = (l21 + 1)) {
			fRec20[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 3); l22 = (l22 + 1)) {
			fRec21[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 3); l23 = (l23 + 1)) {
			fRec22[l23] = 0.0f;
		}
		for (int l24 = 0; (l24 < 3); l24 = (l24 + 1)) {
			fRec23[l24] = 0.0f;
		}
		for (int l25 = 0; (l25 < 3); l25 = (l25 + 1)) {
			fRec24[l25] = 0.0f;
		}
		for (int l26 = 0; (l26 < 3); l26 = (l26 + 1)) {
			fRec25[l26] = 0.0f;
		}
		for (int l27 = 0; (l27 < 3); l27 = (l27 + 1)) {
			fRec26[l27] = 0.0f;
		}
		for (int l28 = 0; (l28 < 3); l28 = (l28 + 1)) {
			fRec27[l28] = 0.0f;
		}
		for (int l29 = 0; (l29 < 3); l29 = (l29 + 1)) {
			fRec28[l29] = 0.0f;
		}
		for (int l30 = 0; (l30 < 3); l30 = (l30 + 1)) {
			fRec29[l30] = 0.0f;
		}
		for (int l31 = 0; (l31 < 3); l31 = (l31 + 1)) {
			fRec30[l31] = 0.0f;
		}
		for (int l32 = 0; (l32 < 3); l32 = (l32 + 1)) {
			fRec31[l32] = 0.0f;
		}
		for (int l33 = 0; (l33 < 3); l33 = (l33 + 1)) {
			fRec32[l33] = 0.0f;
		}
		for (int l34 = 0; (l34 < 3); l34 = (l34 + 1)) {
			fRec33[l34] = 0.0f;
		}
		for (int l35 = 0; (l35 < 3); l35 = (l35 + 1)) {
			fRec34[l35] = 0.0f;
		}
		for (int l36 = 0; (l36 < 3); l36 = (l36 + 1)) {
			fRec35[l36] = 0.0f;
		}
		for (int l37 = 0; (l37 < 3); l37 = (l37 + 1)) {
			fRec36[l37] = 0.0f;
		}
		for (int l38 = 0; (l38 < 3); l38 = (l38 + 1)) {
			fRec37[l38] = 0.0f;
		}
		for (int l39 = 0; (l39 < 3); l39 = (l39 + 1)) {
			fRec38[l39] = 0.0f;
		}
		for (int l40 = 0; (l40 < 3); l40 = (l40 + 1)) {
			fRec39[l40] = 0.0f;
		}
		for (int l41 = 0; (l41 < 3); l41 = (l41 + 1)) {
			fRec40[l41] = 0.0f;
		}
		for (int l42 = 0; (l42 < 3); l42 = (l42 + 1)) {
			fRec41[l42] = 0.0f;
		}
		for (int l43 = 0; (l43 < 3); l43 = (l43 + 1)) {
			fRec42[l43] = 0.0f;
		}
		for (int l44 = 0; (l44 < 3); l44 = (l44 + 1)) {
			fRec43[l44] = 0.0f;
		}
		for (int l45 = 0; (l45 < 3); l45 = (l45 + 1)) {
			fRec44[l45] = 0.0f;
		}
		for (int l46 = 0; (l46 < 3); l46 = (l46 + 1)) {
			fRec45[l46] = 0.0f;
		}
		for (int l47 = 0; (l47 < 3); l47 = (l47 + 1)) {
			fRec46[l47] = 0.0f;
		}
		for (int l48 = 0; (l48 < 3); l48 = (l48 + 1)) {
			fRec47[l48] = 0.0f;
		}
		for (int l49 = 0; (l49 < 3); l49 = (l49 + 1)) {
			fRec48[l49] = 0.0f;
		}
		for (int l50 = 0; (l50 < 3); l50 = (l50 + 1)) {
			fRec49[l50] = 0.0f;
		}
		for (int l51 = 0; (l51 < 3); l51 = (l51 + 1)) {
			fRec50[l51] = 0.0f;
		}
		for (int l52 = 0; (l52 < 3); l52 = (l52 + 1)) {
			fRec51[l52] = 0.0f;
		}
		for (int l53 = 0; (l53 < 3); l53 = (l53 + 1)) {
			fRec52[l53] = 0.0f;
		}
		for (int l54 = 0; (l54 < 3); l54 = (l54 + 1)) {
			fRec53[l54] = 0.0f;
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
	
	virtual synth_french_bell* clone() {
		return new synth_french_bell();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("synth.french_bell");
		ui_interface->declare(&fHslider1, "unit", "Hz");
		ui_interface->addHorizontalSlider("cutoff", &fHslider1, 4000.0f, 20.0f, 20000.0f, 1.0f);
		ui_interface->addHorizontalSlider("decay_ratio", &fHslider4, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("decay_slope", &fHslider5, 3.0f, 0.0f, 10.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("gain", &fHslider0, 1.0f, 0.0f, 1.0f, 0.00999999978f);
		ui_interface->addButton("gate", &fButton0);
		ui_interface->declare(&fEntry0, "type", "int");
		ui_interface->addNumEntry("pos", &fEntry0, 0.0f, 0.0f, 6.0f, 1.0f);
		ui_interface->addHorizontalSlider("sharp", &fHslider2, 0.25f, 0.00999999978f, 5.0f, 0.00999999978f);
		ui_interface->declare(&fHslider3, "unit", "sec");
		ui_interface->addHorizontalSlider("t60", &fHslider3, 20.0f, 0.100000001f, 30.0f, 0.00999999978f);
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = std::tan((fConst1 * float(fHslider1)));
		float fSlow1 = (1.0f / fSlow0);
		float fSlow2 = (((fSlow1 + 1.41421354f) / fSlow0) + 1.0f);
		float fSlow3 = (float(fHslider0) / fSlow2);
		float fSlow4 = (1.0f / fSlow2);
		float fSlow5 = (((fSlow1 + -1.41421354f) / fSlow0) + 1.0f);
		float fSlow6 = (2.0f * (1.0f - (1.0f / synth_french_bell_faustpower2_f(fSlow0))));
		float fSlow7 = (1.0f / std::max<float>(1.0f, (fConst10 * float(fHslider2))));
		float fSlow8 = float(fButton0);
		float fSlow9 = float(fHslider3);
		float fSlow10 = float(fHslider4);
		float fSlow11 = float(fHslider5);
		float fSlow12 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.813481808f * fSlow10)), fSlow11))));
		float fSlow13 = (fConst11 * (0.0f - (2.0f * fSlow12)));
		float fSlow14 = synth_french_bell_faustpower2_f(fSlow12);
		int iSlow15 = (50 * int(float(fEntry0)));
		float fSlow16 = ftbl0synth_french_bellSIG0[(iSlow15 + 38)];
		float fSlow17 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.804473042f * fSlow10)), fSlow11))));
		float fSlow18 = (fConst13 * (0.0f - (2.0f * fSlow17)));
		float fSlow19 = synth_french_bell_faustpower2_f(fSlow17);
		float fSlow20 = ftbl0synth_french_bellSIG0[(iSlow15 + 37)];
		float fSlow21 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.804077804f * fSlow10)), fSlow11))));
		float fSlow22 = (fConst14 * (0.0f - (2.0f * fSlow21)));
		float fSlow23 = synth_french_bell_faustpower2_f(fSlow21);
		float fSlow24 = ftbl0synth_french_bellSIG0[(iSlow15 + 36)];
		float fSlow25 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.797843695f * fSlow10)), fSlow11))));
		float fSlow26 = (fConst15 * (0.0f - (2.0f * fSlow25)));
		float fSlow27 = synth_french_bell_faustpower2_f(fSlow25);
		float fSlow28 = ftbl0synth_french_bellSIG0[(iSlow15 + 35)];
		float fSlow29 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.420471191f * fSlow10)), fSlow11))));
		float fSlow30 = (fConst16 * (0.0f - (2.0f * fSlow29)));
		float fSlow31 = synth_french_bell_faustpower2_f(fSlow29);
		float fSlow32 = ftbl0synth_french_bellSIG0[(iSlow15 + 11)];
		float fSlow33 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.419923306f * fSlow10)), fSlow11))));
		float fSlow34 = (fConst17 * (0.0f - (2.0f * fSlow33)));
		float fSlow35 = synth_french_bell_faustpower2_f(fSlow33);
		float fSlow36 = ftbl0synth_french_bellSIG0[(iSlow15 + 10)];
		float fSlow37 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.416682959f * fSlow10)), fSlow11))));
		float fSlow38 = (fConst18 * (0.0f - (2.0f * fSlow37)));
		float fSlow39 = synth_french_bell_faustpower2_f(fSlow37);
		float fSlow40 = ftbl0synth_french_bellSIG0[(iSlow15 + 9)];
		float fSlow41 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.416534275f * fSlow10)), fSlow11))));
		float fSlow42 = (fConst19 * (0.0f - (2.0f * fSlow41)));
		float fSlow43 = synth_french_bell_faustpower2_f(fSlow41);
		float fSlow44 = ftbl0synth_french_bellSIG0[(iSlow15 + 8)];
		float fSlow45 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.351837754f * fSlow10)), fSlow11))));
		float fSlow46 = (fConst20 * (0.0f - (2.0f * fSlow45)));
		float fSlow47 = synth_french_bell_faustpower2_f(fSlow45);
		float fSlow48 = ftbl0synth_french_bellSIG0[(iSlow15 + 7)];
		float fSlow49 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.335368067f * fSlow10)), fSlow11))));
		float fSlow50 = (fConst21 * (0.0f - (2.0f * fSlow49)));
		float fSlow51 = synth_french_bell_faustpower2_f(fSlow49);
		float fSlow52 = ftbl0synth_french_bellSIG0[(iSlow15 + 6)];
		float fSlow53 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.277805746f * fSlow10)), fSlow11))));
		float fSlow54 = (fConst22 * (0.0f - (2.0f * fSlow53)));
		float fSlow55 = synth_french_bell_faustpower2_f(fSlow53);
		float fSlow56 = ftbl0synth_french_bellSIG0[(iSlow15 + 5)];
		float fSlow57 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.277704f * fSlow10)), fSlow11))));
		float fSlow58 = (fConst23 * (0.0f - (2.0f * fSlow57)));
		float fSlow59 = synth_french_bell_faustpower2_f(fSlow57);
		float fSlow60 = ftbl0synth_french_bellSIG0[(iSlow15 + 4)];
		float fSlow61 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.239414543f * fSlow10)), fSlow11))));
		float fSlow62 = (fConst24 * (0.0f - (2.0f * fSlow61)));
		float fSlow63 = synth_french_bell_faustpower2_f(fSlow61);
		float fSlow64 = ftbl0synth_french_bellSIG0[(iSlow15 + 3)];
		float fSlow65 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.237331033f * fSlow10)), fSlow11))));
		float fSlow66 = (fConst25 * (0.0f - (2.0f * fSlow65)));
		float fSlow67 = synth_french_bell_faustpower2_f(fSlow65);
		float fSlow68 = ftbl0synth_french_bellSIG0[(iSlow15 + 2)];
		float fSlow69 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.722866178f * fSlow10)), fSlow11))));
		float fSlow70 = (fConst26 * (0.0f - (2.0f * fSlow69)));
		float fSlow71 = synth_french_bell_faustpower2_f(fSlow69);
		float fSlow72 = ftbl0synth_french_bellSIG0[(iSlow15 + 31)];
		float fSlow73 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.712272525f * fSlow10)), fSlow11))));
		float fSlow74 = (fConst27 * (0.0f - (2.0f * fSlow73)));
		float fSlow75 = synth_french_bell_faustpower2_f(fSlow73);
		float fSlow76 = ftbl0synth_french_bellSIG0[(iSlow15 + 30)];
		float fSlow77 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.707877755f * fSlow10)), fSlow11))));
		float fSlow78 = (fConst28 * (0.0f - (2.0f * fSlow77)));
		float fSlow79 = synth_french_bell_faustpower2_f(fSlow77);
		float fSlow80 = ftbl0synth_french_bellSIG0[(iSlow15 + 29)];
		float fSlow81 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.706308484f * fSlow10)), fSlow11))));
		float fSlow82 = (fConst29 * (0.0f - (2.0f * fSlow81)));
		float fSlow83 = synth_french_bell_faustpower2_f(fSlow81);
		float fSlow84 = ftbl0synth_french_bellSIG0[(iSlow15 + 28)];
		float fSlow85 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.66284585f * fSlow10)), fSlow11))));
		float fSlow86 = (fConst30 * (0.0f - (2.0f * fSlow85)));
		float fSlow87 = synth_french_bell_faustpower2_f(fSlow85);
		float fSlow88 = ftbl0synth_french_bellSIG0[(iSlow15 + 27)];
		float fSlow89 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.66188705f * fSlow10)), fSlow11))));
		float fSlow90 = (fConst31 * (0.0f - (2.0f * fSlow89)));
		float fSlow91 = synth_french_bell_faustpower2_f(fSlow89);
		float fSlow92 = ftbl0synth_french_bellSIG0[(iSlow15 + 26)];
		float fSlow93 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.171829924f * fSlow10)), fSlow11))));
		float fSlow94 = (fConst32 * (0.0f - (2.0f * fSlow93)));
		float fSlow95 = synth_french_bell_faustpower2_f(fSlow93);
		float fSlow96 = ftbl0synth_french_bellSIG0[iSlow15];
		float fSlow97 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.172310486f * fSlow10)), fSlow11))));
		float fSlow98 = (fConst33 * (0.0f - (2.0f * fSlow97)));
		float fSlow99 = synth_french_bell_faustpower2_f(fSlow97);
		float fSlow100 = ftbl0synth_french_bellSIG0[(iSlow15 + 1)];
		float fSlow101 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.421312571f * fSlow10)), fSlow11))));
		float fSlow102 = (fConst34 * (0.0f - (2.0f * fSlow101)));
		float fSlow103 = synth_french_bell_faustpower2_f(fSlow101);
		float fSlow104 = ftbl0synth_french_bellSIG0[(iSlow15 + 12)];
		float fSlow105 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.448252648f * fSlow10)), fSlow11))));
		float fSlow106 = (fConst35 * (0.0f - (2.0f * fSlow105)));
		float fSlow107 = synth_french_bell_faustpower2_f(fSlow105);
		float fSlow108 = ftbl0synth_french_bellSIG0[(iSlow15 + 13)];
		float fSlow109 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.449614525f * fSlow10)), fSlow11))));
		float fSlow110 = (fConst36 * (0.0f - (2.0f * fSlow109)));
		float fSlow111 = synth_french_bell_faustpower2_f(fSlow109);
		float fSlow112 = ftbl0synth_french_bellSIG0[(iSlow15 + 14)];
		float fSlow113 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.468473375f * fSlow10)), fSlow11))));
		float fSlow114 = (fConst37 * (0.0f - (2.0f * fSlow113)));
		float fSlow115 = synth_french_bell_faustpower2_f(fSlow113);
		float fSlow116 = ftbl0synth_french_bellSIG0[(iSlow15 + 15)];
		float fSlow117 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.469576955f * fSlow10)), fSlow11))));
		float fSlow118 = (fConst38 * (0.0f - (2.0f * fSlow117)));
		float fSlow119 = synth_french_bell_faustpower2_f(fSlow117);
		float fSlow120 = ftbl0synth_french_bellSIG0[(iSlow15 + 16)];
		float fSlow121 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.543067336f * fSlow10)), fSlow11))));
		float fSlow122 = (fConst39 * (0.0f - (2.0f * fSlow121)));
		float fSlow123 = synth_french_bell_faustpower2_f(fSlow121);
		float fSlow124 = ftbl0synth_french_bellSIG0[(iSlow15 + 17)];
		float fSlow125 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.547943473f * fSlow10)), fSlow11))));
		float fSlow126 = (fConst40 * (0.0f - (2.0f * fSlow125)));
		float fSlow127 = synth_french_bell_faustpower2_f(fSlow125);
		float fSlow128 = ftbl0synth_french_bellSIG0[(iSlow15 + 18)];
		float fSlow129 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.548808336f * fSlow10)), fSlow11))));
		float fSlow130 = (fConst41 * (0.0f - (2.0f * fSlow129)));
		float fSlow131 = synth_french_bell_faustpower2_f(fSlow129);
		float fSlow132 = ftbl0synth_french_bellSIG0[(iSlow15 + 19)];
		float fSlow133 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.591734827f * fSlow10)), fSlow11))));
		float fSlow134 = (fConst42 * (0.0f - (2.0f * fSlow133)));
		float fSlow135 = synth_french_bell_faustpower2_f(fSlow133);
		float fSlow136 = ftbl0synth_french_bellSIG0[(iSlow15 + 20)];
		float fSlow137 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.592126191f * fSlow10)), fSlow11))));
		float fSlow138 = (fConst43 * (0.0f - (2.0f * fSlow137)));
		float fSlow139 = synth_french_bell_faustpower2_f(fSlow137);
		float fSlow140 = ftbl0synth_french_bellSIG0[(iSlow15 + 21)];
		float fSlow141 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.623202741f * fSlow10)), fSlow11))));
		float fSlow142 = (fConst44 * (0.0f - (2.0f * fSlow141)));
		float fSlow143 = synth_french_bell_faustpower2_f(fSlow141);
		float fSlow144 = ftbl0synth_french_bellSIG0[(iSlow15 + 22)];
		float fSlow145 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.623547137f * fSlow10)), fSlow11))));
		float fSlow146 = (fConst45 * (0.0f - (2.0f * fSlow145)));
		float fSlow147 = synth_french_bell_faustpower2_f(fSlow145);
		float fSlow148 = ftbl0synth_french_bellSIG0[(iSlow15 + 23)];
		float fSlow149 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.650980294f * fSlow10)), fSlow11))));
		float fSlow150 = (fConst46 * (0.0f - (2.0f * fSlow149)));
		float fSlow151 = synth_french_bell_faustpower2_f(fSlow149);
		float fSlow152 = ftbl0synth_french_bellSIG0[(iSlow15 + 24)];
		float fSlow153 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.652162194f * fSlow10)), fSlow11))));
		float fSlow154 = (fConst47 * (0.0f - (2.0f * fSlow153)));
		float fSlow155 = synth_french_bell_faustpower2_f(fSlow153);
		float fSlow156 = ftbl0synth_french_bellSIG0[(iSlow15 + 25)];
		float fSlow157 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.747751713f * fSlow10)), fSlow11))));
		float fSlow158 = (fConst48 * (0.0f - (2.0f * fSlow157)));
		float fSlow159 = synth_french_bell_faustpower2_f(fSlow157);
		float fSlow160 = ftbl0synth_french_bellSIG0[(iSlow15 + 32)];
		float fSlow161 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.754392803f * fSlow10)), fSlow11))));
		float fSlow162 = (fConst49 * (0.0f - (2.0f * fSlow161)));
		float fSlow163 = synth_french_bell_faustpower2_f(fSlow161);
		float fSlow164 = ftbl0synth_french_bellSIG0[(iSlow15 + 33)];
		float fSlow165 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.796411395f * fSlow10)), fSlow11))));
		float fSlow166 = (fConst50 * (0.0f - (2.0f * fSlow165)));
		float fSlow167 = synth_french_bell_faustpower2_f(fSlow165);
		float fSlow168 = ftbl0synth_french_bellSIG0[(iSlow15 + 34)];
		float fSlow169 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.818596661f * fSlow10)), fSlow11))));
		float fSlow170 = (fConst51 * (0.0f - (2.0f * fSlow169)));
		float fSlow171 = synth_french_bell_faustpower2_f(fSlow169);
		float fSlow172 = ftbl0synth_french_bellSIG0[(iSlow15 + 39)];
		float fSlow173 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.827996731f * fSlow10)), fSlow11))));
		float fSlow174 = (fConst52 * (0.0f - (2.0f * fSlow173)));
		float fSlow175 = synth_french_bell_faustpower2_f(fSlow173);
		float fSlow176 = ftbl0synth_french_bellSIG0[(iSlow15 + 40)];
		float fSlow177 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.8323915f * fSlow10)), fSlow11))));
		float fSlow178 = (fConst53 * (0.0f - (2.0f * fSlow177)));
		float fSlow179 = synth_french_bell_faustpower2_f(fSlow177);
		float fSlow180 = ftbl0synth_french_bellSIG0[(iSlow15 + 41)];
		float fSlow181 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.864814281f * fSlow10)), fSlow11))));
		float fSlow182 = (fConst54 * (0.0f - (2.0f * fSlow181)));
		float fSlow183 = synth_french_bell_faustpower2_f(fSlow181);
		float fSlow184 = ftbl0synth_french_bellSIG0[(iSlow15 + 42)];
		float fSlow185 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.866454065f * fSlow10)), fSlow11))));
		float fSlow186 = (fConst55 * (0.0f - (2.0f * fSlow185)));
		float fSlow187 = synth_french_bell_faustpower2_f(fSlow185);
		float fSlow188 = ftbl0synth_french_bellSIG0[(iSlow15 + 43)];
		float fSlow189 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.898583353f * fSlow10)), fSlow11))));
		float fSlow190 = (fConst56 * (0.0f - (2.0f * fSlow189)));
		float fSlow191 = synth_french_bell_faustpower2_f(fSlow189);
		float fSlow192 = ftbl0synth_french_bellSIG0[(iSlow15 + 44)];
		float fSlow193 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.899338603f * fSlow10)), fSlow11))));
		float fSlow194 = (fConst57 * (0.0f - (2.0f * fSlow193)));
		float fSlow195 = synth_french_bell_faustpower2_f(fSlow193);
		float fSlow196 = ftbl0synth_french_bellSIG0[(iSlow15 + 45)];
		float fSlow197 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.899686933f * fSlow10)), fSlow11))));
		float fSlow198 = (fConst58 * (0.0f - (2.0f * fSlow197)));
		float fSlow199 = synth_french_bell_faustpower2_f(fSlow197);
		float fSlow200 = ftbl0synth_french_bellSIG0[(iSlow15 + 46)];
		float fSlow201 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.903072059f * fSlow10)), fSlow11))));
		float fSlow202 = (fConst59 * (0.0f - (2.0f * fSlow201)));
		float fSlow203 = synth_french_bell_faustpower2_f(fSlow201);
		float fSlow204 = ftbl0synth_french_bellSIG0[(iSlow15 + 47)];
		float fSlow205 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.940961897f * fSlow10)), fSlow11))));
		float fSlow206 = (fConst60 * (0.0f - (2.0f * fSlow205)));
		float fSlow207 = synth_french_bell_faustpower2_f(fSlow205);
		float fSlow208 = ftbl0synth_french_bellSIG0[(iSlow15 + 48)];
		float fSlow209 = std::pow(0.00100000005f, (fConst12 / (fSlow9 * std::pow((1.0f - (0.942742527f * fSlow10)), fSlow11))));
		float fSlow210 = (fConst61 * (0.0f - (2.0f * fSlow209)));
		float fSlow211 = synth_french_bell_faustpower2_f(fSlow209);
		float fSlow212 = ftbl0synth_french_bellSIG0[(iSlow15 + 49)];
		for (int i = 0; (i < count); i = (i + 1)) {
			iRec3[0] = ((1103515245 * iRec3[1]) + 12345);
			fRec2[0] = ((4.65661287e-10f * float(iRec3[0])) - (fConst4 * ((fConst7 * fRec2[2]) + (fConst8 * fRec2[1]))));
			fRec1[0] = ((fConst4 * (((fConst6 * fRec2[0]) + (fConst9 * fRec2[1])) + (fConst6 * fRec2[2]))) - (fSlow4 * ((fSlow5 * fRec1[2]) + (fSlow6 * fRec1[1]))));
			fVec0[0] = fSlow8;
			iRec4[0] = (((iRec4[1] + (iRec4[1] > 0)) * (fSlow8 <= fVec0[1])) + (fSlow8 > fVec0[1]));
			float fTemp0 = (fSlow7 * float(iRec4[0]));
			float fTemp1 = (fSlow3 * ((fRec1[2] + (fRec1[0] + (2.0f * fRec1[1]))) * std::max<float>(0.0f, std::min<float>(fTemp0, (2.0f - fTemp0)))));
			fRec0[0] = (fTemp1 - ((fSlow13 * fRec0[1]) + (fSlow14 * fRec0[2])));
			fRec5[0] = (fTemp1 - ((fSlow18 * fRec5[1]) + (fSlow19 * fRec5[2])));
			fRec6[0] = (fTemp1 - ((fSlow22 * fRec6[1]) + (fSlow23 * fRec6[2])));
			fRec7[0] = (fTemp1 - ((fSlow26 * fRec7[1]) + (fSlow27 * fRec7[2])));
			fRec8[0] = (fTemp1 - ((fSlow30 * fRec8[1]) + (fSlow31 * fRec8[2])));
			fRec9[0] = (fTemp1 - ((fSlow34 * fRec9[1]) + (fSlow35 * fRec9[2])));
			fRec10[0] = (fTemp1 - ((fSlow38 * fRec10[1]) + (fSlow39 * fRec10[2])));
			fRec11[0] = (fTemp1 - ((fSlow42 * fRec11[1]) + (fSlow43 * fRec11[2])));
			fRec12[0] = (fTemp1 - ((fSlow46 * fRec12[1]) + (fSlow47 * fRec12[2])));
			fRec13[0] = (fTemp1 - ((fSlow50 * fRec13[1]) + (fSlow51 * fRec13[2])));
			fRec14[0] = (fTemp1 - ((fSlow54 * fRec14[1]) + (fSlow55 * fRec14[2])));
			fRec15[0] = (fTemp1 - ((fSlow58 * fRec15[1]) + (fSlow59 * fRec15[2])));
			fRec16[0] = (fTemp1 - ((fSlow62 * fRec16[1]) + (fSlow63 * fRec16[2])));
			fRec17[0] = (fTemp1 - ((fSlow66 * fRec17[1]) + (fSlow67 * fRec17[2])));
			fRec18[0] = (fTemp1 - ((fSlow70 * fRec18[1]) + (fSlow71 * fRec18[2])));
			fRec19[0] = (fTemp1 - ((fSlow74 * fRec19[1]) + (fSlow75 * fRec19[2])));
			fRec20[0] = (fTemp1 - ((fSlow78 * fRec20[1]) + (fSlow79 * fRec20[2])));
			fRec21[0] = (fTemp1 - ((fSlow82 * fRec21[1]) + (fSlow83 * fRec21[2])));
			fRec22[0] = (fTemp1 - ((fSlow86 * fRec22[1]) + (fSlow87 * fRec22[2])));
			fRec23[0] = (fTemp1 - ((fSlow90 * fRec23[1]) + (fSlow91 * fRec23[2])));
			fRec24[0] = (fTemp1 - ((fSlow94 * fRec24[1]) + (fSlow95 * fRec24[2])));
			fRec25[0] = (fTemp1 - ((fSlow98 * fRec25[1]) + (fSlow99 * fRec25[2])));
			fRec26[0] = (fTemp1 - ((fSlow102 * fRec26[1]) + (fSlow103 * fRec26[2])));
			fRec27[0] = (fTemp1 - ((fSlow106 * fRec27[1]) + (fSlow107 * fRec27[2])));
			fRec28[0] = (fTemp1 - ((fSlow110 * fRec28[1]) + (fSlow111 * fRec28[2])));
			fRec29[0] = (fTemp1 - ((fSlow114 * fRec29[1]) + (fSlow115 * fRec29[2])));
			fRec30[0] = (fTemp1 - ((fSlow118 * fRec30[1]) + (fSlow119 * fRec30[2])));
			fRec31[0] = (fTemp1 - ((fSlow122 * fRec31[1]) + (fSlow123 * fRec31[2])));
			fRec32[0] = (fTemp1 - ((fSlow126 * fRec32[1]) + (fSlow127 * fRec32[2])));
			fRec33[0] = (fTemp1 - ((fSlow130 * fRec33[1]) + (fSlow131 * fRec33[2])));
			fRec34[0] = (fTemp1 - ((fSlow134 * fRec34[1]) + (fSlow135 * fRec34[2])));
			fRec35[0] = (fTemp1 - ((fSlow138 * fRec35[1]) + (fSlow139 * fRec35[2])));
			fRec36[0] = (fTemp1 - ((fSlow142 * fRec36[1]) + (fSlow143 * fRec36[2])));
			fRec37[0] = (fTemp1 - ((fSlow146 * fRec37[1]) + (fSlow147 * fRec37[2])));
			fRec38[0] = (fTemp1 - ((fSlow150 * fRec38[1]) + (fSlow151 * fRec38[2])));
			fRec39[0] = (fTemp1 - ((fSlow154 * fRec39[1]) + (fSlow155 * fRec39[2])));
			fRec40[0] = (fTemp1 - ((fSlow158 * fRec40[1]) + (fSlow159 * fRec40[2])));
			fRec41[0] = (fTemp1 - ((fSlow162 * fRec41[1]) + (fSlow163 * fRec41[2])));
			fRec42[0] = (fTemp1 - ((fSlow166 * fRec42[1]) + (fSlow167 * fRec42[2])));
			fRec43[0] = (fTemp1 - ((fSlow170 * fRec43[1]) + (fSlow171 * fRec43[2])));
			fRec44[0] = (fTemp1 - ((fSlow174 * fRec44[1]) + (fSlow175 * fRec44[2])));
			fRec45[0] = (fTemp1 - ((fSlow178 * fRec45[1]) + (fSlow179 * fRec45[2])));
			fRec46[0] = (fTemp1 - ((fSlow182 * fRec46[1]) + (fSlow183 * fRec46[2])));
			fRec47[0] = (fTemp1 - ((fSlow186 * fRec47[1]) + (fSlow187 * fRec47[2])));
			fRec48[0] = (fTemp1 - ((fSlow190 * fRec48[1]) + (fSlow191 * fRec48[2])));
			fRec49[0] = (fTemp1 - ((fSlow194 * fRec49[1]) + (fSlow195 * fRec49[2])));
			fRec50[0] = (fTemp1 - ((fSlow198 * fRec50[1]) + (fSlow199 * fRec50[2])));
			fRec51[0] = (fTemp1 - ((fSlow202 * fRec51[1]) + (fSlow203 * fRec51[2])));
			fRec52[0] = (fTemp1 - ((fSlow206 * fRec52[1]) + (fSlow207 * fRec52[2])));
			fRec53[0] = (fTemp1 - ((fSlow210 * fRec53[1]) + (fSlow211 * fRec53[2])));
			output0[i] = FAUSTFLOAT((0.0199999996f * (((((((((((((fRec0[0] - fRec0[2]) * fSlow16) + (((fRec5[0] - fRec5[2]) * fSlow20) + (((fRec6[0] - fRec6[2]) * fSlow24) + (((fRec7[0] - fRec7[2]) * fSlow28) + (((fRec8[0] - fRec8[2]) * fSlow32) + (((fRec9[0] - fRec9[2]) * fSlow36) + (((fRec10[0] - fRec10[2]) * fSlow40) + (((fRec11[0] - fRec11[2]) * fSlow44) + (((fRec12[0] - fRec12[2]) * fSlow48) + (((fRec13[0] - fRec13[2]) * fSlow52) + (((fRec14[0] - fRec14[2]) * fSlow56) + (((fRec15[0] - fRec15[2]) * fSlow60) + (((fRec16[0] - fRec16[2]) * fSlow64) + (((fRec17[0] - fRec17[2]) * fSlow68) + (((fRec18[0] - fRec18[2]) * fSlow72) + (((fRec19[0] - fRec19[2]) * fSlow76) + (((fRec20[0] - fRec20[2]) * fSlow80) + (((fRec21[0] - fRec21[2]) * fSlow84) + (((fRec22[0] - fRec22[2]) * fSlow88) + (((fRec23[0] - fRec23[2]) * fSlow92) + (((((((((((((((((((((fRec24[0] - fRec24[2]) * fSlow96) + ((fRec25[0] - fRec25[2]) * fSlow100)) + ((fRec26[0] - fRec26[2]) * fSlow104)) + ((fRec27[0] - fRec27[2]) * fSlow108)) + ((fRec28[0] - fRec28[2]) * fSlow112)) + ((fRec29[0] - fRec29[2]) * fSlow116)) + ((fRec30[0] - fRec30[2]) * fSlow120)) + ((fRec31[0] - fRec31[2]) * fSlow124)) + ((fRec32[0] - fRec32[2]) * fSlow128)) + ((fRec33[0] - fRec33[2]) * fSlow132)) + ((fRec34[0] - fRec34[2]) * fSlow136)) + ((fRec35[0] - fRec35[2]) * fSlow140)) + ((fRec36[0] - fRec36[2]) * fSlow144)) + ((fRec37[0] - fRec37[2]) * fSlow148)) + ((fRec38[0] - fRec38[2]) * fSlow152)) + ((fRec39[0] - fRec39[2]) * fSlow156)) + ((fRec40[0] - fRec40[2]) * fSlow160)) + ((fRec41[0] - fRec41[2]) * fSlow164)) + ((fRec42[0] - fRec42[2]) * fSlow168)) + ((fRec43[0] - fRec43[2]) * fSlow172)))))))))))))))))))))) + ((fRec44[0] - fRec44[2]) * fSlow176)) + ((fRec45[0] - fRec45[2]) * fSlow180)) + ((fRec46[0] - fRec46[2]) * fSlow184)) + ((fRec47[0] - fRec47[2]) * fSlow188)) + ((fRec48[0] - fRec48[2]) * fSlow192)) + ((fRec49[0] - fRec49[2]) * fSlow196)) + ((fRec50[0] - fRec50[2]) * fSlow200)) + ((fRec51[0] - fRec51[2]) * fSlow204)) + ((fRec52[0] - fRec52[2]) * fSlow208)) + ((fRec53[0] - fRec53[2]) * fSlow212))));
			iRec3[1] = iRec3[0];
			fRec2[2] = fRec2[1];
			fRec2[1] = fRec2[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			fVec0[1] = fVec0[0];
			iRec4[1] = iRec4[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec12[2] = fRec12[1];
			fRec12[1] = fRec12[0];
			fRec13[2] = fRec13[1];
			fRec13[1] = fRec13[0];
			fRec14[2] = fRec14[1];
			fRec14[1] = fRec14[0];
			fRec15[2] = fRec15[1];
			fRec15[1] = fRec15[0];
			fRec16[2] = fRec16[1];
			fRec16[1] = fRec16[0];
			fRec17[2] = fRec17[1];
			fRec17[1] = fRec17[0];
			fRec18[2] = fRec18[1];
			fRec18[1] = fRec18[0];
			fRec19[2] = fRec19[1];
			fRec19[1] = fRec19[0];
			fRec20[2] = fRec20[1];
			fRec20[1] = fRec20[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fRec22[2] = fRec22[1];
			fRec22[1] = fRec22[0];
			fRec23[2] = fRec23[1];
			fRec23[1] = fRec23[0];
			fRec24[2] = fRec24[1];
			fRec24[1] = fRec24[0];
			fRec25[2] = fRec25[1];
			fRec25[1] = fRec25[0];
			fRec26[2] = fRec26[1];
			fRec26[1] = fRec26[0];
			fRec27[2] = fRec27[1];
			fRec27[1] = fRec27[0];
			fRec28[2] = fRec28[1];
			fRec28[1] = fRec28[0];
			fRec29[2] = fRec29[1];
			fRec29[1] = fRec29[0];
			fRec30[2] = fRec30[1];
			fRec30[1] = fRec30[0];
			fRec31[2] = fRec31[1];
			fRec31[1] = fRec31[0];
			fRec32[2] = fRec32[1];
			fRec32[1] = fRec32[0];
			fRec33[2] = fRec33[1];
			fRec33[1] = fRec33[0];
			fRec34[2] = fRec34[1];
			fRec34[1] = fRec34[0];
			fRec35[2] = fRec35[1];
			fRec35[1] = fRec35[0];
			fRec36[2] = fRec36[1];
			fRec36[1] = fRec36[0];
			fRec37[2] = fRec37[1];
			fRec37[1] = fRec37[0];
			fRec38[2] = fRec38[1];
			fRec38[1] = fRec38[0];
			fRec39[2] = fRec39[1];
			fRec39[1] = fRec39[0];
			fRec40[2] = fRec40[1];
			fRec40[1] = fRec40[0];
			fRec41[2] = fRec41[1];
			fRec41[1] = fRec41[0];
			fRec42[2] = fRec42[1];
			fRec42[1] = fRec42[0];
			fRec43[2] = fRec43[1];
			fRec43[1] = fRec43[0];
			fRec44[2] = fRec44[1];
			fRec44[1] = fRec44[0];
			fRec45[2] = fRec45[1];
			fRec45[1] = fRec45[0];
			fRec46[2] = fRec46[1];
			fRec46[1] = fRec46[0];
			fRec47[2] = fRec47[1];
			fRec47[1] = fRec47[0];
			fRec48[2] = fRec48[1];
			fRec48[1] = fRec48[0];
			fRec49[2] = fRec49[1];
			fRec49[1] = fRec49[0];
			fRec50[2] = fRec50[1];
			fRec50[1] = fRec50[0];
			fRec51[2] = fRec51[1];
			fRec51[1] = fRec51[0];
			fRec52[2] = fRec52[1];
			fRec52[1] = fRec52[0];
			fRec53[2] = fRec53[1];
			fRec53[1] = fRec53[0];
		}
	}

};
// clang-format on
#endif

    template <class T>
    struct _synth_french_bell_UI : public UI {
    static std::string name;
};

template <class T>
std::string _synth_french_bell_UI<T>::name(sym(synth_french_bell));

typedef _synth_french_bell_UI<synth_french_bell> synth_french_bell_UI;

class faust_synth_french_bell_tilde : public FaustExternal<synth_french_bell, synth_french_bell_UI> {
public:
    faust_synth_french_bell_tilde(const ceammc::PdArgs& args)
        : FaustExternal(args)
    {
    }
};

#endif