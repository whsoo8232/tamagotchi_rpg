# 다마고치 RPG (Tamagotchi RPG)

> 터미널에서 실행되는 C++17 기반 **다마고치 + RPG 융합 게임**
> 캐릭터를 육성·훈련시키고, 5스테이지 전투를 모두 승리해 **용사 엔딩**에 도달하는 것이 목표입니다.

**팀 6CanDoIt** · 조광일 · 김태현 · 이현빈 · 정주연 · 국민대학교 객체지향프로그래밍

---

## 🚀 배포 및 시작 방법

### 1. 요구 사항

- **OS**: Linux 또는 macOS (POSIX 터미널 환경) — WSL2 / Ubuntu 권장
- **컴파일러**: `g++` (C++17 이상)
- **터미널**: xterm 호환, 마우스 클릭 입력 지원
- **터미널 크기**: **가로 161컬럼 × 세로 44행 이상** (고정 레이아웃)
- **의존성**: C++ 표준 라이브러리만 사용 (외부 라이브러리 불필요)

### 2. 내려받기 (배포)

```bash
git clone https://github.com/whsoo8232/tamagotchi-rpg.git
cd tamagotchi-rpg
```

### 3. 빌드

```bash
# 빌드 스크립트 사용 (권장)
./build.sh
```

빌드 스크립트는 다음 명령을 실행합니다:

```bash
g++ -o game main.cpp src/models/*.cpp src/views/*.cpp src/controllers/*.cpp -I.
```

> 스크립트에 실행 권한이 없다면 `chmod +x build.sh` 후 실행하세요.
> 빌드가 끝나면 실행 파일 `game` 이 생성됩니다.

### 4. 실행 (시작)

```bash
./game
```

> 게임은 마우스 클릭으로 조작합니다. 실행 후 터미널 창을 **161×44 이상**으로 키운 뒤 시작하세요.
> 화면이 깨져 보이면 터미널 크기를 키우거나 폰트 크기를 줄이면 됩니다.
> 종료하려면 `Ctrl+C` — 터미널 설정은 자동으로 원래대로 복원됩니다.

---

## 🎮 게임 플레이

```
게임 시작
  └─ MainScreen (메인 허브: 육성 / 전투 / 장비 / 훈련)
       ├─ [육성] → 밥주기 / 목욕 / 놀아주기 / 치료
       ├─ [전투] → 스테이지 선택 → 가위바위보 전투 → 승리/패배
       ├─ [장비] → 검 강화 / 갑옷 강화 / 아이템 구매
       └─ [훈련] → 샌드백치기(공격력) / 달리기(체력) / 반응속도(민첩)
```

- **육성**: 배고픔·행복·청결·건강 4개 수치를 관리합니다. 전투를 거듭하면 수치가 떨어지므로 꾸준한 돌봄이 필요합니다.
- **훈련**: 3종의 미니게임으로 공격력·최대HP·민첩을 강화합니다.
- **전투**: 턴제 **가위바위보**로 진행되며, 이기면 데미지·지면 피격이 발생합니다. 전투 중 아이템도 사용 가능합니다.
- **장비**: 검/갑옷을 5티어까지 확률 강화하여 전투력을 높입니다.

### 엔딩 (총 5종)

| 엔딩 | 조건 |
|---|---|
| 🏆 **용사 (HERO)** | 5스테이지(티라노사우루스) 클리어 |
| 🍽 **굶주림 (STARVE)** | 배고픔 수치 0 도달 |
| 🏃 **도망 (RUNAWAY)** | 행복도 수치 0 도달 |
| 🗑 **쓰레기 (TRASH)** | 청결도 수치 0 도달 |
| 🏥 **병원 (HOSPITAL)** | 건강 수치 0 도달 |

---

## 🕹 조작 방법

- **마우스 클릭**: 하단 버튼 바 및 화면 내 선택지를 클릭하여 모든 조작을 수행합니다. (xterm SGR 마우스 프로토콜 사용)
- **Ctrl+C**: 게임 종료 (터미널 상태 자동 복원)

---

## 🏗 아키텍처 (MVC 패턴)

