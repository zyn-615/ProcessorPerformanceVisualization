#ifndef CHIPWINDOW_H
#define CHIPWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSplitter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QCloseEvent>

#include "../components/ClickableItems.h"
#include "../statistics/StatsLoader.h"
#include "../statistics/StatsDisplayer.h"
#include "../graphics/ChipRenderer.h"
#include "DataVisualizer.h"

namespace Ui {
class ChipWindow;
}

class ChipWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChipWindow(QWidget *parent = nullptr);
    ~ChipWindow();
    
    // 处理组件点击事件
    void onComponentClicked(const QString& componentId);

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void showPortTransmissionOverview();
    void resetDataPanel();

private:
    Ui::ChipWindow *ui;
    
    // 图形相关
    QGraphicsScene *scene;
    QGraphicsView *view;
    
    // 布局
    QSplitter *splitter;
    QWidget *dataPanel;
    QScrollArea *scrollArea;
    QVBoxLayout *dataPanelLayout;
    
    // 功能模块
    StatsLoader *statsLoader;
    ChipRenderer *chipRenderer;
    DataVisualizer *dataVisualizer;
    StatsDisplayer *statsDisplayer;
    
    // 初始化方法
    void setupChipView();
    void setupDataPanel();
};

#endif // CHIPWINDOW_H 