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
    QTextEdit* msg_output { nullptr };
    QLineEdit* msg_input { nullptr };
};

#endif // INPUTDIALOG_H
