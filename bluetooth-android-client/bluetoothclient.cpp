#include "bluetoothclient.h"

BluetoothClient::BluetoothClient(QObject *parent) : QObject(parent)
{

    m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(QBluetoothAddress());

    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));
    connect(m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    qDebug() << "Starting discovering services...";
#ifdef Q_OS_ANDROID
    if (QtAndroid::androidSdkVersion() >= 23)
        m_discoveryAgent->setUuidFilter(QBluetoothUuid(reverseUuid));
    else
        m_discoveryAgent->setUuidFilter(QBluetoothUuid(serviceUuid));
#else
    m_discoveryAgent->setUuidFilter(QBluetoothUuid(serviceUuid));
#endif

    m_discoveryAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);

    //    // scan for services
    //    const QBluetoothAddress adapter = QBluetoothAddress();

    //#ifdef Q_OS_ANDROID
    //    if (QtAndroid::androidSdkVersion() >= 23)
    //        remoteSelector.startDiscovery(QBluetoothUuid(reverseUuid));
    //    else
    //        remoteSelector.startDiscovery(QBluetoothUuid(serviceUuid));
    //#else
    //    remoteSelector.startDiscovery(QBluetoothUuid(serviceUuid));
    //#endif
    //    if (remoteSelector.exec() == QDialog::Accepted) {
    //        QBluetoothServiceInfo service = remoteSelector.service();

    //        qDebug() << "Connecting to service 2" << service.serviceName()
    //                 << "on" << service.device().name();

    //        // Create client
    //        qDebug() << "Going to create client";
    //        ChatClient *client = new ChatClient(this);
    //        qDebug() << "Connecting...";

    //        connect(client, SIGNAL(messageReceived(QString,QString)),
    //                this, SLOT(showMessage(QString,QString)));
    //        connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    //        connect(client, SIGNAL(connected(QString)), this, SLOT(connected(QString)));
    //        connect(this, SIGNAL(sendMessage(QString)), client, SLOT(sendMessage(QString)));
    //        qDebug() << "Start client";
    //        client->startClient(service);

    //        clients.append(client);
    //    }

}

BluetoothClient::~BluetoothClient()
{

}

void BluetoothClient::startClient(const QBluetoothServiceInfo &remoteService)
{
    if (socket)
        return;

    // Connect to service
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    qDebug() << "Create socket";
    socket->connectToService(remoteService);
    qDebug() << "ConnectToService done";

    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

void BluetoothClient::connected()
{
    emit connected(socket->peerName());
}

void BluetoothClient::readSocket()
{
    if (!socket)
        return;

    while (socket->canReadLine()) {
        QByteArray line = socket->readLine();
        qDebug() << "Soket read : " << QString::fromUtf8(line.constData(), line.length());
        emit messageReceived(socket->peerName(),
                             QString::fromUtf8(line.constData(), line.length()));
    }
}

void BluetoothClient::sendMessage(const QString &message)
{
    QByteArray text = message.toUtf8() + '\n';
    socket->write(text);
}

void BluetoothClient::stopClient()
{
    delete socket;
    socket = 0;
}

void BluetoothClient::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo)
{
    qDebug() << "Discovered service on"
             << serviceInfo.device().name() << serviceInfo.device().address().toString();
    qDebug() << "\tService name:" << serviceInfo.serviceName();
    qDebug() << "\tDescription:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
    qDebug() << "\tProvider:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
    qDebug() << "\tL2CAP protocol service multiplexer:"
             << serviceInfo.protocolServiceMultiplexer();
    qDebug() << "\tRFCOMM server channel:" << serviceInfo.serverChannel();

    startClient(serviceInfo);

//    QMapIterator<QListWidgetItem *, QBluetoothServiceInfo> i(m_discoveredServices);
//    while (i.hasNext()){
//        i.next();
//        if (serviceInfo.device().address() == i.value().device().address()){
//            return;
//        }
//    }

//    QString remoteName;
//    if (serviceInfo.device().name().isEmpty())
//        remoteName = serviceInfo.device().address().toString();
//    else
//        remoteName = serviceInfo.device().name();

//    QListWidgetItem *item =
//        new QListWidgetItem(QString::fromLatin1("%1 %2").arg(remoteName,
//                                                             serviceInfo.serviceName()));

//    m_discoveredServices.insert(item, serviceInfo);
//    ui->remoteDevices->addItem(item);
}

void BluetoothClient::discoveryFinished() {
    qDebug() << "Discovery finished !";
}
