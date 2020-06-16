#ifndef GRAPHTAB_H
#define GRAPHTAB_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFileInfo;
class QTabWidget;
QT_END_NAMESPACE

class GraphTab : public QWidget {
  Q_OBJECT

public:
  explicit GraphTab(const QString &text, QWidget *parent = 0);

};

#endif // GRAPHTAB_H
