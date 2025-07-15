#include "passwordlineedit.h"

PasswordLineEdit::PasswordLineEdit(QWidget *parent) : ValidatedLineEdit(parent)
{
    // 父类构造调用setupUI构造基本布局
    PasswordLineEdit::setupUI();
}

void PasswordLineEdit::setupUI()
{
    // 先把原输入框移除掉
    m_frameLayout->removeWidget(m_lineEdit);
    // 输入框和toolbutton水平排列
    input_layout = new QHBoxLayout();
    input_layout->setContentsMargins(0, 0, 0, 0);
    input_layout->setSpacing(0);
    // check在输入框内部右侧
    eye_button = new QToolButton();
    eye_button->setIcon(QIcon(":/res/view-off.svg"));
    check = m_lineEdit->addAction(
                  QIcon(":/res/check.svg"),
                  QLineEdit::TrailingPosition
    );
    check->setVisible(false);
    // 输入框和eye-button加到水平布局内
    input_layout->addWidget(m_lineEdit);
    input_layout->addWidget(eye_button);
    m_lineEdit->setEchoMode(QLineEdit::Password);
    // 再放回主布局
    m_frameLayout->insertLayout(0, input_layout);
    connect(eye_button, &QToolButton::pressed, this, &PasswordLineEdit::onEyePressed);
    connect(eye_button, &QToolButton::released, this, &PasswordLineEdit::onEyeReleased);
}

void PasswordLineEdit::updateValidationState()
{
    // 没有验证器
    if (!m_validator) {
        m_isValid = true;
        return;
    }

    QString text = m_lineEdit->text();
    QString errorMessage = m_validator(text);

    if (errorMessage.isEmpty()) {
        // 验证通过
        m_isValid = true;
        hideErrorMessage();
        check->setVisible(true);
        if (!text.isEmpty()) {
            setInputBorderColor("#4CAF50");
        } else {
            setInputBorderColor("#e0e0e0");
        }
    } else {
        // 验证失败
        m_isValid = false;
        showErrorMessage(errorMessage);
        check->setVisible(false);
        setInputBorderColor("#FF5252");
    }

    m_lastValidationResult = errorMessage;
}

void PasswordLineEdit::onEyePressed()
{
    m_lineEdit->setEchoMode(QLineEdit::Normal);
    eye_button->setIcon(QIcon(":/res/view.svg"));
}

void PasswordLineEdit::onEyeReleased()
{
    m_lineEdit->setEchoMode(QLineEdit::Password);
    eye_button->setIcon(QIcon(":/res/view-off.svg"));
}
