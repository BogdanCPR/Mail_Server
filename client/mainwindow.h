#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "home.h"
#include "registration.h"
#include "password_change.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_register_clicked();

    void on_pushButton_forgot_password_clicked();

private:
    Ui::MainWindow *ui;    
};
#endif // MAINWINDOW_H
