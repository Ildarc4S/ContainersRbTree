#include <gtest/gtest.h>
#include <functional>
#include <memory>

#include "../../include/rb_tree/s21_rb_tree.h"
#include "../../include/s21_map.h"

namespace s21rb = s21::rb_tree;
using MapTree = s21::RbTree<int, std::pair<const int, std::string>, 
                          s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                          std::less<int>, std::allocator<std::pair<const int, std::string>>>;


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
    using MapTree = s21::RbTree<int, std::pair<const int, std::string>,
                              s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                              std::less<int>, std::allocator<std::pair<const int, std::string>>>;

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
    using MapTree = s21::RbTree<int, std::pair<const int, std::string>,
                              s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                              std::less<int>, std::allocator<std::pair<const int, std::string>>>;

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
  using MapTree = s21::RbTree<int, std::pair<const int, std::string>,
                            s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                            std::less<int>, std::allocator<std::pair<const int, std::string>>>;

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
  using MapTree = s21::RbTree<int, std::pair<const int, std::string>,
                            s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                            std::less<int>, std::allocator<std::pair<const int, std::string>>>;

  MapTree tree;
  tree.InsertUnique(std::make_pair(50, "50"));
  tree.InsertUnique(std::make_pair(30, "30"));
  tree.InsertUnique(std::make_pair(70, "30"));

  auto hint = tree.Find(70);
  auto result = tree.InsertHintUnique(hint, 80);

  EXPECT_TRUE(result != tree.end());
}

TEST(RbTreeTest, InsertHintCurrentNoRightChild) {
  using MapTree = s21::RbTree<int, std::pair<const int, std::string>,
                            s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                            std::less<int>, std::allocator<std::pair<const int, std::string>>>;

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
  using MapTree = s21::RbTree<int, std::pair<const int, std::string>,
                            s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                            std::less<int>, std::allocator<std::pair<const int, std::string>>>;

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
  using MapTree = s21::RbTree<int, std::pair<const int, std::string>,
                            s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                            std::less<int>, std::allocator<std::pair<const int, std::string>>>;

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
    using MapTree = s21::RbTree<int, std::pair<const int, std::string>,
                            s21::rb_tree::SelectFirst<std::pair<const int, std::string>>,
                            std::less<int>, std::allocator<std::pair<const int, std::string>>>;

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