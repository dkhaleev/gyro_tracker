#ifndef CONSOLETAB_H
#define CONSOLETAB_H

#include <QDialog>
#include "console.h"

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFileInfo;
class QTabWidget;
QT_END_NAMESPACE

class ConsoleTab : public QWidget {
  Q_OBJECT

public:
  explicit ConsoleTab(Console &console, QWidget *parent = 0);

private:
  Console *console;

};

#endif // GENERALTAB_H
