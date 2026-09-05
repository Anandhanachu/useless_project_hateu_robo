// ╔══════════════════════════════════════════════════════════════╗
// ║          HateBot  —  Complete Firmware  v2.0                 ║
// ║  ESP32 + ILI9341 320×240 + MPU6050 + 2×SG90 + Buzzer + LEDs ║
// ╚══════════════════════════════════════════════════════════════╝
//
//  DISPLAY LAYOUT (landscape 320×240)
//  ┌──────────────────────────────────────┐  y=0
//  │  MOOD: ANNOYED   [████████░░] 80%   │  y=0..24   TOP BAR
//  ├──────────────────────────────────────┤  y=25
//  │                                      │
//  │         ╭────────────╮               │
//  │         │  (O_O)     │  ← FACE       │  y=25..194  FACE AREA
//  │         ╰────────────╯               │
//  │                                      │
//  ├──────────────────────────────────────┤  y=195
//  │  -=-  "You again."  -=-             │  y=195..239  INSULT BAR
//  └──────────────────────────────────────┘  y=239

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <ESP32Servo.h>
#include <MPU6050.h>
#include <math.h>

// ═══════════════════════════════════════════════════════
//  PIN DEFINITIONS
// ═══════════════════════════════════════════════════════
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4
#define TFT_BL    15
#define IMU_SDA   21
#define IMU_SCL   22
#define PIN_ARM_L 13
#define PIN_ARM_R 12
#define PIN_BUZZ  27
#define PIN_LED_CALM     14
#define PIN_LED_ANNOYED  16
#define PIN_LED_FURIOUS  17

// ═══════════════════════════════════════════════════════
//  SCREEN LAYOUT CONSTANTS
// ═══════════════════════════════════════════════════════
#define SCR_W       320
#define SCR_H       240
#define TOPBAR_H    25
#define INSULT_Y    195
#define INSULT_H    45
#define FACE_Y      TOPBAR_H
#define FACE_H      (INSULT_Y - TOPBAR_H)   // 170px
#define FACE_CX     160
#define FACE_CY     (TOPBAR_H + FACE_H/2)   // 110

// ═══════════════════════════════════════════════════════
//  COLOUR PALETTE  (RGB565)
// ═══════════════════════════════════════════════════════
#define C_BLACK         0x0000
#define C_WHITE         0xFFFF
#define C_FACE_YELLOW   0xFEA0   // warm emoji yellow
#define C_FACE_SHADOW   0xC4A0   // darker yellow for depth
#define C_CHEEK         0xF810   // orange-red blush

// Background per mood
#define C_BG_CALM       0x0228   // deep teal-black
#define C_BG_ANNOYED    0x2100   // burnt umber
#define C_BG_FURIOUS    0x4000   // deep blood red
#define C_BG_SULK       0x1009   // purple-black

// Grid line per mood
#define C_GRID_CALM     0x0430
#define C_GRID_ANNOYED  0x3180
#define C_GRID_FURIOUS  0x6000
#define C_GRID_SULK     0x180C

// Top bar accent
#define C_BAR_BG        0x0841
#define C_BAR_BORDER    0x4208

// Rage meter segments per mood
#define C_RAGE_CALM     0x07E0   // green
#define C_RAGE_ANNOYED  0xFD20   // amber
#define C_RAGE_FURIOUS  0xF800   // red
#define C_RAGE_SULK     0x8010   // dark magenta

// Text colours
#define C_TXT_CALM      0x67FE   // mint
#define C_TXT_ANNOYED   0xFD20   // amber
#define C_TXT_FURIOUS   0xF81F   // hot pink/red
#define C_TXT_SULK      0x8C71   // grey

// Insult bar
#define C_INSULT_BG     0x0841
#define C_INSULT_BORDER 0x4208
#define C_DECO          0x4208   // decorative dashes

// ═══════════════════════════════════════════════════════
//  MOOD STATE MACHINE
// ═══════════════════════════════════════════════════════
enum Mood { CALM = 0, ANNOYED = 1, FURIOUS = 2, SULKING = 3 };
Mood currentMood    = CALM;
Mood prevMood       = CALM;
bool moodChanged    = false;

int  furiousHits    = 0;
#define SULK_THRESHOLD     3

unsigned long lastDisturbMs   = 0;
#define T_ANNOYED_CALM     12000UL
#define T_FURIOUS_ANNOYED  10000UL
#define T_SULK_CALM        30000UL

// ═══════════════════════════════════════════════════════
//  HARDWARE OBJECTS
// ═══════════════════════════════════════════════════════
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
MPU6050          imu;
Servo            armL, armR;

