#include "ChipWindow.h"
#include "../../forms/ui_chipwindow.h"
#include <QDebug>
#include <QPushButton>

ChipWindow::ChipWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChipWindow)
    , scene(nullptr)
    , view(nullptr)
    , splitter(nullptr)
    , dataPanel(nullptr)
    , statsLoader(nullptr)
    , chipRenderer(nullptr)
    , dataVisualizer(nullptr)
    , statsDisplayer(nullptr)
{
    ui->setupUi(this);
    
    // Create functional modules
    statsLoader = new StatsLoader();
    dataVisualizer = new DataVisualizer(this);
    statsDisplayer = new StatsDisplayer(dataVisualizer, this);
    
    // Setup UI
    setupChipView();
    setupDataPanel();
    
    // Load data
    if (statsLoader->loadStatistics()) {
        // Set statistics data to displayer
        statsDisplayer->setStatsData(
            statsLoader->getStatsData(),
            statsLoader->getPortTransmissions()
        );
    } else {
        qDebug() << "Failed to load statistics data";
    }
    
    // Create renderer and draw chip architecture
    chipRenderer = new ChipRenderer(scene, this);
    chipRenderer->drawChipArchitecture();
}

ChipWindow::~ChipWindow()
{
    delete ui;
    
    // Clean up memory
    if (scene) {
        delete scene;
    }
}

void ChipWindow::closeEvent(QCloseEvent *event)
{
    // Emit window closed signal
    emit windowClosed();
    // Accept close event
    event->accept();
    // Call parent's closeEvent
    QMainWindow::closeEvent(event);
}

void ChipWindow::setupChipView()
{
    setWindowTitle("Processor Architecture Visualization");
    resize(1400, 800);
    
    // Create main splitter
    splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);
    
    // Create graphics scene and view
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    splitter->addWidget(view);
    
    // Set scene size
    scene->setSceneRect(0, 0, 1000, 700);
    
    // Set view properties
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setMinimumWidth(800);
}

void ChipWindow::setupDataPanel()
{
    // Create data display panel
    dataPanel = new QWidget();
    dataPanel->setMinimumWidth(350);
    dataPanel->setMaximumWidth(500);
    dataPanel->setStyleSheet("QWidget { background-color: #f8f9fa; }");
    
    // Create scroll area
    scrollArea = new QScrollArea();
    scrollArea->setWidget(dataPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    splitter->addWidget(scrollArea);
    
    // Set layout
    dataPanelLayout = new QVBoxLayout(dataPanel);
    dataPanelLayout->setSpacing(10);
    dataPanelLayout->setContentsMargins(15, 15, 15, 15);
    
    // Add Port transmission overview button
    QPushButton* portOverviewBtn = dataVisualizer->createStyledButton("Port Transmission Overview", "#007bff");
    connect(portOverviewBtn, &QPushButton::clicked, this, &ChipWindow::showPortTransmissionOverview);
    dataPanelLayout->addWidget(portOverviewBtn);
    
    // Initialize hint
    QLabel* hintLabel = new QLabel("Click on any component to view its statistics\n\nOr click the button above to see port transmission overview");
    hintLabel->setStyleSheet("color: #6c757d; font-size: 14px; font-style: italic; text-align: center;");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);
    dataPanelLayout->addWidget(hintLabel);
    dataPanelLayout->addStretch();
    
    // Set splitter proportions
    splitter->setSizes({800, 400});
}

void ChipWindow::onComponentClicked(const QString& componentId)
{
    statsDisplayer->showComponentData(componentId, dataPanelLayout);
}

void ChipWindow::showPortTransmissionOverview()
{
    statsDisplayer->showPortTransmissionOverview(dataPanelLayout);
    
    // Need to reconnect "Back" button to this class's slot
    // Since StatsDisplayer creates the button without connecting it
    QPushButton* backBtn = dataPanel->findChild<QPushButton*>();
    if (backBtn) {
        disconnect(backBtn, nullptr, nullptr, nullptr);  // Disconnect any possible connections
        connect(backBtn, &QPushButton::clicked, this, &ChipWindow::resetDataPanel);
    }
}

void ChipWindow::resetDataPanel()
{
    statsDisplayer->resetDataPanel(dataPanelLayout);
    
    // Reconnect port overview button
    QPushButton* portOverviewBtn = dataPanel->findChild<QPushButton*>();
    if (portOverviewBtn) {
        disconnect(portOverviewBtn, nullptr, nullptr, nullptr);  // Disconnect any possible connections
        connect(portOverviewBtn, &QPushButton::clicked, this, &ChipWindow::showPortTransmissionOverview);
    }
} 