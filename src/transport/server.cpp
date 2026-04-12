#include <skitr/transport/server.h>
#include <skitr/protocol/json_protocol.h>
#include <skitr/runtime/dispatcher.h>
#include <skitr/core/error.h>
#include <skitr/core/message.h>

#include <asio.hpp>

#include <thread>
#include <atomic>
#include <iostream>
#include <sstream>

using asio::ip::tcp;

namespace skitter{
    namespace transport {
        struct Server::Impl {
            Impl(uint16_t port, runtime::Dispatcher& dispatcher) : io_ctx_(), acceptor_(io_ctx_), dispatcher_(dispatcher), is_running_(false), bound_port_(port) {
                asio::error_code ec;

                tcp::endpoint endpoint(tcp::v4(), port);
                acceptor_.open(endpoint.protocol(), ec);
                if (ec) throw std::runtime_error("Failed to open acceptor: " + ec.message());

                acceptor_.set_option(asio::socket_base::reuse_address(true), ec);
                acceptor_.bind(endpoint, ec);
                if (ec) throw std::runtime_error("Failed to bind acceptor: " + ec.message());
                acceptor_.listen(asio::socket_base::max_listen_connections, ec);
                if (ec) throw std::runtime_error("Failed to listen: " + ec.message());

                bound_port_ = acceptor_.local_endpoint().port();
            }

            ~Impl() {
                stop();
            }

            void start() {
                if (is_running_.exchange(true)) return;
                accept_thread_ = std::thread([this] { acceptLoop(); });
                io_thread_ = std::thread([this] { io_ctx_.run(); });
            }

            void stop() {
                if (!is_running_.exchange(false)) return;
                asio::error_code ec;
                acceptor_.close(ec);
                io_ctx_.stop();
                if (accept_thread_.joinable()) accept_thread_.join();
                if (io_thread_.joinable()) io_thread_.join();
            }

            void acceptLoop() {
                while (is_running_) {
                    asio::error_code ec;
                    tcp::socket socket(io_ctx_);
                    acceptor_.accept(socket, ec);
                    if (ec) {
                        if (is_running_) {
                            // accept error while running
                        }
                        break;
                    }
                    std::thread(&Impl::handleConnection, this, std::move(socket)).detach();
                }
            }

            void handleConnection(tcp::socket socket) {
                try {
                    asio::streambuf buffer;

                    while (true) {
                        asio::error_code ec;
                        std::size_t n = asio::read_until(socket, buffer, "\n", ec);
                        if (ec) {
                            break; // client disconnected
                        }

                        std::istream is(&buffer);
                        std::string line;
                        std::getline(is, line);
                        if (!line.empty() && line.back() == '\r') line.pop_back();

                        std::string parse_error;
                        auto req_opt = protocol::from_json_request(line, &parse_error);
                        skitter::core::RpcResponse res;
                        if (!req_opt.has_value()) {
                            res = skitter::core::RpcResponse::failure(skitter::core::makeParseError(parse_error), std::nullopt);
                        } else {
                            res = dispatcher_.dispatch(*req_opt);
                        }

                        std::string out = protocol::to_json(res) + "\n";
                        asio::write(socket, asio::buffer(out), ec);
                    }

                    asio::error_code ignore;
                    socket.shutdown(tcp::socket::shutdown_both, ignore);
                    socket.close(ignore);

                } catch (const std::exception& ex) {
                    asio::error_code ignore;
                    socket.shutdown(tcp::socket::shutdown_both, ignore);
                    socket.close(ignore);
                }
            }


            asio::io_context io_ctx_;
            tcp::acceptor acceptor_;
            runtime::Dispatcher& dispatcher_;
            std::atomic<bool> is_running_;
            std::thread accept_thread_;
            std::thread io_thread_;
            uint16_t bound_port_;
        };

        Server::Server(uint16_t port, runtime::Dispatcher& dispatcher) : impl_(std::make_unique<Impl>(port, dispatcher)) {}

        Server::~Server() = default;

        void Server::start() {
            impl_->start();
        }

        void Server::stop() {
            impl_->stop();
        }

        uint16_t Server::port() const noexcept {
            return impl_->bound_port_;
        }
    } // namespace transport
} // namespace skitter


