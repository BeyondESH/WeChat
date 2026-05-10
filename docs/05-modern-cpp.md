# C++ 新特性实战总结（基于本项目代码）

关联文档：`00-project-overview.md`、`06-external-libs.md`。

> 项目实际使用到的特性覆盖 C++11/14/17/20，多数来自 C++17/20 标准库与现代工程实践。

## 1. C++17/20 关键特性落地
### 1.1 `std::filesystem`（C++17）
- **用途**：读取 `config.ini` 路径。
- **位置**：`GateServer/ConfigMgr.hpp`（`current_path()` / `parent_path()`）。

### 1.2 `std::map::contains`（C++20）
- **用途**：配置读取时判断 section 是否存在。
- **位置**：`GateServer/ConfigMgr.hpp`。
- **面试点**：提示项目在 C++17 基础上已引入 C++20 API。

## 2. 现代 C++ 资源管理（RAII）
### 2.1 智能指针
- **`std::unique_ptr`**：gRPC Stub 池、连接池、work_guard 管理。
- **`std::shared_ptr`**：会话对象、MySQL Session 管理。
- **位置**：`*GrpcClient.cpp`、`AsioIOcontextPool.*`、`MySQLMgr.*`。

### 2.2 RAII 辅助类
- **`SessionGuard`**：MySQL Session 自动归还。
- **`Defer`**：自动归还 gRPC Stub。
- **位置**：`ChatServer/MySQLMgr.*`、`GateServer/StatusGrpcClient.cpp`。

## 3. Lambda 与函数式回调
- **网络回调**：`async_accept`、`async_read`、`async_write`。
- **业务分发**：HTTP/TCP handler 注册。
- **位置**：`GateServer/HttpConnection.cpp`、`ChatServer/LogicSystem.cpp`。

## 4. 并发相关现代特性
- `std::thread`：IO 线程池、后台重连、消息处理线程。
- `std::mutex` / `std::lock_guard` / `std::unique_lock`：连接池与队列保护。
- `std::condition_variable`：连接池与消息队列同步。
- `std::atomic_bool`：连接池停止标记。

## 5. 其他实用特性
- **`auto` 类型推导**：提升可读性，减少模板噪声。
- **范围 for**：遍历配置、连接池、用户集合。
- **异常处理**：`try/catch` 包装网络与数据库异常路径。

---

## 面试建议口径
- 强调现代 C++ 在**资源管理、并发模型、异常安全**方面的工程价值。
- 能够结合具体文件举例说明（上方已列出）。
