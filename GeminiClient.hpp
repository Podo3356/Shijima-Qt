#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>

// Gemini API와 통신하는 간단한 래퍼
class GeminiClient : public QObject
{
    Q_OBJECT
public:
    struct PetContext {
        QString petName;
        float   hunger   = 0.0f;
        float   fullness = 0.0f;
        float   affinity = 0.0f;
        QString mood;
        QString lastEvent;  // "사용자가 먹이를 줬다" 같은 설명
    };

    explicit GeminiClient(QObject *parent = nullptr);

    // 환경변수 GEMINI_API_KEY를 사용해서 호출
    void askPet(const PetContext &ctx);

signals:
    void replyReady(const QString &text);

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager m_nam;
};
