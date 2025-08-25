#ifndef _S21_RB_TREE_
#define _S21_RB_TREE_

#include "s21_rb_tree_utils.h"
#include <queue>

namespace s21 {

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_ = std::allocator<Val_> >
class RbTree {
private:

  using ValAlloc_ = std::allocator_traits<Alloc_>::template rebind_alloc<Val_>;
  using ValPtr_ = std::allocator_traits<ValAlloc_>::pointer;
  using NodeTraits_ = rb_tree::NodeTraits<Val_, ValPtr_>;

public:

  using iterator = NodeTraits_::Iterator_;
  using const_iterator = NodeTraits_::ConstIterator_;
  using key_type = Key_;
  // using size_type = ;

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

  std::pair<BasePtr_, BasePtr_> GetInsertUniquePos(const key_type& k);
  std::pair<BasePtr_, BasePtr_> GetInsertHintUniquePos(const_iterator hint, const key_type& key);

  void RotateLeft(BasePtr_ x);
  void RotateRight(BasePtr_ x);
  void Transplant(BasePtr_ u, BasePtr_ v);
  void UpdateBoundaryPointers();
  void RebalanceInsert(BasePtr_ x);
  void RebalanceErase(BasePtr_ x, BasePtr_ x_parent);

  bool IsBlack(BasePtr_ node);
  bool IsRed(BasePtr_ node);

public:

  RbTree() = default;

  template<typename Arg_>
  std::pair<iterator, bool> InsertUnique(Arg_&& x);

  template<typename Arg_>
  iterator InsertHintUnique(iterator hint, Arg_&& x);

  iterator Erase(iterator position);

  iterator LowerBound(const key_type& key);
  iterator LowerBound(BasePtr_ x, BasePtr_ y, const Key_& key);

