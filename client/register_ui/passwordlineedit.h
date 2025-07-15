#ifndef PASSWORDLINEEDIT_H
#define PASSWORDLINEEDIT_H

#include "validatedlineedit.h"
#include <QWidgetAction>
#include <QToolButton>
class PasswordLineEdit : public ValidatedLineEdit
{
    Q_OBJECT
public:
    PasswordLineEdit(QWidget* parent = nullptr);

private:
    void setupUI() override;
    void updateValidationState() override;
private slots:
    void onEyePressed();
    void onEyeReleased();
private:
    QHBoxLayout* input_layout;
    QToolButton* eye_button;
    QAction* check;
};

#endif // PASSWORDLINEEDIT_H
