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
    using JsonData = userver::formats::json::Value;

public:
    explicit UserService(const userver::components::ComponentContext& context, std::string_view db_service_name);
    UserService(const UserService&) = delete;
    UserService(UserService&&) = delete;

protected:
    userver::storages::postgres::ClusterPtr db() const
    {
        return p_db;
    }

    JsonData find(std::size_t table_id) const;

protected:
    using IDType = std::size_t;

    Worker extract(const JsonData& request) const;
    bool validate(const JsonData& request) const noexcept;

protected:
    userver::storages::postgres::ClusterPtr p_db = nullptr;
};

}