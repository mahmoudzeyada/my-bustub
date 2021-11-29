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
#include <queue>

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) { this->num_pages = num_pages; }

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  bool should_victim = !pool.empty();
  if (should_victim) {
    *frame_id = pool.front();
    pool.pop();
  }
  return should_victim;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  queue tmp_pool = pool;
  queue<frame_id_t> new_pool;
  frame_id_t q_element;
  while (!tmp_pool.empty()) {
    q_element = tmp_pool.front();
    if (q_element != frame_id) {
      new_pool.push(q_element);
    }
    tmp_pool.pop();
  }
  pool = new_pool;
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  if (ShouldUnPinFrame(frame_id)) {
    pool.push(frame_id);
  }
}

bool LRUReplacer::ShouldUnPinFrame(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(latch_);
  bool should_bin = Size() < num_pages;
  if (should_bin) {
    queue tmp_pool = pool;
    frame_id_t q_element;
    while (!tmp_pool.empty()) {
      q_element = tmp_pool.front();
      if (q_element == frame_id) {
        should_bin = false;
        break;
      }
      tmp_pool.pop();
    }
  }
  return should_bin;
}

size_t LRUReplacer::Size() { return pool.size(); }

}  // namespace bustub
