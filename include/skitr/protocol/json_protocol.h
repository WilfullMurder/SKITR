#ifndef SKITR_JSON_PROTOCOL_H
#define SKITR_JSON_PROTOCOL_H

#include <skitr/skitr_export.h>
#include <skitr/core/message.h>

#include <string>
#include <optional>

namespace skitter {
    namespace protocol {

        /**
         * Serializes an RpcRequest to a JSON string according to the JSON-RPC 2.0 specification.
         * @param req The RpcRequest to serialize. This should be a valid RpcRequest object containing the method name, parameters, and optional id.
         * @return A JSON string representing the RpcRequest, which can be sent over the wire to a JSON-RPC server.
         */
        SKITR_EXPORT std::string to_json(const core::RpcRequest& req);

        /**
         * Deserializes a JSON string into an RpcRequest object according to the JSON-RPC 2.0 specification.
         * @param src The JSON string to deserialize. This should be a valid JSON string representing a JSON-RPC request object.
         * @param out_error An optional pointer to a string where an error message will be written if deserialization fails.
         * @return An optional RpcRequest object. If deserialization is successful and the JSON string represents a valid JSON-RPC request, the optional will contain the RpcRequest. If deserialization fails or the JSON string does not represent a valid JSON-RPC request, the optional will be empty and out_error will contain a descriptive error message if out_error is not null.
         */
        SKITR_EXPORT std::optional<core::RpcRequest> from_json_request(const std::string& src, std::string* out_error = nullptr);

        /**
         * Serializes an RpcResponse to a JSON string according to the JSON-RPC 2.0 specification.
         * @param res The RpcResponse to serialize. This should be a valid RpcResponse object containing either a result or an error, and an optional id.
         * @return A JSON string representing the RpcResponse, which can be sent over the wire to a JSON-RPC client.
         */
        SKITR_EXPORT std::string to_json(const core::RpcResponse& res);

        /**
         * Deserializes a JSON string into an RpcResponse object according to the JSON-RPC 2.0 specification.
         * @param src The JSON string to deserialize. This should be a valid JSON string representing a JSON-RPC response object.
         * @param out_error An optional pointer to a string where an error message will be written if deserialization fails.
         * @return An optional RpcResponse object. If deserialization is successful and the JSON string represents a valid JSON-RPC response, the optional will contain the RpcResponse. If deserialization fails or the JSON string does not represent a valid JSON-RPC response, the optional will be empty and out_error will contain a descriptive error message if out_error is not null.
         */
        SKITR_EXPORT std::optional<core::RpcResponse> from_json_response(const std::string& src, std::string* out_error = nullptr);

    } // namespace protocol
} // namespace skitter


#endif //SKITR_JSON_PROTOCOL_H
