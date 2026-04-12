#include <skitr/transport/server.h>
#include <skitr/runtime/dispatcher.h>
#include <skitr/registry/method_registry.h>
#include <skitr/core/value.h>

#include <iostream>
#include <thread>
#include <csignal>
#include <atomic>

using namespace skitter;

std::atomic<bool> running{true};

void signalHandler(int) {
    running = false;
}

int main(int argc, char* argv[]) {
    uint16_t port = 12345; // default port
    if (argc > 1) port = static_cast<uint16_t>(std::stoi(argv[1]));

    // setup signal handler for clean shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // create method registry and register example methods
    registry::MethodRegistry reg;

    reg.registerMethod("echo", registry::MethodRegistry::Handler(
            [](const std::vector<core::Value>& params) -> core::Value {
                if (!params.empty()) return params[0];
                return core::Value();
            }
    ));

    reg.registerMethod("add", registry::MethodRegistry::Handler(
            [](const std::vector<core::Value>& params) -> core::Value {
                if (params.size() >= 2 && params[0].isInt() && params[1].isInt()) {
                    return core::Value(params[0].asInt() + params[1].asInt());
                }
                return core::Value();
            }
    ));

    // create dispatcher
    runtime::Dispatcher dispatcher(reg);

    // start server
    transport::Server server(port, dispatcher);
    server.start();

    std::cout << "Server running on port " << server.port() << ", press Ctrl+C to quit.\n";

    // keep main thread alive until signal
    while (running) std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::cout << "Shutting down server...\n";
    server.stop();
    std::cout << "Server stopped.\n";

    return 0;
}