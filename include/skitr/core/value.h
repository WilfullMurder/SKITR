#ifndef SKITR_VALUE_H
#define SKITR_VALUE_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <stdexcept>

#include <skitr/skitr_export.h>

namespace skitter {
    namespace core {
        /**
         * Represents a JSON value, which can be one of the following types: null, boolean, integer, double, string, array, or object.
         */
        enum class Type {
            Null,
            Bool,
            Int,
            Double,
            String,
            Array,
            Object
        };

        /**
         * A class representing a JSON value.
         * It can hold any of the types defined in the Type enum.
         * For arrays and objects, it uses shared pointers to allow for efficient copying and mutation.
         */
        class SKITR_EXPORT Value {
        public:
            using array_t = std::vector<Value>;
            using object_t = std::map<std::string, Value>;
            using array_ptr = std::shared_ptr<array_t>;
            using object_ptr = std::shared_ptr<object_t>;

            Value() noexcept;
            Value(bool b);
            Value(int64_t i);
            Value(double d);
            Value(const std::string &s);
            Value(std::string &&s);
            Value(const char *s);
            Value(const array_t &a);
            Value(array_t &&a);
            Value(const object_t &o);
            Value(object_t &&o);

            static std::string typeName(Type t) noexcept;

            Type type() const noexcept;
            bool isNull() const noexcept;
            bool isBool() const noexcept;
            bool isInt() const noexcept;
            bool isDouble() const noexcept;
            bool isString() const noexcept;
            bool isArray() const noexcept;
            bool isObject() const noexcept;

            bool asBool() const;
            int64_t asInt() const;
            double asDouble() const;
            const std::string &asString() const;
            array_t &asArray();
            object_t &asObject();
            const array_t &asArray() const;
            const object_t &asObject() const;

            Value(const Value &);
            Value(Value &&) noexcept;
            Value &operator=(const Value &);
            Value &operator=(Value &&) noexcept = default;
            ~Value() = default;

        private:
            void ensureUniqueArray();
            void ensureUniqueObject();

            Type type_;
            std::variant<std::monostate, bool, int64_t, double, std::string, array_ptr, object_ptr> storage_;
    };
    } // namespace core
} // namespace skitter


#endif //SKITR_VALUE_H
