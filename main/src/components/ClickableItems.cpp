#include "ClickableItems.h"
#include "../ui/ChipWindow.h"
#include <QGraphicsScene>

// 可点击矩形项实现
ClickableRectItem::ClickableRectItem(double x, double y, double width, double height, 
                                   const QString& componentId, QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, width, height, parent), componentId(componentId)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setCursor(Qt::PointingHandCursor);
}

void ClickableRectItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        ChipWindow* window = qobject_cast<ChipWindow*>(scene()->views().first()->window());
        if (window) {
            window->onComponentClicked(componentId);
        }
    }
    QGraphicsRectItem::mousePressEvent(event);
}

// 可点击椭圆项实现
ClickableEllipseItem::ClickableEllipseItem(double x, double y, double width, double height, 
                                         const QString& componentId, QGraphicsItem* parent)
    : QGraphicsEllipseItem(x, y, width, height, parent), componentId(componentId)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setCursor(Qt::PointingHandCursor);
}

void ClickableEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        ChipWindow* window = qobject_cast<ChipWindow*>(scene()->views().first()->window());
        if (window) {
            window->onComponentClicked(componentId);
        }
    }
    QGraphicsEllipseItem::mousePressEvent(event);
} 