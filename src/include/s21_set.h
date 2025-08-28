#ifndef _S21_SET_H_
#define _S21_SET_H_

#include <functional>
#include <memory>

#include "rb_tree/s21_rb_tree.h"

namespace s21 {

template<typename Key_, typename Compare_ = std::less<Key_>,
         typename Alloc_ = std::allocator<Key_> >
class Set {
public:

  using key_type = Key_;
  using value_type = Key_;
  using key_compare = Compare_;
  using allocator_type = Alloc_;

private:
  using KeyAlloc_ = std::allocator_traits<allocator_type>::template rebind_alloc<value_type>;
  using RbTree_ = RbTree<key_type, value_type, rb_tree::Identity<value_type>,
                         key_compare, KeyAlloc_>;

  using AllocTraits_ = std::allocator_traits<KeyAlloc_>;

  RbTree_ rb_tree_;

  using reference = value_type&;
  using const_reference = const value_type&;

  using iterator = RbTree_::iterator;
  using const_iterator = RbTree_::const_iterator;

  using size_type = RbTree_::size_type;

  template<typename OtherCompare_>
	using OtherSet_ = Set<Key_, OtherCompare_, Alloc_>;

  template<typename, typename>
	friend struct RbTreeMergeHelper;

public:

  Set() = default;
  Set(const Set&) = default;
  Set(Set&&) = default;
  ~Set() = default;

  Set(std::initializer_list<value_type> const &items,
      const key_compare& compare = key_compare(),
      const allocator_type& alloc = allocator_type());

  Set& operator=(const Set& other) = default;
  Set& operator=(Set&& other) = default;

  iterator Begin() noexcept;
  iterator End() noexcept;

  const_iterator Begin() const noexcept;
  const_iterator End() const noexcept;

  bool Empty() const noexcept;
  size_type Size() const noexcept;
  size_type MaxSize() const noexcept;

  void Clear();
  std::pair<iterator, bool> Insert(const value_type& value);
  void Erase(iterator position);
  void Swap(Set& other);

  template<typename OtherCompare_>
  void Merge(OtherSet_<OtherCompare_>& other_set);

  iterator Find(const key_type& key);
  bool Contains(const key_type& key);
};

template<typename Key_, typename Compare_, typename Alloc_, typename OtherCompare_>
struct
RbTreeMergeHelper<Set<Key_, Compare_, Alloc_>, OtherCompare_> {
private:
  friend class Set<Key_, OtherCompare_, Alloc_>;

  static auto&
  GetRbTree(Set<Key_, OtherCompare_, Alloc_>& other_set) {
    return other_set.rb_tree_;
  }
};

template <typename Key_, typename Compare_, typename Alloc_>
Set<Key_, Compare_, Alloc_>::
Set(std::initializer_list<value_type> const &items,
    const key_compare& compare,
    const allocator_type& alloc)
: rb_tree_(compare, alloc) {
  rb_tree_.InsertRangeUnique(items.begin(), items.end());
}

template <typename Key_, typename Compare_, typename Alloc_>
Set<Key_, Compare_, Alloc_>::iterator
Set<Key_, Compare_, Alloc_>::Begin() noexcept {
  return rb_tree_.begin();
}

template <typename Key_, typename Compare_, typename Alloc_>
Set<Key_, Compare_, Alloc_>::iterator
Set<Key_, Compare_, Alloc_>::End() noexcept {
  return rb_tree_.end();
}

template <typename Key_, typename Compare_, typename Alloc_>
Set<Key_, Compare_, Alloc_>::const_iterator
Set<Key_, Compare_, Alloc_>::Begin() const noexcept {
  return rb_tree_.begin();
}

template <typename Key_, typename Compare_, typename Alloc_>
Set<Key_, Compare_, Alloc_>::const_iterator
Set<Key_, Compare_, Alloc_>::End() const noexcept {
  return rb_tree_.end();
}

template <typename Key_, typename Compare_, typename Alloc_>
bool
Set<Key_, Compare_, Alloc_>::Empty() const noexcept {
  return rb_tree_.Empty();
}

template <typename Key_, typename Compare_, typename Alloc_>
Set<Key_, Compare_, Alloc_>::size_type
Set<Key_, Compare_, Alloc_>::Size() const noexcept {
  return rb_tree_.Size();
}


template <typename Key_, typename Compare_, typename Alloc_>
Set<Key_, Compare_, Alloc_>::size_type
Set<Key_, Compare_, Alloc_>::MaxSize() const noexcept {
  return rb_tree_.MaxSize();
}

template <typename Key_, typename Compare_, typename Alloc_>
void
Set<Key_, Compare_, Alloc_>::Clear() {
  rb_tree_.Clear();
}

template <typename Key_, typename Compare_, typename Alloc_>
std::pair<typename Set<Key_, Compare_, Alloc_>::iterator, bool>
Set<Key_, Compare_, Alloc_>::Insert(const value_type& value) {
  return rb_tree_.InsertUnique(value);
}

template <typename Key_, typename Compare_, typename Alloc_>
void
Set<Key_, Compare_, Alloc_>::Erase(iterator position) {
  rb_tree_.Erase(position);
}

template <typename Key_, typename Compare_, typename Alloc_>
void
Set<Key_, Compare_, Alloc_>::Swap(Set& other) {
  rb_tree_.Swap(other.rb_tree_);
}

template <typename Key_, typename Compare_, typename Alloc_>
template<typename OtherCompare_>
void
Set<Key_, Compare_, Alloc_>::Merge(OtherSet_<OtherCompare_>& other) {
  rb_tree_.MergeUnique(RbTreeMergeHelper<Set, OtherCompare_>::GetRbTree(other));
}

template <typename Key_, typename Compare_, typename Alloc_>
Set<Key_, Compare_, Alloc_>::iterator
Set<Key_, Compare_, Alloc_>::Find(const key_type& key) {
  return rb_tree_.Find(key);
}

template <typename Key_, typename Compare_, typename Alloc_>
bool
Set<Key_, Compare_, Alloc_>::Contains(const key_type& key) {
  return rb_tree_.Find(key) != rb_tree_.end();
}

} //  namespace s21

#endif //  _S21_SET_H_
