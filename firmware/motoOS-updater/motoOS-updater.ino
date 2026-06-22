// ====================================================================
// HOLY VAULT ECOSYSTEM - EXTERNAL UPDATER KERNEL
// ====================================================================
const int pin_trigger_btn = 2; // دکمه فیزیکی برای شروع پروسه آپدیت
const int pin_target_reset = 4; // اتصال به پایه RESET آردوینو اصلی موتور

void setup() {
  Serial.begin(9600); // همگام با باودریت هسته اصلی
  pinMode(pin_trigger_btn, INPUT_PULLUP);
  
  // پایه ریست را روی حالت ورودی با امپدانس بالا می‌گذاریم تا مزاحم کار عادی موتور نشود
  pinMode(pin_target_reset, INPUT); 
}

void loop() {
  // منتظر فشردن کلید آپدیت بمان
  if (digitalRead(pin_trigger_btn) == LOW) {
    delay(200); // رفع نویز کلید
    
    // ۱. هارد ریست فیزیکی برد اصلی موتور برای باز شدن بافرهای اولیه
    pinMode(pin_target_reset, OUTPUT);
    digitalWrite(pin_target_reset, LOW); // کشیدن پایه ریست به زمین
    delay(300);
    pinMode(pin_target_reset, INPUT); // رها کردن پایه ریست جهت بوت مجدد
    delay(1000); // مهلت برای استارت اولیه بوت‌لودر
    
    // ۲. ارسال سیگنال ورود به فاز گیت آپدیت
    Serial.println("SYS_UPDATE_START");
    delay(1500); 
    
    // ۳. تزریق اطلاعات فریم‌ور و کالیبراسیون جدید به سیستم اصلی
    // فرمت پکت کالیبراسیون: UP_DATA:MIN_SERVO,MAX_SERVO,LDR_LIMIT
    // در این پکت تستی: زاویه ساسات تغییر کرده و حد لایت‌سنسور فیکس شده است
    Serial.println("UP_DATA:10,170,620"); 
    
    // ۴. اتمام کار و انتظار برای اعمال ری‌استارت خودکار توسط ووتش‌داگ موتور
    delay(5000); 
  }
}