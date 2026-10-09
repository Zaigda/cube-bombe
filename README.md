# Cube-bombe

The cube-bombe is a bomb that two players defuse together (Enjmin "ALT in&out" workshop). It holds 3 enigmas (puzzles),
played in any order: in each one, a player acts without seeing and the other guides. You cannot lose an enigma and there
is no penalty: only the global timer makes the bomb explode. This repo is the firmware of the cube's 3 ESP32-S3 boards.

| Board | Env (program) | Role |
|---|---|---|
| Board 1 | `board-a` | **The brain.** Runs the whole game and the 3 enigmas, drives the 4 HUB75 panels (64x32 LEDs each), reads the snake buttons and the cables |
| Board 2 | `board-b` | Remote I/O of enigma 2: reads its 2 microphones and its Start/Reset button, lights its status LED |
| Board 3 | `board-c` | Remote I/O of enigma 3: reads its 9 light sensors and its Start/Reset button, lights its 3x3 LED grid and its status LED |

Boards 2 and 3 have no game rules. Every 50 ms they send what they read to board 1 over a serial wire (UART), and board
1 sends back what their LEDs show. A board silent for 500 ms counts as "nothing pressed, LEDs off" on the other side.

| Panel | Screen | Shows |
|---|---|---|
| 0 and 1 | `snakeScreen` (64x64) | Enigma 1: the snake, then "BRANCHE" |
| 2 | `guideScreen` | Enigma 2 for the guide: alien, shots, ship, kill counter |
| 3 | `singerScreen` | Enigma 2 for the singer: the ship only |

## How a game goes

- **Enigma 1, snake + cables.** Two buttons steer the snake: each press is a quarter turn to the snake's own left or
  right (2 quick presses give 2 quarter turns, one per step). One level per cable: the snake has the cable's color, the
  apple the color of the plug that cable goes into. A wall or its own body = "RATE", and the snake restarts at level 1
  with the same colors. After the 3rd apple the screen shows "BRANCHE": plug each cable into its plug.
- **Enigma 2, Space Invaders by voice.** The singer moves the ship with the left or right microphone (both heard: the
  louder wins). The ship fires on its own. Only the guide sees the alien and the kill counter (`N/20`). An alien
  reaching the ship's line = "RATE", and the counter goes back to 0. 20 kills win.
- **Enigma 3, light.** 9 holes, each with a light sensor and an LED. Before its Start, the middle LED blinks. A random
  pattern of 6 holes must be covered. A covered hole of the pattern lights its LED; covering all 9 holes turns every LED
  off. Solved when exactly the pattern is covered (solved: all 9 LEDs on). No screen and no "RATE": only the grid and
  the status LED.

1. **Waiting.** At power-on (after a 2 s self-test, see "Before each game"), the snake and invaders screens show
   "DEPART" pulsing slowly, with the bomb timer under it (5:00, still). All status LEDs are off.
2. **Start.** Each enigma has a Start/Reset button. A press starts it: its game replaces "DEPART", its LED blinks. **The
   first Start of any enigma starts the timer.** The timer only shows under "DEPART" (enigmas not started yet); it turns
   red for the last 30 s. In the last 10 s, the screens of the enigmas not in play ("DEPART" or "DESAMORCE") blink a
   red border, and the 9 grid LEDs blink together if the light enigma is not in play.
3. **Reset one enigma.** A press during play (or during "RATE") restarts that enigma from zero: back to "DEPART", LED
   off. The timer keeps running. A second press starts it again.
4. **Mistake.** "RATE" in big white letters on red for 1 s, then play goes on.
5. **Solved.** A green flash, then "DESAMORCE" in green; LED steady on. Its button does nothing any more.
6. **All 3 solved: VICTOIRE.** On every panel, once: a green flash, rings growing (white, cyan, green), then "VICTOIRE"
   crossing each panel once in big green letters. Then every panel stays on "VICTOIRE" in green with the time that was
   left under it, until the global reset. Every status LED on, one LED running round the grid.
7. **Timer at 0: BOOM.** On every panel, in a loop: a white flash, rings growing (yellow, orange, red), then "BOOM"
   blinking red. Every status LED and the whole grid blink fast.
