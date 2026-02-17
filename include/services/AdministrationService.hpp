#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>

namespace services::control_role {
class AdministrationService final
    : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "administration-service";
    using userver::server::handlers::HttpHandlerJsonBase::HttpHandlerJsonBase;

    Value HandleRequestJsonThrow(const HttpRequest& request,
        const Value& request_json,
        RequestContext& context) const override;

private:
    Value processCreation(const HttpRequest& request,
        const Value& request_json) const;

    Value processModification(const HttpRequest& request,
        const Value& request_json) const;

    bool validateJson(const Value& request_json) const;
};
} // namespace services::control_role