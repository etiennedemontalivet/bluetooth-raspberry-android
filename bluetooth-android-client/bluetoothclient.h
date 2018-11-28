#ifndef BLUETOOTHCLIENT_H
#define BLUETOOTHCLIENT_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include <QBluetoothServiceDiscoveryAgent>
#include <QMutex>

static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");
#ifdef Q_OS_ANDROID
static const QLatin1String reverseUuid("c8e96402-0102-cf9c-274b-701a950fe1e8");
#endif

class BluetoothClient : public QObject
{
    Q_OBJECT

public:
    explicit BluetoothClient(QObject *parent = nullptr);
    ~BluetoothClient();

    Q_PROPERTY(QString data READ getData NOTIFY dataChanged)

    void startClient(const QBluetoothServiceInfo &remoteService);
    void stopClient();

public slots:
    void sendMessage(const QString &message);
    void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);
    void discoveryFinished();
    QString getData();

signals:
    void messageReceived(const QString &sender, const QString &message);
    void connected(const QString &name);
    void disconnected();
    void dataChanged();

private slots:
    void readSocket();
    void connected();

private:
    QBluetoothSocket *m_socket;
    QBluetoothServiceDiscoveryAgent *m_discoveryAgent;
    QString m_data;
};


#endif // BLUETOOTHCLIENT_H
