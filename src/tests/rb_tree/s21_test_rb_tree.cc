#include <gtest/gtest.h>

#include <functional>
#include <memory>

#include "../../include/rb_tree/s21_rb_tree.h"

namespace s21rb = s21::rb_tree;
using MapTree =
    s21::RbTree<int, std::pair<const int, std::string>,
                s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                std::less<int>,
                std::allocator<std::pair<const int, std::string>>>;

using Tree = s21::RbTree<int, int, s21rb::Identity<int>, std::less<int>,
                         std::allocator<int>>;

TEST(RbTreeTest, InsertHintEnd) {
  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "fifty"));
  tree.InsertUnique(std::make_pair(70, "seventy"));

  auto hint = tree.end();
  auto result = tree.InsertHintUnique(hint, 90);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 90);
}

TEST(RbTreeTest, InsertHintBegin) {
  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "fifty"));
  tree.InsertUnique(std::make_pair(30, "thirty"));

  auto hint = tree.end();
  auto result = tree.InsertHintUnique(hint, 10);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 10);
}

TEST(RbTreeTest, InsertHintMiddle) {
  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "fifty"));
  tree.InsertUnique(std::make_pair(30, "thirty"));
  tree.InsertUnique(std::make_pair(40, "forty"));

  auto hint = tree.Find(40);
  auto result = tree.InsertHintUnique(hint, 35);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 35);
}

TEST(RbTreeTest, InsertHintDuplicate) {
  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "fifty"));

  auto hint = tree.Find(50);
  auto result = tree.InsertHintUnique(hint, 50);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 50);
}

TEST(RbTreeTest, InsertHintNearMax) {
  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "fifty"));
  tree.InsertUnique(std::make_pair(80, "eighty"));

  auto hint = tree.Find(80);
  auto result = tree.InsertHintUnique(hint, 85);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 85);
}

TEST(RbTreeTest, InsertHintNearMin) {
  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "fifty"));
  tree.InsertUnique(std::make_pair(20, "twenty"));

  auto hint = tree.Find(20);
  auto result = tree.InsertHintUnique(hint, 15);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 15);
}

TEST(RbTreeTest, InsertHintRightSideNeedUniquePos) {
  using MapTree =
      s21::RbTree<int, std::pair<const int, std::string>,
                  s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                  std::less<int>,
                  std::allocator<std::pair<const int, std::string>>>;

  MapTree tree;
  /* Дерево:
        50
       /  \
      30   70
           / \
          65  80
  */
  tree.InsertUnique(std::make_pair(50, "50"));
  tree.InsertUnique(std::make_pair(30, "30"));
  tree.InsertUnique(std::make_pair(70, "70"));
  tree.InsertUnique(std::make_pair(65, "65"));
  tree.InsertUnique(std::make_pair(80, "80"));

  // 1. key_compare_(GetKey(70), 75) = 70 < 75 → true
  // 2. x != impl_.header_.right_ (70 ≠ 80)
  // 3. after = ++iterator(70) = 80
  // 4. !key_compare_(75, GetKey(80)) = !(75 < 80) = !true = false
  // 5. Попадаем в else: result = GetInsertUniquePos(k)
  auto hint = tree.Find(70);
  auto result = tree.InsertHintUnique(hint, 75);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 75);
}

TEST(RbTreeTest, InsertHintLeftmost) {
  using MapTree =
      s21::RbTree<int, std::pair<const int, std::string>,
                  s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                  std::less<int>,
                  std::allocator<std::pair<const int, std::string>>>;

  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "50"));
  tree.InsertUnique(std::make_pair(30, "30"));
  tree.InsertUnique(std::make_pair(70, "70"));

  auto hint = tree.begin();
  auto result = tree.InsertHintUnique(hint, 20);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 20);
}

