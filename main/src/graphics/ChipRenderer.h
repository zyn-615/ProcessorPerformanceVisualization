#ifndef CHIPRENDERER_H
#define CHIPRENDERER_H

#include "../components/ClickableItems.h"
#include <QGraphicsScene>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QObject>

class ChipRenderer : public QObject
{
    Q_OBJECT

public:
    ChipRenderer(QGraphicsScene* scene, QObject* parent = nullptr);
    
    // 绘制芯片架构的主方法
    void drawChipArchitecture();

private:
    QGraphicsScene* scene;
    
    // 绘制各类组件的方法
    ClickableRectItem* drawCPU(double x, double y, const QString& name, const QString& componentId);
    ClickableRectItem* drawL2Cache(double x, double y, const QString& name, const QString& componentId);
    ClickableRectItem* drawL3Cache(double x, double y, const QString& name, const QString& componentId);
    ClickableRectItem* drawDDR(double x, double y, const QString& componentId);
    ClickableEllipseItem* drawBusNode(double x, double y, int nodeId);
    
    // 绘制连接线的方法
    void drawThickLine(double x1, double y1, double x2, double y2);
    void drawNormalLine(double x1, double y1, double x2, double y2);
    void drawBusLine(double x1, double y1, double x2, double y2);
};

#endif // CHIPRENDERER_H 