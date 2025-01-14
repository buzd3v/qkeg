#pragma once
#include "Sound.h"
#include <unordered_map>
class SoundPool
{
    SoundPool()
    {
        InitAudioDevice(); // Ensure audio device is initialized
    }
    ~SoundPool()
    {
        audios.clear();
        CloseAudioDevice();
    }

    void loadSound(const std::string &name, const std::string &filePath) { audios[name] = Audio(filePath); }

    Audio *getSound(const std::string &name)
    {
        if (audios.find(name) != audios.end())
        {
            return &audios[name];
        }
        return nullptr;
    }

  private:
    std::unordered_map<std::string, Audio> audios{}; // Changed Sound to Audio
};