#ifndef VERIFICATIONCODEEDIT_H
#define VERIFICATIONCODEEDIT_H

#include "validatedlineedit.h"
#include <QPushButton>
class VerificationCodeEdit : public ValidatedLineEdit
{
    Q_OBJECT
public:
    VerificationCodeEdit(QWidget* parent = nullptr);
    ~VerificationCodeEdit();
    void startTimer();
signals:
    void requestCodeClicked();
private:
    void setupUI() override;
    void updateValidationState() override;
private slots:
    void onTextChanged() override;
    void onEditingFinished() override;
    void handleCodeBtnClicked();
    void handleTimeout();
private:
    QHBoxLayout* input_layout;
    QPushButton* code_button;
    QTimer* timer; // 倒数计时器
    int count; // 倒数多少个数

    static const int BUTTON_WIDTH = 80;
};

#endif // VERIFICATIONCODEEDIT_H
