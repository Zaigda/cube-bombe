# Cube-bombe

The cube-bombe is a bomb that two players defuse together (Enjmin "ALT in&out" workshop). It holds 3 enigmas (puzzles), played in any order:
in each one, a player acts without seeing and the other guides. You cannot lose an enigma: only the global timer makes the bomb explode.
This repo is the firmware, the program that runs on the cube's ESP32-S3 boards (small programmable boards). Today it runs only enigma 1's snake,
on board A: 2 HUB75 panels (64x32 LED matrices) form one 64x64 picture, and 2 buttons turn the snake a quarter turn to its own left or right
(relative to where the head points, not to the screen), at its next move.

## Change the game

Paths are relative to `firmware/src/`. After a change, build and upload again (see "Build and upload"). In `Config.h`, values are grouped
in blocks: `SnakeTuning::STEP_MS` is the line `constexpr uint32_t STEP_MS = 300;` inside the block `namespace SnakeTuning { ... }`.
Search for the last word only (`STEP_MS`), change only the number, and keep the `;` at the end.

| I want to... | File | Change this | Now |
|---|---|---|---|
| Make the snake faster or slower | `Config.h` | `SnakeTuning::STEP_MS`: time between two moves, in ms. Lower = faster. Speed in LEDs per second = `CELL_SIZE * 1000 / STEP_MS`. Only the last press before each move counts: at high speed, two quick presses give a single quarter turn | `300` (about 13 LEDs/s) |
| Make the cells bigger or smaller | `Config.h` | `SnakeTuning::CELL_SIZE`: width of one cell (snake part or apple), in LEDs. The grid is `64 / CELL_SIZE` cells wide and `SNAKE_PANELS * 32 / CELL_SIZE` high: pick a value that divides 32. On 2 panels, 2 gives 32 x 32 cells, 4 gives 16 x 16, 8 gives 8 x 8. Keep the grid much bigger than the snake: the apple needs a free cell, or the game freezes | `4` (16 x 16 grid) |
| Change the starting length | `Config.h` | `SnakeTuning::START_LENGTH`, in cells (1 cell = `CELL_SIZE` x `CELL_SIZE` LEDs). At most `START_COLUMN + 1` (the tail is drawn left of the head) and at most `MAX_LENGTH` | `2` |
| Change where the snake starts | `Config.h` | `SnakeTuning::START_COLUMN`: column of the head, 0 = left edge. It starts on the middle row, going right: keep it at most the grid width minus 2, or it hits the wall at once and restarts forever | `3` |
| Change how much it grows | `Config.h` | `SnakeTuning::GROWTH_FACTOR`: each apple multiplies the length (1 = never grows). Capped by `MAX_LENGTH` in `Enigma/SnakeEnigma.h` | `2` (2, 4, 8, 16 cells), cap `64` |
| Change the apple colors | `Core/BombCode.cpp` | `COLORS`: one color per apple, from `Core/Palette.h`. In the enigma 1 design, they will be the colors of the real cable plugs | `RED, GREEN, BLUE, YELLOW` |
| Always use the same apple order | `main.cpp` | Remove the 2 `code.Shuffle()` calls (they pick a new random order at boot and after each win). The apples then follow `COLORS` | random |
| Change the number of apples | `Core/BombCode.h` | `BombCode::LENGTH`. Then give `COLORS` (in `Core/BombCode.cpp`) exactly that many colors (a missing one is black, so invisible; an extra one does not compile), and write `0` to `LENGTH - 1` in `order` (in `BombCode.h`, just below `LENGTH`) | `4` |
| Change the snake colors | `Enigma/SnakeEnigma.cpp` | In `Draw()`, the line `... (i == 0) ? Palette::ORANGE : Palette::WHITE;`: the first color is the head, the second the body. Avoid apple colors: the guide would confuse them | orange, white |
| Change the background | `main.cpp` | `snakeScreen.fillScreen(Palette::BLACK)` in `loop()` | black |
| Add a color | `Core/Palette.h` | After `ORANGE`, before the closing `}`, add a line like `constexpr uint16_t PURPLE = Rgb(160, 0, 255);` (red, green, blue, from 0 to 255). Then use `Palette::PURPLE` anywhere (in `COLORS`, in `Draw()`...). LEDs do not look like a PC screen: check it on the panel | |
| Fix double or missed button presses | `Inputs/PinButton.h` | `DEBOUNCE_MS`: how long a press must stay stable before it counts, in ms | `30` |
| Change the panel brightness | `Config.h` | `Panels::BRIGHTNESS`, from 0 to 255 | `60` |
| Fix a panel shown upside down | `Config.h` | `Panels::ROTATED_180`: one value per panel, in the order panel 0, 1, 2, 3 (panel 0 = the one whose ribbon cable comes from the board; 2 and 3 are not plugged yet). Swap the wrong panel's value (`true` <-> `false`). Correct picture at power-on: the snake (orange head, white body) appears on the left, just below the line between the two panels, moving right. If the top half of the picture is on the bottom panel, swap the two panels: `ROTATED_180` only turns each panel on its own | `{false, true, false, false}` |
| Change the number of panels | `Config.h` | `Panels::PANEL_COUNT`: panels in the chain, from 1 to `MAX_PANELS` (4). `Panels::SNAKE_PANELS`: how many of them the snake uses, from panel 0 | `2`, `2` |
| Move a button | `Config.h` | `Pins::SNAKE_LEFT_BUTTON`, `Pins::SNAKE_RIGHT_BUTTON` (GPIO numbers, see "Wiring"). Then move the wire to the new pin (the other leg stays on GND) | `41`, `42` |

