```cpp
template<typename Key_, typename Compare_ = std::less<Key_>,
         typename Alloc_ = std::allocator<Key_> >
class set {
  typedef Key_     key_type;
  typedef Key_     value_type;

  typedef typename _Alloc_traits::reference		 reference;
  typedef typename _Alloc_traits::const_reference	 const_reference;

  typedef typename _Rep_type::const_iterator	 iterator;
  typedef typename _Rep_type::const_iterator	 const_iterator;
  
  typedef typename _Rep_type::size_type		 size_type;

  set();
  set(std::initializer_list<value_type> const &items);
  set(const set &s);
  set(set &&s);
  ~set();
  operator=(set &&s);

  iterator begin();
  iterator end();

  bool empty();
  size_type size();
  size_type max_size();

  void clear();
  std::pair<iterator, bool> insert(const value_type& value);
  void erase(iterator pos);
  void swap(set& other);
  void merge(set& other);

  iterator find(const Key& key);
  bool contains(const Key& key);
};
```