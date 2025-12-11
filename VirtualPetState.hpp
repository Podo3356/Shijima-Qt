// VirtualPetState.hpp
#pragma once

#include <QPointF>
#include <QPixmap>
#include <QString>

enum class PetMode {
    IdleWander,
    MovingToFood,
    Eating
};

struct VirtualPetState {
    // 렌더링용
    QPixmap sprite;
    bool valid = false;          // 이미지 로딩 성공 여부

    // 캐릭터 프로필 (JSON 원문)
    QString profileJson;

    // 위치 / 이동
    QPointF position { 100.0, 100.0 };
    QPointF velocity { 20.0, 0.0 };  // px / sec 단위 느낌으로

    // 먹이 관련
    bool hasFoodTarget = false;
    QPointF foodPosition;
    double hunger = 0.5;         // 0.0~1.0

    PetMode mode = PetMode::IdleWander;
    double eatingTimer = 0.0;    // 먹는 중 남은 시간 (초)

    // 방향 (좌우반전용)
    bool facingLeft = false;
};
