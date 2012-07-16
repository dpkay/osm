#ifndef MAPVIEWERWIDGET_H
#define MAPVIEWERWIDGET_H

#include <QGLWidget>
#include "ui_mapviewerwidget.h"
#include <miro/miro.h>

class MapViewerWidget : public QGLWidget
{
    Q_OBJECT

public:
    MapViewerWidget(QWidget *parent = 0);
    ~MapViewerWidget();

private:
    Ui::MapViewerWidget ui;
};

#endif // MAPVIEWERWIDGET_H
