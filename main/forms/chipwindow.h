#ifndef CHIPWINDOW_H
#define CHIPWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ChipWindow; }
QT_END_NAMESPACE

class ChipWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChipWindow(QWidget *parent = nullptr);
    ~ChipWindow();

private:
    Ui::ChipWindow *ui;
};

#endif // CHIPWINDOW_H 