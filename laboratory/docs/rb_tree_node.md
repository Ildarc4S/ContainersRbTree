# _Node_base
**Назначение `_Node_base`**

Это базовый класс для узлов красно-чёрного дерева, который:
1. Управляет топологией дерева (родитель, левый/правый потомки).
2. Хранит цвет узла (красный/чёрный).
3. Предоставляет методы для навигации (поиск мин/макс узла в поддереве).
4. Не содержит данных — только служебные поля для балансировки.

```cpp
template<typename _VoidPtr>
struct _Node_base {
  using _Base_ptr = __ptr_rebind<_VoidPtr, _Node_base>;

  _Rb_tree_color	_M_color;
  _Base_ptr		_M_parent;
  _Base_ptr		_M_left;
  _Base_ptr		_M_right;

  static _Base_ptr
  _S_minimum(_Base_ptr __x) _GLIBCXX_NOEXCEPT {
    while (__x->_M_left) {
      __x = __x->_M_left;
    }
    return __x;
  }

  static _Base_ptr
  _S_maximum(_Base_ptr __x) _GLIBCXX_NOEXCEPT {
    while (__x->_M_right) {
      __x = __x->_M_right;
    }
    return __x;
  }

  // This is not const-correct, but it's only used in a const access path
  // by std::_Rb_tree::_M_end() where the pointer is used to initialize a
  // const_iterator and so constness is restored.
  _Base_ptr
  _M_base_ptr() const noexcept {
    return pointer_traits<_Base_ptr>::pointer_to(
            *const_cast<_Node_base*>(this));
  }
};
```

## Пошаговый разбор:

### 1. **Шаблон и псевдонимы типов**
```cpp
template<typename _VoidPtr>
struct _Node_base {
  using _Base_ptr = __ptr_rebind<_VoidPtr, _Node_base>;
};
```
- **`_VoidPtr`** — тип указателя, который может быть `void*` или "умным" указателем аллокатора.
- **`_Base_ptr`** — указатель на `_Node_base`.  
  `__ptr_rebind` — внутренний механизм STL для преобразования `_VoidPtr` в указатель на текущий тип (аналог `std::pointer_traits`).

---

### 2. **Поля структуры**
```cpp
_Rb_tree_color  _M_color;    // Цвет узла (красный, чёрный)
_Base_ptr       _M_parent;   // Указатель на родителя
_Base_ptr       _M_left;     // Указатель на левого потомка
_Base_ptr       _M_right;    // Указатель на правого потомка
```
- **`_M_color`** — поддерживает инварианты красно-чёрного дерева.
- **Указатели** (`_M_parent`, `_M_left`, `_M_right`) — формируют структуру дерева.

---

### 3. **Метод `_S_minimum`**
```cpp
static _Base_ptr _S_minimum(_Base_ptr __x) _GLIBCXX_NOEXCEPT {
  while (__x->_M_left) {
    __x = __x->_M_left;
  }
  return __x;
}
```
- Находит узел с **минимальным ключом** в поддереве, начиная с `__x` (идёт по левым потомкам до конца).
- Используется в `begin()` для получения первого элемента дерева за `O(log n)`.

---

### 4. **Метод `_S_maximum`**
```cpp
static _Base_ptr _S_maximum(_Base_ptr __x) _GLIBCXX_NOEXCEPT {
  while (__x->_M_right) {
    __x = __x->_M_right;
  }
  return __x;
}
```
- Находит узел с **максимальным ключом** в поддереве (идёт по правым потомкам).
- Используется для `end()` и операций с `reverse_iterator`.

---

### 5. **Метод `_M_base_ptr`**
```cpp
_Base_ptr _M_base_ptr() const noexcept {
  return pointer_traits<_Base_ptr>::pointer_to(*const_cast<_Node_base*>(this));
}
```
- Возвращает указатель на текущий узел, преобразуя `const this` в не-`const`(используя `const_cast`).
- Нужен для совместимости с `const`-методами (например, `end() const`).  
- `pointer_to` — создаёт указатель на `_Base_ptr` из ссылки, учитывая возможные перегрузки аллокатора.


# _Node 

`_Node` — это **шаблонный класс узла** красно-чёрного дерева, который:
- Наследует базовую структуру `_Node_base` (указатели и цвет).
- Хранит **данные** (значение типа `value_type`).
- Предоставляет методы для доступа к данным и преобразования указателей.

