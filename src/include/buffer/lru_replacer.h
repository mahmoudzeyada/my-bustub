//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.h
//
// Identification: src/include/buffer/lru_replacer.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <vector>
#include <unordered_map>

#include "buffer/replacer.h"
#include "common/config.h"

namespace bustub {
using namespace std;
/**
 * LRUReplacer implements the lru replacement policy, which approximates the Least Recently Used policy.
 */

class LRUReplacer : public Replacer {
 public:
  /**
   * Create a new LRUReplacer.
   * @param num_pages the maximum number of pages the LRUReplacer will be required to store
   */
  explicit LRUReplacer(size_t num_pages);

  /**
   * Destroys the LRUReplacer.
   */
  ~LRUReplacer() override;

  bool Victim(frame_id_t *frame_id) override;

  void Pin(frame_id_t frame_id) override;

  void Unpin(frame_id_t frame_id) override;

  size_t Size() override;

 private:
  std::mutex latch_;
  vector<frame_id_t> pool_;
  std::unordered_map<frame_id_t, bool> ref_table_;
  size_t num_pages;
  size_t clock_hand_ = -1;
  void IncreaseClockHand();
  void DecreaseClockHand();
  void PinFrame(frame_id_t frame_id);
};

}  // namespace bustub
