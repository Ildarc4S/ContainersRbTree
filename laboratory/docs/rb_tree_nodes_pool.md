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

## Функция `_M_extract()`
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
Эта функция предназначена для извлечения узлов из кч дерева для их повторного использования. Возвращается извлеченный узел.

### Пошаговое выполнение
1. Проверка на пустоту:
```cpp
if (!_M_nodes) {
  return _M_nodes;
}
```
2. Извлечение текущего узла:
```cpp
_Base_ptr __node = _M_nodes; // Запоминаем текущий узел
_M_nodes = _M_nodes->_M_parent; // Переходим к родителю
```
3. Обработка случая, когда родитель существует:
```cpp
if (_M_nodes) {
  // Удаляем ссылку на извлеченный узел у родителя
  if (_M_nodes->_M_right == __node) { // Если узел был правым ребенком
    _M_nodes->_M_right = _Base_ptr(); // Обнуляем правую ссылку
    
    // Ищем следующий узел для извлечения
    if (_M_nodes->_M_left) { // Если есть левый ребенок
      _M_nodes = _M_nodes->_M_left; // Переходим к нему
      
      // Идем до крайнего правого узла в поддереве
      while (_M_nodes->_M_right) {
        _M_nodes = _M_nodes->_M_right;
      }
      
      // Если у крайнего правого есть левый ребенок - переходим к нему
      if (_M_nodes->_M_left) {
        _M_nodes = _M_nodes->_M_left;
      }
    }
  } else { // Если узел был левым ребенком
    _M_nodes->_M_left = _Base_ptr(); // Просто обнуляем левую ссылку
  }
}
```
4. Обработка случая, когда родителя нет (извлечен корень):
```cpp
else {
  _M_root = nullptr; // Обнуляем корень, так как мы его извлекли
}
```
5. Возврат извлеченного узла:
```cpp
return __node;
```

#### Пример для понимания:
**Исходное дерево:**
```
   A
  / \
 B   C
/ \
D  E
```
- `_M_root` = A  
- `_M_nodes` изначально = C (самый правый узел, `_M_rightmost()`).  

---

**Вызов 1: `_M_extract()` (извлекаем C)**
```cpp
_Base_ptr __node = _M_nodes;     // __node = C
_M_nodes = _M_nodes->_M_parent;  // _M_nodes = A (родитель C)

if (_M_nodes) { // A существует
  if (_M_nodes->_M_right == __node) {  // A->right == C? Да!
    _M_nodes->_M_right = _Base_ptr();  // A->right = null
    
    if (_M_nodes->_M_left) {           // A->left = B? Да!
      _M_nodes = _M_nodes->_M_left;  // _M_nodes = B
        
      while (_M_nodes->_M_right) { // Ищем самый правый в поддереве B
        _M_nodes = _M_nodes->_M_right;
      }
      // B->right = E → _M_nodes = E
        
      if (_M_nodes->_M_left) {        // E->left есть? Нет.
        _M_nodes = _M_nodes->_M_left;
    }
  }
}
// ... (ветка else не выполняется)
}
```

**Схема после извлечения C:**
```
       A
      /
     B
    / \
   D   E
```
- `_M_root` = A  
- `_M_nodes` = E (следующий кандидат)  
- **Связи:** `A->right = nullptr`, остальное без изменений.  

---

**Вызов 2: `_M_extract()` (извлекаем E)**
```cpp
_Base_ptr __node = _M_nodes;     // __node = E
_M_nodes = _M_nodes->_M_parent;  // _M_nodes = B (родитель E)

if (_M_nodes) {   // B существует
  if (_M_nodes->_M_right == __node) {  // B->right == E? Да!
    _M_nodes->_M_right = _Base_ptr();  // B->right = null
    
    if (_M_nodes->_M_left) {           // B->left = D? Да!
      _M_nodes = _M_nodes->_M_left;    // _M_nodes = D
        
      while (_M_nodes->_M_right) { // D->right есть? Нет.
        _M_nodes = _M_nodes->_M_right;
      }
        
      if (_M_nodes->_M_left) {     // D->left есть? Нет.
        _M_nodes = _M_nodes->_M_left;
      }
    }
  }
}
```

