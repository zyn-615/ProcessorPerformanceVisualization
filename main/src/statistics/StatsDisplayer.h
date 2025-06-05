#ifndef STATSDISPLAYER_H
#define STATSDISPLAYER_H

#include "ComponentStats.h"
#include "../ui/DataVisualizer.h"
#include <QVBoxLayout>
#include <QMap>
#include <QString>
#include <QRegularExpression>

class StatsDisplayer : public QObject
{
    Q_OBJECT

public:
    StatsDisplayer(DataVisualizer* visualizer, QObject* parent = nullptr);
    
    // 设置数据源
    void setStatsData(const QMap<QString, ComponentStats>& statsData, 
                      const QMap<QString, QString>& portTransmissions);
    
    // 显示组件数据
    void showComponentData(const QString& componentId, QVBoxLayout* layout);
    
    // 显示特定类型组件的数据
    void showCPUData(const ComponentStats& stats, QVBoxLayout* layout);
    void showL2CacheData(const ComponentStats& stats, QVBoxLayout* layout);
    void showL3CacheData(const ComponentStats& stats, QVBoxLayout* layout);
    void showMemoryData(const ComponentStats& stats, QVBoxLayout* layout);
    void showBusNodeData(const QString& nodeId, QVBoxLayout* layout);
    
    // 显示详细信息
    void showPortTransmissionStats(int nodeNum, QVBoxLayout* layout);
    void showNodeEdgeStats(int nodeNum, QVBoxLayout* layout);
    
    // 端口传输概览
    void showPortTransmissionOverview(QVBoxLayout* layout);
    
    // 重置面板到初始状态
    void resetDataPanel(QVBoxLayout* layout);

private:
    DataVisualizer* visualizer;
    QMap<QString, ComponentStats> statsData;
    QMap<QString, QString> portTransmissions;
};

#endif // STATSDISPLAYER_H 