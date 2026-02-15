#pragma once
#include "Sound.h"
#include <singleton_atomic.hpp>
#include <unordered_map>

class SoundPool : public SingletonAtomic<SoundPool>
{
  public:
    SoundPool()
    {
        InitAudioDevice(); // Ensure audio device is initialized
    }

    void loadSound(const std::string &name, const std::string &filePath) { audios[name] = Audio(filePath); }

    void cleanUp()
    {
        audios.clear();
        CloseAudioDevice();
    }

    Audio *getSound(const std::string &name)
    {
        if (audios.find(name) != audios.end())
        {
            return &audios[name];
        }
        return nullptr;
    }

  protected:
    std::unordered_map<std::string, Audio> audios{}; // Changed Sound to Audio
};