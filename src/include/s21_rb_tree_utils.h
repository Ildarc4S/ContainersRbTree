#ifndef _S21_RB_TREE_UTILS_
#define _S21_RB_TREE_UTILS_

#include <memory>

namespace s21 {
namespace rb_tree {

enum class NodeColor : std::uint8_t {
  kBlack,
  kRed
};

template <typename Ptr_>
struct NodeBase {
  using BasePtr_ = std::pointer_traits<Ptr_>::template rebind<NodeBase>;

  NodeColor color_;
  BasePtr_ parent_;
  BasePtr_ right_;
  BasePtr_ left_;

  static BasePtr_ Minimum(BasePtr_ elm) noexcept;
  static BasePtr_ Maximum(BasePtr_ elm) noexcept;
  BasePtr_ GetBasePtr() const noexcept;
};

template <typename NodeBase_>
struct Header {
 private:
  using BasePtr_ = typename NodeBase_::BasePtr_;

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

  using ValueType_ = std::pointer_traits<ValPtr_>::element_type;
  using NodePtr_ = std::pointer_traits<ValPtr_>:: template rebind<Node>;

  Node() noexcept ;
  Node(Node&& other) = delete;

  union Storage_ {
    Storage_() noexcept {};
    ~Storage_() {};
    ValueType_ data_;
  };

  Storage_ storage_;

  ValueType_* Valptr();
  ValueType_ const*  Valptr() const;
  NodePtr_ Nodeptr() noexcept;
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

  using Node_ = Node<ValPtr_>;
  using NodeBase_ = NodeBase<
                      typename std::pointer_traits<ValPtr_>::
                      template rebind<void>>;
  using BasePtr_ = std::pointer_traits<ValPtr_>::
                      template rebind<NodeBase_>;

  using value_type = std::pointer_traits<ValPtr_>::element_type;
  using reference = MaybeConst_<value_type>&;
  using pointer = MaybeConst_<value_type>*;

  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;

  Iterator() noexcept = default;
  Iterator(const Iterator&) = default;

  constexpr explicit Iterator(BasePtr_ node) noexcept;
  constexpr Iterator(const Iterator<false, ValPtr_>& it) requires IsConst_;

  [[nodiscard]] reference operator*() const noexcept;
  [[nodiscard]] pointer operator->() const noexcept;

  constexpr Iterator& operator++() noexcept;
  constexpr Iterator& operator++(int) noexcept;

  constexpr Iterator& operator--() noexcept;
  constexpr Iterator& operator--(int) noexcept;

  template <bool B, typename T>
  friend bool operator==(const Iterator<B, T>& first,
                                       const Iterator<B, T>& second);
  template <bool B, typename T>
  friend bool operator!=(const Iterator<B, T>& first,
                                       const Iterator<B, T>& second);

  BasePtr_ node_;
};

} //  rb_tree
} //  s21

#endif //  _S21_RB_TREE_UTILS_

