# 外部 C++ 库清单与使用点

关联文档：`00-project-overview.md`、`05-modern-cpp.md`。

> 本清单聚焦 C++ 侧依赖（Node.js 与 Qt 生态在“补充说明”中列出）。

## C++ 外部库
| 库 | 用途 | 主要使用点 | 备注 / 替代方案 |
| --- | --- | --- | --- |
| Boost.Asio | 异步 TCP IO | `GateServer/AsioIOcontextPool.*`、`ChatServer/CServer.cpp`、`ChatServer/CSession.cpp` | 可替代：libevent、asio standalone |
| Boost.Beast | HTTP 解析与响应 | `GateServer/HttpConnection.cpp` | 可替代：cpp-httplib、restinio |
| Boost.UUID | token/会话 ID 生成 | `StatusServer/StatusServiceImpl.cpp`、`ChatServer/CSession.cpp` | 可替代：libuuid |
| Boost.PropertyTree | 读取 INI 配置 | `*/ConfigMgr.hpp` | 可替代：inih、toml++ |
| gRPC / Protobuf | 服务间 RPC | `message.proto` + `*GrpcClient.cpp` + `StatusServiceImpl.cpp` | 可替代：brpc、thrift |
| MySQL Connector/C++ (X DevAPI) | MySQL 访问 | `*MySQLMgr.*` | 可替代：mysqlclient + ORM |
| hiredis | Redis 客户端 | `*RedisMgr.*` | 可替代：redis-plus-plus |
| nlohmann/json | JSON 解析与序列化 | `GateServer/LogicSystem.cpp`、`ChatServer/LogicSystem.cpp` | 可替代：rapidjson |
| OpenSSL (EVP) | SHA-256 密码哈希 | `*/Crypto.cpp` | 可替代：libsodium |

## 补充：非 C++ 依赖
- **VerifyServer (Node.js)**：`@grpc/grpc-js`、`ioredis`、`nodemailer`、`uuid`。
- **Qt 客户端**：Qt 6、Qt Network 模块、QSS 样式表。

---

## 使用建议（面试视角）
- 说明“为什么选”与“替代方案”，体现工程取舍能力。
- 重点强调 **Boost.Asio/Beast + gRPC + DB/Redis** 的高并发体系。
