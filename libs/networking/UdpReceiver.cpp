#include "UdpReceiver.h"
#include <QNetworkDatagram>
#include <QDebug>

UdpReceiver::UdpReceiver(quint16 port, QObject *parent)
: QObject(parent)
{
    if(!socket_.bind(QHostAddress::Any, port))
    {
        qFatal() << "Failed to bind UDP port" << port << ":" << socket_.errorString();
    }

    connect (&socket_, &QUdpSocket::readyRead, this, &UdpReceiver::HandleReadyRead);
}

void UdpReceiver::HandleReadyRead()
{
    while(socket_.hasPendingDatagrams())
    {
        emit DatagramReceived(socket_.receiveDatagram().data());
    }
}