TEST(RbTreeTest, InsertHintRightChild) {
  using MapTree =
      s21::RbTree<int, std::pair<const int, std::string>,
                  s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                  std::less<int>,
                  std::allocator<std::pair<const int, std::string>>>;

  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "50"));
  tree.InsertUnique(std::make_pair(30, "30"));
  tree.InsertUnique(std::make_pair(70, "30"));
  tree.InsertUnique(std::make_pair(20, "30"));
  tree.InsertUnique(std::make_pair(40, "30"));

  auto hint = tree.Find(40);
  auto result = tree.InsertHintUnique(hint, 25);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 25);
}

TEST(RbTreeTest, InsertHintAtRightmost) {
  using MapTree =
      s21::RbTree<int, std::pair<const int, std::string>,
                  s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                  std::less<int>,
                  std::allocator<std::pair<const int, std::string>>>;

  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "50"));
  tree.InsertUnique(std::make_pair(30, "30"));
  tree.InsertUnique(std::make_pair(70, "30"));

  auto hint = tree.Find(70);
  auto result = tree.InsertHintUnique(hint, 80);

  EXPECT_TRUE(result != tree.end());
}

TEST(RbTreeTest, InsertHintCurrentNoRightChild) {
  using MapTree =
      s21::RbTree<int, std::pair<const int, std::string>,
                  s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                  std::less<int>,
                  std::allocator<std::pair<const int, std::string>>>;

  MapTree tree;
  /* Tree:
      50
    /  \
   30   70
  */
  tree.InsertUnique(std::make_pair(50, "50"));
  tree.InsertUnique(std::make_pair(30, "30"));
  tree.InsertUnique(std::make_pair(70, "30"));

  auto hint = tree.Find(70);
  auto result = tree.InsertHintUnique(hint, 65);

  EXPECT_TRUE(result != tree.end());
}

TEST(RbTreeTest, InsertHintNoRightChildAndKeyGreaterEqualNext) {
  using MapTree =
      s21::RbTree<int, std::pair<const int, std::string>,
                  s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                  std::less<int>,
                  std::allocator<std::pair<const int, std::string>>>;

  MapTree tree;
  /* Tree:
       50
      /  \
     30   70
          /
        65
  */
  tree.InsertUnique(std::make_pair(50, "50"));
  tree.InsertUnique(std::make_pair(30, "30"));
  tree.InsertUnique(std::make_pair(70, "70"));
  tree.InsertUnique(std::make_pair(65, "65"));

  // 1. key_compare_(GetKey(65), 71) = 65 < 71 → true
  // 2. after = ++iterator(65) = 70
  // 3. !key_compare_(71, GetKey(70)) = !(71 < 70) = !false = true
  // 4. !GetRight(65) = true
  auto hint = tree.Find(65);
  auto result = tree.InsertHintUnique(hint, 71);

  EXPECT_TRUE(result != tree.end());
}

TEST(RbTreeTest, InsertHintRightChildAndKeyGreaterEqualNext) {
  using MapTree =
      s21::RbTree<int, std::pair<const int, std::string>,
                  s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                  std::less<int>,
                  std::allocator<std::pair<const int, std::string>>>;

  MapTree tree;
  /* Tree:
        50
      /  \
      30   70
          /  \
        65   75
          \
          68
  */
  tree.InsertUnique(std::make_pair(50, "50"));
  tree.InsertUnique(std::make_pair(30, "30"));
  tree.InsertUnique(std::make_pair(70, "70"));
  tree.InsertUnique(std::make_pair(65, "65"));
  tree.InsertUnique(std::make_pair(75, "75"));
  tree.InsertUnique(std::make_pair(68, "68"));

  // 1. key_compare_(GetKey(65), 72) = 65 < 72 → true
  // 2. after = ++iterator(65) = 68
  // 3. !key_compare_(72, GetKey(68)) = !(72 < 68) = !false = true
  // 4. GetRight(65) = true
  // 5. else: result.first = after.node_; result.second = after.node_;
  auto hint = tree.Find(65);
  auto result = tree.InsertHintUnique(hint, 72);

  EXPECT_TRUE(result != tree.end());
  EXPECT_EQ(result->first, 72);
}

