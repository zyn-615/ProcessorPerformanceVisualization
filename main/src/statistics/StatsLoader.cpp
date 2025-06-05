#include "StatsLoader.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDir>

StatsLoader::StatsLoader()
{
}

bool StatsLoader::loadStatistics()
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
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open statistic.txt at" << usedPath;
        return false;
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
                
                if (value.contains("//")) {
                    value = value.split("//").first().trimmed();
                }
                
                if (!currentComponent.isEmpty()) {
                    currentStats.data[key] = value;
                }
                
                portTransmissions[key] = value;
            }
        }
    }
    
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
    return true;
}

void StatsLoader::calculateDerivedStats()
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