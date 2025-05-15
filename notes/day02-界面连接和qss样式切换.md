客户端设计：登录和注册界面的切换

具体实现效果就是在登录界面设置一个**注册**按钮，点击之后切换到注册界面

## 界面架构

—mainwindow

​	—logindialog

​	—registerdialog

logindialog和registerdialog作为mainwindow中的两个会话窗口组件存在。

```c++
// mainwindow中要有login和register的指针
LoginDialog *login_dlg_;
RegisterDialog *register_dlg_;
```

通过`setCentralWidget(login_dlg_)`和`register_dlg_->hide()`可以选择性地显示其中一个会话窗口。

记得设置窗口标志，并尽量让login和register窗口和mainwindow为同一大小，以实现嵌入（login和register窗口直接在mainwindow中显示）的效果

```c++
login_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
register_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
```



## 连接逻辑

注册按钮属于login的私有属性，无法直接被外部访问，所以在login中额外创建信号

1. 在login中创建一个公有信号

   ```c++
   signals:
       void switchRegister();
   ```

2. 在mainwindow中创建槽函数来接收

   ```c++
   public slots:
       void slotSwitchRegister();
   ```

3. 槽函数的具体操作就是显示register会话窗口并将login会话窗口隐藏

4. 注册的取消按钮逻辑同上

## QSS样式切换

在register的会话窗口上设置了错误提示的lable`err_tip`，需要在适当的时候为其设置不同的颜色：

1. 通用的刷新函数（刷新qss）

   ```c++
   extern std::function<void(QWidget*)> repolish;
   // 刷新组件
   std::function<void(QWidget*)> repolish = [](QWidget* w) {
       w->style()->unpolish(w);
       w->style()->polish(w);
   };
   ```

2. qss样式设置

   ```c++
   // 对应着name这个组件的两种不同样式
   #name['state'='xxx1'] {
   	// 属性, 如color等
   	color: red;
   	...
   }
   #name['state'='xxx2'] {
   	// 属性, 如color等
   	color: green;
   	...
   }
   ```

3. 在对应的函数中调用repolish刷新（以刷新error_tip为例）：

   ```c++
   void RegisterDialog::showTip(QString str, bool b_ok)
   {
       if (b_ok) {
           // 更新样式
           ui->err_tip->setProperty("state", "normal");
       } else {
           ui->err_tip->setProperty("state", "err");
       }
       ui->err_tip->setText(str);
       repolish(ui->err_tip);
   }
   ```

