/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ZDRUMMACHINE_ZDRUMMACHINECONSTANTS_H
#define ZDRUMMACHINE_ZDRUMMACHINECONSTANTS_H

constexpr int kBufferSizeInBursts = 2; // Use 2 bursts as the buffer size (double buffer)
constexpr int kMaxQueueItems = 8; // Must be power of 2

// Frames per second
constexpr int32_t kFramesPerSecond = 48000;

// Stream's channel count - Stereo
constexpr int kChannelCount = 2;

// Filename for metronome sound asset (in assets folder)
constexpr char kMetronomeWeakFilename[] { "metronome_weak.mp3" };

// Filename for the drum sounds
constexpr char kMidTomFilename[] { "mid_tom.mp3" };
constexpr char kSnareDrumFilename[] { "snare_drum.mp3" };

// mock bpm
constexpr int kStandardBpm = 60;

struct AudioProperties {
    int32_t channelCount;
    int32_t sampleRate;
};

#endif //ZDRUMMACHINE_ZDRUMMACHINECONSTANTS_H