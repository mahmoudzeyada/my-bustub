#include "buffer/frame.h"

namespace bustub {
Frame::Frame(frame_id_t id, int ref) {
  this->id = id;
  this->ref = ref;
}
Frame::~Frame() = default;
frame_id_t Frame::GetId() { return id; }
void Frame::Pin() {
  if (IsUnpinned()) {
    ref = PIN_VALUE;
    removable = UN_REMOVED_VALUE;

  } else if (!IsRemoved()) {
    Remove();
  }
}
void Frame::UnPin() {
  ref = UNPINNED_VALUE;
  removable = UN_REMOVED_VALUE;
}
bool Frame::IsPinned() { return ref == PIN_VALUE; }
bool Frame::IsUnpinned() { return !IsPinned(); }
void Frame::Remove() {
  removable = REMOVED_VALUE;
  ref = PIN_VALUE;
}
bool Frame::IsRemoved() { return removable == REMOVED_VALUE; }
bool Frame::CanBeRemoved() { return !IsRemoved() && IsPinned(); }
Frame FrameFactory::CreateUnPinnedFrame(frame_id_t id) {
  Frame createdFrame = Frame(id, 1);
  createdFrame.UnPin();
  return createdFrame;
}
Frame FrameFactory::CreatePinnedFrame(frame_id_t id) {
  Frame createdFrame = Frame(id, 1);
  createdFrame.Pin();
  return createdFrame;
}
};  // namespace bustub