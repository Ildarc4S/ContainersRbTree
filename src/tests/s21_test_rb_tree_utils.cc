#include <gtest/gtest.h>

#include "s21_rb_tree_utils.h"

#define EXPECT_TYPE_SAME(T1, T2) \
  static_assert(std::is_same_v<T1, T2>, "Types should be the same"); \
  ASSERT_TRUE((std::is_same_v<T1, T2>))

TEST(RBTreeUtilsTest, BasePtrTypeDeduction) {
 using NodeBaseType = s21::rb_tree::NodeBase<int*>;
 EXPECT_TYPE_SAME(NodeBaseType::BasePtr_, s21::rb_tree::NodeBase<int*>*);
}
