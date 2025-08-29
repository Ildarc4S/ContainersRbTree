#ifndef _S21_MAP_H_
#define _S21_MAP_H_

#include <functional>
#include <memory>

#include "rb_tree/s21_rb_tree.h"

namespace s21 {

template <typename Key_, typename T_,
          typename Compare_ =  std::less<Key_>,
          typename Alloc_ = std::allocator<
                              std::pair<const Key_, T_>>>
class Map {
public:
  using key_type = Key_;
  using map_type = T_;
  using key_compare = Compare_;
  using allocator_type = Alloc_;
  using value_type = std::pair<const Key_, T_>;

private:
  using PairAlloc_ = std::allocator_traits<allocator_type>::template rebind_alloc<value_type>;
  using RbTree_ = RbTree<key_type, value_type, rb_tree::SelectFirst<value_type>,
                         key_compare, PairAlloc_>;

  using AllocTraits_ = std::allocator_traits<PairAlloc_>;

  template<typename OtherCompare_>
	using OtherMap_ = Map<Key_, T_, OtherCompare_, Alloc_>;

  template<typename, typename>
	friend struct RbTreeMergeHelper;

  RbTree_ rb_tree_;

public:
  using reference = value_type&;
  using const_reference = const value_type&;

  using iterator = RbTree_::iterator;
  using const_iterator = RbTree_::const_iterator;

  using size_type = RbTree_::size_type;

  Map() = default;
  Map(const Map&) = default;
  Map(Map&&) = default;
  ~Map() = default;

  Map(std::initializer_list<value_type> const &items,
      const key_compare& compare = key_compare(),
      const allocator_type& alloc = allocator_type());

  Map& operator=(Map&) = default;
  Map& operator=(Map&&) = default;

  map_type& At(const key_type& key);
  map_type& operator[](const key_type& key);

  iterator Begin() noexcept;
  iterator End() noexcept;

  const_iterator Begin() const noexcept;
  const_iterator End() const noexcept;

  bool Empty() const noexcept;
  size_type Size() const noexcept;
  size_type MaxSize() const noexcept;

  void Clear();
  std::pair<iterator, bool> Insert(const value_type& pair);
  std::pair<iterator, bool> Insert(const key_type& key, const map_type& value);
  std::pair<iterator, bool> InsertOrAssign(const key_type& key, const map_type& value);

  void Erase(iterator pos);
  void Swap(Map& other);

  template<typename OtherCompare_>
  void Merge(OtherMap_<OtherCompare_>& other);

  bool Contains(const key_type& key) const;
};

template<typename Key_, typename Val_, typename Compare_,
         typename Alloc_, typename OtherCompare_>
struct RbTreeMergeHelper<Map<Key_, Val_, Compare_, Alloc_>, OtherCompare_> {
private:
  friend class Map<Key_, Val_, Compare_, Alloc_>;

  static auto&
  GetRbTree(Map<Key_, Val_, OtherCompare_, Alloc_>& other_map) {
    return other_map.rb_tree_;
  }
};

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
Map<Key_, T_, Compare_, Alloc_>::
Map(std::initializer_list<value_type> const &items,
    const key_compare& compare,
    const allocator_type& alloc)
: rb_tree_(compare, alloc) {
  rb_tree_.InsertRangeUnique(items.begin(), items.end());
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
Map<Key_, T_, Compare_, Alloc_>::map_type&
Map<Key_, T_, Compare_, Alloc_>::At(const key_type& key) {
  auto it = rb_tree_.LowerBound(key);
  if (it == rb_tree_.end() || key_compare()(key, it->first)) {
    throw std::out_of_range("Map::at: key not found");
  }
  return it->second;
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
Map<Key_, T_, Compare_, Alloc_>::map_type&
Map<Key_, T_, Compare_, Alloc_>::operator[](const key_type& key) {
  auto it = rb_tree_.LowerBound(key);
  if (it == rb_tree_.end() || key_compare()(key, it->first)) {
    it = rb_tree_.InsertHintUnique(it, key);
  }
  return it->second;
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
Map<Key_, T_, Compare_, Alloc_>::iterator
Map<Key_, T_, Compare_, Alloc_>::Begin() noexcept {
  return rb_tree_.begin();
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
Map<Key_, T_, Compare_, Alloc_>::iterator
Map<Key_, T_, Compare_, Alloc_>::End() noexcept {
  return rb_tree_.end();
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
Map<Key_, T_, Compare_, Alloc_>::const_iterator
Map<Key_, T_, Compare_, Alloc_>::Begin() const noexcept {
  return rb_tree_.begin();
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
Map<Key_, T_, Compare_, Alloc_>::const_iterator
Map<Key_, T_, Compare_, Alloc_>::End() const noexcept {
  return rb_tree_.end();
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
bool
Map<Key_, T_, Compare_, Alloc_>::Empty() const noexcept {
  return rb_tree_.Empty();
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
Map<Key_, T_, Compare_, Alloc_>::size_type
Map<Key_, T_, Compare_, Alloc_>::Size() const noexcept {
  return rb_tree_.Size();
}


template <typename Key_, typename T_, typename Compare_, typename Alloc_>
Map<Key_, T_, Compare_, Alloc_>::size_type
Map<Key_, T_, Compare_, Alloc_>::MaxSize() const noexcept {
  return rb_tree_.MaxSize();
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
void
Map<Key_, T_, Compare_, Alloc_>::Clear() {
  rb_tree_.Clear();
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
std::pair<typename Map<Key_, T_, Compare_, Alloc_>::iterator, bool>
Map<Key_, T_, Compare_, Alloc_>::Insert(const value_type& pair) {
  return rb_tree_.InsertUnique(pair);
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
std::pair<typename Map<Key_, T_, Compare_, Alloc_>::iterator, bool>
Map<Key_, T_, Compare_, Alloc_>::Insert(const key_type& key, const map_type& value) {
  return rb_tree_.InsertUnique(std::make_pair(key, value));
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
std::pair<typename Map<Key_, T_, Compare_, Alloc_>::iterator, bool>
Map<Key_, T_, Compare_, Alloc_>::InsertOrAssign(const key_type& key, const map_type& value) {
  auto it = rb_tree_.LowerBound(key);
  bool inserted = false;

  if (it == rb_tree_.end() || key_compare()(key, it->first)) {
    auto insert_result = rb_tree_.InsertUnique(std::make_pair(key, value));
    it = insert_result.first;
    inserted = insert_result.second;
  } else {
    it->second = value;
  }

  return {it, inserted};
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
void
Map<Key_, T_, Compare_, Alloc_>::Erase(iterator position) {
  rb_tree_.Erase(position);
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
void
Map<Key_, T_, Compare_, Alloc_>::Swap(Map& other) {
  rb_tree_.Swap(other.rb_tree_);
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
template<typename OtherCompare_>
void
Map<Key_, T_, Compare_, Alloc_>::Merge(OtherMap_<OtherCompare_>& other) {
  rb_tree_.MergeUnique(RbTreeMergeHelper<Map, OtherCompare_>::GetRbTree(other));
}

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
bool
Map<Key_, T_, Compare_, Alloc_>::Contains(const key_type& key) const {
  return rb_tree_.Find(key) != rb_tree_.end();
}

} //  namespace s21
#endif //  _S21_MAP_H_
