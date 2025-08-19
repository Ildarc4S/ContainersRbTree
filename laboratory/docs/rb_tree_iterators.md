# Итераторы в STL _Rb_tree

Итераторы представлены в виде структур `_Iterator`,`_Rb_tree_iterator`,`_Rb_tree_const_iterator`. В `_Rb_tree` итераторы двунаправленные(bidirectional), поддерживающие:
- Перемещение вперед (operator++)
- Перемещение назад (operator--)
- Доступ к элементам (operator* и operator->)

Весь код:
```cpp
template<bool _Const, typename _ValPtr>
struct _Iterator {
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
  : _M_node() {
  }

  constexpr explicit
  _Iterator(_Base_ptr __x) noexcept
  : _M_node(__x) {
  }

  _Iterator(const _Iterator&) = default;
  _Iterator& operator=(const _Iterator&) = default;

  constexpr
  _Iterator(const _Iterator<false, _ValPtr>& __it) requires _Const
  : _M_node(__it._M_node) {
  }

  [[__nodiscard__]]
  reference operator*() const noexcept { 
    return *static_cast<_Node&>(*_M_node)._M_valptr();
  }

  [[__nodiscard__]]
  pointer operator->() const noexcept { 
    return static_cast<_Node&>(*_M_node)._M_valptr();
  }

  _GLIBCXX14_CONSTEXPR
  _Iterator& operator++() noexcept {
    if (_M_node->_M_right) {
      _M_node = _M_node->_M_right;
      while (_M_node->_M_left) {
        _M_node = _M_node->_M_left;
      }
    } else {
      _Base_ptr __y = _M_node->_M_parent;
      while (_M_node == __y->_M_right) {
        _M_node = __y;
        __y = __y->_M_parent;
      }
      if (_M_node->_M_right != __y) {
        _M_node = __y;
      }
    }

    return *this;
  }

  _GLIBCXX14_CONSTEXPR
  _Iterator operator++(int) noexcept {
    _Iterator __tmp(this->_M_node);
    ++*this;
    return __tmp;
  }

  _GLIBCXX14_CONSTEXPR
  _Iterator& operator--() noexcept {
    if (_M_node->_M_color == _S_red
        && _M_node->_M_parent->_M_parent == _M_node) {
      _M_node = _M_node->_M_right;
    } else if (_M_node->_M_left) {
      _Base_ptr __y = _M_node->_M_left;
      while (__y->_M_right) {
        __y = __y->_M_right;
      }
      _M_node = __y;
    } else {
      _Base_ptr __y = _M_node->_M_parent;
      while (_M_node == __y->_M_left) {
        _M_node = __y;
        __y = __y->_M_parent;
      }
      _M_node = __y;
    }
    return *this;
  }

  _GLIBCXX14_CONSTEXPR
  _Iterator operator--(int) noexcept {
    _Iterator __tmp(this->_M_node);
    --*this;
    return __tmp;
  }

  [[__nodiscard__]]
  friend bool 
  operator==(const _Iterator& __x, const _Iterator& __y) _GLIBCXX_NOEXCEPT {   
    return __x._M_node == __y._M_node; 
  }

#if ! __cpp_lib_three_way_comparison
  [[__nodiscard__]]
  friend bool
  operator!=(const _Iterator& __x, const _Iterator& __y) _GLIBCXX_NOEXCEPT {
    return __x._M_node != __y._M_node; 
  }
#endif

  _Base_ptr _M_node;
};
```

## Шаблонные параметры
```cpp
template<bool _Const, typename _ValPtr>
```
- `_Const`: определяет константность итератора (true - константный)
- `_ValPtr`: тип указателя на значение (может быть умным указателем)

## Вспомогательные определения

### 1. `__maybe_const` — условный const-квалификатор
```cpp
template<typename _Tp>
using __maybe_const = __conditional_t<_Const, const _Tp, _Tp>;
```
Это вспомогательный псевдоним (alias), который добавляет `const` к типу `_Tp`, если шаблонный параметр `_Const` равен `true`. Если `_Const` — `false`, тип остаётся неизменным.

