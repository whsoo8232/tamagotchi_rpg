# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

```bash
# Build
bash build.sh
# or directly:
g++ -o game main.cpp src/models/*.cpp src/views/*.cpp src/controllers/*.cpp -I.

# Run
./game
```

No test suite exists. Verify changes by building and running manually.

## Architecture

This is a terminal-based tamagotchi RPG written in C++. The design follows MVC:

- **`src/models/`** — Data only. `Character` holds the player's stats (hunger, happiness, cleanliness, health, money, attack, hp, agility, level, exp). `Monster` holds enemy stats and exposes `Monster::generateStageMonster(stage)` to create stage-appropriate enemies.
- **`src/views/Renderer`** — All terminal output goes through here. Uses ANSI escape codes for color and cursor control. `clearCanvas()` redraws the whole screen each frame. Layout assumes ~120 columns wide and ~35 rows tall.
- **`src/controllers/`** — Game logic and flow:
  - `GameEngine` owns the game loop, a single `Character player`, and a `std::unique_ptr<Screen> currentScreen`. Each loop iteration calls `currentScreen->handleInput()` then `currentScreen->render()`.
  - `Screen` is an abstract base class. All screens implement `render(player)` and `handleInput(engine, player, event)`.
  - Screens navigate to each other via `engine.changeScreen(std::make_unique<XScreen>())`.
  - `InputHandler` sets the terminal to raw mode (via `termios`) and parses both keyboard and mouse click events (xterm mouse protocol).

## Screen Inventory

| Screen | Key | Description |
|---|---|---|
| `MainScreen` | hub | Shows character art + 4 menu buttons |
| `RaisingScreen` | 1 | Feed, bathe, play, treat actions |
| `BattleScreen` | 2 | Stage select + rock-paper-scissors combat |
| `TrainingScreen` | 4 | Train attack/hp/agility stats |
| `EquipmentScreen` | 3 | Equipment management |

## Input Handling

Both keyboard and mouse are supported. Mouse coordinates are relative to terminal cell position. The bottom menu bar is detected at `event.y >= 30`; buttons divide the ~120-column width into equal segments. Keyboard shortcuts in each screen use number keys (`1`–`4`) and `q`/`Esc` to quit.

## ASCII Art

`dots/` contains ASCII art text files for monsters (`monster1.txt`–`monster5.txt`) and RPS choices (`rock.txt`, `paper.txt`, `scissors.txt`). `Monster::generateStageMonster()` reads these files to populate `Monster::ascii`.

## Game-Over Condition

`Character::isEndingCondition()` is checked every input event in the main loop. If it returns true, the game ends immediately with a message.
