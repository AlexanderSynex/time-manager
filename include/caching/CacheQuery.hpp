#pragma once

namespace services::auth::cache
{
static constexpr auto cacheQuery
    = "SELECT "
      "auth_schema.tokens.token, "
      "auth_schema.tokens.login,"
      "auth_schema.users.scopes, "
      "auth_schema.users.table_id "
      "FROM auth_schema.tokens "
      "LEFT JOIN auth_schema.users ON "
      "auth_schema.users.login=auth_schema.tokens.login";
}
