// ============================================================
//  HateBot — The Utterly Useless Judgmental Robot
//  Main Firmware  |  ESP32 + ILI9341 + MPU6050 + 2xSG90
// ============================================================

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <ESP32Servo.h>
#include <MPU6050.h>

// ── Pin Map ─────────────────────────────────────────────────
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4
#define TFT_BL    15
#define IMU_SDA   21
#define IMU_SCL   22
#define PIN_ARM_L 13
#define PIN_ARM_R 12
#define PIN_BUZZ  27
#define PIN_LED_CALM     14   // green
#define PIN_LED_ANNOYED  16   // yellow
#define PIN_LED_FURIOUS  17   // red

// ── Mood State ───────────────────────────────────────────────
enum Mood { CALM, ANNOYED, FURIOUS, SULKING };
Mood currentMood = CALM;

// Time-tracking
unsigned long lastDisturbanceMs  = 0;
unsigned long lastIdleTickMs     = 0;
unsigned long lastBlinkMs        = 0;
unsigned long sulkStartMs        = 0;
bool          ledBlinkState      = false;

// Escalation counter for FURIOUS → SULKING
int  furiousHitCount = 0;
#define SULK_TRIGGER_HITS  3   // hits while FURIOUS to trigger SULK

// De-escalation timeouts (ms)
#define TIMEOUT_ANNOYED_CALM    12000UL
#define TIMEOUT_FURIOUS_ANNOYED 10000UL
#define TIMEOUT_SULK_CALM       30000UL

// Idle animation interval
#define IDLE_TICK_INTERVAL  8000UL

// ── Hardware Objects ─────────────────────────────────────────
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
MPU6050          imu;
Servo            armL, armR;

// ── Insult String Pools ──────────────────────────────────────
const char* insults_calm[] = {
  "Oh. It's you.",
  "Still here?",
  "...cool.",
  "Ugh.",
  "Whatever.",
};
const char* insults_annoyed[] = {
  "Seriously?",
  "Don't.",
  "I'm busy existing.",
  "Please stop.",
  "You again.",
  "Not now.",
};
const char* insults_furious[] = {
  "HOW DARE YOU.",
  "I HAVE HAD IT.",
  "STOP. TOUCHING. ME.",
  "PERSONAL SPACE!!!",
  "ABSOLUTELY NOT.",
};
const char* insults_sulk[] = {
  "I'm done.",
  "Talk to the hand.",
  "...",
  "Goodbye.",
};

uint8_t insultIdx[4] = {0, 0, 0, 0};

// ── Colour palette ───────────────────────────────────────────
#define COL_BG_CALM      0x0841
#define COL_BG_ANNOYED   0x2940
#define COL_BG_FURIOUS   0x4800
#define COL_BG_SULK      0x1082
#define COL_FACE         0xFFE0
#define COL_BLACK        0x0000
#define COL_WHITE        0xFFFF
#define COL_TEXT_CALM    0x07FF
#define COL_TEXT_ANNOYED 0xFD20
#define COL_TEXT_FURIOUS 0xF800
#define COL_TEXT_SULK    0xC618

// ── Arm positions (degrees) ──────────────────────────────────
#define ARM_REST     90
#define ARM_TWITCH   95
#define ARM_FLICK    110
#define ARM_FURIOUS  130
#define ARM_SULK     140

// ── IMU thresholds ───────────────────────────────────────────
#define IMU_READ_HZ        50
#define IMU_BASELINE_READS 100
#define SHAKE_VAR_THRESH   6000
#define TILT_DEG_THRESH    15.0f
#define PICKUP_G_THRESH    0.3f
#define PICKUP_SUSTAIN_MS  500
#define TAP_ACCEL_THRESH   18000

float baselineAx, baselineAy, baselineAz;
float prevHighPassAcc = 0;
unsigned long pickupStartMs = 0;
bool pickupSustaining = false;

