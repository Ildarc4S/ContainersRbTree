#include <gtest/gtest.h>
#include <memory>

#include "s21_rb_tree_utils.h"
#include "s21_test_utils.h"

namespace s21rb = s21::rb_tree;;

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
    using NodeBaseType = s21rb::NodeBase<TypeParam>;
    using ExpectedRebind = typename std::pointer_traits<TypeParam>::template rebind<NodeBaseType>;
    EXPECT_TYPE_SAME(typename NodeBaseType::BasePtr_, ExpectedRebind);
}

// -----------------------------
// NodeBaseTypes tests
// -----------------------------
TYPED_TEST(UniversalPtrTest, NodeValueTypeAndNodePtrConsistency) {
    using NodeT = s21rb::Node<TypeParam>;
    using ExpectedValueType = typename std::pointer_traits<TypeParam>::element_type;
    using ExpectedNodePtr = typename std::pointer_traits<TypeParam>::template rebind<NodeT>;
    EXPECT_TYPE_SAME(typename NodeT::ValueType_, ExpectedValueType);
    EXPECT_TYPE_SAME(typename NodeT::NodePtr_, ExpectedNodePtr);
}

// -----------------------------
// IteratorTypes tests
// -----------------------------
TYPED_TEST(UniversalPtrTest, IteratorInternalTypesNonConst) {
  using ValPtr = TypeParam;
  using Iter = s21rb::Iterator<false, ValPtr>;

  using ExpectedNode = s21rb::Node<ValPtr>;
  using ExpectedNodeBase = s21rb::NodeBase<typename std::pointer_traits<ValPtr>::template rebind<void>>;
  using ExpectedBasePtr = typename std::pointer_traits<ValPtr>::template rebind<ExpectedNodeBase>;
  using ExpectedValueType = typename std::pointer_traits<ValPtr>::element_type;
  using ExpectedReference = ExpectedValueType&;
  using ExpectedPointer = ExpectedValueType*;

  EXPECT_TYPE_SAME(typename Iter::Node_, ExpectedNode);
  EXPECT_TYPE_SAME(typename Iter::NodeBase_, ExpectedNodeBase);
  EXPECT_TYPE_SAME(typename Iter::BasePtr_, ExpectedBasePtr);
  EXPECT_TYPE_SAME(typename Iter::value_type, ExpectedValueType);
  EXPECT_TYPE_SAME(typename Iter::reference, ExpectedReference);
  EXPECT_TYPE_SAME(typename Iter::pointer, ExpectedPointer);
  EXPECT_TYPE_SAME(typename Iter::iterator_category, std::bidirectional_iterator_tag);
  EXPECT_TYPE_SAME(typename Iter::difference_type, std::ptrdiff_t);
}

TYPED_TEST(UniversalPtrTest, IteratorInternalTypesConst) {
  using ValPtr = TypeParam;
  using Iter = s21rb::Iterator<true, ValPtr>;

  using ExpectedNode = s21rb::Node<ValPtr>;
  using ExpectedNodeBase = s21rb::NodeBase<typename std::pointer_traits<ValPtr>::template rebind<void>>;
  using ExpectedBasePtr = typename std::pointer_traits<ValPtr>::template rebind<ExpectedNodeBase>;
  using ExpectedValueType = typename std::pointer_traits<ValPtr>::element_type;
  using ExpectedReference = const ExpectedValueType&;
  using ExpectedPointer = const ExpectedValueType*;

  EXPECT_TYPE_SAME(typename Iter::Node_, ExpectedNode);
  EXPECT_TYPE_SAME(typename Iter::NodeBase_, ExpectedNodeBase);
  EXPECT_TYPE_SAME(typename Iter::BasePtr_, ExpectedBasePtr);
  EXPECT_TYPE_SAME(typename Iter::value_type, ExpectedValueType);
  EXPECT_TYPE_SAME(typename Iter::reference, ExpectedReference);
  EXPECT_TYPE_SAME(typename Iter::pointer, ExpectedPointer);
  EXPECT_TYPE_SAME(typename Iter::iterator_category, std::bidirectional_iterator_tag);
  EXPECT_TYPE_SAME(typename Iter::difference_type, std::ptrdiff_t);
}

