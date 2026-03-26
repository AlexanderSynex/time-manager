#pragma once

#include <optional>
#include <string_view>

namespace services::auth::schemas::types
{
struct BearerAuthSchema
{
  static constexpr std::string_view AuthType = "Bearer";
  static std::optional<std::string> extractToken (std::string_view header);
};
}
