#include "chipwindow.h"
#include "ui_chipwindow.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QDir>
#include <QPushButton>
#include <algorithm>
#include <QCloseEvent>

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

// ChipWindow主类实现
ChipWindow::ChipWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChipWindow)
    , scene(nullptr)
    , view(nullptr)
    , splitter(nullptr)
    , dataPanel(nullptr)
{
    ui->setupUi(this);
    setupChipView();
    setupDataPanel();
    loadStatistics();
    drawChipArchitecture();
}

ChipWindow::~ChipWindow()
{
    delete ui;
    if (scene) {
        delete scene;
    }
}

void ChipWindow::closeEvent(QCloseEvent *event)
{
    // 发射窗口关闭信号
    emit windowClosed();
    // 接受关闭事件
    event->accept();
    // 调用父类的closeEvent
    QMainWindow::closeEvent(event);
}

void ChipWindow::setupChipView()
{
    setWindowTitle("Processor Architecture Visualization");
    resize(1400, 800);
    
    // 创建主分割器
    splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);
    
    // 创建图形场景和视图
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    splitter->addWidget(view);
    
    // 设置场景尺寸
    scene->setSceneRect(0, 0, 1000, 700);
    
    // 设置视图属性
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setMinimumWidth(800);
}

void ChipWindow::setupDataPanel()
{
    // 创建数据展示面板
    dataPanel = new QWidget();
    dataPanel->setMinimumWidth(350);
    dataPanel->setMaximumWidth(500);
    dataPanel->setStyleSheet("QWidget { background-color: #f8f9fa; }");
    
    // 创建滚动区域
    scrollArea = new QScrollArea();
    scrollArea->setWidget(dataPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    splitter->addWidget(scrollArea);
    
    // 设置布局
    dataPanelLayout = new QVBoxLayout(dataPanel);
    dataPanelLayout->setSpacing(10);
    dataPanelLayout->setContentsMargins(15, 15, 15, 15);
    
    // 添加Port传输概览按钮
    QPushButton* portOverviewBtn = new QPushButton("Port Transmission Overview");
    portOverviewBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #007bff;"
        "  color: white;"
        "  border: none;"
        "  padding: 10px;"
        "  border-radius: 5px;"
        "  font-weight: bold;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #0056b3;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #004085;"
        "}"
    );
    connect(portOverviewBtn, &QPushButton::clicked, this, &ChipWindow::showPortTransmissionOverview);
    dataPanelLayout->addWidget(portOverviewBtn);
    
    // 初始化提示
    QLabel* hintLabel = new QLabel("Click on any component to view its statistics\n\nOr click the button above to see port transmission overview");
    hintLabel->setStyleSheet("color: #6c757d; font-size: 14px; font-style: italic; text-align: center;");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);
    dataPanelLayout->addWidget(hintLabel);
    dataPanelLayout->addStretch();
    
    // 设置分割器比例
    splitter->setSizes({800, 400});
}

void ChipWindow::loadStatistics()
{
    // 尝试多个可能的路径
    QStringList possiblePaths = {
        "files/statistic.txt",                    // 相对于工作目录
        "../files/statistic.txt",                 // 如果在build目录运行
        "../../files/statistic.txt",              // 多层嵌套情况
        QDir::currentPath() + "/files/statistic.txt",  // 当前目录
        QDir::currentPath() + "/../files/statistic.txt" // 上级目录
    };
    
    QFile file;
    QString usedPath;
    
    // 尝试每个路径
    for (const QString& path : possiblePaths) {
        file.setFileName(path);
        if (file.exists()) {
            usedPath = path;
            qDebug() << "Found statistic.txt at:" << path;
            break;
        }
    }
    
    if (usedPath.isEmpty()) {
        qDebug() << "Cannot find statistic.txt in any of the expected locations:";
        for (const QString& path : possiblePaths) {
            qDebug() << "  Tried:" << path;
        }
        qDebug() << "Current working directory:" << QDir::currentPath();
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open statistic.txt at" << usedPath;
        return;
    }
    
    qDebug() << "Successfully opened statistic.txt from:" << usedPath;
    
    QTextStream in(&file);
    QString currentComponent;
    ComponentStats currentStats;
    int lineCount = 0;
    int componentCount = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineCount++;
        if (line.isEmpty() || line.startsWith("//")) continue;
        
        // 检测组件开始
        if (line.contains("Latency:")) {
            if (!currentComponent.isEmpty()) {
                statsData[currentComponent] = currentStats;
                qDebug() << "Parsed component:" << currentComponent << "with" << currentStats.data.size() << "data entries";
                componentCount++;
            }
            currentComponent = line.split(" ").first();
            currentStats = ComponentStats();
            currentStats.name = currentComponent;
            continue;
        }
        
        // 解析数据行
        if (line.contains(":")) {
            QStringList parts = line.split(":");
            if (parts.size() >= 2) {
                QString key = parts[0].trimmed();
                QString value = parts[1].trimmed();
                
                // 移除注释
                if (value.contains("//")) {
                    value = value.split("//").first().trimmed();
                }
                
                // 如果当前有组件上下文，添加到组件数据
                if (!currentComponent.isEmpty()) {
                    currentStats.data[key] = value;
                }
                
                // 同时将所有键值对添加到portTransmissions用于Bus数据
                portTransmissions[key] = value;
            }
        }
    }
    
    // 保存最后一个组件
    if (!currentComponent.isEmpty()) {
        statsData[currentComponent] = currentStats;
        qDebug() << "Parsed component:" << currentComponent << "with" << currentStats.data.size() << "data entries";
        componentCount++;
    }
    
    qDebug() << "Total lines processed:" << lineCount;
    qDebug() << "Total components parsed:" << componentCount;
    qDebug() << "Port transmissions found:" << portTransmissions.size();
    qDebug() << "Components found:" << statsData.keys();
    
    // 检查特定组件
    if (statsData.contains("MemoryNode0")) {
        qDebug() << "MemoryNode0 data:" << statsData["MemoryNode0"].data;
    } else {
        qDebug() << "MemoryNode0 not found in parsed data";
    }
    
    // 计算衍生数据
    calculateDerivedStats();
}