**Пример:**  
Для `_Const = true`:
- `__maybe_const<int>` → `const int`  
Для `_Const = false`:
- `__maybe_const<int>` → `int`

**Зачем нужно?**  
Позволяет одним шаблоном определять как константные, так и неконстантные итераторы, избегая дублирования кода.

---

### 2. `__ptr_traits` — traits-класс для указателя
```cpp
using __ptr_traits = pointer_traits<_ValPtr>;
```
Создаёт псевдоним для `std::pointer_traits<_ValPtr>`. Этот traits-класс предоставляет единый интерфейс для работы с указателями, включая:
- Обычные указатели (`T*`)
- Умные указатели (например, `allocator::pointer`)

**Методы `pointer_traits`**:
- `element_type` — тип элемента (аналог `T` для `T*`)
- `pointer_to` — создаёт указатель на объект
- `rebind` — меняет тип элемента (аналог `allocator::rebind`)

**Зачем нужно?**  
Унифицирует работу с любыми типами указателей, включая кастомные аллокаторы.

---

### 3. `value_type` — тип элемента
```cpp
using value_type = typename __ptr_traits::element_type;
```
Извлекает тип элемента, на который указывает `_ValPtr`.  
Для `_ValPtr = T*`:
- `value_type` → `T`  
Для умных указателей — зависит от их реализации.

**Пример:**  
Если `_ValPtr` — это `std::allocator<int>::pointer`, то:
- `value_type` → `int`

**Зачем нужно?**  
Определяет тип значения, хранящегося в узле дерева. Это обязательный тип для итераторов (требуется стандартом C++).

---

### 4. `reference` — тип ссылки
```cpp
using reference = __maybe_const<value_type>&;
```
Определяет тип ссылки на элемент:
- Если `_Const = true` → `const value_type&`  
- Если `_Const = false` → `value_type&`

**Пример:**  
Для `value_type = std::string`:
- Константный итератор: `reference` → `const std::string&`  
- Неконстантный: `reference` → `std::string&`

**Зачем нужно?**  
Определяет тип, возвращаемый `operator*()`. Соответствует стандартным требованиям к итераторам.

---

### 5. `pointer` — тип указателя
```cpp
using pointer = __maybe_const<value_type>*;
```
Аналогично `reference`, но для указателей:
- Если `_Const = true` → `const value_type*`  
- Если `_Const = false` → `value_type*`

**Пример:**  
Для `value_type = double`:
- Константный итератор: `pointer` → `const double*`  
- Неконстантный: `pointer` → `double*`

**Зачем нужно?**  
Определяет тип, возвращаемый `operator->()`. Также требуется стандартом.

---

### 6. `iterator_category` — категория итератора
```cpp
using iterator_category = bidirectional_iterator_tag;
```
Указывает, что это **двунаправленный итератор** (может перемещаться как вперёд, так и назад, но не поддерживает произвольный доступ).

**Зачем нужно?**  
Позволяет алгоритмам (например, `std::distance`) оптимизировать работу на основе возможностей итератора.

---

### 7. `difference_type` — тип для расстояния
```cpp
using difference_type = ptrdiff_t;
```
Определяет тип для хранения "расстояния" между итераторами (обычно `ptrdiff_t` из `<cstddef>`).

**Пример использования:**  
```cpp
_Rb_tree::iterator it1, it2;
difference_type dist = it2 - it1; // Количество элементов между ними
```

**Зачем нужно?**  
Требуется стандартом для всех итераторов. Используется в алгоритмах типа `std::distance`.

---

### Итоговая таблица

