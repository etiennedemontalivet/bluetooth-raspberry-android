#ifndef BLUETOOTHSERVER_H
#define BLUETOOTHSERVER_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServer>

static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");

class BluetoothServer : public QObject
{
    Q_OBJECT

public:
    explicit BluetoothServer(QObject *parent = nullptr);
    ~BluetoothServer();

    void startServer(const QBluetoothAddress &localAdapter = QBluetoothAddress());
    void stopServer();

public slots:
    void sendMessage(const QString &message);

signals:
    void messageReceived(const QString &sender, const QString &message);
    void clientConnected(const QString &name);
    void clientDisconnected(const QString &name);

private slots:
    void clientConnected();
    void clientDisconnected();
    void readSocket();

private:
    QBluetoothServer *rfcommServer;
    QBluetoothServiceInfo serviceInfo;
    QList<QBluetoothSocket *> clientSockets;
};

#endif // BLUETOOTHSERVER_H