// ═══════════════════════════════════════════════════════
//  IMU THRESHOLDS
// ═══════════════════════════════════════════════════════
#define IMU_HZ             50
#define IMU_CAL_SAMPLES    150
#define SHAKE_VAR_THRESH   7000.0f
#define TILT_DEG_THRESH    14.0f
#define PICKUP_G_THRESH    0.28f
#define PICKUP_SUSTAIN_MS  500
#define TAP_HP_THRESH      16000.0f

float  bAx, bAy, bAz;
float  prevHPacc       = 0;
bool   inPickup        = false;
unsigned long pickupMs = 0;

// ═══════════════════════════════════════════════════════
//  ANIMATION STATE  (all non-blocking via millis())
// ═══════════════════════════════════════════════════════

// Eye blink
unsigned long blinkTimer    = 0;
unsigned long blinkInterval = 3800;
bool          isBlinking    = false;
unsigned long blinkStartMs  = 0;
#define BLINK_DUR  120

// Typewriter insult
char          insultBuf[32]  = "";
uint8_t       insultReveal   = 0;   // chars revealed so far
unsigned long typewriterMs   = 0;
#define TYPEWRITER_SPEED_MS  55

// Screen shake (FURIOUS)
int8_t        shakeX         = 0;
int8_t        shakeY         = 0;
unsigned long shakeMs        = 0;
#define SHAKE_INTERVAL_MS  80

// Rage meter animation
uint8_t       rageDisplay    = 0;   // current displayed rage (0–100)
uint8_t       rageTarget     = 0;
unsigned long rageMeterMs    = 0;
#define RAGE_STEP_MS   30

// Star particles (CALM idle)
struct Star { uint16_t x; uint8_t y; uint8_t phase; };
#define NUM_STARS 12
Star stars[NUM_STARS];
unsigned long starMs = 0;
#define STAR_TICK_MS 400

// LED blink
unsigned long ledBlinkMs    = 0;
bool          ledBlinkState = false;

// Idle servo twitch
unsigned long idleTwitchMs  = 0;
#define IDLE_TWITCH_INTERVAL 9000UL

// ═══════════════════════════════════════════════════════
//  INSULT STRING POOLS
// ═══════════════════════════════════════════════════════
const char* pool_calm[] = {
  "Oh. It's you.",  "Still here?",    "...cool.",
  "Ugh.",           "Whatever.",      "Fine. Hi.",
  "Not now.",       "mm.",
};
const char* pool_annoyed[] = {
  "Seriously?",     "Don't.",         "I'm busy existing.",
  "Please stop.",   "You again.",     "Not now.",
  "Oh come ON.",    "For real?",      "Excuse you.",
};
const char* pool_furious[] = {
  "HOW DARE YOU.",  "I HAVE HAD IT.", "STOP. TOUCHING. ME.",
  "PERSONAL SPACE!","ABSOLUTELY NOT.","GET. AWAY.",
  "I AM DONE.",
};
const char* pool_sulk[] = {
  "I'm done.",      "Talk to hand.",  "...",
  "Goodbye.",       "*silence*",
};

uint8_t insultIdx[4] = {0, 0, 0, 0};
const int poolSizes[4] = {8, 9, 7, 5};
const char** pools[4] = { pool_calm, pool_annoyed, pool_furious, pool_sulk };

// Rage targets per mood (%)
const uint8_t ragePct[4] = {10, 45, 85, 30};

// Mood labels
const char* moodLabel[4] = {"CALM", "ANNOYED", "FURIOUS", "SULKING"};

// ═══════════════════════════════════════════════════════
//  COLOUR HELPERS
// ═══════════════════════════════════════════════════════
uint16_t bgColor(Mood m)   { const uint16_t t[]={C_BG_CALM,C_BG_ANNOYED,C_BG_FURIOUS,C_BG_SULK}; return t[m]; }
uint16_t gridColor(Mood m) { const uint16_t t[]={C_GRID_CALM,C_GRID_ANNOYED,C_GRID_FURIOUS,C_GRID_SULK}; return t[m]; }
uint16_t rageColor(Mood m) { const uint16_t t[]={C_RAGE_CALM,C_RAGE_ANNOYED,C_RAGE_FURIOUS,C_RAGE_SULK}; return t[m]; }
uint16_t txtColor(Mood m)  { const uint16_t t[]={C_TXT_CALM,C_TXT_ANNOYED,C_TXT_FURIOUS,C_TXT_SULK}; return t[m]; }

