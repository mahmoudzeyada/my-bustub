//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include <list>
#include <unordered_map>

namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  replacer_ = new LRUReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete replacer_;
}

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  std::lock_guard<std::mutex> lock(latch_);
  auto iterator = page_table_.find(page_id);
  if (iterator != page_table_.end()) {
    replacer_->Pin(iterator->second);
    return GetPages() + iterator->second;
  }
  auto frame_id = Victim();
  if (frame_id == -1) {
    return nullptr;
  }
  return ProcessNewPage(frame_id, page_id);
}

bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  std::lock_guard<std::mutex> lock(latch_);
  auto iterator = page_table_.find(page_id);
  if (iterator == page_table_.end()) {
    return false;
  }
  auto page = GetPages() + iterator->second;
  page->pin_count_ = page->GetPinCount() == 0 ? 0 : page->GetPinCount() - 1;
  if (page->GetPinCount() == 0) {
    replacer_->Unpin(iterator->second);
  }
  page->is_dirty_ |= is_dirty;
  return true;
}

void BufferPoolManager::FlushAllPagesImpl() {
  std::lock_guard<std::mutex> lock(latch_);
  for (auto &it : page_table_) {
    auto page_id = it.first;
    FlushPageImpl(page_id);
  }
}

bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  std::lock_guard<std::mutex> lock(latch_);
  auto iterator = page_table_.find(page_id);
  if (iterator == page_table_.end()) {
    return false;
  }
  auto page = GetPages() + iterator->second;
  disk_manager_->WritePage(page->GetPageId(), page->GetData());
  page->is_dirty_ = false;
  return true;
}

Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
  std::lock_guard<std::mutex> lock(latch_);
  if (IsAllBinned()) {
    return nullptr;
  }
  auto frame_id = Victim();

  if (frame_id == -1) {
    return nullptr;
  }
  *page_id = disk_manager_->AllocatePage();
  auto page = ProcessNewPage(frame_id, *page_id);
  return page;
}

bool BufferPoolManager::IsAllBinned() {
  for (size_t i = 0; i < pool_size_; i++) {
    auto page = GetPages() + i;
    if (page->GetPinCount() == 0) {
      return false;
    }
  }
  return true;
}

frame_id_t BufferPoolManager::Victim() {
  frame_id_t frame_id;
  if (!free_list_.empty()) {
    frame_id = free_list_.front();
    free_list_.pop_front();
  } else {
    auto is_Found = replacer_->Victim(&frame_id);
    if (!is_Found) {
      return -1;
    }
  }
  auto replacment_page = GetPages() + frame_id;
  page_table_.erase(replacment_page->GetPageId());
  if (replacment_page->IsDirty()) {
    disk_manager_->WritePage(replacment_page->GetPageId(), replacment_page->GetData());
  }
  return frame_id;
}

Page *BufferPoolManager::ProcessNewPage(frame_id_t frame_id, page_id_t page_id) {
  auto page = GetPages() + frame_id;
  page->ResetMemory();
  page->page_id_ = page_id;
  page->pin_count_ = 1;
  page->is_dirty_ = false;
  std::pair<page_id_t, frame_id_t> pair(page_id, frame_id);
  page_table_.insert(pair);
  disk_manager_->ReadPage(page_id, page->GetData());
  return page;
}

bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  std::lock_guard<std::mutex> lock(latch_);
  auto iterator = page_table_.find(page_id);
  if (iterator == page_table_.end()) {
    return true;
  }
  auto page = GetPages() + iterator->second;
  if (page->GetPinCount() > 0) {
    return false;
  }
  page_table_.erase(page->GetPageId());
  page->ResetMemory();
  free_list_.push_back(iterator->second);
  disk_manager_->DeallocatePage(page_id);
  return true;
}

}  // namespace bustub
