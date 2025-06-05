#ifndef DATAVISUALIZER_H
#define DATAVISUALIZER_H

#include <QWidget>
#include <QColor>
#include <QMap>
#include <QString>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
#include <QPushButton>

class DataVisualizer : public QObject
{
    Q_OBJECT

public:
    DataVisualizer(QObject* parent = nullptr);
    
    // 清除数据面板
    void clearDataPanel(QVBoxLayout* layout);
    
    // 创建数据组件
    QWidget* createDataGroup(const QString& title, const QMap<QString, QString>& data);
    QWidget* createPercentageBar(const QString& label, double percentage, const QColor& color);
    QWidget* createStatCard(const QString& title, const QString& value, const QString& unit = "");
    
    // 添加标题
    QLabel* createTitleLabel(const QString& title);
    
    // 创建按钮
    QPushButton* createStyledButton(const QString& text, const QString& colorCode, 
                                   bool isPrimary = true);
};

#endif // DATAVISUALIZER_H 