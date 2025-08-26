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

TEST(RBTreeTest, Contains) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1;

  tree1.InsertUnique(std::pair<const int, int>(1, 11));

  auto key = 1;
  std::cout << (tree1.Find(key) != tree1.end()
                ? "Exists"
                : "Not Exists") << std::endl;
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


TEST(RBTreeTest, MergeTree) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1;
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree2;

  tree1.InsertUnique(std::pair<const int, int>(1, 11));
  tree1.InsertUnique(std::pair<const int, int>(2, 21));
  tree1.PrintTreeByLevelsSimple();

  tree2.InsertUnique(std::pair<const int, int>(1, 11));
  tree2.InsertUnique(std::pair<const int, int>(2, 21));
  tree2.InsertUnique(std::pair<const int, int>(3, 21));
  tree2.PrintTreeByLevelsSimple();

  tree1.MergeUnique(tree2);

  tree1.PrintTreeByLevelsSimple();
}

TEST(RBTreeTest, SwapTree) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1;
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree2;

  tree1.InsertUnique(std::pair<const int, int>(1, 11));
  tree1.InsertUnique(std::pair<const int, int>(2, 21));
  tree1.PrintTreeByLevelsSimple();

  tree2.InsertUnique(std::pair<const int, int>(1, 11));
  tree2.InsertUnique(std::pair<const int, int>(2, 21));
  tree2.InsertUnique(std::pair<const int, int>(3, 21));
  tree2.PrintTreeByLevelsSimple();

  tree1.Swap(tree2);

  tree1.PrintTreeByLevelsSimple();
  tree2.PrintTreeByLevelsSimple();
}

TEST(RBTreeTest, InitializerList) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1 = {
    {1, 11},
    {2, 12},
    {3, 13},
    {4, 14}
  };

  tree1.PrintTreeByLevelsSimple();
}

TEST(RBTreeTest, InsertOrAsign) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1 = {
    {1, 11},
    {2, 12},
    {3, 13},
    {4, 14}
  };

  auto key = 5;
  auto value = 15;
  auto it = tree1.LowerBound(key);

  if (it == tree1.end() || std::less<int>()(key, (*it).first)) {
    auto result = tree1.InsertUnique(std::make_pair(key, std::forward<decltype(value)>(value)));
  } else {
    (*it).second = std::forward<decltype(value)>(value);
  }

  tree1.PrintTreeByLevelsSimple();
}

TEST(RBTreeTest, OperatorEqual) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1 = {
    {1, 11},
    {2, 12},
    {3, 13},
    {4, 14}
  };

  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree2 = {
    {1, 21},
    {2, 22},
    {3, 23},
    {4, 24}
  };

  tree1.PrintTreeByLevelsSimple();
  tree2.PrintTreeByLevelsSimple();

  tree1 = tree2;

  tree1.PrintTreeByLevelsSimple();
  tree2.PrintTreeByLevelsSimple();
}

TEST(RBTreeTest, OperatorMoveEqual) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1 = {
    {1, 11},
    {2, 12},
    {3, 13},
    {4, 14}
  };

  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree2 = {
    {1, 21},
    {2, 22},
    {3, 23},
    {4, 24}
  };

  tree1.PrintTreeByLevelsSimple();
  tree2.PrintTreeByLevelsSimple();

  tree1 = std::move(tree2);

  tree1.PrintTreeByLevelsSimple();
  tree2.PrintTreeByLevelsSimple();
}

TEST(RBTreeTest, Iterator) {
  s21::RbTree<int, std::pair<const int, int>, SelectFirst, std::less<int>> tree1 = {
    {1, 11},
    {2, 12},
    {3, 13},
    {4, 14}
  };

  for (auto& a : tree1) {
    std::cout << "{" << a.first << " " << a.second << "}" << std::endl;
  }
}