void ChipWindow::calculateDerivedStats()
{
    // 为CPU计算衍生统计
    for (int i = 0; i < 4; i++) {
        QString cpuId = QString("CPU%1").arg(i);
        if (statsData.contains(cpuId)) {
            ComponentStats& stats = statsData[cpuId];
            
            // 计算命中率
            if (stats.data.contains("ld_cache_hit_count") && stats.data.contains("ld_cache_miss_count")) {
                int hits = stats.data["ld_cache_hit_count"].toInt();
                int misses = stats.data["ld_cache_miss_count"].toInt();
                double hitRate = hits > 0 ? (double)hits / (hits + misses) * 100 : 0;
                stats.percentages["ld_hit_rate"] = hitRate;
            }
            
            if (stats.data.contains("st_cache_hit_count") && stats.data.contains("st_cache_miss_count")) {
                int hits = stats.data["st_cache_hit_count"].toInt();
                int misses = stats.data["st_cache_miss_count"].toInt();
                double hitRate = hits > 0 ? (double)hits / (hits + misses) * 100 : 0;
                stats.percentages["st_hit_rate"] = hitRate;
            }
            
            // 计算IPC
            if (stats.data.contains("finished_inst_count") && stats.data.contains("total_tick_processed")) {
                double ipc = (double)stats.data["finished_inst_count"].toInt() / stats.data["total_tick_processed"].toInt();
                stats.percentages["ipc"] = ipc;
            }
        }
    }
    
    // 为缓存计算命中率
    for (int i = 0; i < 4; i++) {
        QString l2Id = QString("L2Cache%1").arg(i);
        QString l3Id = QString("L3Cache%1").arg(i);
        
        if (statsData.contains(l2Id)) {
            ComponentStats& stats = statsData[l2Id];
            
            // L2命中率
            if (stats.data.contains("l2_hit_count") && stats.data.contains("l2_miss_count")) {
                int hits = stats.data["l2_hit_count"].toInt();
                int misses = stats.data["l2_miss_count"].toInt();
                double hitRate = hits > 0 ? (double)hits / (hits + misses) * 100 : 0;
                stats.percentages["l2_hit_rate"] = hitRate;
            }
            
            // L1D命中率
            if (stats.data.contains("l1d_hit_count") && stats.data.contains("l1d_miss_count")) {
                int hits = stats.data["l1d_hit_count"].toInt();
                int misses = stats.data["l1d_miss_count"].toInt();
                double hitRate = hits > 0 ? (double)hits / (hits + misses) * 100 : 0;
                stats.percentages["l1d_hit_rate"] = hitRate;
            }
            
            // L1I命中率
            if (stats.data.contains("l1i_hit_count") && stats.data.contains("l1i_miss_count")) {
                int hits = stats.data["l1i_hit_count"].toInt();
                int misses = stats.data["l1i_miss_count"].toInt();
                double hitRate = hits > 0 ? (double)hits / (hits + misses) * 100 : 0;
                stats.percentages["l1i_hit_rate"] = hitRate;
            }
        }
        
        if (statsData.contains(l3Id)) {
            ComponentStats& stats = statsData[l3Id];
            
            // L3命中率
            if (stats.data.contains("llc_hit_count") && stats.data.contains("llc_miss_count")) {
                int hits = stats.data["llc_hit_count"].toInt();
                int misses = stats.data["llc_miss_count"].toInt();
                double hitRate = hits > 0 ? (double)hits / (hits + misses) * 100 : 0;
                stats.percentages["llc_hit_rate"] = hitRate;
            }
        }
    }
}

