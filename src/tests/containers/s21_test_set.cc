#include <gtest/gtest.h>

#include "../../include/s21_set.h"

TEST(SetTest, DefaultConstructor) {
  s21::Set<int> set;
  EXPECT_TRUE(set.Empty());
  EXPECT_EQ(set.Size(), 0);

  s21::Set<int, std::greater<int>> set_greater;
  EXPECT_TRUE(set_greater.Empty());

  s21::Set<std::string, std::greater<std::string>> set_string_greater;
  EXPECT_TRUE(set_string_greater.Empty());
}

TEST(SetTest, InitializerListConstructor) {
  s21::Set<int> set{1, 2, 3};
  EXPECT_EQ(set.Size(), 3);
  EXPECT_TRUE(set.Contains(1));
  EXPECT_TRUE(set.Contains(2));
  EXPECT_TRUE(set.Contains(3));

  s21::Set<int, std::greater<int>> set_greater{1, 2, 3};
  EXPECT_EQ(set_greater.Size(), 3);
  EXPECT_TRUE(set_greater.Contains(3));
  EXPECT_TRUE(set_greater.Contains(1));

  auto it = set_greater.Begin();
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 1);
}

TEST(SetTest, CopyConstructor) {
  s21::Set<int> set1{1, 2};
  s21::Set<int> set2(set1);
  EXPECT_EQ(set2.Size(), 2);
  EXPECT_TRUE(set2.Contains(1));
  EXPECT_TRUE(set2.Contains(2));

  s21::Set<int, std::greater<int>> set3{3, 1};
  s21::Set<int, std::greater<int>> set4(set3);
  EXPECT_EQ(set4.Size(), 2);
  EXPECT_TRUE(set4.Contains(3));
}

TEST(SetTest, MoveConstructor) {
  s21::Set<int> set1{1, 2};
  s21::Set<int> set2(std::move(set1));
  EXPECT_EQ(set2.Size(), 2);
  EXPECT_TRUE(set1.Empty());

  s21::Set<int, std::greater<int>> set3{5, 2};
  s21::Set<int, std::greater<int>> set4(std::move(set3));
  EXPECT_EQ(set4.Size(), 2);
  EXPECT_TRUE(set3.Empty());
  EXPECT_EQ(*set4.Begin(), 5);
}

TEST(SetTest, CopyAssignment) {
  s21::Set<int> set1{1, 2};
  s21::Set<int> set2;
  set2 = set1;
  EXPECT_EQ(set2.Size(), 2);
  EXPECT_TRUE(set2.Contains(1));

  s21::Set<int, std::greater<int>> set3{10};
  s21::Set<int, std::greater<int>> set4;
  set4 = set3;
  EXPECT_EQ(set4.Size(), 1);
  EXPECT_TRUE(set4.Contains(10));
}

TEST(SetTest, MoveAssignment) {
  s21::Set<int> set1{1, 2};
  s21::Set<int> set2;
  set2 = std::move(set1);
  EXPECT_EQ(set2.Size(), 2);
  EXPECT_TRUE(set1.Empty());

  s21::Set<int, std::greater<int>> set3{7, 3};
  s21::Set<int, std::greater<int>> set4;
  set4 = std::move(set3);
  EXPECT_EQ(set4.Size(), 2);
  EXPECT_TRUE(set3.Empty());

  auto it = set4.Begin();
  EXPECT_EQ(*it, 7);
  ++it;
  EXPECT_EQ(*it, 3);
}

TEST(SetTest, CustomComparatorConstructor) {
  struct AbsCompare {
    bool operator()(int a, int b) const { return std::abs(a) < std::abs(b); }
  };

  s21::Set<int, AbsCompare> set{-5, 3, 5};

  EXPECT_EQ(set.Size(), 2);
  EXPECT_TRUE(set.Contains(-5));
  EXPECT_TRUE(set.Contains(3));
  EXPECT_TRUE(set.Contains(5));
}

TEST(SetTest, StringCustomComparator) {
  struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const {
      return strcasecmp(a.c_str(), b.c_str()) < 0;
    }
  };

  s21::Set<std::string, CaseInsensitiveCompare> set{"Apple", "banana",
                                                    "CHERRY"};
  EXPECT_EQ(set.Size(), 3);
  EXPECT_TRUE(set.Contains("Apple"));
  EXPECT_TRUE(set.Contains("banana"));
  EXPECT_TRUE(set.Contains("CHERRY"));
}

