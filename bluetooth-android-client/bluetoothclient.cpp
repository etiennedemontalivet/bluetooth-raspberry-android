#include "bluetoothclient.h"
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

BluetoothClient::BluetoothClient(QObject *parent) : QObject(parent)
{
    // Init shared data
    m_data = "Not Connected";

    // Init discovery agent
    m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(QBluetoothAddress());

    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));
    connect(m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    qDebug() << "Starting discovering services...";
    setData("Discovering...");

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

    setData("Start client !");
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
        setData(QString::fromUtf8(line.constData(), line.length()));
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
    setData("Service Discoverd ! " + serviceInfo.device().address().toString());
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
    setData("Discovering over");
}

QString BluetoothClient::getData() {
    return m_data;
}

void BluetoothClient::setData(QString newData) {
    if(newData != m_data) {
        m_data = newData;
        emit dataChanged();
    }
}
