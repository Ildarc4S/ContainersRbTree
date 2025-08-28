#include <map>

int main() {
  std::map<int, int> a, b;
  a.insert(std::pair<const int, int>(2, 2));
  a.insert(std::pair<const int, int>(1, 2));

  b.insert(std::pair<const int, int>(2, 2));
  b.insert(std::pair<const int, int>(1, 2));
  b.insert(std::pair<const int, int>(3, 2));

  a = b;
  return 0;
}