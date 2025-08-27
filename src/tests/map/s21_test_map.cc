#include <gtest/gtest.h>
#include "../../include/s21_map.h"

TEST(MapTests, ConstructMap) {
  s21::Map<int, int> a = {
    {1, 11},
    {2, 12},
    {3, 13},
    {4, 14}
  };
  std::cout << a.At(2);
}

TEST(MapTests, Insert) {
  s21::Map<int, int> a;
  a.Insert(1, 2);
}

TEST(MapTests, InsertOrAssign) {
  s21::Map<int, int> a;
  a.Insert(1, 2);
  std::cout << a.At(1) << std::endl;

  a.InsertOrAssign(1, 3);
  std::cout << a.At(1) << std::endl;
}


TEST(MapTests, Merge) {
  s21::Map<int, int> a = {
    {1, 11},
    {2, 12},
    {3, 13},
    {4, 14}
  };

  s21::Map<int, int> b = {
    {1, 11},
    {2, 12},
    {3, 13},
    {4, 14},
    {5, 15}
  };
  a.Merge(b);

  for (auto it = a.Begin(); it != a.End(); ++it) {
    std::cout << "{" << it->first << "," << it->second << "} ";
  }
}