# Boost.Asio / Beast 专题（基础语法 → 项目实战）

关联文档：`00-project-overview.md`、`01-api-reference.md`、`04-threading-threadpool.md`。

## 1. 基础语法与模型速览
### 1.1 核心概念
- **io_context**：事件循环与调度器，负责驱动异步 IO。
- **executor_work_guard**：防止 io_context 退出，维持线程运行。
- **acceptor / socket**：监听与连接对象。
- **buffer**：读写缓冲区。
- **异步回调（handler）**：`async_accept / async_read / async_write`。

### 1.2 典型写法要点
- **创建 io_context** → **创建 acceptor** → **async_accept** → **创建会话** → **async_read** → **async_write**。
- **多线程驱动**：多个线程调用 `io_context.run()` 实现并发。
- **错误处理**：通过 `boost::system::error_code` 分支处理。

### 1.3 Boost.Beast HTTP 处理
- **HTTP 解析**：`boost::beast::http::async_read` 解析请求。
- **HTTP 响应**：构造 `http::response`，`async_write` 回写。
- **短连接**：响应后 `shutdown` socket。

---

## 2. 项目中的 Asio/Beast 实战
### 2.1 IO 线程池与生命周期管理
- **实现位置**：`GateServer/AsioIOcontextPool.*`、`ChatServer/AsioIOcontextPool.*`
- **关键点**：
  - `std::thread::hardware_concurrency()` 决定 IO 线程数。
  - `executor_work_guard` 保活。
  - 轮询分配 io_context，实现负载均衡。

### 2.2 GateServer：HTTP 接入层（Beast）
- **监听与接入**：`GateServer/CServer.cpp`
  - `async_accept` 接新连接。
  - 为每个连接分配独立 `HttpConnection`。
- **请求处理**：`GateServer/HttpConnection.cpp`
  - `async_read` 解析 HTTP。
  - 根据 `GET/POST` 调用 `LogicSystem` 路由分发。
  - 统一响应并关闭连接。
- **超时控制**：`deadline_timer`（`HttpConnection::checkDeadLine`）。

### 2.3 ChatServer：TCP 长连接（Asio）
- **监听与会话管理**：`ChatServer/CServer.cpp`
  - `async_accept` 后创建 `CSession`。
  - `_sessions` 保存在线连接。
- **协议解析与粘包处理**：`ChatServer/CSession.cpp`
  - 先读 4 字节头（msg_id + len），再读 body。
  - `network_to_host_short` 解析网络字节序。
  - `MSG_MAX_LEN` 保护最大包长度。
- **异步读写**：
  - `async_read_some` 驱动循环读取。
  - 发送队列 + `async_write` 串行化发送。

### 2.4 StatusServer：信号处理与优雅退出
- **实现位置**：`StatusServer/main.cpp`
- 使用 `boost::asio::signal_set` 监听 SIGINT/SIGTERM，触发 gRPC Server Shutdown。

---

## 3. 设计价值（面试可讲）
- **高并发**：异步 IO + IO 线程池，避免阻塞。
- **资源隔离**：连接、会话、逻辑处理分层。
- **可维护性**：统一的回调分发（HTTP 路由 / TCP 消息 ID）。
- **可靠性**：消息长度校验、超时、错误码反馈。

---

## 4. 可改进点（与风险文档呼应）
- 缺少 TLS/HTTPS 支持（当前为明文）。
- TCP 协议仅 JSON 文本，缺少压缩/二进制优化。
- Handler 中的异常处理可更细化（例如统一日志/metrics）。

详见：`07-risks-improvements.md`。