8. **Frozen.** After VICTOIRE or BOOM, every enigma button is ignored until the global reset.
9. **New game (global reset).** Hold the snake Start/Reset button 8 s. After 2 s the snake screen shows "RESET" and a
   bar filling up to 8 s; letting go earlier cancels it. At 8 s the bar turns green with "PRET": let go. Everything
   goes back to step 1: timer full and stopped, new cable code, new light pattern. It works at any moment. A short
   press (under 2 s) of this button still starts or resets the snake; it counts when the button is released ("DEPART"
   turns green while the button is held). **Backup:** the RST button of board 1 restarts the game too (after its
   self-test).

## Change the game

Paths are relative to `firmware/src/`. Values are grouped in blocks: `SnakeTuning::STEP_MS` is the line
`constexpr uint32_t STEP_MS = 250;` inside `namespace SnakeTuning { ... }`. Search for the last word (`STEP_MS`), change
only the number, keep the `;`. Times are in ms, positions and sizes in LEDs. Then upload to the board(s) in the last
column.

| I want to... | File | Change this | Now | Upload to |
|---|---|---|---|---|
| Change the game length | `Board1/Config.h` | `GameTuning::GAME_DURATION_MS` | 5 min | `board-a` |
| Change when the timer turns red | `Board1/Config.h` | `GameTuning::FINAL_WARNING_MS` | 30 s | `board-a` |
| Change the last-seconds alarm (red border, grid blinking) | `Board1/Config.h` | `GameTuning::FINAL_ALARM_MS` (when it starts), `FINAL_ALARM_BLINK_MS` (keep a multiple of 50), `FINAL_ALARM_BORDER` (border thickness) | 10 s, `250`, `2` | `board-a` |
| Change the global reset | `Board1/Config.h` | `GameTuning::GLOBAL_RESET_HOLD_MS` (hold time), `RESET_FEEDBACK_AFTER_MS` (when "RESET" shows; also the longest short press of the snake button) | `8000`, `2000` | `board-a` |
| Change the "DEPART" pulse | `Board1/Config.h` | `GameTuning::PULSE_MS` (one cycle, also the blink of the grid LED before the light Start), `PULSE_MIN_LEVEL` (darkest, 0-255) | `2000`, `120` | `board-a` |
| Change a text | `Board1/Config.h` | `Texts::START`, `FAILED`, `SOLVED`, `PLUG`, `VICTORY`, `BOOM`, `RESET`, `RESET_DONE`, `BOARD_2`, `BOARD_3`: uppercase, no accents (the font has none), 10 characters at most | "DEPART", "RATE", ... | `board-a` |
| Change "RATE" or the solved flash | `Board1/Config.h` | `StationTuning::FAIL_MS` (how long "RATE" stays), `FAILED_TEXT_SIZE`, `SOLVED_FLASH_MS` (green screen before "DESAMORCE") | `1000`, `2`, `400` | `board-a` |
| Move "DEPART", the timer or the board warning | `Board1/Config.h` | `StationTuning::START_TEXT_TOP`, `TIMER_TEXT_TOP`, `TIMER_TEXT_SIZE` (the final "VICTOIRE" and time left use the same 3), `TIMER_COLOR`, `LINK_CHECK_TOP` ("CARTE 2"), `LINK_CHECK_LINE` (gap to "CARTE 3") | `4`, `14`, `2`, cyan, `36`, `10` | `board-a` |
| Change the BOOM animation (VICTOIRE uses the same flash and ring values) | `Board1/Config.h` | `EndTuning::BOOM_FLASH_MS`, `BOOM_RINGS_MS`, `BOOM_RING_COUNT`, `BOOM_RING_GAP`, `BOOM_TEXT_MS`, `BOOM_BLINK_MS`, `BOOM_TEXT_SIZE` | `150`, `1200`, `3`, `6`, `2400`, `300`, `2` | `board-a` |
| Change the VICTOIRE text | `Board1/Config.h` | `EndTuning::VICTORY_TEXT_SIZE`, `VICTORY_SCROLL_STEP_MS` (time per LED of scrolling), `VICTORY_TIME_COLOR` (the time left, shown after) | `3`, `20`, white | `board-a` |
| Change the VICTOIRE grid chase | `Board1/Config.h` | `EndTuning::CHASE_STEP_MS` (keep a multiple of 50), `CHASE_ORDER` (grid order 0 1 2 / 3 4 5 / 6 7 8) | `150`, round the edge | `board-a` |
| Make the snake faster or slower | `Board1/Config.h` | `SnakeTuning::STEP_MS`. LEDs per second = `CELL_SIZE * 1000 / STEP_MS` | `250` (8 LEDs/s) | `board-a` |
| Change the snake cells | `Board1/Config.h` | `SnakeTuning::CELL_SIZE` (a value that divides 32) | `2` | `board-a` |
| Change the snake length per level | `Board1/Config.h` | `SnakeTuning::LEVEL_LENGTHS` | `16, 32, 48` | `board-a` |
| Change where the snake starts | `Board1/Config.h` | `SnakeTuning::START_X`, `START_Y` (the head; it goes right) | `16`, `48` | `board-a` |
| Change where the first apple appears | `Board1/Config.h` | `SnakeTuning::FIRST_APPLE_MIN_X` ... `FIRST_APPLE_MAX_Y` | x 32-48, y 16-32 | `board-a` |
| Change the "eating" head | `Board1/Config.h` | `SnakeTuning::MOUTH_SIZE`, `MOUTH_MS` | `4`, `250` | `board-a` |
| Change how many turn presses wait | `Board1/Config.h` | `SnakeTuning::TURN_QUEUE_SIZE` | `2` | `board-a` |
| Change cable or plug colors, or the solutions | `Board1/Core/BombCode.cpp` | `CABLE_COLORS`, `PLUG_COLORS`, `SOLUTIONS` (plug of cable 0, 1, 2; at least 2 rows, all different, with `SOLUTION_COUNT`) | 3 solutions | `board-a` |
| Change Space Invaders speeds | `Board1/Config.h` | `InvadersTuning::ALIEN_STEP_MS` (alien fall), `SHIP_STEP_MS`, `FIRE_MS`, `BULLET_STEP_MS` | `500`, `40`, `500`, `20` | `board-a` |
| Change the kills to win | `Board1/Config.h` | `InvadersTuning::ALIEN_COUNT` | `20` | `board-a` |
| Change the kill counter | `Board1/Config.h` | `InvadersTuning::COUNTER_LEFT`, `COUNTER_TOP`, `COUNTER_LEVEL` (gray, 0-255), `HIT_FLASH_MS` (after a hit: counter white, hit alien orange) | `1`, `1`, `180`, `300` | `board-a` |
| Change the light sensitivity | `Board1/Config.h` | `LightTuning::COVERED_PERCENT`: covered below this % of the brightest light the hole had since Start | `50` | `board-a` |
| Change the light patterns | `Board1/Config.h` | `LightTuning::PATTERNS` (1 = hole to cover); a new pattern also raises `PATTERN_COUNT` | 6 patterns | `board-a` |
| Change the grid LED blinking before the light Start | `Board1/Config.h` | `LightTuning::IDLE_LED` (grid order 0 1 2 / 3 4 5 / 6 7 8) | `4` (middle) | `board-a` |
| Smooth the light sensors | `Board3/Config.h` | `LightTuning::SAMPLES` (reads averaged per value) | `4` | `board-c` |
| Change the grid test at power-on | `Board3/Config.h` | `GridTuning::SWEEP_STEP_MS` (time per LED) | `150` | `board-c` |
| Change the microphone sensitivity | `Board2/Config.h` | `SoundTuning::NOISE_RATIO`, `MIN_MARGIN`: lower = more sensitive | `2.0`, `15` | `board-b` |
| Change the voice reaction time | `Board2/Config.h` | `SoundTuning::ATTACK_WINDOWS` (to move), `RELEASE_WINDOWS` (to stop), in steps of 50 ms | `2`, `5` | `board-b` |
| Change when a long sound is ignored | `Board2/Config.h` | `SoundTuning::MAX_HOLD_MS`: after this, the sound is learned as background (2 s deaf) | `4000` | `board-b` |
| Change the LED blinking | `Common/Config.h` | `StatusLight::BLINK_MS` (playing), `FAST_BLINK_MS` (BOOM) | `400`, `150` | all 3 |
| Change the serial links | `Common/Config.h` | `LinkTuning::BAUD`, `SEND_MS`, `TIMEOUT_MS` | `115200`, `50`, `500` | all 3 |
| Change the panel brightness | `Board1/Config.h` | `Panels::BRIGHTNESS` (0-255) | `60` | `board-a` |
| Reduce flicker on camera | `Board1/Config.h` | `Panels::MIN_REFRESH_HZ`: raise it (e.g. `200`); colors get fewer shades | `60` (library default) | `board-a` |
| Fix a panel shown upside down | `Board1/Config.h` | `Panels::ROTATED_180`: one value per panel, 0 to 3; swap the wrong one (the self-test shows it) | `{false, true, false, true}` | `board-a` |
| Change the panel test at power-on | `Board1/Config.h` | `Panels::TEST_TEXT_SIZE` (panel number), `TEST_SQUARE_SIZE` (top-left square) | `3`, `4` | `board-a` |
| Swap the guide and singer panels | `Board1/Config.h` | `Panels::GUIDE_PANEL`, `SINGER_PANEL` | `2`, `3` | `board-a` |
| Fix double or missed button presses | `Common/Inputs/PinButton.h` | `DEBOUNCE_MS` (board 1 buttons) | `30` | `board-a` |
| Add a color | `Board1/Core/Palette.h` | Add `constexpr uint16_t MAGENTA = Rgb(255, 0, 255);`, use `Palette::MAGENTA`; prefer channels at 0 or 255 (the panel darkens middle values), check it on the panel | | `board-a` |

