#ifndef GENERALTAB_H
#define GENERALTAB_H

#include <QDialog>
#include <QObject>
#include <qgridlayout.h>
#include "objectGL.h"


QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFileInfo;
class QTabWidget;
QT_END_NAMESPACE

class GeneralTab : public QWidget {
  Q_OBJECT

public:
  explicit GeneralTab(ObjectOpenGL *objectGL, QWidget *parent = 0);

private:
  ObjectOpenGL  *objectGL;
  QGridLayout   *gridLayout;
  QWidget       *gridLayoutWidget;
  QWidget       *centralWidget;
  ObjectOpenGL  *ObjectGL;


};

#endif // GENERALTAB_H

