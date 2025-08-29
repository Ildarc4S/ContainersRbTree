#include <gtest/gtest.h>
#include "../../include/s21_multiset.h"

TEST(MultisetTest, DefaultConstructor) {
  s21::Multiset<int> multiset;
  EXPECT_TRUE(multiset.Empty());
  EXPECT_EQ(multiset.Size(), 0);
}

TEST(MultisetTest, InitializerListConstructor) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 3, 3};
  EXPECT_EQ(multiset.Size(), 6);
  EXPECT_EQ(multiset.Count(1), 1);
  EXPECT_EQ(multiset.Count(2), 2);
  EXPECT_EQ(multiset.Count(3), 3);
}

TEST(MultisetTest, CopyConstructor) {
  s21::Multiset<int> multiset1{1, 2, 2};
  s21::Multiset<int> multiset2(multiset1);

  EXPECT_EQ(multiset2.Size(), 3);
  EXPECT_EQ(multiset2.Count(1), 1);
  EXPECT_EQ(multiset2.Count(2), 2);
}

TEST(MultisetTest, MoveConstructor) {
  s21::Multiset<int> multiset1{1, 2, 2};
  s21::Multiset<int> multiset2(std::move(multiset1));

  EXPECT_EQ(multiset2.Size(), 3);
  EXPECT_TRUE(multiset1.Empty());
}

TEST(MultisetTest, CopyAssignment) {
  s21::Multiset<int> multiset1{1, 2, 2};
  s21::Multiset<int> multiset2;
  multiset2 = multiset1;

  EXPECT_EQ(multiset2.Size(), 3);
  EXPECT_EQ(multiset2.Count(2), 2);
}

TEST(MultisetTest, MoveAssignment) {
  s21::Multiset<int> multiset1{1, 2, 2};
  s21::Multiset<int> multiset2;
  multiset2 = std::move(multiset1);

  EXPECT_EQ(multiset2.Size(), 3);
  EXPECT_TRUE(multiset1.Empty());
}

TEST(MultisetTest, InsertDuplicates) {
  s21::Multiset<int> multiset;

  auto it1 = multiset.Insert(5);
  EXPECT_EQ(*it1, 5);
  EXPECT_EQ(multiset.Size(), 1);

  auto it2 = multiset.Insert(5);
  EXPECT_EQ(*it2, 5);
  EXPECT_EQ(multiset.Size(), 2);

  auto it3 = multiset.Insert(5);
  EXPECT_EQ(*it3, 5);
  EXPECT_EQ(multiset.Size(), 3);

  EXPECT_EQ(multiset.Count(5), 3);
}

TEST(MultisetTest, CountMethod) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};

  EXPECT_EQ(multiset.Count(1), 1);
  EXPECT_EQ(multiset.Count(2), 2);
  EXPECT_EQ(multiset.Count(3), 3);
  EXPECT_EQ(multiset.Count(4), 4);
  EXPECT_EQ(multiset.Count(999), 0);
}

TEST(MultisetTest, FindMethod) {
  s21::Multiset<int> multiset{1, 2, 2, 3};

  auto it = multiset.Find(2);
  EXPECT_NE(it, multiset.End());
  EXPECT_EQ(*it, 2);

  const auto& const_multiset = multiset;
  auto const_it = const_multiset.Find(2);
  EXPECT_NE(const_it, const_multiset.End());
  EXPECT_EQ(*const_it, 2);

  EXPECT_EQ(multiset.Find(999), multiset.End());
}

TEST(MultisetTest, ContainsMethod) {
  s21::Multiset<int> multiset{1, 2, 2};

  EXPECT_TRUE(multiset.Contains(1));
  EXPECT_TRUE(multiset.Contains(2));
  EXPECT_FALSE(multiset.Contains(3));
}

