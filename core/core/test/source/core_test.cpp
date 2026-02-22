#include <string>

#include "core/core.hpp"

auto main() -> int
{
  auto const exported = exported_class {};

  return std::string("core") == exported.name() ? 0 : 1;
}
