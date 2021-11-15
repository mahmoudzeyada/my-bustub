#pragma once

#include "common/config.h"

namespace bustub {
class Frame {
 public:
  Frame(frame_id_t id, int ref);
  ~Frame();
  frame_id_t GetId();
  void Pin();
  void UnPin();
  bool IsPinned();
  bool IsUnpinned();
  void Remove();
  bool IsRemoved();
  bool CanBeRemoved();

 private:
  frame_id_t id;
  int ref;
  int removable;
  int PIN_VALUE = 1;
  int UNPINNED_VALUE = 0;
  int REMOVED_VALUE = 1;
  int UN_REMOVED_VALUE = 0;
};

class FrameFactory {
 public:
  static Frame CreateUnPinnedFrame(frame_id_t id);
  static Frame CreatePinnedFrame(frame_id_t id);
};
}  // namespace bustub