| Строка кода | Назначение | Пример для `_Const=true`, `_ValPtr=int*` |
|-------------|------------|------------------------------------------|
| `template<typename _Tp> using __maybe_const = ...` | Условный `const` | `__maybe_const<int>` → `const int` |
| `using __ptr_traits = pointer_traits<_ValPtr>;` | Traits для указателя | `__ptr_traits<int*>` |
| `using value_type = ...` | Тип элемента | `value_type` → `int` |
| `using reference = ...` | Тип ссылки | `reference` → `const int&` |
| `using pointer = ...` | Тип указателя | `pointer` → `const int*` |
| `using iterator_category = ...` | Категория итератора | `bidirectional_iterator_tag` |
| `using difference_type = ...` | Тип расстояния | `ptrdiff_t` |

## Узлы дерева

```cpp
using _Node = __rb_tree::_Node<_ValPtr>;
using _Node_base = __rb_tree::_Node_base<__ptr_rebind<_ValPtr, void>>;
using _Base_ptr = typename _Node_base::_Base_ptr;
```
Определяются следующие типы:
- `_Node`: полный узел дерева (содержит значение)
- `_Node_base`: базовый узел (только структура дерева, без значения)
- `_Base_ptr`: указатель на базовый узел

## Конструкторы

```cpp
_Iterator() noexcept : _M_node() { }

constexpr explicit
_Iterator(_Base_ptr __x) noexcept : _M_node(__x) { }

_Iterator(const _Iterator&) = default;
_Iterator& operator=(const _Iterator&) = default;

constexpr
_Iterator(const _Iterator<false, _ValPtr>& __it) requires _Const
    : _M_node(__it._M_node) { }
```
1. Конструктор по умолчанию - создает невалидный итератор
2. Конструктор из указателя на узел
3. Конструктор копирования и оператор присваивания по умолчанию
4. Конструктор преобразования из неконстантного в константный итератор (только если `_Const == true`, обеспечивается через `requires`. Подробнее в С++20 концепты). Принимает константную ссылку на неконстантный итератор и внутри копирует указатель на узел из исходного итератора.

## Доступ к элементам

```cpp
[[__nodiscard__]]
reference
operator*() const noexcept
{ return *static_cast<_Node&>(*_M_node)._M_valptr(); }

[[__nodiscard__]]
pointer
operator->() const noexcept
{ return static_cast<_Node&>(*_M_node)._M_valptr(); }
```
- `[[__nodiscard__]]` - атрибут С++17 указывающий, что результат функции не может быть проигнорирован.
- `operator*`:
  1. Приводит `_Base_ptr` к ссылке на `_Node`
  2. Получает указатель на значение через `_M_valptr()`
  3. Разыменовывает его
- `operator->` возвращает указатель на значение напрямую

## Инкремент (`operator++`)

```cpp
_GLIBCXX14_CONSTEXPR _Iterator&
operator++() noexcept
{
  if (_M_node->_M_right) {  // Если есть правый потомок
    _M_node = _M_node->_M_right;  // Переходим в него
    while (_M_node->_M_left) {      // И идем до самого левого
      _M_node = _M_node->_M_left;
    }
  } else { // Если правого потомка нет
    _Base_ptr __y = _M_node->_M_parent;
    // Поднимаемся вверх, пока не найдем узел, который мы еще не обработали
    while (_M_node == __y->_M_right) {
      _M_node = __y;
      __y = __y->_M_parent;
    }
    if (_M_node->_M_right != __y) {
      _M_node = __y;
    }
  }
  return *this;
}
```
**Алгоритм:**
1. Если есть правый потомок:
   - Перейти в него
   - Пройти до конца по левым потомкам
2. Если нет правого потомка:
   - Подниматься вверх, пока текущий узел является правым потомком
   - Перейти к родителю

Давайте детально разберём, как работают указатели `_M_node` и `__y` в этом алгоритме инкремента итератора для красно-чёрного дерева.

### Терминология и начальное состояние
- `_M_node` - текущий указатель итератора (изначально указывает на узел, который мы хотим инкрементировать)
- `__y` - временный указатель, используемый для навигации по дереву

### Подробный разбор алгоритма

#### Примеры:
#### Случай 1: Есть правый потомок (`if (_M_node->_M_right)`)
Для узла `B` в дереве:
```
     D
   /   \
  B     F
 / \   / \
A   C E   G
```
- `_M_node` перейдёт от `B` → `C` (правый потомок)
- Так как у `C` нет левых потомков, останавливаемся на `C`

