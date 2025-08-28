#ifndef _S21_RB_TREE_
#define _S21_RB_TREE_

#include "s21_rb_tree_utils.h"
#include <queue>

namespace s21 {

template<typename Tree, typename OtherCompare_>
struct RbTreeMergeHelper {
};

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_ = std::allocator<Val_> >
class RbTree {
private:

  using ValAlloc_ = std::allocator_traits<Alloc_>::template rebind_alloc<Val_>;
  using ValPtr_ = std::allocator_traits<ValAlloc_>::pointer;
  using NodeTraits_ = rb_tree::NodeTraits<Val_, ValPtr_>;

public:
  using value_type = Val_;
  using iterator = NodeTraits_::Iterator_;
  using const_iterator = NodeTraits_::ConstIterator_;
  using key_type = Key_;
  using size_type = size_t;

private:
  using BasePtr_ = NodeTraits_::BasePtr_;
  using NodeBase_ = NodeTraits_::NodeBase_;
  using NodePtr_ = NodeTraits_::NodePtr_;
  using Node_    = NodeTraits_::Node_;
  using Header_  = NodeTraits_::Header_;

  using NodeAlloc_       = std::allocator_traits<Alloc_>::template rebind_alloc<Node_>;
  using NodeAllocTraits_ = std::allocator_traits<NodeAlloc_>;

  using Color_ = rb_tree::NodeColor;

  struct AllocNode;
  struct Impl;

  template<typename OtherCompare_>
	using OtherTree_ = RbTree<Key_, Val_, KeyOfValue_, OtherCompare_, Alloc_>;

  template<typename, typename>
  friend struct RbTreeMergeHelper;

  Compare_ key_compare_;
  Impl impl_;

private:

  BasePtr_ GetBegin() const noexcept;
  BasePtr_ GetEnd() const noexcept;
  BasePtr_ GetLeft(BasePtr_ node) const noexcept;
  BasePtr_ GetRight(BasePtr_ node) const noexcept;
  const Key_& GetKey(const BasePtr_ node);

  NodePtr_ NewNode();
  void DeleteNode(NodePtr_ node_ptr);

  template<typename Arg_>
  void ConstructNode(NodePtr_ node, Arg_ arg);
  void DestroyNode(NodePtr_ node);

  template<typename Arg_>
  NodePtr_ CreateNode(Arg_ arg);
  void DropNode(NodePtr_ node);

  template<typename Arg_>
  iterator InsertNode(BasePtr_ node, BasePtr_ parent, Arg_&& arg);
  void EraseNode(iterator position);
  void EraseSubtree(BasePtr_ node);

  BasePtr_ CopyTree(BasePtr_ node, BasePtr_ parent, AllocNode& alloc_node);

  std::pair<BasePtr_, BasePtr_> GetInsertUniquePos(const key_type& k);
  std::pair<BasePtr_, BasePtr_> GetInsertHintUniquePos(const_iterator hint, const key_type& key);

  std::pair<BasePtr_, BasePtr_> GetInsertEqualPos(const key_type& k);

  iterator LowerBound(BasePtr_ x, BasePtr_ y, const Key_& key);
  iterator UpperBound(BasePtr_ x, BasePtr_ y, const Key_& key);

  static void RotateLeft(BasePtr_ x, BasePtr_& root);
  static void RotateRight(BasePtr_ x,  BasePtr_& root);
  static void Transplant(BasePtr_ u, BasePtr_ v, NodeBase_& header);
  static void UpdateBoundaryPointers(NodeBase_& header);
  static void RebalanceInsert(BasePtr_ x, BasePtr_& root);
  static void RebalanceErase(BasePtr_ x, BasePtr_ x_parent, BasePtr_& root);

  static bool IsBlack(BasePtr_ node);
  static bool IsRed(BasePtr_ node);

public:

  RbTree() = default;
  RbTree(RbTree&&) = default;

