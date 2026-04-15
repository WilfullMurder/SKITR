#include <gtest/gtest.h>
#include <skitr/core/value.h>
#include <chrono>
#include <vector>
#include <string>

using skitter::core::Value;
using skitter::core::Type;

TEST(ValueTests, TypeNameReturnsCorrectStrings) {
    EXPECT_EQ(Value::typeName(Type::Null), "Null");
    EXPECT_EQ(Value::typeName(Type::Bool), "Bool");
    EXPECT_EQ(Value::typeName(Type::Int), "Int");
    EXPECT_EQ(Value::typeName(Type::Double), "Double");
    EXPECT_EQ(Value::typeName(Type::String), "String");
    EXPECT_EQ(Value::typeName(Type::Array), "Array");
    EXPECT_EQ(Value::typeName(Type::Object), "Object");
    EXPECT_EQ(Value::typeName(Type::Binary), "Binary");
    EXPECT_EQ(Value::typeName(Type::DateTime), "DateTime");
}

TEST(ValueTests, ConstructedFromPrimitiveTypes_ReturnsCorrectValues) {
    Value b(true);
    EXPECT_TRUE(b.isBool());
    EXPECT_TRUE(b.asBool());

    Value i(int64_t(123));
    EXPECT_TRUE(i.isInt());
    EXPECT_EQ(i.asInt(), 123);

    Value d(3.14);
    EXPECT_TRUE(d.isDouble());
    EXPECT_DOUBLE_EQ(d.asDouble(), 3.14);

    Value s(std::string("hello"));
    EXPECT_TRUE(s.isString());
    EXPECT_EQ(s.asString(), "hello");
}

TEST(ValueTests, AsWrongTypeThrowsRuntimeError) {
    Value v; // Null
    EXPECT_THROW(v.asBool(), std::runtime_error);
    EXPECT_THROW(v.asInt(), std::runtime_error);
    EXPECT_THROW(v.asDouble(), std::runtime_error);
    EXPECT_THROW(v.asString(), std::runtime_error);

    Value vi(int64_t(5));
    EXPECT_THROW(vi.asBool(), std::runtime_error);
    EXPECT_THROW(vi.asDouble(), std::runtime_error);
    EXPECT_THROW(vi.asString(), std::runtime_error);
}

TEST(ValueTests, AsArrayFromNull_CreatesArrayAndAllowsModification) {
    Value v; // Null
    EXPECT_TRUE(v.isNull());
    auto &arr = v.asArray();
    EXPECT_TRUE(v.isArray());
    EXPECT_EQ(arr.size(), 0u);

    arr.push_back(Value(int64_t(42)));
    EXPECT_EQ(v.asArray().size(), 1u);
    EXPECT_EQ(v.asArray().at(0).asInt(), 42);
}

TEST(ValueTests, AsObjectFromNull_CreatesObjectAndAllowsModification) {
    Value v; // Null
    EXPECT_TRUE(v.isNull());
    auto &obj = v.asObject();
    EXPECT_TRUE(v.isObject());
    EXPECT_EQ(obj.size(), 0u);

    obj["key"] = Value(std::string("value"));
    EXPECT_EQ(v.asObject().at("key").asString(), "value");
}

TEST(ValueTests, ArrayCopyingProducesIndependentCopies) {
    std::vector<Value> base = { Value(int64_t(1)), Value(int64_t(2)) };
    Value original(base);
    Value copy = original;

    EXPECT_EQ(original.asArray().size(), 2u);
    EXPECT_EQ(copy.asArray().size(), 2u);

    copy.asArray().push_back(Value(int64_t(3)));
    EXPECT_EQ(copy.asArray().size(), 3u);
    EXPECT_EQ(original.asArray().size(), 2u);
}

TEST(ValueTests, ObjectCopyingProducesIndependentCopies) {
    std::map<std::string, Value> base = { {"a", Value(int64_t(10))} };
    Value original(base);
    Value copy = original;

    EXPECT_EQ(original.asObject().size(), 1u);
    EXPECT_EQ(copy.asObject().size(), 1u);

    copy.asObject()["b"] = Value(int64_t(20));
    EXPECT_EQ(copy.asObject().size(), 2u);
    EXPECT_EQ(original.asObject().size(), 1u);
}

TEST(ValueTests, BinaryConstructAndAccess) {
    Value::binary_t data = { 0x01, 0x02, 0x03 };
    Value vb(data);
    EXPECT_TRUE(vb.isBinary());
    EXPECT_EQ(vb.asBinary(), data);

    // toString currently emits a marker with the byte count, verify it contains that marker
    auto s = vb.toString();
    EXPECT_NE(s.find("<binary:3 bytes>"), std::string::npos);
}

TEST(ValueTests, SetBinaryAndAccess) {
    Value v;
    Value::binary_t data = { 0xAA, 0xBB };
    v.setBinary(data);
    EXPECT_TRUE(v.isBinary());
    EXPECT_EQ(v.asBinary(), data);
}

TEST(ValueTests, DateTimeConstructAndAccess) {
    using clock = std::chrono::system_clock;
    auto now = clock::now();
    Value dt(now);
    EXPECT_TRUE(dt.isDateTime());
    EXPECT_EQ(dt.asDateTime(), now);

    // toString should produce a quoted representation for datetime
    auto s = dt.toString();
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.front(), '"');
}

TEST(ValueTests, SetDateTimeAndAccess) {
    using clock = std::chrono::system_clock;
    auto epoch = clock::time_point{}; // epoch
    Value v;
    v.setDateTime(epoch);
    EXPECT_TRUE(v.isDateTime());
    EXPECT_EQ(v.asDateTime(), epoch);
}