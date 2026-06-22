#include <Servo.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <avr/wdt.h> // کتابخانه سخت‌افزاری سگ نگهبان برای ری‌استارت خودکار

// ====================================================================
// HARDWARE PINOUT (HOLY VAULT CORE STANDARD)
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
const uint8_t pin_auto_light_key = A1; 
const uint8_t pin_learn_key = A2;       
const uint8_t pin_touch_master = A3;    
const uint8_t pin_ntc = A6;             
const uint8_t pin_vbat = A7;            

#define RELAY_ON LOW   
#define RELAY_OFF HIGH 

Servo throttleServo;
SSD1306AsciiWire oled; 

// ====================================================================
// STORAGE & DATABASE CONFIG
// ====================================================================
char masterTag[11] = "";
char userTags[4][11]; 
bool hasMaster = false;
bool adminMode = false;
bool valetMode = false;                

const int EEPROM_ADDR_MASTER = 0;
const int EEPROM_ADDR_USERS = 12;

int servoMin = 0; 
int servoMax = 180; 
int servoHeatPos = 60;  
int ldrThreshold = 512; 
int ldrPolarity = 1;

// ====================================================================
// STATE MACHINES & CORE TIMERS
// ====================================================================
bool isAuthorized = false;
bool isIgnOn = false;
bool isCranking = false;
bool autoHeatingActive = false;
bool ecoModeActive = false;            
bool updateModeActive = false; // پرچم وضعیت آپدیت سیستم

unsigned long crankStartTime = 0;
bool crankLockout = false;
unsigned long lockoutStartTime = 0;
unsigned long autoHeatStartTime = 0;
unsigned long lastTelemetryCheck = 0;

unsigned long masterFirstSeen = 0;
unsigned long lastMasterSeen = 0;
bool masterIsHeld = false;

unsigned long lastActivityTime = 0;      
unsigned long lightOnStartTime = 0;     
bool welcomeLightActive = false;
bool followMeLightActive = false;
unsigned long valetEngineStartTime = 0; 

char serialBuffer[35]; 
uint8_t serialIndex = 0;

bool last_stK = LOW; 
unsigned long stK_pressTime = 0; 
bool stK_handled = false;
uint8_t morseStep = 0; 
unsigned long lastMorseTime = 0; 

bool last_startRem = LOW; unsigned long startRem_pressTime = 0; bool startRem_handled = false;
bool last_heatRem = LOW; unsigned long heatRem_pressTime = 0; bool heatRem_handled = false;

// ====================================================================
// BRUTAL SOUND ENGINE
// ====================================================================
void playSound(uint8_t type) {
  if (ecoModeActive) return; 
  switch(type) {
    case 1: 
      tone(pin_buzzer, 1900, 40); delay(50); tone(pin_buzzer, 2400, 60); delay(70); break;
    case 2: 
      tone(pin_buzzer, 1100, 80); delay(100); tone(pin_buzzer, 800, 120); delay(130); break;
    case 3: 
      for(int i = 0; i < 3; i++) { tone(pin_buzzer, 300, 130); delay(150); } break;
    case 4: 
      tone(pin_buzzer, 1400, 40); delay(50); tone(pin_buzzer, 1800, 40); delay(50); tone(pin_buzzer, 2500, 100); delay(110); break;
    case 5: 
      tone(pin_buzzer, 2500, 40); delay(50); tone(pin_buzzer, 1800, 40); delay(50); tone(pin_buzzer, 1400, 100); delay(110); break;
    case 6: 
      tone(pin_buzzer, 2200, 50); delay(60); break;
    case 7: 
      tone(pin_buzzer, 800, 90);  delay(110); tone(pin_buzzer, 600, 140); delay(150); break;
  }
  noTone(pin_buzzer);
}

// ====================================================================
// ANALOG TELEMETRY CORE
// ====================================================================
float getBatteryVoltage() {
  int raw = analogRead(pin_vbat);
  return (raw * 5.0 / 1023.0) * 3.127; 
}

float getEngineTemp() {
  int raw = analogRead(pin_ntc);
  if (raw == 0) return 0;
  float resistance = 10000.0 * (1023.0 / (float)raw - 1.0);
  float temp = 1.0 / (1.0 / 298.15 + (1.0 / 3950.0) * log(resistance / 10000.0)) - 273.15;
  return temp;
}