// ── Forward Declarations ─────────────────────────────────────
void calibrateIMU();
int  readIMUEvent();
void escalateMood(int weight);
void deEscalateIfCalmFor(unsigned long quietMs);
void reactToMood(Mood m);
void idleAnimationTick();
void drawFace(Mood m);
void drawFaceGeometry(Mood m);
void showInsult(Mood m);
void buzzPattern(Mood m);
void buzz(int freq, int dur);
void moveArms(Mood m);
void armSet(int degL, int degR, int delayMs);
void setLED(Mood m);
uint16_t moodBg(Mood m);
uint16_t moodTextColor(Mood m);

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);

  // TFT
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(COL_BG_CALM);

  // LEDs + Buzzer
  pinMode(PIN_LED_CALM,    OUTPUT);
  pinMode(PIN_LED_ANNOYED, OUTPUT);
  pinMode(PIN_LED_FURIOUS, OUTPUT);
  pinMode(PIN_BUZZ,        OUTPUT);

  // Servos
  armL.attach(PIN_ARM_L, 500, 2400);
  armR.attach(PIN_ARM_R, 500, 2400);
  armSet(ARM_REST, ARM_REST, 500);

  // IMU
  Wire.begin(IMU_SDA, IMU_SCL);
  imu.initialize();
  if (!imu.testConnection()) {
    Serial.println("MPU6050 not found — check wiring.");
  }
  calibrateIMU();

  // Boot state
  drawFace(CALM);
  setLED(CALM);
  lastDisturbanceMs = millis();
  lastIdleTickMs    = millis();

  Serial.println("HateBot online. Already hates you.");
}

// ============================================================
//  MAIN LOOP
// ============================================================
void loop() {
  unsigned long now = millis();

  // Red LED blink for FURIOUS / SULKING
  if (currentMood == FURIOUS || currentMood == SULKING) {
    if (now - lastBlinkMs > 500) {
      ledBlinkState = !ledBlinkState;
      digitalWrite(PIN_LED_FURIOUS, ledBlinkState ? HIGH : LOW);
      lastBlinkMs = now;
    }
  }

  // IMU read & escalation
  int disturbance = readIMUEvent();
  if (disturbance > 0) {
    escalateMood(disturbance);
    lastDisturbanceMs = now;
    reactToMood(currentMood);
  }

  // De-escalation
  deEscalateIfCalmFor(now - lastDisturbanceMs);

  // Idle tick
  if (now - lastIdleTickMs > IDLE_TICK_INTERVAL) {
    idleAnimationTick();
    lastIdleTickMs = now;
  }

  delay(1000 / IMU_READ_HZ);
}

// ============================================================
//  IMU CALIBRATION
// ============================================================
void calibrateIMU() {
  long sumAx = 0, sumAy = 0, sumAz = 0;
  for (int i = 0; i < IMU_BASELINE_READS; i++) {
    int16_t ax, ay, az, gx, gy, gz;
    imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sumAx += ax; sumAy += ay; sumAz += az;
    delay(10);
  }
  baselineAx = (float)sumAx / IMU_BASELINE_READS;
  baselineAy = (float)sumAy / IMU_BASELINE_READS;
  baselineAz = (float)sumAz / IMU_BASELINE_READS;
  Serial.printf("Baseline: Ax=%.0f Ay=%.0f Az=%.0f\n",
                baselineAx, baselineAy, baselineAz);
}

// ============================================================
//  IMU EVENT CLASSIFICATION
//  Returns: 0=none, 1=tap/tilt, 2=shake, 3=pickup
// ============================================================
int readIMUEvent() {
  int16_t ax, ay, az, gx, gy, gz;
  imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float dAx = ax - baselineAx;
  float dAy = ay - baselineAy;
  float dAz = az - baselineAz;

  // Tap — high-pass spike
  float accelMag = sqrt(dAx*dAx + dAy*dAy + dAz*dAz);
  float highPass  = accelMag - prevHighPassAcc;
  prevHighPassAcc = accelMag;
  if (abs(highPass) > TAP_ACCEL_THRESH) return 1;

  // Shake — variance over rolling window
  static float magWindow[8] = {0};
  static int wIdx = 0;
  magWindow[wIdx++ % 8] = accelMag;
  float mean = 0;
  for (int i = 0; i < 8; i++) mean += magWindow[i];
  mean /= 8.0f;
  float variance = 0;
  for (int i = 0; i < 8; i++) variance += (magWindow[i]-mean)*(magWindow[i]-mean);
  variance /= 8.0f;
  if (variance > SHAKE_VAR_THRESH) return 2;

  // Pickup — sustained Z deviation (MPU6050 default: ±2g → 16384 LSB/g)
  float zDiffG = abs(dAz) / 16384.0f;
  unsigned long now = millis();
  if (zDiffG > PICKUP_G_THRESH) {
    if (!pickupSustaining) {
      pickupSustaining = true;
      pickupStartMs = now;
    } else if (now - pickupStartMs > PICKUP_SUSTAIN_MS) {
      pickupSustaining = false;
      return 3;
    }
  } else {
    pickupSustaining = false;
  }

  // Tilt — static angle departure
  float tiltAngle = atan2(dAy, dAz) * 180.0f / PI;
  if (abs(tiltAngle) > TILT_DEG_THRESH) return 1;

  return 0;
}

