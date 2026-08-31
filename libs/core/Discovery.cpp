#include "Discovery.h"
#include "SharedTypes.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkDatagram>
#include <iostream>

using namespace std;

Discovery::Discovery(const QString &myName, const QString &peerName, QObject *parent)
    : QObject(parent),
      myName_(myName),
      peerName_(peerName)
{
    socket_.bind(QHostAddress::AnyIPv4, SharedTypes::discoveryPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(&socket_, &QUdpSocket::readyRead, this, &Discovery::ReadPendingDatagrams);
    connect(&announceTimer_, &QTimer::timeout, this, &Discovery::Announce);
    announceTimer_.start(SharedTypes::discoveryIntervalMilliseconds);
}

void Discovery::Announce()
{
    QJsonObject announcement;
    announcement["app"] = myName_;

    QByteArray datagram = QJsonDocument(announcement).toJson(QJsonDocument::Compact);
    socket_.writeDatagram(datagram, QHostAddress::Broadcast, SharedTypes::discoveryPort);
}

void Discovery::ReadPendingDatagrams()
{
    while (socket_.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket_.receiveDatagram();

        QJsonDocument document = QJsonDocument::fromJson(datagram.data());
        if (!document.isObject())
            continue;

        QJsonObject announcement = document.object();
        if(announcement["app"].toString() != peerName_)
            continue;

        if(datagram.senderAddress() != peerAddress_)
        {
            peerAddress_ = datagram.senderAddress();
            cout << "Discovered " << peerName_.toStdString() << " at " << peerAddress_.toString().toStdString() << endl;
            emit peerDiscovered(peerAddress_);
        }
    }
}