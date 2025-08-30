#include <bits/stl_tree.h>
#include <iostream>

struct A {
  int x;
  A(int a) : x(a) {
    std::cout << "Construct " << x << std::endl;
  }
  bool operator<(const A& other) const {
    return x < other.x;
  }
};

int main() {
  std::_Rb_tree<A, A, std::_Identity<A>, std::less<A>, std::allocator<A>> tree;

  tree._M_insert_unique(10);
  //tree._M_insert_unique(A(5));
  //tree._M_insert_unique(A(15));

  for (auto it = tree.begin(); it != tree.end(); ++it) {
    std::cout << it->x << std::endl;
  }
  return 0;
}
