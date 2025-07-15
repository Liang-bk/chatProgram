// ValidatedLineEdit.cpp
#include "validatedlineedit.h"
#include <QApplication>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QWidgetAction>
#include <QToolButton>
#include <QStyleOption>
#include <QPainter>
ValidatedLineEdit::ValidatedLineEdit(QWidget *parent)
    : QWidget(parent)
    , m_lineEdit(nullptr)
    , m_errorLabel(nullptr)
    , m_mainLayout(nullptr)
    , m_innerFrame(nullptr)
    , m_isValid(true)
    , m_errorVisible(false)
    , m_validationTimer(nullptr)
{
    ValidatedLineEdit::setupUI();
}

ValidatedLineEdit::~ValidatedLineEdit() = default;

void ValidatedLineEdit::setupUI()
{
    // 初始设置为基础高度
    setFixedHeight(BASE_HEIGHT);
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(SPACING);
    // 创建frame
    m_innerFrame = new QFrame();
    // m_innerFrame->setStyleSheet(
    //     "QFrame {"
    //     "    background-color: white;"
    //     "    border: 2px solid #e0e0e0;"
    //     "    border-radius: 6px;"
    //     "}"
    // );
    m_frameLayout = new QVBoxLayout(m_innerFrame);
    m_frameLayout->setContentsMargins(0, 0, 0, 0);
    m_frameLayout->setSpacing(0);

    // 创建输入框
    m_lineEdit = new QLineEdit();
    m_lineEdit->setFixedHeight(INPUT_HEIGHT);
    // 替换原来的 m_lineEdit->setStyleSheet()
    // m_lineEdit->setStyleSheet(
    //     "QLineEdit {"
    //     "    border: none;"
    //     "    background-color: transparent;"
    //     "    padding: 0 12px;"
    //     "    font-size: 14px;"
    //     "}"
    //     "QLineEdit:focus {"
    //     "    outline: none;"
    //     "}"
    //     );
    m_frameLayout->addWidget(m_lineEdit);

    // 添加到输入框布局
    m_mainLayout->addWidget(m_innerFrame);

    // 创建错误提示标签
    m_errorLabel = new QLabel();
    m_errorLabel->setFixedHeight(ERROR_HEIGHT);
    // m_errorLabel->setStyleSheet(
    //     "QLabel {"

    //     "}"
    //     );
    m_errorLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_errorLabel->hide();

    // 添加到主布局
    m_mainLayout->addWidget(m_errorLabel);

    // 创建验证定时器（防抖）
    m_validationTimer = new QTimer(this);
    m_validationTimer->setSingleShot(true);
    m_validationTimer->setInterval(300);

    // 连接信号
    connect(m_lineEdit, &QLineEdit::textChanged, this, &ValidatedLineEdit::onTextChanged);
    connect(m_lineEdit, &QLineEdit::editingFinished, this, &ValidatedLineEdit::onEditingFinished);
    connect(m_validationTimer, &QTimer::timeout, this, &ValidatedLineEdit::updateValidationState);
}

void ValidatedLineEdit::setPlaceholderText(const QString &text)
{
    if (m_lineEdit) {
        m_lineEdit->setPlaceholderText(text);
    }
}

void ValidatedLineEdit::setValidator(std::function<QString(const QString&)> validator)
{
    m_validator = validator;
}

QString ValidatedLineEdit::text() const
{
    return m_lineEdit ? m_lineEdit->text() : QString();
}

void ValidatedLineEdit::setText(const QString &text)
{
    if (m_lineEdit) {
        m_lineEdit->setText(text);
        validate();
    }
}

bool ValidatedLineEdit::isValid() const
{
    return m_isValid;
}

void ValidatedLineEdit::validate()
{
    m_validationTimer->stop();
    updateValidationState();
}

void ValidatedLineEdit::onTextChanged()
{
    // 输入时隐藏错误提示和成功图标
    hideErrorMessage();
    setInputBorderColor("#e0e0e0");

    // 重启验证定时器
    m_validationTimer->start();
}

void ValidatedLineEdit::onEditingFinished()
{
    // 失去焦点时立即验证
    m_validationTimer->stop();
    updateValidationState();
}

void ValidatedLineEdit::updateValidationState()
{
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
        if (!text.isEmpty()) {
            setInputBorderColor("#4CAF50");
        } else {
            setInputBorderColor("#e0e0e0");
        }
    } else {
        // 验证失败
        m_isValid = false;
        showErrorMessage(errorMessage);
        setInputBorderColor("#FF5252");
    }

    m_lastValidationResult = errorMessage;
}

void ValidatedLineEdit::showErrorMessage(const QString &message)
{
    if (!m_errorVisible) {
        m_errorVisible = true;
        m_errorLabel->setText(message);
        m_errorLabel->show();
        adjustHeight();
    } else {
        m_errorLabel->setText(message);
    }
}

void ValidatedLineEdit::hideErrorMessage()
{
    if (m_errorVisible) {
        m_errorVisible = false;
        m_errorLabel->hide();
        adjustHeight();
    }
}


void ValidatedLineEdit::setInputBorderColor(const QString &color)
{
    // 将边框颜色设置到Frame而不是lineEdit
    QString style = QString(
        "QFrame {"
        "    background-color: white;"
        "    border: 2px solid %1;"
        "    border-radius: 6px;"
        "}"
    ).arg(color);
    m_innerFrame->setStyleSheet(style);
}

void ValidatedLineEdit::adjustHeight()
{
    // 根据错误提示的显示状态调整高度
    int newHeight = m_errorVisible ? EXPANDED_HEIGHT : BASE_HEIGHT;

    // 创建高度变化动画
    QPropertyAnimation *animation = new QPropertyAnimation(this, "maximumHeight");
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(height());
    animation->setEndValue(newHeight);

    // 同时设置最小高度
    QPropertyAnimation *minAnimation = new QPropertyAnimation(this, "minimumHeight");
    minAnimation->setDuration(200);
    minAnimation->setEasingCurve(QEasingCurve::OutCubic);
    minAnimation->setStartValue(height());
    minAnimation->setEndValue(newHeight);

    // 启动动画
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    minAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

