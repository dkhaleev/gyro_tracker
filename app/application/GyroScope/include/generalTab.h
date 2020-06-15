#ifndef GENERALTAB_H
#define GENERALTAB_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFileInfo;
class QTabWidget;
QT_END_NAMESPACE

class GeneralTab : public QWidget {
  Q_OBJECT

public:
  explicit GeneralTab(const QString &text, QWidget *parent = 0);

};

#endif // GENERALTAB_H
