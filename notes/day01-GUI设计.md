## 添加界面文件

项目名→添加新文件→Qt 设计器界面类（ui/h）→命名

创建完毕后会生成界面文件（.ui）和一个对应的类（.h/.cpp）

## 界面设计

Qt的设计界面允许直接拖放部件并设置属性（高度/宽度）

一般的拖放可以直接用鼠标拖拉大小，但为了适应比例，一般采用下面的布局方式

### 水平垂直布局

1. 水平布局：将整个界面按列分割，一个部件会占用一列
2. 垂直布局：将整个界面按行分割，一个部件会占用一行

tips：先拖入一个垂直布局，然后在垂直布局里继续拖入水平布局可以实现每行一个水平布局，

然后水平布局里又继续分割为列的效果，实现不规则网格的效果。

**缺点**：目前来看的缺点是无法用鼠标调整大小，需要设置部件的max和minsize来同时决定一个部件的大小。

## 信号和槽

GUI大多是**事件驱动**模型，外部的鼠标点击，键盘点击被视为一个事件，GUI内部有一个循环在不断监听这些事件，当有事件发生时，循环获取这个事件并交使用对应的函数进行处理。

1. 信号（**signals**）：形式类似函数

   ```c++
   // 在类中声明信号
   signals:
       void switchRegister();
   ```

   信号类似于外部硬件事件在软件内部的表现形式，好比一个映射，比如在Qt界面上创建一个按钮，那么鼠标点击按钮事件在Qt内会被映射为`QPushButton::clicked`，然后由Qt内部的事件循环获知，再进行进一步处理（如果有connect的话）。

   上面的switchRegister是用户自定义的信号，正如电路传递一样，一个信号可以给另外一个信号发送信息从而进行更远的消息传递。

2. 槽函数（**slots**）：具体处理信号的函数

   槽的本质是一个回调函数，具体说明就是，例如`QPushButton::clicked`这个信号，Qt的事件循环捕捉到了外部的鼠标点击按钮这个事件，但由于clicked信号没有终点（没有一个函数来处理这个信号），那么在传递的过程中就会丢失，之后也不会提及，槽函数就相当于一个信号的终点（需要用户主动连接），信号传递到了终点，就会顺其自然的执行该函数。

3. 连接信号和槽函数（**connect**）：就是为一个信号设置一个回调函数，当事件驱动捕捉到一个信号时，顺着信号传递的方向进行处理：

   - 如果下一个还是信号，发出该信号，再由事件驱动循环捕捉
   - 是终点，执行该函数

   ```c++
   // sender: 信号的发送者(比如一个按钮对象button)
   // signal: 要发送的信号(QPushButton::clicked)
   // receiver: 信号的接收者(比如一个窗口widget)
   // method: 接收者的信号和槽函数(如果是信号, 会自动转发; 如果是槽函数, 会直接执行槽函数)
   connect(const QObject* sender,
   		const char* singal,
   		const QObject* receiver,
   		const char* method,
   		Qt::ConnectionType type = Qt::AutoConnection)
   ```

   比如`connect(btn, &QPushButton::clicked, this, &QWidget::closeWindow)`：

   就建立一个回调机制：当按钮被点击时，关闭当前窗口（closeWindow）是一个具体的槽函数，

   里面可以写关闭窗口的代码。

## 资源

项目名→添加新文件→Qt Resource File（qrc）→命名

资源通常用来保存一些图片，文字样式等，不存储具体代码逻辑

### qss

qss是Qt中一套设计样式的格式，类似css：

```qss
QDialog#LoginDialog, #RegisterDialog{
    background-color:rgb(255, 255, 255)
}

#user_label, pass_label{
    color:black;
}

#err_tip[state='normal']{
    color: green;
}
#err_tip[state='err']{
    color: red;
}
```

