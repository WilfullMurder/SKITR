#include <asio.hpp>
#include <iostream>
#include <string>
#include <csignal>

using asio::ip::tcp;

std::atomic<bool> running{true};

void signalHandler(int) {
    running = false;
}

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    uint16_t port = 12345;

    if (argc > 1) host = argv[1];
    if (argc > 2) port = static_cast<uint16_t>(std::stoi(argv[2]));

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        asio::io_context io;
        tcp::socket socket(io);
        socket.connect(tcp::endpoint(asio::ip::make_address(host), port));

        std::cout << "Connected to " << host << ":" << port << "\n";
        std::cout << "Type JSON-RPC requests (or 'exit' to quit)\n";

        while (running) {
            std::cout << "> ";
            std::string line;
            if (!std::getline(std::cin, line)) break;
            if (line == "exit") break;
            if (line.empty()) continue;

            line.push_back('\n'); // server expects newline

            asio::write(socket, asio::buffer(line));

            asio::streambuf respbuf;
            asio::read_until(socket, respbuf, '\n');
            std::istream is(&respbuf);
            std::string response;
            std::getline(is, response);
            if (!response.empty() && response.back() == '\r') response.pop_back();

            std::cout << "< " << response << "\n";
        }

        asio::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);
        socket.close(ec);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    std::cout << "Client exited.\n";
    return 0;
}