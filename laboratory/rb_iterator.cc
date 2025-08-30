#include <bits/stl_tree.h>
#include <iostream>


/*
 *  10
 * 5  15
 *3
 * */

int main() {
  std::_Rb_tree<int,
                int,
                std::_Identity<int>,
                std::less<int>
  > tree;

  tree._M_insert_unique(10);
  tree._M_insert_unique(5);
  tree._M_insert_unique(15);
  tree._M_insert_unique(3);
  tree._M_insert_unique(3);
  tree._M_insert_unique(4);

  std::cout << "Cout:\n";
  for (auto it = tree.begin(); it != tree.end(); ++it) {
    std::cout << *it << " ";
  }
  return 0;
}
