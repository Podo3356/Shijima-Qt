// PetLoader.cc
#include "PetLoader.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace PetLoader {

bool loadPet(const QString &rootDir, VirtualPetState &outState)
{
    // 1) 이미지 로딩
    const QString imagePath = rootDir + "/pet.png";
    QPixmap sprite;
    if (!sprite.load(imagePath)) {
        qWarning() << "[PetLoader] Failed to load pet image:" << imagePath;
        return false;
    }

    // 2) JSON 로딩
    const QString profilePath = rootDir + "/profile.json";
    QFile f(profilePath);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "[PetLoader] Failed to open profile:" << profilePath;
        return false;
    }
    const QByteArray bytes = f.readAll();
    f.close();

    QJsonParseError err {};
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "[PetLoader] JSON parse error:" << err.errorString();
        return false;
    }

    // 전체 JSON을 문자열 그대로 Gemini에게 줄 거라서
    outState.sprite = sprite;
    outState.valid = true;
    outState.profileJson = QString::fromUtf8(bytes);

    // 초기 위치는 대충 오른쪽 아래
    outState.position = QPointF(300.0, 300.0);
    outState.velocity = QPointF(30.0, 0.0);
    outState.hunger = 0.5;
    outState.mode = PetMode::IdleWander;
    outState.hasFoodTarget = false;
    outState.eatingTimer = 0.0;
    outState.facingLeft = false;

    return true;
}

} // namespace PetLoader
