#include <gtest/gtest.h>
#include "../../include/s21_multiset.h"

TEST(MultisetTest, DefaultConstructor) {
  s21::Multiset<int> multiset;
  EXPECT_TRUE(multiset.Empty());
}

TEST(MultisetTest, InitializerListConstructor) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 3, 3};
  EXPECT_EQ(multiset.Size(), 6);
}

TEST(MultisetTest, CopyConstructor) {
  s21::Multiset<int> multiset1{1, 2, 2, 3};
  s21::Multiset<int> multiset2(multiset1);
  EXPECT_EQ(multiset2.Size(), 4);
}

TEST(MultisetTest, MoveConstructor) {
  s21::Multiset<int> multiset1{1, 2, 2, 3};
  s21::Multiset<int> multiset2(std::move(multiset1));
  EXPECT_EQ(multiset2.Size(), 4);
  EXPECT_TRUE(multiset1.Empty());
}

TEST(MultisetTest, CopyAssignment) {
  s21::Multiset<int> multiset1{1, 2, 2, 3};
  s21::Multiset<int> multiset2;
  multiset2 = multiset1;
  EXPECT_EQ(multiset2.Size(), 4);
}

TEST(MultisetTest, MoveAssignment) {
  s21::Multiset<int> multiset1{1, 2, 2, 3};
  s21::Multiset<int> multiset2;
  multiset2 = std::move(multiset1);
  EXPECT_EQ(multiset2.Size(), 4);
  EXPECT_TRUE(multiset1.Empty());
}

TEST(MultisetTest, BeginEnd) {
  s21::Multiset<int> multiset{2, 1, 2, 3};
  auto it = multiset.Begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
}

TEST(MultisetTest, ConstBeginEnd) {
  const s21::Multiset<int> multiset{2, 1, 2, 3};
  auto it = multiset.Begin();
  EXPECT_EQ(*it, 1);
}

TEST(MultisetTest, Empty) {
  s21::Multiset<int> empty_multiset;
  s21::Multiset<int> non_empty_multiset{1, 2, 2};
  EXPECT_TRUE(empty_multiset.Empty());
  EXPECT_FALSE(non_empty_multiset.Empty());
}

TEST(MultisetTest, Size) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 3, 3};
  EXPECT_EQ(multiset.Size(), 6);
}

TEST(MultisetTest, MaxSize) {
  s21::Multiset<int> multiset;
  EXPECT_GT(multiset.MaxSize(), 0);
}

TEST(MultisetTest, Clear) {
  s21::Multiset<int> multiset{1, 2, 2, 3};
  multiset.Clear();
  EXPECT_TRUE(multiset.Empty());
}

TEST(MultisetTest, Insert) {
  s21::Multiset<int> multiset;
  auto result = multiset.Insert(42);
  EXPECT_EQ(*result, 42);
}

TEST(MultisetTest, InsertDuplicate) {
  s21::Multiset<int> multiset{42};
  auto result = multiset.Insert(42);
  EXPECT_EQ(*result, 42);
  EXPECT_EQ(multiset.Size(), 2);
}

TEST(MultisetTest, Erase) {
  s21::Multiset<int> multiset{1, 2, 2, 3};
  auto it = multiset.Find(2);
  multiset.Erase(it);
  EXPECT_EQ(multiset.Size(), 3);

  it = multiset.Find(2);
  EXPECT_NE(it, multiset.End());
  EXPECT_EQ(*it, 2);
}

TEST(MultisetTest, Swap) {
  s21::Multiset<int> multiset1{1, 1, 2};
  s21::Multiset<int> multiset2{3, 3, 4};
  multiset1.Swap(multiset2);
  EXPECT_EQ(multiset1.Size(), 3);
  EXPECT_EQ(multiset2.Size(), 3);
  EXPECT_EQ(*multiset1.Begin(), 3);
  EXPECT_EQ(*multiset2.Begin(), 1);
}

TEST(MultisetTest, FindExisting) {
  s21::Multiset<int> multiset{1, 2, 2, 3};
  auto it = multiset.Find(2);
  EXPECT_NE(it, multiset.End());
  EXPECT_EQ(*it, 2);
}

TEST(MultisetTest, FindNonExisting) {
  s21::Multiset<int> multiset{1, 2, 2, 3};
  auto it = multiset.Find(42);
  EXPECT_EQ(it, multiset.End());
}

TEST(MultisetTest, ContainsExisting) {
  s21::Multiset<int> multiset{1, 2, 2, 3};
  EXPECT_TRUE(multiset.Contains(2));
}

TEST(MultisetTest, ContainsNonExisting) {
  s21::Multiset<int> multiset{1, 2, 2, 3};
  EXPECT_FALSE(multiset.Contains(42));
}

TEST(MultisetTest, LowerBound) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 4};
  auto it = multiset.LowerBound(2);
  EXPECT_NE(it, multiset.End());
  EXPECT_EQ(*it, 2);

  auto prev = it;
  --prev;
  EXPECT_EQ(*prev, 1);
}

TEST(MultisetTest, UpperBound) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 4};
  auto it = multiset.UpperBound(2);
  EXPECT_NE(it, multiset.End());
  EXPECT_EQ(*it, 3);

  auto prev = it;
  --prev;
  EXPECT_EQ(*prev, 2);
}

TEST(MultisetTest, EqualRange) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 4};
  auto range = multiset.EqualRange(2);

  EXPECT_NE(range.first, multiset.End());
  EXPECT_NE(range.second, multiset.End());
  EXPECT_EQ(*range.first, 2);
  EXPECT_EQ(*range.second, 3);

  size_t count = 0;
  for (auto it = range.first; it != range.second; ++it) {
      EXPECT_EQ(*it, 2);
      ++count;
  }
  EXPECT_EQ(count, 2);
}

TEST(MultisetTest, Count) {
s21::Multiset<int> multiset{1, 2, 2, 3, 3, 3};
EXPECT_EQ(multiset.Count(1), 1);
EXPECT_EQ(multiset.Count(2), 2);
EXPECT_EQ(multiset.Count(3), 3);
EXPECT_EQ(multiset.Count(4), 0);
}

TEST(MultisetTest, MergeBasic) {
s21::Multiset<int> multiset1{1, 2, 2};
s21::Multiset<int> multiset2{2, 3, 3};

multiset1.Merge(multiset2);

EXPECT_EQ(multiset1.Size(), 6);
EXPECT_EQ(multiset2.Size(), 0);
}

TEST(MultisetTest, MergeEmpty) {
  s21::Multiset<int> multiset1{1, 2};
  s21::Multiset<int> multiset2;

  multiset1.Merge(multiset2);
  EXPECT_EQ(multiset1.Size(), 2);
  EXPECT_TRUE(multiset2.Empty());
}

TEST(MultisetTest, Iteration) {
  s21::Multiset<int> multiset{3, 1, 2, 2, 1};
  std::vector<int> values;

  for (auto it = multiset.Begin(); it != multiset.End(); ++it) {
    values.push_back(*it);
  }

  std::vector<int> expected{1, 1, 2, 2, 3};
  EXPECT_EQ(values, expected);
}