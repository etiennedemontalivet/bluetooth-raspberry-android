#include <QCoreApplication>
#include "worker.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Worker *worker = new Worker();

    return a.exec();
}
