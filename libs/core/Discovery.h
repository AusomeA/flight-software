#pragma once
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QElapsedTimer>
#include <QMap>
#include <QList>

class Discovery : public QObject
{
    Q_OBJECT

    public:
    Discovery(const QString &myName, const QString &vehicleName, QObject *parent = nullptr);

    QList<QHostAddress> LivePeerAddresses(const QString &appName) const;

    signals: 
    void peerAppeared(const QString &appName, QHostAddress address);
    void peerDisappeared(const QString &appName, QHostAddress address);

    private:
    struct Peer
    {
        QString appName;
        QHostAddress address;
        QElapsedTimer lastHeard;
    };

    void Announce();
    void ReadPendingDatagrams();
    void RemoveStalePeers();

    QString myName_;
    QString vehicleName_;
    QString instanceID_;
    QUdpSocket socket_;
    QTimer announceTimer_;
    QMap <QString, Peer> peers_;
};