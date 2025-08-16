# Rb_tree pool

Код из `STL`:
```cpp
struct _Reuse_or_alloc_node
{
  _Reuse_or_alloc_node(_Rb_tree& __t)
    : _M_root(__t._M_root()),
      _M_nodes(__t._M_rightmost()),
      _M_t(__t) {

    if (_M_root) {
      _M_root->_M_parent = _Base_ptr();

      if (_M_nodes->_M_left) { 
        _M_nodes = _M_nodes->_M_left;
      }
    } else {
      _M_nodes = _Base_ptr();
    }
  } 

  _Reuse_or_alloc_node(const _Reuse_or_alloc_node&) = delete;

  ~_Reuse_or_alloc_node() {
    if (_M_root) {
      _M_t._M_erase(static_cast<_Node&>(*_M_root)._M_node_ptr());
    }
  }

  template<typename _Arg>
  _Node_ptr operator()(_GLIBCXX_FWDREF(_Arg) __arg) {
    _Base_ptr __base = _M_extract();
     
    if (__base) {
      _Node_ptr __node = static_cast<_Node&>(*__base)._M_node_ptr();
      
      _M_t._M_destroy_node(__node);
      _M_t._M_construct_node(__node, _GLIBCXX_FORWARD(_Arg, __arg));
        
      return __node;
    }

    return _M_t._M_create_node(_GLIBCXX_FORWARD(_Arg, __arg));
  }

 private:
  _Base_ptr _M_extract() {
    if (!_M_nodes) {
      return _M_nodes;
    }

    _Base_ptr __node = _M_nodes;
    _M_nodes = _M_nodes->_M_parent;
    if (_M_nodes) {
      if (_M_nodes->_M_right == __node) {
        _M_nodes->_M_right = _Base_ptr();

        if (_M_nodes->_M_left) {
          _M_nodes = _M_nodes->_M_left;

          while (_M_nodes->_M_right) {
            _M_nodes = _M_nodes->_M_right;
          }

          if (_M_nodes->_M_left) {
            _M_nodes = _M_nodes->_M_left;
          }
        }
      } else { // __node is on the left.
       _M_nodes->_M_left = _Base_ptr();
      }
    } else {
      _M_root = _Base_ptr();
    }
    return __node;
  }

  _Base_ptr _M_root;
  _Base_ptr _M_nodes;
  _Rb_tree& _M_t;
};
```
В коде используются макросы:`_GLIBCXX_FWDREF(_Arg)` и  `_GLIBCXX_FORWARD(_Arg, __arg)`. Это аналогично использованию двойных ссылок и `std::forward`:
соответственно `_Arg&&` и `std::forward<_Arg>(__arg)>`.  
Произведем замену:
