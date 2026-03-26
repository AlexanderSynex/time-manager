#pragma once

#include "services/auth/AuthChecker.hpp"
#include "services/auth/schemas/BearerSchema.hpp"

namespace services::auth::schemas
{
using AuthBearerChecker = AuthChecker<types::BearerAuthSchema>;
}
