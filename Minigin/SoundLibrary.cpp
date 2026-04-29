//
// Created by frane on 4/29/2026.
//

#include "SoundLibrary.h"

#include "ServiceLocator.h"
#include "Minigin.h"

void SoundLibrary::Load() {
    Beep = ServiceLocator::GetInstance().getSoundSystem().registerSound(MINIGIN_DATA_PATH + "beep.wav");
    Boop = ServiceLocator::GetInstance().getSoundSystem().registerSound(MINIGIN_DATA_PATH + "boop.wav");
}
