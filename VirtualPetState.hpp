#pragma once

#include <algorithm>
#include <QString>

// 아주 간단한 가상 펫 상태 구조체
struct VirtualPetState
{
    // 0 = 배부름, 100 = 매우 배고픔
    float hunger   = 40.0f;
    // 0 = 비어 있음, 100 = 가득 참
    float fullness = 60.0f;
    // 0 = 서먹, 100 = 매우 친함
    float affinity = 50.0f;

    QString mood   = QStringLiteral("평온함");

    // 먹이를 다 먹은 직후 호출
    void onFed()
    {
        hunger   = std::max(0.0f, hunger - 25.0f);
        fullness = std::min(100.0f, fullness + 25.0f);
        affinity = std::min(100.0f, affinity + 5.0f);
        updateMood();
    }

    // 틱마다 호출 (약 25fps 기준 한 틱당 40ms 정도로 생각)
    void onTick(int ms)
    {
        // 10분마다 hunger +1 정도
        hunger = std::min(100.0f, hunger + ms / 600000.0f);
        updateMood();
    }

    void updateMood()
    {
        if (hunger > 80.0f) {
            mood = QStringLiteral("배고픔");
        } else if (affinity > 80.0f) {
            mood = QStringLiteral("매우 행복함");
        } else if (affinity < 20.0f) {
            mood = QStringLiteral("서먹함");
        } else {
            mood = QStringLiteral("평온함");
        }
    }
};
