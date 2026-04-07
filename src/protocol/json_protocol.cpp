#include <skitr/protocol/json_protocol.h>

#include <nlohmann/json.hpp>

#include <skitr/core/value.h>
#include <skitr/core/error.h>

using nlohmann::json;

namespace skitter {
    namespace protocol {

        static json valueToJson(const core::Value& v);
        static core::Value jsonToValue(const json& j);


        std::string to_json(const core::RpcRequest& req) {
            json j;
            j["jsonrpc"] = "2.0";
            j["method"] = req.method;
            json params = json::array();

            for (const auto &p: req.params) params.push_back(valueToJson(p));
            j["params"] = std::move(params);

            if(req.id.has_value()) j["id"] = valueToJson(req.id.value());
            return j.dump();
        }

        std::optional<core::RpcRequest> from_json_request(const std::string& src, std::string* out_error) {
            try {
                json j = json::parse(src);
                if (!j.is_object()){
                    if (out_error) *out_error = "Top-level JSON is not an object";
                    return std::nullopt;
                }
                core::RpcRequest req;
                if (!j.contains("method") || !j["method"].is_string()) {
                    if (out_error) *out_error = "Missing or invalid 'method'";
                    return std::nullopt;
                }
                req.method = j["method"].get<std::string>();
                if(req.method.empty()) {
                    if (out_error) *out_error = "'method' cannot be empty";
                    return std::nullopt;
                }

                if(j.contains("params")) {
                    if (!j["params"].is_array()) {
                        if (out_error) *out_error = "'params' must be an array";
                        return std::nullopt;
                    }
                    for (const auto &p: j["params"]) req.params.push_back(jsonToValue(p));
                }
                if(j.contains("id")) {
                    req.id = jsonToValue(j["id"]);
                }else {
                    req.id = std::nullopt;
                }
                return req;
            }catch (const json::parse_error& ex) {
                if (out_error) *out_error = std::string("JSON parse error: ") + ex.what();
                return std::nullopt;
            } catch (const std::exception& ex) {
                if (out_error) *out_error = std::string("Error: ") + ex.what();
                return std::nullopt;
            }catch (...) {
                if (out_error) *out_error = "Unknown error";
                return std::nullopt;
            }
        }

        std::string to_json(const core::RpcResponse& res) {
            json j;
            j["jsonrpc"] = "2.0";
            if (res.result.has_value()) {
                j["result"] = valueToJson(*res.result);
            } else if (res.error.has_value()) {
                json e;
                e["code"] = static_cast<int>(res.error->code);
                e["message"] = res.error->message;
                if (res.error->data.has_value()) e["data"] = valueToJson(*res.error->data);
                j["error"] = std::move(e);
            } else{
                // neither result nor error - produce an object with null result per choice; keep it minimal
                j["result"] = nullptr;
            }

            if(res.id.has_value()) j["id"] = valueToJson(*res.id);
            return j.dump();
        }

        std::optional<core::RpcResponse> from_json_response(const std::string& src, std::string* out_error) {
            try{
                json j = json::parse(src);
                if (!j.is_object()){
                    if (out_error) *out_error = "Top-level JSON is not an object";
                    return std::nullopt;
                }
                core::RpcResponse res;

                bool hasResult = j.contains("result");
                bool hasError = j.contains("error");
                if(hasResult && hasError) {
                    if (out_error) *out_error = "Response cannot have both 'result' and 'error'";
                    return std::nullopt;
                }

                if(hasResult) {
                    res.result = jsonToValue(j["result"]);
                }else if(hasError) {
                    const auto &e = j["error"];
                    if (!e.is_object() || !e.contains("code") || !e.contains("message")){
                        if (out_error) *out_error = "Invalid 'error' object";
                        return std::nullopt;
                    }
                    int code = e["code"].get<int>();
                    std::string msg = e["message"].get<std::string>();
                    std::optional<core::Value> data = std::nullopt;
                    if (e.contains("data")) data = jsonToValue(e["data"]);
                    res.error = core::RpcError{static_cast<core::ErrorCode>(code), std::move(msg), std::move(data)};
                }

                if(j.contains("id")) {
                    res.id = jsonToValue(j["id"]);
                }else {
                    res.id = std::nullopt;
                }

                return res;
            }catch (const json::parse_error& ex) {
                if (out_error) *out_error = std::string("JSON parse error: ") + ex.what();
                return std::nullopt;
            } catch (const std::exception& ex) {
                if (out_error) *out_error = std::string("Error: ") + ex.what();
                return std::nullopt;
            }catch (...) {
                if (out_error) *out_error = "Unknown error";
                return std::nullopt;
            }
        }

        static json valueToJson(const core::Value& v) {
            using core::Type;
            switch (v.type()) {
                case Type::Null:
                    return nullptr;
                case Type::Bool:
                    return v.asBool();
                case Type::Int:
                    return v.asInt();
                case Type::Double:
                    return v.asDouble();
                case Type::String:
                    return v.asString();
                case Type::Array: {
                    json arr = json::array();
                    for (const auto &elem: v.asArray()) arr.push_back(valueToJson(elem));
                    return arr;
                }
                case Type::Object: {
                    json obj = json::object();
                    for (const auto &[key, val]: v.asObject()) obj[key] = valueToJson(val);
                    return obj;
                }
                default:
                    return nullptr; // should never happen
            }
        }

        static core::Value jsonToValue(const json& j) {
            using core::Value;
            if (j.is_null()) return Value();
            if (j.is_boolean()) return Value(j.get<bool>());
            if (j.is_number_integer()) return Value(static_cast<int64_t>(j.get<int64_t>()));
            if (j.is_number_unsigned()) return Value(static_cast<int64_t>(j.get<uint64_t>()));
            if (j.is_number_float()) return Value(j.get<double>());
            if (j.is_string()) return Value(j.get<std::string>());
            if (j.is_array()) {
                Value::array_t arr;
                arr.reserve(j.size());
                for (const auto &el: j) arr.push_back(jsonToValue(el));
                return Value(std::move(arr));
            }
            if (j.is_object()) {
                Value::object_t obj;
                for (auto it = j.begin(); it != j.end(); ++it) {
                    obj.emplace(it.key(), jsonToValue(it.value()));
                }
                return Value(std::move(obj));
            }
            // fallback to null
            return Value();
        }
    } // namespace protocol
} // namespace skitter