```cpp
template<typename _ValPtr>
struct _Node : public __rb_tree::_Node_base<__ptr_rebind<_ValPtr, void>> {

  using value_type = typename pointer_traits<_ValPtr>::element_type;
  using _Node_ptr = __ptr_rebind<_ValPtr, _Node>;

  _Node() noexcept { }
  ~_Node() { }
  _Node(_Node&&) = delete;

  union _Uninit_storage {
    _Uninit_storage() noexcept { }
    ~_Uninit_storage() { }

    value_type _M_data;
  };
  _Uninit_storage _M_u;

  value_type* _M_valptr() { 
    return std::addressof(_M_u._M_data); 
  }

  value_type const* _M_valptr() const { 
    return std::addressof(_M_u._M_data);
  }

  _Node_ptr _M_node_ptr() noexcept { 
    return pointer_traits<_Node_ptr>::pointer_to(*this); 
  }
};
```

## Пошаговый разбор структуры `_Node`

### 1. **Шаблон и псевдонимы типов**
```cpp
template<typename _ValPtr>
struct _Node : public __rb_tree::_Node_base<__ptr_rebind<_ValPtr, void>> {
```
- **`_ValPtr`** — тип указателя на значение (может быть обычным `T*` или fancy pointer аллокатора).
- Наследуется от `_Node_base`, где:
  - `__ptr_rebind<_ValPtr, void>` — преобразует `_ValPtr` в указатель на `void` (базовый тип для `_Node_base`).

#### Псевдонимы:
```cpp
using value_type = typename pointer_traits<_ValPtr>::element_type;
using _Node_ptr = __ptr_rebind<_ValPtr, _Node>;
```
- **`value_type`** — тип значения, на которое указывает `_ValPtr` (например, `int` для `int*`).
- **`_Node_ptr`** — указатель на текущий тип `_Node` (аналог `_Node*`, но с учётом fancy pointers).

---

### 2. **Конструкторы и деструктор**
```cpp
_Node() noexcept { }     // Конструктор по умолчанию
~_Node() { }             // Деструктор
_Node(_Node&&) = delete; // Запрет перемещения
```
- **Почему запрещено перемещение?**  
  Узлы дерева не должны перемещаться в памяти — их адреса фиксированы из-за указателей `_M_parent`/`_M_left`/`_M_right`.

---

### 4. **Хранение данных: `_Uninit_storage`**
```cpp
union _Uninit_storage {
  _Uninit_storage() noexcept { }
  ~_Uninit_storage() { }

  value_type _M_data;
};
_Uninit_storage _M_u;
```
- **union** используется для:
  - Ручного управления временем жизни `_M_data` (конструктор/деструктор вызываются явно).
  - Оптимизации памяти (не тратится место на флаги инициализации).
- Конструктор/деструктор `_Uninit_storage` тривиальны — не вызывают конструктор `value_type`.
- Реальное создание/удаление `_M_data` управляется методами `_Rb_tree` (например, `_M_create_node`).

---

### 5. **Метод `_M_valptr()`**
```cpp
value_type* _M_valptr() { 
  return std::addressof(_M_u._M_data); 
}
```
- Возвращает указатель на данные (`_M_data`) внутри `union`.
- **Зачем `addressof`?**  
  Защита от перегруженного `operator&` у `value_type`(Обычный оператор & можно перегрузить, и тогда он может возвращать не настоящий адрес, а что-то другое)

#### Const-версия:
```cpp
value_type const* _M_valptr() const { 
    return std::addressof(_M_u._M_data);
}
```
Нужна для константных объектов (например, в `const_iterator`).

---

### 6. **Метод `_M_node_ptr()`**
```cpp
_Node_ptr _M_node_ptr() noexcept { 
  return pointer_traits<_Node_ptr>::pointer_to(*this); 
}
```
- Преобразует текущий объект (`*this`) в указатель `_Node_ptr`.

---

### 7. **Пример использования в `_Rb_tree`**
1. **Создание узла**:
   ```cpp
   _Node* node = _M_get_node();  // Аллокация памяти
   new (node) _Node();           // Вызов конструктора
   ```
2. **Инициализация данных**:
   ```cpp
   // Вызов конструктора value_type вручную
   new (node->_M_valptr()) value_type(std::forward<Args>(args)...);
   ```
3. **Удаление узла**:
   ```cpp
   node->_M_valptr()->~value_type(); // Ручной вызов деструктора
   _M_put_node(node);                // Освобождение памяти
   ```

---

### 8. **Почему не `std::optional` или `std::variant`?**
- **Причина 1**: Минимальный оверхед. `union` не хранит флаги инициализации.
- **Причина 2**: Совместимость с C++98 (STL должна работать в старых стандартах).
- **Причина 3**: Полный контроль над временем жизни (важно для исключений).

# _Header

### **Назначение `_Header`**
1. **Управляет метаданными** красно-чёрного дерева (корень, крайние узлы, размер).
2. **Содержит фиктивный узел** (`_M_header`), упрощающий алгоритмы работы с деревом.
3. **Обеспечивает корректную инициализацию** и перемещение дерева.

