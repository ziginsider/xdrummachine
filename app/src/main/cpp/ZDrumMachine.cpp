//
// Created by Ziginsider on 10/21/2021.
//

#include <utils/logging.h>
#include <constants/ZDrumMachineConstants.h>
#include <thread>
#include <cinttypes>

#include "ZDrumMachine.h"

ZDrumMachine::ZDrumMachine(AAssetManager &assetManager, int bpmReal) : mAssetManager(assetManager) {
    mBpmReal = bpmReal;
}

void ZDrumMachine::start(int bpm) {
    mBpmReal = bpm;
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

void ZDrumMachine::pause() {
    // For some reason this failed sometimes, so we just stop()
    // TODO remove this workaround and fix the issue
    // but it seems work !!!

//    mPlayingState = PlayingState::Paused;
//    if (mAudioStream) {
//        mAudioStream->pause();
//    }
    stop();
}

void ZDrumMachine::onErrorAfterClose(AudioStream *stream, Result result) {
    if (result == Result::ErrorDisconnected) {
        release();
        mAudioStream.reset();
        start(mBpmReal);
    } else {
        LOGE("Stream error: %s", convertToText(result));
    }
}

DataCallbackResult
ZDrumMachine::onAudioReady(AudioStream *oboeStream, void *audioData, int32_t numFrames) {

    auto *outputBuffer = static_cast<float *>(audioData);

    int64_t nextMetronomeWeakEventMs;
    int64_t nextDrumMidTomEventMs;
    int64_t nextDrumSnareEventMs;

    for (int i = 0; i < numFrames; ++i) {
        mSongPositionMs = convertFramesToMillis(mCurrentFrame, mAudioStream->getSampleRate());

        // Metronome weak
        if (mMetronomeWeakEvents.peek(nextMetronomeWeakEventMs) &&
            mSongPositionMs >=
            static_cast<int64_t>((nextMetronomeWeakEventMs + (mMultiplierMetronomeWeak * 4000)) *
                                 mBpm)) {
            mMetronomeWeakSound->setPlaying(true);
            mMetronomeWeakEvents.pop(nextMetronomeWeakEventMs);

            if (mMetronomeWeakEvents.isEmpty()) {
                mMultiplierMetronomeWeak += 1;
                scheduleMetronomeEvents();
            }
        }
        // End Metronome weak

        // Drum Mid Tom
        if (mDrumMidTomEvents.peek(nextDrumMidTomEventMs) &&
            mSongPositionMs >=
            static_cast<int64_t>((nextDrumMidTomEventMs + (mMultiplierDrumMidTom * 4000)) * mBpm)) {
            mDrumMidTomeSound->setPlaying(true);
            mDrumMidTomEvents.pop(nextDrumMidTomEventMs);

            if (mDrumMidTomEvents.isEmpty()) {
                mMultiplierDrumMidTom += 1;
                scheduleDrumMidTomEvents();
            }
        }
        // End Drum Mid Tom

        // Drum Snare
        if (mDrumShareEvents.peek(nextDrumSnareEventMs) &&
            mSongPositionMs >=
            static_cast<int64_t>((nextDrumSnareEventMs + (mMultiplierDrumSnare * 4000)) * mBpm)) {
            mDrumShareSound->setPlaying(true);
            mDrumShareEvents.pop(nextDrumSnareEventMs);

            if (mDrumShareEvents.isEmpty()) {
                mMultiplierDrumSnare += 1;
                scheduleDrumSnareEvents();
            }
        }
        // End Drum Snare

        mMixer.renderAudio(outputBuffer + (oboeStream->getChannelCount() * i), 1);
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

    // TODO in func
    if (mMetronomeWeakEvents.isEmpty()) {
        scheduleMetronomeEvents();
    }
    if (mDrumMidTomEvents.isEmpty()) {
        scheduleDrumMidTomEvents();
    }
    if (mDrumShareEvents.isEmpty()) {
        scheduleDrumSnareEvents();
    }

    // Bpm
    mBpm = (static_cast<double>(kStandardBpm) / mBpmReal);

    StreamState streamState = mAudioStream->getState();
    if (streamState == StreamState::Open || streamState == StreamState::Stopped ||
        streamState == StreamState::Closed || streamState == StreamState::Uninitialized) {
        Result result = mAudioStream->requestStart();
        if (result != Result::OK) {
            LOGE("Failed to start stream. Error: %s", convertToText(result));
            mPlayingState = PlayingState::FailedToLoad;
            return;
        }
    }
    if (streamState == StreamState::Paused) {
        Result result = mAudioStream->start();
        if (result != Result::OK) {
            LOGE("Failed to start stream. Error: %s", convertToText(result));
            mPlayingState = PlayingState::FailedToLoad;
            return;
        }
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

    // Create a data source and player for the metronome weak sound
    std::shared_ptr<AAssetDataSource> mMetronomeWeakSource{
            AAssetDataSource::newFromCompressedAssetZ(mAssetManager, kMetronomeWeakFilename,
                                                      targetProperties)
    };
    if (mMetronomeWeakSource == nullptr) {
        LOGE("Could not load source data for metronome weak sound");
        return false;
    }
    mMetronomeWeakSound = std::make_unique<Player>(mMetronomeWeakSource);

    // Create a data source and player for the drum mid tom sound
    std::shared_ptr<AAssetDataSource> mDrumMidTomSource{
            AAssetDataSource::newFromCompressedAssetZ(mAssetManager, kMidTomFilename,
                                                      targetProperties)
    };
    if (mDrumMidTomSource == nullptr) {
        LOGE("Could not load source data for drum mid tom sound");
        return false;
    }
    mDrumMidTomeSound = std::make_unique<Player>(mDrumMidTomSource);

    // Create a data source and player for the drum snare sound
    std::shared_ptr<AAssetDataSource> mDrumSnareSource{
            AAssetDataSource::newFromCompressedAssetZ(mAssetManager, kSnareDrumFilename,
                                                      targetProperties)
    };
    if (mDrumSnareSource == nullptr) {
        LOGE("Could not load source data for drum snare sound");
        return false;
    }
    mDrumShareSound = std::make_unique<Player>(mDrumSnareSource);

    //Add players to a mixer
    mMixer.addTrack(mMetronomeWeakSound.get());
    mMixer.addTrack(mDrumMidTomeSound.get());
    mMixer.addTrack(mDrumShareSound.get());

    return true;
}

void ZDrumMachine::release() {
    mPlayingState = PlayingState::Stopped;
    mMixer.removeAllTracks();
    mCurrentFrame = 0;
    mSongPositionMs = 0;
    mMultiplierMetronomeWeak = 0;
    mMultiplierDrumMidTom = 0;
    mMultiplierDrumSnare = 0;
}

LockFreeQueue<int64_t, kMaxQueueItems> *ZDrumMachine::getSoundEvents(int idSound) {
    if (idSound == 0) {
        return &mMetronomeWeakEvents;
    } else if (idSound == 1) {
        return &mDrumMidTomEvents;
    } else if (idSound == 2) {
        return &mDrumShareEvents;
    }
    return nullptr;
}

vector<int64_t> *ZDrumMachine::getSoundBank(int idSound) {
    if (idSound == 0) {
        return &mMetronomeWeakBank;
    } else if (idSound == 1) {
        return &mDrumMidTomBank;
    } else if (idSound == 2) {
        return &mDrumSnareBank;
    }
    return nullptr;
}

void ZDrumMachine::setPatternForSound(int idSound, const vector<int> &pattern) {
    LOGD("Set pattern for idSound = %d, pattern = %s", idSound, toString(pattern).c_str());
    auto particularEvents = getSoundEvents(idSound);
    auto bank = getSoundBank(idSound);

    // clear
    (*bank).clear();
    (*particularEvents).clear();

    for (auto b: pattern) (*bank).push_back(b);
    for (auto t: pattern) (*particularEvents).push(t);
}

void ZDrumMachine::scheduleMetronomeEvents() {
    //for (auto t : kTestMetronomeEvents) mMetronomeWeakEvents.push(t);
    for (auto t : mMetronomeWeakBank) mMetronomeWeakEvents.push(t);
}

void ZDrumMachine::scheduleDrumMidTomEvents() {
    for (auto t : mDrumMidTomBank) mDrumMidTomEvents.push(t);
}

void ZDrumMachine::scheduleDrumSnareEvents() {
    for (auto t : mDrumSnareBank) mDrumShareEvents.push(t);
}