void ChipWindow::onComponentClicked(const QString& componentId)
{
    showComponentData(componentId);
}

void ChipWindow::showComponentData(const QString& componentId)
{
    clearDataPanel();
    
    if (!statsData.contains(componentId)) {
        QLabel* noDataLabel = new QLabel(QString("No statistics available for %1").arg(componentId));
        noDataLabel->setStyleSheet("color: #dc3545; font-size: 14px;");
        dataPanelLayout->addWidget(noDataLabel);
        return;
    }
    
    const ComponentStats& stats = statsData[componentId];
    
    // 标题
    QLabel* titleLabel = new QLabel(QString("%1 Statistics").arg(stats.name));
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    dataPanelLayout->addWidget(titleLabel);
    
    if (componentId.startsWith("CPU")) {
        showCPUData(stats);
    } else if (componentId.startsWith("L2Cache")) {
        showL2CacheData(stats);
    } else if (componentId.startsWith("L3Cache")) {
        showL3CacheData(stats);
    } else if (componentId == "MemoryNode0") {
        showMemoryData(stats);
    } else if (componentId.startsWith("node_")) {
        showBusNodeData(componentId);
    }
    
    dataPanelLayout->addStretch();
}

void ChipWindow::showCPUData(const ComponentStats& stats)
{
    // 基础性能指标
    QMap<QString, QString> basicData;
    basicData["Total Cycles"] = stats.data.value("total_tick_processed", "0");
    basicData["Instructions Completed"] = stats.data.value("finished_inst_count", "0");
    
    if (stats.percentages.contains("ipc")) {
        basicData["IPC"] = QString::number(stats.percentages["ipc"], 'f', 3);
    }
    
    dataPanelLayout->addWidget(createDataGroup("Performance Metrics", basicData));
    
    // LOAD指令统计
    QMap<QString, QString> loadData;
    loadData["Total LOAD Instructions"] = stats.data.value("ld_inst_cnt", "0");
    loadData["Cache Hits"] = stats.data.value("ld_cache_hit_count", "0");
    loadData["Cache Misses"] = stats.data.value("ld_cache_miss_count", "0");
    loadData["Total Memory Cycles"] = stats.data.value("ld_mem_tick_sum", "0");
    
    if (stats.data.contains("ld_inst_cnt") && stats.data["ld_inst_cnt"].toInt() > 0) {
        double avgCycles = stats.data["ld_mem_tick_sum"].toDouble() / stats.data["ld_inst_cnt"].toInt();
        loadData["Avg Cycles per LOAD"] = QString::number(avgCycles, 'f', 2);
    }
    
    dataPanelLayout->addWidget(createDataGroup("LOAD Instructions", loadData));
    
    // LOAD命中率进度条
    if (stats.percentages.contains("ld_hit_rate")) {
        dataPanelLayout->addWidget(createPercentageBar("LOAD Hit Rate", 
                                  stats.percentages["ld_hit_rate"], QColor(40, 167, 69)));
    }
    
    // STORE指令统计
    QMap<QString, QString> storeData;
    storeData["Total STORE Instructions"] = stats.data.value("st_inst_cnt", "0");
    storeData["Cache Hits"] = stats.data.value("st_cache_hit_count", "0");
    storeData["Cache Misses"] = stats.data.value("st_cache_miss_count", "0");
    storeData["Total Memory Cycles"] = stats.data.value("st_mem_tick_sum", "0");
    
    if (stats.data.contains("st_inst_cnt") && stats.data["st_inst_cnt"].toInt() > 0) {
        double avgCycles = stats.data["st_mem_tick_sum"].toDouble() / stats.data["st_inst_cnt"].toInt();
        storeData["Avg Cycles per STORE"] = QString::number(avgCycles, 'f', 2);
    }
    
    dataPanelLayout->addWidget(createDataGroup("STORE Instructions", storeData));
    
    // STORE命中率进度条
    if (stats.percentages.contains("st_hit_rate")) {
        dataPanelLayout->addWidget(createPercentageBar("STORE Hit Rate", 
                                  stats.percentages["st_hit_rate"], QColor(220, 53, 69)));
    }
}