  RbTree(const RbTree& other);
  RbTree(const Compare_& compare, const Alloc_& alloc);

  ~RbTree();

  RbTree& operator=(const RbTree& other_tree);
  RbTree& operator=(RbTree&& other_tree);

  template<typename Arg_>
  std::pair<iterator, bool> InsertUnique(Arg_&& x);

  template<typename Arg_>
  iterator InsertEqual(Arg_&& x);

  template<typename Arg_>
  iterator InsertHintUnique(iterator hint, Arg_&& x);

  template<typename Iter_>
  requires rb_tree::SameValueType<value_type, Iter_>
  void InsertRangeUnique(Iter_ begin, Iter_ end);

  template<typename Iter_>
  requires rb_tree::SameValueType<value_type, Iter_>
  void InsertRangeEqual(Iter_ begin, Iter_ end);

  iterator Erase(iterator position);
  iterator LowerBound(const key_type& key);
  iterator UpperBound(const key_type& key);
  std::pair<iterator, iterator> EqualRange(const key_type& key);
  iterator Find(const key_type& key);

  template <typename OtherCompare_, typename GetPosFunc>
  void Merge(OtherTree_<OtherCompare_>& other_tree, GetPosFunc get_pos) noexcept;

  template<typename OtherCompare_>
  void MergeUnique(OtherTree_<OtherCompare_>& other_tree) noexcept;

  template<typename OtherCompare_>
  void MergeEqual(OtherTree_<OtherCompare_>& other_tree) noexcept;

  void Swap(RbTree& other_tree);

  bool Empty() const noexcept;
  size_type Size() const noexcept;
  size_type Count(const Key_& key);
  size_type MaxSize() const noexcept;
  void Clear() noexcept;

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;

  static void PushNode(BasePtr_ parent, BasePtr_ new_node, NodeBase_& header, bool is_left);
  static BasePtr_ ExtractNode(BasePtr_ z, NodeBase_& header);

