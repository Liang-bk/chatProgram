#include "checklineedit.h"

CheckLineEdit::CheckLineEdit(QWidget *parent) : ValidatedLineEdit(parent)
{
    CheckLineEdit::setupUI();
}

void CheckLineEdit::setupUI()
{
    // 垂直布局: 输入框, 错误提示
    // 父类构造函数已经调用过了, 无需重新调用
    // check图标, 嵌入到输入框右侧
    check = m_lineEdit->addAction(
        QIcon(":/res/check.svg"),
        QLineEdit::TrailingPosition
    );
    // 初始设置为不可见
    check->setVisible(false);

}

void CheckLineEdit::updateValidationState()
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
