## Getting Started

# SKITR Getting Started

This guide helps you build, run, and test SKITR locally.

## Requirements

- C++17 or newer
- CMake 3.20+
- A C++ compiler (GCC, Clang, MSVC)
- [ASIO](https://think-async.com/Asio/) (header-only)
- [nlohmann::json](https://github.com/nlohmann/json) (header-only)

---

## Build Instructions

```bash
# Clone the repository
git clone https://github.com/your-org/skitr.git
cd skitr

# Create a build directory
mkdir build && cd build

# Configure the project
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the server and libraries
cmake --build . --config Debug
```

## Using SKITR in your project
1. Include SKITR headers in your C++ project.
```
#include <skitr/core/value.h>
#include <skitr/core/message.h>
#include <skitr/core/error.h>
#include <skitr/runtime/dispatcher.h>
#include <skitr/registry/method.h>
#include <skitr/transport/server.h>
```
Example:
```cpp
// Initialize I/O context
asio::io_context ioc;

// Start RPC server
skitter::rpc::RpcServer server(ioc, 12345);
ioc.run();
```

## Testing JSON-RPC
Use the built-in client for testing by running the client executable from the build folder and sending JSON-RPC requests to the server.

Alternatively, use a JSON-RPC client (e.g., Python jsonrpcclient) to send requests:
```python
from jsonrpcclient import request
import requests

url = "http://localhost:12345"
response = request(url, "add", [1, 2])
print(response)
```

## Further Reading
- [Architecture Overview](architecture.md)
- [JSON-RPC 2.0 Specification](https://www.jsonrpc.org/specification)
- [ASIO Documentation](https://think-async.com/Asio/)
