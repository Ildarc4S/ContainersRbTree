#include <gtest/gtest.h>
#include "../../include/s21_set.h"

TEST(SetTest, DefaultConstructor) {
  s21::Set<int> set;
  EXPECT_TRUE(set.Empty());
}

TEST(SetTest, InitializerListConstructor) {
  s21::Set<int> set{1, 2, 3, 4};
  EXPECT_EQ(set.Size(), 4);
}

TEST(SetTest, CopyConstructor) {
    s21::Set<int> set1{1, 2, 3};
    s21::Set<int> set2(set1);
    EXPECT_EQ(set2.Size(), 3);
}

TEST(SetTest, MoveConstructor) {
    s21::Set<int> set1{1, 2, 3};
    s21::Set<int> set2(std::move(set1));
    EXPECT_EQ(set2.Size(), 3);
    EXPECT_TRUE(set1.Empty());
}

TEST(SetTest, CopyAssignment) {
    s21::Set<int> set1{1, 2, 3};
    s21::Set<int> set2;
    set2 = set1;
    EXPECT_EQ(set2.Size(), 3);
}

TEST(SetTest, MoveAssignment) {
    s21::Set<int> set1{1, 2, 3};
    s21::Set<int> set2;
    set2 = std::move(set1);
    EXPECT_EQ(set2.Size(), 3);
    EXPECT_TRUE(set1.Empty());
}

TEST(SetTest, BeginEnd) {
    s21::Set<int> set{1, 2, 3};
    auto it = set.Begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
}

TEST(SetTest, ConstBeginEnd) {
    const s21::Set<int> set{1, 2, 3};
    auto it = set.Begin();
    EXPECT_EQ(*it, 1);
}

TEST(SetTest, Empty) {
    s21::Set<int> empty_set;
    s21::Set<int> non_empty_set{1, 2};
    EXPECT_TRUE(empty_set.Empty());
    EXPECT_FALSE(non_empty_set.Empty());
}

TEST(SetTest, Size) {
    s21::Set<int> set{1, 2, 3};
    EXPECT_EQ(set.Size(), 3);
}

TEST(SetTest, MaxSize) {
    s21::Set<int> set;
    EXPECT_GT(set.MaxSize(), 0);
}

TEST(SetTest, Clear) {
    s21::Set<int> set{1, 2, 3};
    set.Clear();
    EXPECT_TRUE(set.Empty());
}

TEST(SetTest, Insert) {
    s21::Set<int> set;
    auto result = set.Insert(42);
    EXPECT_TRUE(result.second);
    EXPECT_EQ(*result.first, 42);
}

TEST(SetTest, InsertDuplicate) {
    s21::Set<int> set{42};
    auto result = set.Insert(42);
    EXPECT_FALSE(result.second);
}

TEST(SetTest, Erase) {
    s21::Set<int> set{1, 2, 3};
    auto it = set.Find(2);
    set.Erase(it);
    EXPECT_EQ(set.Size(), 2);
    EXPECT_EQ(set.Find(2), set.End());
}

TEST(SetTest, Swap) {
    s21::Set<int> set1{1, 2};
    s21::Set<int> set2{3, 4};
    set1.Swap(set2);
    EXPECT_EQ(set1.Size(), 2);
    EXPECT_EQ(set2.Size(), 2);
    EXPECT_EQ(*set1.Begin(), 3);
    EXPECT_EQ(*set2.Begin(), 1);
}

TEST(SetTest, FindExisting) {
    s21::Set<int> set{1, 2, 3};
    auto it = set.Find(2);
    EXPECT_NE(it, set.End());
    EXPECT_EQ(*it, 2);
}

TEST(SetTest, FindNonExisting) {
    s21::Set<int> set{1, 2, 3};
    auto it = set.Find(42);
    EXPECT_EQ(it, set.End());
}

TEST(SetTest, ContainsExisting) {
    s21::Set<int> set{1, 2, 3};
    EXPECT_TRUE(set.Contains(2));
}

TEST(SetTest, ContainsNonExisting) {
    s21::Set<int> set{1, 2, 3};
    EXPECT_FALSE(set.Contains(42));
}

TEST(SetTest, MergeBasic) {
  s21::Set<int> set1{1, 2, 3};
  s21::Set<int> set2{3, 4, 5};

  set1.Merge(set2);

  EXPECT_EQ(set1.Size(), 5);  // 1, 2, 3, 4, 5
  EXPECT_EQ(set2.Size(), 1);  // только дубликат 3 остался
}

TEST(SetTest, MergeEmpty) {
    s21::Set<int> set1{1, 2};
    s21::Set<int> set2;

    set1.Merge(set2);
    EXPECT_EQ(set1.Size(), 2);
    EXPECT_TRUE(set2.Empty());
}

// TEST(SetTest, RangeBasedFor) {
//     s21::Set<int> set{1, 2, 3};
//     int sum = 0;
//     for (auto value : set) {
//         sum += value;
//     }
//     EXPECT_EQ(sum, 6);
// }