#include "console.h"
#include <QScrollBar>
#include <QtCore/QDebug>

Console::Console(QWidget *parent):QPlainTextEdit(parent){
  QTextStream("stdout")<<"Console! \r\n";
  document()->setMaximumBlockCount(100);
  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::black);
  p.setColor(QPalette::Text, Qt::green);
  setPalette(p);
}

void Console::putData(const QByteArray &data)
{
    insertPlainText(QString(data));

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}
