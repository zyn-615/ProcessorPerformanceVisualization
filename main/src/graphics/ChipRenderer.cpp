#include "ChipRenderer.h"

ChipRenderer::ChipRenderer(QGraphicsScene* scene, QObject* parent)
    : QObject(parent), scene(scene)
{
}

void ChipRenderer::drawChipArchitecture()
{
    // 定义位置常量 - 进一步调整尺寸
    const double CPU_WIDTH = 100;
    const double CPU_HEIGHT = 70;
    const double L2_CACHE_WIDTH = 60;   // L2缓存进一步减小
    const double L2_CACHE_HEIGHT = 35;  // L2缓存更小更扁
    const double L3_CACHE_WIDTH = 80;   // L3缓存保持原大小
    const double L3_CACHE_HEIGHT = 50;
    const double DDR_WIDTH = 50;        // DDR稍微窄一点
    const double DDR_HEIGHT = 220;      // DDR更长，更符合真实内存条
    const double NODE_SIZE = 16;
    
    // 计算居中布局的基础位置
    double total_width = 4 * CPU_WIDTH + 3 * 80; // 4个CPU + 3个间距
    double start_x = (1000 - total_width) / 2;
    
    // CPU 位置 (顶部一排，均匀分布)
    double cpu_y = 80;
    double cpu_spacing = 120;
    double cpu0_x = start_x;
    double cpu1_x = start_x + cpu_spacing;
    double cpu2_x = start_x + 2 * cpu_spacing;
    double cpu3_x = start_x + 3 * cpu_spacing;
    
    // L2 缓存位置 (CPU正下方，居中对齐，使用新的L2尺寸)
    double l2_y = cpu_y + CPU_HEIGHT + 30;
    double l2_0_x = cpu0_x + (CPU_WIDTH - L2_CACHE_WIDTH) / 2;
    double l2_1_x = cpu1_x + (CPU_WIDTH - L2_CACHE_WIDTH) / 2;
    double l2_2_x = cpu2_x + (CPU_WIDTH - L2_CACHE_WIDTH) / 2;
    double l2_3_x = cpu3_x + (CPU_WIDTH - L2_CACHE_WIDTH) / 2;
    
    // 总线节点位置 (L2缓存下方，完美对齐)
    double bus_y = l2_y + L2_CACHE_HEIGHT + 50;
    double node0_x = cpu0_x + CPU_WIDTH / 2;
    double node1_x = cpu1_x + CPU_WIDTH / 2;
    double node2_x = cpu2_x + CPU_WIDTH / 2;
    double node3_x = cpu3_x + CPU_WIDTH / 2;
    double node4_x = node3_x + 120; // 向右延伸
    
    // L3 缓存位置 (总线节点正下方，使用L3尺寸)
    double l3_y = bus_y + NODE_SIZE + 50;
    double l3_0_x = node0_x - L3_CACHE_WIDTH / 2;
    double l3_1_x = node1_x - L3_CACHE_WIDTH / 2;
    double l3_2_x = node2_x - L3_CACHE_WIDTH / 2;
    double l3_3_x = node3_x - L3_CACHE_WIDTH / 2;
    
    // 节点5位置 (底部居中)
    double node5_x = (node0_x + node4_x) / 2;
    double node5_y = l3_y + L3_CACHE_HEIGHT + 80;
    
    // DDR 位置 (右侧，与总线对齐，使用新的DDR尺寸)
    double ddr_x = node4_x + 80;
    double ddr_y = bus_y - DDR_HEIGHT / 2 + NODE_SIZE / 2;
    
    // === 绘制组件 ===
    
    // 绘制 CPU (顶部一排) - 现在可点击
    drawCPU(cpu0_x, cpu_y, "CPU0", "CPU0");
    drawCPU(cpu1_x, cpu_y, "CPU1", "CPU1");
    drawCPU(cpu2_x, cpu_y, "CPU2", "CPU2");
    drawCPU(cpu3_x, cpu_y, "CPU3", "CPU3");
    
    // 绘制 L2 缓存 (CPU正下方) - 现在可点击
    drawL2Cache(l2_0_x, l2_y, "L2", "L2Cache0");
    drawL2Cache(l2_1_x, l2_y, "L2", "L2Cache1");
    drawL2Cache(l2_2_x, l2_y, "L2", "L2Cache2");
    drawL2Cache(l2_3_x, l2_y, "L2", "L2Cache3");
    
    // 绘制总线节点 - 现在可点击
    drawBusNode(node0_x, bus_y, 0);
    drawBusNode(node1_x, bus_y, 1);
    drawBusNode(node2_x, bus_y, 2);
    drawBusNode(node3_x, bus_y, 3);
    drawBusNode(node4_x, bus_y, 4);
    drawBusNode(node5_x, node5_y, 5);
    
    // 绘制 L3 缓存 (总线节点下方) - 现在可点击
    drawL3Cache(l3_0_x, l3_y, "L3", "L3Cache0");
    drawL3Cache(l3_1_x, l3_y, "L3", "L3Cache1");
    drawL3Cache(l3_2_x, l3_y, "L3", "L3Cache2");
    drawL3Cache(l3_3_x, l3_y, "L3", "L3Cache3");
    
    // 绘制 DDR (右侧) - 现在可点击
    drawDDR(ddr_x, ddr_y, "MemoryNode0");
    
    // === 绘制连接线 ===
    
    // CPU 到 L2 缓存的专属通道 (黑色粗线)
    drawThickLine(cpu0_x + CPU_WIDTH/2, cpu_y + CPU_HEIGHT, 
                  cpu0_x + CPU_WIDTH/2, l2_y);
    drawThickLine(cpu1_x + CPU_WIDTH/2, cpu_y + CPU_HEIGHT, 
                  cpu1_x + CPU_WIDTH/2, l2_y);
    drawThickLine(cpu2_x + CPU_WIDTH/2, cpu_y + CPU_HEIGHT, 
                  cpu2_x + CPU_WIDTH/2, l2_y);
    drawThickLine(cpu3_x + CPU_WIDTH/2, cpu_y + CPU_HEIGHT, 
                  cpu3_x + CPU_WIDTH/2, l2_y);
    
    // L2 缓存到总线节点 (灰色细线)
    drawNormalLine(node0_x, l2_y + L2_CACHE_HEIGHT, node0_x, bus_y);
    drawNormalLine(node1_x, l2_y + L2_CACHE_HEIGHT, node1_x, bus_y);
    drawNormalLine(node2_x, l2_y + L2_CACHE_HEIGHT, node2_x, bus_y);
    drawNormalLine(node3_x, l2_y + L2_CACHE_HEIGHT, node3_x, bus_y);
    
    // 总线节点到 L3 缓存 (灰色细线)
    drawNormalLine(node0_x, bus_y + NODE_SIZE, node0_x, l3_y);
    drawNormalLine(node1_x, bus_y + NODE_SIZE, node1_x, l3_y);
    drawNormalLine(node2_x, bus_y + NODE_SIZE, node2_x, l3_y);
    drawNormalLine(node3_x, bus_y + NODE_SIZE, node3_x, l3_y);
    
    // 总线主干 - 水平连接 (蓝色粗线)
    drawBusLine(node0_x, bus_y + NODE_SIZE/2, 
                node1_x, bus_y + NODE_SIZE/2);
    drawBusLine(node1_x, bus_y + NODE_SIZE/2, 
                node2_x, bus_y + NODE_SIZE/2);
    drawBusLine(node2_x, bus_y + NODE_SIZE/2, 
                node3_x, bus_y + NODE_SIZE/2);
    drawBusLine(node3_x, bus_y + NODE_SIZE/2, 
                node4_x, bus_y + NODE_SIZE/2);
    
    // 到节点5的连接 - 使用折线绕开组件
    // 从节点0到节点5 (绕过左侧)
    double bypass_left_x = node0_x - 60;  // 左侧绕行点
    double bypass_bottom_y = node5_y + NODE_SIZE/2;  // 底部连接点
    
    // 节点0 → 左侧绕行点 → 底部 → 节点5
    drawBusLine(node0_x, bus_y + NODE_SIZE/2, 
                bypass_left_x, bus_y + NODE_SIZE/2);  // 水平向左
    drawBusLine(bypass_left_x, bus_y + NODE_SIZE/2, 
                bypass_left_x, bypass_bottom_y);      // 垂直向下
    drawBusLine(bypass_left_x, bypass_bottom_y, 
                node5_x, bypass_bottom_y);            // 水平向右到节点5
    
    // 从节点4到节点5 (绕过右侧)
    double bypass_right_x = node4_x + 60; // 右侧绕行点
    
    // 节点4 → 右侧绕行点 → 底部 → 节点5
    drawBusLine(node4_x, bus_y + NODE_SIZE/2, 
                bypass_right_x, bus_y + NODE_SIZE/2); // 水平向右
    drawBusLine(bypass_right_x, bus_y + NODE_SIZE/2, 
                bypass_right_x, bypass_bottom_y);     // 垂直向下
    drawBusLine(bypass_right_x, bypass_bottom_y, 
                node5_x, bypass_bottom_y);            // 水平向左到节点5
    
    // 节点4到DDR的连接 (红色线条，也使用折线避免跨过组件)
    QPen ddrPen(QColor(200, 50, 50), 3);
    ddrPen.setCapStyle(Qt::RoundCap);
    
    // DDR连接使用L型路径
    double ddr_connection_y = bus_y + NODE_SIZE/2;
    scene->addLine(node4_x + NODE_SIZE/2, bus_y + NODE_SIZE/2, 
                   ddr_x - 10, ddr_connection_y, ddrPen);      // 水平到DDR左侧
    scene->addLine(ddr_x - 10, ddr_connection_y, 
                   ddr_x - 10, ddr_y + DDR_HEIGHT/2, ddrPen);  // 垂直到DDR中心
    scene->addLine(ddr_x - 10, ddr_y + DDR_HEIGHT/2, 
                   ddr_x, ddr_y + DDR_HEIGHT/2, ddrPen);       // 水平连接到DDR
}

