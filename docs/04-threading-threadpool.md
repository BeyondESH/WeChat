# 多线程与线程池专题（基础语法 → 项目实战）

关联文档：`02-boost-asio-beast.md`、`03-grpc-distributed.md`。

## 1. 基础语法与并发工具
- `std::thread`：线程创建与管理。
- `std::mutex / std::lock_guard / std::unique_lock`：互斥与临界区保护。
- `std::condition_variable`：线程间同步等待/唤醒。
- `std::atomic_bool`：轻量级状态标记。

---

## 2. 项目中的线程与线程池实践
### 2.1 IO 线程池（Asio）
- **位置**：`GateServer/AsioIOcontextPool.*`、`ChatServer/AsioIOcontextPool.*`
- **机制**：
  - 创建 N 个 `io_context`。
  - 每个 `io_context` 启动一个线程 `run()`。
  - 使用轮询策略分配连接到不同 `io_context`。

### 2.2 ChatServer 逻辑处理线程
- **位置**：`ChatServer/LogicSystem.cpp`
- **机制**：
  - 网络线程只做解包与入队。
  - 逻辑线程通过 `std::queue + condition_variable` 拉取消息。
  - 限制队列上限 `QUEUE_MAX_SIZE`，防止堆积。

### 2.3 gRPC Stub/Channel 池
- **位置**：`GateServer/VerifyGrpcClient.cpp`、`GateServer/StatusGrpcClient.cpp`、`ChatServer/StatusGrpcClient.cpp`
- **机制**：
  - Stub 使用 `queue + mutex + condition_variable` 管理。
  - 借出/归还控制并发，减少连接创建成本。

### 2.4 MySQL/Redis 连接池
- **MySQL**：`ChatServer/MySQLMgr.*`、`GateServer/MySQLMgr.*`
  - 连接池 + 失效检测 + 后台重连。
- **Redis**：`GateServer/RedisMgr.*`、`ChatServer/RedisMgr.*`
  - 连接池 + 断线重连线程。

### 2.5 会话与发送队列
- **位置**：`ChatServer/CSession.cpp`
- **机制**：
  - `_send_queue` 保护并发写，防止并行 `async_write` 交错。

---

## 3. 面试可重点强调
- **分层并发模型**：IO 线程池负责网络，逻辑线程负责业务。
- **池化复用**：DB/Redis/gRPC 全部池化，提升吞吐与稳定性。
- **锁粒度控制**：短临界区，避免锁持有过久。

---

## 4. 可改进点
- 消息队列为单线程处理，吞吐可通过多消费者或任务队列提升。
- 连接池缺少监控指标（活跃数/等待时间/失败率）。

详见：`07-risks-improvements.md`。
