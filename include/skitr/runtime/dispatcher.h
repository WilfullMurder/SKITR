#ifndef SKITR_DISPATCHER_H
#define SKITR_DISPATCHER_H

#include <skitr/skitr_export.h>
#include <skitr/core/message.h>
#include <skitr/registry/method_registry.h>

namespace skitter {
    namespace core {

        /**
         * The Dispatcher class is responsible for routing incoming RPC requests to the appropriate method handlers registered in a MethodRegistry.
         */
        class SKITR_EXPORT Dispatcher {
        public:
            explicit Dispatcher(registry::MethodRegistry& registry) noexcept;
            virtual ~Dispatcher() = default;

            /**
             * Dispatches the given RPC call to the appropriate method handler in the registry and returns the response.
             * @param req The RPC request to dispatch. This should be a valid RpcRequest object containing the method name and parameters.
             * @return An RpcResponse object representing the result of the method call.
             */
            RpcResponse dispatch(const RpcRequest& req) noexcept;

        private:
            registry::MethodRegistry& registry_; ///< A reference to the MethodRegistry containing the registered method handlers. The Dispatcher does not take ownership of the registry and assumes it will remain valid for the lifetime of the Dispatcher.
        };
    } // namespace core
} // namespace skitter

#endif //SKITR_DISPATCHER_H
