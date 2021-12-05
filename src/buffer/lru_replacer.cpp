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
#include "buffer/lru_replacer.h"
#include <iostream>
#include "common/logger.h"
#include <algorithm>

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) {
  this->num_pages_ = num_pages;
}

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  if (pool_.size()) {
    *frame_id = pool_.front();
    pool_.erase(pool_.begin());
    return true;
  }
  return false;
}


void LRUReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  for (auto it = pool_.begin(); it != pool_.end(); ++it){
    if (*it == frame_id) {
      pool_.erase(it);
      return;
    }
  }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  if (pool_.size() > num_pages_) {
    return;
  }
  if(std::find(pool_.begin(), pool_.end(), frame_id) != pool_.end()) {
    return;
  }
  pool_.emplace_back(frame_id);
}

size_t LRUReplacer::Size() {
  std::lock_guard<std::mutex> lock(latch_);
  return pool_.size();
}

}  // namespace bustub
