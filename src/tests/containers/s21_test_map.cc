#include <gtest/gtest.h>
#include "../../include/s21_map.h"

TEST(MapTest, DefaultConstructor) {
  s21::Map<int, std::string> map;
  EXPECT_TRUE(map.Empty());
  EXPECT_EQ(map.Size(), 0);

  s21::Map<int, std::string, std::greater<int>> map_greater;
  EXPECT_TRUE(map_greater.Empty());

  s21::Map<std::string, int, std::greater<std::string>> map_string_greater;
  EXPECT_TRUE(map_string_greater.Empty());
}

TEST(MapTest, InitializerListConstructor) {
  s21::Map<int, std::string> map{
    {1, "one"},
    {2, "two"},
    {3, "three"}
  };
  EXPECT_EQ(map.Size(), 3);
  EXPECT_EQ(map.At(1), "one");
  EXPECT_EQ(map[2], "two");

  s21::Map<int, std::string, std::greater<int>> map_greater{
    {1, "one"},
    {2, "two"},
    {3, "three"}
  };
  EXPECT_EQ(map_greater.Size(), 3);
  EXPECT_EQ(map_greater.At(3), "three");
  EXPECT_EQ(map_greater[1], "one");

  auto it = map_greater.Begin();
  EXPECT_EQ(it->first, 3);
  EXPECT_EQ(it->second, "three");
}

TEST(MapTest, CopyConstructor) {
  s21::Map<int, std::string> map1{{1, "one"}, {2, "two"}};
  s21::Map<int, std::string> map2(map1);
  EXPECT_EQ(map2.Size(), 2);
  EXPECT_EQ(map2.At(1), "one");
  EXPECT_EQ(map2[2], "two");

  s21::Map<int, std::string, std::greater<int>> map3{{3, "three"}, {1, "one"}};
  s21::Map<int, std::string, std::greater<int>> map4(map3);
  EXPECT_EQ(map4.Size(), 2);
  EXPECT_EQ(map4.At(3), "three");
}

TEST(MapTest, MoveConstructor) {
  s21::Map<int, std::string> map1{{1, "one"}, {2, "two"}};
  s21::Map<int, std::string> map2(std::move(map1));
  EXPECT_EQ(map2.Size(), 2);
  EXPECT_TRUE(map1.Empty());

  s21::Map<int, std::string, std::greater<int>> map3{{5, "five"}, {2, "two"}};
  s21::Map<int, std::string, std::greater<int>> map4(std::move(map3));
  EXPECT_EQ(map4.Size(), 2);
  EXPECT_TRUE(map3.Empty());
  EXPECT_EQ(map4.Begin()->first, 5);
}

TEST(MapTest, CopyAssignment) {
  s21::Map<int, std::string> map1{{1, "one"}, {2, "two"}};
  s21::Map<int, std::string> map2;
  map2 = map1;
  EXPECT_EQ(map2.Size(), 2);
  EXPECT_EQ(map2.At(1), "one");

  s21::Map<int, std::string, std::greater<int>> map3{{10, "ten"}};
  s21::Map<int, std::string, std::greater<int>> map4;
  map4 = map3;
  EXPECT_EQ(map4.Size(), 1);
  EXPECT_EQ(map4.At(10), "ten");
}

TEST(MapTest, MoveAssignment) {
  s21::Map<int, std::string> map1{{1, "one"}, {2, "two"}};
  s21::Map<int, std::string> map2;
  map2 = std::move(map1);
  EXPECT_EQ(map2.Size(), 2);
  EXPECT_TRUE(map1.Empty());

  s21::Map<int, std::string, std::greater<int>> map3{{7, "seven"}, {3, "three"}};
  s21::Map<int, std::string, std::greater<int>> map4;
  map4 = std::move(map3);
  EXPECT_EQ(map4.Size(), 2);
  EXPECT_TRUE(map3.Empty());

  auto it = map4.Begin();
  EXPECT_EQ(it->first, 7);
  EXPECT_EQ(it->second, "seven");
}

TEST(MapTest, CustomComparatorConstructor) {
  struct AbsCompare {
    bool operator()(int a, int b) const {
      return std::abs(a) < std::abs(b);
    }
  };

  s21::Map<int, std::string, AbsCompare> map{
    {-5, "minus five"},
    {3, "three"},
    {5, "five"}
  };

  EXPECT_EQ(map.Size(), 2);
  EXPECT_TRUE(map.Contains(5));
  EXPECT_TRUE(map.Contains(3));
}

