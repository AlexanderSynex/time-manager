#pragma once

#include "info/DBInfo.hpp"
#include "middleware/CorsMiddlewareFactory.hpp"
#include "middleware/MiddlewarePipelineBuilder.hpp"
#include <userver/clients/dns/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/server/middlewares/cors.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>

namespace services::components
{
///@brief Набор общих компонент, используемых сервисами
static const auto CommonComponents
    = [] () -> userver::components::ComponentList {
  return userver::components::ComponentList{}
      .Append<userver::clients::dns::Component> ()
      .Append<userver::components::TestsuiteSupport> ()
      .Append<userver::components::Postgres> (db::info::db_name)
      .Append<userver::server::middlewares::CorsFactory> ();
};
}
