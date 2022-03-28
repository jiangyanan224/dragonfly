// Copyright 2022, Roman Gershman.  All rights reserved.
// See LICENSE for licensing terms.
//

#pragma once

#include <cstdint>

namespace dfly {

class ExpirePeriod {
 public:
  ExpirePeriod() : val_(0), gen_(0), precision_(0) {
    static_assert(sizeof(ExpirePeriod) == 4);
  }

  explicit ExpirePeriod(uint64_t ms, unsigned gen = 0) : ExpirePeriod() {
    Set(ms);
  }

  // in milliseconds
  uint64_t duration() const {
    return precision_ ? val_ * 1000 : val_;
  }

  unsigned generation() const {
    return gen_;
  }

  void Set(uint64_t ms);

 private:
  uint32_t val_ : 25;
  uint32_t gen_ : 6;
  uint32_t precision_ : 1;  // 0 - ms, 1 - sec.
};

inline void ExpirePeriod::Set(uint64_t ms) {
  constexpr uint64_t kBarrier = (1ULL << 25);

  if (ms < kBarrier) {
    val_ = ms;
    precision_ = 0;
    return;
  }

  precision_ = 1;
  if (ms < kBarrier << 10) {
    ms = (ms + 500) / 1000;
  }
  val_ = ms >= kBarrier ? kBarrier - 1 : ms;
}

}  // namespace dfly