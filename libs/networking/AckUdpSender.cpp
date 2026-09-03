#include "AckUdpSender.h"

#include "SharedTypes.h"
#include "EnvelopeJson.h"
#include <QNetworkDatagram>
#include <iostream>

using namespace std;

AckUdpSender::AckUdpSender(QObject *parent)
    : QObject(parent)
{
    socket_.bind(QHostAddress::AnyIPv4, 0); // 0 for any available port
    connect(&socket_, &QUdpSocket::readyRead, this, &AckUdpSender::ReadAcks);
    connect(&retryTimer_, &QTimer::timeout, this, &AckUdpSender::RetryPendingMessages);
}

qint64 AckUdpSender::SendAck(const QString &type, const QJsonObject &body, const QHostAddress &address, quint16 port)
{
    Envelope envelope;
    envelope.type = type;
    envelope.sequence = nextSequence_++;
    envelope.body = body;

    PendingMessage message;
    message.datagram = EnvelopeToJson(envelope);
    message.address = address;
    message.port = port;
    message.attempts = 1;
    message.sinceLastSend.start();

    socket_.writeDatagram(message.datagram, address, port);
    pendingMessages_[envelope.sequence] = message;

    if (!retryTimer_.isActive())
        retryTimer_.start(retryCheckIntervalMilliseconds);

    return envelope.sequence;
}

void AckUdpSender::ReadAcks()
{
    while(socket_.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket_.receiveDatagram();
        std::optional<Envelope> envelope = EnvelopeFromJson(datagram.data());
        if(!envelope || envelope->type != SharedTypes::ackMessageType)
            continue;

        if(!pendingMessages_.contains(envelope->sequence))
            continue;

        pendingMessages_.remove(envelope->sequence);
        emit Acknowledged(envelope->sequence, envelope->body["accepted"].toBool());
    }

    if(pendingMessages_.isEmpty())
        retryTimer_.stop();
}

void AckUdpSender::RetryPendingMessages()
{
    QList<qint64> gaveUpList;

    QMutableMapIterator<qint64, PendingMessage> iterator(pendingMessages_);
    while (iterator.hasNext())
    {
        iterator.next();
        PendingMessage &message = iterator.value();

        if(message.sinceLastSend.elapsed() < SharedTypes::ackTimeoutMilliseconds)
            continue;

        if(message.attempts >= SharedTypes::maxSendAttempts)
        {
            cout << "No ack after " << message.attempts << " attempts for sequence " << iterator.key() << endl;
            gaveUpList.append(iterator.key());
            iterator.remove();
            continue;
        }

        socket_.writeDatagram(message.datagram, message.address, message.port);
        message.attempts++;
        message.sinceLastSend.start();
    }

    if(pendingMessages_.isEmpty())
        retryTimer_.stop();

    for(qint64 sequence : gaveUpList)
        emit GaveUp(sequence);
}