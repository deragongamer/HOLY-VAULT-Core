# ⚡ HOLY VAULT Core // MOTO-OS ⚡
> **System Architecture:** Dual-Kernel Hybrid Serial Network 
> **Production Status:** Deployment Ready (v2.0.4-stable)

---

## 🌍 Language / زبان
* [English Technical Manual](#english-technical-manual)
* [سند فنی و دفترچه راهنمای فارسی](#سند-فنی-و-دفترچه-راهنمای-فارسی)

---

# ENGLISH TECHNICAL MANUAL

The **HOLY VAULT Core** is an ultra-heavy-duty, industrial-grade dual-kernel vehicle security ecosystem. Engineered specifically to replace vulnerable analog motorcycle ignition circuits, it establishes an unbreachable digital fortress by combining sub-GHz RF hardware overrides, cryptographic Morse code dynamic authenticators, and aggressive power-saving deep sleep state machines.

## 📁 Repository Directory Tree
the-HOLY-VAULT-Core/
├── firmware/
│   ├── motoOS-core/
│   │   └── motoOS-core.ino
│   └── motoOS_wifi_nodemcu/
│       └── motoOS_wifi_nodemcu.ino
└── README.md

---

## 🛠️ HARDWARE PINOUT SPECIFICATIONS (LINEAR MAPPING)

### 1. Arduino Nano Core (The Logic Matrix & Relay Executor)
D2 = Passive Buzzer [OUTPUT] -> Dedicated acoustic transducer driving the brutal alarm engine and discrete system chirps.
D3 = Ignition Relay [OUTPUT] -> High-current isolation barrier controlling the main electrical power grid of the vehicle.
D4 = Starter Relay [OUTPUT] -> Solenoid actuator line driving the high-amperage engine cranking motor.
D5 = Choke/Throttle Servo [OUTPUT] -> PWM signal line managing the physical automated cold-start thermal choke positioning.
D6 = Siren/Horn Relay [OUTPUT] -> High-decibel external broadcast relay for emergency alarm events.
D7 = Headlight Relay [OUTPUT] -> Power relay driving the automated night illumination and "Follow-Me-Home" delay paths.
D8 = Handlebar Starter Key [INPUT] -> Resistor-pulled physical encoder line capturing live Morse overrides and manual start signals.
D9 = RF Remote Unlock [INPUT] -> Low-level hardware trigger isolating the sub-GHz remote receiver's unlock channel.
D10 = RF Remote Lock [INPUT] -> Low-level hardware trigger isolating the sub-GHz remote receiver's arming channel.
D11 = RF Remote Choke [INPUT] -> Remote receiver channel line capturing manual auxiliary choke override requests.
D12 = RF Remote Start [INPUT] -> Remote receiver channel line managing wireless engine ignition initiation commands.
D13 = Auto-Choke Switch [INPUT] -> Physical hardware toggle latching or bypassing the automated NTC thermal algorithms.
A0 = LDR Photocell [ANALOG] -> Environmental ambient light sensor mapped for automated tunnel and low-lux detection.
A1 = Auto-Light Switch [INPUT] -> Hardware toggle enabling or killing the autonomous night lighting subroutine.
A2 = RFID Learn Key [INPUT] -> Internal tactile configuration button initiating auxiliary user card EEPROM database writes.
A3 = Master Register Key [INPUT] -> Protected tactile button triggering hard-factory resets or master clearance procedures.
A4 = SSD1306 OLED SDA [I2C] -> Dedicated serial data line streaming telemetry packets to the physical display node.
A5 = SSD1306 OLED SCL [I2C] -> Hardware clock synchronization line driving the display rendering subsystem pipelines.
A6 = NTC 10K Thermistor [ANALOG] -> High-accuracy engine cylinder block thermal sensor calibrated for the 55°C operational boundary.
A7 = Voltage Divider Matrix [ANALOG] -> High-impedance analog network monitoring real-time battery voltage drops.
TX (D1) = ESP8266 RX Link [SERIAL] -> Asynchronous transmitter line dispatching system status telemetry strings to the Web Panel.
RX (D0) = ESP8266 TX Link [SERIAL] -> Asynchronous receiver line listening for encrypted wireless proximity authorization tokens.

### 2. ESP8266 NodeMCU Node (The Wireless Gateway & Radar Node)
TX (GPIO1) = Arduino Nano RX Node [SERIAL] -> High-priority data transmission line firing proximity tokens and Valet state changes.
RX (GPIO3) = Arduino Nano TX Node [SERIAL] -> Data reception line accepting live battery and temperature matrix data packets.
SoftAP = Local Network Gateway [WIFI] -> Broadcasts the local administrative WiFi network known as "HOLY_VAULT_GATE".
Station = Proximity Background Radar [WIFI] -> Background asynchronous probe scanner measuring RSSI density of target smartphone.

---

## 💀 DEFENSIVE SUBROUTINES & OPERATIONAL PHILOSOPHY

### 1. High-Density Proximity Radar (Keyless Go)
The wireless gateway node operates as a non-discoverable background network scanner. It continuously sweeps the environment for your paired smartphone's unique encrypted hotspot. The system transitions into an unlocked state if and only if the physical signal density satisfies the strict mathematical threshold of an RSSI greater than -50 dBm. If signal saturation degrades past this boundary as you walk away, the core executes an immediate ignition cutoff sequence, runs a 30-second follow-me headlight pathway, and locks all data sectors.

### 2. Cryptographic Morse Code Overrides
In tactical environments where RF jamming is prevalent, wireless transceivers are compromised, or the smartphone matrix completely loses charge, the Handlebar Starter Button alters its functional state machine into a high-security input encoder. Inputting the exact hardcoded brutal sequence (2 Short, 1 Long, 3 Short) triggers a total hardware bypass. The system samples pulse timings down to a millisecond grid; any input variance exceeding a 150ms window resets the authorization stack to zero to mitigate algorithmic brute-force attacks.

### 3. Automated 28-Hour Eco-Sleep Architecture
To maintain the structural health of the vehicle's primary battery cells during extended storage, the system runs a background epoch timer. If the vault remains continuously locked without physical or serial interaction for 28 consecutive hours, it enforces a total blackout state (`ECO:1`). The main core completely severs the OLED rendering pipeline power rails and forces the ESP8266 radio subsystem into a physical hardware sleep mode, reducing current consumption to negligible micro-amperes. The core awakens only via a hardware-level pulse on localized sensor lines.

### 4. Valet Mode Isolation Protocols
When handing the vehicle over to third-party maintenance crews, the administrator can deploy Valet Mode via the local server gate interface. This completely decouples the authentication requirement, allowing engine operation without a smartphone or RFID master tag present. However, to guarantee asset protection, the core locks the automated choke servo inside strict low-RPM boundaries to prevent engine abuse, blocks administrative memory sectors to prevent unauthorized key duplication, and deploys a non-negotiable 10-minute continuous runtime limit. If engine idling breaks the 10-minute threshold, ignition grids are cut, and sirens are triggered.

---

---

# سند فنی و راهنمای فارسی

پروژه **HOLY VAULT Core** یک اکوسیستم امنیتی دو هسته‌ای، صنعتی و فوق سنگین برای وسایل نقلیه (نسخه موتورسیکلت) است. این سیستم سیم‌کشی آنالوگ، سنتی و آسیب‌پذیر انجین را به یک دژ دیجیتال نفوذناپذیر تبدیل می‌کند. این هسته با ادغام ماژول‌های اسکنر فرکانسی، رمزنگارهای فیزیکی مورس و بردهای کنترل رله، سطوح امنیتی نظامی را برای دارایی شما فراهم می‌آورد.

## 📁 ساختار پوشه‌بندی مخزن گیت‌هاب
the-HOLY-VAULT-Core/
├── firmware/
│   ├── motoOS-core/
│   │   └── motoOS-core.ino
│   └── motoOS_wifi_nodemcu/
│       └── motoOS_wifi_nodemcu.ino
└── README.md

---

## 🛠️ مشخصات پین‌اوت سخت‌افزاری (نگاشت خطی پین‌ها)

### ۱. هسته اصلی پردازش و کنترل رله‌ها (Arduino Nano)
D2 = بازر پسیو [خروجی] -> درایور صوتی اصلی برای شلیک فرکانس‌های هشدار دزدگیر، چیرپ‌های تایید فرامین و آلارم زنده.
D3 = رله سوئیچ اصلی [خروجی] -> قطع‌کننده جریان بالای اصلی جهت مدیریت شریان برق سراسری موتورسیکلت.
D4 = رله استارت [خروجی] -> خط تحریک بوبین استارت الکتریکی جهت درگیر کردن دنده‌استارت انجین.
D5 = سروو موتور ساسات [خروجی] -> سیگنال PWM مدیریت زاویه فیزیکی ساسات اتوماتیک و گاز ریز حرارتی.
D6 = رله بوق یا آژیر خارجی [خروجی] -> رانش جریان خروجی به بوق اصلی وسیله در صورت بروز تحریک فیزیکی و سرقت.
D7 = رله چراغ جلو [خروجی] -> اتوماسیون چراغ اصلی انجین و مدیریت زمان‌بندی مدار مشایعت شبانه (Follow-Me).
D8 = شاسی استارت فرمان [ورودی] -> انکودر فیزیکی جهت خوانش پترن مورس اضطراری و فرستنده پالس استارت دستی.
D9 = ریموت رادیویی کانال Unlock [ورودی] -> پین دیجیتال ایزوله‌شده جهت دریافت فرامین بازگشایی از راه دور گیرنده رادیویی.
D10 = ریموت رادیویی کانال Lock [ورودی] -> پین دیجیتال ایزوله‌شده جهت دریافت فرامین قفل و فعالسازی دزدگیر گیرنده رادیویی.
D11 = ریموت رادیویی کانال ساسات [ورودی] -> کانال کمکی جهت فعالسازی یا لغو دستی وضعیت ساسات از راه دور.
D12 = ریموت رادیویی کانال استارت [ورودی] -> دریافت فرمان استارت بدون کلید از روی ریموت رادیویی سخت‌افزاری.
D13 = کلید اتوساسات [ورودی] -> کلید کلنگی فیزیکی جهت تایید یا لغو کامل کدهای منطقی سنسور حرارتی سلولار.
A0 = فتوسل یا سنسور نور LDR [آنالوگ] -> آنالیز دائم شدت لومن نور محیط برای تشخیص ورود به تونل یا تاریکی شب.
A1 = کلید چراغ اتوماتیک [ورودی] -> لغو فیزیکی یا فعال‌سازی خطوط اتوماسیون چراغ مشایعت و روشنایی خودکار.
A2 = کلید ذخیره کارت یا Learn [ورودی] -> شاسی پنهان داخلی برای ورود به مد پذیرش کارت‌های RFID جدید در حافظه EEPROM.
A3 = کلید ثبت مسترکارت [ورودی] -> پین حفاظت‌شده سخت‌افزاری جهت پاکسازی کامل حافظه یا معرفی مسترکارت مادر.
A4 = خط دیتای نمایشگر OLED SDA [پروتکل I2C] -> باس دیتای سریال جهت رندر و ارسال کدهای تلمتری به مانیتور سیستم.
A5 = خط کلاک نمایشگر OLED SCL [پروتکل I2C] -> خط همگام‌سازی کلاک برای پایداری فریم‌های رندر شده مانیتور دژ.
A6 = سنسور دمای سیلندر NTC 10K [آنالوگ] -> سنجش زنده حرارت بدنه انجین کالیبره‌شده روی مرز عملیاتی ۵۵ درجه سانتی‌گراد.
A7 = تقسیم مقاومت ولتاژ باطری [آنالوگ] -> شبکه مانیتورینگ آنلاین افت ولتاژ سلول‌های باطری در زمان استارت و درجا کار کردن.
TX (D1) = لینک دیتای فرستنده به وای‌فای [سریال] -> ارسال کدهای وضعیت ولتاژ باطری و دمای زنده به گیت بی‌سیم جهت نمایش در وب‌پنل.
RX (D0) = لینک دیتای گیرنده از وای‌فای [سریال] -> گوش به زنگ بودن دائم برای دریافت توکن‌های بازگشایی قفل از رادار مجاورتی.

### ۲. ماژول ارتباط وایرلس و وب‌پنل (ESP8266 NodeMCU)
TX (GPIO1) = خط فرستنده به آردوینو [سریال] -> شلیک کدهای تایید هویت گوشی مجاورتی و فرامین مد تعمیرگاه به هسته نانو.
RX (GPIO3) = خط گیرنده از آردوینو [سریال] -> دریافت پکت‌های داده‌ای ولتاژ و حرارت انجین جهت آپدیت منوهای وب‌پنل.
SoftAP = نقطه اتصال محلی وای‌فای -> بالا آوردن یک شبکه بیسیم محلی آفلاین با نام اختصاصی "HOLY_VAULT_GATE".
Station = رادار اسکن محیطی -> اسکن آسنکرون پس‌زمینه جهت یافتن و ارزیابی چگالی سیگنال RSSI هات‌اسپات گوشی مالک.

---

## 💀 مکانیزم‌های دفاعی و فلسفه عملکرد ادوات

### ۱. رادار مجاورتی کی‌لس هوشمند (Proximity Keyless Go)
ماژول بی‌سیم سیستم به عنوان یک رادار پنهان و غیرقابل ردیابی در پس‌زمینه عمل می‌کند. این گیت دائم فرکانس‌های محیطی را برای یافتن سیگنال هویت‌گذاری شده‌ی هات‌اسپات گوشی شما اسکن می‌کند. قفل خزانه‌های سیستم تنها و تنها زمانی باز می‌شود که چگالی امواج در یک شرایط فیزیکی پایدار به مرز ریاضی فرکانسی RSSI بزرگتر از ۵۰- دسی‌بل برسد. به محض دور شدن شما و افت سیگنال از این مرز، سیستم در صدم ثانیه برق مدار جرقه زنی را قطع کرده، مدار مشایعت چراغ جلو را به مدت ۳۰ ثانیه فعال می‌کند و دژ را آرم می‌سازد.

### ۲. رمزنگار مورس فیزیکی (سوییچ مخفی سخت‌افزاری)
در شرایطی که باطری گوشی شما تمام شده باشد، یا امواج رادیویی منطقه تحت جمرهای فرکانسی (Jamming) مختل شده باشند، کلید استارت فیزیکی روی فرمان موتورسیکلت تغییر ماهیت داده و تبدیل به یک انکودر پیشرفته می‌شود. وارد کردن پترن سخت‌گیرانه (۲ بار کوتاه، ۱ بار بلند ممتد، ۳ بار کوتاه) یک فرمان بای‌پاس سخت‌افزاری صادر می‌کند. سنجش زمان کلیک‌ها با دقت میلی‌ثانیه بررسی می‌شود؛ به طوری که هرگونه خطای زمانی بیش از ۱۵۰ میلی‌ثانیه، کل استک پردازش رمز را صفر می‌کند تا از هک‌های الگوریتمی حدس رمز جلوگری شود.

### ۳. معماری خواب زمستانی ۲۸ ساعته (حفظ توان باطری)
برای جلوگیری از تخلیه باطری موتورسیکلت در زمان‌های پارک طولانی‌مدت، یک تایمر اپوک در پس‌زمینه هسته فعال است. اگر سیستم به مدت ۲۸ ساعت متوالی بدون هیچگونه پالس ورودی یا فرکانسی در حالت قفل بماند، فرمان فوق‌العاده سنگین `ECO:1` صادر می‌شود. با این فرمان، جریان مانیتور OLED قطع شده و بخش رادیویی وای‌فای کاملاً به خواب فیزیکی می‌رود تا مصرف برق به نزدیک صفر میکروآمپر برسد. بیدار شدن سیستم در این حالت فقط با پالس‌های فیزیکی کلیدهای اصلی امکان‌پذیر است.

### ۴. پروتکل قرنطینه حالت تعمیرگاه (Valet Mode)
در زمان سپردن موتورسیکلت به تعمیرکار، مالک می‌تواند از طریق منوی وب‌پنل اختصاصی، حالت تعمیرگاه را فعال کند. این مد، تایید هویت‌های گوشی و کارت را معلق می‌کند تا موتور بدون نیاز به مالک روشن شود. اما جهت تضمین سلامت دستگاه، سروو موتور اتوساسات روی زوایای پایین قفل می‌شود تا تعمیرکار نتواند به انجین گاز هرز یا آسیب بزند، حافظه ثبت کارت‌ها مسدود شده و یک تایمر معکوس ۱۰ دقیقه‌ای فعال می‌شود. اگر موتور بیش از ۱۰ دقیقه روشن بماند، سیستم کل سوئیچ اصلی را قطع کرده و آژیرهای خطر را فعال می‌کند.