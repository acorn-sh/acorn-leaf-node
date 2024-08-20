#include "terminal.h"
#include <QScrollBar>

Terminal::Terminal(Ui::MainWindow *ui) : ui(ui) {}

void Terminal::setupTerminal()
{
    ui->terminal->setReadOnly(true);
    ui->terminal->setStyleSheet("background-color: #d5d3e0; color: black;");
}

void Terminal::scrollToBottom()
{
    // Ensure that at least two lines are visible
    QScrollBar *scrollBar = ui->terminal->verticalScrollBar();
    int maxValue = scrollBar->maximum();
    int pageSize = scrollBar->pageStep();

    // Calculate the value to make the last two lines visible
    int value = maxValue - pageSize + (scrollBar->singleStep() * 2);
    scrollBar->setValue(std::max(0, value));
}