ClickableRectItem* ChipRenderer::drawCPU(double x, double y, const QString& name, const QString& componentId)
{
    // 绘制阴影
    QGraphicsRectItem* shadow = scene->addRect(x + 3, y + 3, 100, 70, 
                                             QPen(Qt::NoPen), 
                                             QBrush(QColor(0, 0, 0, 50)));
    shadow->setZValue(-1);
    
    // 绘制主体 (可点击)
    ClickableRectItem* rect = new ClickableRectItem(x, y, 100, 70, componentId);
    rect->setPen(QPen(QColor(60, 60, 60), 2));
    rect->setBrush(QBrush(QColor(220, 220, 220)));
    rect->setZValue(1);
    scene->addItem(rect);
    
    // 绘制文字 (居中)
    QGraphicsTextItem* text = scene->addText(name, QFont("Arial", 11, QFont::Bold));
    QRectF textRect = text->boundingRect();
    text->setPos(x + (100 - textRect.width()) / 2, y + (70 - textRect.height()) / 2);
    text->setDefaultTextColor(QColor(40, 40, 40));
    text->setZValue(2);
    
    return rect;
}

ClickableRectItem* ChipRenderer::drawL2Cache(double x, double y, const QString& name, const QString& componentId)
{
    // 绘制阴影
    QGraphicsRectItem* shadow = scene->addRect(x + 2, y + 2, 60, 35, 
                                             QPen(Qt::NoPen), 
                                             QBrush(QColor(0, 0, 0, 40)));
    shadow->setZValue(-1);
    
    // 绘制主体 (可点击)
    ClickableRectItem* rect = new ClickableRectItem(x, y, 60, 35, componentId);
    rect->setPen(QPen(QColor(30, 100, 180), 2));
    rect->setBrush(QBrush(QColor(173, 216, 230)));
    rect->setZValue(1);
    scene->addItem(rect);
    
    // 绘制文字 (居中)
    QGraphicsTextItem* text = scene->addText(name, QFont("Arial", 8, QFont::Bold));
    QRectF textRect = text->boundingRect();
    text->setPos(x + (60 - textRect.width()) / 2, y + (35 - textRect.height()) / 2 - 5);
    text->setDefaultTextColor(QColor(20, 60, 120));
    text->setZValue(2);
    
    // 添加Port号标识
    QString cacheIndexStr = componentId.right(1);
    int cacheIndex = cacheIndexStr.toInt();
    QMap<int, int> l2CacheToPort = {{0, 0}, {1, 2}, {2, 5}, {3, 7}};
    
    if (l2CacheToPort.contains(cacheIndex)) {
        int portId = l2CacheToPort[cacheIndex];
        QGraphicsTextItem* portText = scene->addText(QString("P%1").arg(portId), QFont("Arial", 6, QFont::Bold));
        portText->setPos(x + 2, y + 20);
        portText->setDefaultTextColor(QColor(255, 100, 0));
        portText->setZValue(2);
    }
    
    return rect;
}