TEST(MapTest, StringCustomComparator) {
  struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const {
      return strcasecmp(a.c_str(), b.c_str()) < 0;
    }
  };

  s21::Map<std::string, int, CaseInsensitiveCompare> map{
    {"Apple", 1},
    {"banana", 2},
    {"CHERRY", 3}
  };

  EXPECT_EQ(map.Size(), 3);
  EXPECT_EQ(map["Apple"], 1);
  EXPECT_EQ(map["banana"], 2);
  EXPECT_EQ(map["CHERRY"], 3);
}

TEST(MapTest, AtMethods) {
  s21::Map<int, std::string> map{{1, "one"}, {2, "two"}};
  EXPECT_EQ(map.At(1), "one");
  EXPECT_EQ(map.At(2), "two");
  EXPECT_THROW(map.At(999), std::out_of_range);

  s21::Map<int, std::string, std::greater<int>> map_greater{{3, "three"}, {1, "one"}};
  EXPECT_EQ(map_greater.At(3), "three");
  EXPECT_EQ(map_greater.At(1), "one");
  EXPECT_THROW(map_greater.At(999), std::out_of_range);
}

TEST(MapTest, SubscriptOperator) {
  s21::Map<int, std::string> map;
  map[1] = "one";
  map[2] = "two";
  EXPECT_EQ(map[1], "one");
  EXPECT_EQ(map[2], "two");
  EXPECT_EQ(map[3], "");

  s21::Map<int, std::string, std::greater<int>> map_greater;
  map_greater[5] = "five";
  map_greater[2] = "two";
  EXPECT_EQ(map_greater[5], "five");
  EXPECT_EQ(map_greater[2], "two");
}

TEST(MapTest, InsertMethods) {
  s21::Map<int, std::string> map;
  auto result1 = map.Insert({1, "one"});
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(result1.first->first, 1);

  auto result2 = map.Insert({1, "duplicate"});
  EXPECT_FALSE(result2.second);
  EXPECT_EQ(result2.first->second, "one");

  s21::Map<int, std::string, std::greater<int>> map_greater;
  auto result3 = map_greater.Insert({10, "ten"});
  EXPECT_TRUE(result3.second);
  EXPECT_EQ(result3.first->first, 10);
}

TEST(MapTest, InsertOrAssign) {
  s21::Map<int, std::string> map;
  auto result1 = map.InsertOrAssign(1, "one");
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(result1.first->second, "one");

  auto result2 = map.InsertOrAssign(1, "new_one");
  EXPECT_FALSE(result2.second);
  EXPECT_EQ(result2.first->second, "new_one");

  s21::Map<int, std::string, std::greater<int>> map_greater;
  auto result3 = map_greater.InsertOrAssign(5, "five");
  EXPECT_TRUE(result3.second);
  EXPECT_EQ(result3.first->second, "five");
}

TEST(MapTest, Erase) {
  s21::Map<int, std::string> map{{1, "one"}, {2, "two"}, {3, "three"}};

  size_t initial_size = map.Size();

  auto it = map.Begin();
  while (it != map.End() && it->first != 2) {
    ++it;
  }

  ASSERT_NE(it, map.End());
  ASSERT_EQ(it->first, 2);
  ASSERT_EQ(it->second, "two");

  map.Erase(it);

  EXPECT_EQ(map.Size(), initial_size - 1);

  bool found = false;
  for (auto it2 = map.Begin(); it2 != map.End() && !found; ++it2) {
    if (it2->first == 2) {
      found = true;
    }
  }
  EXPECT_FALSE(found);

  s21::Map<int, std::string, std::greater<int>> map_greater{{5, "five"}, {3, "three"}};
  size_t initial_size_greater = map_greater.Size();

  auto it3 = map_greater.Begin();
  ASSERT_NE(it3, map_greater.End());
  ASSERT_EQ(it3->first, 5);
  ASSERT_EQ(it3->second, "five");

  map_greater.Erase(it3);
  EXPECT_EQ(map_greater.Size(), initial_size_greater - 1);

  bool found_greater = false;
  for (auto it4 = map_greater.Begin(); it4 != map_greater.End() && !found_greater; ++it4) {
    if (it4->first == 5) {
      found_greater = true;
    }
  }
  EXPECT_FALSE(found_greater);
}

