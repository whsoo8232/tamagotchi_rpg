# Tamagotchi RPG

터미널에서 실행되는 C++ 기반 다마고치 + RPG 융합 게임 
캐릭터를 육성하고, 전투에서 승리하는 것이 목표

---

## 요구 사항

- Linux / macOS (POSIX 터미널 환경)
- `g++` (C++17 이상)
- 터미널 크기: **가로 120컬럼 × 세로 35행 이상** 권장
- xterm 호환 터미널 (마우스 클릭 지원 시)

---

## 빌드 및 실행

```bash
# 빌드
./build.sh

# 실행
./game
```

빌드 스크립트 없이 직접 컴파일:

```bash
g++ -o game main.cpp src/models/*.cpp src/views/*.cpp src/controllers/*.cpp -I.
```

---

## 게임 구조

```
tamagotchi_rpg/
├── main.cpp
├── build.sh
├── dots/                     # ASCII 아트 파일
│   ├── monster1.txt ~ monster5.txt
│   ├── rock.txt / paper.txt / scissors.txt
├── src/
│   ├── models/
│   │   ├── Character         # 플레이어 스탯 관리
│   │   └── Monster           # 몬스터 스탯 및 생성
│   ├── views/
│   │   └── Renderer          # ANSI 코드 기반 터미널 렌더링
│   └── controllers/
│       ├── GameEngine        # 메인 게임 루프
│       ├── InputHandler      # 키보드 / 마우스 입력 처리
│       ├── Screen            # 화면 추상 기반 클래스
│       └── *Screen           # 각 화면 구현
```

---

## 아키텍처 (MVC)

| 레이어 | 역할 |
|---|---|
| **Model** | `Character`, `Monster` — 순수 데이터 및 상태 관리 |
| **View** | `Renderer` — ANSI 이스케이프 코드로 터미널 출력 |
| **Controller** | `GameEngine` + `*Screen` — 게임 흐름 및 입력 처리 |

`GameEngine`은 단일 `Character player`와 현재 화면(`currentScreen`)을 소유하며, 매 루프마다 `handleInput()` → `render()` 순으로 실행합니다.  
화면 전환은 `engine.changeScreen(std::make_unique<XScreen>())` 호출로 이루어집니다.

---

## 화면 목록

| 화면 | 단축키 | 설명 |
|---|---|---|
| `MainScreen` | — | 시작 허브. 캐릭터 아트 및 4개 메뉴 버튼 표시 |
| `RaisingScreen` | `1` | 밥주기 / 목욕 / 놀기 / 치료 — 기본 육성 행동 |
| `BattleScreen` | `2` | 스테이지 선택 + 가위바위보 전투 |
| `EquipmentScreen` | `3` | 장비 관리 |
| `TrainingScreen` | `4` | 공격력 / HP / 민첩 스탯 훈련 |
| `PunchingBagScreen` | — | 훈련 미니게임: 샌드백 치기 |
| `RunningScreen` | — | 훈련 미니게임: 달리기 |
| `ReactionScreen` | — | 훈련 미니게임: 반응속도 |

---

## 캐릭터 스탯

| 스탯 | 범위 | 설명 |
|---|---|---|
| `hunger` | 0–100 | 허기. 0이 되면 위험 |
| `happiness` | 0–100 | 행복도 |
| `cleanliness` | 0–100 | 청결도 |
| `health` | 0–100 | 건강 |
| `attack` | — | 전투 공격력 |
| `hp` | — | 전투 HP |
| `agility` | — | 민첩 (회피 등에 영향) |
| `level` / `exp` | — | 레벨 및 경험치 |
| `money` | — | 보유 금액 |

`Character::isEndingCondition()` 이 `true`를 반환하면 게임이 즉시 종료됩니다.

---

## 전투 시스템

- 스테이지 1–5 중 선택하면 해당 스테이지 몬스터가 생성됩니다.
- 전투는 **가위바위보** 방식으로 진행됩니다.
- 각 몬스터는 `dots/monster*.txt`의 ASCII 아트를 가집니다.
- 몬스터는 HP, 공격력, 회피율(`evasion`) 스탯을 보유합니다.

---

## 입력 방식

### 키보드
- `1`–`4`: 각 화면으로 이동 (메인에서)
- `q` / `Esc`: 뒤로 가기 또는 종료
- 방향키 (`↑↓←→`): 일부 화면에서 선택 이동

### 마우스
- xterm SGR 마우스 프로토콜(`\033[?1006h`) 사용
- 클릭 좌표 `(x, y)`를 직접 파싱하여 버튼 영역 판정
- 하단 메뉴바: `y >= 30` 영역, 약 120컬럼을 버튼 수로 균등 분할

---

## 게임 오버 조건

`Character::isEndingCondition()` 내부 조건을 충족하면 (예: 특정 스탯이 한계치에 도달) 즉시 게임 종료 메시지가 출력됩니다.
