#include <skitr/runtime/dispatcher.h>
#include <skitr/registry/method_registry.h>

#include <exception>
#include <stdexcept>

namespace skitter {
    namespace core{

        Dispatcher::Dispatcher(registry::MethodRegistry& registry) noexcept : registry_(registry) {}

        RpcResponse Dispatcher::dispatch(const RpcRequest& req) noexcept {
            if(req.method.empty()) {
                return RpcResponse{
                    std::nullopt,
                    makeInvalidRequestError("Empty method"),
                    req.id
                };
            }

            if(!registry_.hasMethod(req.method)) {
                return RpcResponse{
                    std::nullopt,
                    makeMethodNotFoundError("Method not found: " + req.method),
                    req.id
                };
            }

            try{
                auto handler = registry_.getHandler(req.method);
                Value result = handler(req.params);
                return RpcResponse{
                    std::make_optional(std::move(result)),
                    std::nullopt,
                    req.id
                };
            }catch (const RpcError& err) {
                return RpcResponse{
                    std::nullopt,
                    err,
                    req.id
                };
            }catch (const std::exception& ex) {
                return RpcResponse{
                    std::nullopt,
                    makeInternalError("Unhandled exception: " + std::string(ex.what())),
                    req.id
                };
            }catch (...) {
                return RpcResponse{
                    std::nullopt,
                    makeInternalError("Unknown exception"),
                    req.id
                };
            }
        }

    } // namespace core
} // namespace skitter
