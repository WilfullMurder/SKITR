#include <skitr/registry/method_registry.h>

#include <stdexcept>

namespace skitter {
    namespace registry {

        bool MethodRegistry::registerMethod(const std::string& name, Handler handler, bool allowOverwrite) {
            if(name.empty()) {
                throw std::invalid_argument("Method name cannot be empty");
            }
            if(!handler) {
                throw std::invalid_argument("Handler must be valid");
            }

            std::lock_guard<std::mutex> lock(mutex_);
            auto it = methods_.find(name);
            if(it != methods_.end()){
                if(!allowOverwrite) return false;
                it->second = std::move(handler);
                return true;
            }
            methods_.emplace(name, std::move(handler));
            return true;
        }

        bool MethodRegistry::hasMethod(const std::string& name) const {
            std::lock_guard <std::mutex> lock(mutex_);
            return methods_.find(name) != methods_.end();
        }

        MethodRegistry::Handler MethodRegistry::getHandler(const std::string& name) const {
            std::lock_guard <std::mutex> lock(mutex_);
            auto it = methods_.find(name);
            if(it == methods_.end()) {
                throw std::runtime_error("Method not found: " + name);
            }
            return it->second; // copy safe; callers can invoke without lock
        }

        bool MethodRegistry::unregisterMethod(const std::string& name) {
            std::lock_guard <std::mutex> lock(mutex_);
            return methods_.erase(name) > 0;
        }

        std::vector<std::string> MethodRegistry::listMethods() const {
            std::lock_guard <std::mutex> lock(mutex_);
            std::vector<std::string> out;
            out.reserve(methods_.size());
            for(const auto& kv : methods_) out.push_back(kv.first);
            return out;
        }

        std::size_t MethodRegistry::size() const {
            std::lock_guard <std::mutex> lock(mutex_);
            return methods_.size();
        }

    } // namespace registry
} // namespace skitter