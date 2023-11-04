#include "home.h"
#include "ui_home.h"

Home::Home(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Home)
{
    ui->setupUi(this);
}

Home::~Home()
{
    delete ui;
}

void Home::on_pushButton_login_clicked()
{
    MainWindow *login = new MainWindow();
    login->show();
    close();
}

