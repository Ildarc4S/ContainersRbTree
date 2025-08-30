# _Alloc_node

`_Alloc_node` нужен для инкапсуляции вызова `_M_create_node` с поддержкой perfect forwarding при операциях вставки и копирования в `_Rb_tree`.**

```cpp
struct _Alloc_node
{
  _Alloc_node(_Rb_tree& __t)
  : _M_t(__t) {
  }

  template<typename _Arg>
  _Node_ptr
  operator()(_GLIBCXX_FWDREF(_Arg) __arg) const { 
    return _M_t._M_create_node(_GLIBCXX_FORWARD(_Arg, __arg));
  }

 private:
  _Rb_tree& _M_t;
};
```
**Замечание:**
- `_GLIBCXX_FWDREF(_Arg)` = `_Argg&&`    
- `_GLIBCXX_FORWARD(_Arg, __arg)` = `std::forward<_Arg>(__arg)`

## **Пошаговый разбор**
### **1. Конструктор `_Alloc_node(_Rb_tree& __t)`**
Принимает ссылку на `_Rb_tree` и сохраняет её в поле `_M_t`.  
```cpp
_Alloc_node(_Rb_tree& __t)
: _M_t(__t) {
}
```
Остальные конструкторы и деструктор отсутствуют. 
---

### **2. Оператор `operator()`**  
**Сигнатура:**  
```cpp
template<typename _Arg>
_Node_ptr
operator()(_GLIBCXX_FWDREF(_Arg) __arg) const { 
  return _M_t._M_create_node(_GLIBCXX_FORWARD(_Arg, __arg));
}
```  

1. Принимает аргумент `__arg` (с perfect forwarding, чтобы сохранить категорию значения — lvalue/rvalue).  
2. Вызывает `_M_t._M_create_node` и передаёт ему `__arg` через `std::forward`.  
3. Возвращает указатель на созданный узел (`_Node_ptr`).  

**Зачем нужен?**  
1. **Упрощает создание узлов**  
   Вместо того чтобы везде писать:  
   ```cpp
   _M_t._M_create_node(std::forward<_Arg>(__arg))
   ```  
   можно просто использовать `_Alloc_node` как функтор:  
   ```cpp
   _Alloc_node creator(tree);
   auto new_node = creator(arg);  // Создаёт узел
   ```  

2. **Гарантирует правильную передачу аргументов**  
   Благодаря `std::forward`, аргументы передаются в `_M_create_node` без лишних копирований (если это rvalue) или с сохранением ссылки (если lvalue).  


# _Auto_node

Это RAII-обёртка, которая гарантирует безопасное создание и автоматическое удаление узла, если его не вставили в дерево.

```cpp
struct _Auto_node {
  template<typename... _Args>
  _Auto_node(_Rb_tree& __t, _Args&&... __args)
  : _M_t(__t),
    _M_node(__t._M_create_node(std::forward<_Args>(__args)...)) {
  }

  _Auto_node(_Auto_node&& __n)
  : _M_t(__n._M_t), _M_node(__n._M_node) { 
    __n._M_node = nullptr; 
  }

  ~_Auto_node() {
    if (_M_node) {
      _M_t._M_drop_node(_M_node);
    }
  }

  const _Key& _M_key() const { 
    return _S_key(_M_node); 
  }

  iterator _M_insert(pair<_Base_ptr, _Base_ptr> __p) {
    auto __it = _M_t._M_insert_node(__p.first, __p.second, _M_node);
    _M_node = nullptr;
    return __it;
  }

  iterator _M_insert_equal_lower() {
    auto __it = _M_t._M_insert_equal_lower_node(_M_node);
    _M_node = nullptr;
    return __it;
  }

  _Rb_tree& _M_t;
  _Node_ptr _M_node;
};
```

## Пошаговый разбор `_Auto_node`

---

### **1. Конструкторы**

**a) Основной конструктор:**
```cpp
template<typename... _Args>
_Auto_node(_Rb_tree& __t, _Args&&... __args)
: _M_t(__t),
  _M_node(__t._M_create_node(std::forward<_Args>(__args)...)) {
}
```
- Создает узел дерева с переданными аргументами `__args...` через `_M_create_node`.  
- Использует perfect forwarding (`std::forward`), чтобы корректно передать lvalue/rvalue.  
- Сохраняет ссылку на дерево `_M_t` и созданный узел `_M_node`.  

**b) Конструктор перемещения:**
```cpp
_Auto_node(_Auto_node&& __n)
: _M_t(__n._M_t), _M_node(__n._M_node) { 
  __n._M_node = nullptr; 
}
```
- "Перехватывает" владение узлом у другого `_Auto_node` (`__n`).  
- После перемещения исходный объект `__n` теряет узел (`_M_node = nullptr`), чтобы его деструктор не удалил узел.  
- Нужен для поддержки семантики перемещения. 
---

### **2. Деструктор**
```cpp
~_Auto_node() {
  if (_M_node) {
    _M_t._M_drop_node(_M_node);
  }
}
```
- Если узел не был вставлен в дерево (т.е. `_M_node != nullptr`), удаляет его через `_M_drop_node`.  
- Гарантирует, что неудачно созданный узел не останется в памяти (RAII).  
---

### **3. Метод `_M_key()`**
```cpp
const _Key& _M_key() const { 
  return _S_key(_M_node); 
}
```
- Возвращает ключ узла, вызывая статический метод `_S_key` (аналог `_Rb_tree::_S_key`).  
- Позволяет получить ключ узла без прямого доступа к его полям (инкапсуляция).

---

### **4. Метод `_M_insert()`**
```cpp
iterator _M_insert(pair<_Base_ptr, _Base_ptr> __p) {
  auto __it = _M_t._M_insert_node(__p.first, __p.second, _M_node);
  _M_node = nullptr;
  return __it;
}
```
- Вставляет узел `_M_node` в дерево на позицию, заданную парой `(__p.first, __p.second)`.  
- После вставки обнуляет `_M_node`, чтобы деструктор не удалил узел.  
- Возвращает итератор на вставленный элемент.  

---

### **5. Метод `_M_insert_equal_lower()`**
```cpp
iterator _M_insert_equal_lower() {
  auto __it = _M_t._M_insert_equal_lower_node(_M_node);
  _M_node = nullptr;
  return __it;
}
```
- Вставляет узел `_M_node` в "нижнюю" позицию для неуникальных ключей (например, в `multiset`).  
- **Отличие от `_M_insert`:**  
  Использует `_M_insert_equal_lower_node` вместо `_M_insert_node` (логика для дубликатов ключей).  

---

### **6. Поля класса**
```cpp
_Rb_tree& _M_t;  // Ссылка на дерево
_Node_ptr _M_node; // Указатель на узел
```
- **`_M_t`:**  
  Нужна для вызова методов дерева (`_M_create_node`, `_M_drop_node` и т.д.).  
- **`_M_node`:**  
  Хранит созданный узел до момента вставки. Если `nullptr` — узел уже вставлен или перемещён.

---

### Пример использования
```cpp
// Внутри метода emplace():
_Auto_node new_node(*this, std::forward<Args>(args)...); // Создаём узел
auto pos = _M_get_insert_pos(new_node._M_key());
if (pos.second) {
  return new_node._M_insert(pos); // Вставляем и передаём владение
}
return iterator(pos.first); // Узел удалится в деструкторе new_node
```

