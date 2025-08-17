```cpp
 template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    inline void
    swap(_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>& __x,
	 _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>& __y)
    { __x.swap(__y); }

#if __cplusplus >= 201103L
  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    void
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_move_data(_Rb_tree& __x, false_type)
    {
      if (_M_get_Node_allocator() == __x._M_get_Node_allocator())
	_M_move_data(__x, true_type());
      else
	{
	  constexpr bool __move = !__move_if_noexcept_cond<value_type>::value;
	  _Alloc_node __an(*this);
	  _M_root() = _M_copy<__move>(__x, __an);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++17-extensions" // if constexpr
	  if constexpr (__move)
	    __x.clear();
#pragma GCC diagnostic pop
	}
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    inline void
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_move_assign(_Rb_tree& __x, true_type)
    {
      clear();
      if (__x._M_root())
	_M_move_data(__x, true_type());
      std::__alloc_on_move(_M_get_Node_allocator(),
			   __x._M_get_Node_allocator());
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    void
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_move_assign(_Rb_tree& __x, false_type)
    {
      if (_M_get_Node_allocator() == __x._M_get_Node_allocator())
	return _M_move_assign(__x, true_type{});

      // Try to move each node reusing existing nodes and copying __x nodes
      // structure.
      _Reuse_or_alloc_node __roan(*this);
      _M_impl._M_reset();
      if (__x._M_root())
	{
	  _M_root() = _M_copy<__as_rvalue>(__x, __roan);
	  __x.clear();
	}
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    inline _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>&
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    operator=(_Rb_tree&& __x)
    noexcept(_Node_alloc_traits::_S_nothrow_move()
	     && is_nothrow_move_assignable<_Compare>::value)
    {
      _M_impl._M_key_compare = std::move(__x._M_impl._M_key_compare);
      _M_move_assign(__x,
		     __bool_constant<_Node_alloc_traits::_S_nothrow_move()>());
      return *this;
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    template<typename _Iterator>
      void
      _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
      _M_assign_unique(_Iterator __first, _Iterator __last)
      {
	_Reuse_or_alloc_node __roan(*this);
	_M_impl._M_reset();
	for (; __first != __last; ++__first)
	  _M_insert_unique_(end(), *__first, __roan);
      }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    template<typename _Iterator>
      void
      _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
      _M_assign_equal(_Iterator __first, _Iterator __last)
      {
	_Reuse_or_alloc_node __roan(*this);
	_M_impl._M_reset();
	for (; __first != __last; ++__first)
	  _M_insert_equal_(end(), *__first, __roan);
      }
#endif

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>&
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    operator=(const _Rb_tree& __x)
    {
      if (this != std::__addressof(__x))
	{
	  // Note that _Key may be a constant type.
#if __cplusplus >= 201103L
	  if (_Node_alloc_traits::_S_propagate_on_copy_assign())
	    {
	      auto& __this_alloc = this->_M_get_Node_allocator();
	      auto& __that_alloc = __x._M_get_Node_allocator();
	      if (!_Node_alloc_traits::_S_always_equal()
		  && __this_alloc != __that_alloc)
		{
		  // Replacement allocator cannot free existing storage, we need
		  // to erase nodes first.
		  clear();
		  std::__alloc_on_copy(__this_alloc, __that_alloc);
		}
	    }
#endif

	  _Reuse_or_alloc_node __roan(*this);
	  _M_impl._M_reset();
	  _M_impl._M_key_compare = __x._M_impl._M_key_compare;
	  if (__x._M_root())
	    _M_root() = _M_copy<__as_lvalue>(__x, __roan);
	}

      return *this;
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
#if __cplusplus >= 201103L
    template<typename _Arg, typename _NodeGen>
#else
    template<typename _NodeGen>
#endif
      typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
      _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
      _M_insert_(_Base_ptr __x, _Base_ptr __p,
#if __cplusplus >= 201103L
		 _Arg&& __v,
#else
		 const _Val& __v,
#endif
		 _NodeGen& __node_gen)
      {
	bool __insert_left = (__x || __p == _M_end()
			      || _M_key_compare(_KeyOfValue()(__v),
						_S_key(__p)));

	_Base_ptr __z =
	  __node_gen(_GLIBCXX_FORWARD(_Arg, __v))->_M_base_ptr();

	_Node_traits::_S_insert_and_rebalance
	  (__insert_left, __z, __p, this->_M_impl._M_header);
	++_M_impl._M_node_count;
	return iterator(__z);
      }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
#if __cplusplus >= 201103L
    template<typename _Arg>
#endif
    typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
#if __cplusplus >= 201103L
    _M_insert_lower(_Base_ptr __p, _Arg&& __v)
#else
    _M_insert_lower(_Base_ptr __p, const _Val& __v)
#endif
    {
      bool __insert_left = (__p == _M_end()
			    || !_M_key_compare(_S_key(__p),
					       _KeyOfValue()(__v)));

      _Base_ptr __z =
	_M_create_node(_GLIBCXX_FORWARD(_Arg, __v))->_M_base_ptr();
      _Node_traits::_S_insert_and_rebalance
	(__insert_left, __z, __p, this->_M_impl._M_header);
      ++_M_impl._M_node_count;
      return iterator(__z);
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
#if __cplusplus >= 201103L
    template<typename _Arg>
#endif
    typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
#if __cplusplus >= 201103L
    _M_insert_equal_lower(_Arg&& __v)
#else
    _M_insert_equal_lower(const _Val& __v)
#endif
    {
      _Base_ptr __x = _M_begin();
      _Base_ptr __y = _M_end();
      while (__x)
	{
	  __y = __x;
	  __x = !_M_key_compare(_S_key(__x), _KeyOfValue()(__v)) ?
		_S_left(__x) : _S_right(__x);
	}
      return _M_insert_lower(__y, _GLIBCXX_FORWARD(_Arg, __v));
    }

  template<typename _Key, typename _Val, typename _KoV,
	   typename _Compare, typename _Alloc>
    template<bool _MoveValues, typename _NodeGen>
      typename _Rb_tree<_Key, _Val, _KoV, _Compare, _Alloc>::_Base_ptr
      _Rb_tree<_Key, _Val, _KoV, _Compare, _Alloc>::
      _M_copy(_Node_ptr __x, _Base_ptr __p, _NodeGen& __node_gen)
      {
	// Structural copy. __x and __p must be non-null.
	_Node_ptr __top = _M_clone_node<_MoveValues>(__x, __node_gen);
	_Base_ptr __top_base = __top->_M_base_ptr();
	__top->_M_parent = __p;

	__try
	  {
	    if (__x->_M_right)
	      __top->_M_right =
		_M_copy<_MoveValues>(_S_right(__x), __top_base, __node_gen);
	    __p = __top_base;
	    __x = _S_left(__x);

	    while (__x)
	      {
		_Base_ptr __y =
		  _M_clone_node<_MoveValues>(__x, __node_gen)->_M_base_ptr();
		__p->_M_left = __y;
		__y->_M_parent = __p;
		if (__x->_M_right)
		  __y->_M_right = _M_copy<_MoveValues>(_S_right(__x),
						       __y, __node_gen);
		__p = __y;
		__x = _S_left(__x);
	      }
	  }
	__catch(...)
	  {
	    _M_erase(__top);
	    __throw_exception_again;
	  }
	return __top_base;
      }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    void
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_erase(_Node_ptr __x)
    {
      // Erase without rebalancing.
      while (__x)
	{
	  _M_erase(_S_right(__x));
	  _Node_ptr __y = _S_left(__x);
	  _M_drop_node(__x);
	  __x = __y;
	}
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    typename _Rb_tree<_Key, _Val, _KeyOfValue,
		      _Compare, _Alloc>::_Base_ptr
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_lower_bound(_Base_ptr __x, _Base_ptr __y,
		   const _Key& __k) const
    {
      while (__x)
	if (!_M_key_compare(_S_key(__x), __k))
	  __y = __x, __x = _S_left(__x);
	else
	  __x = _S_right(__x);
      return __y;
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    typename _Rb_tree<_Key, _Val, _KeyOfValue,
		      _Compare, _Alloc>::_Base_ptr
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_upper_bound(_Base_ptr __x, _Base_ptr __y,
		   const _Key& __k) const
    {
      while (__x)
	if (_M_key_compare(__k, _S_key(__x)))
	  __y = __x, __x = _S_left(__x);
	else
	  __x = _S_right(__x);
      return __y;
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::iterator,
	 typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::iterator>
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    equal_range(const _Key& __k)
    {
      typedef pair<iterator, iterator> _Ret;

      _Base_ptr __x = _M_begin();
      _Base_ptr __y = _M_end();
      while (__x)
	{
	  if (_M_key_compare(_S_key(__x), __k))
	    __x = _S_right(__x);
	  else if (_M_key_compare(__k, _S_key(__x)))
	    __y = __x, __x = _S_left(__x);
	  else
	    {
	      _Base_ptr __xu(__x);
	      _Base_ptr __yu(__y);
	      __y = __x, __x = _S_left(__x);
	      __xu = _S_right(__xu);
	      return _Ret(iterator(_M_lower_bound(__x, __y, __k)),
			  iterator(_M_upper_bound(__xu, __yu, __k)));
	    }
	}
      return _Ret(iterator(__y), iterator(__y));
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::const_iterator,
	 typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::const_iterator>
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    equal_range(const _Key& __k) const
    {
      typedef pair<const_iterator, const_iterator> _Ret;

      _Base_ptr __x = _M_begin();
      _Base_ptr __y = _M_end();
      while (__x)
	{
	  if (_M_key_compare(_S_key(__x), __k))
	    __x = _S_right(__x);
	  else if (_M_key_compare(__k, _S_key(__x)))
	    __y = __x, __x = _S_left(__x);
	  else
	    {
	      _Base_ptr __xu(__x);
	      _Base_ptr __yu(__y);
	      __y = __x, __x = _S_left(__x);
	      __xu = _S_right(__xu);
	      return _Ret(const_iterator(_M_lower_bound(__x, __y, __k)),
			  const_iterator(_M_upper_bound(__xu, __yu, __k)));
	    }
	}
      return _Ret(const_iterator(__y), const_iterator(__y));
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    void
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    swap(_Rb_tree& __t)
    _GLIBCXX_NOEXCEPT_IF(__is_nothrow_swappable<_Compare>::value)
    {
      if (!_M_root())
	{
	  if (__t._M_root())
	    _M_impl._M_move_data(__t._M_impl);
	}
      else if (!__t._M_root())
	__t._M_impl._M_move_data(_M_impl);
      else
	{
	  std::swap(_M_root(),__t._M_root());
	  std::swap(_M_leftmost(),__t._M_leftmost());
	  std::swap(_M_rightmost(),__t._M_rightmost());

	  _M_root()->_M_parent = _M_end();
	  __t._M_root()->_M_parent = __t._M_end();
	  std::swap(this->_M_impl._M_node_count, __t._M_impl._M_node_count);
	}
      // No need to swap header's color as it does not change.

      using std::swap;
      swap(this->_M_impl._M_key_compare, __t._M_impl._M_key_compare);

      _Node_alloc_traits::_S_on_swap(_M_get_Node_allocator(),
				     __t._M_get_Node_allocator());
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::_Base_ptr,
	 typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::_Base_ptr>
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_get_insert_unique_pos(const key_type& __k)
    {
      typedef pair<_Base_ptr, _Base_ptr> _Res;
      _Base_ptr __x = _M_begin();
      _Base_ptr __y = _M_end();
      bool __comp = true;
      while (__x)
	{
	  __y = __x;
	  __comp = _M_key_compare(__k, _S_key(__x));
	  __x = __comp ? _S_left(__x) : _S_right(__x);
	}
      iterator __j = iterator(__y);
      if (__comp)
	{
	  if (__j == begin())
	    return _Res(__x, __y);
	  else
	    --__j;
	}
      if (_M_key_compare(_S_key(__j._M_node), __k))
	return _Res(__x, __y);
      return _Res(__j._M_node, _Base_ptr());
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::_Base_ptr,
	 typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::_Base_ptr>
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_get_insert_equal_pos(const key_type& __k)
    {
      typedef pair<_Base_ptr, _Base_ptr> _Res;
      _Base_ptr __x = _M_begin();
      _Base_ptr __y = _M_end();
      while (__x)
	{
	  __y = __x;
	  __x = _M_key_compare(__k, _S_key(__x)) ? _S_left(__x) : _S_right(__x);
	}
      return _Res(__x, __y);
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
#if __cplusplus >= 201103L
    template<typename _Arg>
#endif
    pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::iterator, bool>
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
#if __cplusplus >= 201103L
    _M_insert_unique(_Arg&& __v)
#else
    _M_insert_unique(const _Val& __v)
#endif
    {
      typedef pair<iterator, bool> _Res;
      pair<_Base_ptr, _Base_ptr> __res
	= _M_get_insert_unique_pos(_KeyOfValue()(__v));

      if (__res.second)
	{
	  _Alloc_node __an(*this);
	  return _Res(_M_insert_(__res.first, __res.second,
				 _GLIBCXX_FORWARD(_Arg, __v), __an),
		      true);
	}

      return _Res(iterator(__res.first), false);
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
#if __cplusplus >= 201103L
    template<typename _Arg>
#endif
    typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
#if __cplusplus >= 201103L
    _M_insert_equal(_Arg&& __v)
#else
    _M_insert_equal(const _Val& __v)
#endif
    {
      pair<_Base_ptr, _Base_ptr> __res
	= _M_get_insert_equal_pos(_KeyOfValue()(__v));
      _Alloc_node __an(*this);
      return _M_insert_(__res.first, __res.second,
			_GLIBCXX_FORWARD(_Arg, __v), __an);
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::_Base_ptr,
	 typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::_Base_ptr>
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_get_insert_hint_unique_pos(const_iterator __position,
				  const key_type& __k)
    {
      typedef pair<_Base_ptr, _Base_ptr> _Res;

      // end()
      if (__position._M_node == _M_end())
	{
	  if (size() > 0 && _M_key_compare(_S_key(_M_rightmost()), __k))
	    return _Res(_Base_ptr(), _M_rightmost());
	  else
	    return _M_get_insert_unique_pos(__k);
	}
      else if (_M_key_compare(__k, _S_key(__position._M_node)))
	{
	  // First, try before...
	  iterator __before(__position._M_node);
	  if (__position._M_node == _M_leftmost()) // begin()
	    return _Res(_M_leftmost(), _M_leftmost());
	  else if (_M_key_compare(_S_key((--__before)._M_node), __k))
	    {
	      if (!_S_right(__before._M_node))
		return _Res(_Base_ptr(), __before._M_node);
	      else
		return _Res(__position._M_node, __position._M_node);
	    }
	  else
	    return _M_get_insert_unique_pos(__k);
	}
      else if (_M_key_compare(_S_key(__position._M_node), __k))
	{
	  // ... then try after.
	  iterator __after(__position._M_node);
	  if (__position._M_node == _M_rightmost())
	    return _Res(_Base_ptr(), _M_rightmost());
	  else if (_M_key_compare(__k, _S_key((++__after)._M_node)))
	    {
	      if (!_S_right(__position._M_node))
		return _Res(_Base_ptr(), __position._M_node);
	      else
		return _Res(__after._M_node, __after._M_node);
	    }
	  else
	    return _M_get_insert_unique_pos(__k);
	}
      else
	// Equivalent keys.
	return _Res(__position._M_node, _Base_ptr());
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
#if __cplusplus >= 201103L
    template<typename _Arg, typename _NodeGen>
#else
    template<typename _NodeGen>
#endif
      typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
      _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
      _M_insert_unique_(const_iterator __position,
#if __cplusplus >= 201103L
			_Arg&& __v,
#else
			const _Val& __v,
#endif
			_NodeGen& __node_gen)
    {
      pair<_Base_ptr, _Base_ptr> __res
	= _M_get_insert_hint_unique_pos(__position, _KeyOfValue()(__v));

      if (__res.second)
	return _M_insert_(__res.first, __res.second,
			  _GLIBCXX_FORWARD(_Arg, __v),
			  __node_gen);
      return iterator(__res.first);
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::_Base_ptr,
	 typename _Rb_tree<_Key, _Val, _KeyOfValue,
			   _Compare, _Alloc>::_Base_ptr>
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_get_insert_hint_equal_pos(const_iterator __position, const key_type& __k)
    {
      typedef pair<_Base_ptr, _Base_ptr> _Res;

      // end()
      if (__position._M_node == _M_end())
	{
	  if (size() > 0
	      && !_M_key_compare(__k, _S_key(_M_rightmost())))
	    return _Res(_Base_ptr(), _M_rightmost());
	  else
	    return _M_get_insert_equal_pos(__k);
	}
      else if (!_M_key_compare(_S_key(__position._M_node), __k))
	{
	  // First, try before...
	  iterator __before(__position._M_node);
	  if (__position._M_node == _M_leftmost()) // begin()
	    return _Res(_M_leftmost(), _M_leftmost());
	  else if (!_M_key_compare(__k, _S_key((--__before)._M_node)))
	    {
	      if (!_S_right(__before._M_node))
		return _Res(_Base_ptr(), __before._M_node);
	      else
		return _Res(__position._M_node, __position._M_node);
	    }
	  else
	    return _M_get_insert_equal_pos(__k);
	}
      else
	{
	  // ... then try after.
	  iterator __after(__position._M_node);
	  if (__position._M_node == _M_rightmost())
	    return _Res(_Base_ptr(), _M_rightmost());
	  else if (!_M_key_compare(_S_key((++__after)._M_node), __k))
	    {
	      if (!_S_right(__position._M_node))
		return _Res(_Base_ptr(), __position._M_node);
	      else
		return _Res(__after._M_node, __after._M_node);
	    }
	  else
	    return _Res(_Base_ptr(), _Base_ptr());
	}
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
#if __cplusplus >= 201103L
    template<typename _Arg, typename _NodeGen>
#else
    template<typename _NodeGen>
#endif
      typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator
      _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
      _M_insert_equal_(const_iterator __position,
#if __cplusplus >= 201103L
		       _Arg&& __v,
#else
		       const _Val& __v,
#endif
		       _NodeGen& __node_gen)
      {
	pair<_Base_ptr, _Base_ptr> __res
	  = _M_get_insert_hint_equal_pos(__position, _KeyOfValue()(__v));

	if (__res.second)
	  return _M_insert_(__res.first, __res.second,
			    _GLIBCXX_FORWARD(_Arg, __v),
			    __node_gen);

	return _M_insert_equal_lower(_GLIBCXX_FORWARD(_Arg, __v));
      }

#if __cplusplus >= 201103L
  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    auto
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_node(_Base_ptr __x, _Base_ptr __p, _Node_ptr __z)
    -> iterator
    {
      bool __insert_left = (__x || __p == _M_end()
			    || _M_key_compare(_S_key(__z), _S_key(__p)));

      _Base_ptr __base_z = __z->_M_base_ptr();
      _Node_traits::_S_insert_and_rebalance
	(__insert_left, __base_z, __p, this->_M_impl._M_header);
      ++_M_impl._M_node_count;
      return iterator(__base_z);
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    auto
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_lower_node(_Base_ptr __p, _Node_ptr __z)
    -> iterator
    {
      bool __insert_left = (__p == _M_end()
			    || !_M_key_compare(_S_key(__p), _S_key(__z)));

      _Base_ptr __base_z = __z->_M_base_ptr();
      _Node_traits::_S_insert_and_rebalance
	(__insert_left, __base_z, __p, this->_M_impl._M_header);
      ++_M_impl._M_node_count;
      return iterator(__base_z);
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    auto
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_insert_equal_lower_node(_Node_ptr __z)
    -> iterator
    {
      _Base_ptr __x = _M_begin();
      _Base_ptr __y = _M_end();
      while (__x)
	{
	  __y = __x;
	  __x = !_M_key_compare(_S_key(__x), _S_key(__z)) ?
		_S_left(__x) : _S_right(__x);
	}
      return _M_insert_lower_node(__y, __z);
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    template<typename... _Args>
      auto
      _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
      _M_emplace_unique(_Args&&... __args)
      -> pair<iterator, bool>
      {
	_Auto_node __z(*this, std::forward<_Args>(__args)...);
	auto __res = _M_get_insert_unique_pos(__z._M_key());
	if (__res.second)
	  return {__z._M_insert(__res), true};
	return {iterator(__res.first), false};
      }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    template<typename... _Args>
      auto
      _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
      _M_emplace_equal(_Args&&... __args)
      -> iterator
      {
	_Auto_node __z(*this, std::forward<_Args>(__args)...);
	auto __res = _M_get_insert_equal_pos(__z._M_key());
	return __z._M_insert(__res);
      }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    template<typename... _Args>
      auto
      _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
      _M_emplace_hint_unique(const_iterator __pos, _Args&&... __args)
      -> iterator
      {
	_Auto_node __z(*this, std::forward<_Args>(__args)...);
	auto __res = _M_get_insert_hint_unique_pos(__pos, __z._M_key());
	if (__res.second)
	  return __z._M_insert(__res);
	return iterator(__res.first);
      }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    template<typename... _Args>
      auto
      _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
      _M_emplace_hint_equal(const_iterator __pos, _Args&&... __args)
      -> iterator
      {
	_Auto_node __z(*this, std::forward<_Args>(__args)...);
	auto __res = _M_get_insert_hint_equal_pos(__pos, __z._M_key());
	if (__res.second)
	  return __z._M_insert(__res);
	return __z._M_insert_equal_lower();
      }
#endif


  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    void
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_erase_aux(const_iterator __position)
    {
      _Base_ptr __y = _Node_traits::_S_rebalance_for_erase
	(__position._M_node, this->_M_impl._M_header);
      _M_drop_node(static_cast<_Node&>(*__y)._M_node_ptr());
      --_M_impl._M_node_count;
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    void
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_erase_aux(const_iterator __first, const_iterator __last)
    {
      if (__first == begin() && __last == end())
	clear();
      else
	while (__first != __last)
	  _M_erase_aux(__first++);
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::size_type
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    erase(const _Key& __x)
    {
      pair<iterator, iterator> __p = equal_range(__x);
      const size_type __old_size = size();
      _M_erase_aux(__p.first, __p.second);
      return __old_size - size();
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::size_type
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    _M_erase_unique(const _Key& __x)
    {
      iterator __it = find(__x);
      if (__it == end())
	return 0;

      _M_erase_aux(__it);
      return 1;
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    typename _Rb_tree<_Key, _Val, _KeyOfValue,
		      _Compare, _Alloc>::iterator
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    find(const _Key& __k)
    {
      iterator __j(_M_lower_bound(_M_begin(), _M_end(), __k));
      return (__j == end()
	      || _M_key_compare(__k, _S_key(__j._M_node))) ? end() : __j;
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    typename _Rb_tree<_Key, _Val, _KeyOfValue,
		      _Compare, _Alloc>::const_iterator
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    find(const _Key& __k) const
    {
      const_iterator __j(_M_lower_bound(_M_begin(), _M_end(), __k));
      return (__j == end()
	      || _M_key_compare(__k, _S_key(__j._M_node))) ? end() : __j;
    }

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::size_type
    _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
    count(const _Key& __k) const
    {
      pair<const_iterator, const_iterator> __p = equal_range(__k);
      const size_type __n = std::distance(__p.first, __p.second);
      return __n;
    }

  _GLIBCXX_PURE unsigned int
  _Rb_tree_black_count(const _Rb_tree_node_base* __node,
		       const _Rb_tree_node_base* __root) throw ();

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    bool
    _Rb_tree<_Key,_Val,_KeyOfValue,_Compare,_Alloc>::__rb_verify() const
    {
      if (_M_impl._M_node_count == 0 || begin() == end())
	return _M_impl._M_node_count == 0 && begin() == end()
	       && this->_M_impl._M_header._M_left == _M_end()
	       && this->_M_impl._M_header._M_right == _M_end();

      unsigned int __len = _Rb_tree_black_count(_M_leftmost(), _M_root());
      for (const_iterator __it = begin(); __it != end(); ++__it)
	{
	  _Base_ptr __x = __it._M_node;
	  _Base_ptr __L = _S_left(__x);
	  _Base_ptr __R = _S_right(__x);

	  if (__x->_M_color == _S_red)
	    if ((__L && __L->_M_color == _S_red)
		|| (__R && __R->_M_color == _S_red))
	      return false;

	  if (__L && _M_key_compare(_S_key(__x), _S_key(__L)))
	    return false;
	  if (__R && _M_key_compare(_S_key(__R), _S_key(__x)))
	    return false;

	  if (!__L && !__R && _Rb_tree_black_count(__x, _M_root()) != __len)
	    return false;
	}

      if (_M_leftmost() != _Node_base::_S_minimum(_M_root()))
	return false;
      if (_M_rightmost() != _Node_base::_S_maximum(_M_root()))
	return false;
      return true;
    }

#ifdef __glibcxx_node_extract // >= C++17
  // Allow access to internals of compatible _Rb_tree specializations.
  template<typename _Key, typename _Val, typename _Sel, typename _Cmp1,
	   typename _Alloc, typename _Cmp2>
    struct _Rb_tree_merge_helper<_Rb_tree<_Key, _Val, _Sel, _Cmp1, _Alloc>,
				 _Cmp2>
    {
    private:
      friend class _Rb_tree<_Key, _Val, _Sel, _Cmp1, _Alloc>;

      static auto&
      _S_get_impl(_Rb_tree<_Key, _Val, _Sel, _Cmp2, _Alloc>& __tree)
      { return __tree._M_impl; }
    };
#endif // C++17

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif
```