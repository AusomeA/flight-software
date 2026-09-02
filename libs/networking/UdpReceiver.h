#pragma once
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class UdpReceiver : public QObject
{
    Q_OBJECT

    public:
    UdpReceiver(quint16 port, QObject *parent = nullptr);

    signals:
    void DatagramReceived(const QByteArray &payload, const QHostAddress &senderAddress, quint16 senderPort);

    private:
    QUdpSocket socket_;

    void HandleReadyRead();
};