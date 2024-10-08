#include "Terminal.h"
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include "ResourceManager.h"

Terminal::Terminal(QPlainTextEdit *terminalWidget, QPushButton *expandButton, QObject *parent)
    : QObject(parent), terminalWidget(terminalWidget), expandButton(expandButton) {}

void Terminal::setupTerminal()
{
    terminalWidget->setReadOnly(true);
    terminalWidget->setStyleSheet("background-color: #f6f2e8; color: black;");
    terminalWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    terminalWidget->document()->setDocumentMargin(0);
    terminalWidget->setContentsMargins(5, 0, 0, 0);
    terminalWidget->appendPlainText("\n\n\n\n\n");

    scrollToBottom();
}

void Terminal::appendLog(const QString &log)
{
    terminalWidget->appendPlainText(log);
    scrollToBottom();
}

void Terminal::scrollToBottom()
{
    QScrollBar *scrollBar = terminalWidget->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void Terminal::toggleExpansion(ResourceManager* resourceManager)
{
    if (terminalWidget->maximumHeight() == 38) {
        terminalWidget->setMaximumHeight(400);
        expandButton->setIcon(resourceManager->getIcon("chevron-down"));
        expandButton->setIconSize(QSize(16, 16));
    } else {
        terminalWidget->setMaximumHeight(38);
        expandButton->setIcon(resourceManager->getIcon("chevron-up"));
        expandButton->setIconSize(QSize(16, 16));
    }

    scrollToBottom();
}
