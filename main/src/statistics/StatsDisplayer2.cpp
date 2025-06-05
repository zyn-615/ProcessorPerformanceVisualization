#include "StatsDisplayer.h"
#include <QLabel>
#include <QPushButton>
#include <algorithm>

void StatsDisplayer::showL3CacheData(const ComponentStats& stats, QVBoxLayout* layout)
{
    // First show port information
    QMap<QString, QString> portInfo;
    QString componentName = stats.name;
    int cacheIndex = componentName.right(1).toInt(); // Get 0 from L3Cache0
    
    // Map to port numbers based on setup.txt
    QMap<int, int> l3CacheToPort = {{0, 1}, {1, 3}, {2, 6}, {3, 8}};
    if (l3CacheToPort.contains(cacheIndex)) {
        int portId = l3CacheToPort[cacheIndex];
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
    
    QMap<QString, QString> l3Data;
    l3Data["LLC Hits"] = stats.data.value("llc_hit_count", "0");
    l3Data["LLC Misses"] = stats.data.value("llc_miss_count", "0");
    
    int hits = stats.data.value("llc_hit_count", "0").toInt();
    int misses = stats.data.value("llc_miss_count", "0").toInt();
    l3Data["Total Accesses"] = QString::number(hits + misses);
    
    layout->addWidget(visualizer->createDataGroup("Last Level Cache", l3Data));
    
    if (stats.percentages.contains("llc_hit_rate")) {
        layout->addWidget(visualizer->createPercentageBar("LLC Hit Rate", 
                                 stats.percentages["llc_hit_rate"], QColor(108, 117, 125)));
    }
}

void StatsDisplayer::showMemoryData(const ComponentStats& stats, QVBoxLayout* layout)
{
    // First show port information
    QMap<QString, QString> portInfo;
    // MemoryNode0 connects to port 4 and port 9
    portInfo["Primary Port"] = "4";
    portInfo["Secondary Port"] = "9";
    
    // Calculate port 4 and port 9 transmission statistics
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
    
    layout->addWidget(visualizer->createDataGroup("Port Information", portInfo));
    
    QMap<QString, QString> memData;
    memData["Messages Processed"] = stats.data.value("message_precossed", "0");
    
    if (stats.data.contains("busy_rate")) {
        double busyRate = stats.data["busy_rate"].toDouble() * 100;
        memData["Busy Rate"] = QString::number(busyRate, 'f', 3) + "%";
    }
    
    layout->addWidget(visualizer->createDataGroup("Memory Controller", memData));
    
    if (stats.data.contains("busy_rate")) {
        double busyRate = stats.data["busy_rate"].toDouble() * 100;
        layout->addWidget(visualizer->createPercentageBar("Memory Utilization", 
                                 busyRate, QColor(220, 53, 69)));
    }
}

void StatsDisplayer::showBusNodeData(const QString& nodeId, QVBoxLayout* layout)
{
    int nodeNum = nodeId.split("_").last().toInt();
    
    // Show basic node information
    QMap<QString, QString> nodeData;
    
    // Find node data from Bus statistics
    QString transmitKey = QString("node_%1_transmit_package_number").arg(nodeNum);
    QString busyKey = QString("node_%1_busy_rate").arg(nodeNum);
    
    // Look in portTransmissions (Bus data is parsed directly to portTransmissions)
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
    
    // If not found in portTransmissions, check statsData["Bus"]
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
        // Check all parsed data for node information
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
        layout->addWidget(visualizer->createDataGroup(QString("Bus Node %1").arg(nodeNum), nodeData));
    }
    
    // Show ports connected to this node
    QMap<QString, QString> portData;
    QStringList portsOnNode;
    
    // Based on setup.txt mapping
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
        layout->addWidget(visualizer->createDataGroup("Connected Ports", portData));
    }
    
    // Show port transmission statistics through this node
    showPortTransmissionStats(nodeNum, layout);
    
    // Show edge usage between nodes
    showNodeEdgeStats(nodeNum, layout);
    
    // Add progress bar display
    if (nodeData.contains("Busy Rate")) {
        QString busyRateStr = nodeData["Busy Rate"];
        busyRateStr.remove("%");
        double busyRate = busyRateStr.toDouble();
        layout->addWidget(visualizer->createPercentageBar("Node Utilization", 
                                 busyRate, QColor(255, 193, 7)));
    }
}

void StatsDisplayer::showNodeEdgeStats(int nodeNum, QVBoxLayout* layout)
{
    QMap<QString, QString> edgeData;
    
    // Find edge usage related to this node
    for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
        QString key = it.key();
        QString value = it.value();
        
        // Find edge usage data: edge_X_to_Y_busy_rate
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
        layout->addWidget(visualizer->createDataGroup("Connected Edges", edgeData));
    }
}

