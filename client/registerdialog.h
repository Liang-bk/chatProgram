#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"
namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_get_code_clicked(); // 点击获取按钮的后续操作
    void on_confirm_btn_clicked(); // 点击确认按钮的后续操作
    void slotRegisterModFinish(ReqId id, QString res, ErrorCodes err);

private:
    void initHttpHandlers();
    void showTip(QString str, bool b_ok);

    QMap<ReqId, std::function<void(const QJsonObject&)>> handlers_;
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