void triggerWelcomeLights() {
  welcomeLightActive = true;
  followMeLightActive = false;
  lightOnStartTime = millis();
  digitalWrite(pin_light, RELAY_ON);
}

void triggerFollowMeLights() {
  followMeLightActive = true;
  welcomeLightActive = false;
  lightOnStartTime = millis();
  digitalWrite(pin_light, RELAY_ON);
}

void wakeUpSystem() {
  if (ecoModeActive) {
    ecoModeActive = false;
    lastActivityTime = millis();
    Serial.println("ECO:0"); 
    oled.begin(&Adafruit128x64, 0x3C);
    oled.setFont(System5x7);
    playSound(6);
  }
  lastActivityTime = millis();
}

// ====================================================================
// KERNEL BOOT SEQUENCE
// ====================================================================
void setup() {
  wdt_disable(); // در ابتدای بوت سگ نگهبان را موقتاً خاموش می‌کنیم تا لوپ ریست رخ ندهد
  
  Serial.begin(9600); 
  Wire.begin();
  Wire.setClock(400000L); 

  oled.begin(&Adafruit128x64, 0x3C);
  oled.setFont(System5x7);
  oled.clear();
  
  oled.println("======================");
  oled.println("   >> HOLY VAULT <<   ");
  oled.println("  MOTO-OS KERNEL LOAD ");
  oled.println("======================");

  pinMode(pin_buzzer, OUTPUT); noTone(pin_buzzer);
  pinMode(pin_ign, OUTPUT);    digitalWrite(pin_ign, RELAY_OFF);
  pinMode(pin_st, OUTPUT);     digitalWrite(pin_st, RELAY_OFF);
  pinMode(pin_horn, OUTPUT);   digitalWrite(pin_horn, RELAY_OFF);
  pinMode(pin_light, OUTPUT);  digitalWrite(pin_light, RELAY_OFF);

  pinMode(pin_st_k, INPUT); pinMode(pin_rem_unlock, INPUT); pinMode(pin_rem_lock, INPUT);
  pinMode(pin_rem_heat, INPUT); pinMode(pin_rem_start, INPUT); pinMode(pin_auto_heater_key, INPUT);
  pinMode(pin_auto_light_key, INPUT); pinMode(pin_learn_key, INPUT); pinMode(pin_touch_master, INPUT);

  throttleServo.attach(pin_servo);
  throttleServo.write(servoMin);

  EEPROM.get(EEPROM_ADDR_MASTER, masterTag);
  if(masterTag[0] != 0 && masterTag[0] != 0xFF) hasMaster = true;
  EEPROM.get(EEPROM_ADDR_USERS, userTags);
  
  lastActivityTime = millis();
  delay(800);
  oled.clear();

  wdt_enable(WDTO_4S); // فعال‌سازی سگ نگهبان با مهلت ۴ ثانیه‌ای
}