ClickableRectItem* ChipRenderer::drawL3Cache(double x, double y, const QString& name, const QString& componentId)
{
    // 绘制阴影
    QGraphicsRectItem* shadow = scene->addRect(x + 2, y + 2, 80, 50, 
                                             QPen(Qt::NoPen), 
                                             QBrush(QColor(0, 0, 0, 40)));
    shadow->setZValue(-1);
    
    // 绘制主体 (可点击)
    ClickableRectItem* rect = new ClickableRectItem(x, y, 80, 50, componentId);
    rect->setPen(QPen(QColor(34, 139, 34), 2));
    rect->setBrush(QBrush(QColor(144, 238, 144)));
    rect->setZValue(1);
    scene->addItem(rect);
    
    // 绘制文字 (居中)
    QGraphicsTextItem* text = scene->addText(name, QFont("Arial", 10, QFont::Bold));
    QRectF textRect = text->boundingRect();
    text->setPos(x + (80 - textRect.width()) / 2, y + (50 - textRect.height()) / 2 - 5);
    text->setDefaultTextColor(QColor(20, 80, 20));
    text->setZValue(2);
    
    // 添加Port号标识
    QString cacheIndexStr = componentId.right(1);
    int cacheIndex = cacheIndexStr.toInt();
    QMap<int, int> l3CacheToPort = {{0, 1}, {1, 3}, {2, 6}, {3, 8}};
    
    if (l3CacheToPort.contains(cacheIndex)) {
        int portId = l3CacheToPort[cacheIndex];
        QGraphicsTextItem* portText = scene->addText(QString("P%1").arg(portId), QFont("Arial", 6, QFont::Bold));
        portText->setPos(x + 2, y + 30);
        portText->setDefaultTextColor(QColor(255, 100, 0));
        portText->setZValue(2);
    }
    
    return rect;
}

