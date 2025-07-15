#ifndef CHECKLINEEDIT_H
#define CHECKLINEEDIT_H

#include "validatedlineedit.h"

class CheckLineEdit : public ValidatedLineEdit
{
    Q_OBJECT
public:
    explicit CheckLineEdit(QWidget *parent = nullptr);

private:
    void setupUI() override;
    void updateValidationState() override;
private:
    QAction* check;
};

#endif // CHECKLINEEDIT_H
