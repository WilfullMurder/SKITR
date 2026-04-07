#include <gtest/gtest.h>
#include <skitr/registry/method_registry.h>
#include <skitr/core/value.h>
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm>

using skitter::registry::MethodRegistry;
using skitter::core::Value;

TEST(MethodRegistryTests, RegisterAndLookupHandlerReturnsExpectedValue) {
    MethodRegistry reg;
    reg.registerMethod("sum", MethodRegistry::Handler([](const std::vector<Value>& params){
        int64_t sum = 0;
        for (const auto &v : params) if (v.isInt()) sum += v.asInt();
        return Value(sum);
    }));

    auto handler = reg.getHandler("sum");
    Value res = handler(std::vector<Value>{ Value(int64_t(2)), Value(int64_t(3)) });
    EXPECT_EQ(res.asInt(), 5);
}

TEST(MethodRegistryTests, VarargsHandlerReceivesArrayAndAdapterWorks) {
    MethodRegistry reg;
    reg.registerMethod("sum", MethodRegistry::Handler([](const std::vector<Value>& params){
        int64_t sum = 0;
        for (const auto &a : params) if (a.isInt()) sum += a.asInt();
        return Value(sum);
    }));

    auto handler = reg.getHandler("sum");
    std::vector<Value> params{ Value(int64_t(1)), Value(int64_t(4)), Value(int64_t(5)) };
    Value out = handler(params);
    EXPECT_EQ(out.asInt(), 10);
}

TEST(MethodRegistryTests, DuplicateRegistrationWithoutOverwriteKeepsOriginal) {
    MethodRegistry reg;
    reg.registerMethod("v", MethodRegistry::Handler([](const std::vector<Value>&){ return Value(int64_t(10)); }), false);
    auto handler = reg.getHandler("v");
    EXPECT_EQ(handler(std::vector<Value>{}).asInt(), 10);
}

TEST(MethodRegistryTests, DuplicateRegistrationWithOverwriteReplacesHandler) {
    MethodRegistry reg;
    reg.registerMethod("v", MethodRegistry::Handler([](const std::vector<Value>&){ return Value(int64_t(10)); }), false);
    reg.registerMethod("v", MethodRegistry::Handler([](const std::vector<Value>&){ return Value(int64_t(20)); }), true);
    auto handler = reg.getHandler("v");
    EXPECT_EQ(handler(std::vector<Value>{}).asInt(), 20);
}

TEST(MethodRegistryTests, RegisterWithEmptyNameThrowsInvalidArgument) {
    MethodRegistry reg;
    EXPECT_THROW(reg.registerMethod("", MethodRegistry::Handler([](const std::vector<Value>&){ return Value(); })), std::invalid_argument);
}

TEST(MethodRegistryTests, RegisterWithEmptyHandlerThrowsInvalidArgument) {
    MethodRegistry reg;
    MethodRegistry::Handler h;
    EXPECT_THROW(reg.registerMethod("x", h), std::invalid_argument);
}

TEST(MethodRegistryTests, UnregisterRemovesMethodAndHasMethodReflectsIt) {
    MethodRegistry reg;
    reg.registerMethod("to_remove", MethodRegistry::Handler([](const std::vector<Value>&){ return Value(); }));
    EXPECT_TRUE(reg.hasMethod("to_remove"));
    EXPECT_TRUE(reg.unregisterMethod("to_remove"));
    EXPECT_FALSE(reg.hasMethod("to_remove"));
    EXPECT_EQ(reg.size(), 0u);
}

TEST(MethodRegistryTests, ListMethodsContainsRegisteredNames) {
    MethodRegistry reg;
    reg.registerMethod("a", MethodRegistry::Handler([](const std::vector<Value>&){ return Value(int64_t(0)); }));
    reg.registerMethod("b", MethodRegistry::Handler([](const std::vector<Value>&){ return Value(int64_t(0)); }));
    auto names = reg.listMethods();
    EXPECT_NE(std::find(names.begin(), names.end(), "a"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "b"), names.end());
    EXPECT_EQ(reg.size(), names.size());
}

TEST(MethodRegistryTests, GetHandlerForMissingNameThrowsRuntimeError) {
    MethodRegistry reg;
    EXPECT_THROW(reg.getHandler("missing"), std::runtime_error);
}

TEST(MethodRegistryTests, ConcurrentRegistrationsProduceCorrectSize) {
    MethodRegistry reg;
    const int threads = 8;
    const int perThread = 200;
    std::atomic<int> counter{0};
    std::vector<std::thread> workers;
    for (int t = 0; t < threads; ++t) {
        workers.emplace_back([&reg, &counter, t, perThread]() {
            for (int i = 0; i < perThread; ++i) {
                std::string name = "m_" + std::to_string(t) + "_" + std::to_string(i);
                reg.registerMethod(name, MethodRegistry::Handler([](const std::vector<Value>&){ return Value(); }));
                ++counter;
            }
        });
    }
    for (auto &th : workers) th.join();
    EXPECT_EQ(reg.size(), static_cast<std::size_t>(threads * perThread));
}