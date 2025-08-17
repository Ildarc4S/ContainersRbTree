#ifndef _STL_TREE_H
#define _STL_TREE_H 1

#include <bits/stl_algobase.h>
#include <bits/allocator.h>
#include <bits/stl_function.h>
#include <bits/cpp_type_traits.h>
#include <bits/ptr_traits.h>
#include <ext/alloc_traits.h>
#if __cplusplus >= 201103L
# include <ext/aligned_buffer.h>
#endif
#ifdef __glibcxx_node_extract // >= C++17
# include <bits/node_handle.h>
#endif

#if ! defined _GLIBCXX_USE_ALLOC_PTR_FOR_RB_TREE
# define _GLIBCXX_USE_ALLOC_PTR_FOR_RB_TREE 1
#endif

namespace __rb_tree
{
  template<typename _VoidPtr>
  struct _Node_base
  {
    using _Base_ptr = __ptr_rebind<_VoidPtr, _Node_base>;

    _Rb_tree_color	_M_color;
    _Base_ptr		_M_parent;
    _Base_ptr		_M_left;
    _Base_ptr		_M_right;

    static _Base_ptr _S_minimum(_Base_ptr __x);
    static _Base_ptr _S_maximum(_Base_ptr __x);
    _Base_ptr _M_base_ptr() const noexcept;
  };

  template<typename _NodeBase>
  struct _Header
  {
   private:
    using _Base_ptr =  typename _NodeBase::_Base_ptr;

   public:
    _NodeBase		_M_header;
    size_t		_M_node_count; // Keeps track of size of tree.

    _Header() noexcept;
    _Header(_Header&& __x) noexcept;
    void _M_move_data(_Header& __from);
    void _M_reset();
  };

  template<typename _ValPtr>
  struct _Node : public __rb_tree::_Node_base<__ptr_rebind<_ValPtr, void>>
  {
    using value_type = typename pointer_traits<_ValPtr>::element_type;
    using _Node_ptr = __ptr_rebind<_ValPtr, _Node>;

    _Node() noexcept { }
    ~_Node() { }
    _Node(_Node&&) = delete;

    union _Uninit_storage
    {
      _Uninit_storage() noexcept { }
      ~_Uninit_storage() { }

      value_type _M_data;
    };
    _Uninit_storage _M_u;

    value_type* _M_valptr();
    value_type const* _M_valptr() const;

    _Node_ptr _M_node_ptr() noexcept;
  };
} // namespace __rb_tree

  _Rb_tree_node_base* _Rb_tree_increment(_Rb_tree_node_base* __x);
  _Rb_tree_node_base* _Rb_tree_decrement(_Rb_tree_node_base* __x);

  template<typename _Tp>
  struct _Rb_tree_iterator
  {
    typedef _Tp  value_type;
    typedef _Tp& reference;
    typedef _Tp* pointer;

    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t			 difference_type;

    typedef _Rb_tree_node_base::_Base_ptr	_Base_ptr;
    typedef _Rb_tree_node<_Tp>*		_Node_ptr;

    _Rb_tree_iterator();

    explicit _Rb_tree_iterator(_Base_ptr __x);

    reference operator*();
    pointer   operator->() const;

    _Rb_tree_iterator& operator++();
    _Rb_tree_iterator  operator++(int);
    _Rb_tree_iterator& operator--();
    _Rb_tree_iterator  operator--(int);
    friend bool operator==(const _Rb_tree_iterator& __x,
                           const _Rb_tree_iterator& __y);
    _Base_ptr _M_node;
  };

  template<typename _Tp>
  struct _Rb_tree_const_iterator
  {
    typedef _Tp	 value_type;
    typedef const _Tp& reference;
    typedef const _Tp* pointer;

    typedef _Rb_tree_iterator<_Tp> iterator;

    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t			 difference_type;

    typedef _Rb_tree_node_base::_Base_ptr	_Base_ptr;
    typedef const _Rb_tree_node<_Tp>*		_Node_ptr;

    _Rb_tree_const_iterator();
    explicit _Rb_tree_const_iterator(_Base_ptr __x);
    _Rb_tree_const_iterator(const iterator& __it);

    reference operator*() const;
    pointer   operator->() const;

    _Rb_tree_const_iterator& operator++();
    _Rb_tree_const_iterator  operator++(int);
    _Rb_tree_const_iterator& operator--();
    _Rb_tree_const_iterator  operator--(int);

    friend bool operator==(const _Rb_tree_const_iterator& __x,
                           const _Rb_tree_const_iterator& __y);
    _Base_ptr _M_node;
  };

  void
  _Rb_tree_insert_and_rebalance(const bool __insert_left,
				_Rb_tree_node_base* __x,
				_Rb_tree_node_base* __p,
				_Rb_tree_node_base& __header);

  _Rb_tree_node_base*
  _Rb_tree_rebalance_for_erase(_Rb_tree_node_base* const __z,
			       _Rb_tree_node_base& __header);

