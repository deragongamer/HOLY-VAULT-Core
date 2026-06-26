#include <Servo.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

// ====================================================================
// HARDWARE PINOUT
// ====================================================================
const uint8_t pin_buzzer = 2;          
const uint8_t pin_ign = 3;             
const uint8_t pin_st = 4;              
const uint8_t pin_servo = 5;           
const uint8_t pin_horn = 6;            
const uint8_t pin_light = 7;           
const uint8_t pin_st_k = 8;            
const uint8_t pin_rem_unlock = 9;      
const uint8_t pin_rem_lock = 10;       
const uint8_t pin_rem_heat = 11;       
const uint8_t pin_rem_start = 12;      
const uint8_t pin_auto_heater_key = 13;
const uint8_t pin_ldr = A0;            
const uint8_t pin_touch_master = A3;    

// منطق رله‌ها (برای ووکوی HIGH روشن است)
#define RELAY_ON HIGH   
#define RELAY_OFF LOW 

Servo throttleServo;
SSD1306AsciiWire oled; 

// ====================================================================
// STATE MACHINES & TIMERS
// ====================================================================
bool isAuthorized = false;
bool isIgnOn = false;
bool isCranking = false;

bool autoCrankActive = false; 
unsigned long autoCrankStartTime = 0;

bool last_stK = LOW; 
unsigned long stK_pressTime = 0; 
bool stK_handled = false;

uint8_t morseStep = 0; 
unsigned long lastMorseTime = 0; 
String morseVisual = "";

bool last_startRem = LOW; 
int lastUiState = -1; 
unsigned long lastUiUpdateTime = 0;

// ====================================================================
// SOUND ENGINE
// ====================================================================
void playSound(uint8_t type) {
  switch(type) {
    case 1: tone(pin_buzzer, 1900, 40); delay(50); tone(pin_buzzer, 2400, 60); break; 
    case 2: tone(pin_buzzer, 1100, 80); delay(100); tone(pin_buzzer, 800, 120); break; 
    case 6: tone(pin_buzzer, 2200, 30); break; 
  }
  noTone(pin_buzzer);
}

// ====================================================================
// UI ENGINE (MINIMALIST & ANIMATED)
// ====================================================================
void drawClosedLock() {
  oled.clear();
  oled.set1X();
  oled.println(F("       .--.       "));
  oled.println(F("       |  |       "));
  oled.println(F("      [====]      "));
  oled.println(F("      [    ]      "));
  oled.println();
  oled.set2X();
  oled.println(F("  LOCKED  "));
}

void playUnlockAnimation() {
  oled.clear();
  oled.set1X();
  oled.println(F("       .--.       "));
  oled.println(F("       |          ")); // دهانه قفل باز شد
  oled.println(F("      [====]      "));
  oled.println(F("      [    ]      "));
  oled.println();
  oled.set2X();
  oled.println(F(" UNLOCKED "));
  delay(700); 
  oled.clear();
}

void showBootScreen() {
  oled.clear();
  oled.set1X();
  oled.setCursor(0, 1);
  oled.println(F("  HOLY VAULT CORE   "));
  oled.println(F("  --- MOTO-OS ---   "));
  oled.println();
  oled.print(F(" L O A D I N G "));
  oled.println();
  oled.print(F("["));
  for(int i=0; i<18; i++) {
      oled.print(F("="));
      delay(70);
  }
  oled.print(F("]"));
  delay(400);
  oled.clear();
}

void updateOLED() {
  // این سیستم جدید به هیچ‌وجه چشمک نمی‌زنه
  if (millis() - lastUiUpdateTime < 50) return;
  lastUiUpdateTime = millis();

  if (!isAuthorized) {
    if (lastUiState != 0) {
      drawClosedLock();
      lastUiState = 0;
    }
    // نمایش زنده مورس در خط پایین
    oled.set1X();
    oled.setCursor(0, 7);
    if (morseStep > 0) {
      oled.print(F("CODE: ")); oled.print(morseVisual); oled.print(F("       "));
    } else {
      oled.print(F("                     "));
    }
  } else {
    // وارد شدن به صفحه خلوت و مینی‌مال
    if (lastUiState != 1) {
      oled.clear();
      lastUiState = 1;
    }

    oled.set1X();
    oled.setCursor(0, 0);
    if (isIgnOn) oled.print(F("ign    ")); else oled.print(F("       "));
    if (isCranking) oled.print(F("start  ")); else oled.print(F("       "));
    
    // نشانگر بوق و چراغ
    oled.setCursor(85, 0); 
    if (digitalRead(pin_light) == RELAY_ON) oled.print(F("[LGT]")); else oled.print(F("     "));
    if (digitalRead(pin_horn) == RELAY_ON)  oled.print(F("[HRN]")); else oled.print(F("     "));

    // خط وسط: فقط اگر IGN خاموش باشه ready نشون میده
    oled.setCursor(0, 3);
    if (isIgnOn) {
      oled.print(F("                     "));
    } else {
      oled.print(F("        ready        ")); 
    }
  }
}

// ====================================================================
// CORE CONTROLS
// ====================================================================
void toggleIgnition(bool state) {
  isIgnOn = state;
  digitalWrite(pin_ign, isIgnOn ? RELAY_ON : RELAY_OFF);
  if (!isIgnOn) { isCranking = false; digitalWrite(pin_st, RELAY_OFF); }
  updateOLED(); 
}

