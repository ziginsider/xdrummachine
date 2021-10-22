//
// Created by Ziginsider on 10/21/2021.
//

#include <utils/logging.h>
#include <thread>
#include <cinttypes>

#include "ZDrumMachine.h"

ZDrumMachine::ZDrumMachine(AAssetManager &assetManager) : mAssetManager(assetManager) {
}

void ZDrumMachine::onErrorAfterClose(AudioStream *stream, Result result) {
    if (result == Result::ErrorDisconnected) {
        mPlayingState = PlayingState::Stopped;
        mAudioStream.reset();
        mMixer.removeAllTracks();
        mCurrentFrame = 0;
        mSongPositionMs = 0;

        //start();
    } else {
        LOGE("Stream error: %s", convertToText(result));
    }
}

DataCallbackResult
ZDrumMachine::onAudioReady(AudioStream *oboeStream, void *audioData, int32_t numFrames) {

}
