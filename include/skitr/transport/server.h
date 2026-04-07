#ifndef SKITR_SERVER_H
#define SKITR_SERVER_H

#include <skitr/skitr_export.h>
#include <cstdint>
#include <memory>



namespace skitter {
    namespace core{
        class Dispatcher;
    }
    namespace transport{
        /**
         * A simple TCP server that listens for incoming connections and processes JSON-RPC requests using a Dispatcher.
         * Runs in its own thread and can be started and stopped as needed.
         * Binds to the specified port (or an ephemeral port if 0 is given) and handles each incoming connection by reading a line of input, parsing it as a JSON-RPC request, dispatching it to the Dispatcher, and sending back the JSON-RPC response.
         * Designed to be simple and easy to use for testing and integration purposes.
         */
        class SKITR_EXPORT Server {
        public:
            Server(uint16_t port, skitter::core::Dispatcher& dispatcher);
            ~Server();

            void start();
            void stop();

            uint16_t port() const noexcept;

            Server(const Server&) = delete;
            Server& operator=(const Server&) = delete;

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;
        };
    } // namespace transport
} // namespace skitter

#endif //SKITR_SERVER_H
