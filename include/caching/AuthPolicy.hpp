#pragma once

#include "CacheQuery.hpp"
#include <functional>
#include <string>
#include <unordered_map>
#include <userver/cache/base_postgres_cache.hpp>
#include <userver/crypto/algorithm.hpp>
#include <userver/server/auth/user_auth_info.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <vector>
namespace services::caching
{

struct AuthInfo
{
  userver::server::auth::UserAuthInfo::Ticket token;
  std::string login;
  std::vector<std::string> scopes = {};
  int table_id;
};

struct AuthCachePolicy
{
  static constexpr std::string_view kName = "auth-cache";

  using ValueType = AuthInfo;
  static constexpr auto kKeyMember = &AuthInfo::token;

  using UpdatedFieldType = userver::storages::postgres::TimePointTz;
  static constexpr const char *kQuery = auth::cache::cacheQuery;
  static constexpr const char *kUpdatedField = "last_update";

  using CacheContainer = std::unordered_map<
      userver::server::auth::UserAuthInfo::Ticket, AuthInfo,
      std::hash<userver::server::auth::UserAuthInfo::Ticket>,
      userver::crypto::algorithm::StringsEqualConstTimeComparator>;
};

using AuthCache = userver::components::PostgreCache<AuthCachePolicy>;

}