TEST(SetTest, InsertMethods) {
  s21::Set<int> set;
  auto result1 = set.Insert(1);
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(*result1.first, 1);

  auto result2 = set.Insert(1);
  EXPECT_FALSE(result2.second);
  EXPECT_EQ(*result2.first, 1);

  s21::Set<int, std::greater<int>> set_greater;
  auto result3 = set_greater.Insert(10);
  EXPECT_TRUE(result3.second);
  EXPECT_EQ(*result3.first, 10);
}

TEST(SetTest, Erase) {
  s21::Set<int> set{1, 2, 3};
  size_t initial_size = set.Size();

  auto it = set.Begin();
  while (it != set.End() && *it != 2) {
    ++it;
  }

  ASSERT_NE(it, set.End());
  ASSERT_EQ(*it, 2);

  set.Erase(it);
  EXPECT_EQ(set.Size(), initial_size - 1);
  EXPECT_FALSE(set.Contains(2));

  s21::Set<int, std::greater<int>> set_greater{5, 3};
  size_t initial_size_greater = set_greater.Size();

  auto it3 = set_greater.Begin();
  ASSERT_NE(it3, set_greater.End());
  ASSERT_EQ(*it3, 5);

  set_greater.Erase(it3);
  EXPECT_EQ(set_greater.Size(), initial_size_greater - 1);
  EXPECT_FALSE(set_greater.Contains(5));
}

TEST(SetTest, Swap) {
  s21::Set<int> set1{1};
  s21::Set<int> set2{2};
  set1.Swap(set2);
  EXPECT_EQ(set1.Size(), 1);
  EXPECT_EQ(set2.Size(), 1);
  EXPECT_TRUE(set1.Contains(2));
  EXPECT_TRUE(set2.Contains(1));

  s21::Set<int, std::greater<int>> set3{5};
  s21::Set<int, std::greater<int>> set4{3};
  set3.Swap(set4);
  EXPECT_EQ(set3.Size(), 1);
  EXPECT_EQ(set4.Size(), 1);
  EXPECT_TRUE(set3.Contains(3));
  EXPECT_TRUE(set4.Contains(5));
}

TEST(SetTest, Merge) {
  s21::Set<int> set1{1, 2};
  s21::Set<int> set2{2, 3};
  set1.Merge(set2);
  EXPECT_EQ(set1.Size(), 3);
  EXPECT_EQ(set2.Size(), 1);

  s21::Set<int, std::greater<int>> set3{5};
  s21::Set<int, std::greater<int>> set4{3};
  set3.Merge(set4);
  EXPECT_EQ(set3.Size(), 2);
  EXPECT_TRUE(set4.Empty());

  s21::Set<int, std::less<int>> set5{5};
  s21::Set<int, std::greater<int>> set6{3};
  set5.Merge(set6);
  EXPECT_EQ(set5.Size(), 2);
  EXPECT_TRUE(set6.Empty());
}

TEST(SetTest, Contains) {
  s21::Set<int> set{1, 2};
  EXPECT_TRUE(set.Contains(1));
  EXPECT_TRUE(set.Contains(2));
  EXPECT_FALSE(set.Contains(999));

  s21::Set<int, std::greater<int>> set_greater{5};
  EXPECT_TRUE(set_greater.Contains(5));
  EXPECT_FALSE(set_greater.Contains(1));
}

TEST(SetTest, Find) {
  s21::Set<int> set{1, 2, 3};

  auto it = set.Find(2);
  EXPECT_NE(it, set.End());
  EXPECT_EQ(*it, 2);

  it = set.Find(999);
  EXPECT_EQ(it, set.End());

  const auto& const_ref_set = set;
  auto const_it = const_ref_set.Find(2);
  EXPECT_NE(const_it, const_ref_set.End());
  EXPECT_EQ(*const_it, 2);

  const_it = const_ref_set.Find(999);
  EXPECT_EQ(const_it, const_ref_set.End());

  s21::Set<int, std::greater<int>> set_greater{5, 3};

  auto it2 = set_greater.Find(5);
  EXPECT_NE(it2, set_greater.End());
  EXPECT_EQ(*it2, 5);

  const auto& const_ref_set_greater = set_greater;
  auto const_it2 = const_ref_set_greater.Find(3);
  EXPECT_NE(const_it2, const_ref_set_greater.End());
  EXPECT_EQ(*const_it2, 3);

  const_it2 = const_ref_set_greater.Find(999);
  EXPECT_EQ(const_it2, const_ref_set_greater.End());

  EXPECT_EQ(*it2, *const_ref_set_greater.Find(5));
}