// ═══════════════════════════════════════════════════════
//  FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════
void calibrateIMU();
int  readIMUEvent();
void escalateMood(int w);
void deEscalate(unsigned long quietMs);
void onMoodChange(Mood newMood);
void triggerReaction(Mood m);

// Display
void drawFullScreen(Mood m);
void drawBackground(Mood m, int8_t ox=0, int8_t oy=0);
void drawTopBar(Mood m);
void drawRageMeter(uint8_t pct, Mood m);
void drawInsultBar(Mood m);
void drawInsultText(const char* s, Mood m, uint8_t charsToShow);
void drawFace(Mood m, bool blink=false, int8_t ox=0, int8_t oy=0);
void drawFaceCalm(int cx, int cy, bool blink);
void drawFaceAnnoyed(int cx, int cy, bool blink);
void drawFaceFurious(int cx, int cy, bool blink);
void drawFaceSulking(int cx, int cy, bool blink);

// Insult
void startNewInsult(Mood m);

// Hardware
void buzz(int freq, int dur);
void buzzPattern(Mood m);
void armSet(int dL, int dR, int ms);
void moveArms(Mood m);
void setLED(Mood m);
void initStars();
void tickStars();

// ═══════════════════════════════════════════════════════
//  SETUP
// ═══════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  delay(200);

  // TFT
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  tft.begin();
  tft.setSPISpeed(40000000);
  tft.setRotation(1);   // landscape

  // Hardware
  pinMode(PIN_LED_CALM,    OUTPUT);
  pinMode(PIN_LED_ANNOYED, OUTPUT);
  pinMode(PIN_LED_FURIOUS, OUTPUT);
  pinMode(PIN_BUZZ,        OUTPUT);

  armL.attach(PIN_ARM_L, 500, 2400);
  armR.attach(PIN_ARM_R, 500, 2400);
  armSet(90, 90, 600);

  Wire.begin(IMU_SDA, IMU_SCL);
  Wire.setClock(400000);
  imu.initialize();
  if (!imu.testConnection())
    Serial.println("[WARN] MPU6050 not found — check wiring");

  // Boot splash
  tft.fillScreen(C_BLACK);
  tft.setTextColor(C_TXT_CALM);
  tft.setTextSize(2);
  tft.setCursor(60, 100);
  tft.print("HateBot v2.0");
  tft.setTextSize(1);
  tft.setTextColor(C_TXT_SULK);
  tft.setCursor(88, 125);
  tft.print("calibrating...");
  buzz(220, 80); delay(100); buzz(330, 80); delay(100); buzz(440, 120);

  calibrateIMU();

  // Initial render
  initStars();
  rageTarget  = ragePct[CALM];
  rageDisplay = 0;
  drawFullScreen(CALM);
  setLED(CALM);
  startNewInsult(CALM);

  lastDisturbMs  = millis();
  idleTwitchMs   = millis();
  blinkTimer     = millis();
  blinkInterval  = random(3000, 6000);

  Serial.println("[HateBot] Online. Already hates you.");
}

// ═══════════════════════════════════════════════════════
//  MAIN LOOP
// ═══════════════════════════════════════════════════════
void loop() {
  unsigned long now = millis();

  // ── IMU ─────────────────────────────────────
  int ev = readIMUEvent();
  if (ev > 0) {
    escalateMood(ev);
    lastDisturbMs = now;
  }
  deEscalate(now - lastDisturbMs);

  // ── Screen shake (FURIOUS) ───────────────────
  if (currentMood == FURIOUS) {
    if (now - shakeMs > SHAKE_INTERVAL_MS) {
      shakeX = random(-2, 3);
      shakeY = random(-2, 3);
      shakeMs = now;
      // Only redraw face area on shake ticks for efficiency
      drawBackground(currentMood, shakeX, shakeY);
      drawFace(currentMood, isBlinking, shakeX, shakeY);
    }
  }

  // ── Rage meter animation ─────────────────────
  if (rageDisplay != rageTarget && now - rageMeterMs > RAGE_STEP_MS) {
    rageDisplay += (rageDisplay < rageTarget) ? 1 : -1;
    drawRageMeter(rageDisplay, currentMood);
    rageMeterMs = now;
  }

  // ── Eye blink ────────────────────────────────
  if (!isBlinking && now - blinkTimer > blinkInterval) {
    isBlinking   = true;
    blinkStartMs = now;
    blinkInterval = random(2500, 6000);
  }
  if (isBlinking) {
    if (now - blinkStartMs > BLINK_DUR) {
      isBlinking = false;
      blinkTimer = now;
      if (currentMood != FURIOUS) {   // shake handles furious redraws
        drawFace(currentMood, false, 0, 0);
      }
    } else {
      drawFace(currentMood, true, 0, 0);
    }
  }

  // ── Typewriter insult reveal ──────────────────
  if (insultReveal < strlen(insultBuf) && now - typewriterMs > TYPEWRITER_SPEED_MS) {
    insultReveal++;
    drawInsultText(insultBuf, currentMood, insultReveal);
    typewriterMs = now;
  }

  // ── Star particles (CALM) ────────────────────
  if (currentMood == CALM && now - starMs > STAR_TICK_MS) {
    tickStars();
    starMs = now;
  }

  // ── LED blink (FURIOUS / SULKING) ────────────
  if (currentMood == FURIOUS || currentMood == SULKING) {
    if (now - ledBlinkMs > 450) {
      ledBlinkState = !ledBlinkState;
      digitalWrite(PIN_LED_FURIOUS, ledBlinkState ? HIGH : LOW);
      ledBlinkMs = now;
    }
  }

  // ── Idle arm twitch (CALM) ───────────────────
  if (currentMood == CALM && now - idleTwitchMs > IDLE_TWITCH_INTERVAL) {
    armSet(95, 90, 350);
    delay(200);
    armSet(90, 90, 350);
    buzz(180, 60);
    idleTwitchMs = now;
  }

  delay(1000 / IMU_HZ);
}

