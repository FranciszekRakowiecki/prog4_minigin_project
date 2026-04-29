//
// Created by frane on 4/29/2026.
//

#ifndef MINIGIN_SOUNDSYSTEMSDL_H
#define MINIGIN_SOUNDSYSTEMSDL_H
#include "SoundSystem.h"


class SoundSystemSDL : public SoundSystem {
public:
    void playSound(SoundId id, float volume) override;

    bool Init() override;

    void Shutdown() override;

    SoundId registerSound(const std::string &path) override;

    SoundSystemSDL();
    ~SoundSystemSDL();

    void update() override;

    class Impl;

private:
    std::unique_ptr<Impl> m_Impl;
};


#endif //MINIGIN_SOUNDSYSTEMSDL_H