void startCranking() {
  isCranking = true;
  digitalWrite(pin_st, RELAY_ON);
  updateOLED();
}

void stopCranking() {
  if (isCranking) {
    isCranking = false;
    digitalWrite(pin_st, RELAY_OFF);
    updateOLED();
  }
}

// ====================================================================
// SETUP
// ====================================================================
void setup() {
  Serial.begin(9600); 
  Wire.begin(); Wire.setClock(400000L); 
  oled.begin(&Adafruit128x64, 0x3C); oled.setFont(System5x7); 

  pinMode(pin_buzzer, OUTPUT); noTone(pin_buzzer);
  pinMode(pin_ign, OUTPUT);    digitalWrite(pin_ign, RELAY_OFF);
  pinMode(pin_st, OUTPUT);     digitalWrite(pin_st, RELAY_OFF);
  pinMode(pin_horn, OUTPUT);   digitalWrite(pin_horn, RELAY_OFF);
  pinMode(pin_light, OUTPUT);  digitalWrite(pin_light, RELAY_OFF);

  pinMode(pin_st_k, INPUT_PULLUP); 
  pinMode(pin_rem_unlock, INPUT_PULLUP); pinMode(pin_rem_lock, INPUT_PULLUP);
  pinMode(pin_rem_start, INPUT_PULLUP);  pinMode(pin_touch_master, INPUT_PULLUP);

  showBootScreen(); // نمایش لودینگ Moto-OS
  
  lastUiState = -1; 
  updateOLED(); 
}

// ====================================================================
// MAIN LOOP
// ====================================================================
void loop() {
  unsigned long currentMillis = millis();

  // ----------------------------------------------------------------
  // 1. ENGINE: MORSE & IGNITION BUTTON (Real Motorcycle Behavior)
  // ----------------------------------------------------------------
  bool stK = (digitalRead(pin_st_k) == LOW);
  
  if (stK && !last_stK) { // لحظه فشردن
    stK_pressTime = currentMillis; 
    stK_handled = false; 
  }

  // تایم‌اوت مورس
  if (!isAuthorized && morseStep > 0 && (currentMillis - lastMorseTime > 3000)) { 
    morseStep = 0; morseVisual = ""; playSound(2); updateOLED();
  }

  // نگه داشتن دکمه: رفتار ۱۰۰٪ مشابه دکمه استارت واقعی
  if (stK && isAuthorized) {
    if (currentMillis - stK_pressTime >= 400) { 
      stK_handled = true; 
      if (!isIgnOn) toggleIgnition(true); 
      startCranking();
    }
  }

  if (!stK && last_stK) { // لحظه رها کردن
    stopCranking(); // قطع آنی استارت با برداشتن دست
    unsigned long duration = currentMillis - stK_pressTime;
    lastMorseTime = currentMillis;

    if (!isAuthorized) { 
      // سیستم قفل است -> ثبت کد مورس
      if (duration > 50 && duration < 600) { morseStep++; morseVisual += "* "; playSound(6); } 
      else if (duration >= 600 && duration < 3000) { morseStep++; morseVisual += "- "; playSound(6); }
      
      if (morseStep == 6) { 
        isAuthorized = true; morseStep = 0; morseVisual = ""; 
        playSound(1); playUnlockAnimation(); updateOLED();
      }
    } 
    else { 
      // سیستم باز است -> کلیک کوتاه یعنی قطع/وصل کردن برق IGN
      if (!stK_handled && duration > 50 && duration < 400) { 
        toggleIgnition(!isIgnOn); 
      }
    }
  }
  last_stK = stK;

  // ----------------------------------------------------------------
  // 2. ENGINE: SMART REMOTE START (Auto-Crank)
  // ----------------------------------------------------------------
  bool startRem = (digitalRead(pin_rem_start) == LOW);
  
  if (startRem && !last_startRem) { 
    if (!isIgnOn) {
      // یک تک‌کلیک ریموت: آنلاک + باز کردن سوییچ + فعال کردن استارت خودکار
      isAuthorized = true; 
      playSound(1);
      playUnlockAnimation();
      toggleIgnition(true);
      autoCrankActive = true;       
      autoCrankStartTime = currentMillis;
    } else {
      // کلیک مجدد ریموت: خاموش کردن و قفل موتور
      toggleIgnition(false);
      isAuthorized = false;
      playSound(2);
      updateOLED();
    }
  }
  last_startRem = startRem;

  // اجرای استارت اتوماتیک ریموت به مدت ۱.۵ ثانیه
  if (autoCrankActive) {
    startCranking();
    if (currentMillis - autoCrankStartTime > 1500) {
      stopCranking();
      autoCrankActive = false;
    }
  }

  // ----------------------------------------------------------------
  // 3. ENGINE: LOCK / UNLOCK REMOTES
  // ----------------------------------------------------------------
  if (digitalRead(pin_rem_unlock) == LOW && !isAuthorized) { 
    isAuthorized = true; playSound(1); playUnlockAnimation(); updateOLED(); 
  }
  if (digitalRead(pin_rem_lock) == LOW && isAuthorized) { 
    isAuthorized = false; toggleIgnition(false); playSound(2); updateOLED(); 
  }
  
  updateOLED(); // آپدیت مداوم بدون چشمک زدن
}