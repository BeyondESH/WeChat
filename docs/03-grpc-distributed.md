# gRPC 分布式专题（基础语法 → 项目实战）

关联文档：`00-project-overview.md`、`01-api-reference.md`、`04-threading-threadpool.md`。

## 1. gRPC 基础语法与模型
- **Proto 定义**：`service` + `message`，支持强类型编解码。
- **Server 端**：`ServerBuilder` 注册服务，`BuildAndStart()` 启动。
- **Client 端**：`CreateChannel` + `Stub` 发起 RPC。
- **调用上下文**：`ClientContext / ServerContext` 携带超时、元信息。
- **错误处理**：`grpc::Status` 判断 `ok()`，业务错误码由应用层字段承载。

---

## 2. 项目中的 gRPC 实战
### 2.1 Proto 与服务定义
位置：
- `GateServer/message.proto`
- `ChatServer/message.proto`
- `StatusServer/message.proto`
- `VerifyServer/message.proto`

主要服务：
- **VerifyService**：`GetVerifyCode`（验证码服务）
- **StatusService**：`GetChatServer`（分配 ChatServer）
- **StatusService**：`CheckToken`（验证登录 token）

### 2.2 VerifyServer（Node.js）
- **实现位置**：`VerifyServer/server.js`
- **核心逻辑**：
  - 读取 Redis 中验证码；不存在则生成 6 位验证码。
  - 设置 TTL（600s），发送邮件（nodemailer）。
  - 通过 gRPC 返回 `error` 字段。

### 2.3 StatusServer（C++）
- **实现位置**：`StatusServer/StatusServiceImpl.cpp`、`StatusServer/main.cpp`
- **GetChatServer**：
  - 读取配置中的 ChatServer 列表。
  - 选择连接数较少的实例（简单负载均衡）。
  - 生成 UUID token，存入内存 map。
- **CheckToken**：
  - 校验 `uid → token` 是否匹配。

### 2.4 GateServer / ChatServer 客户端调用
- **VerifyGrpcClient**：`GateServer/VerifyGrpcClient.cpp`
  - Stub 池化，避免高频创建 Channel。
- **StatusGrpcClient**：`GateServer/StatusGrpcClient.cpp`、`ChatServer/StatusGrpcClient.cpp`
  - Stub 复用 + 条件变量管理。

---

## 3. 连接池与并发控制
- **Stub/Channel 池**：
  - `RPCStubPool` / `StatusConnectPool` 使用 `std::mutex` + `std::condition_variable`。
  - 限制并发创建成本，提升吞吐。
- **错误映射**：
  - gRPC 网络失败 → `RPC_FAILED`。
  - 业务失败 → `error` 字段传递。

---

## 4. 面试要点与亮点
- **服务解耦**：HTTP / gRPC / TCP 各司其职。
- **控制平面**：StatusServer 做连接分配与 token 验证。
- **高可用性方向**：Stub 池化 + gRPC 的连接复用。

---

## 5. 可改进点（与风险文档呼应）
- gRPC 目前使用 `InsecureChannelCredentials`，缺少 TLS。
- token 仅内存存储，重启即失效，需引入持久化或分布式存储。

详见：`07-risks-improvements.md`。
