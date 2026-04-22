#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keyboard.h>
#include <ConsumerKeyboard.h>
#include <EEPROM.h>

// =========================
// OLED
// =========================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =========================
// Pins XIAO RP2040
// =========================
const uint8_t switchPins[6] = {0, 28, 1, 29, 27, 26};
const uint8_t ENC_PIN_A = 4;   // CLK
const uint8_t ENC_PIN_B = 3;   // DT
const uint8_t ENC_BTN   = 2;   // Bouton encodeur
// OLED : SDA=6, SCL=7 (Wire par défaut sur XIAO RP2040)

// =========================
// Parametres
// =========================
#define NUM_PROFILES 4
const unsigned long SCREEN_TIMEOUT = 5000;
const unsigned long DEBOUNCE_MS    = 25;

// =========================
// Types d'action
// =========================
#define ACTION_NONE     0
#define ACTION_KEY      1
#define ACTION_CONSUMER 2

// Modifiers — même valeurs USB HID standard que Pro Micro
#define MOD_NONE  0x00
#define MOD_CTRL  0xE0
#define MOD_SHIFT 0xE1
#define MOD_ALT   0xE2
#define MOD_GUI   0xE3

struct KeyAction {
  uint8_t type;
  uint8_t modifier;
  uint8_t mod2;
  uint8_t key;
  uint8_t key2;
};

// =========================
// EEPROM Layout — identique Pro Micro
// 4 profils × 8 actions × 5 octets = 160 octets
// Magic : addr 160
// =========================
#define EEPROM_MAGIC_ADDR 160
#define EEPROM_MAGIC_VAL  0xF2

KeyAction config[NUM_PROFILES][6];
KeyAction encConfig[NUM_PROFILES][2];

void loadDefaults() {
  for (int p = 0; p < NUM_PROFILES; p++) {
    for (int b = 0; b < 6; b++)
      config[p][b] = {ACTION_NONE, 0, 0, 0, 0};
    encConfig[p][0] = {ACTION_KEY, MOD_NONE, MOD_NONE, 0x52, 0}; // ArrowUp
    encConfig[p][1] = {ACTION_KEY, MOD_NONE, MOD_NONE, 0x51, 0}; // ArrowDown
  }
}

void saveToEEPROM() {
  int addr = 0;
  for (int p = 0; p < NUM_PROFILES; p++) {
    for (int b = 0; b < 6; b++) {
      EEPROM.update(addr++, config[p][b].type);
      EEPROM.update(addr++, config[p][b].modifier);
      EEPROM.update(addr++, config[p][b].mod2);
      EEPROM.update(addr++, config[p][b].key);
      EEPROM.update(addr++, config[p][b].key2);
    }
  }
  for (int p = 0; p < NUM_PROFILES; p++) {
    for (int d = 0; d < 2; d++) {
      EEPROM.update(addr++, encConfig[p][d].type);
      EEPROM.update(addr++, encConfig[p][d].modifier);
      EEPROM.update(addr++, encConfig[p][d].mod2);
      EEPROM.update(addr++, encConfig[p][d].key);
      EEPROM.update(addr++, encConfig[p][d].key2);
    }
  }
  EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VAL);
  EEPROM.commit(); // Nécessaire sur RP2040
}

