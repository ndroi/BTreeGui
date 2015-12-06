#include "qtbtree.h"
#include <QtWidgets/QApplication>
int main(int argc, char *argv[])
{
	
	QApplication a(argc, argv);
	QtBtree w;
	w.show();
	return a.exec();
}
