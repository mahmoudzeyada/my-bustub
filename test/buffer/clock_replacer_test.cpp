//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer_test.cpp
//
// Identification: test/buffer/clock_replacer_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <thread>  // NOLINT
#include <vector>

#include "buffer/clock_replacer.h"
#include "gtest/gtest.h"

namespace bustub {

TEST(ClockReplacerTest, SampleTest) {
  ClockReplacer clock_replacer(7);

  clock_replacer.Unpin(1);
  clock_replacer.Unpin(2);
  clock_replacer.Unpin(3);
  clock_replacer.Unpin(4);
  clock_replacer.Unpin(5);
  clock_replacer.Unpin(6);
  clock_replacer.Unpin(1);
  EXPECT_EQ(6, clock_replacer.Size());

  int value;
  clock_replacer.Victim(&value);
  EXPECT_EQ(1, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(2, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(3, value);
  EXPECT_EQ(3, clock_replacer.Size());

  clock_replacer.Pin(3);
  clock_replacer.Pin(4);
  EXPECT_EQ(2, clock_replacer.Size());
  clock_replacer.Unpin(4);
  clock_replacer.Victim(&value);
  EXPECT_EQ(5, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(6, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(4, value);
}

TEST(ClockReplacerTest, UnPinAllTwice) {
  ClockReplacer clock_replacer(7);

  clock_replacer.Unpin(1);
  clock_replacer.Unpin(2);
  clock_replacer.Unpin(3);
  clock_replacer.Unpin(4);
  clock_replacer.Unpin(5);
  clock_replacer.Unpin(6);
  clock_replacer.Unpin(1);
  clock_replacer.Unpin(1);
  clock_replacer.Unpin(2);
  clock_replacer.Unpin(3);
  clock_replacer.Unpin(4);
  clock_replacer.Unpin(5);
  clock_replacer.Unpin(6);
  clock_replacer.Unpin(1);
  clock_replacer.Unpin(7);
  EXPECT_EQ(7, clock_replacer.Size());
}

TEST(ClockReplacerTest, VictimAll) {
  ClockReplacer clock_replacer(7);

  clock_replacer.Unpin(1);
  clock_replacer.Unpin(2);
  clock_replacer.Unpin(3);
  clock_replacer.Unpin(4);
  clock_replacer.Unpin(5);
  clock_replacer.Unpin(6);
  clock_replacer.Unpin(7);
  EXPECT_EQ(7, clock_replacer.Size());
  int value;
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(0, clock_replacer.Size());
  bool res = clock_replacer.Victim(&value);
  EXPECT_EQ(false,res);
}


TEST(ClockReplacerTest, VictimFirstElementAfterFullRotation) {
  ClockReplacer clock_replacer(7);

  clock_replacer.Unpin(1);
  clock_replacer.Unpin(2);
  clock_replacer.Unpin(3);
  clock_replacer.Unpin(4);
  clock_replacer.Unpin(5);
  clock_replacer.Unpin(6);

  int value;
  clock_replacer.Victim(&value);
  clock_replacer.Unpin(1);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(1, value);
  EXPECT_EQ(0, clock_replacer.Size());
}

}
