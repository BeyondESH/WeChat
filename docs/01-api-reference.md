# API 文档（HTTP / gRPC / TCP）

关联文档：`00-project-overview.md`、`02-boost-asio-beast.md`、`03-grpc-distributed.md`。

## 通用约定
### 错误码（ErrorCodes）
来源：`GateServer/const.h`、`StatusServer/const.h`、`ChatServer/const.h`
- `SUCCESS(0)`：成功
- `ERR_NETWORK(1000)`：网络错误
- `ERROR_JSON(1001)`：JSON 解析错误
- `RPC_FAILED(1002)`：RPC 调用失败
- `VerifyCodeExpired(1003)`：验证码过期
- `VerifyCodeError(1004)`：验证码错误
- `UserNotExist(1005)`：用户不存在
- `UserExist(1006)`：用户已存在
- `PasswordError(1007)`：密码错误
- `EmailExist(1008)`：邮箱已存在
- `EmailNotExist(1009)`：邮箱不存在
- `PasswordUpFailed(1010)`：密码修改失败
- `ERR_MYSQL(1011)`：MySQL 连接错误
- `PasswordSame(1012)`：新旧密码相同
- `Token_Invalid(1013)`：Token 无效

### TCP 帧格式（ChatServer）
实现位置：`ChatServer/CSession.cpp`
- **头部 4 字节**：
  - 2 字节 `msg_id`（网络字节序）
  - 2 字节 `body_len`（网络字节序）
- **消息体**：JSON 字符串（UTF-8）

---

## HTTP API（GateServer）
处理位置：`GateServer/LogicSystem.cpp`

### 1) `GET /get_test`
- **作用**：连通性测试
- **响应**：纯文本 `receive get_test request`

### 2) `POST /get_varifycode`
- **作用**：申请邮箱验证码（调用 VerifyServer gRPC）
- **请求 JSON**：`{ "email": "xxx@example.com" }`
- **响应 JSON**：`{ "error": int, "email": string, "code": string }`
- **错误码**：`ERROR_JSON`、`RPC_FAILED`
- **处理位置**：`GateServer/LogicSystem.cpp`（`regPost("/get_varifycode")`）

### 3) `POST /user_register`
- **作用**：注册账号（校验 Redis 验证码 + 写入 MySQL）
- **请求 JSON**：`{ "email": string, "verifyCode": string, "user": string, "password": string }`
- **响应 JSON**：`{ "error": int, "email": string, "verifyCode": string, "user": string, "password": string }`
- **错误码**：`ERROR_JSON`、`VerifyCodeExpired`、`VerifyCodeError`、`UserExist`、`EmailExist`、`ERR_NETWORK`
- **处理位置**：`GateServer/LogicSystem.cpp`（`regPost("/user_register")`）

### 4) `POST /reset_password`
- **作用**：重置密码（校验 Redis 验证码 + 更新 MySQL）
- **请求 JSON**：`{ "email": string, "verifyCode": string, "password": string }`
- **响应 JSON**：`{ "error": int, "email": string, "verifyCode": string, "password": string }`
- **错误码**：`ERROR_JSON`、`VerifyCodeExpired`、`VerifyCodeError`、`EmailNotExist`、`PasswordSame`、`ERR_NETWORK`
- **处理位置**：`GateServer/LogicSystem.cpp`（`regPost("/reset_password")`）

### 5) `POST /account_login`
- **作用**：账号登录（MySQL 校验 + StatusServer 分配 ChatServer）
- **请求 JSON**：`{ "user": string, "password": string }`
- **响应 JSON**：`{ "error": int, "uid": int, "token": string, "host": string, "port": string, "user": string, "password": string }`
- **错误码**：`ERROR_JSON`、`ERR_MYSQL`、`UserNotExist`、`PasswordError`、`RPC_FAILED`
- **处理位置**：`GateServer/LogicSystem.cpp`（`regPost("/account_login")`）

### 6) `POST /email_login`
- **作用**：邮箱登录（MySQL 校验 + StatusServer 分配 ChatServer）
- **请求 JSON**：`{ "email": string, "password": string }`
- **响应 JSON**：`{ "error": int, "uid": int, "token": string, "host": string, "port": string, "email": string, "password": string }`
- **错误码**：`ERROR_JSON`、`ERR_MYSQL`、`EmailNotExist`、`PasswordError`、`RPC_FAILED`
- **处理位置**：`GateServer/LogicSystem.cpp`（`regPost("/email_login")`）

---

## gRPC API（VerifyServer / StatusServer）
Proto 定义：`GateServer/message.proto`、`ChatServer/message.proto`、`StatusServer/message.proto`、`VerifyServer/message.proto`

### VerifyService
- **RPC**：`GetVerifyCode(GetVerifyReq) returns (GetVerifyRsp)`
- **请求**：`{ email: string }`
- **响应**：`{ error: int32, email: string, code: string }`
- **实现**：`VerifyServer/server.js`
- **调用方**：`GateServer/VerifyGrpcClient.cpp`

