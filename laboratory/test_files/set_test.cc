#include <set>

int main() {
  std::set<int> s1 = {1, 2, 3};
  std::set<int> s2 = {3,4, 5};

  s1.merge(s2);

  return 0;
}