// ============================================================
//  MOOD ESCALATION
// ============================================================
void escalateMood(int weight) {
  if (currentMood == SULKING) return;  // sulk ignores all input

  if (currentMood == FURIOUS) {
    furiousHitCount++;
    if (furiousHitCount >= SULK_TRIGGER_HITS) {
      currentMood     = SULKING;
      sulkStartMs     = millis();
      furiousHitCount = 0;
      return;
    }
  } else {
    furiousHitCount = 0;
  }

  int next = min((int)currentMood + weight, (int)FURIOUS);
  currentMood = (Mood)next;
}

// ============================================================
//  DE-ESCALATION
// ============================================================
void deEscalateIfCalmFor(unsigned long quietMs) {
  Mood prev = currentMood;
  switch (currentMood) {
    case SULKING:
      if (quietMs > TIMEOUT_SULK_CALM) { currentMood = CALM; furiousHitCount = 0; }
      break;
    case FURIOUS:
      if (quietMs > TIMEOUT_FURIOUS_ANNOYED) currentMood = ANNOYED;
      break;
    case ANNOYED:
      if (quietMs > TIMEOUT_ANNOYED_CALM)    currentMood = CALM;
      break;
    default: break;
  }
  if (currentMood != prev) {
    drawFace(currentMood);
    setLED(currentMood);
  }
}

// ============================================================
//  FULL MOOD REACTION
// ============================================================
void reactToMood(Mood m) {
  drawFace(m);
  showInsult(m);
  buzzPattern(m);
  moveArms(m);
  setLED(m);
}

// ============================================================
//  IDLE ANIMATION (CALM only)
// ============================================================
void idleAnimationTick() {
  if (currentMood != CALM) return;
  armSet(ARM_TWITCH, ARM_REST, 400);
  delay(300);
  armSet(ARM_REST,   ARM_REST, 400);
  buzz(180, 80);
}

// ============================================================
//  FACE DRAWING
// ============================================================
void drawFace(Mood m) {
  tft.fillScreen(moodBg(m));
  int cx = 160, cy = 100, r = 60;
  tft.fillCircle(cx, cy, r, COL_FACE);
  tft.drawCircle(cx, cy, r, COL_BLACK);
  drawFaceGeometry(m);
}

void drawFaceGeometry(Mood m) {
  int cx = 160, cy = 100;
  switch (m) {

    case CALM: {
      // Half-closed bored slits
      tft.fillRect(cx-38, cy-18, 22, 8, COL_BLACK);
      tft.fillRect(cx+16, cy-18, 22, 8, COL_BLACK);
      // Flat mouth
      tft.drawFastHLine(cx-18, cy+22, 36, COL_BLACK);
      // Flat eyebrows
      tft.drawFastHLine(cx-38, cy-28, 22, COL_BLACK);
      tft.drawFastHLine(cx+16, cy-28, 22, COL_BLACK);
      break;
    }

    case ANNOYED: {
      // Side-eye — pupils shifted inward
      tft.fillCircle(cx-28, cy-14, 10, COL_BLACK);
      tft.fillCircle(cx+28, cy-14, 10, COL_BLACK);
      tft.fillCircle(cx-31, cy-14,  4, COL_WHITE);
      tft.fillCircle(cx+25, cy-14,  4, COL_WHITE);
      // Angled brows (inner corner lower)
      tft.drawLine(cx-38, cy-32, cx-16, cy-26, COL_BLACK);
      tft.drawLine(cx+16, cy-26, cx+38, cy-32, COL_BLACK);
      // Slight frown
      tft.drawLine(cx-18, cy+24, cx,    cy+20, COL_BLACK);
      tft.drawLine(cx,    cy+20, cx+18, cy+24, COL_BLACK);
      break;
    }

    case FURIOUS: {
      // Hard squinted eyes
      tft.fillRect(cx-38, cy-20, 20, 12, COL_BLACK);
      tft.fillRect(cx+18, cy-20, 20, 12, COL_BLACK);
      // Sharp V brows
      tft.drawLine(cx-38, cy-36, cx-18, cy-26, COL_BLACK);
      tft.drawLine(cx-18, cy-26, cx-14, cy-36, COL_BLACK);
      tft.drawLine(cx+14, cy-36, cx+18, cy-26, COL_BLACK);
      tft.drawLine(cx+18, cy-26, cx+38, cy-36, COL_BLACK);
      // Bared teeth rect
      tft.drawRect(cx-18, cy+18, 36, 10, COL_BLACK);
      for (int i = 0; i < 6; i++) {
        tft.drawFastVLine(cx-18 + i*6, cy+18, 10, COL_BLACK);
      }
      break;
    }

    case SULKING: {
      // X eyes
      tft.drawLine(cx-38, cy-20, cx-18, cy-8,  COL_BLACK);
      tft.drawLine(cx-18, cy-20, cx-38, cy-8,  COL_BLACK);
      tft.drawLine(cx+18, cy-20, cx+38, cy-8,  COL_BLACK);
      tft.drawLine(cx+38, cy-20, cx+18, cy-8,  COL_BLACK);
      // Resigned flat mouth
      tft.drawFastHLine(cx-14, cy+22, 28, COL_BLACK);
      // Sub-text
      tft.setTextColor(moodTextColor(SULKING));
      tft.setTextSize(1);
      tft.setCursor(cx-46, cy+38);
      tft.print("[ talk to the hand ]");
      break;
    }
  }
}

