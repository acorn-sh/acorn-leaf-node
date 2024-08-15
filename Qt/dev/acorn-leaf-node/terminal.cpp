#include "terminal.h"

Terminal::Terminal(Ui::MainWindow *ui) : ui(ui) {}

void Terminal::setupTerminal()
{
    ui->terminal->setReadOnly(true);
    ui->terminal->setStyleSheet("background-color: #d5d3e0; color: black;");
}
