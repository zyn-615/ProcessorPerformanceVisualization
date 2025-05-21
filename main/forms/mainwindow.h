#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; } // Forward declaration for the UI class generated from mainwindow.ui
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT // Macro for enabling signals and slots

public:
    MainWindow(QWidget *parent = nullptr); // Constructor
    ~MainWindow();                         // Destructor

private:
    Ui::MainWindow *ui; // Pointer to the UI definition
};
#endif // MAINWINDOW_H 