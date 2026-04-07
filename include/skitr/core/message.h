#ifndef SKITR_MESSAGE_H
#define SKITR_MESSAGE_H

#include <string>
#include <vector>
#include <optional>
#include <skitr/core/value.h>
#include <skitr/skitr_export.h>
#include <skitr/core/error.h>

namespace skitter {
    namespace core {

        /**
         * Represents a JSON-RPC 2.0 request or notification. The 'method' field is required and must be a non-empty string.
         * The 'params' field is optional and can be an array (for positional parameters) or an object (for named parameters).
         * For simplicity, we only support positional parameters as a vector of Value.
         * The 'id' field is optional. If it is present, the message is a request and a response with the same 'id' must be sent back.
         * If it is absent, the message is a notification and no response should be sent.
         */
        struct SKITR_EXPORT RpcRequest {
            std::string method; ///< Must be non-empty, case-sensitive key into registry
            std::vector<Value> params; ///< Positional parameters. Can be empty if method takes no parameters.
            std::optional<Value> id; ///< present => response expected; absent => notification

            bool isNotification() const noexcept { return !id.has_value(); }
        };

        /**
         * Represents a JSON-RPC 2.0 response.
         * The 'id' field must be present and must be the same as the 'id' of the corresponding request.
         * Exactly one of 'result' or 'error' must be present. If 'result' is present, it contains the successful result of the method call.
         */
        struct SKITR_EXPORT RpcResponse {
            std::optional<Value> result; ///< present => success; absent => failure
            std::optional<RpcError> error; ///< present => failure; absent => success
            std::optional<Value> id; ///< Must be present and must be the same as the 'id' of the corresponding request

            /**
             * Checks if the response is valid according to JSON-RPC 2.0 rules: exactly one of 'result' or 'error' must be present, and 'id' must be present.
             * @return  true if the response is valid, false otherwise
             * @note    This does not check if 'id' matches any request, as that is the responsibility of the caller.
             */
            bool isValid() const noexcept {
                return (result.has_value() ^ error.has_value());
            }

            /**
             * Creates a successful response with the given result and optional id. If id is not provided, it will be set to null in the JSON output.
             * @param res  The result value to include in the response. Can be any JSON value, including null.
             * @param idv  The id value to include in the response. If not provided, it will be set to null in the JSON output. This allows creating responses for notifications if needed, although typically notifications should not have responses.
             * @return  A RpcResponse object representing a successful response with the given result and id.
             */
            static RpcResponse success(Value res, std::optional<Value> idv = std::nullopt) {
                RpcResponse r;
                r.result = std::move(res);
                r.error = std::nullopt;
                r.id = std::move(idv);
                return r;
            }

            /**
             * Creates a failure response with the given error and optional id. If id is not provided, it will be set to null in the JSON output.
             * @param err  The RpcError object containing the error code, message, and optional data to include in the response.
             * @param idv  The id value to include in the response. If not provided, it will be set to null in the JSON output. This allows creating responses for notifications if needed, although typically notifications should not have responses.
             * @return  A RpcResponse object representing a failure response with the given error and id.
             */
            static RpcResponse failure(RpcError err, std::optional<Value> idv = std::nullopt) {
                RpcResponse r;
                r.result = std::nullopt;
                r.error = std::move(err);
                r.id = std::move(idv);
                return r;
            }
        };
    } // namespace core
} // namespace skitter

#endif //SKITR_MESSAGE_H
