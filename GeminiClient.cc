#include <QSettings>
#include "GeminiClient.hpp"
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

GeminiClient::GeminiClient(QObject *parent)
    : QObject(parent)
{
}

void GeminiClient::askPet(const PetContext &ctx)
{
    QSettings settings;
    QString apiKey = settings.value("Gemini/ApiKey").toString();

    // fallback(환경변수)
    if (apiKey.isEmpty()) {
        apiKey = qEnvironmentVariable("GEMINI_API_KEY");
    }
    
    if (apiKey.isEmpty()) {
        qWarning() << "[GeminiClient] Gemini API key not set.";
        return;
    }

    QUrl url(QStringLiteral(
        "https://generativelanguage.googleapis.com/"
        "v1beta/models/gemini-2.5-flash:generateContent"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("key"), apiKey);
    url.setQuery(query);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  QStringLiteral("application/json; charset=utf-8"));

    QString prompt = QStringLiteral(
        "You are a cute desktop virtual pet named %1. "
        "You speak in short, friendly Korean sentences.\n"
        "State:\n"
        "- Hunger: %2/100\n"
        "- Fullness: %3/100\n"
        "- Affinity with user: %4/100\n"
        "- Mood: %5\n"
        "Describe your reaction to this event:\n"
        "%6\n"
        "Respond only as the pet, in first person, within one or two sentences.")
        .arg(ctx.petName)
        .arg(ctx.hunger)
        .arg(ctx.fullness)
        .arg(ctx.affinity)
        .arg(ctx.mood)
        .arg(ctx.lastEvent);

    QJsonObject part;
    part.insert(QStringLiteral("text"), prompt);

    QJsonArray parts;
    parts.append(part);

    QJsonObject content;
    content.insert(QStringLiteral("parts"), parts);

    QJsonArray contents;
    contents.append(content);

    QJsonObject root;
    root.insert(QStringLiteral("contents"), contents);

    const QJsonDocument doc(root);
    const QByteArray body = doc.toJson(QJsonDocument::Compact);

    QNetworkReply *reply = m_nam.post(req, body);
    connect(reply, &QNetworkReply::finished,
            this, &GeminiClient::onReplyFinished);
}

void GeminiClient::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        return;
    }

    const QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString text;

    if (doc.isObject()) {
        const QJsonObject root = doc.object();
        const QJsonArray candidates = root.value(QStringLiteral("candidates")).toArray();
        if (!candidates.isEmpty()) {
            const QJsonObject c0 = candidates.first().toObject();
            const QJsonObject content =
                c0.value(QStringLiteral("content")).toObject();
            const QJsonArray parts =
                content.value(QStringLiteral("parts")).toArray();
            if (!parts.isEmpty()) {
                text = parts.first().toObject()
                           .value(QStringLiteral("text"))
                           .toString();
            }
        }
    }

    if (!text.isEmpty()) {
        emit replyReady(text.trimmed());
    } else {
        qWarning() << "[GeminiClient] Empty or invalid response";
    }
}
