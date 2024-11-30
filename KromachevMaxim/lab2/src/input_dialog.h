#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "includes.h"

class InputDialog : public QWidget
{
    Q_OBJECT

public:
    explicit InputDialog(QWidget *parent = nullptr);
    ~InputDialog() override = default;

private:
    QTextEdit* textEdit { nullptr };
    QLineEdit* lineEdit { nullptr };
};

#endif // INPUTDIALOG_H
