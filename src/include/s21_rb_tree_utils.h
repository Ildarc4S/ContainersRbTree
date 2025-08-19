#ifndef _S21_RB_TREE_UTILS_
#define _S21_RB_TREE_UTILS_

#include <iostream>
#include <memory>

namespace s21 {
namespace rb_tree {

enum class RbTreeColor : std::uint8_t {
  kBlack,
  kRed
};

template <typename Ptr_>
struct NodeBase {
  using BasePtr = std::pointer_traits<Ptr_>::template rebind<NodeBase>;

  RbTreeColor color_;
  BasePtr parent_;
  BasePtr right_;
  BasePtr left_;

  static BasePtr Minimum(BasePtr elm) noexcept;
  static BasePtr Maximum(BasePtr elm) noexcept;
  BasePtr GetBasePtr() const noexcept;
};

template <typename NodeBase_>
struct Header {
 private:
  using BasePtr = typename NodeBase_::BasePtr;

 public:
  Header() noexcept;
  Header(Header&& other) noexcept;

  void MoveData(Header&& other) noexcept;
  void Reset();

  NodeBase_ header_;
  std::size_t node_count_;
};

template <typename ValPtr_>
struct Node : public NodeBase<
              typename std::pointer_traits<ValPtr_>::
              template rebind<void>> {

  using ValueType = std::pointer_traits<ValPtr_>::element_type;
  using NodePtr = std::pointer_traits<ValPtr_>:: template rebind<Node>;

  Node() noexcept ;
  Node(Node&& other) = delete;

  union Storage_ {
    Storage_() noexcept {};
    ~Storage_() {};
    ValueType data_;
  };

  Storage_ storage_;

  ValueType* Valptr();
  ValueType const*  Valptr() const;
  NodePtr Nodeptr() noexcept;
};

template <typename KeyCompare_>
struct KeyCompare {
  KeyCompare_ key_compare_;

  KeyCompare()
    noexcept(std::is_nothrow_default_constructible_v<KeyCompare_>);
  KeyCompare(const KeyCompare& other) = default;
  KeyCompare(KeyCompare&& other)
    noexcept(std::is_nothrow_default_constructible_v<KeyCompare_>);
  explicit KeyCompare(const KeyCompare_& other);
};

template <bool IsConst_, typename ValPtr_>
struct Iterator {

  template <typename T>
  using MaybeConst_ = std::conditional_t<IsConst_, const T, T>;

  using Node =  Node<ValPtr_>;
  using NodeBase = NodeBase<std::pointer_traits<ValPtr_>:: template rebind<void>>;
  using BasePtr = typename NodeBase::BasePtr;

  using value_type = std::pointer_traits<ValPtr_>::element_type;
  using reference = MaybeConst_<value_type>&;
  using pointer = MaybeConst_<value_type>*;

  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;

  Iterator() noexcept = default;
  Iterator(const Iterator&) = default;

  constexpr explicit Iterator(BasePtr node) noexcept;
  constexpr Iterator(const Iterator<false, ValPtr_>& it) requires IsConst_;

  [[nodiscard]] reference operator*() const noexcept;
  [[nodiscard]] pointer operator->() const noexcept;

  constexpr Iterator& operator++() noexcept;
  constexpr Iterator& operator++(int) noexcept;

  constexpr Iterator& operator--() noexcept;
  constexpr Iterator& operator--(int) noexcept;

  [[nodiscard]] friend bool operator==(const Iterator& first,
                                       const Iterator& second);
  [[nodiscard]] friend bool operator!=(const Iterator& first,
                                       const Iterator& second);

  BasePtr node_;
};

} //  rb_tree
} //  s21

#endif //  _S21_RB_TREE_UTILS_