TEST(MultisetTest, EqualRange) {
  s21::Multiset<int> multiset{1, 2, 2, 2, 3, 4, 4};

  auto range = multiset.EqualRange(2);
  size_t count = 0;
  for (auto it = range.first; it != range.second; ++it) {
    EXPECT_EQ(*it, 2);
    count++;
  }
  EXPECT_EQ(count, 3);

  const auto& const_multiset = multiset;
  auto const_range = const_multiset.EqualRange(4);
  count = 0;
  for (auto it = const_range.first; it != const_range.second; ++it) {
    EXPECT_EQ(*it, 4);
    count++;
  }
  EXPECT_EQ(count, 2);

  auto empty_range = multiset.EqualRange(999);
  EXPECT_EQ(empty_range.first, empty_range.second);
}

TEST(MultisetTest, LowerBound) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 4, 4, 4};

  auto lb = multiset.LowerBound(2);
  EXPECT_NE(lb, multiset.End());
  EXPECT_EQ(*lb, 2);

  const auto& const_multiset = multiset;
  auto const_lb = const_multiset.LowerBound(3);
  EXPECT_NE(const_lb, const_multiset.End());
  EXPECT_EQ(*const_lb, 3);

  auto lb2 = multiset.LowerBound(5);
  EXPECT_EQ(lb2, multiset.End());
}

TEST(MultisetTest, UpperBound) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 4, 4, 4};

  auto ub = multiset.UpperBound(2);
  EXPECT_NE(ub, multiset.End());
  EXPECT_EQ(*ub, 3);

  const auto& const_multiset = multiset;
  auto const_ub = const_multiset.UpperBound(3);
  EXPECT_NE(const_ub, const_multiset.End());
  EXPECT_EQ(*const_ub, 4);

  auto ub2 = multiset.UpperBound(5);
  EXPECT_EQ(ub2, multiset.End());
}

TEST(MultisetTest, Erase) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 3, 3};

  size_t initial_size = multiset.Size();
  auto it = multiset.Find(2);

  ASSERT_NE(it, multiset.End());
  multiset.Erase(it);

  EXPECT_EQ(multiset.Size(), initial_size - 1);
  EXPECT_EQ(multiset.Count(2), 1);
}

TEST(MultisetTest, Clear) {
  s21::Multiset<int> multiset{1, 2, 2, 3};
  multiset.Clear();

  EXPECT_TRUE(multiset.Empty());
  EXPECT_EQ(multiset.Size(), 0);
}

TEST(MultisetTest, Swap) {
  s21::Multiset<int> multiset1{1, 2, 2};
  s21::Multiset<int> multiset2{3, 3, 4};

  multiset1.Swap(multiset2);

  EXPECT_EQ(multiset1.Size(), 3);
  EXPECT_EQ(multiset2.Size(), 3);
  EXPECT_EQ(multiset1.Count(3), 2);
  EXPECT_EQ(multiset2.Count(2), 2);
}

TEST(MultisetTest, Merge) {
  s21::Multiset<int> multiset1{1, 2, 2};
  s21::Multiset<int> multiset2{2, 3, 3};

  multiset1.Merge(multiset2);

  EXPECT_EQ(multiset1.Size(), 6);
  EXPECT_EQ(multiset1.Count(1), 1);
  EXPECT_EQ(multiset1.Count(2), 3);
  EXPECT_EQ(multiset1.Count(3), 2);
  EXPECT_TRUE(multiset2.Empty());
}

TEST(MultisetTest, IterationWithDuplicates) {
  s21::Multiset<int> multiset{1, 2, 2, 3, 3, 3};

  std::vector<int> values;
  for (auto it = multiset.Begin(); it != multiset.End(); ++it) {
    values.push_back(*it);
  }

  std::vector<int> expected{1, 2, 2, 3, 3, 3};
  EXPECT_EQ(values, expected);
}

TEST(MultisetTest, ReverseIteration) {
  s21::Multiset<int> multiset{1, 2, 2, 3};

  std::vector<int> reverse_values;
  auto it = multiset.End();
  while (it != multiset.Begin()) {
    --it;
    reverse_values.push_back(*it);
  }

  std::vector<int> expected{3, 2, 2, 1};
  EXPECT_EQ(reverse_values, expected);
}

