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
}

BluetoothClient::~BluetoothClient()
{
    if(m_socket)
        delete m_socket;
    if(m_discoveryAgent)
        delete m_discoveryAgent;
}

void BluetoothClient::startClient(const QBluetoothServiceInfo &remoteService)
{
    if (m_socket)
        return;

    // Connect to service
    m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    qDebug() << "Create socket";
    m_socket->connectToService(remoteService);
    qDebug() << "ConnectToService done";

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

void BluetoothClient::connected()
{
    emit connected(m_socket->peerName());
}

void BluetoothClient::readSocket()
{
    if (!m_socket)
        return;

    while (m_socket->canReadLine()) {
        QByteArray line = m_socket->readLine();
        qDebug() << "Soket read : " << QString::fromUtf8(line.constData(), line.length());
        emit messageReceived(m_socket->peerName(),
                             QString::fromUtf8(line.constData(), line.length()));
    }
}

void BluetoothClient::sendMessage(const QString &message)
{
    QByteArray text = message.toUtf8() + '\n';
    m_socket->write(text);
}

void BluetoothClient::stopClient()
{
    delete m_socket;
    m_socket = nullptr;
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
}

void BluetoothClient::discoveryFinished() {
    qDebug() << "Discovery finished !";
}
