#include <string>

#include "core/core.hpp"

exported_class::exported_class()
    : m_name {"core"}
{
}

auto exported_class::name() const -> char const*
{
  return m_name.c_str();
}