void ChipWindow::showL2CacheData(const ComponentStats& stats)
{
    // 首先显示Port信息
    QMap<QString, QString> portInfo;
    QString componentName = stats.name;
    int cacheIndex = componentName.right(1).toInt(); // 从L2Cache0获取0
    
    // 根据setup.txt的映射关系确定port号
    QMap<int, int> l2CacheToPort = {{0, 0}, {1, 2}, {2, 5}, {3, 7}};
    if (l2CacheToPort.contains(cacheIndex)) {
        int portId = l2CacheToPort[cacheIndex];
        portInfo["Port ID"] = QString::number(portId);
        
        // 计算该port的总发送和接收包数
        int totalSent = 0;
        int totalReceived = 0;
        
        for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
            QString key = it.key();
            int value = it.value().toInt();
            
            // 解析 transmit_package_number_from_X_to_Y
            QRegularExpression re("transmit_package_number_from_(\\d+)_to_(\\d+)");
            QRegularExpressionMatch match = re.match(key);
            if (match.hasMatch()) {
                int fromPort = match.captured(1).toInt();
                int toPort = match.captured(2).toInt();
                
                if (fromPort == portId) totalSent += value;
                if (toPort == portId) totalReceived += value;
            }
        }
        
        portInfo["Total Packages Sent"] = QString::number(totalSent);
        portInfo["Total Packages Received"] = QString::number(totalReceived);
    }
    
    dataPanelLayout->addWidget(createDataGroup("Port Information", portInfo));
    
    // L1I缓存统计
    QMap<QString, QString> l1iData;
    l1iData["L1I Hits"] = stats.data.value("l1i_hit_count", "0");
    l1iData["L1I Misses"] = stats.data.value("l1i_miss_count", "0");
    
    dataPanelLayout->addWidget(createDataGroup("L1 Instruction Cache", l1iData));
    
    if (stats.percentages.contains("l1i_hit_rate")) {
        dataPanelLayout->addWidget(createPercentageBar("L1I Hit Rate", 
                                  stats.percentages["l1i_hit_rate"], QColor(23, 162, 184)));
    }
    
    // L1D缓存统计
    QMap<QString, QString> l1dData;
    l1dData["L1D Hits"] = stats.data.value("l1d_hit_count", "0");
    l1dData["L1D Misses"] = stats.data.value("l1d_miss_count", "0");
    
    dataPanelLayout->addWidget(createDataGroup("L1 Data Cache", l1dData));
    
    if (stats.percentages.contains("l1d_hit_rate")) {
        dataPanelLayout->addWidget(createPercentageBar("L1D Hit Rate", 
                                  stats.percentages["l1d_hit_rate"], QColor(40, 167, 69)));
    }
    
    // L2缓存统计
    QMap<QString, QString> l2Data;
    l2Data["L2 Hits"] = stats.data.value("l2_hit_count", "0");
    l2Data["L2 Misses"] = stats.data.value("l2_miss_count", "0");
    
    dataPanelLayout->addWidget(createDataGroup("L2 Cache", l2Data));
    
    if (stats.percentages.contains("l2_hit_rate")) {
        dataPanelLayout->addWidget(createPercentageBar("L2 Hit Rate", 
                                  stats.percentages["l2_hit_rate"], QColor(255, 193, 7)));
    }
}

void ChipWindow::showL3CacheData(const ComponentStats& stats)
{
    // 首先显示Port信息
    QMap<QString, QString> portInfo;
    QString componentName = stats.name;
    int cacheIndex = componentName.right(1).toInt(); // 从L3Cache0获取0
    
    // 根据setup.txt的映射关系确定port号
    QMap<int, int> l3CacheToPort = {{0, 1}, {1, 3}, {2, 6}, {3, 8}};
    if (l3CacheToPort.contains(cacheIndex)) {
        int portId = l3CacheToPort[cacheIndex];
        portInfo["Port ID"] = QString::number(portId);
        
        // 计算该port的总发送和接收包数
        int totalSent = 0;
        int totalReceived = 0;
        
        for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
            QString key = it.key();
            int value = it.value().toInt();
            
            // 解析 transmit_package_number_from_X_to_Y
            QRegularExpression re("transmit_package_number_from_(\\d+)_to_(\\d+)");
            QRegularExpressionMatch match = re.match(key);
            if (match.hasMatch()) {
                int fromPort = match.captured(1).toInt();
                int toPort = match.captured(2).toInt();
                
                if (fromPort == portId) totalSent += value;
                if (toPort == portId) totalReceived += value;
            }
        }
        
        portInfo["Total Packages Sent"] = QString::number(totalSent);
        portInfo["Total Packages Received"] = QString::number(totalReceived);
    }
    
    dataPanelLayout->addWidget(createDataGroup("Port Information", portInfo));
    
    QMap<QString, QString> l3Data;
    l3Data["LLC Hits"] = stats.data.value("llc_hit_count", "0");
    l3Data["LLC Misses"] = stats.data.value("llc_miss_count", "0");
    
    int hits = stats.data.value("llc_hit_count", "0").toInt();
    int misses = stats.data.value("llc_miss_count", "0").toInt();
    l3Data["Total Accesses"] = QString::number(hits + misses);
    
    dataPanelLayout->addWidget(createDataGroup("Last Level Cache", l3Data));
    
    if (stats.percentages.contains("llc_hit_rate")) {
        dataPanelLayout->addWidget(createPercentageBar("LLC Hit Rate", 
                                  stats.percentages["llc_hit_rate"], QColor(108, 117, 125)));
    }
}