TEST(SetTest, IterationOrder) {
  s21::Set<int> set{3, 1, 2};
  auto it = set.Begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);

  s21::Set<int, std::greater<int>> set_greater{3, 1, 2};
  auto it2 = set_greater.Begin();
  EXPECT_EQ(*it2, 3);
  ++it2;
  EXPECT_EQ(*it2, 2);
  ++it2;
  EXPECT_EQ(*it2, 1);
}

TEST(SetTest, Clear) {
  s21::Set<int> set{1, 2};
  set.Clear();
  EXPECT_TRUE(set.Empty());

  s21::Set<int, std::greater<int>> set_greater{5};
  set_greater.Clear();
  EXPECT_TRUE(set_greater.Empty());
}

TEST(SetTest, SizeAndEmpty) {
  s21::Set<int> set;
  EXPECT_TRUE(set.Empty());
  EXPECT_EQ(set.Size(), 0);

  set.Insert(1);
  EXPECT_FALSE(set.Empty());
  EXPECT_EQ(set.Size(), 1);

  s21::Set<int, std::greater<int>> set_greater;
  EXPECT_TRUE(set_greater.Empty());
  set_greater.Insert(5);
  EXPECT_FALSE(set_greater.Empty());
  EXPECT_EQ(set_greater.Size(), 1);
}

TEST(SetTest, MaxSize) {
  s21::Set<int> set1;
  s21::Set<int, std::greater<int>> set2;

  EXPECT_GT(set1.MaxSize(), 0);
  EXPECT_GT(set2.MaxSize(), 0);
}

TEST(SetInsertTest, InsertUniqueKeys) {
  s21::Set<int> set;

  auto result1 = set.Insert(1);
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(*result1.first, 1);
  EXPECT_EQ(set.Size(), 1);

  auto result2 = set.Insert(2);
  EXPECT_TRUE(result2.second);
  EXPECT_EQ(*result2.first, 2);
  EXPECT_EQ(set.Size(), 2);

  auto result3 = set.Insert(3);
  EXPECT_TRUE(result3.second);
  EXPECT_EQ(*result3.first, 3);
  EXPECT_EQ(set.Size(), 3);
}

TEST(SetInsertTest, InsertDuplicateKey) {
  s21::Set<int> set;

  auto result1 = set.Insert(1);
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(set.Size(), 1);

  auto result2 = set.Insert(1);
  EXPECT_FALSE(result2.second);
  EXPECT_EQ(*result2.first, 1);
  EXPECT_EQ(set.Size(), 1);
}

TEST(SetInsertTest, InsertWithCustomComparator) {
  s21::Set<int, std::greater<int>> set;

  auto result1 = set.Insert(5);
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(*result1.first, 5);

  auto result2 = set.Insert(3);
  EXPECT_TRUE(result2.second);
  EXPECT_EQ(*result2.first, 3);

  auto it = set.Begin();
  EXPECT_EQ(*it, 5);
  ++it;
  EXPECT_EQ(*it, 3);
}

TEST(SetIteratorTest, BeginEndEmptySet) {
  s21::Set<int> set;

  EXPECT_EQ(set.Begin(), set.End());
  EXPECT_EQ(set.End(), set.Begin());

  const auto& const_set = set;
  EXPECT_EQ(const_set.Begin(), const_set.End());
}

TEST(SetIteratorTest, BeginNonEmptySet) {
  s21::Set<int> set;
  set.Insert(2);
  set.Insert(1);
  set.Insert(3);

  auto it = set.Begin();
  EXPECT_EQ(*it, 1);

  const auto& const_set = set;
  auto const_it = const_set.Begin();
  EXPECT_EQ(*const_it, 1);
}

