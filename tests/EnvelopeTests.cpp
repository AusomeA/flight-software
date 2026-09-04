#include <gtest/gtest.h>
#include "EnvelopeJson.h"
#include "SharedTypes.h"

static Envelope NonDefaultEnvelope()
{
    Envelope envelope;
    envelope.type = SharedTypes::faultInjectionMessageType;
    envelope.sequence = 42;
    envelope.body["faultName"] = "stuckSensor";
    envelope.body["enabled"] = true;
    return envelope;
}

TEST(EnvelopeJson, RoundTripCheck)
{
    const Envelope original = NonDefaultEnvelope();
    const std::optional<Envelope> parsed = EnvelopeFromJson(EnvelopeToJson(original));
    
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->type, original.type);
    EXPECT_EQ(parsed->sequence, original.sequence);
    EXPECT_EQ(parsed->body, original.body);
}

TEST(EnvelopeJson, RejectsGarbageCheck)
{
    EXPECT_FALSE(EnvelopeFromJson("not json at all").has_value());
    EXPECT_FALSE(EnvelopeFromJson("[1, 2, 3]").has_value());
    EXPECT_FALSE(EnvelopeFromJson("").has_value());
}

TEST(EnvelopeJson, RejectsMissingKeysCheck)
{
    for(const QString &keyToRemove : requiredEnvelopeKeys)
    {
        QJsonDocument document = QJsonDocument::fromJson(EnvelopeToJson(NonDefaultEnvelope()));
        QJsonObject json = document.object();
        json.remove(keyToRemove);

        const QByteArray damagedJson = QJsonDocument(json).toJson(QJsonDocument::Compact);
        EXPECT_FALSE(EnvelopeFromJson(damagedJson).has_value()) << "accepted envelope is missing key: " << keyToRemove.toStdString();
    }
}

TEST(EnvelopeJson, EmptyBodyIsValidCheck)
{
    Envelope envelope = NonDefaultEnvelope();
    envelope.body = QJsonObject();

    const std::optional<Envelope> parsed = EnvelopeFromJson(EnvelopeToJson(envelope));
    ASSERT_TRUE(parsed.has_value());
    EXPECT_TRUE(parsed->body.isEmpty());
}