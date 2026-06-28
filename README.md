# 팰월드 모작 (PalWorld Replica)
> **Unreal Engine 5와 C++를 활용하여 구현한 팰월드(PalWorld) 보스전 모작 3D 액션 프로젝트**

본 프로젝트는 팰월드의 핵심 플레이인 **보스 몬스터(그리즐볼트)전**을 재현하기 위해 팀원들과 협업하여 진행한 언리얼 엔진 5 기반 프로젝트입니다. 플레이어와 소환된 펠(Anubis 등)의 협동 전투를 목표로 설계되었으며, 보스의 동적 의사 결정 AI와 전투 흐름 설계를 총괄하였습니다.

---

## 🛠️ 개발 환경 및 사용 기술
- **Engine**: Unreal Engine 5.3+
- **Language**: C++ (핵심 로직), Blueprint (애셋 맵핑 및 연출)
- **Design Pattern**: Component-Based FSM (유한 상태 머신), Observer 패턴 (Delegate 연동)
- **Physics**: Radial Force Component, Character Ragdoll, Collision Raycasting

---

## ✨ 담당 역할 & 핵심 구현 기술 (양원규)

프로젝트 내에서 **보스 캐릭터(그리즐볼트)의 상태 제어 컴포넌트(FSM) 설계, 동적 AI 포지셔닝 알고리즘, 그리고 이벤트 기반의 실시간 타겟 갱신 시스템**을 단독 설계 및 개발하였습니다.

### 1. 컴포넌트 기반 FSM 설계 (`UGrizzBoltActorComponent`)
- 보스 캐릭터 클래스(`AGrizzBolt`)의 가독성과 확장성을 위해 상태 제어 로직을 컴포넌트로 분리하는 **Component-Based Architecture**를 적용했습니다.
- 총 9가지 행동 상태(`IDLE`, `MOVE`, `SKILL1~4`, `DAMAGE`, `DIE`, `FINDTARGET`)를 매 프레임 안전하게 추적 및 전이하는 FSM 엔진을 구축하였습니다.

### 2. 거리 기반 동적 포지셔닝 AI 알고리즘 (`TickMove`)
단순히 유저를 향해 직선 돌진하는 단조로운 패턴을 탈피하기 위해, 타겟과의 거리 데이터를 세분화한 **의사 결정 트리**를 구축했습니다.
- **감지 및 타겟팅**: 감지 범위(`DetectionRange: 11000f`) 내 타겟 감지 시 이동을 개시하고, 유효 범위 밖으로 이탈 시 즉시 `IDLE` 상태로 복구됩니다.
- **무작위 콤보 공격**: 적정 사거리(`SkillDistance: 1500f`) 도달 시 무작위 확률 분기(`FMath::RandRange`)를 통해 쿨타임 기반의 3가지 스킬(라인 썬더, 트리플 스파크, 미니건 사격)을 유기적으로 발동합니다.
- **백스텝 및 회피 (Backstepping)**: 타겟과 초근접(`NearDistance: 300f` 이하) 시 타겟의 반대 방향 벡터(`-DirToTarget.GetSafeNormal()`)로 이동 입력을 주어 안전거리를 확보하는 회피 기동을 수행합니다. 
- **백스텝 연계 중거리 기믹**: 백스텝 도중 **1/5 확률**로 중거리(`MiddleDistance: 700f`)에서 회피를 긴급 제동하고, 타겟 방향으로 고개를 돌려 강력한 범위형 낙뢰 스킬(`SKILL3`)을 발동하도록 연계 패턴을 설계했습니다.

```cpp
// [구현 핵심] TickMove 중 일부 - 근접 시 백스텝 및 중거리 스킬 연계 로직
if (CachedDistanceToTarget <= NearDistance)
{
    if (!bIsBackingAway)
    {
        // 1/5 확률로 중거리(700f)에서 제동 후 Skill3 연계 설정
        bIsMovingToMiddleDistance = FMath::RandRange(0, 4) == 0; 
        bIsBackingAway = true;
    }
}

if (bIsBackingAway)
{
    FVector MoveBackDirection = -DirToTarget.GetSafeNormal();
    FRotator BackRotation = MoveBackDirection.Rotation();
    BackRotation.Pitch = FMath::Clamp(BackRotation.Pitch, -10.f, 25.f);
    Me->SetActorRotation(BackRotation);
    Me->AddMovementInput(MoveBackDirection); // 반대 방향 회피 이동

    if (bIsMovingToMiddleDistance && CachedDistanceToTarget >= MiddleDistance)
    {
        // 중거리 도달 시 즉시 상태를 리셋하고 SKILL3 시퀀스 발동
        bIsBackingAway = false;
        CurrentState = EGrizzBoltState::SKILL3;
        TransitionToSkillState();
        return;
    }
}
```

### 3. 멀티캐스트 델리게이트 연동 실시간 어그로(Aggro) 시스템 (`OnPalRetrieved`)
플레이어가 전투 중 펠(Pal)을 소환하거나 해제(회수)하는 경우 발생할 수 있는 **타겟 유실 버그를 방지**하기 위해 이벤트 기반 시스템을 설계했습니다.
- 플레이어 캐릭터의 **펠 소환/회수 멀티캐스트 델리게이트(`OnPalInteract`)**에 보스 컴포넌트의 `OnPalRetrieved` 멤버 함수를 직접 바인딩(`AddUObject`)했습니다.
- 소환된 펠이 회수(`!bIsSummon`)되는 즉시 이벤트 시그널을 감지하여 실시간으로 타겟 탐색 함수(`FindTarget()`)를 재작동시킵니다.
- 월드 내 살아있는 모든 `ACharacter`들의 상태 컴포넌트(`UPStatusComponent`) 정보를 루프 조회하여, 생존 여부(`IsDead()`)를 실시간 검증하고 가장 가까운 액터를 즉시 새로운 타겟으로 지정함으로써 무결한 전투 흐름을 보장합니다.

```cpp
// [구현 핵심] 델리게이트 이벤트 수신 및 실시간 타겟 재탐색
void UGrizzBoltActorComponent::OnPalRetrieved(bool bIsSummon)
{
    if (!bIsSummon)
    {
        // 소환되어 있던 펠이 회수되면, 즉시 가장 가까운 다른 생존 대상을 찾아 타겟 갱신
        Target = Cast<ACharacter>(FindTarget());
    }
}
```

### 4. 물리 시뮬레이션 연동 (`GrizzBolt.cpp`)
- 보스의 사망 상태(`EGrizzBoltState::DIE`)로 전이되는 즉시 메시의 피지컬 에셋 물리 시뮬레이션을 활성화(`SetAllBodiesSimulatePhysics(true)`)하여 **Ragdoll 물리 사망 효과**를 연출했습니다.
- 범위 공격 발동 시 `URadialForceComponent`를 활용해 타겟 및 주변 펠들을 방사형으로 밀쳐내는 넉백 임팩트를 제어했습니다.

---

## 👥 협업 파트 요약

- **플레이어 시스템 (최정선)**: 캐릭터 기본 기동, 총기 사격 메커니즘, 펠 소환 인터페이스 제어
- **펠 시스템 (Justin)**: 아누비스/캐티메이지 펠 소환체 AI 패턴 설계 및 고유 스킬 구현
- **레벨 디자인 (김소연)**: 보스전 아레나 레벨 및 연출 환경 기획, 콜리전 볼륨 배치
- **기획 및 발표 (최요한)**: 보스 패턴 기획 및 타임라인 연출 리소스 관리, 최종 프레젠테이션