#### Случай 2: Нет правого потомка (`else`)
Для узла `C` в том же дереве:
1. Начальное состояние: `_M_node` = `C`, `__y` = `B` (родитель)
2. `C` является правым потомком `B` → поднимаемся:
   - `_M_node` = `B`
   - `__y` = `D`
3. `B` - левый потомок `D` → цикл прекращается
4. Поскольку `B->_M_right` (`C`) != `D` → `_M_node` = `D`

#### Доп. пример:
Рассмотрим два последовательных вызова `operator++` начиная с узла `E` в дереве:

```
       D
     /   \
    B     F
   / \   / \
  A   C E   G
```

##### Первый `++` (из `E` → `F`)

1. Начальное состояние: `_M_node` = `E`
2. У `E` нет правого потомка → переходим в `else`
3. `__y` = `F` (родитель `E`)
4. `E` - левый потомок `F` → условие `while` (`_M_node == __y->_M_right`) не выполняется
5. Проверка `if (_M_node->_M_right != __y)`:
   - `E->_M_right` = `nullptr` ≠ `F` → условие выполняется
6. `_M_node` = `F` (результат первого инкремента)

##### Второй `++` (из `F` → `G`)

1. Теперь `_M_node` = `F`
2. У `F` есть правый потомок `G` → переходим в `if`
3. `_M_node` = `G` (правый потомок `F`)
4. У `G` нет левых потомков → цикл `while` не выполняется
5. Результат: `_M_node` остаётся `G` (конечный результат)

##### Итоговая последовательность:
`E` → (`++`) → `F` → (`++`) → `G`

#### Особые случаи:
1. **End-итератор**: Если `_M_node` указывает на end-узел (обычно обозначается специальным образом), поведение не определено

2. **Корневой узел**: Для корня без правого потомка `__y` дойдёт до end-узла


## Постфиксный инкремент

```cpp
_GLIBCXX14_CONSTEXPR 
_Iterator operator++(int) noexcept {
  _Iterator __tmp(this->_M_node);
  ++*this;
  return __tmp;
}
```
Стандартная реализация:
1. Сохраняет текущее состояние
2. Вызывает префиксный инкремент
3. Возвращает сохраненное состояние

## Декремент (`operator--`)
```cpp
_GLIBCXX14_CONSTEXPR 
_Iterator& operator--() noexcept {
  // Часть 1: Обработка end-итератора
  if (_M_node->_M_color == _S_red && 
    _M_node->_M_parent->_M_parent == _M_node) {
    _M_node = _M_node->_M_right;
  } else if (_M_node->_M_left) {
  // Часть 2: Есть левый потомок
    _Base_ptr __y = _M_node->_M_left;
    while (__y->_M_right) {
      __y = __y->_M_right;
    }
    _M_node = __y;
  } else {
  // Часть 3: Нет левого потомка
    _Base_ptr __y = _M_node->_M_parent;
    while (_M_node == __y->_M_left) {
      _M_node = __y;
      __y = __y->_M_parent;
    }
    _M_node = __y;
  }
  return *this;
}
```

---

### Часть 1: Обработка end-итератора

```cpp
if (_M_node->_M_color == _S_red && 
    _M_node->_M_parent->_M_parent == _M_node) {
    _M_node = _M_node->_M_right;
}
```

**Что проверяем:**
1. `_M_node->_M_color == _S_red`  
   - End-узел обычно помечается красным цветом (специальное значение)
2. `_M_node->_M_parent->_M_parent == _M_node`  
   - Круговоя ссылка: родитель корня указывает на end-узел

**Что делаем:**
- Переходим к максимальному элементу дерева (`_M_node->_M_right`), так как:
  - `_M_header._M_right` всегда указывает на максимальный элемент
  - Для `end()` нужно получить последний элемент

