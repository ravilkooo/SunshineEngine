#include "Audio/AudioComponent.h"
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>
#include "Audio/AudioSystem.h"

#define ADD_FIELD(name) #name, &AudioComponent::name
#define AUDIOCOMP_FIELD_PAIRS AUDIOCOMP_LUA_FIELDS_APPLY(ADD_FIELD)
#undef ADD_FIELD

#define ADD_METHOD_WITH_LEAD(k, fn) , k, fn
#define AUDIOCOMP_METHOD_PAIRS AUDIOCOMP_LUA_METHODS_APPLY(ADD_METHOD_WITH_LEAD)
#undef ADD_METHOD_WITH_LEAD

LUA_REGISTER_COMPONENT(
    AudioComponent,
    "AudioComponent",
    AUDIOCOMP_FIELD_PAIRS
    AUDIOCOMP_METHOD_PAIRS,
    "getAudio")