#pragma once

#include <QPixmap>
#include <QPointF>
#include <QString>
#include <algorithm>
#include <cmath>

enum class PetMode {
    IdleWander,
    MovingToFood,
    Eating
};

struct VirtualPetState
{
    // 렌더링용 스프라이트
    QPixmap sprite;
    bool    valid = false;        // 이미지 로딩 성공 여부

    // 캐릭터 프로필 (JSON 원문)
    QString profileJson;

    // 위치 / 이동
    QPointF position { 100.0, 100.0 };   // 화면 좌표 (픽셀)
    QPointF velocity { 20.0,   0.0 };    // px/s 느낌

    // 먹이 관련
    bool    hasFoodTarget = false;
    QPointF foodPosition;               // 먹이가 떨어진 위치

    // 상태 수치 0.0 ~ 1.0
    double  hunger   = 0.5;             // 배고픔 (높을수록 배고픔)
    double  fullness = 0.5;             // 포만감
    double  affinity = 0.5;             // 호감도
    QString mood     = QStringLiteral("neutral");

    PetMode mode        = PetMode::IdleWander;
    double  eatingTimer = 0.0;          // 먹는 중 남은 시간 (초)

    // 방향 (좌우반전)
    bool facingLeft = false;

    // 매 프레임 호출 (dtMs : 밀리초)
    void onTick(double dtMs)
    {
        double dt = dtMs / 1000.0; // 초 단위

        // 기본적으로 시간이 지나면 조금씩 배고파짐 / 포만감 감소
        hunger   = std::clamp(hunger   + dt * 0.01, 0.0, 1.0);
        fullness = std::clamp(fullness - dt * 0.01, 0.0, 1.0);

        // 아주 단순한 무드 갱신 로직
        if (fullness > 0.7 && affinity > 0.6) {
            mood = QStringLiteral("행복");
        } else if (hunger > 0.8) {
            mood = QStringLiteral("배고픔");
        } else {
            mood = QStringLiteral("보통");
        }

        // 상태에 따라 이동/행동
        switch (mode) {
        case PetMode::IdleWander:
        {
            // 그냥 좌우로 살짝 움직이는 느낌
            position += velocity * dt;
            break;
        }
        case PetMode::MovingToFood:
        {
            if (!hasFoodTarget)
                break;

            QPointF dir = foodPosition - position;
            double len2 = dir.x() * dir.x() + dir.y() * dir.y();
            if (len2 < 1e-4) {
                // 거의 도착
                position = foodPosition;
                mode = PetMode::Eating;
                eatingTimer = 1.5; // 1.5초 동안 먹는 애니메이션
                break;
            }

            double len = std::sqrt(len2);
            QPointF step = (dir / len) * (60.0 * dt); // 초당 60px 정도
            if (step.x() != 0.0) {
                facingLeft = (step.x() < 0.0);
            }
            position += step;
            break;
        }
        case PetMode::Eating:
        {
            eatingTimer -= dt;
            if (eatingTimer <= 0.0) {
                // 먹기 완료
                mode = PetMode::IdleWander;
                hasFoodTarget = false;
            }
            break;
        }
        }
    }

    // 먹이를 먹었을 때 호출
    void onFed()
    {
        // 배고픔 감소 / 포만감 증가 / 호감도 살짝 증가
        hunger   = std::max(0.0, hunger   - 0.4);
        fullness = std::min(1.0, fullness + 0.4);
        affinity = std::min(1.0, affinity + 0.15);
        mood = QStringLiteral("고마움");
    }
};
