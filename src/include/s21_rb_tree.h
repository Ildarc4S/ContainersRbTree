#ifndef _S21_RB_TREE_
#define _S21_RB_TREE_

#include "s21_rb_tree_utils.h"

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
  using NodePtr_ = NodeTraits_::NodePtr_;
  using Node_    = NodeTraits_::Node_;
  using Header_  = NodeTraits_::Header_;

  using NodeAlloc_       = std::allocator_traits<Alloc_>::template rebind<Node_>;
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

  iterator begin() noexcept;
  iterator end() noexcept;

  NodePtr_ NewNode();
  void DeleteNode(NodePtr_ node_ptr);

  template<typename Arg_>
  void ConstructNode(NodePtr_ node, Arg_ arg);

  template<typename Arg_>
  NodePtr_ CreateNode(Arg_ arg);

  template<typename Arg_>
  iterator InsertNode(BasePtr_ node, BasePtr_ parent, Arg_&& arg);

  std::pair<BasePtr_, BasePtr_> GetInsertUniquePos(const key_type& k);

  void RotateLeft(BasePtr_ x);
  void RotateRight(BasePtr_ x);

  void RebalanceInsert(BasePtr_ x);

public:
  template<typename Arg_>
  std::pair<iterator, bool> InsertUnique(Arg_&& x);

};

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
struct RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::AllocNode {
  AllocNode(RbTree& rb_tree);

	template<typename Arg_>
	_Node_ptr operator()(Arg_&& arg) const;

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
  return rb_tree.CreateNode(std::forward<Arg_>(arg));
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
  return node->left;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetRight(BasePtr_ node) const noexcept {
  return node->right;
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
const Key_&
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::GetKey(BasePtr_ node_ptr) {
  const Node& node = static_cast<const Node&>(*node_ptr);
  static_assert(std::is_invocable_v<const _Compare&, const _Key&, const _Key&>,
              "Comparison object must be invocable as const with two key arguments");

  return KeyOfValue_()(*node.GetValPtr());
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::begin() {
  return iterator(imp_.header_left_);
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::end() {
  return iterator(GetEnd());
}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::NodePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::NewNode() {
  auto ptr = NodeAllocTraits_::allocate(impl_, 1);
  if constexpr (!std::is_same_v<NodePtr_, NodeAllocTraits_::pointer>) {
    ptr = std::to_address(ptr);
  }
  return ptr;
}

template<typename Key_, typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::DeleteNode(NodePtr_ node_ptr) {
  if constexpr (std::is_same_v<NodePtr_, NodeAllocTraits_::pointer>) {
    NodeAllocTraits_::deallocate(impl, node_ptr, 1);
  } else {
    NodeAllocTraits_::deallocate(
      impl,
      std::pointer_traits<NodeAllocTraits_::pointer>::pointer_to(*node_ptr),
      1);
  }
}

template<typename Key_, typename Val_, typename KeyOfValue_, typename Compare_, typename Alloc_>
template<typename Arg_>
void RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::ConstructNode(NodePtr_ node, Arg_ arg) {
  try {
    ::new(std::addressof(*node)) Node_;
    NodeAllocTraits_::construct(impl, node->GetValPtr(), std::forward<Arg_>(arg));
  } catch (...) {
    node->~Node_();
    DeleteNode(node);
    throw;
  }
}

template<typename Key_, typename Val_, typename KeyOfValue_, typename Compare_, typename Alloc_>
template<typename Arg_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::NodePtr_
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::CreateNode(Arg_ arg) {
  NodePtr_ ptr = NewNode();
  ConstructNode(ptr, std::forward<Arg_>(arg));
  return ptr;
}


template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
std::pair<RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_,
          RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::BasePtr_>
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
  iterator it(node);

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

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Arg_>
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator
RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::InsertNode(BasePtr_ node, BasePtr_ parent,
                                                              Arg_&& arg) {
  AllocNode alloc_node(*this);
  BasePtr_ new_base = alloc_node(std::forward<Arg_>(arg))->GetBasePtr();

  bool insert_left = (node != nullptr || parent == GetEnd() ||
                      key_compare_(KeyOfValue_()(arg), GetKey(parent)));

  new_base->parent_ = parent;
  new_base->left_ = nullptr;
  new_base->right_ = nullptr;
  new_base->color_ = Color_::kRed;

  if (insert_left) {
    parent->left_ = new_base;

    if (parent == GetEnd()) {
      impl_.header.parent_ = new_base;
      impl_.header.left_ = new_base;
      impl_.header.right_ = new_base;
    } else if (parent == impl_.header.left_) {
      impl_.header.left_ = new_base;
    }
  } else {
    parent->right_ = new_base;
    if (parent == impl_.header.right_) {
      impl_.header.right_ = new_base;
    }
  }
  RebalanceInsert(new_base);

  ++impl_.node_count_;
  return iterator(new_base);

}

template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
template<typename Arg_>
std::pair<RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::iterator, bool>
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

  if (!x->parent_) {
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

  if (!x->parent_) {
    impl_.header_.parent_ = y;
  } else if (x == x->parent_->right_) {
    x->parent_->right_ = y;
  } else {
    x->parent_->left_ = y;
  }

  y->right_ = x;
  x->parent_ = y;
}


template<typename Key_,     typename Val_, typename KeyOfValue_,
         typename Compare_, typename Alloc_>
void RbTree<Key_, Val_, KeyOfValue_, Compare_, Alloc_>::RebalanceInsert(BasePtr_ x) {
  while (x->parent_ && x->parent_->color_ == Color_::kRed) {
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

} //  s21

#endif //  _S21_RB_TREE_