void ChipWindow::showMemoryData(const ComponentStats& stats)
{
    // 首先显示Port信息
    QMap<QString, QString> portInfo;
    // MemoryNode0连接到port 4和port 9
    portInfo["Primary Port"] = "4";
    portInfo["Secondary Port"] = "9";
    
    // 计算port 4和port 9的传输包统计
    int port4Sent = 0, port4Received = 0;
    int port9Sent = 0, port9Received = 0;
    
    for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
        QString key = it.key();
        int value = it.value().toInt();
        
        QRegularExpression re("transmit_package_number_from_(\\d+)_to_(\\d+)");
        QRegularExpressionMatch match = re.match(key);
        if (match.hasMatch()) {
            int fromPort = match.captured(1).toInt();
            int toPort = match.captured(2).toInt();
            
            if (fromPort == 4) port4Sent += value;
            if (toPort == 4) port4Received += value;
            if (fromPort == 9) port9Sent += value;
            if (toPort == 9) port9Received += value;
        }
    }
    
    portInfo["Port 4 Sent"] = QString::number(port4Sent);
    portInfo["Port 4 Received"] = QString::number(port4Received);
    portInfo["Port 9 Sent"] = QString::number(port9Sent);
    portInfo["Port 9 Received"] = QString::number(port9Received);
    
    dataPanelLayout->addWidget(createDataGroup("Port Information", portInfo));
    
    QMap<QString, QString> memData;
    memData["Messages Processed"] = stats.data.value("message_precossed", "0");
    
    if (stats.data.contains("busy_rate")) {
        double busyRate = stats.data["busy_rate"].toDouble() * 100;
        memData["Busy Rate"] = QString::number(busyRate, 'f', 3) + "%";
    }
    
    dataPanelLayout->addWidget(createDataGroup("Memory Controller", memData));
    
    if (stats.data.contains("busy_rate")) {
        double busyRate = stats.data["busy_rate"].toDouble() * 100;
        dataPanelLayout->addWidget(createPercentageBar("Memory Utilization", 
                                  busyRate, QColor(220, 53, 69)));
    }
}

void ChipWindow::showBusNodeData(const QString& nodeId)
{
    int nodeNum = nodeId.split("_").last().toInt();
    
    // 显示节点基本信息
    QMap<QString, QString> nodeData;
    
    // 从Bus统计中查找节点数据
    QString transmitKey = QString("node_%1_transmit_package_number").arg(nodeNum);
    QString busyKey = QString("node_%1_busy_rate").arg(nodeNum);
    
    // 从portTransmissions中查找（因为Bus数据是直接解析到portTransmissions的）
    bool foundData = false;
    for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
        if (it.key() == transmitKey) {
            nodeData["Packages Transmitted"] = it.value();
            foundData = true;
        } else if (it.key() == busyKey) {
            double busyRate = it.value().toDouble() * 100;
            nodeData["Busy Rate"] = QString::number(busyRate, 'f', 3) + "%";
            foundData = true;
        }
    }
    
    // 如果没有在portTransmissions中找到，从statsData["Bus"]中查找
    if (!foundData && statsData.contains("Bus")) {
        const ComponentStats& busStats = statsData["Bus"];
        if (busStats.data.contains(transmitKey)) {
            nodeData["Packages Transmitted"] = busStats.data[transmitKey];
            foundData = true;
        }
        if (busStats.data.contains(busyKey)) {
            double busyRate = busStats.data[busyKey].toDouble() * 100;
            nodeData["Busy Rate"] = QString::number(busyRate, 'f', 3) + "%";
            foundData = true;
        }
    }
    
    if (!foundData) {
        // 从所有已解析的数据中查找节点信息
        for (auto comp = statsData.begin(); comp != statsData.end(); ++comp) {
            if (comp.value().data.contains(transmitKey)) {
                nodeData["Packages Transmitted"] = comp.value().data[transmitKey];
                foundData = true;
            }
            if (comp.value().data.contains(busyKey)) {
                double busyRate = comp.value().data[busyKey].toDouble() * 100;
                nodeData["Busy Rate"] = QString::number(busyRate, 'f', 3) + "%";
                foundData = true;
            }
        }
    }
    
    if (foundData || !nodeData.isEmpty()) {
        dataPanelLayout->addWidget(createDataGroup(QString("Bus Node %1").arg(nodeNum), nodeData));
    }
    
    // 显示连接到该节点的ports
    QMap<QString, QString> portData;
    QStringList portsOnNode;
    
    // 根据setup.txt的映射关系
    QMap<int, QStringList> nodeToPortsMap = {
        {0, {"0 (L2Cache0)", "1 (L3Cache0)"}},
        {1, {"2 (L2Cache1)", "3 (L3Cache1)"}},
        {2, {"4 (MemoryNode0)"}},
        {3, {"5 (L2Cache2)", "6 (L3Cache2)"}},
        {4, {"7 (L2Cache3)", "8 (L3Cache3)"}},
        {5, {"9 (MemoryNode0)"}}
    };
    
    if (nodeToPortsMap.contains(nodeNum)) {
        portsOnNode = nodeToPortsMap[nodeNum];
        for (int i = 0; i < portsOnNode.size(); ++i) {
            portData[QString("Port %1").arg(i + 1)] = portsOnNode[i];
        }
    }
    
    if (!portData.isEmpty()) {
        dataPanelLayout->addWidget(createDataGroup("Connected Ports", portData));
    }
    
    // 显示通过该节点的port传输统计
    showPortTransmissionStats(nodeNum);
    
    // 显示节点间连接的边（edge）使用率
    showNodeEdgeStats(nodeNum);
    
    // 添加进度条显示
    if (nodeData.contains("Busy Rate")) {
        QString busyRateStr = nodeData["Busy Rate"];
        busyRateStr.remove("%");
        double busyRate = busyRateStr.toDouble();
        dataPanelLayout->addWidget(createPercentageBar("Node Utilization", 
                                  busyRate, QColor(255, 193, 7)));
    }
}