## How it works

`setup()` runs once at power-on, then `loop()` runs again and again, many times per second. `main.cpp` creates every object (a named part of
the program with its own data, like `snake`) and connects them. A class is the kind of an object: `snake` is a `SnakeEnigma`, written in `SnakeEnigma.h/.cpp`.

Every loop reads `now = millis()` (milliseconds since power-on) once and passes it to everyone, then:
1. **Read the buttons**: `inputs.Update(now)`.
2. **Update the enigma**: `snake.Update(now)` moves the snake once every `STEP_MS` and returns an event: `None`; `Mistake` (it hit a wall or itself: it
   restarts by itself, back to `START_LENGTH` and the first apple, same colors); or `Solved` (all apples eaten: `loop()` then calls `code.Shuffle()` and
   `snake.Start(now)`, which gives a new random order and a new snake).
3. **Draw**: clear `snakeScreen` to black, then `snake.Draw(now)` paints the apple and the snake on it.
4. **Show**: `output.Show()` sends the picture to the panels, at most once every `Panels::FRAME_MS` (20 ms).

The objects of `main.cpp`, written `object (Class)`. Arrows follow the data through one loop; the word on an arrow is the function that links them.

```text
inputs (InputManager) --Update(now)--> leftButton (PinButton, GPIO 41) and rightButton (PinButton, GPIO 42)
                                                   |  WasPressed()
                                                   v
code (BombCode) ------- GetColor() ------> snake (SnakeEnigma)
                                                   |  Draw(now) paints on
                                                   v
                                           snakeScreen (Screen)    the 64x64 picture in memory
                                                   |  read by output.Show()
                                                   v
                                           output (PanelOutput) --> panel 0: rows 0-31
                                                                --> panel 1: rows 32-63, turned 180
```

## Files and classes

`firmware/platformio.ini` is the build setup (board, pinned library versions, the `cube` env). The code is in `firmware/src/`:

