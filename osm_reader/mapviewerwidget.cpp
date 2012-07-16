#include "StdAfx.h"
#include "mapviewerwidget.h"

MapViewerWidget::MapViewerWidget(QWidget *parent)
    : QGLWidget(parent)
{
  ui.setupUi(this);
}

MapViewerWidget::~MapViewerWidget()
{

}
