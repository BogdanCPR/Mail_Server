#ifndef HOME_H
#define HOME_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class Home;
}

class Home : public QDialog
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    ~Home();

private slots:
    void on_pushButton_login_clicked();

private:
    Ui::Home *ui;
};

#endif // HOME_H