  void PrintTreeByLevelsSimple() {
    if (!impl_.header_.parent_ || impl_.header_.parent_ == GetEnd()) {
      std::cout << "Tree is empty!" << std::endl;
      return;
    }

    std::cout << "Tree:" << std::endl;

    std::queue<BasePtr_> q;
    q.push(impl_.header_.parent_);

    while (!q.empty()) {
      int level_size = q.size();

      for (int i = 0; i < level_size; ++i) {
        BasePtr_ node = q.front();
        q.pop();

        if (node == GetEnd() || !node) continue;

        const Key_& key = GetKey(node);
        std::string color = (node->color_ == Color_::kRed) ? "R" : "B";
        std::cout << key << color << ":" << (*static_cast<Node_&>(*node).GetValPtr()).second << " ";

        if (node->left_ && node->left_ != GetEnd()) {
          q.push(node->left_);
        }
        if (node->right_ && node->right_ != GetEnd()) {
          q.push(node->right_);
        }
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
};

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
struct RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::AllocNode {
  AllocNode(RbTree& rb_tree);

	template<typename Arg_>
	NodePtr_ operator()(Arg_&& arg) const;

private:
  RbTree& rb_tree_;
};

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
struct RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::Impl
: public NodeAlloc_
, public Header_ {
  Impl()
    noexcept(std::is_nothrow_default_constructible_v<NodeAlloc_>)
  : NodeAlloc_() {
  }

  Impl(const Impl& other)
    : NodeAlloc_(NodeAllocTraits_::select_on_container_copy_construction(other))
    , Header_() {
  }

  Impl(Impl&&) = default;

  Impl(NodeAlloc_&& node_alloc)
  : NodeAlloc_(std::move(node_alloc)) {
  }
};

template<typename Key_, typename Val_, typename KeyOfValue_, typename Compare_,
	       typename Alloc_, typename OtherCompare_>
struct RbTreeMergeHelper<
         RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>,
         OtherCompare_> {
private:
  friend class RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>;

  static auto&
  GetImpl(RbTree<Key_, Val_, KeyOfValue_, OtherCompare_, Alloc_>& rb_tree) {
    return rb_tree.impl_;
  }
};

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::AllocNode::AllocNode(RbTree& rb_tree)
: rb_tree_(rb_tree) {
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Arg_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::NodePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::AllocNode::operator()(Arg_&& arg) const {
  return rb_tree_.CreateNode(std::forward<Arg_>(arg));
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
RbTree(const RbTree& other)
  : key_compare_(other.key_compare_)
  , impl_(other.impl_) {
  if (other.impl_.header_.parent_ != nullptr) {
    AllocNode alloc_node(*this);
    impl_.header_.parent_ = CopyTree(other.GetBegin(), GetEnd(), alloc_node);
    UpdateBoundaryPointers(impl_.header_);
    impl_.node_count_ = other.impl_.node_count_;
  }
}


template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
RbTree(const Compare_& compare, const Alloc_& alloc)
  : key_compare_(compare)
  , impl_(NodeAlloc_(alloc)) {
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
~RbTree() {
  EraseSubtree(GetBegin());
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>&
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
operator=(const RbTree& other_tree) {
  if (this != std::addressof(other_tree)) {
    if constexpr (NodeAllocTraits_::propagate_on_container_copy_assignment::value) {
      impl_ = other_tree.impl_;
    }
    key_compare_ = other_tree.key_compare_;
    Clear();

    if (other_tree.impl_.header_.parent_ != nullptr) {
      AllocNode alloc_node(*this);
      impl_.header_.parent_ = CopyTree(other_tree.GetBegin(), GetEnd(), alloc_node);

      UpdateBoundaryPointers(impl_.header_);
      impl_.node_count_ =  other_tree.impl_.node_count_;
    }
  }
  return *this;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>&
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
operator=(RbTree&& other_tree) {
  if (this != std::addressof(other_tree)) {
    Clear();
    Swap(other_tree);
  }
  return *this;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetBegin() const noexcept {
  return impl_.header_.parent_;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetEnd() const noexcept {
  return impl_.header_.GetBasePtr();
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetLeft(BasePtr_ node) const noexcept {
  return node->left_;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetRight(BasePtr_ node) const noexcept {
  return node->right_;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
const Key_&
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetKey(BasePtr_ node_ptr) {
  const Node_& node = static_cast<const Node_&>(*node_ptr);
  static_assert(std::is_invocable_v<const Compare_&, const Key_&, const Key_&>,
              "Comparison object must be invocable as const with two key arguments");

  return KeyOfValue_()(*node.GetValPtr());
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::begin() noexcept {
  return iterator(impl_.header_.left_);
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::end() noexcept {
  return iterator(GetEnd());
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::const_iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::begin() const noexcept {
  return const_iterator(impl_.header_.left_);
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::const_iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::end() const noexcept {
  return const_iterator(GetEnd());
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::NodePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::NewNode() {
  auto ptr = NodeAllocTraits_::allocate(impl_, 1);
  if constexpr (!std::is_same_v<NodePtr_, typename NodeAllocTraits_::pointer>) {
    ptr = std::to_address(ptr);
  }
  return ptr;
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::DeleteNode(NodePtr_ node_ptr) {
  if constexpr (std::is_same_v<NodePtr_, typename NodeAllocTraits_::pointer>) {
    NodeAllocTraits_::deallocate(impl_, node_ptr, 1);
  } else {
    NodeAllocTraits_::deallocate(
      impl_,
      std::pointer_traits<typename NodeAllocTraits_::pointer>::pointer_to(*node_ptr),
      1);
  }
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Arg_>
void RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::ConstructNode(NodePtr_ node, Arg_ arg) {
  try {
    ::new(std::addressof(*node)) Node_;
    NodeAllocTraits_::construct(impl_, node->GetValPtr(), std::forward<Arg_>(arg));
  } catch (...) {
    node->~Node_();
    DeleteNode(node);
    throw;
  }
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::DestroyNode(NodePtr_ node) {
  NodeAllocTraits_::destroy(impl_, node->GetValPtr());
  node->~Node_();
}

template<typename Key_, typename Val_, typename KeyOfValue_, typename Compare_, typename Alloc_>
template<typename Arg_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::NodePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::CreateNode(Arg_ arg) {
  NodePtr_ ptr = NewNode();
  ConstructNode(ptr, std::forward<Arg_>(arg));
  return ptr;
}

template<typename Key_, typename Val_, typename KeyOfValue_, typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::DropNode(NodePtr_ node) {
  DestroyNode(node);
  DeleteNode(node);
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
std::pair<
  typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_,
  typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_
>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
GetInsertUniquePos(const key_type& k) {
  std::pair<BasePtr_, BasePtr_> result;
  BasePtr_ node = GetBegin();
  BasePtr_ parent = GetEnd();
  bool found_duplicate = false;

  while (node && !found_duplicate) {
    parent = node;

    if (key_compare_(k, GetKey(node))) {
      node = GetLeft(node);
    } else if (key_compare_(GetKey(node), k)) {
      node = GetRight(node);
    } else {
      found_duplicate = true;
    }
  }

  if (found_duplicate) {
    result = std::make_pair(parent, BasePtr_());
  } else {
    result = std::make_pair(BasePtr_(), parent);
  }

  return result;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
std::pair<typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_,
          typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetInsertEqualPos(const key_type& k) {

  BasePtr_ node = GetBegin();
  BasePtr_ parent = GetEnd();

  while(node) {
    parent = node;
    if (key_compare_(k, GetKey(node))) {
      node = GetLeft(node);
    } else {
      node = GetRight(node);
    }
  }

  return std::pair<BasePtr_, BasePtr_>(node, parent);
}


template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
std::pair<typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_,
          typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetInsertHintUniquePos(
  const_iterator hint, const key_type& k) {

  BasePtr_ x = hint.node_;
  std::pair<BasePtr_, BasePtr_> result{BasePtr_(), BasePtr_()};

  if (x == GetEnd()) {
    if (impl_.node_count_ > 0 && key_compare_(GetKey(impl_.header_.right_), k)) {
      result.second = impl_.header_.right_;
    } else {
      result = GetInsertUniquePos(k);
    }
  } else if (key_compare_(k, GetKey(x))) {
    iterator before(x);
    if (x == impl_.header_.left_) {
      result.first = impl_.header_.left_;
      result.second = impl_.header_.left_;
    } else if (!key_compare_(GetKey((--before).node_), k)) {
      if (!GetRight(before.node_)) {
        result.second = before.node_;
      } else {
        result.first = x;
        result.second = x;
      }
    } else {
      result = GetInsertUniquePos(k);
    }
  } else if (key_compare_(GetKey(x), k)) {
    iterator after(x);
    if (x == impl_.header_.right_) {
      result.second = impl_.header_.right_;
    } else if (!key_compare_(k, GetKey((++after).node_))) {
      if (!GetRight(x)) {
        result.second = x;
      } else {
        result.first = after.node_;
        result.second = after.node_;
      }
    } else {
      result = GetInsertUniquePos(k);
    }
  } else {
    result.first = x;
  }

  return result;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Arg_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::InsertNode(BasePtr_ node,
                                                              BasePtr_ parent,
                                                              Arg_&& arg) {
  AllocNode alloc_node(*this);
  BasePtr_ new_base = alloc_node(std::forward<Arg_>(arg))->GetBasePtr();

  bool insert_left = (node != nullptr || parent == GetEnd() ||
                      key_compare_(KeyOfValue_()(arg), GetKey(parent)));

  PushNode(parent, new_base, impl_.header_, insert_left);

  ++impl_.node_count_;
  return iterator(new_base);

}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Arg_>
std::pair<typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator,
          bool>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::InsertUnique(Arg_&& x) {
  using Result_ = std::pair<iterator, bool>;
  std::pair<BasePtr_, BasePtr_> res = GetInsertUniquePos(KeyOfValue_()(x));
  Result_ result = Result_(res.first, false);

  if (res.second) {
    result =  Result_(InsertNode(res.first, res.second, std::forward<Arg_>(x)), true);
  }

  return result;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Arg_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::InsertEqual(Arg_&& x) {
  std::pair<BasePtr_, BasePtr_> res = GetInsertEqualPos(KeyOfValue_()(x));
  return InsertNode(res.first, res.second, std::forward<Arg_>(x));
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Arg_>
typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
InsertHintUnique(iterator hint, Arg_&& arg) {
  std::pair<BasePtr_, BasePtr_> pos = GetInsertHintUniquePos(hint, arg);
  iterator result(pos.first);
  if (pos.second) {
    result = InsertNode(pos.first,
                        pos.second,
                        std::make_pair(std::forward<Arg_>(arg),
                                       typename Val_::second_type{}));
  }

  return result;
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
Erase(iterator position) {
  if (position == end()) {
    throw std::out_of_range("Iterator cannot be end");
  }
  iterator result = position;
  ++result;
  EraseNode(position);

  return result;
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template <typename OtherCompare_, typename GetPosFunc>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
Merge(OtherTree_<OtherCompare_>& other_tree, GetPosFunc get_pos) noexcept {
  auto it = other_tree.begin();
  auto end = other_tree.end();
  while (it != end) {
    auto current = it++;
    auto pos = get_pos(KeyOfValue_()(*current));

    if (pos.second) {
      auto& other_impl = RbTreeMergeHelper<RbTree, OtherCompare_>::GetImpl(other_tree);

      BasePtr_ node_ptr = ExtractNode(current.node_, other_impl.header_);
      --other_impl.node_count_;

      bool insert_left = (pos.first != nullptr || pos.second == GetEnd() ||
                          key_compare_(GetKey(node_ptr), GetKey(pos.second)));

      PushNode(pos.second, node_ptr, impl_.header_, insert_left);
      ++impl_.node_count_;
    }
  }
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template <typename OtherCompare_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
MergeUnique(OtherTree_<OtherCompare_>& other_tree) noexcept {
  Merge(other_tree, [this](const key_type& key) {
    return GetInsertUniquePos(key);
  });
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template <typename OtherCompare_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
MergeEqual(OtherTree_<OtherCompare_>& other_tree) noexcept {
  Merge(other_tree, [this](const key_type& key) {
    return GetInsertEqualPos(key);
  });
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
Swap(RbTree& other_tree) {
  impl_.SwapData(other_tree.impl_);
  std::swap(key_compare_, other_tree.key_compare_);

  if constexpr (NodeAllocTraits_::propagate_on_container_swap::value) {
    std::swap(impl_, other_tree.impl_);
  }
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
bool
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
Empty() const noexcept {
  return impl_.node_count_ == 0;
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::size_type
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
Size() const noexcept {
  return impl_.node_count_;
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::size_type
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
Count(const Key_& key) {
  auto range = EqualRange(key);
  return rb_tree::distance(range.first, range.second);
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::size_type
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
MaxSize() const noexcept {
  return NodeAllocTraits_::max_size(impl_);
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
Clear() noexcept {
  EraseSubtree(GetBegin());
  impl_.Reset();
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
PushNode(BasePtr_ parent, BasePtr_ new_node, NodeBase_& header, bool insert_left) {
  new_node->parent_ = parent;
  new_node->left_ = new_node->right_ = nullptr;
  new_node->color_ = Color_::kRed;

  if (insert_left) {
    parent->left_ = new_node;

    if (parent == header.GetBasePtr()) {
      header.parent_ = new_node;
      header.left_ = new_node;
      header.right_ = new_node;
    } else if (parent == header.left_) {
      header.left_ = new_node;
    }
  } else {
    parent->right_ = new_node;
    if (parent == header.right_) {
      header.right_ = new_node;
    }
  }
  RebalanceInsert(new_node, header.parent_);
  UpdateBoundaryPointers(header);
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
ExtractNode(BasePtr_ z, NodeBase_& header) {
  BasePtr_ y = z;
  Color_ y_original_color = y->color_;
  BasePtr_ x = nullptr;
  BasePtr_ x_parent = z->parent_;

  if (z->left_ == nullptr) {
    x = z->right_;
    Transplant(z, z->right_, header);
  } else if (z->right_ == nullptr) {
    x = z->left_;
    x_parent = z;
    Transplant(z, z->left_, header);
  } else {
    y = z->right_;
    while (y->left_ != nullptr) {
      y = y->left_;
    }
    y_original_color = y->color_;
    x = y->right_;
    x_parent = y;

    if (y->parent_ == z) {
      if (x != nullptr) {
        x->parent_ = y;
      }
    } else {
      Transplant(y, y->right_, header);
      y->right_ = z->right_;
      if (y->right_ != nullptr) {
        y->right_->parent_ = y;
      }
      x_parent = y->parent_;
    }

    Transplant(z, y, header);
    y->left_ = z->left_;
    if (y->left_ != nullptr) {
      y->left_->parent_ = y;
    }
    y->color_ = z->color_;
  }

  if (y_original_color == Color_::kBlack) {
    RebalanceErase(x, x_parent, header.parent_);
  }

  UpdateBoundaryPointers(header);

  return z;
}


template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
EraseNode(iterator position) {
  BasePtr_ extract_node = ExtractNode(position.node_, impl_.header_);
  DropNode(static_cast<Node_&>(*extract_node).GetNodePtr());
  --impl_.node_count_;
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
EraseSubtree(BasePtr_ node) {
  while (node && node != GetEnd()) {
    EraseSubtree(node->right_);
    BasePtr_ left = node->left_;
    DropNode(static_cast<Node_&>(*node).GetNodePtr());
    node = left;
  }
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
CopyTree(BasePtr_ node, BasePtr_ parent, AllocNode& alloc_node) {
  if (!node) {
    return nullptr;
  }

  NodePtr_ new_node = alloc_node(*static_cast<Node_*>(node)->GetValPtr());
  new_node->color_ = node->color_;
  new_node->left_ = new_node->right_ = BasePtr_();
  new_node->parent_ = parent;

  BasePtr_ new_base_ptr = new_node->GetBasePtr();

  try {
    if (GetRight(node)) {
      new_node->right_ = CopyTree(GetRight(node), new_base_ptr, alloc_node);
    }

    BasePtr_ current_parent = new_base_ptr;
    BasePtr_ current_source = GetLeft(node);

    while (current_source) {
      NodePtr_ left_child_node = alloc_node(*static_cast<Node_*>(current_source)->GetValPtr());
      left_child_node->color_ = current_source->color_;
      left_child_node->left_ = left_child_node->right_ = BasePtr_();
      left_child_node->parent_ = current_parent;

      BasePtr_ left_child_base = left_child_node->GetBasePtr();

      current_parent->left_ = left_child_base;
      left_child_base->parent_ = current_parent;

      if (GetRight(current_source)) {
        left_child_base->right_ = CopyTree(GetRight(current_source), left_child_base, alloc_node);
      }

      current_parent = left_child_base;
      current_source = GetLeft(current_source);
    }
  } catch (...) {
    EraseSubtree(new_base_ptr);
    throw;
  }

  return new_base_ptr;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::LowerBound(const Key_& key) {
  return LowerBound(GetBegin(), GetEnd(), key);
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::UpperBound(const Key_& key) {
  return UpperBound(GetBegin(), GetEnd(), key);
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
std::pair<
  typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator,
  typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
EqualRange(const Key_& key) {
  BasePtr_ current_node = GetBegin();
  BasePtr_ upper_bound_candidate = GetEnd();

  std::pair<iterator, iterator> result{
    iterator(upper_bound_candidate),
    iterator(upper_bound_candidate)};
  bool found = false;

  while (current_node && !found) {
    if (key_compare_(GetKey(current_node), key)) {
      current_node = GetRight(current_node);
    } else if (key_compare_(key, GetKey(current_node))) {
      upper_bound_candidate = current_node;
      current_node = GetLeft(current_node);
    } else {
      iterator lower_bound = LowerBound(GetLeft(current_node),
                                        current_node, key);
      iterator upper_bound = UpperBound(GetRight(current_node),
                                        upper_bound_candidate, key);

      result = std::make_pair(lower_bound, upper_bound);
      found = true;
    }
  }

  return result;
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Iter_>
requires rb_tree::SameValueType<Val_, Iter_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
InsertRangeUnique(Iter_ begin, Iter_ end) {
  while (begin != end) {
    InsertUnique(*begin++);
  }
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Iter_>
requires rb_tree::SameValueType<Val_, Iter_>
void
s21::RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
InsertRangeEqual(Iter_ begin, Iter_ end) {
  while (begin != end) {
    InsertEqual(*begin++);
  }
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::LowerBound(BasePtr_ x,
                                                              BasePtr_ y,
                                                              const Key_& key) {
  while (x) {
    if (!key_compare_(GetKey(x), key)) {
      y = x;
      x = GetLeft(x);
    } else {
      x = GetRight(x);
    }
  }

  return iterator(y);
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::UpperBound(BasePtr_ x,
                                                              BasePtr_ y,
                                                              const Key_& key) {
  while (x) {
    if (key_compare_(key, GetKey(x))) {
      y = x;
      x = GetLeft(x);
    } else {
      x = GetRight(x);
    }
  }

  return iterator(y);
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::Find(const key_type& key) {
  iterator it = LowerBound(key);
  if (it != end() && key_compare_(key, GetKey(it.node_))) {
    it = end();
  }
  return it;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
RotateLeft(BasePtr_ x, BasePtr_& root) {
  if (!x->right_) {
    return;
  }

  BasePtr_ y = x->right_;
  x->right_ = y->left_;

  if (y->left_) {
    y->left_->parent_ = x;
  }
  y->parent_ = x->parent_;

  if (x == root) {
    root = y;
  } else if (x == x->parent_->left_) {
    x->parent_->left_ = y;
  } else {
    x->parent_->right_ = y;
  }

  y->left_ = x;
  x->parent_ = y;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
RotateRight(BasePtr_ x, BasePtr_& root) {
  if (!x->left_) {
    return;
  }

  BasePtr_ y = x->left_;
  x->left_ = y->right_;

  if (y->right_) {
    y->right_->parent_ = x;
  }

  y->parent_ = x->parent_;

  if (x == root) {
    root = y;
  } else if (x == x->parent_->right_) {
    x->parent_->right_ = y;
  } else {
    x->parent_->left_ = y;
  }

  y->right_ = x;
  x->parent_ = y;
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
Transplant(BasePtr_ u, BasePtr_ v, NodeBase_& header) {
  if (u->parent_ == header.GetBasePtr()) {
    header.parent_ = v;
  } else if (u == u->parent_->left_) {
    u->parent_->left_ = v;
  } else {
    u->parent_->right_ = v;
  }

  if (v != nullptr) {
    v->parent_ = u->parent_;
  }
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::UpdateBoundaryPointers(NodeBase_& header) {
  if (header.parent_ != header.GetBasePtr() && header.parent_ != nullptr) {
    header.left_ = NodeBase_::Minimum(header.parent_);
    header.right_ = NodeBase_::Maximum(header.parent_);
  } else {
    header.parent_ = header.GetBasePtr();
    header.left_ = header.GetBasePtr();
    header.right_ = header.GetBasePtr();
  }
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
RebalanceInsert(BasePtr_ x, BasePtr_& root) {
  while (x != root && x->parent_->color_ == Color_::kRed) {
    if (x->parent_ == x->parent_->parent_->left_) {
      BasePtr_ u = x->parent_->parent_->right_;

      if (u && u->color_ == Color_::kRed) {
        x->parent_->color_ = Color_::kBlack;
        u->color_ = Color_::kBlack;
        x->parent_->parent_->color_ = Color_::kRed;
        x = x->parent_->parent_;
      } else {
        if (x == x->parent_->right_) {
          x = x->parent_;
          RotateLeft(x, root);
        }
        x->parent_->color_ = Color_::kBlack;
        x->parent_->parent_->color_ = Color_::kRed;
        RotateRight(x->parent_->parent_, root);
      }
    } else {
      BasePtr_ u = x->parent_->parent_->left_;

      if (u && u->color_ == Color_::kRed) {
        x->parent_->color_ = Color_::kBlack;
        u->color_ = Color_::kBlack;
        x->parent_->parent_->color_ = Color_::kRed;
        x = x->parent_->parent_;
      } else {
        if (x == x->parent_->left_) {
          x = x->parent_;
          RotateRight(x, root);
        }
        x->parent_->color_ = Color_::kBlack;
        x->parent_->parent_->color_ = Color_::kRed;
        RotateLeft(x->parent_->parent_, root);
      }
    }
  }

  root->color_ = Color_::kBlack;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
RebalanceErase(BasePtr_ x, BasePtr_ x_parent, BasePtr_& root) {
  while (x != root && IsBlack(x) && x_parent != nullptr) {
    if (x == x_parent->left_) {
      BasePtr_ w = x_parent->right_;

      if (IsRed(w)) {
        w->color_ = Color_::kBlack;
        x_parent->color_ = Color_::kRed;
        RotateLeft(x_parent, root);
        w = x_parent->right_;
      }
      if (IsBlack(w->left_) && IsBlack(w->right_)) {
        w->color_ = Color_::kRed;
        x = x_parent;
        x_parent = x->parent_;
      } else {
        if (IsBlack(w->right_)) {
          w->left_->color_ = Color_::kBlack;
          w->color_ = Color_::kRed;
          RotateRight(w, root);
          w = x_parent->right_;
        }

        w->color_ = x_parent->color_;
        x_parent->color_ = Color_::kBlack;
        w->right_->color_ = Color_::kBlack;
        RotateLeft(x_parent, root);
        x = root;
        x_parent = nullptr;
      }
    } else {
      BasePtr_ w = x_parent->left_;

      if (IsRed(w)) {
        w->color_ = Color_::kBlack;
        x_parent->color_ = Color_::kRed;
        RotateRight(x_parent, root);
        w = x_parent->left_;
      }
      if (IsBlack(w->right_) && IsBlack(w->left_)) {
        w->color_ = Color_::kRed;
        x = x_parent;
        x_parent = x->parent_;
      } else {
        if (IsBlack(w->left_)) {
          w->right_->color_ = Color_::kBlack;
          w->color_ = Color_::kRed;
          RotateLeft(w, root);
          w = x_parent->left_;
        }

        w->color_ = x_parent->color_;
        x_parent->color_ = Color_::kBlack;
        w->left_->color_ = Color_::kBlack;
        RotateRight(x_parent, root);
        x = root;
        x_parent = nullptr;
      }
    }
  }

  if (x) {
    x->color_ = Color_::kBlack;
  }
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
bool RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::IsBlack(BasePtr_ node) {
  return node == nullptr || node->color_ == Color_::kBlack;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
bool RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::IsRed(BasePtr_ node) {
  return node != nullptr && node->color_ == Color_::kRed;
}

} //  s21

#endif //  _S21_RB_TREE_
