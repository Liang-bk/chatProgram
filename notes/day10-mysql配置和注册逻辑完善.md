数据库User表应该是什么样的？

用户名：可重复

账户名：不可重复，唯一

邮箱：可以使用同一个邮箱创建多个账户

```mysql
CREATE TABLE users (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    -- id: 内部唯一标识符，主键，自增长，无业务含义，通常用于内部关联和索引。
    -- BIGINT UNSIGNED: 保证足够大的范围，且无负数。

    uid VARCHAR(255) NOT NULL UNIQUE,
    -- uid: 类似QQ号/微信号的唯一标识符。
    --      VARCHAR(20): 考虑到QQ号或微信号通常是数字或字母数字组合，且长度有限。
    --      NOT NULL: 不能为空。
    --      UNIQUE: 确保每个账户的uid都是唯一的。这是核心的唯一性约束。
    --      这个uid字段将是用户在登录、查找朋友时使用的主要“账号”。

    username VARCHAR(255) NOT NULL,
    -- username: 用户名，可以重复。
    --           VARCHAR(255): 常用字符串长度。
    --           NOT NULL: 不能为空。
    --           允许重复意味着你可能需要组合 uid 或 id 来唯一标识一个用户在界面上的显示。

    email VARCHAR(255),
    -- email: 邮箱地址。
    --        VARCHAR(255): 常用字符串长度。
    --        允许为 NULL: 如果用户注册时可以不提供邮箱。
    --        不加 UNIQUE: 符合“一个邮箱可以对应多个账户”的需求。

    password_hash VARCHAR(255) NOT NULL,
    -- password_hash: 存储密码的哈希值（绝不能直接存储明文密码）。
    --                VARCHAR(255): 存储哈希值（如 bcrypt 生成的哈希）通常足够。
    --                NOT NULL: 密码必须存在。

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- created_at: 记录账户创建的时间。

    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
    -- updated_at: 记录账户最后更新的时间，每次更新记录时自动更新。
);

-- 添加索引 (可选但推荐，特别是对常用查询字段)
-- 为 username 添加索引，因为即使不唯一，也可能常用作查询条件
CREATE INDEX idx_username ON users (username);
-- 如果 email 经常被用来查询（尽管不是唯一），也可以添加索引
-- CREATE INDEX idx_email ON users (email);
```





加入排他锁：

```mysql
CREATE TABLE email_account_counts (
    email VARCHAR(255) PRIMARY KEY, -- 邮箱地址作为主键
    count INT NOT NULL DEFAULT 0,   -- 该邮箱已注册的账户数量
    version INT NOT NULL DEFAULT 1  -- 乐观锁版本号 (可选，但推荐)
);
```





存储过程：(id无论成功还是失败都会自增，除非改为手动自增)

```mysql
CREATE DEFINER=`root`@`localhost` PROCEDURE `register_user`(
    IN p_uid VARCHAR(20),
    IN p_username VARCHAR(255),
    IN p_email VARCHAR(255),
    IN p_password_hash VARCHAR(255),
    OUT p_status INT,           -- 0: 成功, 1: 邮箱账户数量超限, 2: UID重复, 3: 未知错误
    OUT p_new_user_id BIGINT    -- 如果成功，返回新用户的 ID
)
BEGIN
    DECLARE current_email_count INT DEFAULT 0;
		proc_main: BEGIN
    -- 声明专门用于处理唯一约束冲突的 HANDLER
    DECLARE EXIT HANDLER FOR 1062 -- MySQL 错误码 1062 (ER_DUP_ENTRY)
    BEGIN
        SET p_status = 2; -- UID重复
        -- 不 LEAVE PROCEDURE，让事务回滚，并由外部应用处理重试UID
    END;

    -- 通用异常处理器，确保返回错误状态
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK; -- 发生任何未捕获的SQL异常时回滚
        SET p_status = 3; -- 未知错误
    END;

    SET p_status = 0; -- 默认设置为成功
    SET p_new_user_id = NULL;

    START TRANSACTION; -- 开启事务

    -- Step 1: 尝试获取并锁定 email_account_counts 表中的行
    -- 如果行不存在，则插入；如果存在，则锁定并读取
    INSERT INTO email_account_counts (email, count, version)
    VALUES (p_email, 0, 1)
    ON DUPLICATE KEY UPDATE version = version; -- 仅为 ON DUPLICATE KEY UPDATE 语法需要，不实际更新 count

    -- 获取当前邮箱的账户数量，并施加排他锁 (FOR UPDATE)
    -- 这将阻塞其他尝试对同一邮箱行进行 FOR UPDATE 的事务
    SELECT count INTO current_email_count
    FROM email_account_counts
    WHERE email = p_email FOR UPDATE;

    -- Step 2: 检查邮箱账户数量是否超限
    IF current_email_count >= 3 THEN
        SET p_status = 1; -- 邮箱账户数量超限
        ROLLBACK; -- 回滚事务
        LEAVE proc_main; -- 退出存储过程
    END IF;

    -- Step 3: 插入新用户到 users 表
    INSERT INTO users (uid, username, email, password_hash)
    VALUES (p_uid, p_username, p_email, p_password_hash);

    SET p_new_user_id = LAST_INSERT_ID();

    -- Step 4: 更新 email_account_counts 表的计数
    UPDATE email_account_counts
    SET count = count + 1
    WHERE email = p_email;

    COMMIT; -- 提交事务
	
		END proc_main;
END
```

使用代码调用存储过程需要额外注意：需要先初始化两个out的变量，然后再select

mysql-connector注意事项：execute()可能有返回集，需要主动或被动的消费掉，才能执行下一条命令，否则会报错：Commands out of sync; you can't run this command now

