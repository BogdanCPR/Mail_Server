#ifndef PASSWORD_CHANGE_H
#define PASSWORD_CHANGE_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class Password_change;
}

class Password_change : public QDialog
{
    Q_OBJECT

public:
    explicit Password_change(QWidget *parent = nullptr);
    ~Password_change();

private slots:
    void on_pushButton_new_passw_clicked();

private:
    Ui::Password_change *ui;
};

#endif // PASSWORD_CHANGE_H
