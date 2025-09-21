/*==============================================================================
RNBO DSP Plugin Example
Copyright (c), Firelight Technologies Pty, Ltd 2004-2025.

This example shows how to integrate RNBO C++ source code into an FMOD effect.
1. Add the rnbo and rnbo/common directories to your include paths.
2. Add the generated RNBO source .cpp file and rnbo/RNBO.cpp files to your list
   of source files.
3. Build and copy the generated library into your FMOD Studio script directory:
   https://fmod.com/docs/2.03/studio/plugin-reference.html#loading-plug-ins
Note:
FMOD parameters have a max length of 15 characters. RNBO parameters greater
than 15 chracters will be truncated.
==============================================================================*/
#if __has_include("RNBO.h")
#include "RNBO.h"

#include <math.h>
#include <stdio.h>
#include <atomic>
#include <string.h>

#include "fmod.hpp"

static FMOD_DSP_PARAMETER_DESC** params = nullptr;
static int numParams;
static int numInputs;
static int numOutputs;
static std::atomic<int> gSysCount = 0;

static FMOD_RESULT F_CALL FMOD_RNBO_Create(FMOD_DSP_STATE *dsp_state)
{
    dsp_state->plugindata = FMOD_DSP_ALLOC(dsp_state, sizeof(RNBO::CoreObject));
    if (!dsp_state->plugindata)
    {
        return FMOD_ERR_MEMORY;
    }

    RNBO::CoreObject *rnboObject = (RNBO::CoreObject *)dsp_state->plugindata;

    int sampleRate;
    unsigned int blockSize;

    FMOD_DSP_GETSAMPLERATE(dsp_state, &sampleRate);
    FMOD_DSP_GETBLOCKSIZE(dsp_state, &blockSize);

    new (rnboObject) RNBO::CoreObject();

    rnboObject->prepareToProcess(sampleRate, blockSize);

    return FMOD_OK;
}

static FMOD_RESULT F_CALL FMOD_RNBO_Release(FMOD_DSP_STATE *dsp_state)
{
    RNBO::CoreObject *rnboObject = (RNBO::CoreObject *)dsp_state->plugindata;
    rnboObject->~CoreObject();

    FMOD_DSP_FREE(dsp_state, rnboObject);

    return FMOD_OK;
}

static FMOD_RESULT F_CALL FMOD_RNBO_Process(FMOD_DSP_STATE *dsp_state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY *inbufferarray, FMOD_DSP_BUFFER_ARRAY *outbufferarray, FMOD_BOOL inputsidle, FMOD_DSP_PROCESS_OPERATION op)
{
    RNBO::CoreObject *rnboObject = (RNBO::CoreObject *)dsp_state->plugindata;

    if (op == FMOD_DSP_PROCESS_QUERY)
    {
        if (numInputs > 0)
        {
            if (inputsidle)
            {
                return FMOD_ERR_DSP_SILENCE;
            }
        }
        else
        {
            if (outbufferarray && outbufferarray[0].buffernumchannels)
            {
                outbufferarray[0].buffernumchannels[0] = numOutputs;
            }
        }
    }
    else
    {
        rnboObject->process(inbufferarray[0].buffers[0], inbufferarray[0].buffernumchannels[0],
            outbufferarray[0].buffers[0], outbufferarray[0].buffernumchannels[0], length);
    }

    return FMOD_OK;
}

static FMOD_RESULT F_CALL FMOD_RNBO_SetParamFloat(FMOD_DSP_STATE *dsp_state, int index, float value)
{
    RNBO::CoreObject *rnboObject = (RNBO::CoreObject *)dsp_state->plugindata;

    rnboObject->setParameterValue(index, value);
    return FMOD_OK;
}

static FMOD_RESULT F_CALL FMOD_RNBO_GetParamFloat(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr)
{
    RNBO::CoreObject *rnboObject = (RNBO::CoreObject *)dsp_state->plugindata;

    *value = (float)rnboObject->getParameterValue(index);
    if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%s", rnboObject->getParameterName(index));

    return FMOD_OK;
}

static FMOD_RESULT F_CALL FMOD_RNBO_SysRegister(FMOD_DSP_STATE* dsp_state)
{
    RNBO::CoreObject* rnboObject = (RNBO::CoreObject*)dsp_state->plugindata;

    gSysCount++;

    return FMOD_OK;
}

static FMOD_RESULT F_CALL FMOD_RNBO_SysDeregister(FMOD_DSP_STATE* dsp_state)
{
    RNBO::CoreObject* rnboObject = (RNBO::CoreObject*)dsp_state->plugindata;

    gSysCount--;
    if (gSysCount == 0)
    {
        for (int i = 0; i < numParams; ++i)
        {
            free(params[i]);
        }
        free(params);

        params = nullptr;
    }

    return FMOD_OK;
}

extern "C" F_EXPORT FMOD_DSP_DESCRIPTION *F_CALL FMODGetDSPDescription()
{
    static FMOD_DSP_DESCRIPTION desc = { FMOD_PLUGIN_SDK_VERSION, "FMOD RNBO", 0x00010000 };
    desc.create = FMOD_RNBO_Create;
    desc.release = FMOD_RNBO_Release;
    desc.process = FMOD_RNBO_Process;
    desc.setparameterfloat = FMOD_RNBO_SetParamFloat;
    desc.getparameterfloat = FMOD_RNBO_GetParamFloat;
    desc.sys_register = FMOD_RNBO_SysRegister;
    desc.sys_deregister = FMOD_RNBO_SysDeregister;

    if (!params)
    {
        RNBO::CoreObject tmpRnboObject;

        numParams = tmpRnboObject.getNumParameters();
        numInputs = tmpRnboObject.getNumInputChannels();
        numOutputs = tmpRnboObject.getNumOutputChannels();

        desc.numparameters = numParams;
        desc.numinputbuffers = numInputs > 0;
        desc.numoutputbuffers = numOutputs > 0;

        params = (FMOD_DSP_PARAMETER_DESC**)malloc(desc.numparameters * sizeof(FMOD_DSP_PARAMETER_DESC*));

        for (int i = 0; i < numParams; ++i)
        {
            RNBO::ParameterInfo info = {};
            tmpRnboObject.getParameterInfo(i, &info);

            params[i] = (FMOD_DSP_PARAMETER_DESC *)malloc(sizeof(FMOD_DSP_PARAMETER_DESC));
            memset(params[i], 0, sizeof(FMOD_DSP_PARAMETER_DESC));

            // FMOD_DSP_PARAMETER_DESC::name can only hold 15 characters (excluding \0 null terminator).
            // RNBO parameters greater than 15 chracters will be truncated.
            snprintf(params[i]->name, sizeof(params[i]->name), "%s", tmpRnboObject.getParameterId(i));
            snprintf(params[i]->label, sizeof(params[i]->label), "%s", info.unit);

            params[i]->type = FMOD_DSP_PARAMETER_TYPE_FLOAT;
            params[i]->floatdesc.defaultval = info.initialValue;
            params[i]->floatdesc.min = info.min;
            params[i]->floatdesc.max = info.max;
        }

        desc.paramdesc = params;
    }
    return &desc;
}

#endif