| 레이어 | 클래스 | 역할 |
|---|---|---|
| **Model** | `Character`, `Monster`, `Equipment`, `Item` | 플레이어/몬스터 데이터 및 정적 테이블 |
| **View** | `Renderer` | ANSI 색상·ASCII 아트 렌더링 (정적 유틸리티) |
| **Controller** | `GameEngine`, `InputHandler`, `Screen` 및 서브클래스 | 게임 루프, 입력 처리, 화면 관리 |

추상 클래스 `Screen`을 8개의 화면 클래스가 상속하며, `GameEngine`은 `unique_ptr<Screen>` 하나로 **다형성** 기반 화면 전환을 처리합니다. 새 화면 추가 시 `Screen` 서브클래스 하나만 작성하면 됩니다.

```cpp
// 게임 루프 핵심 (GameEngine::run)
while (running) {
    InputEvent event = inputHandler.pollEvent();        // 논블로킹 입력
    currentScreen->handleInput(*this, player, event);   // 입력 처리
    if (player.isEndingCondition()) { ... }             // 엔딩 판정
    currentScreen->render(player);                      // 렌더링
}
```

---

## 📁 프로젝트 구조

```
tamagotchi_rpg/
├── main.cpp                  # 진입점, GameEngine::run() 호출
├── build.sh                  # 컴파일 셸 스크립트
├── AA/                       # ASCII 아트 리소스 (캐릭터·몬스터·장비·아이템)
└── src/
    ├── models/
    │   ├── Character.h/.cpp   # 플레이어 모델 (육성·전투 스탯, 장비, 아이템)
    │   ├── Monster.h/.cpp     # 몬스터 모델 (스테이지별 팩토리 생성)
    │   ├── Equipment.h        # 검/갑옷 5티어 테이블
    │   └── Item.h             # 아이템 테이블
    ├── views/
    │   └── Renderer.h/.cpp    # 터미널 렌더링 유틸리티 (정적 클래스)
    └── controllers/
        ├── Screen.h           # 추상 기반 클래스
        ├── GameEngine.h/.cpp  # 게임 루프 및 화면 관리
        ├── InputHandler.h/.cpp# termios raw mode + xterm 마우스 파싱
        ├── MainScreen.*        # 메인 허브 화면
        ├── RaisingScreen.*     # 육성 화면
        ├── BattleScreen.*      # 전투 화면
        ├── TrainingScreen.*    # 훈련 선택 허브
        ├── PunchingBagScreen.* # 샌드백 미니게임
        ├── RunningScreen.*     # 달리기 미니게임
        ├── ReactionScreen.*    # 반응속도 미니게임
        ├── EquipmentScreen.*   # 장비/아이템 상점
        └── EndingScreen.*      # 엔딩 화면
```

---

## ⚙️ 핵심 기술 요소

- **추상 클래스 · 다형성**: `Screen` 인터페이스 기반 8개 화면을 `unique_ptr`로 통합 관리
- **저수준 I/O**: `termios` raw mode 입력, xterm 마우스 프로토콜 직접 파싱, ANSI Escape Code
- **논블로킹 타이머**: `std::chrono::steady_clock` + 상태 머신(`enum class`)으로 실시간 미니게임 구현
- **UTF-8 / CJK 정렬**: `getDisplayWidth()`로 한글·이모지의 표시 너비를 직접 계산하여 정확한 컬럼 정렬
- **RAII**: `InputHandler` 생성/소멸자에서 터미널 상태 자동 활성화·복원 (비정상 종료 시그널 핸들러 포함)
- **렌더링 최적화**: stdout 완전 버퍼링(65536B)으로 프레임 단위 출력 → 화면 깜빡임 방지

---

## 👥 팀원별 기여

| 팀원 | 담당 |
|---|---|
| **조광일** | 게임 엔진·입출력 (GameEngine, InputHandler, Screen, MainScreen) |
| **김태현** | 전투·장비 로직 (BattleScreen, EquipmentScreen, Monster, Equipment, Item) |
| **이현빈** | 훈련 로직·캐릭터 디자인 (Training/PunchingBag/Running/ReactionScreen, ASCII 아트) |
| **정주연** | 육성 로직·렌더링 (Character, RaisingScreen, Renderer) |

---

> 📄 자세한 설계·구현 내용은 `최종보고서_6CanDoIt_다마고치RPG.pdf` 를 참고하세요.
