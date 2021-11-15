#include "buffer/clock_replacer.h"
#include <iostream>
#include "buffer/frame.h"
#include "common/logger.h"

namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {
  this->num_pages = num_pages;
  for (size_t i = clock_hand; i < num_pages; i++) {
    frame_id_t frame_id = i + 1;
    Frame frame = FrameFactory::CreatePinnedFrame(frame_id);
    frames.push_back(frame);
  };
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) {
  for (size_t i = clock_hand; i < num_pages; i++) {
    UpdateClockHand(i);
    Frame &frame = frames[i];
    if (frame.CanBeRemoved()) {
      *frame_id = VictimFrame(i);
      return true;
    }
    frame.Pin();
    frames[i] = frame;
  };
  return VictimMinFrame(frame_id);
}

bool ClockReplacer::VictimMinFrame(frame_id_t *frame_id) {
  size_t min_frame_index = FindMinframeIndex();
  if (min_frame_index == NOT_FOUND_INDEX) {
    frame_id = nullptr;
    return false;
  }
  Frame min_frame = frames[min_frame_index];
  VictimFrame(min_frame_index);
  UpdateClockHand(min_frame_index);
  *frame_id = min_frame.GetId();
  return true;
}

size_t ClockReplacer::FindMinframeIndex() {
  for (size_t i = 0; i < num_pages; i++) {
    Frame frame = frames[i];
    if (!frame.IsRemoved()) {
      return i;
    }
  }
  return NOT_FOUND_INDEX;
}

void ClockReplacer::UpdateClockHand(size_t frame_index) {
  clock_hand = frame_index == num_pages - 1 ? 0 : frame_index + 1;
}

void ClockReplacer::Pin(frame_id_t frame_id) {
  size_t found_index = frame_id - 1;
  Frame frame = frames[found_index];
  !frame.IsRemoved() && VictimFrame(found_index);
}

frame_id_t ClockReplacer::VictimFrame(size_t frame_index) {
  Frame frame = frames[frame_index];
  frame.Remove();
  frames[frame_index] = frame;
  size--;
  return frame.GetId();
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
  size_t frame_index = frame_id - 1;
  Frame &frame = frames.at(frame_index);
  if (!frame.IsUnpinned()) {
    frame.UnPin();
    size++;
  }
}

size_t ClockReplacer::Size() { return size; }

}  // namespace bustub