ClickableRectItem* ChipRenderer::drawDDR(double x, double y, const QString& componentId)
{
    // 绘制阴影
    QGraphicsRectItem* shadow = scene->addRect(x + 3, y + 3, 50, 220, 
                                             QPen(Qt::NoPen), 
                                             QBrush(QColor(0, 0, 0, 50)));
    shadow->setZValue(-1);
    
    // 绘制主体 (可点击)
    ClickableRectItem* rect = new ClickableRectItem(x, y, 50, 220, componentId);
    rect->setPen(QPen(QColor(180, 50, 50), 2));
    rect->setBrush(QBrush(QColor(255, 192, 203)));
    rect->setZValue(1);
    scene->addItem(rect);
    
    // 绘制文字 (居中)
    QGraphicsTextItem* text = scene->addText("DDR", QFont("Arial", 11, QFont::Bold));
    QRectF textRect = text->boundingRect();
    text->setPos(x + (50 - textRect.width()) / 2, y + (220 - textRect.height()) / 2 - 10);
    text->setDefaultTextColor(QColor(120, 30, 30));
    text->setZValue(2);
    
    // 添加Port号标识 (MemoryNode0连接到port 4和port 9)
    QGraphicsTextItem* port4Text = scene->addText("P4", QFont("Arial", 6, QFont::Bold));
    port4Text->setPos(x + 2, y + 80);
    port4Text->setDefaultTextColor(QColor(255, 100, 0));
    port4Text->setZValue(2);
    
    QGraphicsTextItem* port9Text = scene->addText("P9", QFont("Arial", 6, QFont::Bold));
    port9Text->setPos(x + 2, y + 140);
    port9Text->setDefaultTextColor(QColor(255, 100, 0));
    port9Text->setZValue(2);
    
    return rect;
}

