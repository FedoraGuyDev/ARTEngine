#include "engineAudio.h"

#include "AL/al.h"
#include "AL/alc.h"
#include "stbVorbis/stb_vorbis.c"
#include <iostream>

static ALCdevice* g_device = nullptr;
static ALCcontext* g_context = nullptr;
static bool g_ok = false;

bool AudioInit(){
    g_device = alcOpenDevice(nullptr);
    if(!g_device){
        std::cout << "[OPENAL] ARTEngine could not open the audio device :C" << std::endl;
        return false;
    }

    g_context = alcCreateContext(g_device,nullptr);
    if(!g_context || !alcMakeContextCurrent(g_context)){
        std::cout << "[OPENAL] It could not be possible to create or activate the context :C" << std::endl;
        if(g_context) alcDestroyContext(g_context);
        alcCloseDevice(g_device);
        g_context = nullptr;
        g_device = nullptr;
        return false;
    }

    std::cout << "[OPENAL] " << alGetString(AL_VENDOR) << " | " << alGetString(AL_VERSION) << std::endl;
    alGetError();
    g_ok = true;
    return true;
}

void AudioShutdown(){
    alcMakeContextCurrent(nullptr);
    if(g_context) alcDestroyContext(g_context);
    if(g_device) alcCloseDevice(g_device);
    g_context = nullptr;
    g_device = nullptr;
    g_ok = false;
}

bool AudioIsOk(){
    return g_ok;
}
