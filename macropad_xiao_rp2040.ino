#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keyboard.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NEO_PWR 11
#define NEO_PIN 12
Adafruit_NeoPixel pixel(1, NEO_PIN, NEO_GRB + NEO_KHZ800);

const uint8_t switchPins[6] = {0, 28, 1, 29, 27, 26};
const uint8_t ENC_PIN_A = 4;
const uint8_t ENC_PIN_B = 3;
const uint8_t ENC_BTN   = 2;

#define NUM_PROFILES 4
const unsigned long SCREEN_TIMEOUT = 5000;
const unsigned long DEBOUNCE_MS    = 25;

#define ACTION_NONE     0
#define ACTION_KEY      1
#define ACTION_CONSUMER 2

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

#define EEPROM_MAGIC_ADDR 160
#define EEPROM_MAGIC_VAL  0xF5

KeyAction config[NUM_PROFILES][6];
KeyAction encConfig[NUM_PROFILES][2];

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
// NeoPixel helpers
// =========================
void setPixel(uint8_t r, uint8_t g, uint8_t b) {
  pixel.setPixelColor(0, pixel.Color(r, g, b));
  pixel.show();
}

void pixelOff() { setPixel(0, 0, 0); }

void pixelForProfile(uint8_t p) {
  switch (p) {
    case 0: setPixel(0,  0,  80); break; // Bleu
    case 1: setPixel(0,  80, 0);  break; // Vert
    case 2: setPixel(0,  60, 60); break; // Cyan
    case 3: setPixel(60, 0,  60); break; // Magenta
  }
}

void pixelKeyPress() {
  setPixel(80, 80, 80);
  delay(60);
  pixelForProfile(currentProfile);
}

void pixelProfileChange(uint8_t newProfile) {
  for (int i = 0; i < 2; i++) {
    setPixel(80, 80, 80);
    delay(80);
    pixelOff();
    delay(60);
  }
  pixelForProfile(newProfile);
}

// =========================
// EEPROM
// =========================
void loadDefaults() {
  for (int p = 0; p < NUM_PROFILES; p++) {
    for (int b = 0; b < 6; b++)
      config[p][b] = {ACTION_NONE, 0, 0, 0, 0};
    encConfig[p][0] = {ACTION_KEY, MOD_NONE, MOD_NONE, 0x52, 0};
    encConfig[p][1] = {ACTION_KEY, MOD_NONE, MOD_NONE, 0x51, 0};
  }
}

void saveToEEPROM() {
  int addr = 0;
  for (int p = 0; p < NUM_PROFILES; p++)
    for (int b = 0; b < 6; b++) {
      EEPROM.update(addr++, config[p][b].type);
      EEPROM.update(addr++, config[p][b].modifier);
      EEPROM.update(addr++, config[p][b].mod2);
      EEPROM.update(addr++, config[p][b].key);
      EEPROM.update(addr++, config[p][b].key2);
    }
  for (int p = 0; p < NUM_PROFILES; p++)
    for (int d = 0; d < 2; d++) {
      EEPROM.update(addr++, encConfig[p][d].type);
      EEPROM.update(addr++, encConfig[p][d].modifier);
      EEPROM.update(addr++, encConfig[p][d].mod2);
      EEPROM.update(addr++, encConfig[p][d].key);
      EEPROM.update(addr++, encConfig[p][d].key2);
    }
  EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VAL);
  EEPROM.commit();
}

