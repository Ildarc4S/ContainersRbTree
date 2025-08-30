# _Node_traits

`_Node_traits` — это внутренний вспомогательный шаблонный класс в реализации красно-черного дерева (`_Rb_tree`) из STL. Он предоставляет **единый интерфейс для работы с узлами дерева**, абстрагируясь от конкретного типа указателей (обычные `T*` или "fancy pointers" из аллокаторов).

```cpp
template<typename _Val, typename _ValPtr>
struct _Node_traits {
  using _Node = __rb_tree::_Node<_ValPtr>;
  using _Node_ptr = __ptr_rebind<_ValPtr, _Node>;
  using _Node_base = __rb_tree::_Node_base<__ptr_rebind<_ValPtr, void>>;
  using _Base_ptr = __ptr_rebind<_ValPtr, _Node_base>;
  using _Header_t = __rb_tree::_Header<_Node_base>;
  using _Iterator = __rb_tree::_Iterator<false, _ValPtr>;
  using _Const_iterator = __rb_tree::_Iterator<true, _ValPtr>;

  static void
  _Rotate_left(_Base_ptr __x, _Base_ptr& __root) {
    const _Base_ptr __y = __x->_M_right;
    __x->_M_right = __y->_M_left;

    if (__y->_M_left) {
      __y->_M_left->_M_parent = __x;
    }
    __y->_M_parent = __x->_M_parent;

    if (__x == __root) {
      __root = __y;
    } else if (__x == __x->_M_parent->_M_left) {
      __x->_M_parent->_M_left = __y;
    } else {
      __x->_M_parent->_M_right = __y;
    }

    __y->_M_left = __x;
    __x->_M_parent = __y;
  }

  static void
  _Rotate_right(_Base_ptr __x, _Base_ptr& __root) {
    const _Base_ptr __y = __x->_M_left;
    __x->_M_left = __y->_M_right;

    if (__y->_M_right) {
      __y->_M_right->_M_parent = __x;
    }
    __y->_M_parent = __x->_M_parent;

    if (__x == __root) { 
      __root = __y;
    } else if (__x == __x->_M_parent->_M_right) {
      __x->_M_parent->_M_right = __y;
    } else {
      __x->_M_parent->_M_left = __y;
    }

    __y->_M_right = __x;
    __x->_M_parent = __y;
  }

  static void
  _S_insert_and_rebalance(const bool __insert_left,
                          _Base_ptr __x, _Base_ptr __p,
                          _Node_base& __header) {
    _Base_ptr& __root = __header._M_parent;

    __x->_M_parent = __p;
    __x->_M_left = __x->_M_right = nullptr;
    __x->_M_color = _S_red;

    if (__insert_left) {
      __p->_M_left = __x;

      if (std::__to_address(__p) == std::addressof(__header)) {
        __header._M_parent = __x;
        __header._M_right = __x;
      } else if (__p == __header._M_left) {
        __header._M_left = __x;
      } else {
        __p->_M_right = __x;

        if (__p == __header._M_right) {
          __header._M_right = __x;
        }
      }
    // Rebalance.
    while (__x != __root
           && __x->_M_parent->_M_color == _S_red) {
      const _Base_ptr __xpp = __x->_M_parent->_M_parent;

      if (__x->_M_parent == __xpp->_M_left) {
        const _Base_ptr __y = __xpp->_M_right;

        if (__y && __y->_M_color == _S_red) {
          __x->_M_parent->_M_color = _S_black;
          __y->_M_color = _S_black;
          __xpp->_M_color = _S_red;
          __x = __xpp;
        } else {
          if (__x == __x->_M_parent->_M_right) {
            __x = __x->_M_parent;
            _Rotate_left(__x, __root);
          }
          __x->_M_parent->_M_color = _S_black;
          __xpp->_M_color = _S_red;
          _Rotate_right(__xpp, __root);
        }
      } else {
        const _Base_ptr __y = __xpp->_M_left;
        if (__y && __y->_M_color == _S_red) {
            __x->_M_parent->_M_color = _S_black;
            __y->_M_color = _S_black;
            __xpp->_M_color = _S_red;
            __x = __xpp;
        } else {
          if (__x == __x->_M_parent->_M_left) {
            __x = __x->_M_parent;
            _Rotate_right(__x, __root);
          }
          
          __x->_M_parent->_M_color = _S_black;
          __xpp->_M_color = _S_red;
          _Rotate_left(__xpp, __root);
        }
      }
    }
    __root->_M_color = _S_black;
  }

  static _Base_ptr
  _S_rebalance_for_erase(_Base_ptr __z, _Node_base& __header) {
    _Base_ptr& __root = __header._M_parent;
    _Base_ptr& __leftmost = __header._M_left;
    _Base_ptr& __rightmost = __header._M_right;
    _Base_ptr __y = __z;
    _Base_ptr __x{};
    _Base_ptr __x_parent{};

    if (!__y->_M_left) {  // __z has at most one non-null child. y == z.
      __x = __y->_M_right;     // __x might be null.
    } else {
      if (!__y->_M_right) {  // __z has exactly one non-null child. y == z.
        __x = __y->_M_left;    // __x is not null.
      } else {
          // __z has two non-null children.  Set __y to
        __y = __y->_M_right;   //   __z's successor.  __x might be null.
        while (__y->_M_left) {
          __y = __y->_M_left;
        }
        __x = __y->_M_right;
      }
    }

    if (__y != __z) {
      // relink y in place of z. y is z's successor
      __z->_M_left->_M_parent = __y;
      __y->_M_left = __z->_M_left;
      if (__y != __z->_M_right) {
        __x_parent = __y->_M_parent;
        if (__x) {
          __x->_M_parent = __y->_M_parent;
        }
        __y->_M_parent->_M_left = __x;   // __y must be a child of _M_left
        __y->_M_right = __z->_M_right;
        __z->_M_right->_M_parent = __y;
      } else {
        __x_parent = __y;
      }

      if (__root == __z) {
        __root = __y;
      } else if (__z->_M_parent->_M_left == __z) {
        __z->_M_parent->_M_left = __y;
      } else {
        __z->_M_parent->_M_right = __y;
      }

      __y->_M_parent = __z->_M_parent;
      std::swap(__y->_M_color, __z->_M_color);
      __y = __z;
      // __y now points to node to be actually deleted
    } else {                        // __y == __z
      __x_parent = __y->_M_parent;
      if (__x) {
        __x->_M_parent = __y->_M_parent;
      }

      if (__root == __z) {
        __root = __x;
      } else {
        if (__z->_M_parent->_M_left == __z) {
          __z->_M_parent->_M_left = __x;
        } else {
          __z->_M_parent->_M_right = __x;
        }
      }
        
      if (__leftmost == __z) {
        if (!__z->_M_right) {        // __z->_M_left must be null also
          __leftmost = __z->_M_parent;
          // makes __leftmost == _M_header if __z == __root
        } else {
          __leftmost = _Node_base::_S_minimum(__x);
        }
      }
        
      if (__rightmost == __z) {
        if (__z->_M_left == 0) {         // __z->_M_right must be null also
          __rightmost = __z->_M_parent;
          // makes __rightmost == _M_header if __z == __root
        } else {                      // __x == __z->_M_left
          __rightmost = _Node_base::_S_maximum(__x);
        }
      }
    }

    if (__y->_M_color != _S_red) {
      while (__x != __root && (__x == 0 || __x->_M_color == _S_black)) {
        if (__x == __x_parent->_M_left) {
          _Base_ptr __w = __x_parent->_M_right;
          if (__w->_M_color == _S_red) {
            __w->_M_color = _S_black;
            __x_parent->_M_color = _S_red;
            _Rotate_left(__x_parent, __root);
            __w = __x_parent->_M_right;
          }
          if ((!__w->_M_left || __w->_M_left->_M_color == _S_black) &&
              (!__w->_M_right || __w->_M_right->_M_color == _S_black)) {
            __w->_M_color = _S_red;
            __x = __x_parent;
            __x_parent = __x_parent->_M_parent;
          } else {
            if (!__w->_M_right || __w->_M_right->_M_color == _S_black) {
              __w->_M_left->_M_color = _S_black;
              __w->_M_color = _S_red;
              _Rotate_right(__w, __root);
              __w = __x_parent->_M_right;
            }
            __w->_M_color = __x_parent->_M_color;
            __x_parent->_M_color = _S_black;
            if (__w->_M_right) {
              __w->_M_right->_M_color = _S_black;
            }
            _Rotate_left(__x_parent, __root);
            break;
          }
        } else {
          // same as above, with _M_right <-> _M_left.
          _Base_ptr __w = __x_parent->_M_left;
          if (__w->_M_color == _S_red) {
            __w->_M_color = _S_black;
            __x_parent->_M_color = _S_red;
            _Rotate_right(__x_parent, __root);
            __w = __x_parent->_M_left;
          }
          if ((!__w->_M_right || __w->_M_right->_M_color == _S_black) &&
              (!__w->_M_left || __w->_M_left->_M_color == _S_black)) {
            __w->_M_color = _S_red;
            __x = __x_parent;
            __x_parent = __x_parent->_M_parent;
          } else {
            if (!__w->_M_left || __w->_M_left->_M_color == _S_black) {
              __w->_M_right->_M_color = _S_black;
              __w->_M_color = _S_red;
              _Rotate_left(__w, __root);
              __w = __x_parent->_M_left;
            }
            __w->_M_color = __x_parent->_M_color;
            __x_parent->_M_color = _S_black;
            if (__w->_M_left) {
              __w->_M_left->_M_color = _S_black;
            }
            _Rotate_right(__x_parent, __root);
            break;
          }
        }
      }
      if (__x) {
        __x->_M_color = _S_black;
      }
    }

    return __y;
  }
};
```

---

### **Что хранит `_Node_traits`?**
1. **Типы узлов и итераторов**:
   - `_Node` — тип узла дерева (зависит от `_ValPtr`).
   - `_Node_ptr` — указатель на узел (может быть `T*` или fancy pointer).
   - `_Node_base` — базовый класс узла (без значения, только структура дерева).
   - `_Base_ptr` — указатель на базовый узел.
   - `_Header_t` — заголовок дерева (хранит корень, крайние элементы и счетчик узлов).
   - `_Iterator`, `_Const_iterator` — итераторы для обхода дерева.

2. **Методы для балансировки**:
   - `_Rotate_left`, `_Rotate_right` — повороты поддеревьев (лево/право).
   - `_S_insert_and_rebalance` — балансировка узла после вставки.
   - `_S_rebalance_for_erase` — балансировка после удаления узла.

