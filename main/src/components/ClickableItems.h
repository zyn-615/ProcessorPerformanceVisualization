#ifndef CLICKABLEITEMS_H
#define CLICKABLEITEMS_H

#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

// 前向声明
class ChipWindow;

// 自定义可点击的矩形图形项
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

// 自定义可点击的椭圆图形项
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

#endif // CLICKABLEITEMS_H 