**Схема после извлечения E:**
```
       A
      /
     B
    /
   D
```
- `_M_root` = A  
- `_M_nodes` = D (следующий кандидат)  
- **Связи:** `B->right = nullptr`, остальное без изменений.  

---

**Вызов 3: `_M_extract()` (извлекаем D)**
```cpp
_Base_ptr __node = _M_nodes;     // __node = D
_M_nodes = _M_nodes->_M_parent;  // _M_nodes = B (родитель D)

if (_M_nodes) { // B существует
  if (_M_nodes->_M_right == __node) { // B->right == D? Нет!
    // Переходим в else
  } else {
    _M_nodes->_M_left = _Base_ptr();  // B->left = null
  }
}
```
**Схема после извлечения D:**
```text
       A
      /
     B
```
- `_M_root` = `A`
- `_M_nodes` = `B` (следующий кандидат)
- Связи: `B->left = nullptr`, `B->right` уже был `nullptr`.

**Вызов 4: `_M_extract()` (извлекаем B)**
```cpp
_Base_ptr __node = _M_nodes;     // __node = B
_M_nodes = _M_nodes->_M_parent;  // _M_nodes = A (родитель B)

if (_M_nodes) {                  // A существует
    if (_M_nodes->_M_right == __node) {  // A->right == B? Нет!
        // Переходим в else:
        _M_nodes->_M_left = nullptr;     // A->left = null
    }
}
return __node;  // Возвращаем B
```
**Схема после извлечения B:**
```text
    A
```
- `_M_root` = `A`
- `_M_nodes` = `A` (следующий кандидат)
- Связи: `A->left = nullptr`, `A->right` уже был `nullptr`.

**Вызов 5: `_M_extract()` (извлекаем A)**
```cpp
_Base_ptr __node = _M_nodes;     // __node = A
_M_nodes = _M_nodes->_M_parent;  // _M_nodes = nullptr (у A нет родителя)

if (_M_nodes) { // Пропускаем (родителя нет)
  ... 
} else {
  _M_root = _Base_ptr();  // Дерево пусто!
}
return __node;  // Возвращаем A
**Схема после извлечения A:**
```text
[дерево пусто]
```
- `_M_root` = `nullptr`
- `_M_nodes` = `nullptr`
- Связи: Все удалены.

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
Этот оператор и позволяет переиспользовать существующий удаленный узел, либо создает новый узел.

### Пошагоывй разбор кода
1. Извлечение базового узла
```cpp
_Base_ptr __base = _M_extract();
```
Вызывается _M_extract(), который был рассмотрен ранее.

2. Если узел найден (__base != nullptr)
```cpp
if (__base) {
  _Node_ptr __node = static_cast<_Node&>(*__base)._M_node_ptr();
  
  _M_t._M_destroy_node(__node);
  _M_t._M_construct_node(__node, std::forward<_Arg>(__arg));
    
  return __node;
}
```
Что происходит:
1. Приведение типа к `_Node_ptr`(такой код уже был рассмотрен ранее).
2. Уничтожение старого значения:  
```cpp
_M_t._M_destroy_node(__node);
``` 
вызывает деструктор старого значения в узле.

3. Создание нового значения:
```cpp
_M_t._M_construct_node(__node, std::forward<_Arg>(__arg))
```
использует perfect forwarding, чтобы создать новое значение на месте старого.

4. Возврат переиспользованного узла:
Возвращается __node (теперь с новыми данными).

3. Если узлов для переиспользования нет (__base == nullptr)
```cpp
return _M_t._M_create_node(std::forward<_Arg>(__arg));
```
Если `_M_extract()` вернул `nullptr`, вызывается `_M_create_node`, который:
- Выделяет память под новый узел.
- Конструирует в нём значение из __arg (снова через std::forward для perfect forwarding).

Разбор закончен :)