## How the code works

`setup()` runs once at power-on, then `loop()` runs again and again. Each loop of board 1 reads `now = millis()` once,
then: receives what boards 2 and 3 sent; `inputs.Update(now)` reads the buttons and cables; `game.Update(now)` checks
the global reset, updates the 3 stations, starts the timer and ends the game; the snake status LED shows its station's
`GetLedState()`; the stations draw their enigma and texts (before the first Start, `DrawLinkCheck()` adds "CARTE 2" or
"CARTE 3" for a silent board), then `bombDisplay` draws VICTOIRE, BOOM or RESET over them, and `output.Show()` sends
the pictures to the panels; every 50 ms, it sends boards 2 and 3 what their LEDs show (the `GetLedState()` of their
station, and the grid from `GetGridMask()`).

## Files and classes

`firmware/platformio.ini` holds one env per board. The code is in `firmware/src/`: `Board1/`, `Board2/` and `Board3/`
are each board's program, `Common/` is built into all 3.

| File | Class | Role |
|---|---|---|
| `Board1/main.cpp` | | Creates and connects every object, `setup()`, `loop()`, the links (`Serial2` to board 2, `Serial1` to board 3) |
| `Board1/Config.h` | | `GameTuning`, `EndTuning`, `Texts`, `SnakeTuning`, `InvadersTuning`, `LightTuning`, `StationTuning`, `Panels`, `Pins` |
| `Board1/Core/GameManager` | `GameManager` | The bomb rules: timer start, VICTOIRE, BOOM, freeze, global reset |
| `Board1/Core/BombDisplay` | `BombDisplay` | Draws VICTOIRE, the BOOM animation and the RESET bar over the enigmas |
| `Board1/Core/Countdown` | `Countdown` | The bomb timer: `Reset()` fills and stops it, `Start()`, `Stop()` |
| `Board1/Core/BombCode` | `BombCode` | The secret of enigma 1: the plug of each cable, the order of the snake levels |
| `Board1/Core/Palette.h` | | The panel colors |
| `Board1/Enigma/Enigma.h` | `Enigma` | Base of every enigma: `Start()` resets it, `Update()` returns `None`, `Mistake` or `Solved`, `Draw()` |
| `Board1/Enigma/EnigmaStation` | `EnigmaStation` | One enigma with its Start/Reset button, LED state (`GetLedState()`) and texts (DEPART + timer, RATE, DESAMORCE) |
| `Board1/Enigma/SnakeEnigma`, `CableEnigma`, `SnakeCablesEnigma` | `SnakeEnigma`, `CableEnigma`, `SnakeCablesEnigma` | Enigma 1: the snake, the cables, and both in a row |
| `Board1/Enigma/InvadersEnigma` | `InvadersEnigma` | Enigma 2, Space Invaders |
| `Board1/Enigma/LightEnigma` | `LightEnigma` | Enigma 3, the light holes; computes the grid LEDs |
| `Board1/Outputs/` | `Screen`, `PanelOutput` | A `Screen` is a picture in memory (Adafruit GFX) covering whole panels; `DrawCenteredText()` without a top writes the text once per panel, never across two. `PanelOutput` shows the Screens on the panels; refuses a Screen of the wrong size or on a used panel |
| `Board2/main.cpp`, `Microphone.h`, `Config.h` | `Microphone` | Board 2: detects a loud sound on each microphone, sends it with the button |
| `Board3/main.cpp`, `Config.h` | | Board 3: sends the light values and the button, lights the grid and the status LED |
| `Common/Config.h` | | `LinkTuning`, `StatusLight`, `PinCheck` (the pin checks) |
| `Common/Inputs/` | `Sensor`, `Button`, `PinButton`, `RemoteButton`, `CablePanel`, `InputManager` | A `PinButton` given a max press time (the snake Start/Reset) only counts short presses; a `RemoteButton` is on board 2 or 3 |
| `Common/Outputs/` | `StatusLed`, `LedState` | `LedState` (off, blinking, on, fast blinking) is what a status LED shows, also sent to boards 2 and 3 |
| `Common/Messages/` | `Enigma2...Message`, `Enigma3...Message`, `MessageReader` | The messages between the boards; `MessageLink` sends them, `MessageReader` reads them, empties them after 500 ms of silence and tells whether the board is silent (`IsLinked()`) |

