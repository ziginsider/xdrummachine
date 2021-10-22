//
// Created by Ziginsider on 10/21/2021.
//

#include <utils/logging.h>
#include <constants/ZDrumMachineConstants.h>
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

    auto *outputBuffer = static_cast<float *>(audioData);

    int64_t nextMetronomeWeakEventMs;

    for (int i = 0; i < numFrames; ++i) {
        mSongPositionMs = convertFramesToMillis(
                mCurrentFrame,
                mAudioStream->getSampleRate());

        if (mMetronomeWeakEvents.peek(nextMetronomeWeakEventMs) &&
            mSongPositionMs >= nextMetronomeWeakEventMs) {
            mMetronomeWeakSound->setPlaying(true);
            mMetronomeWeakEvents.pop(nextMetronomeWeakEventMs);
        }

        mMixer.renderAudio(outputBuffer+(oboeStream->getChannelCount()*i), 1);
        mCurrentFrame++;
    }

    return DataCallbackResult::Continue;
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
    // Create an audio stream
    AudioStreamBuilder builder;
    builder.setFormat(AudioFormat::Float);
    builder.setFormatConversionAllowed(true);
    builder.setPerformanceMode(PerformanceMode::LowLatency);
    builder.setSharingMode(SharingMode::Exclusive);
    builder.setSampleRate(kFramesPerSecond);
    builder.setSampleRateConversionQuality(SampleRateConversionQuality::Medium);
    builder.setChannelCount(kChannelCount);
    builder.setDataCallback(this);
    builder.setErrorCallback(this);
    Result result = builder.openStream(mAudioStream);
    if (result != Result::OK) {
        LOGE("Failed to open stream. Error: %s", convertToText(result));
        return false;
    }

    mMixer.setChannelCount(mAudioStream->getChannelCount());

    return true;
}

bool ZDrumMachine::setupAudioSources() {

    // Set the properties of our audio source(s) to match that of our audio stream
    AudioProperties targetProperties{
            .channelCount = mAudioStream->getChannelCount(),
            .sampleRate = mAudioStream->getSampleRate()
    };

    // Create a data source and player for the clap sound
    std::shared_ptr<AAssetDataSource> mMetronomeWeakSource{
            AAssetDataSource::newFromCompressedAssetZ(mAssetManager, kMetronomeWeakFilename,
                                                      targetProperties)
    };
    if (mMetronomeWeakSource == nullptr) {
        LOGE("Could not load source data for clap sound");
        return false;
    }
    mMetronomeWeakSound = std::make_unique<Player>(mMetronomeWeakSource);

    //Add players to a mixer
    mMixer.addTrack(mMetronomeWeakSound.get());

    return true;
}

void ZDrumMachine::scheduleSongEvents() {
    for (auto t : kTestMetronomeEvents) mMetronomeWeakEvents.push(t);
}

void ZDrumMachine::release() {
    mPlayingState = PlayingState::Stopped;
    mMixer.removeAllTracks();
    mCurrentFrame = 0;
    mSongPositionMs = 0;
}
