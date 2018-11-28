#include "bluetoothserver.h"
#include <QDebug>
#include <unistd.h>

BluetoothServer::BluetoothServer(QObject *parent) : QObject(parent)
{
    QBluetoothLocalDevice localDevice;
    QString localDeviceName;

    // Check if Bluetooth is available on this device
    if (localDevice.isValid()) {
        qDebug() << "Bluetooth is available !";

        // Turn Bluetooth on
        localDevice.powerOn();

        // Read local device name
        localDeviceName = localDevice.name();
        qDebug() << "My name is :" << localDeviceName << "with address : " << localDevice.address().toString();

        // Make it visible to others
        localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);



        // Get connected devices
        QList<QBluetoothAddress> remotes;
        remotes = localDevice.connectedDevices();

    }
}

void BluetoothServer::startServer(const QBluetoothAddress &localAdapter) {
    qDebug() << "Starting bluetooth server...";
    rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(rfcommServer, SIGNAL(newConnection()), this, SLOT(clientConnected()));
    bool result = rfcommServer->listen(localAdapter);
    if (!result) {
        qWarning() << "Cannot bind bluetooth server to" << localAdapter.toString();
        return;
    }

    // Set server attributes
    qDebug() << "Setting bluetooth server attributes...";
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("Bt Rpi 3 Server"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription,
                             tr("Example bluetooth rpi3 server"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("qt-project.org"));

    // Create unique bt uuid
    serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));

    // Make it discoverable
    qDebug() << "Make bluetooth server discoverable...";
    QBluetoothServiceInfo::Sequence publicBrowse;
    publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
    serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                             publicBrowse);

    // Used protocol
    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
             << QVariant::fromValue(quint8(rfcommServer->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);

    serviceInfo.registerService(localAdapter);
}

BluetoothServer::~BluetoothServer() {
    stopServer();
}

void BluetoothServer::stopServer() {
    // Unregister service
    serviceInfo.unregisterService();

    // Close sockets
    qDeleteAll(clientSockets);

    // Close server
    delete rfcommServer;
    rfcommServer = nullptr;
}

void BluetoothServer::sendMessage(const QString &message) {
    qDebug() << "Bluetooth sending: " << message;
    QByteArray text = message.toUtf8() + '\n';

    foreach (QBluetoothSocket *socket, clientSockets)
        socket->write(text);
}

void BluetoothServer::clientConnected() {
    qDebug() << "New connection detected !";
    QBluetoothSocket *socket = rfcommServer->nextPendingConnection();
    if (!socket)
        return;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    clientSockets.append(socket);
    qDebug() << "Client [" << socket->peerName() << "] connected !";
    emit clientConnected(socket->peerName());
}

void BluetoothServer::clientDisconnected() {
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;

    emit clientDisconnected(socket->peerName());

    clientSockets.removeOne(socket);

    socket->deleteLater();
}

void BluetoothServer::readSocket() {
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;

    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        emit messageReceived(socket->peerName(),
                             QString::fromUtf8(line.constData(), line.length()));
    }
}