**Add an enigma:** derive a class from `Enigma` in `Board1/Enigma/`; in `Board1/main.cpp`, create it with its button
and `EnigmaStation`, then in `setup()` call `inputs.Add()`, `station.AddScreen()` and `game.AddStation()`. Its status
LED shows `station.GetLedState()`: a `StatusLed` on board 1 (like `snakeLed` in `loop()`), or a field of a message.

## Wiring

A GPIO (pin) is a numbered leg of the board. Pins take 3.3 V only: never 5 V. **Buttons** go between their pin and GND
(internal pull-up). **LEDs** go from their pin, through a 220 ohm resistor, to GND. To move a wire, change its number in
`Pins` of that board's `Config.h` (a new pin also goes in `USED_PINS`): the build refuses a pin used twice, a reserved
one (USB 19-20, flash and PSRAM 26-37, COM port 43-44) or, on board 1, a panel pin.

**Board 1** (`board-a`)

| What | GPIO |
|---|---|
| Panel 0 input (HUB75): R1 G1 B1 / R2 G2 B2 | 4 5 6 / 7 15 16 |
| Panel 0 input: A B C D / LAT OE CLK (E not wired) | 17 18 8 3 / 46 10 9 |
| Snake turn left / right (button to GND) | 41 / 42 |
| Snake Start/Reset, and global reset held 8 s (button to GND) | 40 |
| Snake status LED | 39 |
| Cables 0 / 1 / 2 (red / green / blue on the snake screen) | 11 / 12 / 13 |
| Plugs 0 / 1 / 2 (green / blue / yellow on the snake screen) | 48 / 47 / 45 |
| Link to board 2: TX / RX | 21 / 1 |
| Link to board 3: TX / RX | 14 / 2 |

