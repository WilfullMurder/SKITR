#ifndef SKITR_ERROR_H
#define SKITR_ERROR_H

#include <skitr/skitr_export.h>
#include <string>
#include <optional>
#include <stdexcept>
#include <skitr/core/value.h>

namespace skitter {
    namespace core {
        /**
         * Standard JSON-RPC 2.0 error codes.
         * These are defined in the JSON-RPC 2.0 specification and should be used for all errors that occur during request parsing, validation, or method execution.
         */
        enum class SKITR_EXPORT ErrorCode {
            ParseError = -32700,
            InvalidRequest = -32600,
            MethodNotFound = -32601,
            InvalidParams = -32602,
            InternalError = -32603,
            ServerErrorStart = -32099,
            ServerErrorEnd = -32000
        };

        /**
         * Represents a JSON-RPC 2.0 error object.
         * Contains an error code, a human-readable message, and optional additional data about the error.
         */
        struct SKITR_EXPORT RpcError {
            ErrorCode code{ErrorCode::InternalError}; ///< The error code, which must be one of the standard JSON-RPC 2.0 error codes or a custom server error code in the range -32099 to -32000.
            std::string message; ///< A human-readable message providing more details about the error. This should be a concise description of the error condition.
            std::optional <Value> data; ///< Optional additional data about the error. This can be any JSON value and is intended to provide more context about the error condition.

            RpcError() = default;

            RpcError(ErrorCode c, std::string m, std::optional <Value> d = std::nullopt) : code(c),
                                                                                           message(std::move(m)),
                                                                                           data(std::move(d)) {}
        };

        /**
         * Factory function for creating standard JSON-RPC 2.0 make parse error.
         * @param message A human-readable message describing the error condition. This should be a concise description of the error.
         * @param data Optional additional data about the error. This can be any JSON value and is intended to provide more context about the error condition.
         * @return An RpcError object with the specified error code, message, and optional data.
         */
        inline RpcError makeParseError(const std::string& message, std::optional<Value> data = std::nullopt) {
            return RpcError(ErrorCode::ParseError, message, std::move(data));
        }

        /**
         * Factory function for creating standard JSON-RPC 2.0 invalid request error.
         * @param message A human-readable message describing the error condition. This should be a concise description of the error.
         * @param data Optional additional data about the error. This can be any JSON value and is intended to provide more context about the error condition.
         * @return An RpcError object with the specified error code, message, and optional data.
         */
        inline RpcError makeInvalidRequestError(const std::string& message, std::optional<Value> data = std::nullopt) {
            return RpcError(ErrorCode::InvalidRequest, message, std::move(data));
        }

        /**
         * Factory function for creating standard JSON-RPC 2.0 method not found error.
         * @param message A human-readable message describing the error condition. This should be a concise description of the error.
         * @param data Optional additional data about the error. This can be any JSON value and is intended to provide more context about the error condition.
         * @return An RpcError object with the specified error code, message, and optional data.
         */
        inline RpcError makeMethodNotFoundError(const std::string& message, std::optional<Value> data = std::nullopt) {
            return RpcError(ErrorCode::MethodNotFound, message, std::move(data));
        }

        /**
         * Factory function for creating standard JSON-RPC 2.0 invalid params error.
         * @param message A human-readable message describing the error condition. This should be a concise description of the error.
         * @param data Optional additional data about the error. This can be any JSON value and is intended to provide more context about the error condition.
         * @return An RpcError object with the specified error code, message, and optional data.
         */
        inline RpcError makeInvalidParamsError(const std::string& message, std::optional<Value> data = std::nullopt) {
            return RpcError(ErrorCode::InvalidParams, message, std::move(data));
        }

        /**
         * Factory function for creating standard JSON-RPC 2.0 internal error.
         * @param message A human-readable message describing the error condition. This should be a concise description of the error.
         * @param data Optional additional data about the error. This can be any JSON value and is intended to provide more context about the error condition.
         * @return An RpcError object with the specified error code, message, and optional data.
         */
        inline RpcError makeInternalError(const std::string& message, std::optional<Value> data = std::nullopt) {
            return RpcError(ErrorCode::InternalError, message, std::move(data));
        }

        /**
         * Factory function for creating a custom server error with a code in the range -32099 to -32000.
         * @param code The error code for the server error. Must be in the range -32099 to -32000, inclusive. This allows for up to 100 custom server error codes.
         * @param message A human-readable message describing the error condition. This should be a concise description of the error.
         * @param data Optional additional data about the error. This can be any JSON value and is intended to provide more context about the error condition.
         * @return An RpcError object with the specified error code, message, and optional data.
         */
        inline RpcError makeServerError(int code, const std::string& message, std::optional<Value> data = std::nullopt) {
            if (code < static_cast<int>(ErrorCode::ServerErrorStart) || code > static_cast<int>(ErrorCode::ServerErrorEnd)) {
                throw std::invalid_argument("Server error code must be between -32099 and -32000");
            }
            return RpcError(static_cast<ErrorCode>(code), message, std::move(data));
        }
    } // namespace core
} // namespace skitter



#endif //SKITR_ERROR_H
