# Qt客户端编译说明

## 环境要求

- Qt 6.10+
- CMake 3.20+
- C++17 编译器

## 编译步骤

### 1. 使用Qt Creator

1. 打开 `WeChat.pro` 文件
2. 配置项目，选择Kit（Qt 6.10）
3. 点击构建

### 2. 使用命令行

```bash
cd qt/WeChat
mkdir build
cd build
qmake ../WeChat.pro
make
```

### 3. 依赖项

确保已安装以下Qt模块：
- Qt Core
- Qt GUI
- Qt Network
- Qt Widgets

## 配置

在运行前，确保服务器端已启动：
- GateServer (HTTP 8888)
- StatusServer (gRPC 50053)
- ChatServer (TCP 8090/8091)
- VerifyServer (gRPC 50051)
- MySQL
- Redis

## 功能特性

- 账号/邮箱登录
- 联系人列表
- 单聊（文字+图片）
- 群聊（文字+图片）
- 好友管理
- 群组管理
- 消息历史
- 在线状态显示