```cpp
template<typename _NodeBase>
struct _Header {
private:
  using _Base_ptr =  typename _NodeBase::_Base_ptr;

public:
  _NodeBase		_M_header;
  size_t		_M_node_count; // Keeps track of size of tree.

  _Header() noexcept {
    _M_header._M_color = _S_red;
    _M_reset();
  }

  _Header(_Header&& __x) noexcept {
    if (__x._M_header._M_parent) {
      _M_move_data(__x);
    } else {
      _M_header._M_color = _S_red;
      _M_reset();
    }
  }

  void _M_move_data(_Header& __from) {
    _M_header._M_color = __from._M_header._M_color;
    _M_header._M_parent = __from._M_header._M_parent;
    _M_header._M_left = __from._M_header._M_left;
    _M_header._M_right = __from._M_header._M_right;
    _M_header._M_parent->_M_parent = _M_header._M_base_ptr();
    _M_node_count = __from._M_node_count;

    __from._M_reset();
  }

  void _M_reset() {
    _M_header._M_parent = nullptr;
    _M_header._M_left = _M_header._M_right = _M_header._M_base_ptr();
    _M_node_count = 0;
  }
};
```

## Пошаговый разбор кода

### 1. **Шаблон и псевдонимы типов**
```cpp
template<typename _NodeBase>
struct _Header {
private:
  using _Base_ptr = typename _NodeBase::_Base_ptr;
```
- **`_NodeBase`** — тип базового узла (например, `_Node_base`).
- **`_Base_ptr`** — указатель на `_NodeBase` (может быть обычным `_NodeBase*` или fancy pointer).

---

### 2. **Поля структуры**
```cpp
_NodeBase _M_header;  // Фиктивный узел
size_t    _M_node_count; // Количество элементов в дереве
```
- **`_M_header`** — фиктивный узел.
- **`_M_node_count`** — хранит размер дерева (чтобы `size()` работал за `O(1)`).

---

### 3. **Конструктор по умолчанию**
```cpp
_Header() noexcept {
  _M_header._M_color = _S_red; // Фиктивный узел всегда красный
  _M_reset();
}
```

Инициализирует пустое дерево:
- Цвет фиктивного узла — красный (не влияет на балансировку).
- Вызывает `_M_reset()` для установки начальных значений.

---

### 4. **Move-конструктор**
```cpp
_Header(_Header&& __x) noexcept {
  if (__x._M_header._M_parent) {
    _M_move_data(__x); // Перемещает данные из __x
  } else {
    _M_header._M_color = _S_red;
    _M_reset(); // Инициализирует как пустое
  }
}
```
- Если исходное дерево (`__x`) не пусто — данные перемещаются.
- Если пусто — инициализирует новое дерево как пустое.

---

### 5. **Метод `_M_move_data`**
```cpp
void _M_move_data(_Header& __from) {
  _M_header._M_color = __from._M_header._M_color;   // Цвет
  _M_header._M_parent = __from._M_header._M_parent; // Корень
  _M_header._M_left = __from._M_header._M_left;     // Минимум
  _M_header._M_right = __from._M_header._M_right;   // Максимум
  _M_header._M_parent->_M_parent = _M_header._M_base_ptr(); // Обновляем родителя корня
  _M_node_count = __from._M_node_count;

  __from._M_reset(); // Сбрасываем исходное дерево
}
```
Перемещает владение деревом из `__from` в текущий объект:
1. Копирует указатели и цвет.
2. Обновляет родителя корня (чтобы он указывал на новый фиктивный узел).
3. Сбрасывает исходное дерево (`__from`).

---

### 6. **Метод `_M_reset`**
```cpp
void _M_reset() {
  _M_header._M_parent = nullptr; // Нет корня
  _M_header._M_left = _M_header._M_right = _M_header._M_base_ptr(); // Указывают на себя
  _M_node_count = 0; // Размер = 0
}
```
Сбрасывает дерево в начальное состояние:
  - Корень — `nullptr`.
  - `_M_left` и `_M_right` указывают на фиктивный узел (для корректной работы `begin()`/`end()`).
  - Размер обнуляется.

---

### Пример использования в `_Rb_tree`
1. **Вставка первого элемента**:
   ```cpp
   if (_M_impl._M_node_count == 0) {
     _M_header._M_parent = new_node; // Новый узел становится корнем
     _M_header._M_left = _M_header._M_right = new_node; // Минимум и максимум
   }
   ```
2. **Получение `begin()` и `end()`**:
   ```cpp
   iterator begin() noexcept {
     return iterator(_M_header._M_left); // Первый элемент
   }
   iterator end() noexcept {
     return iterator(&_M_header); // Фиктивный узел
   }
   ```

