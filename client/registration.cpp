#include "registration.h"
#include "ui_registration.h"

Registration::Registration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Registration)
{
    ui->setupUi(this);
}

Registration::~Registration()
{
    delete ui;
}

void Registration::on_pushButton_register_clicked()
{
    QString password = ui->lineEdit_passw->text();
    QString conf_password = ui->lineEdit_conf_passw->text();

    if(password == conf_password)
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