TEST(MapTest, Swap) {
  s21::Map<int, std::string> map1{{1, "one"}};
  s21::Map<int, std::string> map2{{2, "two"}};
  map1.Swap(map2);
  EXPECT_EQ(map1.Size(), 1);
  EXPECT_EQ(map2.Size(), 1);
  EXPECT_EQ(map1.Begin()->first, 2);
  EXPECT_EQ(map2.Begin()->first, 1);

  s21::Map<int, std::string, std::greater<int>> map3{{5, "five"}};
  s21::Map<int, std::string, std::greater<int>> map4{{3, "three"}};
  map3.Swap(map4);
  EXPECT_EQ(map3.Size(), 1);
  EXPECT_EQ(map4.Size(), 1);
  EXPECT_EQ(map3.Begin()->first, 3);
  EXPECT_EQ(map4.Begin()->first, 5);
}

TEST(MapTest, Merge) {
  s21::Map<int, std::string> map1{{1, "one"}, {2, "two"}};
  s21::Map<int, std::string> map2{{2, "duplicate"}, {3, "three"}};
  map1.Merge(map2);
  EXPECT_EQ(map1.Size(), 3);
  EXPECT_EQ(map2.Size(), 1);

  s21::Map<int, std::string, std::greater<int>> map3{{5, "five"}};
  s21::Map<int, std::string, std::greater<int>> map4{{3, "three"}};
  map3.Merge(map4);
  EXPECT_EQ(map3.Size(), 2);
  EXPECT_TRUE(map4.Empty());

  s21::Map<int, std::string, std::less<int>> map5{{5, "five"}};
  s21::Map<int, std::string, std::greater<int>> map6{{3, "three"}};
  map5.Merge(map6);
  EXPECT_EQ(map3.Size(), 2);
  EXPECT_TRUE(map4.Empty());
}

TEST(MapTest, Contains) {
  s21::Map<int, std::string> map{{1, "one"}, {2, "two"}};
  EXPECT_TRUE(map.Contains(1));
  EXPECT_TRUE(map.Contains(2));
  EXPECT_FALSE(map.Contains(999));

  s21::Map<int, std::string, std::greater<int>> map_greater{{5, "five"}};
  EXPECT_TRUE(map_greater.Contains(5));
  EXPECT_FALSE(map_greater.Contains(1));
}

TEST(MapTest, IterationOrder) {
  s21::Map<int, std::string> map{{3, "three"}, {1, "one"}, {2, "two"}};
  auto it = map.Begin();
  EXPECT_EQ(it->first, 1);
  ++it; EXPECT_EQ(it->first, 2);
  ++it; EXPECT_EQ(it->first, 3);

  s21::Map<int, std::string, std::greater<int>> map_greater{{3, "three"}, {1, "one"}, {2, "two"}};
  auto it2 = map_greater.Begin();
  EXPECT_EQ(it2->first, 3);
  ++it2; EXPECT_EQ(it2->first, 2);
  ++it2; EXPECT_EQ(it2->first, 1);
}

TEST(MapTest, Clear) {
  s21::Map<int, std::string> map{{1, "one"}, {2, "two"}};
  map.Clear();
  EXPECT_TRUE(map.Empty());

  s21::Map<int, std::string, std::greater<int>> map_greater{{5, "five"}};
  map_greater.Clear();
  EXPECT_TRUE(map_greater.Empty());
}

TEST(MapTest, SizeAndEmpty) {
  s21::Map<int, std::string> map;
  EXPECT_TRUE(map.Empty());
  EXPECT_EQ(map.Size(), 0);

  map.Insert({1, "one"});
  EXPECT_FALSE(map.Empty());
  EXPECT_EQ(map.Size(), 1);

  s21::Map<int, std::string, std::greater<int>> map_greater;
  EXPECT_TRUE(map_greater.Empty());
  map_greater.Insert({5, "five"});
  EXPECT_FALSE(map_greater.Empty());
  EXPECT_EQ(map_greater.Size(), 1);
}

TEST(MapTest, MaxSize) {
  s21::Map<int, std::string> map1;
  s21::Map<int, std::string, std::greater<int>> map2;

  EXPECT_GT(map1.MaxSize(), 0);
  EXPECT_GT(map2.MaxSize(), 0);
}

