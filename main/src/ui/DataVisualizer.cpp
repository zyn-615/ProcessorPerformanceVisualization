#include "DataVisualizer.h"
#include <QHBoxLayout>

DataVisualizer::DataVisualizer(QObject* parent)
    : QObject(parent)
{
}

void DataVisualizer::clearDataPanel(QVBoxLayout* layout)
{
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

QLabel* DataVisualizer::createTitleLabel(const QString& title)
{
    QLabel* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    return titleLabel;
}

QPushButton* DataVisualizer::createStyledButton(const QString& text, const QString& colorCode, bool isPrimary)
{
    QPushButton* button = new QPushButton(text);
    
    QString style;
    if (isPrimary) {
        style = QString(
            "QPushButton {"
            "  background-color: %1;"
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
        ).arg(colorCode);
    } else {
        style = QString(
            "QPushButton {"
            "  background-color: %1;"
            "  color: white;"
            "  border: none;"
            "  padding: 8px;"
            "  border-radius: 5px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "  background-color: #218838;"
            "}"
        ).arg(colorCode);
    }
    
    button->setStyleSheet(style);
    return button;
}

QWidget* DataVisualizer::createDataGroup(const QString& title, const QMap<QString, QString>& data)
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

QWidget* DataVisualizer::createPercentageBar(const QString& label, double percentage, const QColor& color)
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

QWidget* DataVisualizer::createStatCard(const QString& title, const QString& value, const QString& unit)
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