// ============================================================
//  INSULT TEXT
// ============================================================
void showInsult(Mood m) {
  const char** pool;
  int poolSize;
  switch (m) {
    case CALM:    pool = insults_calm;    poolSize = 5; break;
    case ANNOYED: pool = insults_annoyed; poolSize = 6; break;
    case FURIOUS: pool = insults_furious; poolSize = 5; break;
    case SULKING: pool = insults_sulk;    poolSize = 4; break;
    default:      return;
  }

  const char* line = pool[insultIdx[(int)m] % poolSize];
  insultIdx[(int)m]++;

  tft.fillRect(0, 175, 320, 40, moodBg(m));
  int textSize = (strlen(line) > 14) ? 2 : 3;
  int charW    = textSize * 6;
  int x        = max(0, (320 - (int)strlen(line) * charW) / 2);
  tft.setTextSize(textSize);
  tft.setTextColor(moodTextColor(m));
  tft.setCursor(x, 182);
  tft.print(line);
}

// ============================================================
//  BUZZER
// ============================================================
void buzz(int freq, int dur) {
  tone(PIN_BUZZ, freq, dur);
  delay(dur + 10);
  noTone(PIN_BUZZ);
}

void buzzPattern(Mood m) {
  switch (m) {
    case CALM:
      buzz(180, 60);
      break;
    case ANNOYED:
      buzz(440, 80); delay(60); buzz(330, 120);
      break;
    case FURIOUS:
      for (int i = 0; i < 4; i++) {
        buzz(600 + i*100, 60);
        delay(30);
      }
      break;
    case SULKING:
      buzz(200, 400);
      break;
  }
}

// ============================================================
//  SERVO ARMS
// ============================================================
void armSet(int degL, int degR, int delayMs) {
  armL.write(degL);
  armR.write(180 - degR);   // right arm mirrored
  delay(delayMs);
}

void moveArms(Mood m) {
  switch (m) {
    case CALM:    break;    // handled in idleAnimationTick
    case ANNOYED:
      armSet(ARM_FLICK,   ARM_FLICK,   200);
      delay(100);
      armSet(ARM_REST,    ARM_REST,    300);
      break;
    case FURIOUS:
      for (int i = 0; i < 4; i++) {
        armSet(ARM_FURIOUS, ARM_REST,   100);
        armSet(ARM_REST,    ARM_FURIOUS,100);
      }
      armSet(ARM_REST, ARM_REST, 200);
      break;
    case SULKING:
      armSet(ARM_SULK, ARM_SULK, 500);   // raise & hold
      break;
  }
}

// ============================================================
//  LED MOOD RING
// ============================================================
void setLED(Mood m) {
  digitalWrite(PIN_LED_CALM,    LOW);
  digitalWrite(PIN_LED_ANNOYED, LOW);
  digitalWrite(PIN_LED_FURIOUS, LOW);
  switch (m) {
    case CALM:    digitalWrite(PIN_LED_CALM,    HIGH); break;
    case ANNOYED: digitalWrite(PIN_LED_ANNOYED, HIGH); break;
    case FURIOUS: break;   // blink handled in loop()
    case SULKING: break;   // blink handled in loop()
  }
}

// ============================================================
//  HELPERS
// ============================================================
uint16_t moodBg(Mood m) {
  switch (m) {
    case CALM:    return COL_BG_CALM;
    case ANNOYED: return COL_BG_ANNOYED;
    case FURIOUS: return COL_BG_FURIOUS;
    case SULKING: return COL_BG_SULK;
  }
  return COL_BLACK;
}

uint16_t moodTextColor(Mood m) {
  switch (m) {
    case CALM:    return COL_TEXT_CALM;
    case ANNOYED: return COL_TEXT_ANNOYED;
    case FURIOUS: return COL_TEXT_FURIOUS;
    case SULKING: return COL_TEXT_SULK;
  }
  return COL_WHITE;
}
