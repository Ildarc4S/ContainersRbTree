#include <gtest/gtest.h>
#include <functional>

#include "../include/s21_rb_tree.h"

struct SelectFirst {
  template<typename Pair>
  constexpr const auto& operator()(const Pair& pair) const noexcept {
    return pair.first;
  }
};

TEST(RBTreeTest, ExampleTest) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1;

  tree1.InsertUnique(std::pair<const int, int>(2, 2));
  tree1.InsertUnique(std::pair<const int, int>(1, 2));
  tree1.InsertUnique(std::pair<const int, int>(3, 2));
}
