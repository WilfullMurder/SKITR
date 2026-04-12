<p align="center">
<img src="https://github.com/WilfullMurder/SKITR/blob/main/docs/img/SKITR_logo.png" alt="SKITR logo" width="640"/>
</p>
A lightweight C++ framework for building JSON-RPC 2.0 servers and clients.  
It provides modular components for JSON-RPC messages, method dispatch, and network transport.

---

## Documentation

- **Architecture** – High-level C4 diagrams, component-level details, and sequence diagrams.  
  See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)

- **Getting Started** – Build instructions, example server/client, and prerequisites.  
  See [docs/GETTING_STARTED.md](docs/GETTING_STARTED.md)

---

## Features

- JSON-RPC 2.0 compliant requests, responses, notifications, and errors.
- Modular architecture:
    - `core` – JSON-RPC domain model (`Value`, `Type`, `RpcRequest`, `RpcResponse`, `RpcError`)
    - `protocol` – JSON serialization/deserialization
    - `registry` – Thread-safe method registration
    - `runtime` – Dispatcher for executing requests
    - `transport` – TCP server/client
- External libraries: [`asio`](https://think-async.com/Asio/), [`nlohmann::json`](https://github.com/nlohmann/json)
- Error handling via `RpcError` with standard and custom JSON-RPC error codes

---

## License

MIT License. See [LICENSE](LICENSE) for details.
