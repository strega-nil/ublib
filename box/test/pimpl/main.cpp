#include <iostream>

#include "pimpl.h"

int main() {
  auto pimpl = pimpl_test(0, 1, "println!");
  auto p2 = pimpl;
  std::cout << pimpl << '\n' << p2 << '\n';
}