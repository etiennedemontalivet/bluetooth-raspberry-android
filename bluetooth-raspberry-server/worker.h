#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>

#include "bluetoothserver.h"

class Worker : public QThread
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);

signals:
    void sendSensorValue(QString s);
public slots:
    void run();

private:
    int sensorValue;

    BluetoothServer *server;
};

#endif // WORKER_H
