#include <map>

int main() {
  std::map<int, int> a;
  a.insert(std::pair<const int, int>(2, 2));
  a.insert(std::pair<const int, int>(1, 2));
  return 0;
}