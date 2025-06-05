#include "StatsDisplayer.h"
#include <QLabel>
#include <QPushButton>

StatsDisplayer::StatsDisplayer(DataVisualizer* visualizer, QObject* parent)
    : QObject(parent), visualizer(visualizer)
{
}

void StatsDisplayer::setStatsData(const QMap<QString, ComponentStats>& statsData, 
                                 const QMap<QString, QString>& portTransmissions)
{
    this->statsData = statsData;
    this->portTransmissions = portTransmissions;
}

void StatsDisplayer::showComponentData(const QString& componentId, QVBoxLayout* layout)
{
    visualizer->clearDataPanel(layout);
    
    if (!statsData.contains(componentId)) {
        QLabel* noDataLabel = new QLabel(QString("No statistics available for %1").arg(componentId));
        noDataLabel->setStyleSheet("color: #dc3545; font-size: 14px;");
        layout->addWidget(noDataLabel);
        return;
    }
    
    const ComponentStats& stats = statsData[componentId];
    
    // Title
    QLabel* titleLabel = visualizer->createTitleLabel(QString("%1 Statistics").arg(stats.name));
    layout->addWidget(titleLabel);
    
    if (componentId.startsWith("CPU")) {
        showCPUData(stats, layout);
    } else if (componentId.startsWith("L2Cache")) {
        showL2CacheData(stats, layout);
    } else if (componentId.startsWith("L3Cache")) {
        showL3CacheData(stats, layout);
    } else if (componentId == "MemoryNode0") {
        showMemoryData(stats, layout);
    } else if (componentId.startsWith("node_")) {
        showBusNodeData(componentId, layout);
    }
    
    layout->addStretch();
}

void StatsDisplayer::showCPUData(const ComponentStats& stats, QVBoxLayout* layout)
{
    // Basic performance metrics
    QMap<QString, QString> basicData;
    basicData["Total Cycles"] = stats.data.value("total_tick_processed", "0");
    basicData["Instructions Completed"] = stats.data.value("finished_inst_count", "0");
    
    if (stats.percentages.contains("ipc")) {
        basicData["IPC"] = QString::number(stats.percentages["ipc"], 'f', 3);
    }
    
    layout->addWidget(visualizer->createDataGroup("Performance Metrics", basicData));
    
    // LOAD instruction statistics
    QMap<QString, QString> loadData;
    loadData["Total LOAD Instructions"] = stats.data.value("ld_inst_cnt", "0");
    loadData["Cache Hits"] = stats.data.value("ld_cache_hit_count", "0");
    loadData["Cache Misses"] = stats.data.value("ld_cache_miss_count", "0");
    loadData["Total Memory Cycles"] = stats.data.value("ld_mem_tick_sum", "0");
    
    if (stats.data.contains("ld_inst_cnt") && stats.data["ld_inst_cnt"].toInt() > 0) {
        double avgCycles = stats.data["ld_mem_tick_sum"].toDouble() / stats.data["ld_inst_cnt"].toInt();
        loadData["Avg Cycles per LOAD"] = QString::number(avgCycles, 'f', 2);
    }
    
    layout->addWidget(visualizer->createDataGroup("LOAD Instructions", loadData));
    
    // LOAD hit rate progress bar
    if (stats.percentages.contains("ld_hit_rate")) {
        layout->addWidget(visualizer->createPercentageBar("LOAD Hit Rate", 
                                 stats.percentages["ld_hit_rate"], QColor(40, 167, 69)));
    }
    
    // STORE instruction statistics
    QMap<QString, QString> storeData;
    storeData["Total STORE Instructions"] = stats.data.value("st_inst_cnt", "0");
    storeData["Cache Hits"] = stats.data.value("st_cache_hit_count", "0");
    storeData["Cache Misses"] = stats.data.value("st_cache_miss_count", "0");
    storeData["Total Memory Cycles"] = stats.data.value("st_mem_tick_sum", "0");
    
    if (stats.data.contains("st_inst_cnt") && stats.data["st_inst_cnt"].toInt() > 0) {
        double avgCycles = stats.data["st_mem_tick_sum"].toDouble() / stats.data["st_inst_cnt"].toInt();
        storeData["Avg Cycles per STORE"] = QString::number(avgCycles, 'f', 2);
    }
    
    layout->addWidget(visualizer->createDataGroup("STORE Instructions", storeData));
    
    // STORE hit rate progress bar
    if (stats.percentages.contains("st_hit_rate")) {
        layout->addWidget(visualizer->createPercentageBar("STORE Hit Rate", 
                                 stats.percentages["st_hit_rate"], QColor(220, 53, 69)));
    }
}

void StatsDisplayer::showL2CacheData(const ComponentStats& stats, QVBoxLayout* layout)
{
    // Port information
    QMap<QString, QString> portInfo;
    QString componentName = stats.name;
    int cacheIndex = componentName.right(1).toInt(); // Get 0 from L2Cache0
    
    // Map to port numbers based on setup.txt
    QMap<int, int> l2CacheToPort = {{0, 0}, {1, 2}, {2, 5}, {3, 7}};
    if (l2CacheToPort.contains(cacheIndex)) {
        int portId = l2CacheToPort[cacheIndex];
        portInfo["Port ID"] = QString::number(portId);
        
        // Calculate total sent and received packages
        int totalSent = 0;
        int totalReceived = 0;
        
        for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
            QString key = it.key();
            int value = it.value().toInt();
            
            // Parse transmit_package_number_from_X_to_Y
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
    
    layout->addWidget(visualizer->createDataGroup("Port Information", portInfo));
    
    // L1I cache statistics
    QMap<QString, QString> l1iData;
    l1iData["L1I Hits"] = stats.data.value("l1i_hit_count", "0");
    l1iData["L1I Misses"] = stats.data.value("l1i_miss_count", "0");
    
    layout->addWidget(visualizer->createDataGroup("L1 Instruction Cache", l1iData));
    
    if (stats.percentages.contains("l1i_hit_rate")) {
        layout->addWidget(visualizer->createPercentageBar("L1I Hit Rate", 
                                 stats.percentages["l1i_hit_rate"], QColor(23, 162, 184)));
    }
    
    // L1D cache statistics
    QMap<QString, QString> l1dData;
    l1dData["L1D Hits"] = stats.data.value("l1d_hit_count", "0");
    l1dData["L1D Misses"] = stats.data.value("l1d_miss_count", "0");
    
    layout->addWidget(visualizer->createDataGroup("L1 Data Cache", l1dData));
    
    if (stats.percentages.contains("l1d_hit_rate")) {
        layout->addWidget(visualizer->createPercentageBar("L1D Hit Rate", 
                                 stats.percentages["l1d_hit_rate"], QColor(40, 167, 69)));
    }
    
    // L2 cache statistics
    QMap<QString, QString> l2Data;
    l2Data["L2 Hits"] = stats.data.value("l2_hit_count", "0");
    l2Data["L2 Misses"] = stats.data.value("l2_miss_count", "0");
    
    layout->addWidget(visualizer->createDataGroup("L2 Cache", l2Data));
    
    if (stats.percentages.contains("l2_hit_rate")) {
        layout->addWidget(visualizer->createPercentageBar("L2 Hit Rate", 
                                 stats.percentages["l2_hit_rate"], QColor(255, 193, 7)));
    }
} 