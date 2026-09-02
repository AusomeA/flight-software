#include "Discovery.h"
#include "SharedTypes.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkDatagram>
#include <QUuid>
#include <iostream>

using namespace std;

Discovery::Discovery(const QString &myName, const QString &vehicleName, QObject *parent)
    : QObject(parent),
      myName_(myName),
      vehicleName_(vehicleName),
      instanceID_(QUuid::createUuid().toString())
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
    announcement["vehicle"] = vehicleName_;
    announcement["instanceID"] = instanceID_;

    QByteArray datagram = QJsonDocument(announcement).toJson(QJsonDocument::Compact);
    socket_.writeDatagram(datagram, QHostAddress::Broadcast, SharedTypes::discoveryPort);

    RemoveStalePeers();
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
        
        const QString appName = announcement["app"].toString();
        const QString instanceID = announcement["instanceID"].toString();

        if (appName.isEmpty() || instanceID.isEmpty())
            continue;

        if (instanceID == instanceID_)
            continue;

        const bool isNewInstance = !peers_.contains(instanceID);
        const bool addressChanged = !isNewInstance && peers_[instanceID].address != datagram.senderAddress();

        Peer &peer = peers_[instanceID];
        peer.appName = appName;
        peer.address = datagram.senderAddress();
        peer.lastHeard.start();

        if (isNewInstance || addressChanged)
        {
            cout << "Discovered " << appName.toStdString() << " at " << peer.address.toString().toStdString() << endl;
            emit peerAppeared(appName, peer.address);
        }
    }
}

void Discovery::RemoveStalePeers()
{
    QMutableMapIterator<QString, Peer> iterator(peers_);
    while (iterator.hasNext())
    {
        iterator.next();
        if (iterator.value().lastHeard.elapsed() > SharedTypes::linkLostMilliseconds)
        {
            cout << "Lost " << iterator.value().appName.toStdString() << " at " << iterator.value().address.toString().toStdString() << endl;
            emit peerDisappeared(iterator.value().appName, iterator.value().address);
            iterator.remove();
        }
    }
}