void ChipWindow::showNodeEdgeStats(int nodeNum)
{
    QMap<QString, QString> edgeData;
    
    // 查找与该节点相关的边（edge）使用率
    for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
        QString key = it.key();
        QString value = it.value();
        
        // 查找边使用率数据：edge_X_to_Y_busy_rate
        QRegularExpression edgeRe("edge_(\\d+)_to_(\\d+)_busy_rate");
        QRegularExpressionMatch match = edgeRe.match(key);
        if (match.hasMatch()) {
            int fromNode = match.captured(1).toInt();
            int toNode = match.captured(2).toInt();
            
            if (fromNode == nodeNum || toNode == nodeNum) {
                double busyRate = value.toDouble() * 100;
                QString edgeDesc = QString("Edge %1→%2").arg(fromNode).arg(toNode);
                edgeData[edgeDesc] = QString::number(busyRate, 'f', 3) + "%";
            }
        }
    }
    
    if (!edgeData.isEmpty()) {
        dataPanelLayout->addWidget(createDataGroup("Connected Edges", edgeData));
    }
}

void ChipWindow::showPortTransmissionStats(int nodeNum)
{
    // 获取该节点上的所有ports
    QList<int> portsOnNode;
    
    // 根据setup.txt映射关系
    switch (nodeNum) {
        case 0: portsOnNode = {0, 1}; break;
        case 1: portsOnNode = {2, 3}; break;
        case 2: portsOnNode = {4}; break;
        case 3: portsOnNode = {5, 6}; break;
        case 4: portsOnNode = {7, 8}; break;
        case 5: portsOnNode = {9}; break;
    }
    
    // 收集涉及这些ports的传输数据
    QMap<QString, QString> transmissionData;
    QMap<QString, QString> topTransmissions; // 存储最重要的传输
    
    for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
        QString key = it.key();
        QString value = it.value();
        
        QRegularExpression re("transmit_package_number_from_(\\d+)_to_(\\d+)");
        QRegularExpressionMatch match = re.match(key);
        if (match.hasMatch()) {
            int fromPort = match.captured(1).toInt();
            int toPort = match.captured(2).toInt();
            int packageCount = value.toInt();
            
            // 如果涉及该节点上的任何port
            if (portsOnNode.contains(fromPort) || portsOnNode.contains(toPort)) {
                QString description = QString("Port %1 → Port %2").arg(fromPort).arg(toPort);
                transmissionData[description] = value;
                
                // 只显示传输量较大的连接（>50个包）
                if (packageCount > 50) {
                    topTransmissions[description] = value;
                }
            }
        }
    }
    
    // 优先显示重要的传输
    if (!topTransmissions.isEmpty()) {
        dataPanelLayout->addWidget(createDataGroup("Major Port Transmissions", topTransmissions));
    } else if (!transmissionData.isEmpty()) {
        dataPanelLayout->addWidget(createDataGroup("Port Transmissions", transmissionData));
    }
    
    // 添加传输汇总统计
    if (portsOnNode.size() > 0) {
        QMap<QString, QString> summaryData;
        int totalIn = 0, totalOut = 0;
        
        for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
            QString key = it.key();
            int value = it.value().toInt();
            
            QRegularExpression re("transmit_package_number_from_(\\d+)_to_(\\d+)");
            QRegularExpressionMatch match = re.match(key);
            if (match.hasMatch()) {
                int fromPort = match.captured(1).toInt();
                int toPort = match.captured(2).toInt();
                
                if (portsOnNode.contains(fromPort)) totalOut += value;
                if (portsOnNode.contains(toPort)) totalIn += value;
            }
        }
        
        summaryData["Total Packages In"] = QString::number(totalIn);
        summaryData["Total Packages Out"] = QString::number(totalOut);
        summaryData["Net Flow"] = QString::number(totalIn - totalOut);
        
        dataPanelLayout->addWidget(createDataGroup("Traffic Summary", summaryData));
    }
}

