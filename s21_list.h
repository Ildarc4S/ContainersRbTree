#ifndef S21_LIST_H
#define S21_LIST_H

#include <iostream>

namespace s21 {

/*
 * @class list
 * @brief Реализация контейнера list аналогичного std::list
 */
template <typename T>
class list {
  // Реализация контейнера list
 public:
  list() = default;
  explicit list(int inum) { std::cout << "A"; }
};

}  // namespace s21

#endif  // S21_LIST_H
