#include <gtest/gtest.h>
#include <skitr/core/message.h>
#include <skitr/core/value.h>
#include <skitr/core/error.h>



TEST(MessageTests, IsNotificationWhenIdMissing) {
skitter::core::RpcRequest req;
req.method = "doThing";
req.params = {};
req.id = std::nullopt;
EXPECT_TRUE(req.isNotification());
}

TEST(MessageTests, IsNotNotificationWhenIdPresent) {
skitter::core::RpcRequest req;
req.method = "doThing";
req.id = skitter::core::Value(int64_t(7));
EXPECT_FALSE(req.isNotification());
}

TEST(MessageTests, SuccessFactoryCreatesValidSuccessResponseWithoutId) {
auto res = skitter::core::RpcResponse::success(skitter::core::Value(std::string("ok")));
EXPECT_TRUE(res.result.has_value());
EXPECT_FALSE(res.error.has_value());
EXPECT_FALSE(res.id.has_value());
EXPECT_TRUE(res.isValid());
EXPECT_EQ(res.result->asString(), "ok");
}

TEST(MessageTests, SuccessFactorySetsProvidedId) {
skitter::core::Value id(int64_t(42));
auto res = skitter::core::RpcResponse::success(skitter::core::Value(int64_t(7)), id);
EXPECT_TRUE(res.result.has_value());
EXPECT_FALSE(res.error.has_value());
EXPECT_TRUE(res.id.has_value());
EXPECT_TRUE(res.isValid());
EXPECT_EQ(res.id->asInt(), 42);
EXPECT_EQ(res.result->asInt(), 7);
}

TEST(MessageTests, FailureFactoryCreatesValidFailureResponse) {
skitter::core::RpcError err{skitter::core::ErrorCode::MethodNotFound, "method missing", std::nullopt};
auto res = skitter::core::RpcResponse::failure(err);
EXPECT_TRUE(res.error.has_value());
EXPECT_FALSE(res.result.has_value());
EXPECT_TRUE(res.isValid());
EXPECT_EQ(res.error->code, skitter::core::ErrorCode::MethodNotFound);
EXPECT_EQ(res.error->message, "method missing");
}

TEST(MessageTests, ResponseInvalidWhenNeitherResultNorError) {
skitter::core::RpcResponse res;
res.id = skitter::core::Value(int64_t(1));
EXPECT_FALSE(res.result.has_value());
EXPECT_FALSE(res.error.has_value());
EXPECT_FALSE(res.isValid());
}

TEST(MessageTests, ResponseInvalidWhenBothResultAndErrorSet) {
skitter::core::RpcResponse res;
res.result = skitter::core::Value(int64_t(1));
res.error = skitter::core::RpcError{skitter::core::ErrorCode::InternalError, "err", std::nullopt};
EXPECT_FALSE(res.isValid());
}
