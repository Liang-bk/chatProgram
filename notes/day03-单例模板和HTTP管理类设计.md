## 单例模板

客户端的网络请求和程序是一体化的，这样方便在任何时候需要发送http请求时调用。

为此，首先创建一个单例模板类（后续可能有更多的单例设计，比如日志模块）：

```c++
template<typename T>
class Singleton {
// protect 使子类能够访问
protected:
    Singleton() = default;
    // 不允许拷贝赋值
    Singleton(const Singleton<T>&) = delete;
    Singleton operator = (const Singleton<T>& other) = delete;

    static std::shared_ptr<T> instance_;
public:
    static std::shared_ptr<T> getInstance() {
        // 只初始化1次
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            // 要用make_shared, 需要在子类中设置友元, 否则会报访问错误
            // 在子类中将Singleton类设为友元, 可以new子类, 否则也会报错
            instance_ = std::shared_ptr<T>(new T);
        });
        return instance_;

    }
    /* 更简单的写法: (instance_就无用了)
    static T& getInstance() {
        static T instance;
        return instance;
    }
    */
    void printAddress() {
        std::cout << instance_.get() << std::endl;
    }
    ~Singleton() {
        std::cout << "This is Singleton destruction." << std::endl;
    }

};
// 类内static变量初始化
template<typename T>
std::shared_ptr<T> Singleton<T>::instance_ = nullptr;
```

## HttpManager（HTTP管理类）

该类主要管理http请求发送和响应接收。

首先在.pro文件中添加网络库**network**

```c++
QT       += core gui network
```

### CRTP技术

**派生类Derived将自身作为模板参数传递给基类模板，这样可以在基类的实现中访问特定的类型的this指针**，从而在编译期多态，相比较于虚函数运行多态，没有查虚函数表过程，效率要比虚函数多态高。

```c++
class HttpManager : public QObject, public Singleton<HttpManager>, //(CRTP)
                public std::enable_shared_from_this<HttpManager>
{
    Q_OBJECT
public:
    // 析构设置为public, 那么基类的instance_指针在释放的时候可以正确的访问子类的析构函数
    ~HttpManager();
    void postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
private:
    // 使基类能够new出子类的对象
    friend class Singleton<HttpManager>;
    explicit HttpManager(QObject *parent = nullptr);
    QNetworkAccessManager manager_;

private slots:
    void slotHttpFinish(ReqId id, QString res, ErrorCodes err, Modules mod);
signals:
    void sigHttpFinish(ReqId id, QString res, ErrorCodes err, Modules mod);
    void sigRegisterModFinish(ReqId id, QString res, ErrorCodes err);
};
```

### HttpPost

post请求发送的是一个表单`key1 = value1 && key2 = value2`，是一个json结构的数据

```c++
// url: 请求地址
// json: 要序列化的请求数据
// req_id: 表明是什么功能(注册/登录...)
// mod: 功能隶属于的模块
void postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
```

在global.h定义`ReqId`，`Modules`，`ErrorCodes`三个枚举类型：

```c++
/**
 * @brief The ReqId enum
 * 要实现的请求功能
 */
enum ReqId {
    ID_GET_VARIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002,         // 用户注册
};
/**
 * @brief The ErrorCodes enum
 * 错误码
 */
enum ErrorCodes {
    SUCCESS = 0,        // 成功
    ERR_JSON = 1,       // json解析失败
    ERR_NETWORK = 2     // 网络错误
};

/**
 * @brief The Modules enum
 * 功能隶属于的模块
 */
enum Modules {
    REGISTERMOD = 0,    // 注册模块
};
```

HttpPost函数实现逻辑：

1. 将json数据转为字节数据（序列化后才能在网络上发送）
2. 构造QNetworkRequest也就是Http请求的头部和数据部分
3. 使用QNetworkAccessManager的post的方法来发送http数据包（这里是异步，会立即返回）
4. 连接reply的信号和lambda函数构造接收到响应消息的处理逻辑

```c++
void HttpManager::postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    // 创建一个HTTP请求, 设置请求头和请求体
    // 将json对象转为一个字节数组
    QByteArray data = QJsonDocument(json).toJson();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    // 创建自己的指针, 引用计数+1
    auto self = shared_from_this();
    // 这里是异步发送
    QNetworkReply *reply = manager_.post(request, data);
    // 数据接收完成, 执行处理逻辑 finish信号和lambda的连接
    connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod]() {
        // 出错
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            emit self->sigHttpFinish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }

        QString res = reply->readAll();

        emit self->sigHttpFinish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}
```

关于post异步的理解：manager_.post()之后会立刻返回reply，此时reply中没有数据，并且程序继续向下执行connect，关键在于后面的lambda函数，其使用值捕获（拷贝）了postHttpReq中的局部变量，所以即使postHttpReq函数已经完成了，由于reply（动态内存）已经和lambda函数建立了connect关系，在未释放reply前，lambda函数和其捕获的变量副本会也一直存在，并通过事件循环来调用lambda回调函数。

### 信号和槽函数处理

```c++
signals:
	// Http请求完毕信号
	void sigHttpFinish(ReqId id, QString res, ErrorCodes err, Modules mod);
	// Http注册请求完毕信号
	void sigRegisterModFinish(ReqId id, QString res, ErrorCodes err);
private slots:
	// Http接收到响应后如何处理
    void slotHttpFinish(ReqId id, QString res, ErrorCodes err, Modules mod);
```

post回调函数→

发送sigHttpFinish完成信号→

HttpManager::slotHttpFinish接收→

判断应该归于哪个模块处理→

发送sigRegisterModFinish模块完成信号→

对应模块的槽函数去处理数据