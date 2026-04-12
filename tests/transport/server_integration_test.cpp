#include <gtest/gtest.h>
#include <skitr/transport/server.h>
#include <skitr/registry/method_registry.h>
#include <skitr/runtime/dispatcher.h>
#include <skitr/protocol/json_protocol.h>
#include <skitr/core/value.h>

#include <asio.hpp>
#include <thread>

using namespace skitter;
using asio::ip::tcp;

TEST(TransportIntegration, EchoRoundtrip) {
registry::MethodRegistry reg;
// register a simple "echo" method that returns its first param or null
reg.registerMethod("echo", registry::MethodRegistry::Handler([](const std::vector<core::Value>& params){
if (!params.empty()) return params[0];
return core::Value();
}));

runtime::Dispatcher disp(reg);

// start server on ephemeral port
transport::Server server(0, disp);
server.start();
uint16_t port = server.port();
ASSERT_NE(port, 0u);

// give server a moment to start
std::this_thread::sleep_for(std::chrono::milliseconds(50));

// prepare a request JSON (newline-terminated)
std::string req = R"({"jsonrpc":"2.0","method":"echo","params":["hi"],"id":1})";
req.push_back('\n');

asio::io_context io;
tcp::socket sock(io);
sock.connect(tcp::endpoint(asio::ip::make_address("127.0.0.1"), port));

asio::write(sock, asio::buffer(req));

asio::streambuf respbuf;
asio::read_until(sock, respbuf, '\n');
std::istream is(&respbuf);
std::string line;
std::getline(is, line);
if (!line.empty() && line.back() == '\r') line.pop_back();

std::string parseErr;
auto parsed = protocol::from_json_response(line, &parseErr);
ASSERT_TRUE(parsed.has_value()) << "parse response failed: " << parseErr;

ASSERT_TRUE(parsed->result.has_value());
EXPECT_TRUE(parsed->result->isString());
EXPECT_EQ(parsed->result->asString(), "hi");
ASSERT_TRUE(parsed->id.has_value());
EXPECT_EQ(parsed->id->asInt(), 1);

sock.shutdown(tcp::socket::shutdown_both);
sock.close();

server.stop();
}