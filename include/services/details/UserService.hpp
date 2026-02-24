#pragma once

#include "info/worker.hpp"
#include <cstddef>
#include <string_view>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>

namespace services::control_role::details {
class UserService {
public:
    explicit UserService(const userver::components::ComponentContext& context, std::string_view db_service_name);
    UserService(const UserService&) = delete;
    UserService(UserService&&) = delete;

protected:
    userver::storages::postgres::ClusterPtr db() const
    {
        return p_db;
    }

protected:
    using IDType = std::size_t;

    Worker extract(const userver::formats::json::Value& request_json) const;
    bool validate(const userver::formats::json::Value& request_json) const noexcept;

protected:
    userver::storages::postgres::ClusterPtr p_db = nullptr;
};

}