void loadFromEEPROM() {
  if (EEPROM.read(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_VAL) {
    loadDefaults();
    saveToEEPROM();
    return;
  }
  int addr = 0;
  for (int p = 0; p < NUM_PROFILES; p++) {
    for (int b = 0; b < 6; b++) {
      config[p][b].type     = EEPROM.read(addr++);
      config[p][b].modifier = EEPROM.read(addr++);
      config[p][b].mod2     = EEPROM.read(addr++);
      config[p][b].key      = EEPROM.read(addr++);
      config[p][b].key2     = EEPROM.read(addr++);
    }
  }
  for (int p = 0; p < NUM_PROFILES; p++) {
    for (int d = 0; d < 2; d++) {
      encConfig[p][d].type     = EEPROM.read(addr++);
      encConfig[p][d].modifier = EEPROM.read(addr++);
      encConfig[p][d].mod2     = EEPROM.read(addr++);
      encConfig[p][d].key      = EEPROM.read(addr++);
      encConfig[p][d].key2     = EEPROM.read(addr++);
    }
  }
}

// =========================
// Etat global
// =========================
uint8_t currentProfile = 0;
bool screenAwake = false;
unsigned long lastInteractionTime = 0;

bool lastEncBtnReading = HIGH;
bool encBtnStableState = HIGH;
unsigned long encBtnLastChangeTime = 0;

bool lastSwitchReading[6];
bool switchStableState[6];
unsigned long switchLastChangeTime[6];

int lastEncAState = HIGH;

// =========================
// Envoi des actions
// Sur XIAO RP2040 avec Seeed core, on utilise Keyboard (TinyUSB)
// et ConsumerKeyboard pour les media keys
// =========================
void sendAction(KeyAction action) {
  if (action.type == ACTION_NONE) return;

  if (action.type == ACTION_CONSUMER) {
    uint16_t code = ((uint16_t)action.key2 << 8) | action.key;
    ConsumerKeyboard.write(code);
    return;
  }

  // Clavier — même logique que Pro Micro
  if (action.modifier != MOD_NONE) Keyboard.press((KeyboardKeycode)action.modifier);
  if (action.mod2     != MOD_NONE) Keyboard.press((KeyboardKeycode)action.mod2);
  delay(5);
  Keyboard.press((KeyboardKeycode)action.key);
  delay(15);
  Keyboard.releaseAll();
}

// =========================
// OLED
// =========================
void drawThickRect(int x, int y, int w, int h, int t = 2) {
  for (int i = 0; i < t; i++)
    display.drawRect(x+i, y+i, w-2*i, h-2*i, SSD1306_WHITE);
}

void drawPageIndicators(uint8_t activePage) {
  const int sz = 8, gap = 10;
  const int totalW = NUM_PROFILES * sz + (NUM_PROFILES - 1) * gap;
  const int startX = (SCREEN_WIDTH - totalW) / 2;
  for (int i = 0; i < NUM_PROFILES; i++) {
    int x = startX + i * (sz + gap);
    if (i == activePage) display.fillRect(x, 50, sz, sz, SSD1306_WHITE);
    else drawThickRect(x, 50, sz, sz, 2);
  }
}

void drawCenteredBigNumber(char c) {
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  char txt[2] = {c, '\0'};
  int16_t x1, y1; uint16_t tw, th;
  display.getTextBounds(txt, 0, 0, &x1, &y1, &tw, &th);
  display.setCursor((SCREEN_WIDTH - tw) / 2, 10);
  display.print(txt);
}

void drawProfileScreen() {
  if (!screenAwake) return;
  display.clearDisplay();
  drawCenteredBigNumber('1' + currentProfile);
  drawPageIndicators(currentProfile);
  display.display();
}

void wakeScreen() {
  if (!screenAwake) { display.ssd1306_command(SSD1306_DISPLAYON); screenAwake = true; }
  lastInteractionTime = millis();
  drawProfileScreen();
}

void sleepScreen() {
  if (screenAwake) {
    display.clearDisplay(); display.display();
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    screenAwake = false;
  }
}

// =========================
// Handlers
// =========================
void handleSwitchPress(uint8_t index) { wakeScreen(); sendAction(config[currentProfile][index]); }

void handleEncoderTurn(int direction) {
  wakeScreen();
  sendAction(encConfig[currentProfile][direction > 0 ? 1 : 0]);
}

void updateEncoderButton() {
  bool reading = digitalRead(ENC_BTN);
  if (reading != lastEncBtnReading) { encBtnLastChangeTime = millis(); lastEncBtnReading = reading; }
  if ((millis() - encBtnLastChangeTime) > DEBOUNCE_MS) {
    if (reading != encBtnStableState) {
      encBtnStableState = reading;
      if (encBtnStableState == LOW) {
        currentProfile = (currentProfile + 1) % NUM_PROFILES;
        wakeScreen();
      }
    }
  }
}

void updateSwitches() {
  for (uint8_t i = 0; i < 6; i++) {
    bool reading = digitalRead(switchPins[i]);
    if (reading != lastSwitchReading[i]) { switchLastChangeTime[i] = millis(); lastSwitchReading[i] = reading; }
    if ((millis() - switchLastChangeTime[i]) > DEBOUNCE_MS) {
      if (reading != switchStableState[i]) {
        switchStableState[i] = reading;
        if (switchStableState[i] == LOW) handleSwitchPress(i);
      }
    }
  }
}

void updateEncoder() {
  int currentA = digitalRead(ENC_PIN_A);
  if (currentA != lastEncAState) {
    if (currentA == LOW)
      handleEncoderTurn(digitalRead(ENC_PIN_B) != currentA ? -1 : +1);
  }
  lastEncAState = currentA;
}

// =========================
// Protocole Serial — identique Pro Micro
// Format : "CFG P B T M1 M2 K1 K2\n"
// =========================
void handleSerialConfig() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (!line.startsWith("CFG ")) return;

  int p, b, t, m1, m2, k1, k2;
  if (sscanf(line.c_str(), "CFG %d %d %d %d %d %d %d", &p, &b, &t, &m1, &m2, &k1, &k2) != 7) {
    Serial.println("ERR"); return;
  }
  if (p < 0 || p >= NUM_PROFILES || t < 0 || t > 2) { Serial.println("ERR"); return; }

  KeyAction action = {(uint8_t)t, (uint8_t)m1, (uint8_t)m2, (uint8_t)k1, (uint8_t)k2};

  if      (b >= 0 && b <= 5) config[p][b]   = action;
  else if (b == 10)          encConfig[p][0] = action;
  else if (b == 11)          encConfig[p][1] = action;
  else { Serial.println("ERR"); return; }

  saveToEEPROM();
  Serial.println("OK");
}

void updateScreenTimeout() {
  if (screenAwake && (millis() - lastInteractionTime > SCREEN_TIMEOUT)) sleepScreen();
}

// =========================
// Setup / Loop
// =========================
void setup() {
  // Init EEPROM RP2040 (taille à réserver)
  EEPROM.begin(256);

  for (uint8_t i = 0; i < 6; i++) {
    pinMode(switchPins[i], INPUT_PULLUP);
    lastSwitchReading[i] = switchStableState[i] = digitalRead(switchPins[i]);
    switchLastChangeTime[i] = 0;
  }
  pinMode(ENC_PIN_A, INPUT_PULLUP);
  pinMode(ENC_PIN_B, INPUT_PULLUP);
  pinMode(ENC_BTN,   INPUT_PULLUP);
  lastEncAState     = digitalRead(ENC_PIN_A);
  lastEncBtnReading = encBtnStableState = digitalRead(ENC_BTN);

  Serial.begin(9600);
  Keyboard.begin();
  ConsumerKeyboard.begin();
  loadFromEEPROM();

  // I2C sur GPIO 6 (SDA) et 7 (SCL)
  Wire.setSDA(6);
  Wire.setSCL(7);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) while(true) {}
  display.clearDisplay();
  display.display();
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  screenAwake = false;
}

void loop() {
  handleSerialConfig();
  updateEncoderButton();
  updateEncoder();
  updateSwitches();
  updateScreenTimeout();
}
