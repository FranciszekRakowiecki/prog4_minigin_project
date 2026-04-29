//
// Created by frane on 4/29/2026.
//

#ifndef MINIGIN_SOUNDLIBRARY_H
#define MINIGIN_SOUNDLIBRARY_H
#include "Singleton.h"
#include "SoundSystem.h"


class SoundLibrary : public dae::Singleton<SoundLibrary> {
public:
    // Sample
    SoundId Beep;
    // Sample
    SoundId Boop;

    void Load();
};


#endif //MINIGIN_SOUNDLIBRARY_H