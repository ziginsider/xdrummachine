//
// Created by Ziginsider on 10/21/2021.
//

#ifndef ZDRUMMACHINE_ZDRUMMACHINE_H
#define ZDRUMMACHINE_ZDRUMMACHINE_H

#include <android/asset_manager.h>
#include <oboe/Oboe.h>

#include "utils/LockFreeQueue.h"
#include "utils/UtilityFunctions.h"
#include "constants/ZDrumMachineConstants.h"

using namespace oboe;

class ZDrumMachine {
public:
    explicit ZDrumMachine(AAssetManager&);

private:
    AAssetManager& mAssetManager;
};

#endif //ZDRUMMACHINE_ZDRUMMACHINE_H