  iterator begin() noexcept;
  iterator end() noexcept;

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
  node->~Node();
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
std::pair<typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_,
          typename RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetInsertUniquePos(const key_type& k) {

  BasePtr_ node = GetBegin();
  BasePtr_ parent = GetEnd();
  bool is_compare = true;

  while(node) {
    parent = node;
    is_compare = key_compare_(k, GetKey(node));
    if (is_compare) {
      node = GetLeft(node);
    } else {
      node = GetRight(node);
    }
  }

  using Result_ = std::pair<BasePtr_, BasePtr_>;
  Result_ result = Result_(node, parent);
  iterator it(parent);

  if (is_compare) {
    if (it != begin()) {
      --it;
    }
    if (!key_compare_(GetKey(it.node_), k)) {
      result = Result_(it.node_, BasePtr_());
    }
  } else {
    if (!key_compare_(GetKey(it.node_), k)) {
      result = Result_(it.node_, BasePtr_());
    }
  }

  return result;
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

  new_base->parent_ = parent;
  new_base->left_ = new_base->right_ = nullptr;
  new_base->color_ = Color_::kRed;

  if (insert_left) {
    parent->left_ = new_base;

    if (parent == GetEnd()) {
      impl_.header_.parent_ = new_base;
      impl_.header_.left_ = new_base;
      impl_.header_.right_ = new_base;
    } else if (parent == impl_.header_.left_) {
      impl_.header_.left_ = new_base;
    }
  } else {
    parent->right_ = new_base;
    if (parent == impl_.header_.right_) {
      impl_.header_.right_ = new_base;
    }
  }
  RebalanceInsert(new_base);

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
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
EraseNode(iterator position) {
  BasePtr_ z = position.node_;
  BasePtr_ y = z;
  Color_ y_original_color = y->color_;
  BasePtr_ x = nullptr;
  BasePtr_ x_parent = z->parent_;

  if (z->left_ == nullptr) {
    x = z->right_;
    Transplant(z, z->right_);
  } else if (z->right_ == nullptr) {
    x = z->left_;
    x_parent = z;
    Transplant(z, z->left_);
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
      Transplant(y, y->right_);
      y->right_ = z->right_;
      if (y->right_ != nullptr) {
        y->right_->parent_ = y;
      }
      x_parent = y->parent_;
    }

    Transplant(z, y);
    y->left_ = z->left_;
    if (y->left_ != nullptr) {
      y->left_->parent_ = y;
    }
    y->color_ = z->color_;
  }

  UpdateBoundaryPointers();
  DropNode(static_cast<Node_&>(*z).GetNodePtr());

  --impl_.node_count_;

  if (y_original_color == Color_::kBlack) {
    RebalanceErase(x, x_parent);
  }
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
void RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::RotateLeft(BasePtr_ x) {
  if (!x->right_) {
    return;
  }

  BasePtr_ y = x->right_;
  x->right_ = y->left_;

  if (y->left_) {
    y->left_->parent_ = x;
  }
  y->parent_ = x->parent_;

  if (x == impl_.header_.parent_) {
    impl_.header_.parent_ = y;
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
void RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::RotateRight(BasePtr_ x) {
  if (!x->left_) {
    return;
  }

  BasePtr_ y = x->left_;
  x->left_ = y->right_;

  if (y->right_) {
    y->right_->parent_ = x;
  }

  y->parent_ = x->parent_;

  if (x == impl_.header_.parent_) {
    impl_.header_.parent_ = y;
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
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::Transplant(BasePtr_ u, BasePtr_ v) {
  if (u->parent_ == GetEnd()) {
    impl_.header_.parent_ = v;
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
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::UpdateBoundaryPointers() {
  if (impl_.header_.parent_ != GetEnd() && impl_.header_.parent_ != nullptr) {
    impl_.header_.left_ = NodeBase_::Minimum(impl_.header_.parent_);
    impl_.header_.right_ = NodeBase_::Maximum(impl_.header_.parent_);
  } else {
    impl_.header_.parent_ = GetEnd();
    impl_.header_.left_ = GetEnd();
    impl_.header_.right_ = GetEnd();
  }
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::RebalanceInsert(BasePtr_ x) {
  while (x != impl_.header_.parent_ && x->parent_->color_ == Color_::kRed) {
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
          RotateLeft(x);
        }
        x->parent_->color_ = Color_::kBlack;
        x->parent_->parent_->color_ = Color_::kRed;
        RotateRight(x->parent_->parent_);
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
          RotateRight(x);
        }
        x->parent_->color_ = Color_::kBlack;
        x->parent_->parent_->color_ = Color_::kRed;
        RotateLeft(x->parent_->parent_);
      }
    }
  }

  impl_.header_.parent_->color_ = Color_::kBlack;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::
RebalanceErase(BasePtr_ x, BasePtr_ x_parent) {
  while (x != impl_.header_.parent_ && IsBlack(x)) {
    if (x_parent == nullptr) {
      break;
    }

    if (x == x_parent->left_) {
      BasePtr_ w = x_parent->right_;

      if (IsRed(w)) {
        w->color_ = Color_::kBlack;
        x_parent->color_ = Color_::kRed;
        RotateLeft(x_parent);
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
          RotateRight(w);
          w = x_parent->right_;
        }

        w->color_ = x_parent->color_;
        x_parent->color_ = Color_::kBlack;
        w->right_->color_ = Color_::kBlack;
        RotateLeft(x_parent);
        x = impl_.header_.parent_;
        x_parent = nullptr;
      }
    } else {
      BasePtr_ w = x_parent->left_;

      if (IsRed(w)) {
        w->color_ = Color_::kBlack;
        x_parent->color_ = Color_::kRed;
        RotateRight(x_parent);
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
          RotateLeft(w);
          w = x_parent->left_;
        }

        w->color_ = x_parent->color_;
        x_parent->color_ = Color_::kBlack;
        w->left_->color_ = Color_::kBlack;
        RotateRight(x_parent);
        x = impl_.header_.parent_;
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
