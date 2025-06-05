#ifndef STATSLOADER_H
#define STATSLOADER_H

#include "ComponentStats.h"
#include <QMap>
#include <QString>
#include <QDebug>

class StatsLoader
{
public:
    StatsLoader();
    
    // 加载统计数据
    bool loadStatistics();
    
    // 计算派生统计指标
    void calculateDerivedStats();
    
    // 获取所有组件的统计数据
    const QMap<QString, ComponentStats>& getStatsData() const { return statsData; }
    
    // 获取端口传输数据
    const QMap<QString, QString>& getPortTransmissions() const { return portTransmissions; }

private:
    QMap<QString, ComponentStats> statsData;        // 所有组件的统计数据
    QMap<QString, QString> portTransmissions;       // 端口传输数据
};

#endif // STATSLOADER_H 