TEST(SetIteratorTest, EndIterator) {
  s21::Set<int> set;
  set.Insert(1);

  auto begin = set.Begin();
  auto end = set.End();

  EXPECT_NE(begin, end);

  --end;
  EXPECT_EQ(*end, 1);
}

TEST(SetIteratorTest, ConstIterator) {
  const s21::Set<int> set = {1, 2};

  auto const_it = set.Begin();
  EXPECT_EQ(*const_it, 1);
  EXPECT_TRUE(set.Contains(1));

  ++const_it;
  EXPECT_EQ(*const_it, 2);
  EXPECT_TRUE(set.Contains(2));

  ++const_it;
  EXPECT_EQ(const_it, set.End());
}

TEST(SetIntegrationTest, LargeInsertion) {
  s21::Set<int> set;
  const int count = 1000;

  for (int i = count - 1; i >= 0; --i) {
    auto result = set.Insert(i);
    EXPECT_TRUE(result.second);
  }

  EXPECT_EQ(set.Size(), count);

  int expected = 0;
  for (auto it = set.Begin(); it != set.End(); ++it) {
    EXPECT_EQ(*it, expected);
    expected++;
  }
  EXPECT_EQ(expected, count);
}

TEST(SetTest, IteratorOperations) {
  s21::Set<int> set{1, 2, 3, 4, 5};

  auto it = set.Begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);

  auto it2 = it++;
  EXPECT_EQ(*it2, 2);
  EXPECT_EQ(*it, 3);

  --it;
  EXPECT_EQ(*it, 2);

  auto it3 = it--;
  EXPECT_EQ(*it3, 2);
  EXPECT_EQ(*it, 1);
}

TEST(SetTest, ReverseIteration) {
  s21::Set<int> set{1, 2, 3, 4, 5};

  auto it = set.End();
  --it;

  std::vector<int> reverse_values;
  for (; it != set.Begin(); --it) {
    reverse_values.push_back(*it);
  }
  reverse_values.push_back(*it);

  std::vector<int> expected{5, 4, 3, 2, 1};
  EXPECT_EQ(reverse_values, expected);
}

TEST(SetTest, FindNonExistent) {
  s21::Set<int> set{1, 2, 3};

  auto it = set.Find(999);
  EXPECT_EQ(it, set.End());

  it = set.Find(-1);
  EXPECT_EQ(it, set.End());
}

TEST(SetTest, MergeWithDifferentComparators) {
  s21::Set<int, std::less<int>> set1{1, 3, 5};
  s21::Set<int, std::greater<int>> set2{2, 4, 6};

  set1.Merge(set2);

  EXPECT_EQ(set1.Size(), 6);
  EXPECT_TRUE(set1.Contains(1));
  EXPECT_TRUE(set1.Contains(2));
  EXPECT_TRUE(set1.Contains(3));
  EXPECT_TRUE(set1.Contains(4));
  EXPECT_TRUE(set1.Contains(5));
  EXPECT_TRUE(set1.Contains(6));

  EXPECT_TRUE(set2.Empty());
}

TEST(SetTest, StringSetOperations) {
  s21::Set<std::string> set{"apple", "banana", "cherry"};

  EXPECT_EQ(set.Size(), 3);
  EXPECT_TRUE(set.Contains("apple"));
  EXPECT_TRUE(set.Contains("banana"));
  EXPECT_TRUE(set.Contains("cherry"));

  auto result = set.Insert("date");
  EXPECT_TRUE(result.second);
  EXPECT_EQ(set.Size(), 4);

  result = set.Insert("apple");
  EXPECT_FALSE(result.second);
  EXPECT_EQ(set.Size(), 4);
}

TEST(SetTest, DoubleSetOperations) {
  s21::Set<double> set{1.1, 2.2, 3.3};

  EXPECT_EQ(set.Size(), 3);
  EXPECT_TRUE(set.Contains(1.1));
  EXPECT_TRUE(set.Contains(2.2));
  EXPECT_TRUE(set.Contains(3.3));

  auto result = set.Insert(4.4);
  EXPECT_TRUE(result.second);

  result = set.Insert(1.1);
  EXPECT_FALSE(result.second);
}

TEST(SetTest, EraseThrowsWhenPositionIsEnd) {
  s21::Set<int> set = {1, 2, 3};
  auto end_it = set.End();

  EXPECT_THROW(set.Erase(end_it), std::out_of_range);
}
