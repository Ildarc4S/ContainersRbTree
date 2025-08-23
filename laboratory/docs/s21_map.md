```cpp
template <typename Key_, typename Tp_, 
          typename Compare_ =  std::less<_Key>,
          typename Alloc_ = std::allocator<
                              std::pair<const _Key, _Tp>>>
class map {
  typedef _Key					key_type;
  typedef _Tp					mapped_type;
  typedef std::pair<const _Key, _Tp>		value_type;

  typedef typename _Alloc_traits::reference		 reference;
  typedef typename _Alloc_traits::const_reference	 const_reference;
  typedef typename _Rep_type::iterator		 iterator;
  typedef typename _Rep_type::const_iterator	 const_iterator;
  typedef typename _Rep_type::size_type		 size_type;

  map();
  map(std::initializer_list<value_type> const &items);
  map(const map &m);
  map(map &&m);
  ~map();
  operator=(map &&m);

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
```
## operator=

```cpp
template<typename _Key, typename _Val, typename _KeyOfValue,
         typename _Compare, typename _Alloc>
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>&
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
operator=(const _Rb_tree& __x) {
  if (this != std::__addressof(__x)) {
    if (_Node_alloc_traits::_S_propagate_on_copy_assign()) {
      auto& __this_alloc = this->_M_get_Node_allocator();
      auto& __that_alloc = __x._M_get_Node_allocator();
      if (!_Node_alloc_traits::_S_always_equal()
          && __this_alloc != __that_alloc) {
        // Replacement allocator cannot free existing storage, we need
        // to erase nodes first.
        clear();
        std::__alloc_on_copy(__this_alloc, __that_alloc);
      }
    }

    _Reuse_or_alloc_node __roan(*this);
    _M_impl._M_reset();
    _M_impl._M_key_compare = __x._M_impl._M_key_compare;
    if (__x._M_root()) {
      _M_root() = _M_copy<__as_lvalue>(__x, __roan);
    }
  }

  return *this;
}

template<typename _Key, typename _Val, typename _KeyOfValue,
         typename _Compare, typename _Alloc>
inline _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>&
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
operator=(_Rb_tree&& __x)
  noexcept(_Node_alloc_traits::_S_nothrow_move()
           && is_nothrow_move_assignable<_Compare>::value) {
  _M_impl._M_key_compare = std::move(__x._M_impl._M_key_compare);
  _M_move_assign(__x,
                 __bool_constant<_Node_alloc_traits::_S_nothrow_move()>());
  return *this;
}
```

## at()
```cpp
             ----- at() ----
            /       |       \  
key_compare       iterator   iterator           
key_comp() const; end();     lower_bound(const key_type& __x);
      |              |             |                           \
_Compare           iterator  iterator                           _Base_ptr
key_comp() const;  end();    lower_bound(const key_type& __k);  _M_lower_bound_tr(const _Kt& __k) const;
      |              |             |                                        |
IMPL:             _M_end();  _Base_ptr                                      |
_Key_compare                 _M_lower_bound(_Base_ptr __x,      <-- те же зависимости что и у него          
  _M_key_compare;                          _Base_ptr __y,
		                                       const _Key& __k) const;
                            /        /          |             \                                      
                          /         /           |               \
          static const _Key&      _M_begin();  IMPL:               static _Base_ptr
          _S_key(_Node_ptr __x);  _M_end();    _Key_compare        _S_right(_Base_ptr __x);
                                                 _M_key_compare;   _S_left(_Base_ptr __x);
```

## operator[]
```cpp
            |-- key_compare key_comp() const;
            |
operator[] -|-- iterator end();
            |
            |-- iterator lower_bound(const key_type& __x);
            |
            |-- template<typename... _Args>
	              iterator
	              _M_emplace_hint_unique(const_iterator __pos, _Args&&... __args);
                  |
                  |-- pair<_Base_ptr, _Base_ptr>
                      _M_get_insert_hint_unique_pos(const_iterator __pos,
                                                    const key_type& __k);
                        |
                        |-- pair<_Base_ptr, _Base_ptr>
                        |   _M_get_insert_unique_pos(const key_type& __k);
                        |     |
                        |     |-- _M_begin(); _M_end();
                        |     |
                        |     |-- _M_key_compare();
                        |     |
                        |     |-- _S_left(), _S_right();
                        |     |
                        |     |-- begin();
                        |
                        |-- IMPL: _Key_compare _M_key_compare;
                        |
                        |-- static const _Key&
                        |  _S_key(_Node_ptr __x);
                        |
                        |-- _Base_ptr&
                        |   _M_leftmost();
                        |   _M_rightmost();
                        |
                        |-- static _Base_ptr
                        |   _S_right(_Base_ptr __x);
```

## begin() и end()
```cpp
begin() -|-- Rb_tree::begin();
end()   -|-- Rb_tree::end();
```

