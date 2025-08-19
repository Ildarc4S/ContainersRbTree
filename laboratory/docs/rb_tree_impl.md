# _Rb_tree_impl

## **Назначение класса `_Rb_tree_impl`**
Это **базовый класс для хранения состояния красно-черного дерева** (`_Rb_tree`), который объединяет:
1. **Аллокатор** (`_Node_allocator`).
2. **Компаратор** (`_Rb_tree_key_compare`).
3. **Заголовок дерева** (`_Header_t`).

---
```cpp
template<typename _Key_compare>
struct _Rb_tree_impl
  : public _Node_allocator
  , public _Rb_tree_key_compare<_Key_compare>
  , public _Header_t {
  
  typedef _Rb_tree_key_compare<_Key_compare> _Base_key_compare;

  _Rb_tree_impl()
    _GLIBCXX_NOEXCEPT_IF(
    is_nothrow_default_constructible<_Node_allocator>::value
    && is_nothrow_default_constructible<_Base_key_compare>::value )
  : _Node_allocator() {
  }

  _Rb_tree_impl(const _Rb_tree_impl& __x)
  : _Node_allocator(_Node_alloc_traits::_S_select_on_copy(__x))
  , _Base_key_compare(__x._M_key_compare)
  , _Header_t() {
  }

  _Rb_tree_impl(_Rb_tree_impl&&)
    noexcept( is_nothrow_move_constructible<_Base_key_compare>::value )
  = default;

  explicit
  _Rb_tree_impl(_Node_allocator&& __a)
  : _Node_allocator(std::move(__a)) {
  }

  _Rb_tree_impl(_Rb_tree_impl&& __x, _Node_allocator&& __a)
  : _Node_allocator(std::move(__a)),
    _Base_key_compare(std::move(__x)),
    _Header_t(std::move(__x)) {
  }

  _Rb_tree_impl(const _Key_compare& __comp, _Node_allocator&& __a)
  : _Node_allocator(std::move(__a)), _Base_key_compare(__comp) {
  }
};
```

## **Пошаговый разбор**

### **1. Наследование**
```cpp
struct _Rb_tree_impl
  : public _Node_allocator
  , public _Rb_tree_key_compare<_Key_compare>
  , public _Header_t {
```
- **`_Node_allocator`** — аллокатор для узлов (например, `std::allocator`).
- **`_Rb_tree_key_compare`** — обёртка над компаратором (хранит функцию сравнения ключей).
- **`_Header_t`** — содержит указатели на корень (`_M_parent`), крайние узлы (`_M_left`, `_M_right`) и размер (`_M_node_count`).

---

### **2. Конструкторы**

**a) Конструктор по умолчанию**  
```cpp
_Rb_tree_impl()
  _GLIBCXX_NOEXCEPT_IF( /* проверка на noexcept */ )
: _Node_allocator() {
}
```
- Создаёт пустое дерево.
- `_GLIBCXX_NOEXCEPT_IF` — гарантирует `noexcept`, если аллокатор и компаратор могут быть созданы без исключений.

**b) Конструктор копирования**  
```cpp
_Rb_tree_impl(const _Rb_tree_impl& __x)
: _Node_allocator(_Node_alloc_traits::_S_select_on_copy(__x))
, _Base_key_compare(__x._M_key_compare)
, _Header_t() {
}
```
- Копирует аллокатор (через `_S_select_on_copy`) и компаратор.
- Заголовок (`_Header_t`) инициализируется как пустой (копирование узлов делается отдельно).

**c) Конструктор перемещения**  
```cpp
_Rb_tree_impl(_Rb_tree_impl&&) noexcept = default;
```
- Перемещает аллокатор, компаратор и заголовок (генерируется автоматически).

**d) Конструкторы с аллокатором**  
```cpp
explicit _Rb_tree_impl(_Node_allocator&& __a)
  : _Node_allocator(std::move(__a)) {}

_Rb_tree_impl(_Rb_tree_impl&& __x, _Node_allocator&& __a)
  : _Node_allocator(std::move(__a)),
    _Base_key_compare(std::move(__x)),
    _Header_t(std::move(__x)) {}
```
- Позволяют создать дерево с указанным аллокатором.
- Используются при перемещении с изменением аллокатора.

**e) Конструктор с компаратором и аллокатором**  
```cpp
_Rb_tree_impl(const _Key_compare& __comp, _Node_allocator&& __a)
  : _Node_allocator(std::move(__a)), _Base_key_compare(__comp) {}
```
- Задаёт компаратор и аллокатор явно.

---

### **3. Зачем нужен этот класс?**
1. **Инкапсуляция состояния**  
   Все данные дерева (аллокатор, компаратор, узлы) хранятся в одном месте.

2. **Гибкость**  
   Поддерживает:
   - Разные аллокаторы (через `_Node_allocator`).
   - Пользовательские компараторы (через `_Key_compare`).
   - Перемещение и копирование.

3. **Оптимизация**  
   - `noexcept`-конструкторы позволяют оптимизировать перемещение.
   - Отдельное хранение заголовка (`_Header_t`) ускоряет доступ к корню и размера.

