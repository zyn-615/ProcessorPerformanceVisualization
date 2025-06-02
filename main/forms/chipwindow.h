#ifndef CHIPWINDOW_H
#define CHIPWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QSplitter>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QProgressBar>
#include <QGroupBox>
#include <QTextEdit>
#include <QMap>
#include <QMouseEvent>
#include <QPushButton>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class ChipWindow; }
QT_END_NAMESPACE

// 自定义可点击的图形项
class ClickableRectItem : public QGraphicsRectItem
{
public:
    ClickableRectItem(double x, double y, double width, double height, 
                     const QString& componentId, QGraphicsItem* parent = nullptr);
    QString getComponentId() const { return componentId; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QString componentId;
};

class ClickableEllipseItem : public QGraphicsEllipseItem
{
public:
    ClickableEllipseItem(double x, double y, double width, double height, 
                        const QString& componentId, QGraphicsItem* parent = nullptr);
    QString getComponentId() const { return componentId; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QString componentId;
};

// 统计数据结构
struct ComponentStats {
    QString name;
    QMap<QString, QString> data;
    QMap<QString, double> percentages;
};

class ChipWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChipWindow(QWidget *parent = nullptr);
    ~ChipWindow();
    void onComponentClicked(const QString& componentId);

signals:
    void windowClosed();  // 窗口关闭信号

protected:
    void closeEvent(QCloseEvent *event) override;  // 重写关闭事件

private slots:
    // 保留其他slots，移除onComponentClicked

private:
    Ui::ChipWindow *ui;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QSplitter *splitter;
    QWidget *dataPanel;
    QScrollArea *scrollArea;
    QVBoxLayout *dataPanelLayout;
    
    // 统计数据
    QMap<QString, ComponentStats> statsData;
    QMap<QString, QString> portTransmissions;  // 存储port之间的传输包数据
    
    void setupChipView();
    void setupDataPanel();
    void drawChipArchitecture();
    void loadStatistics();
    void showComponentData(const QString& componentId);
    void clearDataPanel();
    
    // 绘制组件的方法 (更新为可点击版本)
    ClickableRectItem* drawCPU(double x, double y, const QString& name, const QString& componentId);
    ClickableRectItem* drawL2Cache(double x, double y, const QString& name, const QString& componentId);
    ClickableRectItem* drawL3Cache(double x, double y, const QString& name, const QString& componentId);
    ClickableRectItem* drawDDR(double x, double y, const QString& componentId);
    ClickableEllipseItem* drawBusNode(double x, double y, int nodeId);
    
    // 绘制连接线的方法
    void drawThickLine(double x1, double y1, double x2, double y2);
    void drawNormalLine(double x1, double y1, double x2, double y2);
    void drawBusLine(double x1, double y1, double x2, double y2);
    
    // 数据面板组件创建方法
    QWidget* createDataGroup(const QString& title, const QMap<QString, QString>& data);
    QWidget* createPercentageBar(const QString& label, double percentage, const QColor& color);
    QWidget* createStatCard(const QString& title, const QString& value, const QString& unit = "");
    
    // 数据显示方法
    void calculateDerivedStats();
    void showCPUData(const ComponentStats& stats);
    void showL2CacheData(const ComponentStats& stats);
    void showL3CacheData(const ComponentStats& stats);
    void showMemoryData(const ComponentStats& stats);
    void showBusNodeData(const QString& nodeId);
    void showPortTransmissionStats(int nodeNum);  // 新增：显示port传输统计
    void showNodeEdgeStats(int nodeNum);  // 新增：显示节点边统计
    void showPortTransmissionOverview();  // 新增：显示port传输概览
    void resetDataPanel();  // 新增：重置数据面板
};

#endif // CHIPWINDOW_H 