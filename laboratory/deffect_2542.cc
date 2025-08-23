//=====================================================================\\
// Дефект LWG 2542:                                                    \\
// Missing const requirements for associative containers               \\
// Компаратор должен быть вызываем с const Key& аргументами            \\
// В версиях до C++11 это могло компилироваться                        \\
// В C++11 и новее должен срабатывать static_assert с сообщением:      \\
// "comparison object must be invocable with two arguments of key type"\\
//=====================================================================\\

#include <set>

struct BadComparator {
  bool operator()(int& a, int& b) {  // ПРОБЛЕМА: неконстантные ссылки
    return a < b;                    // Компаратор должен принимать const&
  }
};

int main() {
  std::set<int, BadComparator> bad_set;  // Нарушение требований стандарта

  // Эта операция требует вызова компаратора с const int& аргументами,
  // но BadComparator принимает только int&, что приводит к ошибке компиляции
  // или неопределенному поведению в старых версиях
  bad_set.insert(1);
  
  return 0;
}