void ChipWindow::clearDataPanel()
{
    QLayoutItem* item;
    while ((item = dataPanelLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

QWidget* ChipWindow::createDataGroup(const QString& title, const QMap<QString, QString>& data)
{
    QGroupBox* groupBox = new QGroupBox(title);
    groupBox->setStyleSheet(
        "QGroupBox { font-weight: bold; font-size: 14px; color: #495057; border: 2px solid #dee2e6; "
        "border-radius: 5px; margin-top: 1ex; padding-top: 10px; background-color: white; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 10px 0 10px; }"
    );
    
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(5);
    
    for (auto it = data.begin(); it != data.end(); ++it) {
        QHBoxLayout* rowLayout = new QHBoxLayout();
        
        QLabel* keyLabel = new QLabel(it.key() + ":");
        keyLabel->setStyleSheet("font-weight: normal; color: #6c757d;");
        keyLabel->setMinimumWidth(150);
        
        QLabel* valueLabel = new QLabel(it.value());
        valueLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
        valueLabel->setAlignment(Qt::AlignRight);
        
        rowLayout->addWidget(keyLabel);
        rowLayout->addWidget(valueLabel);
        layout->addLayout(rowLayout);
    }
    
    return groupBox;
}

QWidget* ChipWindow::createPercentageBar(const QString& label, double percentage, const QColor& color)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setSpacing(5);
    layout->setContentsMargins(10, 5, 10, 5);
    
    QLabel* labelWidget = new QLabel(QString("%1: %2%").arg(label).arg(QString::number(percentage, 'f', 1)));
    labelWidget->setStyleSheet("font-weight: bold; color: #495057;");
    
    QProgressBar* progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(static_cast<int>(percentage));
    progressBar->setStyleSheet(QString(
        "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center; }"
        "QProgressBar::chunk { background-color: %1; border-radius: 3px; }"
    ).arg(color.name()));
    
    layout->addWidget(labelWidget);
    layout->addWidget(progressBar);
    
    return widget;
}

QWidget* ChipWindow::createStatCard(const QString& title, const QString& value, const QString& unit)
{
    QWidget* card = new QWidget();
    card->setStyleSheet("background-color: white; border: 1px solid #dee2e6; border-radius: 5px; padding: 10px;");
    
    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setAlignment(Qt::AlignCenter);
    
    QLabel* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("color: #6c757d; font-size: 12px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QLabel* valueLabel = new QLabel(value + " " + unit);
    valueLabel->setStyleSheet("color: #2c3e50; font-size: 18px; font-weight: bold;");
    valueLabel->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    
    return card;
}

void ChipWindow::drawChipArchitecture()
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

ClickableRectItem* ChipWindow::drawCPU(double x, double y, const QString& name, const QString& componentId)
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

ClickableRectItem* ChipWindow::drawL2Cache(double x, double y, const QString& name, const QString& componentId)
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

ClickableRectItem* ChipWindow::drawL3Cache(double x, double y, const QString& name, const QString& componentId)
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

ClickableRectItem* ChipWindow::drawDDR(double x, double y, const QString& componentId)
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

ClickableEllipseItem* ChipWindow::drawBusNode(double x, double y, int nodeId)
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

void ChipWindow::drawThickLine(double x1, double y1, double x2, double y2)
{
    // CPU到缓存的专属通道 - 使用渐变效果的粗黑线
    QPen pen(QColor(40, 40, 40), 5);
    pen.setCapStyle(Qt::RoundCap);
    QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, pen);
    line->setZValue(0);
}

void ChipWindow::drawNormalLine(double x1, double y1, double x2, double y2)
{
    // 普通连接 - 使用深灰色中等粗细线条
    QPen pen(QColor(80, 80, 80), 2);
    pen.setCapStyle(Qt::RoundCap);
    QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, pen);
    line->setZValue(0);
}

void ChipWindow::drawBusLine(double x1, double y1, double x2, double y2)
{
    // 总线主干 - 使用蓝色粗线
    QPen pen(QColor(30, 100, 200), 4);
    pen.setCapStyle(Qt::RoundCap);
    QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, pen);
    line->setZValue(0);
}