// ═══════════════════════════════════════════════════════
//  IMU CALIBRATION
// ═══════════════════════════════════════════════════════
void calibrateIMU() {
  long sx=0, sy=0, sz=0;
  for (int i=0; i<IMU_CAL_SAMPLES; i++) {
    int16_t ax,ay,az,gx,gy,gz;
    imu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
    sx+=ax; sy+=ay; sz+=az;
    delay(8);
  }
  bAx = (float)sx/IMU_CAL_SAMPLES;
  bAy = (float)sy/IMU_CAL_SAMPLES;
  bAz = (float)sz/IMU_CAL_SAMPLES;
  Serial.printf("[IMU] Baseline Ax=%.0f Ay=%.0f Az=%.0f\n", bAx,bAy,bAz);
}

// ═══════════════════════════════════════════════════════
//  IMU EVENT  0=none  1=tap/tilt  2=shake  3=pickup
// ═══════════════════════════════════════════════════════
int readIMUEvent() {
  int16_t ax,ay,az,gx,gy,gz;
  imu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);

  float dAx = ax - bAx, dAy = ay - bAy, dAz = az - bAz;
  float mag  = sqrtf(dAx*dAx + dAy*dAy + dAz*dAz);

  // Tap — high-pass spike
  float hp = fabsf(mag - prevHPacc);
  prevHPacc = mag;
  if (hp > TAP_HP_THRESH) return 1;

  // Shake — rolling variance
  static float win[8]={0}; static uint8_t wi=0;
  win[wi++%8] = mag;
  float mean=0; for(int i=0;i<8;i++) mean+=win[i]; mean/=8.0f;
  float var=0;  for(int i=0;i<8;i++) var+=(win[i]-mean)*(win[i]-mean); var/=8.0f;
  if (var > SHAKE_VAR_THRESH) return 2;

  // Pickup — sustained Z departure
  float zg = fabsf(dAz) / 16384.0f;
  unsigned long now = millis();
  if (zg > PICKUP_G_THRESH) {
    if (!inPickup) { inPickup=true; pickupMs=now; }
    else if (now-pickupMs > PICKUP_SUSTAIN_MS) { inPickup=false; return 3; }
  } else inPickup=false;

  // Tilt
  float tilt = atan2f(dAy,dAz)*180.0f/PI;
  if (fabsf(tilt) > TILT_DEG_THRESH) return 1;

  return 0;
}

// ═══════════════════════════════════════════════════════
//  MOOD ESCALATION
// ═══════════════════════════════════════════════════════
void escalateMood(int w) {
  if (currentMood == SULKING) return;
  if (currentMood == FURIOUS) {
    if (++furiousHits >= SULK_THRESHOLD) {
      furiousHits = 0;
      onMoodChange(SULKING);
      return;
    }
  } else furiousHits = 0;

  int next = min((int)currentMood + w, (int)FURIOUS);
  if ((Mood)next != currentMood) onMoodChange((Mood)next);
  else triggerReaction(currentMood);  // same mood — still react
}

