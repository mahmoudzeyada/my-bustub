#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <vector>

#include "buffer/frame.h"
#include "buffer/replacer.h"
#include "common/config.h"

using std::vector;
namespace bustub {
class ClockReplacer : public Replacer {
 public:
  explicit ClockReplacer(size_t num_pages);

  ~ClockReplacer() override;

  bool Victim(frame_id_t *frame_id) override;

  void Pin(frame_id_t frame_id) override;

  void Unpin(frame_id_t frame_id) override;

  size_t Size() override;

 private:
  size_t clock_hand = 0;
  size_t size = 0;
  size_t num_pages;
  vector<Frame> frames;
  size_t NOT_FOUND_INDEX = -1;

  Frame *FindFrame(frame_id_t frame_id);
  size_t FindMinframeIndex();
  void PinFrame(size_t frame_index);
  void UnPinFrame(Frame *frame);
  frame_id_t VictimFrame(size_t frame_index);
  size_t FindFrameIndex(frame_id_t frame_id);
  bool VictimMinFrame(frame_id_t *frame_id);
  void UpdateClockHand(size_t frame_index);
};

}  // namespace bustub