ClickableEllipseItem* ChipRenderer::drawBusNode(double x, double y, int nodeId)
{
    QString componentId = QString("node_%1").arg(nodeId);
    
    // 绘制外层发光效果
    QGraphicsEllipseItem* glow = scene->addEllipse(x - 12, y - 12, 24, 24, 
                                                 QPen(Qt::NoPen), 
                                                 QBrush(QColor(255, 215, 0, 30)));
    glow->setZValue(-0.5);
    
    // 绘制阴影
    QGraphicsEllipseItem* shadow = scene->addEllipse(x - 8 + 2, y - 8 + 2, 16, 16, 
                                                   QPen(Qt::NoPen), 
                                                   QBrush(QColor(0, 0, 0, 80)));
    shadow->setZValue(-1);
    
    // 绘制外环 (装饰性边框)
    QGraphicsEllipseItem* outer_ring = scene->addEllipse(x - 10, y - 10, 20, 20, 
                                                        QPen(QColor(180, 140, 0), 1), 
                                                        QBrush(Qt::NoBrush));
    outer_ring->setZValue(0.5);
    
    // 绘制主体 (可点击)
    ClickableEllipseItem* circle = new ClickableEllipseItem(x - 8, y - 8, 16, 16, componentId);
    circle->setPen(QPen(QColor(200, 150, 0), 2));
    circle->setBrush(QBrush(QColor(255, 215, 0)));
    circle->setZValue(1);
    scene->addItem(circle);
    
    // 绘制内部高亮点 (3D效果)
    QGraphicsEllipseItem* highlight = scene->addEllipse(x - 5, y - 6, 4, 4, 
                                                      QPen(Qt::NoPen), 
                                                      QBrush(QColor(255, 255, 255, 120)));
    highlight->setZValue(1.5);
    
    // 绘制文字 (加粗的节点编号)
    QGraphicsTextItem* text = scene->addText(QString::number(nodeId), QFont("Arial", 8, QFont::Bold));
    QRectF textRect = text->boundingRect();
    text->setPos(x - textRect.width() / 2, y - textRect.height() / 2);
    text->setDefaultTextColor(QColor(80, 50, 0));
    text->setZValue(2);
    
    return circle;
}

void ChipRenderer::drawThickLine(double x1, double y1, double x2, double y2)
{
    // CPU到缓存的专属通道 - 使用渐变效果的粗黑线
    QPen pen(QColor(40, 40, 40), 5);
    pen.setCapStyle(Qt::RoundCap);
    QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, pen);
    line->setZValue(0);
}

void ChipRenderer::drawNormalLine(double x1, double y1, double x2, double y2)
{
    // 普通连接 - 使用深灰色中等粗细线条
    QPen pen(QColor(80, 80, 80), 2);
    pen.setCapStyle(Qt::RoundCap);
    QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, pen);
    line->setZValue(0);
}

void ChipRenderer::drawBusLine(double x1, double y1, double x2, double y2)
{
    // 总线主干 - 使用蓝色粗线
    QPen pen(QColor(30, 100, 200), 4);
    pen.setCapStyle(Qt::RoundCap);
    QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, pen);
    line->setZValue(0);
} 