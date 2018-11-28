#include "worker.h"
#include <QDebug>

Worker::Worker(QObject *parent) : QThread(parent)
{
    sensorValue = 0;

    // start Bluetooth
    server = new BluetoothServer();
    server->startServer();
    connect(this, SIGNAL(sendSensorValue(QString)), server, SLOT(sendMessage(const QString &)));

    // Launch thread
    start();
}

void Worker::run() {
    qDebug() << "In worker thread !";

    forever {
        qDebug() << "Worker thread sending: " << sensorValue;
        emit sendSensorValue(QString::number(sensorValue));

        QThread::msleep(5000);
        sensorValue++;
    }
}