namespace __rb_tree
{
  template<bool _Const, typename _ValPtr>
    struct _Iterator
    {
      template<typename _Tp>
	using __maybe_const = __conditional_t<_Const, const _Tp, _Tp>;

      using __ptr_traits =	pointer_traits<_ValPtr>;
      using value_type =	typename __ptr_traits::element_type;
      using reference =		__maybe_const<value_type>&;
      using pointer =		__maybe_const<value_type>*;

      using iterator_category =	bidirectional_iterator_tag;
      using difference_type =	ptrdiff_t;

      using _Node = __rb_tree::_Node<_ValPtr>;
      using _Node_base = __rb_tree::_Node_base<__ptr_rebind<_ValPtr, void>>;
      using _Base_ptr =	 typename _Node_base::_Base_ptr;

      _Iterator() noexcept;

      constexpr explicit
      _Iterator(_Base_ptr __x) noexcept;

      _Iterator(const _Iterator&) = default;
      _Iterator& operator=(const _Iterator&) = default;

      constexpr
      _Iterator(const _Iterator<false, _ValPtr>& __it) requires _Const;

      [[nodiscard]]
      reference operator*() const noexcept;

      [[nodiscard]]
      pointer operator->() const noexcept;

      _Iterator& operator++() noexcept;
      _Iterator  operator++(int) noexcept;
      _Iterator& operator--() noexcept;
      _Iterator  operator--(int) noexcept;

      [[nodiscard]]
      friend bool operator==(const _Iterator& __x, const _Iterator& __y);

      _Base_ptr _M_node;
    };

  // Determine the node and iterator types used by std::_Rb_tree.
  template<typename _Val, typename _Ptr>
    struct _Node_traits;

  template<typename _Val, typename _ValPtr>
    struct _Node_traits
    {
      using _Node = __rb_tree::_Node<_ValPtr>;
      using _Node_ptr = __ptr_rebind<_ValPtr, _Node>;
      using _Node_base = __rb_tree::_Node_base<__ptr_rebind<_ValPtr, void>>;
      using _Base_ptr = __ptr_rebind<_ValPtr, _Node_base>;
      using _Header_t = __rb_tree::_Header<_Node_base>;
      using _Iterator = __rb_tree::_Iterator<false, _ValPtr>;
      using _Const_iterator = __rb_tree::_Iterator<true, _ValPtr>;

