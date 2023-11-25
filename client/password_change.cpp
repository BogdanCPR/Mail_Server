#include "password_change.h"
#include "ui_password_change.h"

Password_change::Password_change(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Password_change)
{
    ui->setupUi(this);
}

Password_change::~Password_change()
{
    delete ui;
}

void Password_change::on_pushButton_new_passw_clicked()
{
    QString new_pass = ui->lineEdit_new_passw->text();
    QString conf_passw = ui->lineEdit_conf_passw->text();

    if(new_pass == conf_passw)
    {
        this->close();
    }
    else
    {
        QMessageBox *msg = new QMessageBox();
        msg->setStyleSheet("background-color: rgb(255, 255, 255);");
        msg->setText("Passwords do not match!");
        msg->setWindowTitle("Register");
        msg->setIcon(QMessageBox::Warning);
        msg->exec();

        delete msg;
    }
}

