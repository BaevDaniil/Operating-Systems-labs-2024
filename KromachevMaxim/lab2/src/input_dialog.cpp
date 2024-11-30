#include "input_dialog.h"

InputDialog::InputDialog(QWidget *parent)
    : QWidget{parent}
{
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);

    lineEdit = new QLineEdit(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(textEdit);

    auto inputLayout = new QHBoxLayout();
    inputLayout->addWidget(lineEdit, 1);
    inputLayout->addWidget(buttonBox);

    mainLayout->addLayout(inputLayout);
}
