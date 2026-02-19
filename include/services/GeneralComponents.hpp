#pragma once

#include <userver/clients/dns/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>

namespace services::components {
///@brief Набор общих компонент, используемых сервисами
static const auto CommonComponents = []() -> userver::components::ComponentList {
    return userver::components::ComponentList {}
        .Append<userver::clients::dns::Component>()
        .Append<userver::components::TestsuiteSupport>()
        .Append<userver::components::Postgres>("db");
};
}
