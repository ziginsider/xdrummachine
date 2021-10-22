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

// The game will first demonstrate the pattern which the user should copy. It does this by
// "clapping" (playing a clap sound) at certain times during the song. We can specify these times
// here in milliseconds. Our backing track has a tempo of 120 beats per minute, which is 2 beats per
// second. This means a pattern of 3 claps starting on the first beat of the first bar would mean
// playing claps at 0ms, 500ms and 1000ms
constexpr int64_t kTestMetronomeEvents[] { 0, 500, 1000, 5000, 7000, 9000, 10000, 13500 };

// We then want the user to tap on the screen exactly 4 beats after the first clap so we add clap
// windows at 2000ms, 2500ms and 3000ms (or 2, 2.5 and 3 seconds). @see getTapResult for more info.
constexpr int64_t kTestDrumMidTomEvents[] { 2000, 2500, 3000 };
constexpr int64_t kTestDrumShareEvents[] { 3000, 3500, 4000 };

struct AudioProperties {
    int32_t channelCount;
    int32_t sampleRate;
};

#endif //ZDRUMMACHINE_ZDRUMMACHINECONSTANTS_H