| File | Class | Role |
|---|---|---|
| `main.cpp` | | Creates and connects every object; `setup()` and `loop()` |
| `Config.h` | | Values to tune and pins, in the blocks `Timing`, `SnakeTuning`, `Panels`, `Pins` |
| `Core/Palette.h` | | The panel colors |
| `Core/BombCode.h/.cpp` | `BombCode` | The secret apple color order of a game. `Shuffle()` picks a new random order |
| `Core/Countdown.h/.cpp` | `Countdown` | The bomb timer: counts down, `Stop()` freezes it, penalties shorten it. Not wired yet |
| `Core/GameManager.h/.cpp` | `GameManager` | A whole game: waiting, playing the enigmas, defused or exploded. Not wired yet |
| `Enigma/Enigma.h` | `Enigma` | The base of every enigma: `Start()` resets it, `Update()` plays one loop and returns an event, `Draw()` paints it |
| `Enigma/SnakeEnigma.h/.cpp` | `SnakeEnigma` | Enigma 1: the snake |
| `Inputs/Sensor.h` | `Sensor` | Anything `InputManager` reads: `Begin()` once in setup, `Update()` every loop |
| `Inputs/Button.h` | `Button` | A two-state sensor: `IsDown()` while held, `WasPressed()` only on the loop where the press starts |
| `Inputs/PinButton.h/.cpp` | `PinButton` | A push button between a pin and GND. A change counts once stable for 30 ms, which hides the contact bounce |
| `Inputs/InputManager.h/.cpp` | `InputManager` | Keeps every sensor and calls `Begin()` and `Update()` on all of them |
| `Outputs/Screen.h/.cpp` | `Screen` | A picture in memory, 64 LEDs wide and 32 x (number of panels) tall; you choose its first panel when you create it. Enigmas draw on it with Adafruit GFX, a drawing library (`fillRect`, `drawLine`, `drawCircle`, `print`...) |
| `Outputs/Output.h/.cpp` | `Output` | Displays Screens: `AddScreen()` then `Begin()` in setup (`Begin()` returns false on an error), then `Show()` every loop |
| `Outputs/PanelOutput.h/.cpp` | `PanelOutput` | The `Output` for the HUB75 panels: it cuts each Screen into 32-row slices, one per panel, turned when `ROTATED_180` says so. Panel 0 is plugged into the board, panel 1 into panel 0. Its `Begin()` refuses a Screen of the wrong size, past the end of the chain, or on a panel already used. Enigmas never talk to the panels |

## Add a new enigma

Today there is one program (env `cube`, board A). Until there is one env per board, write and test a new enigma here, on its own panel.
Its class files (`Enigma/...`) will move unchanged to its board's program.

1. **The class**: create `Enigma/MyEnigma.h` (and a `.cpp`, like `SnakeEnigma`). It derives from `Enigma` (`class MyEnigma : public Enigma`) and fills in
   `Start()` (reset), `Update()` (one loop; returns `EnigmaEvent::None`, `Mistake` or `Solved`) and `Draw()`. No penalty: on a `Mistake`, `loop()` only prints a message.
2. **Inputs**: add its pin constant and its `static_assert` line at the end of the block `Pins` in `Config.h`. Create a `PinButton` in `main.cpp`, and
   `inputs.Add()` it in `setup()` before `inputs.Begin()`. A new kind of sensor (microphone, light) is a class derived from `Sensor`, like `PinButton`.
3. **Picture**: create a `Screen` on a free panel (its last argument is its first panel), `output.AddScreen()` it before `output.Begin()`, raise
   `PANEL_COUNT`, and set the new panel's `ROTATED_180` value. Two Screens on one panel stop the board at boot (see the monitor).
4. **Run it**: create the enigma in `main.cpp` and `Start()` it in `setup()`. In `loop()`, before `output.Show()`: `Update(now)` (react to its event),
   clear its Screen, `Draw(now)`.

Example: an enigma solved by 3 presses, shown on a third panel (kept in one file to stay short).

