#include <ublib/box.h>

#include <iostream>
#include <string>
#include <variant>

struct lambda {
  struct abs {
    std::string v;
    ublib::box<lambda> e;

    explicit abs(std::string v, lambda e) : v(std::move(v)), e(std::move(e)) {}
  };
  struct app {
    ublib::box<lambda> f;
    ublib::box<lambda> a;

    explicit app(lambda f, lambda a) : f(std::move(f)), a(std::move(a)) {}
  };
  struct var {
    std::string v;

    explicit var(std::string v) : v(std::move(v)) {}
  };

  lambda(abs a) : data(std::move(a)) {}
  lambda(app a) : data(std::move(a)) {}
  lambda(var a) : data(std::move(a)) {}

  using underlying_t = std::variant<abs, app, var>;

  underlying_t data;
};

std::ostream& operator<<(std::ostream& os, lambda const& l) {
  struct rec {
    std::ostream& os;
    std::ostream& operator()(lambda::abs const& e) {
      return os << "(fun " << e.v << " -> " << *e.e << ')';
    }
    std::ostream& operator()(lambda::app const& a) {
      return os << *a.f << ' ' << *a.a;
    }
    std::ostream& operator()(lambda::var const& v) {
      return os << v.v;
    }
  };

  return std::visit(rec{os}, l.data);
}

using ublib::box;

int main() {
  lambda prog = lambda::app(
    lambda::abs("x", lambda::var("x")),
    lambda::var("y"));

  auto p2 = prog;
  prog = lambda::var("y");

  std::cout << p2 << '\n';

  return 0;
}