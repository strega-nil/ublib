#pragma once

#include <iosfwd>
#include <ublib-box/box.h>

class pimpl_test {
  class pimpl;

  ublib::box<pimpl> underlying;

public:
  explicit pimpl_test(int x, int y, std::string name);
  friend std::ostream& operator<<(std::ostream&, pimpl_test const&);

  pimpl_test(pimpl_test const&);
  pimpl_test(pimpl_test&&);
  pimpl_test& operator=(pimpl_test const&);
  pimpl_test& operator=(pimpl_test&&);
  ~pimpl_test();
};