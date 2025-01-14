#pragma once
#include "raylib.h"
#include <string>
class Audio
{
  public:
    Audio() = default;
    Audio(const std::string &filePath)
    {
        sound = LoadSound(filePath.c_str());
        name  = filePath; // Use filePath as the default name.
    }
    ~Audio()
    {
        if (sound.stream.buffer)
        { // Check if sound was properly loaded
            UnloadSound(sound);
        }
    }

    void play() const
    {
        if (sound.stream.buffer)
        {
            PlaySound(sound);
        }
    }
    void pause() const
    {
        if (sound.stream.buffer)
        {
            PauseSound(sound);
        }
    }
    void resume() const
    {
        if (sound.stream.buffer)
        {
            ResumeSound(sound);
        }
    }

    const std::string &GetName() const { return name; }

  private:
    Sound       sound;
    std::string name;
};