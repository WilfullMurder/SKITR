#include <gtest/gtest.h>
#include <skitr/protocol/json_protocol.h>
#include <skitr/core/value.h>
#include <skitr/core/message.h>
#include <skitr/core/error.h>

using skitter::protocol::to_json;
using skitter::protocol::from_json_request;
using skitter::protocol::from_json_response;
using skitter::core::Value;
using skitter::core::RpcRequest;
using skitter::core::RpcResponse;
using skitter::core::RpcError;
using skitter::core::ErrorCode;

TEST(JsonProtocolTests, RequestRoundtripPreservesMethodParamsAndId) {
    RpcRequest req;
    req.method = "complex";
    req.params = {
            Value(),                      // null
            Value(true),                  // bool
            Value(int64_t(42)),           // int
            Value(3.14),                  // double
            Value(std::string("str")),    // string
            Value(std::vector<Value>{ Value(int64_t(1)), Value(std::string("x")) }), // array
            Value(std::map<std::string, Value>{{"k", Value(int64_t(7))}}) // object
    };
    req.id = Value(int64_t(123));

    std::string out = to_json(req);
    std::string err;
    auto parsed = from_json_request(out, &err);

    ASSERT_TRUE(parsed.has_value()) << "Parsing failed: " << err;
    EXPECT_EQ(parsed->method, "complex");
    ASSERT_TRUE(parsed->id.has_value());
    EXPECT_EQ(parsed->id->asInt(), 123);
    ASSERT_EQ(parsed->params.size(), req.params.size());

    EXPECT_TRUE(parsed->params[0].isNull());
    EXPECT_TRUE(parsed->params[1].isBool());
    EXPECT_TRUE(parsed->params[2].isInt());
    EXPECT_EQ(parsed->params[2].asInt(), 42);
    EXPECT_TRUE(parsed->params[3].isDouble());
    EXPECT_DOUBLE_EQ(parsed->params[3].asDouble(), 3.14);
    EXPECT_TRUE(parsed->params[4].isString());
    EXPECT_EQ(parsed->params[4].asString(), "str");
    EXPECT_TRUE(parsed->params[5].isArray());
    EXPECT_EQ(parsed->params[5].asArray().at(0).asInt(), 1);
    EXPECT_TRUE(parsed->params[6].isObject());
    EXPECT_EQ(parsed->params[6].asObject().at("k").asInt(), 7);
}

TEST(JsonProtocolTests, RequestParsingInvalidJsonReturnsError) {
    std::string err;
    auto res = from_json_request("{invalid json", &err);
    EXPECT_FALSE(res.has_value());
    EXPECT_FALSE(err.empty());
    EXPECT_NE(err.find("JSON parse error"), std::string::npos);
}

TEST(JsonProtocolTests, RequestMissingMethodReturnsInvalid) {
    std::string src = R"({"jsonrpc":"2.0","params":[]})";
    std::string err;
    auto res = from_json_request(src, &err);
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(err, "Missing or invalid 'method'");
}

TEST(JsonProtocolTests, RequestWithoutParamsIsAcceptedWithEmptyParams) {
    std::string src = R"({"jsonrpc":"2.0","method":"m"})";
    std::string err;
    auto res = from_json_request(src, &err);
    ASSERT_TRUE(res.has_value()) << err;
    EXPECT_EQ(res->method, "m");
    EXPECT_EQ(res->params.size(), 0u);
    EXPECT_FALSE(res->id.has_value());
}

TEST(JsonProtocolTests, ResponseRoundtripSuccessAndErrorPreserved) {
    // success
    auto successRes = RpcResponse::success(Value(std::string("ok")), Value(int64_t(7)));
    std::string s = to_json(successRes);
    std::string err;
    auto parsedS = from_json_response(s, &err);
    ASSERT_TRUE(parsedS.has_value()) << err;
    EXPECT_TRUE(parsedS->result.has_value());
    EXPECT_FALSE(parsedS->error.has_value());
    EXPECT_EQ(parsedS->result->asString(), "ok");
    ASSERT_TRUE(parsedS->id.has_value());
    EXPECT_EQ(parsedS->id->asInt(), 7);

    // error
    RpcError e = skitter::core::makeMethodNotFoundError("not found");
    auto errRes = RpcResponse::failure(e, Value(int64_t(9)));
    std::string es = to_json(errRes);
    auto parsedE = from_json_response(es, &err);
    ASSERT_TRUE(parsedE.has_value()) << err;
    EXPECT_FALSE(parsedE->result.has_value());
    ASSERT_TRUE(parsedE->error.has_value());
    EXPECT_EQ(parsedE->error->code, ErrorCode::MethodNotFound);
    EXPECT_EQ(parsedE->error->message, "not found");
    ASSERT_TRUE(parsedE->id.has_value());
    EXPECT_EQ(parsedE->id->asInt(), 9);
}

TEST(JsonProtocolTests, ResponseWithBothResultAndErrorIsRejected) {
    std::string src = R"({"jsonrpc":"2.0","result":null,"error":{"code":-32601,"message":"x"}})";
    std::string err;
    auto res = from_json_response(src, &err);
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(err, "Response cannot have both 'result' and 'error'");
}

TEST(JsonProtocolTests, ResponseWithInvalidErrorObjectReturnsError) {
    std::string src = R"({"jsonrpc":"2.0","error":"not_an_object"})";
    std::string err;
    auto res = from_json_response(src, &err);
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(err, "Invalid 'error' object");
}
