#pragma once

#include "CorsMiddleware.hpp"

#include <userver/server/middlewares/configuration.hpp>

namespace services::core
{
class MiddlewarePipelineBuilder
    : public userver::server::middlewares::PipelineBuilder
{
public:
  using MiddlewaresList = userver::server::middlewares::MiddlewaresList;
  MiddlewaresList
  BuildPipeline (MiddlewaresList userver_middleware_pipeline) const override
  {
    auto &resulting_pipeline = userver_middleware_pipeline;
    const auto &middlewares_to_append = GetMiddlewaresToAppend ();

    resulting_pipeline.insert (resulting_pipeline.end (),
                               middlewares_to_append.begin (),
                               middlewares_to_append.end ());
    resulting_pipeline.push_back (
        std::string{ services::middleware::CorsMiddleware::kName });
    return resulting_pipeline;
  }
};
}