**Never wire anything else to the panel pins**: it breaks the display. Panel chain: board 1 -> panel 0 (top of the
snake) -> panel 1 (bottom of the snake, upside down) -> panel 2 (guide) -> panel 3 (singer, upside down). A cable joins
its cable pin to a plug pin.

**Board 2** (`board-b`)

| What | GPIO |
|---|---|
| Left / right microphone AO (on both: G to GND, + to 3.3 V, DO not connected) | 13 / 14 |
| Invaders Start/Reset (button to GND) | 4 |
| Invaders status LED | 12 |
| Link to board 1: TX / RX | 1 / 21 |

**Board 3** (`board-c`). Each hole: a photoresistor from 3.3 V to its sensor pin, a 10 kohm resistor from that pin to
GND (more light must give a higher value), and an LED on its LED pin. Start/Reset: GPIO 39 (button to GND). Status LED:
GPIO 9. Link to board 1: TX 21 / RX 13.

| Hole: sensor / LED GPIO | Left | Middle | Right |
|---|---|---|---|
| Top | 0: 18 / 42 | 1: 16 / 2 | 2: 8 / 1 |
| Middle | 3: 5 / 41 | 4: 15 / 40 | 5: 6 / 47 |
| Bottom | 6: 7 / 12 | 7: 4 / 11 | 8: 17 / 10 |