TEST(RbTreeTest, InsertHintBeforeNodeNoRightChild) {
  using MapTree =
      s21::RbTree<int, std::pair<const int, std::string>,
                  s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                  std::less<int>,
                  std::allocator<std::pair<const int, std::string>>>;

  MapTree tree;
  /* Дерево:
       50
      /  \
     30   70
     /
    20
  */
  tree.InsertUnique(std::make_pair(50, "50"));
  tree.InsertUnique(std::make_pair(30, "30"));
  tree.InsertUnique(std::make_pair(70, "70"));
  tree.InsertUnique(std::make_pair(20, "20"));

  // 1. key_compare_(15, GetKey(30)) = 15 < 30 → true
  // 2. before = --iterator(30) = 20
  // 3. !key_compare_(GetKey(20), 15) = !(20 < 15) = !false = true
  // 4. !GetRight(before.node_) = !GetRight(20) = true
  // 5. result.second = before.node_ = 20

  auto hint = tree.Find(30);
  auto result = tree.InsertHintUnique(hint, 15);

  EXPECT_TRUE(result != tree.end());
}

TEST(RbTreeTest, EraseBlackWWithOneBlackChildren) {
  Tree tree1;
  Tree tree2;

  tree1.InsertUnique(20);
  tree1.InsertUnique(15);
  tree1.InsertUnique(22);
  tree1.InsertUnique(21);

  tree2.InsertUnique(20);
  tree2.InsertUnique(15);
  tree2.InsertUnique(22);
  tree2.InsertUnique(16);

  auto it1 = tree1.Find(15);
  auto it2 = tree2.Find(22);
  ASSERT_NE(it1, tree1.end());
  ASSERT_NE(it2, tree2.end());

  tree1.Erase(it1);
  tree2.Erase(it2);
  EXPECT_EQ(tree1.Size(), 3);
  EXPECT_EQ(tree2.Size(), 3);
}

TEST(RbTreeTest, EraseBlackWWithTwoBlackChildren) {
  Tree tree1;
  Tree tree2;

  tree1.InsertUnique(20);
  tree1.InsertUnique(15);
  tree1.InsertUnique(22);
  tree1.InsertUnique(16);

  tree2.InsertUnique(20);
  tree2.InsertUnique(15);
  tree2.InsertUnique(22);
  tree2.InsertUnique(21);

  auto it1 = tree1.Find(15);
  tree1.Erase(it1);
  ASSERT_NE(it1, tree1.end());
  it1 = tree1.Find(22);
  tree1.Erase(it1);
  ASSERT_NE(it1, tree1.end());
  EXPECT_EQ(tree1.Size(), 2);

  auto it2 = tree2.Find(15);
  tree2.Erase(it2);
  ASSERT_NE(it2, tree2.end());
  it2 = tree2.Find(20);
  tree2.Erase(it2);
  ASSERT_NE(it2, tree2.end());
  EXPECT_EQ(tree2.Size(), 2);
}

TEST(RbTreeTest, EraseRedW) {
  Tree tree1;
  Tree tree2;

  tree1.InsertUnique(57);
  tree1.InsertUnique(44);
  tree1.InsertUnique(66);
  tree1.InsertUnique(62);
  tree1.InsertUnique(75);
  tree1.InsertUnique(70);
  tree1.InsertUnique(87);
  tree1.InsertUnique(81);
  tree1.InsertUnique(89);
  tree1.InsertUnique(90);

  tree2.InsertUnique(30);
  tree2.InsertUnique(6);
  tree2.InsertUnique(50);
  tree2.InsertUnique(40);
  tree2.InsertUnique(3);
  tree2.InsertUnique(20);
  tree2.InsertUnique(4);

  auto it1 = tree1.Find(70);
  tree1.Erase(it1);
  ASSERT_NE(it1, tree1.end());
  EXPECT_EQ(tree1.Size(), 9);

  auto it2 = tree2.Find(50);
  tree2.Erase(it2);
  it2 = tree2.Find(40);
  tree2.Erase(it2);

  ASSERT_NE(it2, tree2.end());
  EXPECT_EQ(tree2.Size(), 5);
}
