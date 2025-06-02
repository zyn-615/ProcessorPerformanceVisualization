#include "forms/mainwindow.h" // Your main window class
#include "forms/chipwindow.h" // Chip window class
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // Manages GUI application resources
    
    MainWindow w;              // Create an instance of your main window
    w.show();                  // Show the window
    
    // ChipWindow cw;              // Create an instance of chip window
    // cw.show();                  // Show the chip window
    
    return a.exec();           // Start the application's event loop
}

