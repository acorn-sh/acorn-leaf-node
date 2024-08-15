#include "containertable.h"
#include <QTime>

ContainerTable::ContainerTable(Ui::MainWindow *ui) : ui(ui)
{
    setupTable();
    populateDummyData();
}

void ContainerTable::setupTable()
{
    QStringList headers = {"Container Name", "Description", "Status", "Time", "Timer", "CPU", "GPU", "HDD", "NET", "Rate", "Income"};
    ui->containerTable->setColumnCount(headers.size());
    ui->containerTable->setHorizontalHeaderLabels(headers);
    ui->containerTable->setStyleSheet("background-color: #d5d3e0; color: black;");

    // Ensure the table is read-only but selectable
    ui->containerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->containerTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->containerTable->setSelectionBehavior(QAbstractItemView::SelectItems);

    // Allow horizontal scroll bar when necessary
    ui->containerTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Set initial widths to fit content for specific columns using ResizeToContents
    ui->containerTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Container Name
    ui->containerTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Description
    ui->containerTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Status

    // Enable resizing by dragging
    for (int i = 3; i < headers.size(); ++i) {
        ui->containerTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }

    // Set minimum width for columns
    ui->containerTable->horizontalHeader()->setMinimumSectionSize(70);

    // Adjust column widths to fit table width
    adjustColumnWidths();
}

void ContainerTable::adjustColumnWidths()
{
    // Resize specific columns to fit their contents
    ui->containerTable->resizeColumnToContents(0); // Container Name
    ui->containerTable->resizeColumnToContents(1); // Description
    ui->containerTable->resizeColumnToContents(2); // Status

    // Adjust remaining columns to fit within the table width
    int totalWidth = ui->containerTable->viewport()->width();
    int fixedColumnsWidth = ui->containerTable->columnWidth(0) + ui->containerTable->columnWidth(1) + ui->containerTable->columnWidth(2);
    int remainingWidth = totalWidth - fixedColumnsWidth;
    int variableColumnsCount = ui->containerTable->columnCount() - 3;

    int columnWidth = remainingWidth / variableColumnsCount;

    for (int i = 3; i < ui->containerTable->columnCount(); ++i) {
        ui->containerTable->setColumnWidth(i, std::max(columnWidth, 50));
    }
}

void ContainerTable::populateDummyData()
{
    ui->containerTable->insertRow(ui->containerTable->rowCount());
    int row = ui->containerTable->rowCount() - 1;

    QTableWidgetItem* itemName = new QTableWidgetItem("Docker1 - ETH Mining");
    itemName->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 0, itemName);

    QTableWidgetItem* itemDesc = new QTableWidgetItem("Ethereum POS mining");
    itemDesc->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 1, itemDesc);

    addControlButtons(row, "Install"); // Change "Install" to the active status as needed

    QTableWidgetItem* itemTime = new QTableWidgetItem("01:45:10");
    itemTime->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 3, itemTime);

    QTableWidgetItem* itemTimer = new QTableWidgetItem("1h");
    itemTimer->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 4, itemTimer);

    QTableWidgetItem* itemCPU = new QTableWidgetItem("Medium");
    itemCPU->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 5, itemCPU);

    QTableWidgetItem* itemGPU = new QTableWidgetItem("Low");
    itemGPU->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 6, itemGPU);

    QTableWidgetItem* itemHDD = new QTableWidgetItem("High");
    itemHDD->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 7, itemHDD);

    QTableWidgetItem* itemNET = new QTableWidgetItem("Low");
    itemNET->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 8, itemNET);

    QTableWidgetItem* itemProfit = new QTableWidgetItem("$5");
    itemProfit->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 9, itemProfit);

    QTableWidgetItem* itemIncome = new QTableWidgetItem("$50");
    itemIncome->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->containerTable->setItem(row, 10, itemIncome);

    // Adjust column widths to fit content
    adjustColumnWidths();
}

void ContainerTable::addControlButtons(int row, const QString &activeStatus)
{
    QWidget* pWidget = new QWidget();
    QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(2, 0, 2, 0);
    pLayout->setSpacing(5);

    QPushButton* btnPlay = new QPushButton();
    QPushButton* btnStop = new QPushButton();
    QPushButton* btnPause = new QPushButton();

    // Set icons
    btnPlay->setIcon(QIcon(":/images/play.png"));
    btnStop->setIcon(QIcon(":/images/stop.png"));
    btnPause->setIcon(QIcon(":/images/pause.png"));
\
    // Set button sizes
    btnPlay->setFixedSize(24, 24);
    btnStop->setFixedSize(24, 24);
    btnPause->setFixedSize(24, 24);
    // Set styles for active and inactive buttons
    QString activeStyle = "QPushButton { border-radius: 0px; }";
    QString inactiveStyle = "QPushButton { border-radius: 0px; color: #6d6781; }";

    btnPlay->setStyleSheet(activeStatus == "Play" ? activeStyle : inactiveStyle);
    btnStop->setStyleSheet(activeStatus == "Stop" ? activeStyle : inactiveStyle);
    btnPause->setStyleSheet(activeStatus == "Pause" ? activeStyle : inactiveStyle);

    // Set grayscale icons for inactive buttons
    if (activeStatus != "Play") btnPlay->setIcon(QIcon(":/images/play_inactive.png"));
    if (activeStatus != "Stop") btnStop->setIcon(QIcon(":/images/stop_inactive.png"));
    if (activeStatus != "Pause") btnPause->setIcon(QIcon(":/images/pause_inactive.png"));

    // Connect button signals to slots
    connect(btnPlay, &QPushButton::clicked, this, [this, row](){ handlePlay(row); });
    connect(btnStop, &QPushButton::clicked, this, [this, row](){ handleStop(row); });
    connect(btnPause, &QPushButton::clicked, this, [this, row](){ handlePause(row); });

    // Add buttons to layout
    pLayout->addWidget(btnPlay);
    pLayout->addWidget(btnStop);
    pLayout->addWidget(btnPause);
    pWidget->setLayout(pLayout);

    ui->containerTable->setCellWidget(row, 2, pWidget);
}

void ContainerTable::updateRuntime()
{
    QTableWidgetItem *item = ui->containerTable->item(0, 3); // Assuming you want to update the 'Time' of the first row
    if (item) {
        QTime time = QTime::fromString(item->text(), "hh:mm:ss");
        time = time.addSecs(1);
        item->setText(time.toString("hh:mm:ss"));
    }
}

void ContainerTable::handlePlay(int row) {
    // Implement what should happen when the play button is clicked
}

void ContainerTable::handleStop(int row) {
    // Implement what should happen when the stop button is clicked
}

void ContainerTable::handlePause(int row) {
    // Implement what should happen when the pause button is clicked
}

