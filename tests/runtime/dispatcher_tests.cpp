#include <gtest/gtest.h>
#include <skitr/runtime/dispatcher.h>
#include <skitr/registry/method_registry.h>
#include <skitr/core/message.h>
#include <skitr/core/error.h>
#include <skitr/core/value.h>

using skitter::registry::MethodRegistry;
using skitter::runtime::Dispatcher;
using skitter::core::Value;
using skitter::core::RpcRequest;
using skitter::core::ErrorCode;
using skitter::core::RpcError;

TEST(DispatcherTests, DispatchReturnsSuccessWhenHandlerRegistered) {
    MethodRegistry reg;
    reg.registerMethod("hello", MethodRegistry::Handler([](const std::vector<Value>&){ return Value(std::string("world")); }));
    Dispatcher disp(reg);

    RpcRequest req;
    req.method = "hello";
    req.params = {};
    req.id = Value(int64_t(5));

    auto res = disp.dispatch(req);
    EXPECT_TRUE(res.result.has_value());
    EXPECT_FALSE(res.error.has_value());
    EXPECT_TRUE(res.id.has_value());
    EXPECT_EQ(res.result->asString(), "world");
    EXPECT_EQ(res.id->asInt(), 5);
}

TEST(DispatcherTests, DispatchReturnsMethodNotFoundErrorWhenMissing) {
    MethodRegistry reg;
    Dispatcher disp(reg);

    RpcRequest req;
    req.method = "no_such";
    req.params = {};
    req.id = Value(int64_t(42));

    auto res = disp.dispatch(req);
    EXPECT_FALSE(res.result.has_value());
    EXPECT_TRUE(res.error.has_value());
    EXPECT_EQ(res.error->code, ErrorCode::MethodNotFound);
    EXPECT_TRUE(res.id.has_value());
    EXPECT_EQ(res.id->asInt(), 42);
}

TEST(DispatcherTests, DispatchReturnsInvalidRequestWhenMethodEmpty) {
    MethodRegistry reg;
    Dispatcher disp(reg);

    RpcRequest req;
    req.method = "";
    req.params = {};
    req.id = std::nullopt;

    auto res = disp.dispatch(req);
    EXPECT_FALSE(res.result.has_value());
    EXPECT_TRUE(res.error.has_value());
    EXPECT_EQ(res.error->code, ErrorCode::InvalidRequest);
    EXPECT_EQ(res.error->message, "Empty method");
    EXPECT_FALSE(res.id.has_value());
}

TEST(DispatcherTests, DispatchReturnsErrorWhenHandlerThrowsRpcError) {
    MethodRegistry reg;
    reg.registerMethod("fail", MethodRegistry::Handler([](const std::vector<Value>&) -> Value{
        throw RpcError{ ErrorCode::InternalError, "handler failed", std::nullopt };
    }));
    Dispatcher disp(reg);

    RpcRequest req;
    req.method = "fail";
    req.params = {};
    req.id = Value(int64_t(7));

    auto res = disp.dispatch(req);
    EXPECT_FALSE(res.result.has_value());
    EXPECT_TRUE(res.error.has_value());
    EXPECT_EQ(res.error->code, ErrorCode::InternalError);
    EXPECT_EQ(res.error->message, "handler failed");
    EXPECT_TRUE(res.id.has_value());
    EXPECT_EQ(res.id->asInt(), 7);
}