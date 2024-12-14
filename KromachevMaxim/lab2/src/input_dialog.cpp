#include "input_dialog.h"

InputDialog::InputDialog(QWidget *parent)
    : QWidget{parent}
{
    msg_output = new QTextEdit(this);
    msg_output->setReadOnly(true);

    msg_input = new QLineEdit(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(msg_output);

    auto inputLayout = new QHBoxLayout();
    inputLayout->addWidget(msg_input, 1);
    inputLayout->addWidget(buttonBox);

    mainLayout->addLayout(inputLayout);
    setWindowTitle("Чат");

    connect(buttonBox, &QDialogButtonBox::accepted, this, &InputDialog::on_send_clicked);
}

void InputDialog::append_msg(const std::string& msg)
{
    msg_output->append(QString::fromStdString("<<< " + msg));
}
