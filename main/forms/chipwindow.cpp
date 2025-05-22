#include "chipwindow.h"
#include "ui_chipwindow.h"

ChipWindow::ChipWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChipWindow)
{
    ui->setupUi(this);
}

ChipWindow::~ChipWindow()
{
    delete ui;
} 