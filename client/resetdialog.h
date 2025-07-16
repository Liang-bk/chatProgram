#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include <global.h>
namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();
    void loadStyleSheet(const QString& file_path);

private:
    void setupUI();
    void initHttpHandlers();
    // void showTip(QString str, bool b_ok);
    void showTipMessageBox(const QString& title, const QString& text, int timeout = 5000);
private:
    QMap<ReqId, std::function<void(const QJsonObject&)>> handlers_;
    Ui::ResetDialog *ui;
signals:
    void switchLogin();

private slots:
    void handleRequestCode();
    void handleRequestReset(); // 点击确认按钮后的操作
    void handleResetFinish(ReqId id, QString res, ErrorCodes err);
};

#endif // RESETDIALOG_H
