#include <iostream>
#include <memory>

namespace s21 {
  enum class RbTreeColor : bool {
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
    NodeBase_ header;
    std::size_t node_count;
    Header() noexcept;
    Header(Header&& other) noexcept;
    void MoveData(Header&& other) noexcept;
    void Reset();
  };

  template <typename ValPtr_>
  struct Node : public NodeBase<
                typename std::pointer_traits<ValPtr_>::template rebind<void>> {

    using ValueType = typename std::pointer_traits<ValPtr_>::element_type;
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
  struct RbTreeKeyCompare {
    KeyCompare_ key_compare_;

    RbTreeKeyCompare()
      noexcept(std::is_nothrow_default_constructible_v<KeyCompare_>);
    RbTreeKeyCompare(const RbTreeKeyCompare& other) = default;
    RbTreeKeyCompare(RbTreeKeyCompare&& other)
      noexcept(std::is_nothrow_default_constructible_v<KeyCompare_>);
    explicit RbTreeKeyCompare(const KeyCompare_& other);
  };

  template <typename Key_, typename Val_, typename KeyOfValue_,
            typename Compare_, typename Alloc_ = std::allocator<Val_>>
  struct RbTree {
    
  };

}  // s21