void loadFromEEPROM() {
  if (EEPROM.read(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_VAL) {
    loadDefaults();
    saveToEEPROM();
    return;
  }
  int addr = 0;
  for (int p = 0; p < NUM_PROFILES; p++)
    for (int b = 0; b < 6; b++) {
      config[p][b].type     = EEPROM.read(addr++);
      config[p][b].modifier = EEPROM.read(addr++);
      config[p][b].mod2     = EEPROM.read(addr++);
      config[p][b].key      = EEPROM.read(addr++);
      config[p][b].key2     = EEPROM.read(addr++);
    }
  for (int p = 0; p < NUM_PROFILES; p++)
    for (int d = 0; d < 2; d++) {
      encConfig[p][d].type     = EEPROM.read(addr++);
      encConfig[p][d].modifier = EEPROM.read(addr++);
      encConfig[p][d].mod2     = EEPROM.read(addr++);
      encConfig[p][d].key      = EEPROM.read(addr++);
      encConfig[p][d].key2     = EEPROM.read(addr++);
    }
}

// =========================
// Conversion HID -> char
// =========================
char hidToChar(uint8_t hid) {
  if (hid >= 0x04 && hid <= 0x1D) return 'a' + (hid - 0x04);
  if (hid >= 0x1E && hid <= 0x26) return '1' + (hid - 0x1E);
  if (hid == 0x27) return '0';
  switch (hid) {
    case 0x28: return KEY_RETURN;
    case 0x29: return KEY_ESC;
    case 0x2A: return KEY_BACKSPACE;
    case 0x2B: return KEY_TAB;
    case 0x2C: return ' ';
    case 0x2D: return '-';
    case 0x2E: return '=';
    case 0x2F: return '[';
    case 0x30: return ']';
    case 0x31: return '\\';
    case 0x33: return ';';
    case 0x34: return '\'';
    case 0x35: return '`';
    case 0x36: return ',';
    case 0x37: return '.';
    case 0x38: return '/';
    case 0x4C: return KEY_DELETE;
    case 0x4F: return KEY_RIGHT_ARROW;
    case 0x50: return KEY_LEFT_ARROW;
    case 0x51: return KEY_DOWN_ARROW;
    case 0x52: return KEY_UP_ARROW;
    case 0x3A: return KEY_F1;
    case 0x3B: return KEY_F2;
    case 0x3C: return KEY_F3;
    case 0x3D: return KEY_F4;
    case 0x3E: return KEY_F5;
    case 0x3F: return KEY_F6;
    case 0x40: return KEY_F7;
    case 0x41: return KEY_F8;
    case 0x42: return KEY_F9;
    case 0x43: return KEY_F10;
    case 0x44: return KEY_F11;
    case 0x45: return KEY_F12;
    default:   return 0;
  }
}

// =========================
// Envoi des actions
// =========================
void sendAction(KeyAction action) {
  if (action.type == ACTION_NONE) return;

  if (action.type == ACTION_CONSUMER) {
    uint16_t code = ((uint16_t)action.key2 << 8) | action.key;
    Keyboard.consumerPress(code);
    delay(15);
    Keyboard.consumerRelease();
    return;
  }

  if (action.modifier == MOD_CTRL)  Keyboard.press(KEY_LEFT_CTRL);
  if (action.modifier == MOD_SHIFT) Keyboard.press(KEY_LEFT_SHIFT);
  if (action.modifier == MOD_ALT)   Keyboard.press(KEY_LEFT_ALT);
  if (action.modifier == MOD_GUI)   Keyboard.press(KEY_LEFT_GUI);
  if (action.mod2 == MOD_CTRL)      Keyboard.press(KEY_LEFT_CTRL);
  if (action.mod2 == MOD_SHIFT)     Keyboard.press(KEY_LEFT_SHIFT);
  if (action.mod2 == MOD_ALT)       Keyboard.press(KEY_LEFT_ALT);
  if (action.mod2 == MOD_GUI)       Keyboard.press(KEY_LEFT_GUI);
  delay(5);

  char k = hidToChar(action.key);
  if (k != 0) Keyboard.press(k);
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
void handleSwitchPress(uint8_t index) {
  wakeScreen();
  pixelKeyPress();
  sendAction(config[currentProfile][index]);
}

void handleEncoderTurn(int direction) {
  wakeScreen();
  pixelKeyPress();
  sendAction(encConfig[currentProfile][direction > 0 ? 1 : 0]);
}

void updateEncoderButton() {
  bool reading = digitalRead(ENC_BTN);
  if (reading != lastEncBtnReading) { encBtnLastChangeTime = millis(); lastEncBtnReading = reading; }
  if ((millis() - encBtnLastChangeTime) > DEBOUNCE_MS) {
    if (reading != encBtnStableState) {
      encBtnStableState = reading;
      if (encBtnStableState == LOW) {
        uint8_t newProfile = (currentProfile + 1) % NUM_PROFILES;
        pixelProfileChange(newProfile);
        currentProfile = newProfile;
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
// Protocole Serial
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
  EEPROM.begin(256);
  Wire.begin();

  // NeoPixel — activer l'alimentation AVANT pixel.begin()
  pinMode(NEO_PWR, OUTPUT);
  digitalWrite(NEO_PWR, HIGH);
  pixel.begin();
  pixel.setBrightness(80);
  pixelOff();

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
  loadFromEEPROM();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) while(true) {}
  display.clearDisplay();
  display.display();
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  screenAwake = false;

  pixelForProfile(currentProfile);
}

void loop() {
  handleSerialConfig();
  updateEncoderButton();
  updateEncoder();
  updateSwitches();
  updateScreenTimeout();
}
