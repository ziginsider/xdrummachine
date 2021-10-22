//
// Created by Ziginsider on 10/21/2021.
//

#include <utils/logging.h>
#include <thread>
#include <cinttypes>

#include "ZDrumMachine.h"

ZDrumMachine::ZDrumMachine(AAssetManager &assetManager) : mAssetManager(assetManager) {
}

void ZDrumMachine::start() {
    mLoadingResult = std::async(&ZDrumMachine::load, this);
}

void ZDrumMachine::stop() {
    release();
    if (mAudioStream) {
        mAudioStream->stop();
        mAudioStream->close();
        mAudioStream.reset();
    }
}

void ZDrumMachine::onErrorAfterClose(AudioStream *stream, Result result) {
    if (result == Result::ErrorDisconnected) {
        release();
        mAudioStream.reset();
        start();
    } else {
        LOGE("Stream error: %s", convertToText(result));
    }
}

DataCallbackResult
ZDrumMachine::onAudioReady(AudioStream *oboeStream, void *audioData, int32_t numFrames) {

}

void ZDrumMachine::load() {
    if (!openStream()) {
        mPlayingState = PlayingState::FailedToLoad;
        return;
    }

    if (!setupAudioSources()) {
        mPlayingState = PlayingState::FailedToLoad;
        return;
    }

    scheduleSongEvents();

    Result result = mAudioStream->requestStart();
    if (result != Result::OK) {
        LOGE("Failed to start stream. Error: %s", convertToText(result));
        mPlayingState = PlayingState::FailedToLoad;
        return;
    }
}

bool ZDrumMachine::openStream() {

    return true;
}

bool ZDrumMachine::setupAudioSources() {

    return false;
}

void ZDrumMachine::scheduleSongEvents() {
    
}

void ZDrumMachine::release() {
    mPlayingState = PlayingState::Stopped;
    mMixer.removeAllTracks();
    mCurrentFrame = 0;
    mSongPositionMs = 0;
}
