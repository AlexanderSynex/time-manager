#include "services/auth/schemas/BearerSchema.hpp"

#include <string>

using namespace services::auth::schemas::types;

std::optional<std::string>
BearerAuthSchema::extractToken (std::string_view header)
{
  const auto authSchemaPos = header.find (' ');
  if (authSchemaPos == std::string::npos
      or std::string_view{ header.data (), authSchemaPos } != AuthType)
    {
      return {};
    }
  return std::string{ header.data () + authSchemaPos + 1 };
}