### StatusService
- **RPC**：`GetChatServer(GetChatServerReq) returns (GetChatServerRsp)`
  - 请求：`{ uid: int32 }`
  - 响应：`{ error: int32, host: string, port: string, token: string }`
  - 实现：`StatusServer/StatusServiceImpl.cpp`
  - 调用方：`GateServer/StatusGrpcClient.cpp`

- **RPC**：`CheckToken(CheckTokenReq) returns (CheckTokenRsp)`
  - 请求：`{ uid: int32, token: string }`
  - 响应：`{ error: int32, uid: int32, token: string }`
  - 实现：`StatusServer/StatusServiceImpl.cpp`
  - 调用方：`ChatServer/StatusGrpcClient.cpp`、`GateServer/StatusGrpcClient.cpp`

---

## TCP API（ChatServer）
实现位置：`ChatServer/LogicSystem.cpp`、`ChatServer/CSession.cpp`

> 说明：部分消息 ID 在代码中被使用但未在 `ChatServer/const.h` 定义，文档中标记为“未显式定义”。

### 已定义消息 ID（`ChatServer/const.h`）
| ID | 名称 | 方向 | 作用 | 请求字段 | 响应字段 | 处理位置 |
| --- | --- | --- | --- | --- | --- | --- |
| 1005 | ID_CHAT_LOGIN | C→S | TCP 登录认证 | uid, token | error, token | `LogicSystem.cpp`, `CSession.cpp` |
| 1006 | ID_CHAT_LOGIN_RSP | S→C | 登录响应 | - | error, token | `CSession.cpp` |
| 1007 | ID_SEARCH_USER_REQ | C→S | 搜索用户 | keyword, searchType | users[] | `LogicSystem.cpp` |
| 1008 | ID_SEARCH_USER_RSP | S→C | 搜索用户响应 | - | error, users[] | `LogicSystem.cpp` |
| 1009 | ID_ADD_FRIEND_REQ | C→S | 发起好友请求 | fromUid, toUid, message | error | `LogicSystem.cpp` |
| 1010 | ID_ADD_FRIEND_RSP | S→C | 好友请求响应 | - | error | `LogicSystem.cpp` |
| 1011 | ID_NOTIFY_ADD_FRIEND_REQ | S→C | 好友请求通知 | fromUid, fromName, fromAvatar, message, requestTime | - | `LogicSystem.cpp` |
| 1013 | ID_AUTH_FRIEND_REQ | C→S | 处理好友请求 | requestId, fromUid, toUid, accept | error | `LogicSystem.cpp` |
| 1014 | ID_AUTH_FRIEND_RSP | S→C | 好友请求处理响应 | - | error | `LogicSystem.cpp` |
| 1015 | ID_NOTIFY_AUTH_FRIEND_REQ | S→C | 好友请求处理通知 | requestId, fromUid, fromName, toUid, accepted, message | - | `LogicSystem.cpp` |
| 1017 | ID_TEXT_CHAT_MSG_REQ | C→S | 发送文本消息 | fromUid, toUid, content | error, messageId, time | `LogicSystem.cpp` |
| 1018 | ID_TEXT_CHAT_MSG_RSP | S→C | 文本消息响应 | - | error, messageId, time | `LogicSystem.cpp` |
| 1019 | ID_NOTIFY_TEXT_CHAT_MSG_REQ | S→C | 文本消息通知 | messageId, fromUid, fromName, fromAvatar, content, time | - | `LogicSystem.cpp` |

### 未显式定义但已使用的消息 ID
| 名称 | 方向 | 作用 | 请求字段 | 响应字段 | 处理位置 | 备注 |
| --- | --- | --- | --- | --- | --- | --- |
| ID_FRIEND_LIST_REQ | C→S | 拉取好友列表 | uid | friends[] | `LogicSystem.cpp` | ID 未在 `const.h` 定义 |
| ID_FRIEND_LIST_RSP | S→C | 好友列表响应 | - | error, friends[] | `LogicSystem.cpp`, `CSession.cpp` | 登录后主动推送 |
| ID_GET_CHAT_HISTORY_REQ | C→S | 拉取聊天历史 | uid, contactUid, count, offset | messages[] | `LogicSystem.cpp` | ID 未在 `const.h` 定义 |
| ID_GET_CHAT_HISTORY_RSP | S→C | 聊天历史响应 | - | error, contact_uid, messages[] | `LogicSystem.cpp` | ID 未在 `const.h` 定义 |
| ID_FRIEND_REQUEST_LIST_RSP | S→C | 未处理好友请求列表 | - | error, requests[] | `CSession.cpp` | 登录后主动推送 |
| ID_RECENT_MESSAGES_RSP | S→C | 最近消息摘要 | - | error, recentMessages[] | `CSession.cpp` | 登录后主动推送 |

---

## 协议细节补充
- **HTTP**：短连接，`Content-Type: text/json`，请求体为 JSON。
- **gRPC**：Unary RPC，InsecureChannelCredentials（无 TLS）。
- **TCP**：自定义帧 + JSON，需先通过 `ID_CHAT_LOGIN` 认证后再处理其它消息。