void StatsDisplayer::showPortTransmissionStats(int nodeNum, QVBoxLayout* layout)
{
    // Get all ports on this node
    QList<int> portsOnNode;
    
    // Based on setup.txt mapping
    switch (nodeNum) {
        case 0: portsOnNode = {0, 1}; break;
        case 1: portsOnNode = {2, 3}; break;
        case 2: portsOnNode = {4}; break;
        case 3: portsOnNode = {5, 6}; break;
        case 4: portsOnNode = {7, 8}; break;
        case 5: portsOnNode = {9}; break;
    }
    
    // Collect transmission data involving these ports
    QMap<QString, QString> transmissionData;
    QMap<QString, QString> topTransmissions; // Store most important transmissions
    
    for (auto it = portTransmissions.begin(); it != portTransmissions.end(); ++it) {
        QString key = it.key();
        QString value = it.value();
        
        QRegularExpression re("transmit_package_number_from_(\\d+)_to_(\\d+)");
        QRegularExpressionMatch match = re.match(key);
        if (match.hasMatch()) {
            int fromPort = match.captured(1).toInt();
            int toPort = match.captured(2).toInt();
            int packageCount = value.toInt();
            
            // If involves any port on this node
            if (portsOnNode.contains(fromPort) || portsOnNode.contains(toPort)) {
                QString description = QString("Port %1 → Port %2").arg(fromPort).arg(toPort);
                transmissionData[description] = value;
                
                // Only show high-volume connections (>50 packages)
                if (packageCount > 50) {
                    topTransmissions[description] = value;
                }
            }
        }
    }
    
    // Prioritize showing important transmissions
    if (!topTransmissions.isEmpty()) {
        layout->addWidget(visualizer->createDataGroup("Major Port Transmissions", topTransmissions));
    } else if (!transmissionData.isEmpty()) {
        layout->addWidget(visualizer->createDataGroup("Port Transmissions", transmissionData));
    }
    
    // Add transmission summary statistics
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
        
        layout->addWidget(visualizer->createDataGroup("Traffic Summary", summaryData));
    }
}

void StatsDisplayer::showPortTransmissionOverview(QVBoxLayout* layout)
{
    visualizer->clearDataPanel(layout);
    
    // Title
    QLabel* titleLabel = visualizer->createTitleLabel("Port Transmission Overview");
    layout->addWidget(titleLabel);
    
    // Create port mapping description
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
    
    layout->addWidget(visualizer->createDataGroup("Port Mapping", portMapping));
    
    // Collect all transmission data
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
            
            // Collect important transmissions (>200 packages)
            if (packageCount > 200) {
                QString desc = QString("Port %1 → Port %2").arg(fromPort).arg(toPort);
                topTransmissions[desc] = QString::number(packageCount);
            }
        }
    }
    
    // Show important transmissions
    layout->addWidget(visualizer->createDataGroup("Major Transmissions (>200 packages)", topTransmissions));
    
    // Show send/receive statistics for each port
    QMap<QString, QString> portStats;
    for (int i = 0; i <= 9; ++i) {
        int sent = portSentTotal.value(i, 0);
        int received = portReceivedTotal.value(i, 0);
        if (sent > 0 || received > 0) {
            portStats[QString("Port %1").arg(i)] = QString("Sent: %1, Received: %2").arg(sent).arg(received);
        }
    }
    
    layout->addWidget(visualizer->createDataGroup("Port Traffic Summary", portStats));
    
    // Sort and show most active ports by transmission volume
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
        layout->addWidget(visualizer->createDataGroup("Most Active Ports (Sending)", activePorts));
    }
    
    // Add back button
    QPushButton* backBtn = visualizer->createStyledButton("← Back to Component View", "#28a745", false);
    // Note: Connection must be made in ChipWindow class
    layout->addWidget(backBtn);
    
    layout->addStretch();
}

void StatsDisplayer::resetDataPanel(QVBoxLayout* layout)
{
    visualizer->clearDataPanel(layout);
    
    // Re-add Port Transmission Overview button
    QPushButton* portOverviewBtn = visualizer->createStyledButton("Port Transmission Overview", "#007bff");
    // Note: Connection must be made in ChipWindow class
    layout->addWidget(portOverviewBtn);
    
    // Initialize hint
    QLabel* hintLabel = new QLabel("Click on any component to view its statistics\n\nOr click the button above to see port transmission overview");
    hintLabel->setStyleSheet("color: #6c757d; font-size: 14px; font-style: italic; text-align: center;");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);
    layout->addWidget(hintLabel);
    layout->addStretch();
} 