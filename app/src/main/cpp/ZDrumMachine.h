//
// Created by Ziginsider on 10/21/2021.
//

#ifndef ZDRUMMACHINE_ZDRUMMACHINE_H
#define ZDRUMMACHINE_ZDRUMMACHINE_H

#include <future>
#include <android/asset_manager.h>
#include <oboe/Oboe.h>

#include "utils/LockFreeQueue.h"
#include "utils/UtilityFunctions.h"
#include "constants/ZDrumMachineConstants.h"
#include "audio/Player.h"
#include "audio/Mixer.h"
#include "audio/AAssetDataSource.h"

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
    explicit ZDrumMachine(AAssetManager &, int);

    // Start playing
    void start(int bpm);

    // Stop playing
    void stop();

    // Pause playing
    void pause();

    // Sound Events
    void setPatternForSound(int idSound, const vector<int>& pattern);

    DataCallbackResult
    onAudioReady(AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

    void onErrorAfterClose(AudioStream *stream, Result result) override;

    // Audio stream
    atomic<int64_t> mSongPositionMs{0};

    int64_t getSongPosition() const { return mSongPositionMs / mBpm; }

private:
    AAssetManager &mAssetManager;
    int mBpmReal;
    shared_ptr<AudioStream> mAudioStream;

    // Sounds
    unique_ptr<Player> mMetronomeWeakSound;
    unique_ptr<Player> mDrumMidTomeSound;
    unique_ptr<Player> mDrumShareSound;

    //bpm
    double mBpm{0.00};

    // Mixer
    Mixer mMixer;

    // Audio stream
    atomic<int64_t> mCurrentFrame{0};
    atomic<int64_t> mMultiplierMetronomeWeak{0};
    atomic<int64_t> mMultiplierDrumMidTom{0};
    atomic<int64_t> mMultiplierDrumSnare{0};

    // Sound Events
    LockFreeQueue<int64_t, kMaxQueueItems> mMetronomeWeakEvents;
    LockFreeQueue<int64_t, kMaxQueueItems> mDrumMidTomEvents;
    LockFreeQueue<int64_t, kMaxQueueItems> mDrumShareEvents;

    // Sound pattern bank. NB: Sound events are the work queues and can be changed during playing.
    // Sound bank otherwise keep sound pattern during the playing session.
    vector<int64_t> mMetronomeWeakBank = vector<int64_t>(kMaxQueueItems, false);
    vector<int64_t> mDrumMidTomBank = vector<int64_t>(kMaxQueueItems, false);
    vector<int64_t> mDrumSnareBank = vector<int64_t>(kMaxQueueItems, false);

    LockFreeQueue<int64_t, kMaxQueueItems> *getSoundEvents(int idSound);
    vector<int64_t> *getSoundBank(int idSound);

    // State
    atomic<PlayingState> mPlayingState{PlayingState::Stopped};

    // Functions
    future<void> mLoadingResult;

    void load();

    bool openStream();

    bool setupAudioSources();

    void scheduleMetronomeEvents();

    void scheduleDrumMidTomEvents();

    void scheduleDrumSnareEvents();

    void release();

};

#endif //ZDRUMMACHINE_ZDRUMMACHINE_H
