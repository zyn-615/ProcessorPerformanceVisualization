#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QRandomGenerator> // For simulating data
#include <QGroupBox>      // For grouping UI elements

// We can create a simple MainWindow class for better organization
class MainWindow : public QWidget {
    Q_OBJECT // Required for classes定義信號和槽

public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Processor Performance Monitor (Demo)");
        setMinimumSize(400, 250);

        // --- Main Layout ---
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // --- CPU Info Group ---
        QGroupBox *infoGroup = new QGroupBox("Static CPU Info");
        QVBoxLayout *infoLayout = new QVBoxLayout;

        QLabel *cpuModelLabel = new QLabel("CPU Model: Intel Core i9-XXXXK (Demo)");
        QLabel *cpuCoresLabel = new QLabel("Cores/Threads: 8/16 (Demo)");
        infoLayout->addWidget(cpuModelLabel);
        infoLayout->addWidget(cpuCoresLabel);
        infoGroup->setLayout(infoLayout);
        mainLayout->addWidget(infoGroup);

        // --- CPU Usage Group ---
        QGroupBox *usageGroup = new QGroupBox("Live CPU Usage");
        QVBoxLayout *usageLayout = new QVBoxLayout;

        QHBoxLayout *cpuUsageLayout = new QHBoxLayout;
        QLabel *cpuUsageLabelText = new QLabel("Overall CPU Usage:");
        cpuUsageValueLabel = new QLabel("0%"); // Member variable to update it
        cpuProgressBar = new QProgressBar;    // Member variable
        cpuProgressBar->setRange(0, 100);
        cpuProgressBar->setTextVisible(false); // Hide the default percentage text on bar

        cpuUsageLayout->addWidget(cpuUsageLabelText);
        cpuUsageLayout->addWidget(cpuProgressBar);
        cpuUsageLayout->addWidget(cpuUsageValueLabel);
        usageLayout->addLayout(cpuUsageLayout);
        usageGroup->setLayout(usageLayout);
        mainLayout->addWidget(usageGroup);

        // --- Timer for dynamic updates ---
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::updateCpuUsage); // Connect signal to slot
        timer->start(500); // Update every 500 ms

        // Initialize usage
        updateCpuUsage();
    }

private slots:
    void updateCpuUsage() {
        // Simulate CPU usage data
        int currentUsage = QRandomGenerator::global()->bounded(0, 101); // 0 to 100
        cpuProgressBar->setValue(currentUsage);
        cpuUsageValueLabel->setText(QString::number(currentUsage) + "%");
    }

private:
    QProgressBar *cpuProgressBar;
    QLabel *cpuUsageValueLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

// We need to include the MOC output for our MainWindow class
#include "main.moc"

