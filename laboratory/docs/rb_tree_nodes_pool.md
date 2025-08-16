# Rb_tree pool
## Общее назначение
`_Reuse_or_alloc_node` — это вспомогательный класс в реализации красно-черного дерева _Rb_tree, который предоставляет механизм:

1. Переиспользования существующих узлов дерева (если они доступны)
2. Выделения новых узлов (если переиспользовать нечего)

Это оптимизация для случаев, когда дерево модифицируется (например, при вставке или удалении), чтобы избежать лишних аллокаций/деаллокаций памяти.

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
В коде используются макросы:`_GLIBCXX_FWDREF(_Arg)` и  `_GLIBCXX_FORWARD(_Arg, __arg)`. Это аналогично использованию двойных ссылок и `std::forward` соответственно: `_Arg&&` и `std::forward<_Arg>(__arg)>`.  
Произведем замену:
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
  _Node_ptr operator()(_Arg&& __arg) {
    _Base_ptr __base = _M_extract();
     
    if (__base) {
      _Node_ptr __node = static_cast<_Node&>(*__base)._M_node_ptr();
      
      _M_t._M_destroy_node(__node);
      _M_t._M_construct_node(__node, std::forward<_Arg>(__arg));
        
      return __node;
    }

    return _M_t._M_create_node(std::forward<_Arg>(__arg));
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

Теперь разберемся со структурой:
## Поля 
- `_M_root` — корень дерева, которое разбирается 
- `_M_nodes` — текущий узел, который можно переиспользовать
- `_M_t` — ссылка на связанное красно-черное дерево

## Дополнительно используемый функционал:
1. `_M_root()` - метод, возвращающий корневой узел дерева.
2. `_M_rightmost()` - вметод, возвращающий указатель на самый правый узел (максимальный элемент дерева).
3. `_Base_ptr()` - вызов конструктора по умолчанию указателя на базовый узел красно-черного дерева(будет эквивалентно присвоению `nullptr`).
4. `_M_erase(_Node_ptr)` - рекурсивно удаляет узел и всех его потомков(освобождает память узла через аллокатор).
5. `_M_node_ptr()` - возвращает `_Node_ptr` указатель на текущий объект(возвращает `this`).
6. `_M_destroy_node(_Node_ptr)` - уничтожает значенние в узле (но не ам узел), т. е. вызывается деструктор хранимого значения, узел остается пустым для переиспользования.
7. `_M_construct_node(_Node_ptr, Args&&...)` - конструирует новое значение в существующем узле.
8. `_M_create_node(Args&&...)` - создает новый узел с переданными аргументами(выделяет память через аллокатор и конструирует значение в узле).

## Конструктор
```cpp
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
```
### Пошаговое выполнение
1. Инициализация членов класса:
```cpp
_M_root(__t._M_root()),     // Сохраняем корень исходного дерева
_M_nodes(__t._M_rightmost()), // Запоминаем самый правый узел
_M_t(__t)                   // Сохраняем ссылку на исходное дерево
```

2. Проверка на пустое дерево:
```cpp
if (_M_root) {  // Если дерево не пустое
  ...           // Шаг 3 и 4  
} else {
  _M_nodes = _Base_ptr();  // Иначе обнуляем указатель
}
```

3. Подготовка корневого узла:
```cpp
_M_root->_M_parent = _Base_ptr();
```
Обнуляет родительский указатель корня, тем самым отделяя `_t` от освного дерева.

4. Поиск узла для переиспользования:
```cpp
if (_M_nodes->_M_left) { 
  _M_nodes = _M_nodes->_M_left;
}
``` 
Если у `_M_nodes`(который является крайним правым узлом) есть левый потомок, то `_M_nodes` будет указывать на него.

#### Визуальный пример
Для дерева:
```text
     D
   /   \
  B     F
 / \   / \
A   C E   G
       \
        H
```
1. `_M_root` → `D` (корень)
2. `_M_nodes` изначально → `G` (самый правый)
3. Так как `G` не имеет левого потомка → `_M_nodes` остаётся `G`

Для дерева:
```text
     D
   /   \
  B     F
 / \   / \
A   C E   G
         /
        H
```
1. `_M_nodes` изначально → `G`
2. У `G` есть левый потомок `H` → `_M_nodes` переходит на `H`

**Замечание:**
```cpp
_Reuse_or_alloc_node(const _Reuse_or_alloc_node&) = delete;
```
Конструктор копирования удален. Сделано это для предотвращения неявного копирования.
Согласно стандарту C++ (начиная с C++11), если класс объявляет:
- Удалённый конструктор копирования, или
- Удалённый деструктор, или
- Удалённый конструктор перемещения,

то оператор копирующего присваивания неявно помечается как = delete.
Таким образом, нам  не надо его помечать `delete`, явно добавляя такой код:
```cpp
_Reuse_or_alloc_node& operator=(const _Reuse_or_alloc_node&) = delete;
```

## Деструктор
```cpp
~_Reuse_or_alloc_node() {
  if (_M_root) {
    _M_t._M_erase(static_cast<_Node&>(*_M_root)._M_node_ptr());
  }
}
```
### Пошаговое выполнение
1. Проверка на то что дерево существует:
```cpp
if (_M_root) {
  ...
}
```

2. Преобразование в ссылку на тип `_Node` из типа `_Base_ptr = _Node_base*`:
```cpp
static_cast<_Node&>(*_M_root)
```
Это возможно по причине того что класс `_Node<_ValPtr>` является наследником класса `_Node_base<_VoidPtr>`.

3. Получение указателя:
И через `_M_node_ptr()` получаем указатель `_Node_ptr` на объект `Node&`.
```cpp
static_cast<_Node&>(*_M_root)._M_node_ptr()
```

4. Удаление дерева:
```cpp
_M_t._M_erase(static_cast<_Node&>(*_M_root)._M_node_ptr());
```
Происходи рекурсивное удаление дерева.

Функция 
```cpp
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
```

## Оператор ()
```cpp
template<typename _Arg>
_Node_ptr operator()(_Arg&& __arg) {
  _Base_ptr __base = _M_extract();
    
  if (__base) {
    _Node_ptr __node = static_cast<_Node&>(*__base)._M_node_ptr();
    
    _M_t._M_destroy_node(__node);
    _M_t._M_construct_node(__node, std::forward<_Arg>(__arg));
      
    return __node;
  }

  return _M_t._M_create_node(std::forward<_Arg>(__arg));
}
```