TEST(MultisetTest, CustomComparator) {
  s21::Multiset<int, std::greater<int>> multiset{1, 2, 2, 3};

  std::vector<int> values;
  for (auto it = multiset.Begin(); it != multiset.End(); ++it) {
    values.push_back(*it);
  }

  std::vector<int> expected{3, 2, 2, 1};
  EXPECT_EQ(values, expected);
}

TEST(MultisetTest, StringMultiset) {
  s21::Multiset<std::string> multiset{"apple", "banana", "apple", "cherry"};

  EXPECT_EQ(multiset.Size(), 4);
  EXPECT_EQ(multiset.Count("apple"), 2);
  EXPECT_EQ(multiset.Count("banana"), 1);
  EXPECT_EQ(multiset.Count("cherry"), 1);
}

TEST(MultisetTest, LargeInsertionWithDuplicates) {
  s21::Multiset<int> multiset;
  const int count = 100;

  for (int i = 0; i < count; ++i) {
    multiset.Insert(i % 10);
  }

  EXPECT_EQ(multiset.Size(), count);

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(multiset.Count(i), count / 10);
  }
}

TEST(MultisetTest, EmptyRangeMethods) {
  s21::Multiset<int> empty_multiset;

  EXPECT_EQ(empty_multiset.EqualRange(1).first, empty_multiset.End());
  EXPECT_EQ(empty_multiset.EqualRange(1).second, empty_multiset.End());
  EXPECT_EQ(empty_multiset.LowerBound(1), empty_multiset.End());
  EXPECT_EQ(empty_multiset.UpperBound(1), empty_multiset.End());
}

TEST(MultisetTest, BoundaryCases) {
  s21::Multiset<int> multiset{1, 3, 3, 5};

  auto lb = multiset.LowerBound(2);
  EXPECT_NE(lb, multiset.End());
  EXPECT_EQ(*lb, 3);

  auto ub = multiset.UpperBound(2);
  EXPECT_NE(ub, multiset.End());
  EXPECT_EQ(*ub, 3);

  auto lb2 = multiset.LowerBound(0);
  EXPECT_NE(lb2, multiset.End());
  EXPECT_EQ(*lb2, 1);

  auto ub2 = multiset.UpperBound(6);
  EXPECT_EQ(ub2, multiset.End());
}

TEST(MultisetTest, MaxSize) {
  s21::Multiset<int> multiset1;
  s21::Multiset<int, std::greater<int>> multiset2;
  s21::Multiset<std::string> multiset3;

  EXPECT_GT(multiset1.MaxSize(), 0);
  EXPECT_GT(multiset2.MaxSize(), 0);
  EXPECT_GT(multiset3.MaxSize(), 0);

  s21::Multiset<int> multiset4;
  EXPECT_EQ(multiset1.MaxSize(), multiset4.MaxSize());

  size_t initial_max_size = multiset1.MaxSize();
  multiset1.Insert(1);
  multiset1.Insert(2);
  EXPECT_EQ(multiset1.MaxSize(), initial_max_size);

  s21::Multiset<int> empty_multiset;
  s21::Multiset<int> filled_multiset{1, 2, 3, 4, 5};
  EXPECT_EQ(empty_multiset.MaxSize(), filled_multiset.MaxSize());

  EXPECT_GE(multiset1.MaxSize(), 1000000);
}

TEST(MultisetTest, ConstBeginEnd) {
  const s21::Multiset<int> const_multiset{1, 2, 2, 3, 3, 3};

  auto const_it = const_multiset.Begin();
  EXPECT_NE(const_it, const_multiset.End());
  EXPECT_EQ(*const_it, 1);

  std::vector<int> const_values;
  for (auto it = const_multiset.Begin(); it != const_multiset.End(); ++it) {
    const_values.push_back(*it);
  }

  std::vector<int> expected{1, 2, 2, 3, 3, 3};
  EXPECT_EQ(const_values, expected);
}

TEST(MultisetTest, ConstBeginEmpty) {
  const s21::Multiset<int> empty_const_multiset;

  EXPECT_EQ(empty_const_multiset.Begin(), empty_const_multiset.End());
}
