# WeChat 项目回顾总览

本项目是一个分布式 IM（即时通讯）系统，核心后端以 C++ 实现，客户端为 Qt，辅以 gRPC/HTTP/TCP 三种通信链路。本文档是整体回顾入口，其它专题文档会在此统一引用。

## 文档结构
- **项目回顾总览**：本文件（系统职责、端到端流程、数据与并发概览）
- **完整 API 清单**：[`01-api-reference.md`](01-api-reference.md)
- **Boost.Asio/Beast 专题**：[`02-boost-asio-beast.md`](02-boost-asio-beast.md)
- **gRPC 分布式专题**：[`03-grpc-distributed.md`](03-grpc-distributed.md)
- **多线程/线程池专题**：[`04-threading-threadpool.md`](04-threading-threadpool.md)
- **C++ 新特性实战**：[`05-modern-cpp.md`](05-modern-cpp.md)
- **外部 C++ 库清单**：[`06-external-libs.md`](06-external-libs.md)
- **风险与改进建议**：[`07-risks-improvements.md`](07-risks-improvements.md)

## 术语与约定
- **GateServer**：HTTP 网关服务，负责验证码、注册、登录入口（Boost.Beast）。
- **VerifyServer**：验证码服务，gRPC 对外提供 GetVerifyCode（当前实现为 Node.js）。
- **StatusServer**：用户状态与 ChatServer 分配服务（C++/gRPC）。
- **ChatServer**：TCP 长连接聊天服务（Boost.Asio）。
- **Client**：Qt 客户端，HTTP 登录 + TCP 长连通信。
- **统一错误码**：ErrorCodes（见各服务 `const.h`），跨 HTTP/gRPC/TCP 复用。
- **协议约定**：HTTP/JSON、gRPC/Protobuf、TCP 自定义帧（详见 API 文档）。

## 系统总览
### 组件职责
- **GateServer**：
  - HTTP 接入点：验证码、注册、密码重置、登录。
  - 通过 gRPC 调用 VerifyServer 获取验证码。
  - 通过 gRPC 调用 StatusServer 获取 ChatServer 地址和登录 token。
- **VerifyServer**：
  - gRPC 服务：生成验证码，写入 Redis 并发送邮件。
- **StatusServer**：
  - gRPC 服务：分配 ChatServer，发放 token 并在内存中验证。
- **ChatServer**：
  - TCP 长连接：登录认证、好友管理、聊天消息、历史与最近消息。
- **Qt 客户端**：
  - HTTP 登录/注册、TCP 长连聊天；消息体为 JSON。

### 通信链路
- **HTTP（Client ↔ GateServer）**：注册/登录/验证码。
- **gRPC（GateServer/ChatServer ↔ StatusServer，GateServer ↔ VerifyServer）**：服务间调用。
- **TCP（Client ↔ ChatServer）**：实时消息与好友管理。

## 端到端流程（高频面试口径）
1. **注册**：
   - Client → GateServer `/get_varifycode` 获取验证码（接口名与 verify 拼写不同，沿用代码实现）。
   - Client → GateServer `/user_register` 提交验证码与账号信息，写入 MySQL。
2. **找回密码**：
   - `/get_varifycode` 获取验证码；`/reset_password` 更新密码。
3. **账号/邮箱登录**：
   - `/account_login` 或 `/email_login` 验证 MySQL。
   - GateServer 调用 StatusServer `GetChatServer` 获取 host/port/token。
4. **TCP 登录认证**：
   - Client 连接 ChatServer，发送 `ID_CHAT_LOGIN`（uid+token）。
   - ChatServer 调用 StatusServer `CheckToken` 验证。
5. **搜索用户**：
   - TCP `ID_SEARCH_USER_REQ` → MySQL 搜索 → `ID_SEARCH_USER_RSP`。
6. **好友请求与处理**：
   - `ID_ADD_FRIEND_REQ` 写入请求并通知对端。
   - `ID_AUTH_FRIEND_REQ` 接受/拒绝并通知对端。
7. **聊天消息**：
   - `ID_TEXT_CHAT_MSG_REQ` 写入消息并通知对端。
8. **历史与最近消息**：
   - `ID_GET_CHAT_HISTORY_REQ` 拉取历史；登录后主动推送好友列表、未处理请求、最近消息。

## 数据与存储
- **MySQL**：`ChatServer/database/chat_tables.sql`
  - `users`、`friendships`、`friend_requests`、`messages`。
  - 关键索引：会话维度、接收方时间维度。
- **Redis**：
  - VerifyServer 中验证码缓存（TTL 600s）。
  - C++ 侧提供 Redis 连接池封装（Gate/Chat/Status 均可用）。
- **StatusServer 内存 token**：
  - `StatusServiceImpl` 以 `unordered_map` 保存 token（进程内）。

## 并发与资源管理（概要）
- **Asio IO 线程池**：`AsioIOcontextPool`，Gate/Chat 复用。
- **TCP 会话管理**：ChatServer 使用 `CSession` 处理异步读写。
- **消息队列线程**：ChatServer `LogicSystem` 独立线程消费请求。
- **连接池**：MySQL/Redis/gRPC Stub/Channel 均有池化。

## 技术栈一览
- **语言**：C++（实际使用到 C++17/20 特性），Node.js（VerifyServer）。
- **网络**：Boost.Asio、Boost.Beast、gRPC/Protobuf。
- **存储**：MySQL Connector/C++ (X DevAPI)、Redis（hiredis）。
- **序列化**：nlohmann/json、Protobuf。
- **安全/加密**：OpenSSL（SHA-256）。
- **客户端**：Qt 6。

> 深潜细节请按专题文档阅读（见“文档结构”）。