```cpp
// Enigma/PressEnigma.h
#pragma once
#include "Enigma.h"
#include "Core/Palette.h"
#include "Inputs/Button.h"
#include "Outputs/Screen.h"

// Example enigma: press the button 3 times to solve it.
class PressEnigma : public Enigma {
public:
    PressEnigma(const Button &pressButton, Screen &pressScreen) : button(pressButton), screen(pressScreen) {}

    void Start(uint32_t) override { presses = 0; }

    EnigmaEvent Update(uint32_t) override {
        if (!button.WasPressed()) {
            return EnigmaEvent::None;
        }
        presses++;
        return presses == PRESSES_TO_SOLVE ? EnigmaEvent::Solved : EnigmaEvent::None;
    }

    void Draw(uint32_t) override { screen.fillRect(0, 0, presses * 8, 8, Palette::GREEN); }

private:
    static constexpr int PRESSES_TO_SOLVE = 3;
    const Button &button;
    Screen &screen;
    int presses = 0;
};

// Config.h: PANEL_COUNT = 3 in block Panels, and at the end of block Pins (before its closing }):
constexpr uint8_t PRESS_BUTTON = 1;
static_assert(!IsReservedPin(PRESS_BUTTON), "PRESS_BUTTON is on a reserved pin");

// main.cpp: #include "Enigma/PressEnigma.h" at the top, then next to the other objects:
PinButton pressButton(Pins::PRESS_BUTTON);
Screen pressScreen(Panels::PANEL_WIDTH, Panels::PANEL_HEIGHT, 2);
PressEnigma press(pressButton, pressScreen);

// main.cpp, setup(): add each "new" line right after the existing line above it.
    output.AddScreen(snakeScreen);
    output.AddScreen(pressScreen);  // new, before output.Begin()
    inputs.Add(rightButton);
    inputs.Add(pressButton);        // new, before inputs.Begin()
    snake.Start(millis());
    press.Start(millis());          // new

// main.cpp, loop(): add these lines between snake.Draw(now); and output.Show();
    if (press.Update(now) == EnigmaEvent::Solved) {
        Serial.println("Press solved!");
    }
    pressScreen.fillScreen(Palette::BLACK);
    press.Draw(now);
```

Reading it: `: public Enigma` = "is an enigma"; `override` = "this enigma's own version of `Start`/`Update`/`Draw`"; `&` = "uses the existing object,
not a copy"; `uint32_t` = a positive whole number (here, the time in ms, left unnamed when unused). Nothing restarts `press` after its win.

## Wiring

A GPIO (or pin) is a numbered leg of the board. Pins take 3.3 V only: never put 5 V on a pin.