**Serial links** (a TX always goes to the other board's RX):

| Wire | From | To |
|---|---|---|
| Board 1 to board 2 | board 1 GPIO 21 (TX) | board 2 GPIO 21 (RX) |
| Board 2 to board 1 | board 2 GPIO 1 (TX) | board 1 GPIO 1 (RX) |
| Board 1 to board 3 | board 1 GPIO 14 (TX) | board 3 GPIO 13 (RX) |
| Board 3 to board 1 | board 3 GPIO 21 (TX) | board 1 GPIO 2 (RX) |
| Common ground | a GND of board 1 | a GND of board 2 and a GND of board 3 |

**Power:** the panels get their own 5 V supply; connect its GND to a GND of board 1. **USB:** plug the port labelled
"USB", not "COM" (the monitor only works there).

## Build and upload

The project is the `firmware/` folder (CLion or VS Code with PlatformIO, or a terminal). The first build downloads the
libraries (a few minutes). Close the monitor before an upload. Upload stuck: hold BOOT, press RST, release BOOT, retry.

- **Always pick the env and the port:** `pio run -e board-a -t upload --upload-port COM5`, then
  `pio device monitor -b 115200 -p COM5`. `pio device list` shows the ports; plug one board at a time to tell them
  apart.
- **Never upload without an env:** `pio run -t upload` (or the project-wide Upload in the IDE) uploads the 3 programs
  one after the other to the same board, and the last one (`board-c`) stays. `pio run` alone only builds the 3: fine.
- **After a change in `Common/`** (messages, LED states, links), upload all 3 boards: they must share the same messages.
- The images are for the ESP32-S3 DevKitC N16R8 only (16 MB flash, 8 MB PSRAM); a Super Mini reboots in a loop.
- Keep the repo in a short folder outside OneDrive (like `F:\C++\cube-bombe`): long paths break the panel library build.

| Monitor line | Meaning |
|---|---|
| `Bomb cube - board 1` / `ESP32 - Board B - Initialized.` / `Bomb cube - board 3 (light sensors)` | Which program the board runs |
| `Panels failed to start: ...` | Board 1: the panels did not start (the snake status LED blinks fast); the end says why |
| `Game: waiting`, `running`, `defused`, `exploded`, `global reset` | The game changed state |
| `Snake: start`, `reset`, `mistake`, `solved!` (same for `Invaders`, `Light`) | A station changed state |
| `Code: levels ..., plugs ...` / `Snake done: plug the cables` / `Cable 0 -> plug 2` | Enigma 1: new secret, cables step, a cable moved (`-1` = unplugged) |
| `Alien hit: 5 killed` / `Alien landed: kill counter back to 0` | Enigma 2 |
| `Light: pattern ...` | Enigma 3: the holes to cover, drawn with `X` |

## Before each game (staff)

1. The cables can stay where the last group left them: a new code (at power-on, RST or reset) never matches the cables
   plugged at that moment.
2. The light holes can stay as they are: a hole covered at Start counts as open until it has seen light once.
3. Hold the snake Start/Reset 8 s (or press RST on board 1). Check: "DEPART" and 5:00 on panels 0, 2 and 3, the middle
   grid LED blinks, status LEDs off, and nothing red on panel 1 (CARTE 2 / CARTE 3 = that board is silent: wire, GND or
   upload).

   **Self-tests at power-on** (or after the RST of that board): for 2 s, each panel shows its number big (0 and 1 = top
   and bottom of the snake, 2 = guide, 3 = singer) with a small white square in its top-left corner. A wrong number =
   the ribbons are in the wrong order; the square bottom-right (the number upside down) = that panel's `ROTATED_180` is
   wrong; a black panel = its ribbon or its 5 V. If the snake status LED blinks fast instead, the panels did not start
   (monitor: `Panels failed to start`). Board 3 lights its 9 grid LEDs one at a time, 0 to 8 (top-left to bottom-right,
   row by row): an LED out of order = two LED wires swapped; one that stays dark = that LED or its wire.
4. Board 2 learns the room's noise in its first 2 s after power-on: no shouting near the microphones then.
5. If the 5-minute music plays next to the cube, start it on the players' first Start.

## To check on the cube

- The BOOM white flash lights the 4 panels at once: check the 5 V supply holds (set `GAME_DURATION_MS` to 20 s to test).
- The microphones with the music playing: tune `NOISE_RATIO`, `MIN_MARGIN` and `MAX_HOLD_MS`.
- The light: if the room light changes a lot, press the light Start/Reset twice (reset, then start) to measure again (it
  also draws a new pattern).