// ====================================================================
// REAL-TIME OPERATIONS LOOP
// ====================================================================
void loop() {
  wdt_reset(); // غذا دادن به سگ نگهبان (پت کردن) جهت اعلام زنده بودن سیستم
  
  // اگر در حال آپدیت سخت‌افزاری هستیم، لوپ اصلی را قفل کن
  if (updateModeActive) {
    oled.home();
    oled.println("!!!!!!!!!!!!!!!!!!!!!!");
    oled.println("  SYS UPDATE ACTIVE   ");
    oled.println("  DO NOT POWER OFF   ");
    oled.println("!!!!!!!!!!!!!!!!!!!!!!");
    delay(100);
    return; 
  }

  unsigned long currentMillis = millis();
  float currentVbat = getBatteryVoltage();
  float currentTemp = getEngineTemp();

  // سیستم حفظ باطری بعد از ۲۸ ساعت قفل ممتد
  if (!isIgnOn && !isAuthorized && !ecoModeActive) {
    if (currentMillis - lastActivityTime >= 100800000UL) { 
      ecoModeActive = true;
      oled.clear(); 
      Serial.println("ECO:1"); 
    }
  }

  // اتوماسیون چراغ‌ها
  if (welcomeLightActive) {
    if (currentMillis - lightOnStartTime >= 15000UL) { 
      welcomeLightActive = false;
      if (digitalRead(pin_auto_light_key) == LOW) digitalWrite(pin_light, RELAY_OFF);
    }
  }
  if (followMeLightActive) {
    if (currentMillis - lightOnStartTime >= 30000UL) { 
      followMeLightActive = false;
      if (digitalRead(pin_auto_light_key) == LOW) digitalWrite(pin_light, RELAY_OFF);
    }
  }

  // محافظ ۱۰ دقیقه‌ای مد تعمیرگاه
  if (valetMode && isIgnOn) {
    if (valetEngineStartTime == 0) valetEngineStartTime = currentMillis;
    if (currentMillis - valetEngineStartTime >= 600000UL) { 
      toggleIgnition(false);
      valetEngineStartTime = 0;
      playSound(3);
    }
  } else {
    valetEngineStartTime = 0;
  }

  // قطع‌کن خودکار استارت
  if (isCranking) {
    if (currentVbat > 13.2) stopCranking(); 
    if (currentMillis - crankStartTime > 7000) { 
      stopCranking(); crankLockout = true; lockoutStartTime = currentMillis;
    }
  }
  if (crankLockout && (currentMillis - lockoutStartTime > 5000)) crankLockout = false;

  // پردازش پورت سریال (گیرنده دستورات ماژول وای‌فای و آردوینو آپدیتر)
  while (Serial.available() > 0) {
    wakeUpSystem();
    char c = Serial.read();
    
    if (c == 0x02) { 
      char tagBuf[12];
      int bytesRead = Serial.readBytesUntil(0x03, tagBuf, 11);
      if (bytesRead >= 10 && !valetMode) { 
        tagBuf[10] = '\0';
        
        if (digitalRead(pin_touch_master) == HIGH) {
          strcpy(masterTag, tagBuf); EEPROM.put(EEPROM_ADDR_MASTER, masterTag);
          hasMaster = true; playSound(4); break;
        }

        if (hasMaster && strcmp(tagBuf, masterTag) == 0) {
          lastMasterSeen = currentMillis;
          if (!masterIsHeld) { masterFirstSeen = currentMillis; masterIsHeld = true; }
          if (currentMillis - masterFirstSeen >= 10000UL) {
            memset(masterTag, 0, sizeof(masterTag)); EEPROM.put(EEPROM_ADDR_MASTER, masterTag);
            hasMaster = false; adminMode = false; masterIsHeld = false; playSound(3);
          }
          break; 
        }

        if (adminMode) {
          int matchIndex = -1;
          for (int i=0; i<4; i++) { if (strcmp(userTags[i], tagBuf) == 0) { matchIndex = i; break; } }
          if (matchIndex != -1) { 
            memset(userTags[matchIndex], 0, 11); EEPROM.put(EEPROM_ADDR_USERS, userTags); playSound(7);
          } else { 
            for (int i=0; i<4; i++) {
              if (userTags[i][0] == '\0' || userTags[i][0] == 0xFF) {
                strcpy(userTags[i], tagBuf); EEPROM.put(EEPROM_ADDR_USERS, userTags); playSound(6); break;
              }
            }
          }
        } else {
          for (int i=0; i<4; i++) {
            if (strcmp(userTags[i], tagBuf) == 0) {
              isAuthorized = true; triggerWelcomeLights(); playSound(1); break;
            }
          }
        }
      }
    } 
    else if (c == '\n' || c == '\r') {
      if (serialIndex > 0) {
        serialBuffer[serialIndex] = '\0';
        
        // فرمان شروع خط آپدیت از آردوینو دوم
        if(strcmp(serialBuffer, "SYS_UPDATE_START") == 0) {
          updateModeActive = true;
          toggleIgnition(false);
          Serial.println("READY_TO_RECEIVE");
        }
        // پکت دیتای شبیه‌سازی فریم‌ور جدید برای بازنویسی تنظیمات پایه‌ای EEPROM
        else if(strncmp(serialBuffer, "UP_DATA:", 8) == 0) {
          // فرمت پکت دریافتی: UP_DATA:MIN_SERVO,MAX_SERVO,LDR_LIMIT
          int sMin, sMax, ldrLim;
          if(sscanf(serialBuffer, "UP_DATA:%d,%d,%d", &sMin, &sMax, &ldrLim) == 3) {
            servoMin = sMin; servoMax = sMax; ldrThreshold = ldrLim;
            Serial.println("UPDATE_SUCCESS_WAIT_RESET");
            delay(2000);
            // با عمداً غذا ندادن به سگ نگهبان (WDT)، سیستم پس از ۴ ثانیه فوراً هارد ری‌استارت می‌شود!
            while(1); 
          }
        }
        else if(strcmp(serialBuffer, "SECURE_PROX_AUTH:77A") == 0 && !isAuthorized) {
          isAuthorized = true; triggerWelcomeLights(); playSound(1);
        }
        else if(strcmp(serialBuffer, "SECURE_PROX_LOCK:99X") == 0 && isAuthorized) {
          isAuthorized = false; toggleIgnition(false); triggerFollowMeLights(); playSound(2);
        }
        else if(strcmp(serialBuffer, "VALET:1") == 0) {
          valetMode = true; isAuthorized = true; adminMode = false; autoHeatingActive = false;
          throttleServo.write(servoMin); playSound(4);
        }
        else if(strcmp(serialBuffer, "VALET:0") == 0) {
          valetMode = false; isAuthorized = false; toggleIgnition(false); playSound(5);
        }
        serialIndex = 0; 
      }
    } else if (serialIndex < 34) {
      serialBuffer[serialIndex++] = c;
    }
  }

  if (masterIsHeld && (currentMillis - lastMasterSeen > 1500)) {
    masterIsHeld = false;
    if (currentMillis - masterFirstSeen < 10000) { adminMode = !adminMode; playSound(adminMode ? 4 : 5); }
  }

  // ورودی ریموت‌های رادیویی
  if (digitalRead(pin_rem_unlock) == HIGH) { wakeUpSystem(); if(!isAuthorized){ isAuthorized = true; triggerWelcomeLights(); playSound(1); } }
  if (digitalRead(pin_rem_lock) == HIGH) { wakeUpSystem(); isAuthorized = false; toggleIgnition(false); triggerFollowMeLights(); playSound(2); delay(200); }

  // موتور آنالیز پترن مورس دکمه استارت (۲ کوتاه، ۱ بلند، ۳ کوتاه)
  bool stK = digitalRead(pin_st_k);
  
  if (stK && !last_stK) { 
    wakeUpSystem(); stK_pressTime = currentMillis; stK_handled = false; 
  }
  
  if (morseStep > 0 && (currentMillis - lastMorseTime > 4000)) { morseStep = 0; }

  if (stK && !stK_handled && (currentMillis - stK_pressTime >= 3000)) {
    stK_handled = true; 
    if (isAuthorized) { if (!isIgnOn) toggleIgnition(true); startCranking(); }
  }
  
  if (!stK && last_stK) { 
    stopCranking(); 
    unsigned long clickDuration = currentMillis - stK_pressTime;
    lastMorseTime = currentMillis;

    if (!isAuthorized && !stK_handled) { 
      bool isShort = (clickDuration > 50 && clickDuration < 950);
      bool isLong  = (clickDuration >= 2400 && clickDuration <= 4300);

      if (isShort && (morseStep == 0 || morseStep == 1)) { morseStep++; }
      else if (isLong && morseStep == 2) { morseStep++; }
      else if (isShort && (morseStep == 3 || morseStep == 4 || morseStep == 5)) {
        morseStep++;
        if (morseStep == 6) { isAuthorized = true; triggerWelcomeLights(); playSound(1); morseStep = 0; }
      } else { morseStep = 0; }
    }
    if (!stK_handled && isAuthorized) toggleIgnition(!isIgnOn); 
  }
  last_stK = stK;

  // منطق ریموت استارت
  bool startRem = digitalRead(pin_rem_start);
  if (startRem && !last_startRem) { wakeUpSystem(); startRem_pressTime = currentMillis; startRem_handled = false; }
  if (startRem && !startRem_handled && (currentMillis - startRem_pressTime >= 3000)) {
    startRem_handled = true; if (!isIgnOn) toggleIgnition(true); startCranking();
  }
  if (!startRem && last_startRem) { stopCranking(); if (!startRem_handled) toggleIgnition(false); }
  last_startRem = startRem;

  bool heatRem = digitalRead(pin_rem_heat);
  if (heatRem && !last_heatRem) { wakeUpSystem(); heatRem_pressTime = currentMillis; heatRem_handled = false; }
  if (heatRem && !heatRem_handled && (currentMillis - heatRem_pressTime >= 1000)) {
    heatRem_handled = true; if(!valetMode) { autoHeatingActive = false; throttleServo.write(servoMax); }
  }
  if (!heatRem && last_heatRem) {
    if (heatRem_handled) throttleServo.write(servoMin);
    else if (!valetMode) { autoHeatingActive = !autoHeatingActive; if (autoHeatingActive) autoHeatStartTime = currentMillis; else throttleServo.write(servoMin); }
  }
  last_heatRem = heatRem;

  // اتو ساسات دمایی حرارتی ۵۵ درجه کارکرد بهینه
  if (autoHeatingActive && isIgnOn && !valetMode) {
    if (currentTemp >= 55.0 || (currentMillis - autoHeatStartTime > 300000UL)) { 
      autoHeatingActive = false; throttleServo.write(servoMin); 
    } else {
      unsigned long cycleTime = (currentMillis - autoHeatStartTime) % 3000;
      throttleServo.write((cycleTime < 1000) ? servoHeatPos : servoMin);
    }
  }

  // لایت سنسور خودکار خزانه‌داری
  if (digitalRead(pin_auto_light_key) == HIGH && !welcomeLightActive && !followMeLightActive) {
    bool turnOnLight = (ldrPolarity == 1) ? (analogRead(pin_ldr) > ldrThreshold) : (analogRead(pin_ldr) < ldrThreshold);
    digitalWrite(pin_light, turnOnLight ? RELAY_ON : RELAY_OFF);
  } else if (digitalRead(pin_auto_light_key) == LOW && !welcomeLightActive && !followMeLightActive) {
    digitalWrite(pin_light, RELAY_OFF);
  }

  // آپدیت مانیتور سیستم و پورت تلمتری سریال
  if (currentMillis - lastTelemetryCheck >= 500 && !ecoModeActive) { 
    lastTelemetryCheck = currentMillis;
    
    Serial.print("BAT:"); Serial.print(currentVbat, 1);
    Serial.print(",TMP:"); Serial.print(currentTemp, 1);
    Serial.print(",IGN:"); Serial.print(isIgnOn);
    Serial.print(String(",LRN:") + digitalRead(pin_learn_key)); 
    Serial.print(",VAL:"); Serial.print(valetMode);
    Serial.print(",ADM:"); Serial.println(adminMode);

    oled.home();
    oled.println("======================");
    oled.print(" HOLY VAULT : "); oled.print(currentTemp, 1); oled.println(" C  ");
    oled.print(" DC VOLTAGE : "); oled.print(currentVbat, 1); oled.println(" V  ");
    oled.println("======================");
    
    oled.print("CORE ST: "); 
    if (valetMode) oled.println("VALET ACTIVE ");
    else oled.println(adminMode ? "ADMIN CONFIG " : (isAuthorized ? "UNLOCKED     " : "ARMED/LOCKED "));
    
    oled.print("CHOKE  : "); 
    if (valetMode) oled.println("LOCKED OFF   ");
    else oled.println(autoHeatingActive ? "ON (AUTO)    " : "OFF          ");
    
    if (currentTemp > 85.0) { oled.println("!! CRITICAL OVERHEAT !!"); } else { oled.println("                        "); }
  }
}

void startCranking() {
  if (crankLockout) return;
  if (getBatteryVoltage() < 11.2) { playSound(3); return; } 
  isCranking = true;
  crankStartTime = millis();
  digitalWrite(pin_st, RELAY_ON);
}

void stopCranking() {
  if (isCranking) {
    isCranking = false;
    digitalWrite(pin_st, RELAY_OFF);
    if (digitalRead(pin_auto_heater_key) == HIGH && getEngineTemp() < 55.0 && !valetMode) { 
      autoHeatingActive = true;
      autoHeatStartTime = millis();
    }
  }
}

void toggleIgnition(bool state) {
  isIgnOn = state;
  digitalWrite(pin_ign, isIgnOn ? RELAY_ON : RELAY_OFF);
  if (!isIgnOn) {
    stopCranking();
    autoHeatingActive = false;
    throttleServo.write(servoMin);
  }
}