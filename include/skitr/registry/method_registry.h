#ifndef SKITR_METHOD_REGISTRY_H
#define SKITR_METHOD_REGISTRY_H

#include <skitr/skitr_export.h>
#include <skitr/core/value.h>

#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <vector>

namespace skitter {
    namespace registry {
        /**
         * A thread-safe registry for JSON-RPC method handlers.
         */
        class SKITR_EXPORT MethodRegistry {
        public:
            using Value = skitter::core::Value;
            using Handler = std::function<Value(const std::vector<Value>&)>;

            MethodRegistry() = default;
            ~MethodRegistry() = default;

            /**
             * Registers a method handler with the given name. If a handler with the same name already exists, the behavior depends on the allowOverwrite flag:
             * @param name The name of the method to register. This must be a non-empty string and should be unique within the registry.
             * @param handler The handler function to associate with the method name. This should be a valid std::function that takes a std::Vector<Value> as input and returns a Value as output.
             * @param allowOverwrite If false (the default), the method will not be registered if a handler with the same name already exists, and the function will return false. If true, the existing handler will be overwritten with the new handler, and the function will return true.
             * @return true if the method was successfully registered (either because it was new or because it was overwritten), false if the method was not registered due to a name conflict and allowOverwrite was false.
             * @throws std::invalid_argument if the method name is empty or if the handler is not valid (e.g., if it is an empty std::function).
             */
            bool registerMethod(const std::string& name, Handler handler, bool allowOverwrite = false);

            /**
             * Checks if a method with the given name is registered in the registry.
             * @param name The name of the method to check for. This should be a non-empty string.
             * @return true if a method with the given name is registered, false otherwise.
             */
            bool hasMethod(const std::string& name) const;

            /**
             * Retrieves the handler function associated with the given method name. If no handler is registered under that name, an exception will be thrown.
             * @param name The name of the method whose handler to retrieve. This should be a non-empty string.
             * @return The handler function associated with the given method name. This will be a valid std::function that can be invoked with a Value parameter to execute the method.
             * @throws std::runtime_error if no handler is registered under the given method name.
             */
            Handler getHandler(const std::string& name) const;

            /**
             * Unregisters the method handler associated with the given name. If no handler is registered under that name, the function will return false.
             * @param name The name of the method to unregister. This should be a non-empty string.
             * @return true if a handler was successfully unregistered, false if no handler was registered under the given name.
             */
            bool unregisterMethod(const std::string& name);

            /**
             * Returns a list of all registered method names in the registry. The order of the method names in the returned vector is not guaranteed.
             * @return A vector containing the names of all registered methods. If no methods are registered, the returned vector will be empty.
             */
            std::vector<std::string> listMethods() const;

            /**
             * @return  The number of registered methods. This will be a non-negative integer. If no methods are registered, this will return 0.
             */
            std::size_t size() const;

        private:
            mutable std::mutex mutex_; ///< Mutex to protect access to the methods_ map for thread safety
            std::unordered_map<std::string, Handler> methods_; ///< Map from method names to their corresponding handler functions. The keys are method names (non-empty strings), and the values are std::function objects that can be invoked to execute the method.
        };
    } // namespace registry
} // namespace skitter

#endif //SKITR_METHOD_REGISTRY_H
