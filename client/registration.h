#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class Registration;
}

class Registration : public QDialog
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent = nullptr);
    ~Registration();

private slots:
    void on_pushButton_register_clicked();

private:
    Ui::Registration *ui;
};

#endif // REGISTRATION_H
