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
#include "audio/Player.h"
#include "audio/Mixer.h"

using namespace oboe;
using namespace std;

enum class PlayingState {
    Stopped,
    Paused,
    Playing,
    FailedToLoad
};

class ZDrumMachine : public AudioStreamDataCallback, AudioStreamErrorCallback {
public:
    explicit ZDrumMachine(AAssetManager &);

    void start();

    void stop();

    DataCallbackResult
    onAudioReady(AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

    void onErrorAfterClose(AudioStream *stream, Result result) override;

private:
    AAssetManager &mAssetManager;
    shared_ptr<AudioStream> mAudioStream;

    // Sounds
    unique_ptr<Player> mMetronomeSound;
    unique_ptr<Player> mDrumMidTomeSound;
    unique_ptr<Player> mDrumShareSound;

    // Mixer
    Mixer mMixer;

    // Audio stream
    atomic<int64_t> mCurrentFrame{0};
    atomic<int64_t> mSongPositionMs{0};

    // Claps
    LockFreeQueue<int64_t, kMaxQueueItems> mClapMetronomeSound;
    LockFreeQueue<int64_t, kMaxQueueItems> mClapDrumMidTomSound;
    LockFreeQueue<int64_t, kMaxQueueItems> mClapDrumShareSound;

    // State
    atomic<PlayingState> mPlayingState {PlayingState::Stopped};

    // Functions
    void Load();
    bool openStream();
    bool setupAudioSources();
    void scheduleSongEvents();

};

#endif //ZDRUMMACHINE_ZDRUMMACHINE_H
