-- 检查并创建数据库
CREATE DATABASE IF NOT EXISTS wechat;
USE wechat;

-- 检查用户表是否存在
SET @table_exists = 0;
SELECT COUNT(*) INTO @table_exists FROM information_schema.tables 
WHERE table_schema = 'wechat' AND table_name = 'user';

-- 如果旧表存在则重命名
SET @rename_sql = IF(@table_exists = 1, 'ALTER TABLE wechat.user RENAME TO users', 'SELECT 1');
PREPARE stmt FROM @rename_sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- 检查新表是否存在
SELECT COUNT(*) INTO @table_exists FROM information_schema.tables 
WHERE table_schema = 'wechat' AND table_name = 'users';

-- 如果新表不存在则创建
SET @create_users_sql = IF(@table_exists = 0, 'CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50) NOT NULL,
    email VARCHAR(100) NOT NULL,
    password VARCHAR(64) NOT NULL,
    avatar VARCHAR(255) NULL DEFAULT "/img/avatar/default.jpg",
    status VARCHAR(50) NULL DEFAULT "online",
    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_login DATETIME NULL
)', 'SELECT 1');
PREPARE stmt FROM @create_users_sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- 如果表已存在但缺少新字段，则添加这些字段
ALTER TABLE users 
ADD COLUMN IF NOT EXISTS avatar VARCHAR(255) NULL DEFAULT '/img/avatar/default.jpg',
ADD COLUMN IF NOT EXISTS status VARCHAR(50) NULL DEFAULT 'online',
ADD COLUMN IF NOT EXISTS create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
ADD COLUMN IF NOT EXISTS last_login DATETIME NULL;

-- 好友关系表
CREATE TABLE IF NOT EXISTS friendships (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user1_id INT NOT NULL,     -- 较小的用户ID
    user2_id INT NOT NULL,     -- 较大的用户ID
    friendship_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY unique_friendship (user1_id, user2_id),
    CONSTRAINT fk_friendship_user1 FOREIGN KEY (user1_id) REFERENCES users(id),
    CONSTRAINT fk_friendship_user2 FOREIGN KEY (user2_id) REFERENCES users(id)
);

-- 好友请求表
CREATE TABLE IF NOT EXISTS friend_requests (
    id INT PRIMARY KEY AUTO_INCREMENT,
    from_uid INT NOT NULL,
    to_uid INT NOT NULL,
    message VARCHAR(255) NULL,
    request_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    response_time DATETIME NULL,
    status ENUM('pending', 'accepted', 'rejected') NOT NULL DEFAULT 'pending',
    CONSTRAINT fk_request_from FOREIGN KEY (from_uid) REFERENCES users(id),
    CONSTRAINT fk_request_to FOREIGN KEY (to_uid) REFERENCES users(id)
);

-- 消息表
CREATE TABLE IF NOT EXISTS messages (
    id INT PRIMARY KEY AUTO_INCREMENT,
    from_uid INT NOT NULL,
    to_uid INT NOT NULL,
    content TEXT NOT NULL,
    send_time DATETIME NOT NULL,
    msg_type TINYINT NOT NULL DEFAULT 0,  -- 0=文本, 1=图片, 2=文件, 3=语音, 4=视频
    status TINYINT NOT NULL DEFAULT 0,     -- 0=发送中, 1=已发送, 2=已送达, 3=已读, 4=发送失败
    CONSTRAINT fk_message_from FOREIGN KEY (from_uid) REFERENCES users(id),
    CONSTRAINT fk_message_to FOREIGN KEY (to_uid) REFERENCES users(id),
    INDEX idx_conversation (from_uid, to_uid, send_time),
    INDEX idx_receiver_time (to_uid, send_time)
);
