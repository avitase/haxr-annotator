#pragma once

#include <string>

#include "core/core_export.hpp"

class CORE_EXPORT exported_class
{
public:
  exported_class();

  auto name() const -> char const*;

private:
  std::string m_name;
};
