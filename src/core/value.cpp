#include <skitr/core/value.h>
#include <stdexcept>
#include <utility>
#include <memory>
#include <type_traits>
#include <string>
#include <vector>
#include <map>
#include <variant>

namespace skitter {
    namespace core {
        std::string Value::typeName(Type t) noexcept {
            switch (t) {
                case Type::Null: return "Null";
                case Type::Bool: return "Bool";
                case Type::Int: return "Int";
                case Type::Double: return "Double";
                case Type::String: return "String";
                case Type::Array: return "Array";
                case Type::Object: return "Object";
                default: return "Unknown";
            }
        }

        void Value::ensureUniqueArray() {
            if(!isArray()){
                throw std::runtime_error("Value is not an array");
            }
            auto &ptr = std::get<array_ptr>(storage_);
            if(!ptr) {
                ptr = std::make_shared<array_t>();
            }else if(!ptr.unique()) {
                ptr = std::make_shared<array_t>(*ptr);
            }
        }

        void Value::ensureUniqueObject() {
            if(!isObject()){
                throw std::runtime_error("Value is not an object");
            }
            auto &ptr = std::get<object_ptr>(storage_);
            if(!ptr) {
                ptr = std::make_shared<object_t>();
            }else if(!ptr.unique()) {
                ptr = std::make_shared<object_t>(*ptr);
            }
        }

        Value::Value() noexcept: type_(Type::Null), storage_(std::monostate{}) {}
        Value::Value(bool b): type_(Type::Bool), storage_(b) {}
        Value::Value(int64_t i): type_(Type::Int), storage_(i) {}
        Value::Value(double d): type_(Type::Double), storage_(d) {}
        Value::Value(const std::string& s): type_(Type::String), storage_(s) {}
        Value::Value(std::string&& s): type_(Type::String), storage_(std::move(s)) {}
        Value::Value(const char* s): type_(Type::String), storage_(std::string(s)) {}
        Value::Value(const std::vector<Value>& arr): type_(Type::Array), storage_(std::make_shared<array_t>(arr)) {}
        Value::Value(std::vector<Value>&& arr): type_(Type::Array), storage_(std::make_shared<array_t>(std::move(arr))) {}
        Value::Value(const std::map<std::string, Value>& obj): type_(Type::Object), storage_(std::make_shared<object_t>(obj)) {}
        Value::Value(std::map<std::string, Value>&& obj): type_(Type::Object), storage_(std::make_shared<object_t>(std::move(obj))) {}

        Value::Value(const Value& other): type_(other.type_){
            switch (other.type_) {
                case Type::Null:
                    storage_ = std::monostate{};
                    break;
                case Type::Bool:
                    storage_ = std::get<bool>(other.storage_);
                    break;
                case Type::Int:
                    storage_ = std::get<int64_t>(other.storage_);
                    break;
                case Type::Double:
                    storage_ = std::get<double>(other.storage_);
                    break;
                case Type::String:
                    storage_ = std::get<std::string>(other.storage_);
                    break;
                case Type::Array: {
                    auto p = std::get<array_ptr>(other.storage_);
                    storage_ = std::make_shared<array_t>(p ? *p : array_t{});
                    break;
                }
                case Type::Object: {
                    auto p = std::get<object_ptr>(other.storage_);
                    storage_ = std::make_shared<object_t>(p ? *p : object_t{});
                    break;
                }
            }
        }
        Value::Value(Value&& other) noexcept: type_(other.type_), storage_(std::move(other.storage_)) {
            other.type_ = Type::Null;
            other.storage_ = std::monostate{};
        }

        Value& Value::operator=(const Value& other) {
            if (this == &other) return *this;
            Value tmp(other);
            *this = std::move(tmp);
            return *this;
        }

        Type Value::type() const noexcept { return type_; }
        bool Value::isNull() const noexcept { return type_ == Type::Null; }
        bool Value::isBool() const noexcept { return type_ == Type::Bool; }
        bool Value::isInt() const noexcept { return type_ == Type::Int; }
        bool Value::isDouble() const noexcept { return type_ == Type::Double; }
        bool Value::isString() const noexcept { return type_ == Type::String; }
        bool Value::isArray() const noexcept { return type_ == Type::Array; }
        bool Value::isObject() const noexcept { return type_ == Type::Object; }

        bool Value::asBool() const {
            if (!isBool()) throw std::runtime_error("Value::asBool: type is " + typeName(type_));
            return std::get<bool>(storage_);
        }

        int64_t Value::asInt() const {
            if (!isInt()) throw std::runtime_error("Value::asInt: type is " + typeName(type_));
            return std::get<int64_t>(storage_);
        }

        double Value::asDouble() const {
            if (!isDouble()) throw std::runtime_error("Value::asDouble: type is " + typeName(type_));
            return std::get<double>(storage_);
        }

        const std::string& Value::asString() const {
            if (!isString()) throw std::runtime_error("Value::asString: type is " + typeName(type_));
            return std::get<std::string>(storage_);
        }

        std::vector<Value>& Value::asArray() {
            if (isNull()) {
                type_ = Type::Array;
                storage_ = std::make_shared<array_t>();
            } else if (!isArray()) {
                throw std::runtime_error("Value::asArray: type is " + typeName(type_));
            }
            ensureUniqueArray();
            return *std::get<array_ptr>(storage_);
        }

        std::map<std::string, Value>& Value::asObject() {
            if (isNull()) {
                type_ = Type::Object;
                storage_ = std::make_shared<object_t>();
            } else if (!isObject()) {
                throw std::runtime_error("Value::asObject: type is " + typeName(type_));
            }
            ensureUniqueObject();
            return *std::get<object_ptr>(storage_);
        }

        const Value::array_t& Value::asArray() const {
            if (!isArray()) throw std::runtime_error("Value::asArray: type is " + typeName(type_));
            const auto &ptr = std::get<array_ptr>(storage_);
            static const array_t empty_array{};
            return ptr ? *ptr : empty_array;
        }

        const Value::object_t& Value::asObject() const {
            if (!isObject()) throw std::runtime_error("Value::asObject: type is " + typeName(type_));
            const auto &ptr = std::get<object_ptr>(storage_);
            static const object_t empty_object{};
            return ptr ? *ptr : empty_object;
        }

    } // namespace core
} // namespace skitter