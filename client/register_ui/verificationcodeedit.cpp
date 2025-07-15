#include "verificationcodeedit.h"


VerificationCodeEdit::VerificationCodeEdit(QWidget *parent)
    : ValidatedLineEdit(parent), count(5)
{
    VerificationCodeEdit::setupUI();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VerificationCodeEdit::handleTimeout);
}

VerificationCodeEdit::~VerificationCodeEdit()
{
    timer->stop();
}

void VerificationCodeEdit::startTimer()
{
    // 先禁用
    code_button->setDisabled(true);
    count = 5;
    code_button->setText(QString::number(count));
    timer->start(1000);
}

void VerificationCodeEdit::setupUI()
{
    // 先把原输入框移除掉
    m_frameLayout->removeWidget(m_lineEdit);
    // 输入框和toolbutton水平排列
    input_layout = new QHBoxLayout();
    input_layout->setContentsMargins(0, 0, 0, 0);
    input_layout->setSpacing(SPACING);
    //
    code_button = new QPushButton();
    code_button->setMinimumWidth(BUTTON_WIDTH);
    code_button->setText("获取验证码");

    // 输入框和eye-button加到水平布局内
    input_layout->addWidget(m_lineEdit);
    input_layout->addWidget(code_button);
    // 再放回主布局
    m_frameLayout->insertLayout(0, input_layout);
    //
    connect(code_button, &QPushButton::clicked, this, &VerificationCodeEdit::handleCodeBtnClicked);
}

void VerificationCodeEdit::updateValidationState()
{

}

void VerificationCodeEdit::onTextChanged()
{

}

void VerificationCodeEdit::onEditingFinished()
{

}

void VerificationCodeEdit::handleCodeBtnClicked()
{
    qDebug() << "点击了获取验证按按钮\n";
    // 转发给外层处理
    emit requestCodeClicked();
}

void VerificationCodeEdit::handleTimeout()
{
    count--;
    if (count <= 0) {
        timer->stop();
        count = 5;
        code_button->setText("获取验证码");
        // 倒计时结束再取消禁用
        code_button->setEnabled(true);
        return;
    }
    // 倒计时的效果
    code_button->setText(QString::number(count));
}
