#ifndef COMPONENTSTATS_H
#define COMPONENTSTATS_H

#include <QString>
#include <QMap>

// 统计数据结构
struct ComponentStats {
    QString name;
    QMap<QString, QString> data;       // 原始统计数据
    QMap<QString, double> percentages; // 计算的百分比指标
};

#endif // COMPONENTSTATS_H 