void deEscalate(unsigned long quietMs) {
  Mood prev = currentMood;
  switch(currentMood) {
    case SULKING: if (quietMs>T_SULK_CALM)        { furiousHits=0; currentMood=CALM; }    break;
    case FURIOUS: if (quietMs>T_FURIOUS_ANNOYED)   currentMood=ANNOYED; break;
    case ANNOYED: if (quietMs>T_ANNOYED_CALM)      currentMood=CALM;    break;
    default: break;
  }
  if (currentMood != prev) {
    rageTarget = ragePct[currentMood];
    drawFullScreen(currentMood);
    setLED(currentMood);
    startNewInsult(currentMood);
    drawFace(currentMood, false, 0, 0);
  }
}

void onMoodChange(Mood newMood) {
  currentMood = newMood;
  rageTarget  = ragePct[newMood];
  drawFullScreen(newMood);
  triggerReaction(newMood);
  startNewInsult(newMood);
  setLED(newMood);
}

void triggerReaction(Mood m) {
  buzzPattern(m);
  moveArms(m);
}

// ═══════════════════════════════════════════════════════
//  INSULT MANAGEMENT
// ═══════════════════════════════════════════════════════
void startNewInsult(Mood m) {
  const char* s = pools[(int)m][insultIdx[(int)m] % poolSizes[(int)m]];
  insultIdx[(int)m]++;
  strncpy(insultBuf, s, sizeof(insultBuf)-1);
  insultBuf[sizeof(insultBuf)-1] = '\0';
  insultReveal = 0;
  typewriterMs = millis();
  // Clear insult area
  tft.fillRect(0, INSULT_Y, SCR_W, INSULT_H, C_INSULT_BG);
  drawInsultBar(m);
}

// ═══════════════════════════════════════════════════════
//  ── DISPLAY ──────────────────────────────────────────
// ═══════════════════════════════════════════════════════

void drawFullScreen(Mood m) {
  tft.fillScreen(C_BLACK);
  drawBackground(m, 0, 0);
  drawTopBar(m);
  drawRageMeter(rageDisplay, m);
  drawInsultBar(m);
  drawFace(m, false, 0, 0);
}

// ── Background with grid ─────────────────────────────
void drawBackground(Mood m, int8_t ox, int8_t oy) {
  uint16_t bg   = bgColor(m);
  uint16_t grid = gridColor(m);
  int y0 = FACE_Y, y1 = INSULT_Y;
  tft.fillRect(0, y0, SCR_W, y1-y0, bg);
  // Vertical grid lines
  for (int x=16; x<SCR_W; x+=16)
    tft.drawFastVLine(x+ox, y0, y1-y0, grid);
  // Horizontal grid lines
  for (int y=y0+16; y<y1; y+=16)
    tft.drawFastHLine(0, y+oy, SCR_W, grid);
}

// ── Top bar ──────────────────────────────────────────
void drawTopBar(Mood m) {
  // Background
  tft.fillRect(0, 0, SCR_W, TOPBAR_H, C_BAR_BG);
  // Bottom border line
  tft.drawFastHLine(0, TOPBAR_H-1, SCR_W, C_BAR_BORDER);

  // "MOOD:" label
  tft.setTextSize(2);
  tft.setTextColor(C_WHITE);
  tft.setCursor(4, 4);
  tft.print("MOOD:");

  // Mood name in accent colour
  tft.setTextColor(txtColor(m));
  tft.setCursor(68, 4);
  tft.print(moodLabel[(int)m]);

  // Draw rage meter placeholder (filled by drawRageMeter)
  drawRageMeter(rageDisplay, m);
}

// ── Rage / Anger meter ───────────────────────────────
void drawRageMeter(uint8_t pct, Mood m) {
  // Positioned right side of top bar
  int bx = 218, by = 4, bw = 90, bh = 15;
  // Border
  tft.drawRect(bx-1, by-1, bw+2, bh+2, C_BAR_BORDER);
  // Background
  tft.fillRect(bx, by, bw, bh, C_BAR_BG);

  // Segmented fill (10 segments)
  int filled = (pct * 10) / 100;
  uint16_t rc = rageColor(m);
  for (int i=0; i<10; i++) {
    int sx = bx + i*9;
    if (i < filled)
      tft.fillRect(sx, by, 8, bh, rc);
    else
      tft.fillRect(sx, by, 8, bh, C_BAR_BG);
  }
  // Percent label
  tft.fillRect(bx+bw+2, by-1, 26, bh+2, C_BAR_BG);
  tft.setTextSize(1);
  tft.setTextColor(rc);
  tft.setCursor(bx+bw+3, by+4);
  char pbuf[6];
  snprintf(pbuf, sizeof(pbuf), "%3d%%", pct);
  tft.print(pbuf);
}

