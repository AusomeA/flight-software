#pragma once
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>

class Discovery : public QObject
{
    Q_OBJECT

    public:
    Discovery(const QString &myName, const QString &peerName, QObject *parent = nullptr);

    signals: 
    void peerDiscovered(QHostAddress address);

    private:
    void Announce();
    void ReadPendingDatagrams();

    QString myName_;
    QString peerName_;
    QUdpSocket socket_;
    QTimer announceTimer_;
    QHostAddress peerAddress_;
};