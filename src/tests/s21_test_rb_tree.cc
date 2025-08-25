#include <gtest/gtest.h>
#include <functional>

#include "../include/s21_rb_tree.h"

struct SelectFirst {
  template<typename Pair>
  constexpr const auto& operator()(const Pair& pair) const noexcept {
    return pair.first;
  }
};

TEST(RBTreeTest, OperatorBracket) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1;

  tree1.InsertUnique(std::pair<const int, int>(1, 2));
  tree1.InsertUnique(std::pair<const int, int>(2, 2));
  tree1.PrintTreeByLevelsSimple();

  int key = 3;
  auto it = tree1.LowerBound(key);
  if (it == tree1.end() || std::less<int>()(key, (*it).first)) {
    it = tree1.InsertHintUnique(it, key);
  }
  (*it).second = 3;

  tree1.PrintTreeByLevelsSimple();
}


TEST(RBTreeTest, MethodAt) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1;

  tree1.InsertUnique(std::pair<const int, int>(1, 11));
  tree1.InsertUnique(std::pair<const int, int>(2, 21));
  tree1.PrintTreeByLevelsSimple();

  auto key = 1;
  auto it = tree1.LowerBound(key);
  if (it == tree1.end() || std::less<int>()(key, it->first)) {
      throw std::out_of_range("map::at: key not found");
  }
  std::cout << "at(): " << it->second << std::endl;
}

TEST(RBTreeTest, Erase) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1;

  tree1.InsertUnique(std::pair<const int, int>(1, 11));
  tree1.InsertUnique(std::pair<const int, int>(2, 21));
  tree1.PrintTreeByLevelsSimple();

  tree1.Erase(tree1.begin());
  tree1.Erase(tree1.begin());

  tree1.PrintTreeByLevelsSimple();
}