// ── Insult bar chrome ────────────────────────────────
void drawInsultBar(Mood m) {
  tft.fillRect(0, INSULT_Y, SCR_W, INSULT_H, C_INSULT_BG);
  tft.drawFastHLine(0, INSULT_Y, SCR_W, C_BAR_BORDER);
  tft.drawFastHLine(0, SCR_H-1, SCR_W, C_BAR_BORDER);

  // Decorative dashes left
  tft.setTextColor(C_DECO);
  tft.setTextSize(2);
  tft.setCursor(4, INSULT_Y+13);
  tft.print("-=");

  // Decorative dashes right
  tft.setCursor(288, INSULT_Y+13);
  tft.print("=-");
}

// ── Insult typewriter text ───────────────────────────
void drawInsultText(const char* s, Mood m, uint8_t chars) {
  // Clear text area within insult bar
  tft.fillRect(34, INSULT_Y+1, 250, INSULT_H-2, C_INSULT_BG);

  char tmp[32]; strncpy(tmp, s, chars); tmp[chars] = '\0';
  uint16_t col = txtColor(m);
  int textSize = (strlen(s) > 12) ? 2 : 3;
  int charW    = textSize * 6;
  int x = max(34, (SCR_W - (int)strlen(s)*charW) / 2);
  int y = INSULT_Y + (INSULT_H - textSize*8)/2;

  tft.setTextSize(textSize);
  tft.setTextColor(col);
  tft.setCursor(x, y);
  tft.print(tmp);

  // Blinking cursor at end
  if (chars < strlen(s)) {
    tft.fillRect(x + chars*charW, y, charW-2, textSize*8, col);
  }
}

// ═══════════════════════════════════════════════════════
//  ── FACE DRAWING ─────────────────────────────────────
// ═══════════════════════════════════════════════════════

void drawFace(Mood m, bool blink, int8_t ox, int8_t oy) {
  int cx = FACE_CX + ox;
  int cy = FACE_CY + oy;

  // Face circle + shadow
  tft.fillCircle(cx+3, cy+4, 58, C_FACE_SHADOW);
  tft.fillCircle(cx, cy, 58, C_FACE_YELLOW);
  tft.drawCircle(cx, cy, 58, C_BLACK);
  tft.drawCircle(cx, cy, 59, C_BLACK);

  switch(m) {
    case CALM:    drawFaceCalm(cx, cy, blink);    break;
    case ANNOYED: drawFaceAnnoyed(cx, cy, blink); break;
    case FURIOUS: drawFaceFurious(cx, cy, blink); break;
    case SULKING: drawFaceSulking(cx, cy, blink); break;
  }
}

// ── CALM face: half-lidded, bored ───────────────────
void drawFaceCalm(int cx, int cy, bool blink) {
  if (blink) {
    // Closed — thin line
    tft.drawFastHLine(cx-35, cy-12, 24, C_BLACK);
    tft.drawFastHLine(cx+11, cy-12, 24, C_BLACK);
    return;
  }
  // Whites
  tft.fillRoundRect(cx-37, cy-20, 26, 16, 4, C_WHITE);
  tft.fillRoundRect(cx+11, cy-20, 26, 16, 4, C_WHITE);
  // Heavy lids (half-close)
  tft.fillRect(cx-37, cy-20, 26, 8, C_FACE_YELLOW);
  tft.fillRect(cx+11, cy-20, 26, 8, C_FACE_YELLOW);
  // Pupils
  tft.fillCircle(cx-24, cy-10, 5, C_BLACK);
  tft.fillCircle(cx+24, cy-10, 5, C_BLACK);
  tft.fillCircle(cx-22, cy-12, 2, C_WHITE);
  tft.fillCircle(cx+22, cy-12, 2, C_WHITE);
  // Eye outlines
  tft.drawRoundRect(cx-37, cy-20, 26, 16, 4, C_BLACK);
  tft.drawRoundRect(cx+11, cy-20, 26, 16, 4, C_BLACK);
  // Flat bored brows
  tft.drawFastHLine(cx-38, cy-28, 26, C_BLACK);
  tft.drawFastHLine(cx+12, cy-28, 26, C_BLACK);
  // Slight downward mouth
  tft.drawLine(cx-18, cy+24, cx, cy+22, C_BLACK);
  tft.drawLine(cx, cy+22, cx+18, cy+24, C_BLACK);
}

