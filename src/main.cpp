#include <userver/clients/dns/component.hpp>
#include <userver/clients/dns/resolver_fwd.hpp>
#include <userver/components/minimal_server_component_list.hpp>

#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/postgres_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utest/using_namespace_userver.hpp>
#include <userver/utils/daemon_run.hpp>

// Note: this is for the purposes of tests/samples only
#include <userver/utest/using_namespace_userver.hpp>

#include "services/AdministrationService.hpp"
#include "services/GeneralComponents.hpp"
#include "services/WorktimeService.hpp"

int
main (int argc, char *argv[])
{
  auto component_list
      = components::MinimalServerComponentList ()
            .AppendComponentList (services::components::CommonComponents ())
            .Append<services::control_role::AdministrationService> ()
            .Append<services::control_role::WorktimeService> ();

  return utils::DaemonMain (argc, argv, component_list);
}
