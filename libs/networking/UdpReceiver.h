#pragma once
#include <QObject>
#include <QUdpSocket>

class UdpReceiver : public QObject
{
    Q_OBJECT

    public:
    UdpReceiver(quint16 port, QObject *parent = nullptr);

    signals:
    void DatagramReceived(const QByteArray &payload);

    private:
    QUdpSocket socket_;

    void HandleReadyRead();
};