// ── ANNOYED face: side-eye, angled brows, frown ─────
void drawFaceAnnoyed(int cx, int cy, bool blink) {
  if (blink) {
    tft.drawFastHLine(cx-35, cy-12, 24, C_BLACK);
    tft.drawFastHLine(cx+11, cy-12, 24, C_BLACK);
    return;
  }
  // Whites
  tft.fillRoundRect(cx-37, cy-22, 28, 20, 5, C_WHITE);
  tft.fillRoundRect(cx+9,  cy-22, 28, 20, 5, C_WHITE);
  // Side-shifted pupils (looking left)
  tft.fillCircle(cx-34, cy-12, 7, C_BLACK);
  tft.fillCircle(cx+12, cy-12, 7, C_BLACK);
  tft.fillCircle(cx-36, cy-14, 3, C_WHITE);
  tft.fillCircle(cx+10, cy-14, 3, C_WHITE);
  // Eye outlines
  tft.drawRoundRect(cx-37, cy-22, 28, 20, 5, C_BLACK);
  tft.drawRoundRect(cx+9,  cy-22, 28, 20, 5, C_BLACK);
  // Angled brows (inner corner lower = irritated)
  tft.drawLine(cx-38, cy-34, cx-12, cy-26, C_BLACK);
  tft.drawLine(cx-38, cy-33, cx-12, cy-25, C_BLACK);
  tft.drawLine(cx+12, cy-26, cx+38, cy-34, C_BLACK);
  tft.drawLine(cx+12, cy-25, cx+38, cy-33, C_BLACK);
  // Frown
  tft.drawLine(cx-20, cy+26, cx-4,  cy+22, C_BLACK);
  tft.drawLine(cx+4,  cy+22, cx+20, cy+26, C_BLACK);
  tft.drawLine(cx-4,  cy+22, cx+4,  cy+22, C_BLACK);
}

// ── FURIOUS face: hard squint, V-brows, bared teeth ─
void drawFaceFurious(int cx, int cy, bool blink) {
  // Squinted eyes (rectangle slits)
  uint16_t eyeCol = blink ? C_FACE_YELLOW : C_BLACK;
  tft.fillRect(cx-38, cy-18, 24, blink?2:10, C_BLACK);
  tft.fillRect(cx+14, cy-18, 24, blink?2:10, C_BLACK);
  // Red angry iris hint
  if (!blink) {
    tft.fillRect(cx-34, cy-16, 16, 6, 0xF000);
    tft.fillRect(cx+18, cy-16, 16, 6, 0xF000);
  }
  // Sharp V eyebrows (double-thick)
  for (int t=0; t<2; t++) {
    tft.drawLine(cx-40, cy-36+t, cx-18, cy-24+t, C_BLACK);
    tft.drawLine(cx-18, cy-24+t, cx-12, cy-36+t, C_BLACK);
    tft.drawLine(cx+12, cy-36+t, cx+18, cy-24+t, C_BLACK);
    tft.drawLine(cx+18, cy-24+t, cx+40, cy-36+t, C_BLACK);
  }
  // Bared teeth
  tft.fillRect(cx-20, cy+18, 40, 14, C_WHITE);
  tft.drawRect(cx-20, cy+18, 40, 14, C_BLACK);
  for (int i=0; i<7; i++)
    tft.drawFastVLine(cx-20+i*6+1, cy+18, 14, C_BLACK);
  // Bottom lip
  tft.drawLine(cx-20, cy+32, cx+20, cy+32, C_BLACK);
  // Anger marks (##)
  tft.setTextColor(0xF800);
  tft.setTextSize(1);
  tft.setCursor(cx-56, cy-40); tft.print("##");
  tft.setCursor(cx+38, cy-40); tft.print("##");
}

// ── SULKING face: X eyes, resigned, silent ──────────
void drawFaceSulking(int cx, int cy, bool blink) {
  // X eyes
  uint16_t ec = blink ? C_FACE_YELLOW : C_BLACK;
  // Left X
  tft.drawLine(cx-38, cy-22, cx-16, cy-6,  ec);
  tft.drawLine(cx-16, cy-22, cx-38, cy-6,  ec);
  tft.drawLine(cx-37, cy-22, cx-15, cy-6,  ec);
  tft.drawLine(cx-15, cy-22, cx-37, cy-6,  ec);
  // Right X
  tft.drawLine(cx+16, cy-22, cx+38, cy-6,  ec);
  tft.drawLine(cx+38, cy-22, cx+16, cy-6,  ec);
  tft.drawLine(cx+17, cy-22, cx+39, cy-6,  ec);
  tft.drawLine(cx+39, cy-22, cx+17, cy-6,  ec);
  // Flat resigned mouth
  tft.drawFastHLine(cx-16, cy+24, 32, C_BLACK);
  tft.drawFastHLine(cx-16, cy+25, 32, C_BLACK);
  // Tear drop left eye
  tft.fillTriangle(cx-30, cy-4, cx-25, cy+6, cx-20, cy-4, 0x001F);
  // Raised eyebrow of indifference (single left brow up)
  tft.drawLine(cx-40, cy-32, cx-14, cy-28, C_BLACK);
  // Small sweat drop
  tft.fillCircle(cx+42, cy-8, 4, 0x001F);
  tft.fillCircle(cx+42, cy-8, 2, 0x07FF);
}

