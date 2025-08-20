#include <gtest/gtest.h>
#include <memory>

#include "s21_rb_tree_utils.h"
#include "s21_test_utils.h"

#define EXPECT_TYPE_SAME(T1, T2) \
  static_assert(std::is_same_v<T1, T2>, "Types should be the same"); \
  ASSERT_TRUE((std::is_same_v<T1, T2>))

template <typename Ptr>
class UniversalPtrTest : public ::testing::Test {};

using AllPtrTypes = ::testing::Types<
    int*, const int*, double*,
    std::unique_ptr<int>, std::shared_ptr<int>,
    std::unique_ptr<double>, std::shared_ptr<double>
>;

TYPED_TEST_SUITE(UniversalPtrTest, AllPtrTypes);

// -----------------------------
// NodeBaseTypes tests
// -----------------------------
TYPED_TEST(UniversalPtrTest, NodeBaseBasePtrRebindConsistency) {
    using NodeBaseType = s21::rb_tree::NodeBase<TypeParam>;
    using ExpectedRebind = typename std::pointer_traits<TypeParam>::template rebind<NodeBaseType>;
    EXPECT_TYPE_SAME(typename NodeBaseType::BasePtr_, ExpectedRebind);
}

// -----------------------------
// NodeBaseTypes tests
// -----------------------------
TYPED_TEST(UniversalPtrTest, NodeValueTypeAndNodePtrConsistency) {
    using NodeT = s21::rb_tree::Node<TypeParam>;
    using ExpectedValueType = typename std::pointer_traits<TypeParam>::element_type;
    using ExpectedNodePtr = typename std::pointer_traits<TypeParam>::template rebind<NodeT>;
    EXPECT_TYPE_SAME(typename NodeT::ValueType_, ExpectedValueType);
    EXPECT_TYPE_SAME(typename NodeT::NodePtr_, ExpectedNodePtr);
}