void ChipWindow::showPortTransmissionOverview()
{
    clearDataPanel();
    
    // 标题
    QLabel* titleLabel = new QLabel("Port Transmission Overview");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    dataPanelLayout->addWidget(titleLabel);
    
    // 创建端口映射说明
    QMap<QString, QString> portMapping;
    portMapping["Port 0"] = "L2Cache0";
    portMapping["Port 1"] = "L3Cache0";
    portMapping["Port 2"] = "L2Cache1";
    portMapping["Port 3"] = "L3Cache1";
    portMapping["Port 4"] = "MemoryNode0 (Primary)";
    portMapping["Port 5"] = "L2Cache2";
    portMapping["Port 6"] = "L3Cache2";
    portMapping["Port 7"] = "L2Cache3";
    portMapping["Port 8"] = "L3Cache3";
    portMapping["Port 9"] = "MemoryNode0 (Secondary)";
    
    dataPanelLayout->addWidget(createDataGroup("Port Mapping", portMapping));
    
    // 统计所有传输数据
    QMap<int, int> portSentTotal;
    QMap<int, int> portReceivedTotal;
    QMap<QString, QString> topTransmissions;
    
    for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
        QString key = it.key();
        QString value = it.value();
        
        QRegularExpression re("transmit_package_number_from_(\\d+)_to_(\\d+)");
        QRegularExpressionMatch match = re.match(key);
        if (match.hasMatch()) {
            int fromPort = match.captured(1).toInt();
            int toPort = match.captured(2).toInt();
            int packageCount = value.toInt();
            
            portSentTotal[fromPort] += packageCount;
            portReceivedTotal[toPort] += packageCount;
            
            // 收集重要的传输（>200个包）
            if (packageCount > 200) {
                QString desc = QString("Port %1 → Port %2").arg(fromPort).arg(toPort);
                topTransmissions[desc] = QString::number(packageCount);
            }
        }
    }
    
    // 显示重要传输
    dataPanelLayout->addWidget(createDataGroup("Major Transmissions (>200 packages)", topTransmissions));
    
    // 显示各端口的发送/接收统计
    QMap<QString, QString> portStats;
    for (int i = 0; i <= 9; ++i) {
        int sent = portSentTotal.value(i, 0);
        int received = portReceivedTotal.value(i, 0);
        if (sent > 0 || received > 0) {
            portStats[QString("Port %1").arg(i)] = QString("Sent: %1, Received: %2").arg(sent).arg(received);
        }
    }
    
    dataPanelLayout->addWidget(createDataGroup("Port Traffic Summary", portStats));
    
    // 按传输量排序显示最活跃的端口
    QList<QPair<int, int>> activePortsSent;
    for (auto it = portSentTotal.begin(); it != portSentTotal.end(); ++it) {
        if (it.value() > 0) {
            activePortsSent.append(qMakePair(it.value(), it.key()));
        }
    }
    std::sort(activePortsSent.begin(), activePortsSent.end(), std::greater<QPair<int, int>>());
    
    QMap<QString, QString> activePorts;
    for (int i = 0; i < qMin(5, activePortsSent.size()); ++i) {
        int packages = activePortsSent[i].first;
        int portId = activePortsSent[i].second;
        activePorts[QString("Port %1").arg(portId)] = QString("%1 packages sent").arg(packages);
    }
    
    if (!activePorts.isEmpty()) {
        dataPanelLayout->addWidget(createDataGroup("Most Active Ports (Sending)", activePorts));
    }
    
    // 添加返回按钮
    QPushButton* backBtn = new QPushButton("← Back to Component View");
    backBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #28a745;"
        "  color: white;"
        "  border: none;"
        "  padding: 8px;"
        "  border-radius: 5px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #218838;"
        "}"
    );
    connect(backBtn, &QPushButton::clicked, this, &ChipWindow::resetDataPanel);
    dataPanelLayout->addWidget(backBtn);
    
    dataPanelLayout->addStretch();
}

void ChipWindow::resetDataPanel()
{
    clearDataPanel();
    
    // 重新添加Port传输概览按钮
    QPushButton* portOverviewBtn = new QPushButton("Port Transmission Overview");
    portOverviewBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #007bff;"
        "  color: white;"
        "  border: none;"
        "  padding: 10px;"
        "  border-radius: 5px;"
        "  font-weight: bold;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #0056b3;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #004085;"
        "}"
    );
    connect(portOverviewBtn, &QPushButton::clicked, this, &ChipWindow::showPortTransmissionOverview);
    dataPanelLayout->addWidget(portOverviewBtn);
    
    // 初始化提示
    QLabel* hintLabel = new QLabel("Click on any component to view its statistics\n\nOr click the button above to see port transmission overview");
    hintLabel->setStyleSheet("color: #6c757d; font-size: 14px; font-style: italic; text-align: center;");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);
    dataPanelLayout->addWidget(hintLabel);
    dataPanelLayout->addStretch();
} 