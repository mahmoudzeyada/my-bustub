//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include "buffer/clock_replacer.h"
#include <iostream>
#include "common/logger.h"

namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {
  this->num_pages = num_pages;
  pool_.clear();
  ref_table_.clear();
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  while (true) {
    auto frame_id_item = pool_.at(clock_hand_);
    auto pair = ref_table_.find(frame_id_item);
    if (pair->second) {
      ref_table_[frame_id_item] = false;
      IncreaseClockHand();
    } else {
      *frame_id = frame_id_item;
      PinFrame(*frame_id);
      break;
    }
  }
  return true;
}

void ClockReplacer::IncreaseClockHand() {
  if (clock_hand_ == pool_.size() - 1) {
    clock_hand_ = 0;
  } else {
    clock_hand_++;
  }
}

void ClockReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  PinFrame(frame_id);
}

void ClockReplacer::PinFrame(frame_id_t frame_id) {
  auto iterator = ref_table_.find(frame_id);
  bool is_found = iterator != ref_table_.end();
  if (is_found) {
    ref_table_.erase(frame_id);
    for (size_t i = 0; i < pool_.size(); i++) {
      auto frame_id_item = pool_.at(i);
      if (frame_id_item == frame_id) {
        pool_.erase(pool_.begin() + i);
        DecreaseClockHand();
        break;
      }
    }
  }
}

void ClockReplacer::DecreaseClockHand() {
  if (clock_hand_ == 0) {
    clock_hand_ = 0;
  } else {
    clock_hand_--;
  }
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  auto iterator = ref_table_.find(frame_id);
  bool is_already_found = iterator != ref_table_.end();
  auto has_space = pool_.size() < num_pages;
  auto isFirstFrame = pool_.empty();
  if (is_already_found) {
    ref_table_[frame_id] = true;
  } else if (has_space) {
    pool_.push_back(frame_id);
    auto pair = std::make_pair(frame_id, true);
    ref_table_.emplace(pair);
    if (isFirstFrame) {
      clock_hand_ = 0;
    }
  }
}

size_t ClockReplacer::Size() {
  std::lock_guard<std::mutex> lock(latch_);
  return pool_.size();
}

}  // namespace bustub
