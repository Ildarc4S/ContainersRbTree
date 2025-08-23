#ifndef _S21_MAP_H_
#define _S21_MAP_H_

#include <functional>
#include <memory>

#include "s21_rb_tree.h"

namespace s21 {

template <typename Key_, typename T_, 
          typename Compare_ =  std::less<Key_>,
          typename Alloc_ = std::allocator<
                              std::pair<const Key_, T_>>>
class map {
public:
  using key_type = Key_;
  using value_type = std::pair<const Key, T>;
  using reference = Alloc_

private:
  using PairAlloc_ = std::allocator_traits<Alloc_>::template rebind<value_type>;
  using RbTree_ = RbTree<key_type, value_type, _Select1st<value_type>,
                         Compare_, PairAlloc_>;

  using AllocTraits_ = std::allocator_traits<PairAlloc_>;
  
  RbTree_ rb_tree_;

public:
  using reference = AllocTraits_::reference;
  using const_reference = AllocTraits_::const_reference;

  using iterator = RbTree_::iterator;
  using const_iterator = RbTree_::const_iterator;
  using size_type = RbTree_::size_type;

  map() = default;
  map(const map&) = default;
  map(map&&) = default;
  ~map() = default;

  map(std::initializer_list<value_type> const &items);

  map& operator=(map&) = default;
  map& operator=(map&&) = default;

  T& at(const Key& key);
  T& operator[](const Key& key);
  
  iterator begin();
  iterator end();

  bool empty();
  size_type size();
  size_type max_size();

  void clear();
  std::pair<iterator, bool> insert(const value_type& value);
  std::pair<iterator, bool> insert(const Key& key, const T& obj);
  std::pair<iterator, bool> insert_or_assign(const Key& key, const T& obj);

  void erase(iterator pos);
  void swap(map& other);
  void merge(map& other);

  bool contains(const Key& key);
};

template <typename Key_, typename T_, typename Compare_, typename Alloc_>
map<Key_, T_, Compare_, Alloc_>::
map(std::initializer_list<value_type> const &items) 
: rb_tree_() {
  rb_tree_.InsertRangeUnique(items.begin(), items.end());
}



} //  namespace s21
#endif //  _S21_MAP_H_