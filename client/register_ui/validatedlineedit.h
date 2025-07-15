#ifndef VALIDATEDLINEEDIT_H
#define VALIDATEDLINEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QPropertyAnimation>
#include <functional>
#include <QAction>
#include <QPushButton>
#include <QWidgetAction>
class ValidatedLineEdit : public QWidget
{
    Q_OBJECT

public:
    explicit ValidatedLineEdit(QWidget *parent = nullptr);
    virtual ~ValidatedLineEdit();

    // 设置占位符文本
    void setPlaceholderText(const QString &text);

    // 设置验证函数，返回空字符串表示验证通过，否则返回错误信息
    void setValidator(std::function<QString(const QString&)> validator);

    // 获取输入文本
    QString text() const;

    // 设置输入文本
    void setText(const QString &text);

    // 获取验证状态
    bool isValid() const;

    // 手动触发验证
    void validate();

protected slots:
    virtual void onTextChanged();
    virtual void onEditingFinished();

protected:
    virtual void setupUI();
    virtual void updateValidationState();
    void showErrorMessage(const QString &message);
    void hideErrorMessage();
    void setInputBorderColor(const QString &color);
    void adjustHeight();
protected:
    QLineEdit *m_lineEdit;  // 基本输入框
    QLabel *m_errorLabel;   // 错误提示
    QVBoxLayout *m_mainLayout;
    QFrame *m_innerFrame;
    QVBoxLayout *m_frameLayout;
    // QHBoxLayout *m_inputLayout;

    std::function<QString(const QString&)> m_validator;
    QString m_lastValidationResult;
    bool m_isValid;
    bool m_errorVisible;

    QTimer *m_validationTimer;

    // 高度常量
    static const int INPUT_HEIGHT = 40;
    static const int ERROR_HEIGHT = 12;
    static const int SPACING = 2;
    static const int BASE_HEIGHT = INPUT_HEIGHT + SPACING;
    static const int EXPANDED_HEIGHT = INPUT_HEIGHT + ERROR_HEIGHT + SPACING;
};

#endif // VALIDATEDLINEEDIT_H
