#include "forms/mainwindow.h" // 主窗口类
#include "src/ui/ChipWindow.h" // 新的芯片窗口路径
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // 管理GUI应用程序资源
    
    MainWindow w;              // 创建主窗口实例
    w.show();                  // 显示窗口
    
    return a.exec();           // 启动应用程序事件循环
}

