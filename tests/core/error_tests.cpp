#include <gtest/gtest.h>
#include <skitr/core/error.h>
#include <skitr/core/message.h>
#include <skitr/core/value.h>

using skitter::core::Value;
using skitter::core::RpcRequest;
using skitter::core::RpcResponse;
using skitter::core::RpcError;
using skitter::core::ErrorCode;
using skitter::core::makeParseError;
using skitter::core::makeMethodNotFoundError;
using skitter::core::makeServerError;


TEST(RpcTests, FailureFactoryCreatesValidFailureResponse) {
    RpcError err = makeMethodNotFoundError("method missing");
    auto res = RpcResponse::failure(err);
    EXPECT_TRUE(res.error.has_value());
    EXPECT_FALSE(res.result.has_value());
    EXPECT_TRUE(res.isValid());
    EXPECT_EQ(res.error->code, ErrorCode::MethodNotFound);
    EXPECT_EQ(res.error->message, "method missing");
}

TEST(RpcTests, ResponseInvalidWhenNeitherResultNorError) {
    RpcResponse res;
    res.id = Value(int64_t(1));
    EXPECT_FALSE(res.result.has_value());
    EXPECT_FALSE(res.error.has_value());
    EXPECT_FALSE(res.isValid());
}

TEST(RpcTests, ResponseInvalidWhenBothResultAndErrorSet) {
    RpcResponse res;
    res.result = Value(int64_t(1));
    res.error = RpcError{ErrorCode::InternalError, "err", std::nullopt};
    EXPECT_FALSE(res.isValid());
}

TEST(RpcErrorTests, MakeParseErrorCarriesData) {
    auto data = Value(std::string("debug"));
    auto err = makeParseError("parse failed", data);
    EXPECT_EQ(err.code, ErrorCode::ParseError);
    EXPECT_EQ(err.message, "parse failed");
    EXPECT_TRUE(err.data.has_value());
    EXPECT_EQ(err.data->asString(), "debug");
}

TEST(RpcErrorTests, MakeServerErrorAcceptsValidRange) {
    auto err = makeServerError(-32050, "server issue");
    EXPECT_EQ(err.message, "server issue");
    EXPECT_EQ(static_cast<int>(err.code), -32050);
}

TEST(RpcErrorTests, MakeServerErrorThrowsOnOutOfRange) {
    EXPECT_THROW(makeServerError(-32100, "too low"), std::invalid_argument);
    EXPECT_THROW(makeServerError(-31999, "too high"), std::invalid_argument);
}
