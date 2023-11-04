#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_login_clicked()
{
    Home *home = new Home();
    home->show();
    this->close();
}


void MainWindow::on_pushButton_register_clicked()
{
    Registration *reg = new Registration(this);
    reg->show();
}


void MainWindow::on_pushButton_forgot_password_clicked()
{
    Password_change *cp = new Password_change(this);
    cp->show();
}

