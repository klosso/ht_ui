#include <QApplication>
#include "htmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HTMainWindow w;
    w.show();

    return a.exec();
}
