#include "stdafx.h"
#include "osm_reader.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	osm_reader w;
	w.show();
	return a.exec();
}