**Пример:**
```
      [H] (end-узел, красный)
       / 
      D (корень)
     / \
    B   F
```
`--end()` перейдёт к `F` (максимальный элемент).

---

### Часть 2: Есть левый потомок

```cpp
else if (_M_node->_M_left) {
  _Base_ptr __y = _M_node->_M_left;
  while (__y->_M_right) {
    __y = __y->_M_right;
  }
  _M_node = __y;
}
```

**Алгоритм:**
1. Переходим в левого потомка (`_M_left`)
2. Идём до крайнего правого узла в этом поддереве

**Почему?**  
Предыдущий элемент — это максимальный элемент в левом поддереве.

**Пример:**
Для узла `D`:
```
      D
     / \
    B   F
   / \
  A   C
```
1. Переходим в левого потомка (`B`)
2. Идём до крайнего правого в поддереве `B` → `C`
3. Результат: `--D` → `C`

---

### Часть 3: Нет левого потомка

```cpp
else {
  _Base_ptr __y = _M_node->_M_parent;
  while (_M_node == __y->_M_left) {
    _M_node = __y;
    __y = __y->_M_parent;
  }
  _M_node = __y;
}
```

**Алгоритм:**
1. Поднимаемся вверх, пока текущий узел — **левый** потомок
2. Как только находим узел, где мы — **правый** потомок, останавливаемся
3. Переходим к этому родителю

**Почему?**  
Если левого поддерева нет, предыдущий элемент — это первый родитель, для которого мы находимся в правом поддереве.

**Пример:**
Для узла `E`:
```
      D
     / \
    B   F
       /
      E
```
1. `E` — левый потомок `F` → поднимаемся до `F`
2. `F` — правый потомок `D` → останавливаемся
3. Результат: `--E` → `D`

---

### Полный пример обхода

Рассмотрим дерево:
```
        D
      /   \
     B     F
    / \   / \
   A   C E   G
```

**Последовательность `operator--`:**

1. `--G` → `F` (из Части 3: переход вверх не заходя в цикл)
    
2. `--F` → `E` (из Части 2: левое поддерево `F` → `E`)

3. `--E` → `D` (из Части 3: подъём до первого родителя, где мы в правом поддереве)

4. `--D` → `C` (из Части 2: левое поддерево `D` → `B` → max правый = `C`)

---

### Особые случаи

1. **Для `begin()`:**  
   `--begin()` не определён (UB), так как предшественника нет.

2. **Для `end()`:**  
   Срабатывает Часть 1 → переходит к `max` элементу.

3. **Для корня без левого поддерева:**  
   Поднимается до end-узла (по круговой ссылке).

---

```cpp
_GLIBCXX14_CONSTEXPR 
_Iterator operator--(int) noexcept {
  _Iterator __tmp(this->_M_node);
  --*this;
  return __tmp;
}
```
Аналогично постфиксному инкременту - сохраняет состояние, выполняет декремент, возвращает старое состояние.

## Операторы сравнения

```cpp
[[__nodiscard__]]
friend bool
operator==(const _Iterator& __x, const _Iterator& __y) _GLIBCXX_NOEXCEPT
{ return __x._M_node == __y._M_node; }

[[__nodiscard__]]
friend bool
operator!=(const _Iterator& __x, const _Iterator& __y) _GLIBCXX_NOEXCEPT
{ return __x._M_node != __y._M_node; }
```
Сравнение итераторов выполняется по указателям на узлы

## Поле данных

```cpp
_Base_ptr _M_node;
```
Единственное поле - указатель на текущий узел дерева. Все операции итератора работают с этим указателем.

