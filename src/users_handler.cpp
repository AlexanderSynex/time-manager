#include "users_handler.hpp"

#include "add_user.hpp"
#include "info/worker.hpp"

std::string UserAddHandler::HandleRequest(
    server::http::HttpRequest &request,
    server::request::RequestContext & /*request_context*/
) const {
  // Setting Content-Type: text/plain in a microservice response ensures
  // the client interprets it as plain text, preventing misinterpretation or
  // errors. Without this header, the client might assume a different format,
  // such as JSON, HTML or XML, leading to potential processing issues or
  // incorrect handling of the data.
  request.GetHttpResponse().SetContentType(http::content_type::kTextPlain);
  return add_user(Worker{111, "Ivan", "Ivanov"});
}
