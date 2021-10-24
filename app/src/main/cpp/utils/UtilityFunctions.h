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

#ifndef ZDRUMMACHINE_UTILITYFUNCTIONS_H
#define ZDRUMMACHINE_UTILITYFUNCTIONS_H

#include <stdint.h>
#include <string>

constexpr int64_t kMillisecondsInSecond = 1000;

int64_t nowUptimeMillis();

constexpr int64_t convertFramesToMillis(const int64_t frames, const int sampleRate) {
    return static_cast<int64_t>((static_cast<double>(frames) / sampleRate) * kMillisecondsInSecond);
}

// std::vector to std::string
template<typename T>
std::string toString(const std::vector<T> &vec) {
    std::string res = "[";
    for (auto element: vec) {
        res += std::to_string(element) + ",";
    }
    res += "]";

    return res;
}

#endif //ZDRUMMACHINE_UTILITYFUNCTIONS_H