# _Rb_tree_iterator и _Rb_tree_const_iterator 
Это старые версии итераторов, `_Iterator` более современная версия, появившаяся для:
- Поддержки новых возможностей C++ (аллокаторы с fancy pointers)
- Унификации кода для константных и неконстантных итераторов
- Более эффективных специализаций
## **_Rb_tree_iterator:**
```cpp
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

  _Rb_tree_iterator() _GLIBCXX_NOEXCEPT
  : _M_node() { }

  explicit
  _Rb_tree_iterator(_Base_ptr __x) _GLIBCXX_NOEXCEPT
  : _M_node(__x) { }

  reference
  operator*() const _GLIBCXX_NOEXCEPT
  { return *static_cast<_Node_ptr>(_M_node)->_M_valptr(); }

  pointer
  operator->() const _GLIBCXX_NOEXCEPT
  { return static_cast<_Node_ptr>(_M_node)->_M_valptr(); }

  _Rb_tree_iterator&
  operator++() _GLIBCXX_NOEXCEPT
  {
_M_node = _Rb_tree_increment(_M_node);
return *this;
  }

  _Rb_tree_iterator
  operator++(int) _GLIBCXX_NOEXCEPT
  {
_Rb_tree_iterator __tmp = *this;
_M_node = _Rb_tree_increment(_M_node);
return __tmp;
  }

  _Rb_tree_iterator&
  operator--() _GLIBCXX_NOEXCEPT
  {
_M_node = _Rb_tree_decrement(_M_node);
return *this;
  }

  _Rb_tree_iterator
  operator--(int) _GLIBCXX_NOEXCEPT
  {
_Rb_tree_iterator __tmp = *this;
_M_node = _Rb_tree_decrement(_M_node);
return __tmp;
  }

  friend bool
  operator==(const _Rb_tree_iterator& __x,
     const _Rb_tree_iterator& __y) _GLIBCXX_NOEXCEPT
  { return __x._M_node == __y._M_node; }

#if ! __cpp_lib_three_way_comparison
  friend bool
  operator!=(const _Rb_tree_iterator& __x,
     const _Rb_tree_iterator& __y) _GLIBCXX_NOEXCEPT
  { return __x._M_node != __y._M_node; }
#endif

  _Base_ptr _M_node;
};
```


## **_Rb_tree_const_iterator:**
```cpp
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

  _Rb_tree_const_iterator() _GLIBCXX_NOEXCEPT
  : _M_node() { }

  explicit
  _Rb_tree_const_iterator(_Base_ptr __x) _GLIBCXX_NOEXCEPT
  : _M_node(__x) { }

  _Rb_tree_const_iterator(const iterator& __it) _GLIBCXX_NOEXCEPT
  : _M_node(__it._M_node) { }

  reference
  operator*() const _GLIBCXX_NOEXCEPT
  { return *static_cast<_Node_ptr>(_M_node)->_M_valptr(); }

  pointer
  operator->() const _GLIBCXX_NOEXCEPT
  { return static_cast<_Node_ptr>(_M_node)->_M_valptr(); }

  _Rb_tree_const_iterator&
  operator++() _GLIBCXX_NOEXCEPT
  {
_M_node = _Rb_tree_increment(_M_node);
return *this;
  }

  _Rb_tree_const_iterator
  operator++(int) _GLIBCXX_NOEXCEPT
  {
_Rb_tree_const_iterator __tmp = *this;
_M_node = _Rb_tree_increment(_M_node);
return __tmp;
  }

  _Rb_tree_const_iterator&
  operator--() _GLIBCXX_NOEXCEPT
  {
_M_node = _Rb_tree_decrement(_M_node);
return *this;
  }

  _Rb_tree_const_iterator
  operator--(int) _GLIBCXX_NOEXCEPT
  {
_Rb_tree_const_iterator __tmp = *this;
_M_node = _Rb_tree_decrement(_M_node);
return __tmp;
  }

  friend bool
  operator==(const _Rb_tree_const_iterator& __x,
     const _Rb_tree_const_iterator& __y) _GLIBCXX_NOEXCEPT
  { return __x._M_node == __y._M_node; }

#if ! __cpp_lib_three_way_comparison
  friend bool
  operator!=(const _Rb_tree_const_iterator& __x,
     const _Rb_tree_const_iterator& __y) _GLIBCXX_NOEXCEPT
  { return __x._M_node != __y._M_node; }
#endif

  _Base_ptr _M_node;
};
```

