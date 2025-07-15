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
    void loadStyleSheet(const QString& file_path);
signals:
    void switchLogin();
private slots:
    void handleRequestCode(); // 点击获取按钮的后续操作
    void handleRequestRegistration(); // 点击确认按钮的后续操作
    void slotRegisterModFinish(ReqId id, QString res, ErrorCodes err);

private:
    void setupUI();
    void initHttpHandlers();
    // void showTip(QString str, bool b_ok);
    void showTipMessageBox(const QString& title, const QString& text, int timeout = 5000);

    QMap<ReqId, std::function<void(const QJsonObject&)>> handlers_;
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