## empty()
```cpp
empty() -|-- Rb_tree::empty();
```

## size()
```cpp
size() -|-- Rb_tree::size();
```

## max_size()
```cpp
max_size() -|-- Rb_tree::max_size();

```

## clear()
```cpp
clear() -|-- Rb_tree::clear()
              |
              |-- void _M_erase(_Node_ptr __x);
              |
              |-- Header: void _M_reset();
```

## insert(const value_type& value)
```cpp
insert() -|-- template<typename _Arg>
              pair<iterator, bool>
              _M_insert_unique(_Arg&& __x);
              |
              |-- pair<_Base_ptr, _Base_ptr>
              |   _M_get_insert_unique_pos(const key_type& __k);
              |     |
              |     |-- _M_begin(); _M_end();
              |     |
              |     |-- _M_key_compare();
              |     |
              |     |-- _S_left(), _S_right();
              |     |
              |     |-- begin();
              | 
              |--	iterator
	                _M_insert_(_Base_ptr __x, _Base_ptr __y,
                              _Arg&& __v, _NodeGen&);
                    |
                    |-- Header: _M_base_ptr();
                    |
                    |-- _M_key_compare();
                    |
                    |-- static const _Key&
                    |  _S_key(_Node_ptr __x);
                    |
                    |-- _M_end();
                    |
                    |-- Node_traits: _S_insert_and_rebalance();
```

## insert(const Key& key, const T& obj)
```cpp
Пока неизвестно
```

## insert_or_assign(const Key& key, const T& obj)
```cpp
insert_or_assign() -|-- iterator lower_bound(const key_type& __x);
                    |
                    |-- end();
                    |
                    |-- key_compare key_comp() const;
                    |
                    |-- template<typename... _Args>
                        iterator
                        emplace_hint(const_iterator __pos, _Args&&... __args);
                         |
                         |-- так же как в operator[]
```

## erase(iterator pos)
```cpp
erase() -|-- iterator end();
         |  
         |-- void
         |   _M_erase_aux(const_iterator __position);
              |
              |-- Node_traits: _S_rebalance_for_erase();
              |
              |-- _M_node_ptr();
              |
              |-- void
                  _M_drop_node(_Node_ptr __p);
                   |
                   |-- void
                   |   _M_destroy_node(_Node_ptr __p);
                   |    |
                   |    |-- _Node_allocator&
                   |    |   _M_get_Node_allocator();
                   |    |
                   |    |-- _M_valptr(); 
                   |  
                   |-- void
                       _M_put_node(_Node_ptr __p);
                        |
                        |-- _Node_allocator&
                        |   _M_get_Node_allocator();
```

## swap(map& other)
```cpp
swap() -|-- void swap(_Rb_tree& __t);
             |
             |-- _Base_ptr& _M_root();
             |
             |-- Impl: _M_move_data();
             |
             |-- _Base_ptr&
             |   _M_leftmost();
             |   _M_rightmost();
             |
             |-- _M_end();s
```

## merge(map& other)
```cpp
merge() -|-- struct std::_Rb_tree_merge_helper;
         |
         |-- static auto&
         |   _S_get_tree(_GLIBCXX_STD_C::map<_Key, _Val, _Cmp2, _Alloc>& __map);
         |
         |--	void
	            _M_merge_unique(_Compatible_tree<_Compare2>& __src);
               |
               |-- iterator
               |   begin(); end();
               |
               |-- pair<_Base_ptr, _Base_ptr>
               |   _M_get_insert_unique_pos(const key_type& __k);
               |     |
               |     |-- _M_begin(); _M_end();
               |     |
               |     |-- _M_key_compare();
               |     |
               |     |-- _S_left(), _S_right();
               |     |
               |     |-- begin();
               |
               |-- _M_node_ptr();
               |
               |-- Node_traits: _S_rebalance_for_erase();
               |
               |-- static auto&
               |   _S_get_impl(_Rb_tree<_Key, _Val, _Sel, _Cmp2, _Alloc>& __tree);
               |
               |-- iterator
                   _M_insert_node(_Base_ptr __x, _Base_ptr __y, _Node_ptr __z);
                    |
                    |-- _M_end();
                    |
                    |-- _M_key_compare();
                    |
                    |-- _S_key();
                    |
                    |-- _M_base_ptr();
                    |
                    |-- Node_traits: _S_insert_and_rebalance();
```

## contains(const Key& key)
```cpp
contains() -|-- iterator
            |   _M_find_tr(const _Kt& __k);
            |    |
            |    |-- const_iterator
            |    |   _M_find_tr(const _Kt& __k) const
            |         |
            |         |-- _Base_ptr
            |         |    _M_lower_bound_tr(const _Kt& __k) const;
            |         |
            |         |-- end();
            |         |
            |         |-- _M_key_compare();
            |         |
            |         |-- static const _Key& 
            |         |   _S_key(_Node_ptr __x);
            |
            |-- end();
```