TEST(MapInsertTest, InsertUniqueKeys) {
  s21::Map<int, std::string> map;

  auto result1 = map.Insert(1, "one");
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(result1.first->first, 1);
  EXPECT_EQ(result1.first->second, "one");
  EXPECT_EQ(map.Size(), 1);

  auto result2 = map.Insert(2, "two");
  EXPECT_TRUE(result2.second);
  EXPECT_EQ(result2.first->first, 2);
  EXPECT_EQ(result2.first->second, "two");
  EXPECT_EQ(map.Size(), 2);

  auto result3 = map.Insert(3, "three");
  EXPECT_TRUE(result3.second);
  EXPECT_EQ(result3.first->first, 3);
  EXPECT_EQ(result3.first->second, "three");
  EXPECT_EQ(map.Size(), 3);
}

TEST(MapInsertTest, InsertDuplicateKey) {
  s21::Map<int, std::string> map;

  auto result1 = map.Insert(1, "one");
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(map.Size(), 1);

  auto result2 = map.Insert(1, "duplicate");
  EXPECT_FALSE(result2.second);
  EXPECT_EQ(result2.first->first, 1);
  EXPECT_EQ(result2.first->second, "one");
  EXPECT_EQ(map.Size(), 1);
}

TEST(MapInsertTest, InsertWithCustomComparator) {
  s21::Map<int, std::string, std::greater<int>> map;

  auto result1 = map.Insert(5, "five");
  EXPECT_TRUE(result1.second);
  EXPECT_EQ(result1.first->first, 5);
  EXPECT_EQ(result1.first->second, "five");

  auto result2 = map.Insert(3, "three");
  EXPECT_TRUE(result2.second);
  EXPECT_EQ(result2.first->first, 3);
  EXPECT_EQ(result2.first->second, "three");

  auto it = map.Begin();
  EXPECT_EQ(it->first, 5);
  EXPECT_EQ(it->second, "five");
  ++it;
  EXPECT_EQ(it->first, 3);
  EXPECT_EQ(it->second, "three");
}

TEST(MapIteratorTest, BeginEndEmptyMap) {
  s21::Map<int, std::string> map;

  EXPECT_EQ(map.Begin(), map.End());
  EXPECT_EQ(map.End(), map.Begin());

  const auto& const_map = map;
  EXPECT_EQ(const_map.Begin(), const_map.End());
}

TEST(MapIteratorTest, BeginNonEmptyMap) {
  s21::Map<int, std::string> map;
  map.Insert(2, "two");
  map.Insert(1, "one");
  map.Insert(3, "three");

  auto it = map.Begin();
  EXPECT_EQ(it->first, 1);
  EXPECT_EQ(it->second, "one");

  const auto& const_map = map;
  auto const_it = const_map.Begin();
  EXPECT_EQ(const_it->first, 1);
  EXPECT_EQ(const_it->second, "one");
}

TEST(MapIteratorTest, EndIterator) {
  s21::Map<int, std::string> map;
  map.Insert(1, "one");

  auto begin = map.Begin();
  auto end = map.End();

  EXPECT_NE(begin, end);

  --end;
  EXPECT_EQ(end->first, 1);
  EXPECT_EQ(end->second, "one");
}

TEST(MapIteratorTest, ConstIterator) {
  const s21::Map<int, std::string> map = {
    {1, "one"},
    {2, "two"}
  };

  auto const_it = map.Begin();
  EXPECT_EQ(const_it->first, 1);
  EXPECT_EQ(const_it->second, "one");

  ++const_it;
  EXPECT_EQ(const_it->first, 2);
  EXPECT_EQ(const_it->second, "two");

  ++const_it;
  EXPECT_EQ(const_it, map.End());
}

TEST(MapIntegrationTest, LargeInsertion) {
  s21::Map<int, int> map;
  const int count = 1000;

  for (int i = count - 1; i >= 0; --i) {
    auto result = map.Insert(i, i * 10);
    EXPECT_TRUE(result.second);
  }

  EXPECT_EQ(map.Size(), count);

  int expected = 0;
  for (auto it = map.Begin(); it != map.End(); ++it) {
    EXPECT_EQ(it->first, expected);
    EXPECT_EQ(it->second, expected * 10);
    expected++;
  }
  EXPECT_EQ(expected, count);
}