      static void _Rotate_left(_Base_ptr __x, _Base_ptr& __root);
      static void _Rotate_right(_Base_ptr __x, _Base_ptr& __root);
      static void _S_insert_and_rebalance(const bool __insert_left,
			                                    _Base_ptr __x, _Base_ptr __p,
			                                    _Node_base& __header);
      static _Base_ptr _S_rebalance_for_erase(_Base_ptr __z,
                                              _Node_base& __header);
    };
} // namespace __rb_tree

  template<typename _Tree1, typename _Cmp2>
    struct _Rb_tree_merge_helper { };

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc = allocator<_Val> >
    class _Rb_tree
    {
      typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template
                              rebind<_Val>::other _Val_alloc_type;

      typedef __gnu_cxx::__alloc_traits<_Val_alloc_type> _Val_alloc_traits;
      typedef typename _Val_alloc_traits::pointer _ValPtr;
      typedef __rb_tree::_Node_traits<_Val, _ValPtr> _Node_traits;

      typedef typename _Node_traits::_Node_base		_Node_base;
      typedef typename _Node_traits::_Node		_Node;

      typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template
	rebind<_Node>::other _Node_allocator;

      typedef __gnu_cxx::__alloc_traits<_Node_allocator> _Node_alloc_traits;

    protected:
      typedef typename _Node_traits::_Base_ptr	_Base_ptr;
      typedef typename _Node_traits::_Node_ptr	_Node_ptr;

    private:

      struct _Reuse_or_alloc_node
      {
     	  _Reuse_or_alloc_node(_Rb_tree& __t);
	      _Reuse_or_alloc_node(const _Reuse_or_alloc_node&) = delete;
        ~_Reuse_or_alloc_node();

        template<typename _Arg>
        _Node_ptr operator()(_Arg&& __arg);
       private:
        _Base_ptr _M_extract();
        _Base_ptr _M_root;
        _Base_ptr _M_nodes;
        _Rb_tree& _M_t;
      };

      struct _Alloc_node
      {
	      _Alloc_node(_Rb_tree& __t);

	      template<typename _Arg>
	      _Node_ptr operator()(_Arg&& __arg) const;

      private:
      	_Rb_tree& _M_t;
      };

    public:
      typedef _Key 				key_type;
      typedef _Val 				value_type;
      typedef value_type* 			pointer;
      typedef const value_type* 		const_pointer;
      typedef value_type& 			reference;
      typedef const value_type& 		const_reference;
      typedef size_t 				size_type;
      typedef ptrdiff_t 			difference_type;
      typedef _Alloc 				allocator_type;

      _Node_allocator& _M_get_Node_allocator();
      const _Node_allocator& _M_get_Node_allocator() const;
      allocator_type get_allocator() const;

    protected:
      _Node_ptr _M_get_node();
      void      _M_put_node(_Node_ptr __p);

      template<typename... _Args>
      void _M_construct_node(_Node_ptr __node, _Args&&... __args);

      template<typename... _Args>
      _Node_ptr _M_create_node(_Args&&... __args);

      void _M_destroy_node(_Node_ptr __p);
      void _M_drop_node(_Node_ptr __p);

      template<bool _MoveValue, typename _NodeGen>
	    _Node_ptr _M_clone_node(_Node_ptr __x, _NodeGen& __node_gen);

    protected:
      typedef typename _Node_traits::_Header_t		_Header_t;

      template<typename _Key_compare>
      struct _Rb_tree_impl
        : public _Node_allocator
        , public _Rb_tree_key_compare<_Key_compare>
        , public _Header_t
      {
        typedef _Rb_tree_key_compare<_Key_compare> _Base_key_compare;

        _Rb_tree_impl();
        _Rb_tree_impl(const _Rb_tree_impl& __x);

        _Rb_tree_impl(_Rb_tree_impl&&);
         noexcept( is_nothrow_move_constructible<_Base_key_compare>::value )
        = default;

        explicit _Rb_tree_impl(_Node_allocator&& __a);

        _Rb_tree_impl(_Rb_tree_impl&& __x, _Node_allocator&& __a);
        _Rb_tree_impl(const _Key_compare& __comp, _Node_allocator&& __a);
      };

      _Rb_tree_impl<_Compare> _M_impl;

    protected:
      _Base_ptr& _M_root();
      _Base_ptr _M_root() const;

      _Base_ptr& _M_leftmost();
      _Base_ptr _M_leftmost() const;

      _Base_ptr& _M_rightmost();
      _Base_ptr _M_rightmost() const;

      _Base_ptr _M_begin() const;
      _Node_ptr _M_begin_node() const;
      _Base_ptr _M_end() const;

      template<typename _Key1, typename _Key2>
	    bool _M_key_compare(const _Key1& __k1,
                          const _Key2& __k2) const;

      static const _Key& _S_key(const _Node& __node);
      static const _Key& _S_key(_Base_ptr __x);
      static const _Key& _S_key(_Node_ptr __x);

      static _Base_ptr _S_left(_Base_ptr __x);
      static _Node_ptr _S_left(_Node_ptr __x);

      static _Base_ptr _S_right(_Base_ptr __x);
      static _Node_ptr _S_right(_Node_ptr __x);

    public:
      typedef typename _Node_traits::_Iterator		iterator;
      typedef typename _Node_traits::_Const_iterator	const_iterator;

      typedef std::reverse_iterator<iterator>       reverse_iterator;
      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

      using node_type = _Node_handle<_Key, _Val, _Node_allocator>;
      using insert_return_type = _Node_insert_return<
      	__conditional_t<is_same_v<_Key, _Val>, const_iterator, iterator>,
	      node_type>;

      pair<_Base_ptr, _Base_ptr>
      _M_get_insert_unique_pos(const key_type& __k);

      pair<_Base_ptr, _Base_ptr>
      _M_get_insert_equal_pos(const key_type& __k);

      pair<_Base_ptr, _Base_ptr>
      _M_get_insert_hint_unique_pos(const_iterator __pos,
				    const key_type& __k);

      pair<_Base_ptr, _Base_ptr>
      _M_get_insert_hint_equal_pos(const_iterator __pos,
				   const key_type& __k);

    private:
      template<typename _Arg, typename _NodeGen>
      iterator
      _M_insert_(_Base_ptr __x, _Base_ptr __y, _Arg&& __v, _NodeGen&);

      iterator
      _M_insert_node(_Base_ptr __x, _Base_ptr __y, _Node_ptr __z);

      template<typename _Arg>
      iterator  _M_insert_lower(_Base_ptr __y, _Arg&& __v);

      template<typename _Arg>
	    iterator _M_insert_equal_lower(_Arg&& __x);

      iterator
      _M_insert_lower_node(_Base_ptr __p, _Node_ptr __z);

      iterator
      _M_insert_equal_lower_node(_Node_ptr __z);

      enum { __as_lvalue, __as_rvalue };

      template<bool _MoveValues, typename _NodeGen>
      _Base_ptr _M_copy(_Node_ptr, _Base_ptr, _NodeGen&);

      template<bool _MoveValues, typename _NodeGen>
      _Base_ptr _M_copy(const _Rb_tree& __x, _NodeGen& __gen);

      _Base_ptr _M_copy(const _Rb_tree& __x);

      void _M_erase(_Node_ptr __x);

      _Base_ptr _M_lower_bound(_Base_ptr __x, _Base_ptr __y,
		                           const _Key& __k) const;

      _Base_ptr _M_upper_bound(_Base_ptr __x, _Base_ptr __y,
		                           const _Key& __k) const;
    public:
      // allocation/deallocation
      _Rb_tree() = default;
      _Rb_tree(const _Compare& __comp,
	       const allocator_type& __a = allocator_type());
      _Rb_tree(const _Rb_tree& __x);
      _Rb_tree(const allocator_type& __a);
      _Rb_tree(const _Rb_tree& __x, const allocator_type& __a);

      _Rb_tree(_Rb_tree&&) = default;
      _Rb_tree(_Rb_tree&& __x, const allocator_type& __a);

    private:
      _Rb_tree(_Rb_tree&& __x, _Node_allocator&& __a, true_type)
      noexcept(is_nothrow_default_constructible<_Compare>::value);
      _Rb_tree(_Rb_tree&& __x, _Node_allocator&& __a, false_type);

    public:
      _Rb_tree(_Rb_tree&& __x, _Node_allocator&& __a)
      noexcept( noexcept(
      _Rb_tree(std::declval<_Rb_tree&&>(), std::declval<_Node_allocator&&>(),
		  std::declval<typename _Node_alloc_traits::is_always_equal>())) );

      ~_Rb_tree();

      _Rb_tree& operator=(const _Rb_tree& __x);

      // Accessors.
      _Compare key_comp() const;

      iterator begin();
      const_iterator begin() const;

      iterator end();
      const_iterator end() const;

      reverse_iterator rbegin();
      const_reverse_iterator rbegin() const;

      reverse_iterator rend();
      const_reverse_iterator rend() const;

      bool empty() const;
      size_type size() const;
      size_type max_size() const;
      void swap(_Rb_tree& __t);

      template<typename _Arg>
      pair<iterator, bool> _M_insert_unique(_Arg&& __x);

      template<typename _Arg>
      iterator _M_insert_equal(_Arg&& __x);

      template<typename _Arg, typename _NodeGen>
      iterator _M_insert_unique_(const_iterator __pos,
                                 _Arg&& __x, _NodeGen&);

      template<typename _Arg>
      iterator _M_insert_unique_(const_iterator __pos, _Arg&& __x);

      template<typename _Arg, typename _NodeGen>
      iterator _M_insert_equal_(const_iterator __pos, _Arg&& __x, _NodeGen&);

      template<typename _Arg>
      iterator _M_insert_equal_(const_iterator __pos, _Arg&& __x);

      template<typename... _Args>
      pair<iterator, bool> _M_emplace_unique(_Args&&... __args);

      template<typename... _Args>
      iterator _M_emplace_equal(_Args&&... __args);

      template<typename... _Args>
      iterator _M_emplace_hint_unique(const_iterator __pos,
                                      _Args&&... __args);

      template<typename... _Args>
      iterator _M_emplace_hint_equal(const_iterator __pos,
                                     _Args&&... __args);

      template<typename _Iter>
      using __same_value_type
        = is_same<value_type, typename iterator_traits<_Iter>::value_type>;

      template<typename _InputIterator>
      __enable_if_t<__same_value_type<_InputIterator>::value>
      _M_insert_range_unique(_InputIterator __first, _InputIterator __last);

      template<typename _InputIterator>
      __enable_if_t<!__same_value_type<_InputIterator>::value>
      _M_insert_range_unique(_InputIterator __first, _InputIterator __last);

      template<typename _InputIterator>
      __enable_if_t<__same_value_type<_InputIterator>::value>
      _M_insert_range_equal(_InputIterator __first, _InputIterator __last);

      template<typename _InputIterator>
      __enable_if_t<!__same_value_type<_InputIterator>::value>
      _M_insert_range_equal(_InputIterator __first, _InputIterator __last);
    private:
      void _M_erase_aux(const_iterator __position);
      void _M_erase_aux(const_iterator __first, const_iterator __last);

    public:
      iterator erase(const_iterator __position);
      iterator erase(iterator __position);
      size_type erase(const key_type& __x);

      size_type _M_erase_unique(const key_type& __x);

      iterator erase(const_iterator __first, const_iterator __last);
      void clear();

      // Set operations.
      iterator       find(const key_type& __k);
      const_iterator find(const key_type& __k) const;

      size_type count(const key_type& __k) const;

      iterator       lower_bound(const key_type& __k);
      const_iterator lower_bound(const key_type& __k) const;

      iterator       upper_bound(const key_type& __k);
      const_iterator upper_bound(const key_type& __k) const;

      pair<iterator, iterator>
      equal_range(const key_type& __k);

      pair<const_iterator, const_iterator>
      equal_range(const key_type& __k) const;

      template<typename _Kt,
	             typename _Req = __has_is_transparent_t<_Compare, _Kt>>
      iterator _M_find_tr(const _Kt& __k);

      template<typename _Kt,
	             typename _Req = __has_is_transparent_t<_Compare, _Kt>>
    	const_iterator _M_find_tr(const _Kt& __k) const;

      template<typename _Kt,
	             typename _Req = __has_is_transparent_t<_Compare, _Kt>>
    	size_type _M_count_tr(const _Kt& __k) const;

      template<typename _Kt,
	             typename _Req = __has_is_transparent_t<_Compare, _Kt>>
	    _Base_ptr _M_lower_bound_tr(const _Kt& __k) const;

      template<typename _Kt,
	             typename _Req = __has_is_transparent_t<_Compare, _Kt>>
	    _Base_ptr _M_upper_bound_tr(const _Kt& __k) const;

      template<typename _Kt,
	             typename _Req = __has_is_transparent_t<_Compare, _Kt>>
      pair<iterator, iterator>
      _M_equal_range_tr(const _Kt& __k);

      template<typename _Kt,
	             typename _Req = __has_is_transparent_t<_Compare, _Kt>>
      pair<const_iterator, const_iterator>
      _M_equal_range_tr(const _Kt& __k) const;

      // Debugging.
      bool __rb_verify() const;

      _Rb_tree& operator=(_Rb_tree&&)
      noexcept(_Node_alloc_traits::_S_nothrow_move()
	       && is_nothrow_move_assignable<_Compare>::value);

      template<typename _Iterator>
	    void _M_assign_unique(_Iterator, _Iterator);

      template<typename _Iterator>
	    void _M_assign_equal(_Iterator, _Iterator);

    private:
      void _M_move_data(_Rb_tree& __x, true_type);
      void _M_move_data(_Rb_tree&, false_type);

      void _M_move_assign(_Rb_tree&, true_type);
      void _M_move_assign(_Rb_tree&, false_type);

      static _Node_ptr _S_adapt(typename _Node_alloc_traits::pointer __ptr);

    public:
      insert_return_type _M_reinsert_node_unique(node_type&& __nh);

      iterator _M_reinsert_node_equal(node_type&& __nh);

      iterator
      _M_reinsert_node_hint_unique(const_iterator __hint, node_type&& __nh);

      iterator
      _M_reinsert_node_hint_equal(const_iterator __hint, node_type&& __nh);

      node_type extract(const_iterator __pos);
      node_type extract(const key_type& __k);

      template<typename _Compare2>
      using _Compatible_tree
        = _Rb_tree<_Key, _Val, _KeyOfValue, _Compare2, _Alloc>;

      template<typename, typename>
	    friend struct _Rb_tree_merge_helper;

      template<typename _Compare2>
      void _M_merge_unique(_Compatible_tree<_Compare2>& __src) noexcept;

      template<typename _Compare2>
	    void _M_merge_equal(_Compatible_tree<_Compare2>& __src) noexcept;

      friend bool operator==(const _Rb_tree& __x, const _Rb_tree& __y);

      friend auto
      operator<=>(const _Rb_tree& __x, const _Rb_tree& __y);

    private:
      struct _Auto_node
      {
	      template<typename... _Args>
	      _Auto_node(_Rb_tree& __t, _Args&&... __args);
        ~_Auto_node();
	      _Auto_node(_Auto_node&& __n);

        const _Key& _M_key() const;

        iterator _M_insert(pair<_Base_ptr, _Base_ptr> __p);
        iterator _M_insert_equal_lower();

        _Rb_tree& _M_t;
        _Node_ptr _M_node;
      };
    };

  template<typename _Key, typename _Val, typename _KeyOfValue,
	   typename _Compare, typename _Alloc>
    inline void
    swap(_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>& __x,
	 _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>& __y)
    { __x.swap(__y); }

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
