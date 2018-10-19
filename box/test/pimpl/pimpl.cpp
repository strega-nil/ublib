#include "pimpl.h"

#include <string>

class pimpl_test::pimpl {
public:
  int x;
  int y;
  std::string name;
};

pimpl_test::pimpl_test(int x, int y, std::string name)
    : underlying(pimpl{x, y, std::move(name)}) {}

pimpl_test::pimpl_test(pimpl_test const&) = default;
pimpl_test::pimpl_test(pimpl_test&&) = default;
pimpl_test& pimpl_test::operator=(pimpl_test const&) = default;
pimpl_test& pimpl_test::operator=(pimpl_test&&) = default;
pimpl_test::~pimpl_test() = default;

std::ostream& operator<<(std::ostream& os, pimpl_test const& f) {
  auto& p = *f.underlying;
  return os << p.name << "(" << p.x << ", " << p.y << ")";
}