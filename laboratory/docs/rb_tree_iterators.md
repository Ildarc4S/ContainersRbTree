# Итераторы в STL _Rb_tree

Итераторы представлены в виде структур `_Iterator`,``,``

```cpp
template<bool _Const, typename _ValPtr>
struct _Iterator
{
    template<typename _Tp>
    using __maybe_const = __conditional_t<_Const, const _Tp, _Tp>;

    using __ptr_traits = pointer_traits<_ValPtr>;
    using value_type   = typename __ptr_traits::element_type;
    using reference    = __maybe_const<value_type>&;
    using pointer      = __maybe_const<value_type>*;

    using iterator_category =	bidirectional_iterator_tag;
    using difference_type =	ptrdiff_t;

    using _Node = __rb_tree::_Node<_ValPtr>;
    using _Node_base = __rb_tree::_Node_base<__ptr_rebind<_ValPtr, void>>;
    using _Base_ptr =	 typename _Node_base::_Base_ptr;

    _Iterator() noexcept
    : _M_node() { }

    constexpr explicit
    _Iterator(_Base_ptr __x) noexcept
    : _M_node(__x) { }

    _Iterator(const _Iterator&) = default;
    _Iterator& operator=(const _Iterator&) = default;

    constexpr
    _Iterator(const _Iterator<false, _ValPtr>& __it) requires _Const
: _M_node(__it._M_node) { }

    [[__nodiscard__]]
    reference
    operator*() const noexcept
    { return *static_cast<_Node&>(*_M_node)._M_valptr(); }

    [[__nodiscard__]]
    pointer
    operator->() const noexcept
    { return static_cast<_Node&>(*_M_node)._M_valptr(); }

    _GLIBCXX14_CONSTEXPR _Iterator&
    operator++() noexcept
    {
if (_M_node->_M_right)
    {
    _M_node = _M_node->_M_right;
    while (_M_node->_M_left)
        _M_node = _M_node->_M_left;
    }
else
    {
    _Base_ptr __y = _M_node->_M_parent;
    while (_M_node == __y->_M_right)
        {
    _M_node = __y;
    __y = __y->_M_parent;
        }
    if (_M_node->_M_right != __y)
        _M_node = __y;
    }

return *this;
    }

    _GLIBCXX14_CONSTEXPR _Iterator
    operator++(int) noexcept
    {
_Iterator __tmp(this->_M_node);
++*this;
return __tmp;
    }

    _GLIBCXX14_CONSTEXPR _Iterator&
    operator--() noexcept
    {
if (_M_node->_M_color == _S_red
    && _M_node->_M_parent->_M_parent == _M_node)
    _M_node = _M_node->_M_right;
else if (_M_node->_M_left)
    {
    _Base_ptr __y = _M_node->_M_left;
    while (__y->_M_right)
        __y = __y->_M_right;
    _M_node = __y;
    }
else
    {
    _Base_ptr __y = _M_node->_M_parent;
    while (_M_node == __y->_M_left)
        {
    _M_node = __y;
    __y = __y->_M_parent;
        }
    _M_node = __y;
    }
return *this;
    }

    _GLIBCXX14_CONSTEXPR _Iterator
    operator--(int) noexcept
    {
_Iterator __tmp(this->_M_node);
--*this;
return __tmp;
    }

    [[__nodiscard__]]
    friend bool
    operator==(const _Iterator& __x, const _Iterator& __y) _GLIBCXX_NOEXCEPT
    { return __x._M_node == __y._M_node; }

#if ! __cpp_lib_three_way_comparison
    [[__nodiscard__]]
    friend bool
    operator!=(const _Iterator& __x, const _Iterator& __y) _GLIBCXX_NOEXCEPT
    { return __x._M_node != __y._M_node; }
#endif

    _Base_ptr _M_node;
};
```