// ═══════════════════════════════════════════════════════
//  STAR PARTICLES (CALM background twinkle)
// ═══════════════════════════════════════════════════════
void initStars() {
  for (int i=0; i<NUM_STARS; i++) {
    stars[i].x = random(4, SCR_W-4);
    stars[i].y = FACE_Y + random(4, FACE_H-4);
    stars[i].phase = random(0, 8);
  }
}

void tickStars() {
  if (currentMood != CALM) return;
  uint16_t bg   = bgColor(CALM);
  uint16_t grid = gridColor(CALM);
  for (int i=0; i<NUM_STARS; i++) {
    // Erase old (redraw bg pixel)
    uint16_t col = ((stars[i].x%16==0)||(stars[i].y%16==0)) ? grid : bg;
    tft.drawPixel(stars[i].x, stars[i].y, col);
    tft.drawPixel(stars[i].x+1, stars[i].y, col);
    tft.drawPixel(stars[i].x, stars[i].y+1, col);
    // Advance phase
    stars[i].phase = (stars[i].phase+1)%8;
    // Pick colour based on phase
    uint16_t sc = (stars[i].phase < 4) ? 0x4208 : 0xC618;
    if (stars[i].phase == 2 || stars[i].phase == 6) sc = C_WHITE;
    // Draw
    tft.drawPixel(stars[i].x, stars[i].y, sc);
    tft.drawPixel(stars[i].x+1, stars[i].y, sc);
    tft.drawPixel(stars[i].x, stars[i].y+1, sc);
    // Occasionally drift
    if (stars[i].phase == 0) {
      stars[i].x = random(4, SCR_W-4);
      stars[i].y = FACE_Y + random(4, FACE_H-4);
    }
  }
}

// ═══════════════════════════════════════════════════════
//  BUZZER
// ═══════════════════════════════════════════════════════
void buzz(int freq, int dur) {
  tone(PIN_BUZZ, freq, dur);
  delay(dur+10);
  noTone(PIN_BUZZ);
}

void buzzPattern(Mood m) {
  switch(m) {
    case CALM:
      buzz(180, 70);
      break;
    case ANNOYED:
      buzz(440,90); delay(50); buzz(330,130);
      break;
    case FURIOUS:
      buzz(500,50); delay(25); buzz(620,50);
      delay(25);    buzz(750,50); delay(25); buzz(900,80);
      break;
    case SULKING:
      buzz(200, 500);
      break;
  }
}

// ═══════════════════════════════════════════════════════
//  SERVO ARMS
// ═══════════════════════════════════════════════════════
void armSet(int dL, int dR, int ms) {
  armL.write(dL);
  armR.write(180-dR);
  delay(ms);
}

void moveArms(Mood m) {
  switch(m) {
    case CALM: break;
    case ANNOYED:
      armSet(110,110,200); delay(80); armSet(90,90,300);
      break;
    case FURIOUS:
      for(int i=0;i<4;i++) {
        armSet(135,90,90); armSet(90,135,90);
      }
      armSet(90,90,200);
      break;
    case SULKING:
      armSet(145,145,600);
      break;
  }
}

// ═══════════════════════════════════════════════════════
//  LED CONTROL
// ═══════════════════════════════════════════════════════
void setLED(Mood m) {
  digitalWrite(PIN_LED_CALM,    LOW);
  digitalWrite(PIN_LED_ANNOYED, LOW);
  digitalWrite(PIN_LED_FURIOUS, LOW);
  ledBlinkState = false;
  switch(m) {
    case CALM:    digitalWrite(PIN_LED_CALM,    HIGH); break;
    case ANNOYED: digitalWrite(PIN_LED_ANNOYED, HIGH); break;
    case FURIOUS: break;   // loop handles blink
    case SULKING: break;   // loop handles blink
  }
}
