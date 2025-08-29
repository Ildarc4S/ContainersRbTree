#ifndef _S21_MULTISET_H_
#define _S21_MULTISET_H_

#include <functional>
#include <memory>

#include "rb_tree/s21_rb_tree.h"

namespace s21 {

template<typename Key_, typename Compare_ = std::less<Key_>,
         typename Alloc_ = std::allocator<Key_> >
class Multiset {
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
	using OtherMultiset_ = Multiset<Key_, OtherCompare_, Alloc_>;

  template<typename, typename>
	friend struct RbTreeMergeHelper;

public:

  Multiset() = default;
  Multiset(const Multiset&) = default;
  Multiset(Multiset&&) = default;
  ~Multiset() = default;

  Multiset(std::initializer_list<value_type> const &items,
           const key_compare& compare = key_compare(),
           const allocator_type& alloc = allocator_type());

  Multiset& operator=(const Multiset&) = default;
  Multiset& operator=(Multiset&&) = default;

  iterator Begin() noexcept;
  iterator End() noexcept;

  const_iterator Begin() const noexcept;
  const_iterator End() const noexcept;

  bool Empty() const noexcept;
  size_type Size() const noexcept;
  size_type MaxSize() const noexcept;

  void Clear();
  iterator Insert(const value_type& value);
  void Erase(iterator position);
  void Swap(Multiset& other);

  template<typename OtherCompare_>
  void Merge(OtherMultiset_<OtherCompare_>& other_set);

  size_type Count(const key_type& key) const;

  iterator Find(const key_type& key);
  const_iterator Find(const key_type& key) const;

  bool Contains(const key_type& key) const;

  std::pair<iterator, iterator> EqualRange(const key_type& key);
  std::pair<const_iterator, const_iterator> EqualRange(const key_type& key) const;

  iterator LowerBound(const key_type& key);
  iterator UpperBound(const key_type& key);

  const_iterator LowerBound(const key_type& key) const;
  const_iterator UpperBound(const key_type& key) const;
};

template<typename Key_, typename Compare_, typename Alloc_, typename OtherCompare_>
struct
RbTreeMergeHelper<Multiset<Key_, Compare_, Alloc_>, OtherCompare_> {
private:
  friend class Multiset<Key_, OtherCompare_, Alloc_>;

  static auto&
  GetRbTree(Multiset<Key_, OtherCompare_, Alloc_>& other_multiset) {
    return other_multiset.rb_tree_;
  }
};

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::
Multiset(std::initializer_list<value_type> const &items,
    const key_compare& compare,
    const allocator_type& alloc)
: rb_tree_(compare, alloc) {
  rb_tree_.InsertRangeEqual(items.begin(), items.end());
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::iterator
Multiset<Key_, Compare_, Alloc_>::Begin() noexcept {
  return rb_tree_.begin();
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::iterator
Multiset<Key_, Compare_, Alloc_>::End() noexcept {
  return rb_tree_.end();
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::const_iterator
Multiset<Key_, Compare_, Alloc_>::Begin() const noexcept {
  return rb_tree_.begin();
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::const_iterator
Multiset<Key_, Compare_, Alloc_>::End() const noexcept {
  return rb_tree_.end();
}

template <typename Key_, typename Compare_, typename Alloc_>
bool
Multiset<Key_, Compare_, Alloc_>::Empty() const noexcept {
  return rb_tree_.Empty();
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::size_type
Multiset<Key_, Compare_, Alloc_>::Size() const noexcept {
  return rb_tree_.Size();
}


template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::size_type
Multiset<Key_, Compare_, Alloc_>::MaxSize() const noexcept {
  return rb_tree_.MaxSize();
}

template <typename Key_, typename Compare_, typename Alloc_>
void
Multiset<Key_, Compare_, Alloc_>::Clear() {
  rb_tree_.Clear();
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::iterator
Multiset<Key_, Compare_, Alloc_>::Insert(const value_type& value) {
  return rb_tree_.InsertEqual(value);
}

template <typename Key_, typename Compare_, typename Alloc_>
void
Multiset<Key_, Compare_, Alloc_>::Erase(iterator position) {
  rb_tree_.Erase(position);
}

template <typename Key_, typename Compare_, typename Alloc_>
void
Multiset<Key_, Compare_, Alloc_>::Swap(Multiset& other) {
  rb_tree_.Swap(other.rb_tree_);
}

template <typename Key_, typename Compare_, typename Alloc_>
template<typename OtherCompare_>
void
Multiset<Key_, Compare_, Alloc_>::Merge(OtherMultiset_<OtherCompare_>& other) {
  rb_tree_.MergeEqual(RbTreeMergeHelper<Multiset, OtherCompare_>::GetRbTree(other));
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::size_type
Multiset<Key_, Compare_, Alloc_>::Count(const key_type& key) const {
  return rb_tree_.Count(key);
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::iterator
Multiset<Key_, Compare_, Alloc_>::Find(const key_type& key) {
  return rb_tree_.Find(key);
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::const_iterator
Multiset<Key_, Compare_, Alloc_>::Find(const key_type& key) const {
  return rb_tree_.Find(key);
}

template <typename Key_, typename Compare_, typename Alloc_>
bool
Multiset<Key_, Compare_, Alloc_>::Contains(const key_type& key) const {
  return rb_tree_.Find(key) != rb_tree_.end();
}

template <typename Key_, typename Compare_, typename Alloc_>
std::pair<
  typename Multiset<Key_, Compare_, Alloc_>::iterator,
  typename Multiset<Key_, Compare_, Alloc_>::iterator
>
Multiset<Key_, Compare_, Alloc_>::EqualRange(const key_type& key) {
  return rb_tree_.EqualRange(key);
}

template <typename Key_, typename Compare_, typename Alloc_>
std::pair<
  typename Multiset<Key_, Compare_, Alloc_>::const_iterator,
  typename Multiset<Key_, Compare_, Alloc_>::const_iterator
>
Multiset<Key_, Compare_, Alloc_>::EqualRange(const key_type& key) const {
  return rb_tree_.EqualRange(key);
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::iterator
Multiset<Key_, Compare_, Alloc_>::LowerBound(const key_type& key) {
  return rb_tree_.LowerBound(key);
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::iterator
Multiset<Key_, Compare_, Alloc_>::UpperBound(const key_type& key) {
  return rb_tree_.UpperBound(key);
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::const_iterator
Multiset<Key_, Compare_, Alloc_>::LowerBound(const key_type& key) const {
  return rb_tree_.LowerBound(key);
}

template <typename Key_, typename Compare_, typename Alloc_>
Multiset<Key_, Compare_, Alloc_>::const_iterator
Multiset<Key_, Compare_, Alloc_>::UpperBound(const key_type& key) const {
  return rb_tree_.UpperBound(key);
}

} //  namespace s21

#endif //  _S21_MULTISET_H_
