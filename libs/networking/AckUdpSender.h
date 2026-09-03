#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QJsonObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QMap>

class AckUdpSender : public QObject
{
    Q_OBJECT

public:
    AckUdpSender(QObject *parent = nullptr);

    qint64 SendAck(const QString &type, const QJsonObject &body, const QHostAddress &address, quint16 port);

signals:
    void Acknowledged(qint64 sequence, bool accepted);
    void GaveUp(qint64 sequence);

private:
    struct PendingMessage
    {
        QByteArray datagram;
        QHostAddress address;
        quint16 port = 0;
        int attempts = 0;
        QElapsedTimer sinceLastSend;
    };

    QUdpSocket socket_;
    QTimer retryTimer_;
    QMap<qint64, PendingMessage> pendingMessages_;
    qint64 nextSequence_ = 0;
    
    static constexpr int retryCheckIntervalMilliseconds = 100;
    void ReadAcks();
    void RetryPendingMessages();
};