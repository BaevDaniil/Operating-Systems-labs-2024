#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "includes.h"

class InputDialog : public QWidget
{
    Q_OBJECT

public:
    explicit InputDialog(QWidget *parent = nullptr);
    ~InputDialog() override = default;
    void append_msg(const std::string& msg);

private slots:
    void on_send_clicked()
    {
        msg_input->clear();
        emit send_msg(msg_input->text().toStdString());
    }

signals:
    void send_msg(const std::string& msg);

private:
    QTextEdit* msg_output { nullptr };
    QLineEdit* msg_input { nullptr };
};

#endif // INPUTDIALOG_H