- **Panels**: GPIO 3 to 10, 15 to 18 and 46 (`Pins::HUB75` in `Config.h` says which signal goes where). **Never wire anything else
  to them**: it breaks the display. A compile-time check (`static_assert` in `Config.h`) refuses a button on a reserved pin
  (panels, USB 19-20, and 35-37, used by the board's extra memory, the PSRAM): add one line per new pin.
- **Buttons**: each one between its pin and GND. Left: GPIO 41, right: GPIO 42. GPIO 13 is kept for the Start button.
- **Free for new buttons**: GPIO 1, 2, 11, 12, 14, 21, 39, 40, 47. An analog sensor (microphone, light sensor) goes on 1 or 2.
- Also avoid GPIO 0 and 45 (boot), 43-44 (wired to the "COM" port chip), and 38 and 48 (the board's RGB LED, depending on the board version).
- **Power**: the panels get their own 5 V supply; connect its GND to a GND pin of the board. **USB**: plug the port labelled "USB", not "COM".

## Build and upload

### Board2 sound sensor

Connect the left sensor's **AO to GPIO 13** (`Pins::INPUT_LEFT_MICRO_ANALOG`) and the right sensor's
**AO to GPIO 14** (`Pins::INPUT_RIGHT_MICRO_ANALOG`). On both sensors, connect **G to GND** and **+ to 3.3 V**;
leave DO disconnected.
`Microphone` in `Board2/Microphone.h` handles each sensor independently: construct with `(pin)`,
call `Begin()` in setup and `Update()` every loop.
`Detected()` returns the current detection state. Both instances share sensitivity settings but have separate
background estimates and timers.
Build from `firmware/` with `pio run -e board-b`, then upload with `pio run -e board-b -t upload`.
Open the USB serial monitor at 115200 baud (`pio device monitor -b 115200`; select Board2's port if needed).

Board2 samples AO at a target 4 kHz without blocking and computes RMS after removing the mean over 50 ms windows.
At startup, leave the usual background noise present but do not shout for two seconds (detection stays false during calibration).
The threshold is the larger of background times `NOISE_RATIO` (2) and background plus `MIN_MARGIN` (15 RMS ADC counts).
Two consecutive loud windows trigger detection; five windows below a lower release threshold clear it.
The background estimate slowly follows ambient changes when no shout is being detected or confirmed.
Levels are relative RMS values, not decibels or the old instantaneous values around 400.
Lower `NOISE_RATIO`/`MIN_MARGIN` in `Board2/Config.h` if a loud sound
near the microphone does not trigger; raise them if ambient sounds trigger. Defaults require testing at the intended distance.
Test silence, nearby player sounds, a brief clap, and the expected background noise. Two consecutive 50 ms windows
above threshold trigger detection; an isolated loud window does not. Test release after stopping as well.
This detects sustained loud sound, not human voices specifically. A sudden, sustained increase in ambient noise
can trigger and keep detection active; restart in the new ambient noise to recalibrate. Clipping or a shout masked
by background noise cannot be corrected by this detector alone.
The reset button and both microphone detection states are sent to Board1 every 50 ms.

### General build instructions

PlatformIO builds the firmware and uploads it to the board. The project is the `firmware/` folder, with one env (build setup): `cube`.
The first build downloads the libraries (a few minutes). Close the monitor before an upload. Upload stuck: hold BOOT, press RST, release BOOT, retry.
- **VS Code** (PlatformIO extension): PlatformIO icon in the left bar > Project Tasks > cube > General > Upload, then Monitor.
- **CLion** (PlatformIO plugin, with PlatformIO Core installed): PlatformIO tool window > cube > Upload, then Monitor.
- **Terminal** opened in `firmware/`: `pio run -e cube -t upload`, then `pio device monitor` (115200 baud). `pio run -e cube` only builds.

| Serial monitor message | Meaning |
|---|---|
| `Bomb cube - snake` | the board and the panels started |
| `Output failed to start: stopped` | the panels did not start; the `PanelOutput: ...` line just before says why |
| `InputManager: too many sensors, increase MAX_SENSORS` | more than 16 sensors added, the extra ones are ignored: raise `MAX_SENSORS` in `Inputs/InputManager.h` |
| `Output: too many screens, increase MAX_SCREENS` | more than 6 Screens added, the extra ones are ignored: raise `MAX_SCREENS` in `Outputs/Output.h` |

- The `cube` image is for the ESP32-S3 DevKitC N16R8 only (16 MB flash, 8 MB PSRAM); a Super Mini reboots in a loop. The build banner's
  `ESP32-S3-DevKitC-1-N8 (8 MB QD, No PSRAM)` is only the board's default name: `platformio.ini` sets 16 MB flash and the PSRAM.
- Keep the repo in a short folder outside OneDrive (like `F:\C++\cube-bombe`): in a deep folder, Windows path limits break the panel library build.

## Status

- **Written, not wired yet**: `GameManager` (written for older rules: enigmas one after the other, a time penalty per mistake; to be rewritten),
  `Countdown` and the `Timing` values (only `GameManager` uses them), and the Start button (GPIO 13).
- **Next**: one env per board: A (snake, then cables), B (Space Invaders with the voice, global timer, big button), C (light). Per enigma: a Start/Reset
  button, a status LED (off, orange = running, green = solved), and a "solved" wire from boards A and C to board B. Enigma 1 as designed: 3 levels
  (the snake colored like a cable, the apple like a plug), then the cables phase, and a queue of 2 turns so two quick presses give two quarter turns.
