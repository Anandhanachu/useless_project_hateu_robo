// ΓòöΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòù
// Γòæ      HateBot ΓÇö Firmware v3  |  Procedural Animation Engine  Γòæ
// Γòæ  ESP32 NodeMCU + ILI9341 320├ù240 + MPU6050 + 2├ùSG90        Γòæ
// ΓòÜΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓò¥
//
//  ZERO bitmaps ΓÇö 100% procedural drawing.
//  All animations are math (sin/cos/lerp) + Adafruit_GFX primitives.
//  Compiled app fits well under 400KB ΓÇö safe for 4MB NodeMCU flash.
//
//  SCREEN LAYOUT (landscape 320├ù240)
//  ΓöîΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÉ y=0
//  Γöé  MOOD: FURIOUS       [ΓûêΓûêΓûêΓûêΓûêΓûêΓûêΓûêΓûêΓûê]  85%  ΓÖÑbeat   Γöé y=0ΓÇÑ24
//  Γö£ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöñ y=25
//  Γöé  ~~grid bg~~                                    Γöé
//  Γöé          Γò¡ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓò«                           Γöé
//  Γöé   steam  Γöé  (face)  Γöé  steam    <effects>       Γöé y=25ΓÇÑ194
//  Γöé   wisps  Γò░ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓò»                           Γöé
//  Γö£ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöñ y=195
//  Γöé  -=  "STOP. TOUCHING. ME."  =-                  Γöé y=195ΓÇÑ239
//  ΓööΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÿ

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Servo.h>
#include <MPU6050.h>
#include <math.h>

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  PINS
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
#define TFT_CS   0
#define TFT_DC   2
#define TFT_RST  16
#define IMU_SDA  4
#define IMU_SCL  5
#define ARM_L    12
#define ARM_R    15
#define BUZZ     1
#define LED_G    3

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  SCREEN CONSTANTS
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
#define SW      160
#define SH      128
#define TB_H    14          // top bar height
#define IB_Y    104         // insult bar top
#define FA_Y    TB_H        // face area top
#define FA_H    (IB_Y-TB_H) // face area height = 170
#define FCX     160         // face center X
#define FCY     (TB_H + FA_H/2)   // face center Y = 110
#define FR      28          // face base radius

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  COLOURS  (RGB565)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
#define C_BLK   0x0000
#define C_WHT   0xFFFF
#define C_FACE  0xFEA0   // warm yellow
#define C_SHAD  0xB580   // face shadow
#define C_BLUSH 0xF810   // cheek blush
#define C_TEAR  0x035F   // tear blue
#define C_TRHGL 0x07FF   // tear highlight
#define C_STEAM 0xBDD7   // steam grey-white
#define C_VEIN  0xF800   // forehead vein red
#define C_ZZZ   0x07E0   // calm ZZZ green-teal
#define C_LGTN  0xFFE0   // lightning yellow

// BG per mood
#define BG_CLM  0x0228
#define BG_ANN  0x2100
#define BG_FUR  0x3800
#define BG_SUL  0x100A

// Grid per mood
#define GR_CLM  0x0430
#define GR_ANN  0x3180
#define GR_FUR  0x5000
#define GR_SUL  0x180C

// Rage colours
#define RC_CLM  0x07E0
#define RC_ANN  0xFD20
#define RC_FUR  0xF800
#define RC_SUL  0x780F

// Text colours
#define TC_CLM  0x67FE
#define TC_ANN  0xFD20
#define TC_FUR  0xF81F
#define TC_SUL  0x8C71

// Bar chrome
#define C_BARBG 0x0841
#define C_BRBRD 0x4208

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  MOOD STATE MACHINE
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
enum Mood { CALM=0, ANNOYED=1, FURIOUS=2, SULKING=3 };
Mood   mood          = CALM;
int    furiousHits   = 0;
#define SULK_AT       3
unsigned long quietMs = 0;
unsigned long lastEvt = 0;
#define T_ANN_CLM   12000UL
#define T_FUR_ANN   10000UL
#define T_SUL_CLM   30000UL

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  FACE PARAMETER SYSTEM  (lerped every frame)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
struct FP {
  float eyeOpen;    // 0=closed .. 1=fully open
  float browAngle;  // 0=flat, +ve=angry V, -ve=sad arch
  float browH;      // eyebrow vertical offset (px)
  float pupilX;     // -1=far left .. +1=far right
  float pupilY;     // -1=up .. +1=down
  float mouth;      // -1=big frown .. 0=flat .. +1=smile
};

// Current (lerped toward target)
FP cur  = {0.8f, 0.0f, 0.0f, 0.0f, 0.0f, -0.1f};

// Targets per mood
const FP tgt[4] = {
//  eyeO  browA  browH  pupX   pupY   mouth
  { 0.55f,  0.0f, -2.0f,  0.0f,  0.0f, -0.15f }, // CALM
  { 0.85f, 22.0f, -5.0f, -0.75f, 0.0f, -0.55f }, // ANNOYED
  { 0.30f, 40.0f,-10.0f,  0.0f,  0.2f, -0.85f }, // FURIOUS
  { 0.90f,  5.0f,  8.0f,  0.0f,  0.3f, -0.05f }, // SULKING
};

inline float lerp(float a, float b, float t) { return a + (b-a)*t; }
#define LERP_SPEED  0.12f   // per frame at 30Hz

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  ANIMATION STATE
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
uint32_t frame       = 0;    // global frame counter
uint32_t loopMs      = 33;   // ~30 Hz

// Eye blink
unsigned long blinkAt  = 0;
bool          blinking = false;
unsigned long blinkMs  = 0;
#define BLINK_DUR  110UL

// Typewriter
char          insultBuf[34] = "";
uint8_t       revealed = 0;
unsigned long twMs     = 0;
#define TW_SPD  52UL

// Rage meter heartbeat
uint8_t  ragePct     = 10;   // displayed value (animated)
uint8_t  rageTarget  = 10;
unsigned long rageMs = 0;
#define RAGE_STEP  25UL
bool     hbFlash     = false;
unsigned long hbMs   = 0;
#define HB_INTERVAL 500UL

// Screen shake  (FURIOUS)
int8_t shX=0, shY=0;
unsigned long shakeMs = 0;
#define SHAKE_SPD 70UL

// ZZZ particles  (CALM)
struct Zzz { int16_t x, y; uint8_t alpha; };
Zzz zzzP[3];
unsigned long zzzMs = 0;
#define ZZZ_SPD 200UL

// Steam wisps  (FURIOUS)
struct Steam { int16_t y; uint8_t r; bool active; };
Steam stmL[4], stmR[4];
unsigned long stmMs = 0;
#define STM_SPD 120UL

// Tear drop  (SULKING)
int16_t tearY = 0;
bool    tearActive = false;
unsigned long tearMs = 0;
#define TEAR_SPD 80UL

// Sweat bead  (SULKING/ANNOYED)
int16_t sweatY = 0;
unsigned long sweatMs = 0;

// Pupil jitter  (ANNOYED)
float pjX=0, pjY=0;
unsigned long pjMs = 0;

// Border lightning  (FURIOUS)
unsigned long lgtMs = 0;
bool lgtOn = false;
#define LGT_SPD 250UL

// Eyebrow twitch  (ANNOYED)
float browTwitch = 0.0f;
unsigned long btMs  = 0;
#define BT_SPD 60UL

// LED blink
unsigned long ledMs = 0;
bool ledBlink = false;

// Mood change flash
unsigned long flashMs = 0;
bool flashing = false;
uint16_t flashCol = 0;

// Idle arm
unsigned long idleArmMs = 0;
#define IDLE_ARM 9000UL

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  STRING POOLS (PROGMEM saves RAM)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
const char s_c0[] PROGMEM = "Oh. It's you.";
const char s_c1[] PROGMEM = "Still here?";
const char s_c2[] PROGMEM = "...cool.";
const char s_c3[] PROGMEM = "Ugh.";
const char s_c4[] PROGMEM = "Whatever.";
const char s_c5[] PROGMEM = "Fine. Hi.";
const char* const pool_calm[] PROGMEM = {s_c0,s_c1,s_c2,s_c3,s_c4,s_c5};

const char s_a0[] PROGMEM = "Seriously?";
const char s_a1[] PROGMEM = "I'm busy existing.";
const char s_a2[] PROGMEM = "Please stop.";
const char s_a3[] PROGMEM = "You again.";
const char s_a4[] PROGMEM = "Not now.";
const char s_a5[] PROGMEM = "Oh come ON.";
const char s_a6[] PROGMEM = "Excuse you.";
const char* const pool_ann[] PROGMEM  = {s_a0,s_a1,s_a2,s_a3,s_a4,s_a5,s_a6};

const char s_f0[] PROGMEM = "HOW DARE YOU.";
const char s_f1[] PROGMEM = "I HAVE HAD IT.";
const char s_f2[] PROGMEM = "STOP.TOUCHING.ME.";
const char s_f3[] PROGMEM = "PERSONAL SPACE!";
const char s_f4[] PROGMEM = "ABSOLUTELY NOT.";
const char s_f5[] PROGMEM = "GET. AWAY.";
const char* const pool_fur[] PROGMEM  = {s_f0,s_f1,s_f2,s_f3,s_f4,s_f5};

const char s_s0[] PROGMEM = "I'm done.";
const char s_s1[] PROGMEM = "Talk to the hand.";
const char s_s2[] PROGMEM = "...";
const char s_s3[] PROGMEM = "Goodbye.";
const char* const pool_sul[] PROGMEM  = {s_s0,s_s1,s_s2,s_s3};

const uint8_t poolSz[4] = {6, 7, 6, 4};
uint8_t insultIdx[4]    = {0, 0, 0, 0};
const uint8_t rTgt[4]   = {10, 45, 85, 28}; // rage % per mood

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  COLOUR HELPERS
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
uint16_t bgC(Mood m)  { const uint16_t t[]={BG_CLM,BG_ANN,BG_FUR,BG_SUL}; return t[m]; }
uint16_t grC(Mood m)  { const uint16_t t[]={GR_CLM,GR_ANN,GR_FUR,GR_SUL}; return t[m]; }
uint16_t rcC(Mood m)  { const uint16_t t[]={RC_CLM,RC_ANN,RC_FUR,RC_SUL}; return t[m]; }
uint16_t tcC(Mood m)  { const uint16_t t[]={TC_CLM,TC_ANN,TC_FUR,TC_SUL}; return t[m]; }
const char* moodStr(Mood m){ const char* t[]={"CALM","ANNOYED","FURIOUS","SULKING"}; return t[m]; }

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  HARDWARE
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);
MPU6050 imu;
Servo   sL, sR;

float bAx,bAy,bAz;
float prevHP=0;
bool  inPick=false;
unsigned long pickMs=0;
#define IMU_CAL  150
#define SH_VAR   7500.0f
#define TILT_DEG 14.0f
#define PICK_G   0.28f
#define PICK_MS  480
#define TAP_HP   15000.0f

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  FORWARD DECLARATIONS
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void calIMU();
int  evIMU();
void escalate(int w);
void deEscalate();
void changeMood(Mood m);
void react(Mood m);
void startInsult(Mood m);
void fetchInsult(Mood m, char* buf, uint8_t sz);

// Display
void drawAll();
void drawBG(int8_t ox=0, int8_t oy=0);
void drawTopBar();
void drawRage(uint8_t pct, bool hb=false);
void drawInsultBar();
void drawInsultText(uint8_t chars);
void drawFace(int8_t ox=0, int8_t oy=0);
void drawEyes(int cx, int cy);
void drawBrows(int cx, int cy);
void drawMouth(int cx, int cy);
void drawFxCalm(int cx, int cy);
void drawFxAnnoyed(int cx, int cy);
void drawFxFurious(int cx, int cy);
void drawFxSulking(int cx, int cy);
void drawLightning();
void flashScreen(uint16_t col, int ms);

// Helpers
void drawMouthCurve(int cx, int cy, float curve, uint16_t col, int thick=2);
void drawThickLine(int x0,int y0,int x1,int y1,int t,uint16_t c);
void armGo(int l, int r, int ms);
void buzz(int f, int d);
void buzzPat(Mood m);
void setLED(Mood m);
void initSteam();
void initZzz();

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  SETUP
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void setup() {
  Serial.begin(115200);

  // TFT boot
  
  tft.initR(INITR_BLACKTAB); tft.setSPISpeed(40000000); tft.setRotation(1);
  tft.fillScreen(C_BLK);

  // Boot splash ΓÇö minimal, fast
  tft.setTextSize(1); tft.setTextColor(TC_CLM);
  tft.setCursor(28, 44); tft.print("HateBot v3.0");
  tft.setTextSize(1); tft.setTextColor(TC_SUL);
  tft.setCursor(38, 57); tft.print("Procedural Edition");
  tft.setCursor(44, 70); tft.print("calibrating...");
  buzz(220,70); delay(80); buzz(330,70); delay(80); buzz(520,120);

  // Hardware
  pinMode(LED_G,OUTPUT); 
  pinMode(BUZZ,OUTPUT);
  sL.attach(ARM_L); sR.attach(ARM_R);
  armGo(90,90,500);

  Wire.begin(IMU_SDA,IMU_SCL);
  imu.initialize();
  if(!imu.testConnection()) Serial.println("[WARN] IMU not found");
  calIMU();

  // Init animations
  initZzz();
  initSteam();
  tearY  = FCY - FR/2;
  sweatY = FCY - FR/3;

  // First draw
  rageTarget = rTgt[CALM];
  drawAll();
  setLED(CALM);
  startInsult(CALM);

  blinkAt    = millis() + random(3000,6000);
  lastEvt    = millis();
  idleArmMs  = millis();
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  MAIN LOOP  (~30 Hz)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void loop() {
  unsigned long now = millis();
  frame++;

  // ΓöÇΓöÇ IMU ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  int ev = evIMU();
  if(ev > 0){ escalate(ev); lastEvt = now; }
  deEscalate();
  quietMs = now - lastEvt;

  // ΓöÇΓöÇ Lerp face params ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  const FP& t = tgt[(int)mood];
  cur.eyeOpen  = lerp(cur.eyeOpen,  t.eyeOpen,  LERP_SPEED);
  cur.browAngle= lerp(cur.browAngle,t.browAngle, LERP_SPEED);
  cur.browH    = lerp(cur.browH,    t.browH,     LERP_SPEED);
  cur.pupilX   = lerp(cur.pupilX,   t.pupilX,    LERP_SPEED);
  cur.pupilY   = lerp(cur.pupilY,   t.pupilY,    LERP_SPEED);
  cur.mouth    = lerp(cur.mouth,    t.mouth,      LERP_SPEED);

  // ΓöÇΓöÇ Screen flash on mood change ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  if(flashing && now - flashMs < 80) {
    tft.fillRect(0, FA_Y, SW, FA_H, flashCol);
    return;
  } else if(flashing) {
    flashing = false;
    drawAll();
    startInsult(mood);
  }

  // ΓöÇΓöÇ Shake (FURIOUS) ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  if(mood==FURIOUS && now-shakeMs > SHAKE_SPD) {
    shX = random(-3,4); shY = random(-3,4);
    shakeMs = now;
  } else if(mood!=FURIOUS){ shX=0; shY=0; }

  // ΓöÇΓöÇ CALM breathing ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  float breathR = FR;
  if(mood==CALM) breathR = FR + sinf(frame*0.08f)*2.0f;

  // ΓöÇΓöÇ Draw face area ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  drawBG(shX, shY);

  // Effects behind face
  if(mood==FURIOUS) drawFxFurious(FCX+shX, FCY+shY);

  // Draw face circle
  {
    int cx = FCX+shX, cy = FCY+shY;
    int r  = (int)breathR;
    tft.fillCircle(cx+3, cy+4, r, C_SHAD);    // shadow
    tft.fillCircle(cx,   cy,   r, C_FACE);    // face
    tft.drawCircle(cx,   cy,   r, C_BLK);
    tft.drawCircle(cx,   cy, r+1, C_BLK);

    drawBrows(cx,cy);
    drawEyes(cx,cy);
    drawMouth(cx,cy);

    // Mood-specific overlaid effects
    if(mood==CALM)    drawFxCalm(cx,cy);
    if(mood==ANNOYED) drawFxAnnoyed(cx,cy);
    if(mood==SULKING) drawFxSulking(cx,cy);
  }

  // Border lightning (FURIOUS, in the face area)
  if(mood==FURIOUS && now-lgtMs > LGT_SPD) {
    lgtOn = !lgtOn; lgtMs = now;
    if(lgtOn) drawLightning();
  }

  // ΓöÇΓöÇ Top bar updates ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  // Rage meter step
  if(ragePct != rageTarget && now-rageMs > RAGE_STEP) {
    ragePct += (ragePct < rageTarget) ? 1 : -1;
    rageMs = now;
  }
  // Heartbeat flash
  if(mood==FURIOUS && now-hbMs > HB_INTERVAL) {
    hbFlash = !hbFlash; hbMs = now;
  } else if(mood!=FURIOUS) hbFlash=false;
  drawRage(ragePct, hbFlash);

  // ΓöÇΓöÇ Blink ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  if(!blinking && now >= blinkAt) {
    blinking = true; blinkMs = now;
    blinkAt  = now + random(3000,7000);
  }
  if(blinking && now-blinkMs > BLINK_DUR) {
    blinking = false;
  }

  // ΓöÇΓöÇ Typewriter ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  if(revealed < strlen(insultBuf) && now-twMs > TW_SPD) {
    revealed++; twMs = now;
    drawInsultText(revealed);
  }

  // ΓöÇΓöÇ LED blink ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  if((mood==FURIOUS||mood==SULKING) && now-ledMs>450) {
    ledBlink=!ledBlink; digitalWrite(LED_G, ledBlink?HIGH:LOW);
    ledMs=now;
  }

  // ΓöÇΓöÇ Idle arm twitch ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
  if(mood==CALM && now-idleArmMs > IDLE_ARM) {
    armGo(96,90,300); delay(200); armGo(90,90,300);
    buzz(180,60); idleArmMs=now;
  }

  delay(loopMs);
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  BACKGROUND + GRID
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawBG(int8_t ox, int8_t oy) {
  uint16_t bg = bgC(mood), gr = grC(mood);
  tft.fillRect(0, FA_Y, SW, FA_H, bg);
  for(int x=8;x<SW;x+=8) tft.drawFastVLine(x+ox, FA_Y, FA_H, gr);
  for(int y=FA_Y+8;y<IB_Y;y+=8) tft.drawFastHLine(0, y+oy, SW, gr);
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  FULL SCREEN REDRAW
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawAll() {
  tft.fillScreen(C_BLK);
  drawBG();
  drawTopBar();
  drawInsultBar();
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  TOP BAR
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawTopBar() {
  tft.fillRect(0,0,SW,TB_H,C_BARBG);
  tft.drawFastHLine(0,TB_H-1,SW,C_BRBRD);
  tft.setTextSize(1); tft.setTextColor(C_WHT);
  tft.setCursor(2,2); tft.print("MOOD:");
  tft.setTextColor(tcC(mood));
  tft.setCursor(34,2); tft.print(moodStr(mood));
  drawRage(ragePct, false);
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  RAGE METER  (10 segments + heartbeat pulse)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawRage(uint8_t pct, bool hb) {
  int bx=90, by=2, bw=48, bh=8;
  tft.drawRect(bx-1,by-1,bw+2,bh+2,C_BRBRD);
  tft.fillRect(bx,by,bw,bh,C_BARBG);

  uint16_t rc = rcC(mood);
  uint8_t segs = (pct*10)/100 + (hb?1:0);
  segs = min(segs,(uint8_t)10);
  for(int i=0;i<10;i++) {
    if(i<segs) tft.fillRect(bx+i*4+1,by+1,3,bh-2,rc);
  }
  // % text
  tft.fillRect(bx+bw+2,by,26,bh,C_BARBG);
  tft.setTextSize(1); tft.setTextColor(rc);
  tft.setCursor(bx+bw+1,by+1);
  char pb[5]; snprintf(pb,5,"%3d%%",pct); tft.print(pb);
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  INSULT BAR  (chrome only)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawInsultBar() {
  tft.fillRect(0,IB_Y,SW,SH-IB_Y,C_BARBG);
  tft.drawFastHLine(0,IB_Y,SW,C_BRBRD);
  tft.drawFastHLine(0,SH-1,SW,C_BRBRD);
  // Decorative arrows
  tft.setTextColor(C_BRBRD); tft.setTextSize(1);
  tft.setCursor(2,IB_Y+6);  tft.print("-=");
  tft.setCursor(140,IB_Y+6);tft.print("=-");
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  TYPEWRITER  TEXT
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawInsultText(uint8_t chars) {
  tft.fillRect(17,IB_Y+2,124,SH-IB_Y-4,C_BARBG);
  char tmp[34]; strncpy(tmp,insultBuf,chars); tmp[chars]='\0';
  int sz = (strlen(insultBuf)>13)?1:2;
  int cw = sz*6;
  int x  = max(17,(SW-(int)strlen(insultBuf)*cw)/2);
  int y  = IB_Y+(SH-IB_Y-sz*8)/2;
  tft.setTextSize(sz); tft.setTextColor(tcC(mood));
  tft.setCursor(x,y); tft.print(tmp);
  // Blinking block cursor
  if(chars < strlen(insultBuf))
    tft.fillRect(x+chars*cw, y, cw-1, sz*8, tcC(mood));
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  EYES  (driven by cur.eyeOpen, cur.pupilX/Y)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawEyes(int cx, int cy) {
  int eo  = (int)(cur.eyeOpen * 8.0f); // 0..16 = eye height
  eo = constrain(eo,0,16);

  // Sulking = X eyes override
  if(mood==SULKING && cur.eyeOpen > 0.7f) {
    uint16_t xc = C_BLK;
    drawThickLine(cx-18,cy-11,cx-8,cy-3,1,xc);
    drawThickLine(cx-8,cy-11,cx-18,cy-3,1,xc);
    drawThickLine(cx+8,cy-11,cx+18,cy-3,1,xc);
    drawThickLine(cx+18,cy-11,cx+8,cy-3,1,xc);
    return;
  }

  // Blink overrides eyeOpen
  int actualEO = blinking ? 1 : eo;

  // Left eye
  int lx=cx-14, rx=cx+14;
  int ey=cy-7;
  int ew=12, eh=max(1,actualEO);

  // White sclera
  tft.fillRoundRect(lx-ew/2, ey-eh/2, ew, eh, eh/3, C_WHT);
  tft.fillRoundRect(rx-ew/2, ey-eh/2, ew, eh, eh/3, C_WHT);

  if(actualEO > 3) {
    // Pupils ΓÇö shifted by pupilX/Y
    int pxOff = (int)(cur.pupilX * 4.0f) + (int)(pjX);
    int pyOff = (int)(cur.pupilY * 2.0f) + (int)(pjY);
    int pr    = (mood==FURIOUS) ? 2 : 3;   // FURIOUS = angry small pupils
    // Throb for FURIOUS
    if(mood==FURIOUS) pr = 2 + (int)(sinf(frame*0.18f)*1.0f);

    tft.fillCircle(lx+pxOff, ey+pyOff, pr, C_BLK);
    tft.fillCircle(rx+pxOff, ey+pyOff, pr, C_BLK);
    // Highlight
    tft.fillCircle(lx+pxOff-2, ey+pyOff-2, 2, C_WHT);
    tft.fillCircle(rx+pxOff-2, ey+pyOff-2, 2, C_WHT);
  }

  // Eye outlines
  tft.drawRoundRect(lx-ew/2, ey-eh/2, ew, eh, eh/3, C_BLK);
  tft.drawRoundRect(rx-ew/2, ey-eh/2, ew, eh, eh/3, C_BLK);

  // Eyelid (top, mood=CALM half-close)
  if(mood==CALM && actualEO > 1) {
    int lidH = actualEO/3;
    tft.fillRect(lx-ew/2, ey-eh/2, ew, lidH, C_FACE);
    tft.fillRect(rx-ew/2, ey-eh/2, ew, lidH, C_FACE);
  }
  // ANNOYED lower-lid shadow
  if(mood==ANNOYED) {
    tft.fillRect(lx-ew/2, ey+eh/2-3, ew, 3, C_SHAD);
    tft.fillRect(rx-ew/2, ey+eh/2-3, ew, 3, C_SHAD);
  }
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  EYEBROWS  (browAngle + browH + browTwitch)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawBrows(int cx, int cy) {
  float ang = cur.browAngle + browTwitch;
  float bh  = cur.browH;
  int   by  = cy - 14 + (int)(bh/2);
  int   rise = (int)(ang * 0.35f); // px rise for inner end

  // Left brow:  from outer-left going inward and up
  int lox=cx-19, liy=by-(rise/2), loy=by;
  int lix=cx-8;
  drawThickLine(lox,loy, lix,liy, 1, C_BLK);
  // V-tip for angry
  if(ang > 15.0f) {
    int tipX = lix-2, tipY = liy-3;
    drawThickLine(lix,liy, tipX,tipY, 1, C_BLK);
  }

  // Right brow: mirrored
  int rox=cx+19, riy=by-(rise/2), roy=by;
  int rix=cx+8;
  drawThickLine(rox,roy, rix,riy, 1, C_BLK);
  if(ang > 15.0f) {
    int tipX = rix+2, tipY = riy-3;
    drawThickLine(rix,riy, tipX,tipY, 1, C_BLK);
  }

  // Raised single-brow for SULKING indifference (left only)
  if(mood==SULKING) {
    drawThickLine(cx-19,by-4, cx-7,by-2, 1, C_BLK);
  }
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  MOUTH  (parametric curve)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawMouth(int cx, int cy) {
  drawMouthCurve(cx, cy, cur.mouth, C_BLK, 2);
  // Teeth for FURIOUS
  if(mood==FURIOUS && cur.mouth < -0.6f) {
    int my = cy+10;
    tft.fillRect(cx-9,my,18,6,C_WHT);
    tft.drawRect(cx-9,my,18,6,C_BLK);
    for(int i=0;i<6;i++) tft.drawFastVLine(cx-9+i*3,my,6,C_BLK);
  }
}

void drawMouthCurve(int cx, int cy, float curve, uint16_t col, int thick) {
  // Draw mouth as 10 line segments following a parabola
  int segments = 10;
  int mw = 10; // half-width
  int mpy= cy+11;
  int depth = (int)(curve * 6.0f); // +ve = smile, -ve = frown

  int px = cx - mw, py = mpy;
  for(int i=1;i<=segments;i++) {
    float t  = (float)i / segments;
    float ft = (2.0f*t - 1.0f);   // -1 to +1
    int nx = cx - mw + (int)(t*2*mw);
    int ny = mpy - (int)((1.0f - ft*ft) * depth);
    if(thick<=1) tft.drawLine(px,py,nx,ny,col);
    else drawThickLine(px,py,nx,ny,thick,col);
    px=nx; py=ny;
  }
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  CALM FX: floating ZZZ
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void initZzz() {
  for(int i=0;i<3;i++) {
    zzzP[i].x = FCX + 21 + i*5;
    zzzP[i].y = FCY - i*9;
    zzzP[i].alpha = i;
  }
}

void drawFxCalm(int cx, int cy) {
  unsigned long now = millis();
  if(now-zzzMs < ZZZ_SPD) return;
  zzzMs = now;
  // Erase old positions (repaint with face color approximation)
  for(int i=0;i<3;i++) {
    tft.fillRect(zzzP[i].x-1, zzzP[i].y-1, 5, 6, bgC(CALM));
  }
  // Drift up
  for(int i=0;i<3;i++) {
    zzzP[i].y -= 1;
    zzzP[i].x += (i%2==0)?1:-1; // slight drift
    if(zzzP[i].y < FA_Y+4) {
      zzzP[i].y = FCY - FR + 5;
      zzzP[i].x = FCX + 22 + i*4;
    }
  }
  // Draw Z letters (size decreasing with height)
  for(int i=0;i<3;i++) {
    int x=zzzP[i].x, y=zzzP[i].y;
    int sz = 3-i;
    if(sz<1)sz=1;
    tft.setTextSize(sz); tft.setTextColor(C_ZZZ);
    tft.setCursor(x,y); tft.print("Z");
  }
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  ANNOYED FX: eyebrow twitch + pupil jitter
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawFxAnnoyed(int cx, int cy) {
  unsigned long now = millis();
  // Eyebrow twitch spike every ~1.5s
  if(now-btMs > 1500UL + random(500)) {
    browTwitch = random(-6,7); btMs=now;
  } else if(now-btMs > 200) browTwitch*=0.7f;

  // Pupil jitter
  if(now-pjMs > 120) {
    pjX = random(-2,3)*0.5f;
    pjY = random(-1,2)*0.5f;
    pjMs=now;
  }
  // Cheek flush
  tft.fillCircle(cx-20, cy+6, 4, C_BLUSH);
  tft.fillCircle(cx+20, cy+6, 4, C_BLUSH);
  // Semi-transparent: re-blend with face color
  tft.fillCircle(cx-20, cy+6, 2, 0xF8C0);
  tft.fillCircle(cx+20, cy+6, 2, 0xF8C0);
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  FURIOUS FX: steam + forehead veins (behind face)
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void initSteam() {
  int bsy = FCY - 5;
  for(int i=0;i<4;i++) {
    stmL[i]={int16_t(bsy-i*7), uint8_t(3-i), i<3};
    stmR[i]={int16_t(bsy-i*7), uint8_t(3-i), i<3};
  }
}

void drawFxFurious(int cx, int cy) {
  unsigned long now = millis();

  // Forehead veins (drawn on face)
  int vy = cy - 18;
  tft.drawLine(cx-9,vy,   cx-5,vy+3,  C_VEIN);
  tft.drawLine(cx-5,vy+3, cx-7,vy+5,  C_VEIN);
  tft.drawLine(cx-7,vy+5, cx-4,vy+7,  C_VEIN);
  tft.drawLine(cx+9,vy,   cx+5,vy+3,  C_VEIN);
  tft.drawLine(cx+5,vy+3, cx+7,vy+5,  C_VEIN);
  tft.drawLine(cx+7,vy+5, cx+4,vy+7,  C_VEIN);

  // Steam wisps (behind face, so drawn before face circle in loop ΓÇö done in drawBG phase)
  if(now-stmMs < STM_SPD) return;
  stmMs=now;

  uint16_t bg = bgC(FURIOUS);
  for(int i=0;i<4;i++) {
    if(!stmL[i].active) continue;
    // Erase
    tft.fillCircle(cx-FR-2, stmL[i].y, stmL[i].r+1, bg);
    tft.fillCircle(cx+FR+2, stmR[i].y, stmR[i].r+1, bg);
    stmL[i].y -= 2;
    stmR[i].y -= 2;
    stmL[i].r  = max(1,stmL[i].r-(i==3?1:0));
    stmR[i].r  = max(1,stmR[i].r-(i==3?1:0));
    // Reset when off screen
    if(stmL[i].y < FA_Y) { stmL[i].y=FCY; stmL[i].r=5; }
    if(stmR[i].y < FA_Y) { stmR[i].y=FCY; stmR[i].r=5; }
    // Draw
    tft.fillCircle(cx-FR-2, stmL[i].y, stmL[i].r, C_STEAM);
    tft.fillCircle(cx+FR+2, stmR[i].y, stmR[i].r, C_STEAM);
  }
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  SULKING FX: falling teardrop + sweat bead
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawFxSulking(int cx, int cy) {
  unsigned long now = millis();

  // Tear drop (left eye)
  if(now-tearMs > TEAR_SPD) {
    // Erase
    tft.fillCircle(cx-27, tearY, 5, bgC(SULKING));
    tft.drawTriangle(cx-30,tearY+2, cx-27,tearY+10, cx-24,tearY+2, bgC(SULKING));
    tearY += 2;
    if(tearY > FCY+FR-10) tearY = FCY-12;
    tearMs=now;
  }
  // Draw tear
  tft.fillCircle(cx-27, tearY, 4, C_TEAR);
  tft.fillCircle(cx-28, tearY-1, 2, C_TRHGL);
  tft.fillTriangle(cx-30,tearY+2, cx-27,tearY+10, cx-24,tearY+2, C_TEAR);

  // Sweat bead (right side)
  if(now-sweatMs > 160) {
    tft.fillCircle(cx+42, sweatY, 4, bgC(SULKING));
    sweatY += 2;
    if(sweatY > FCY+FR-10) sweatY = FCY-20;
    sweatMs=now;
  }
  tft.fillCircle(cx+42, sweatY, 3, 0x035F);
  tft.fillCircle(cx+41, sweatY-1, 1, C_TRHGL);
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  FURIOUS: border lightning in face area
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawLightning() {
  // Random jagged line along top or bottom of face area
  int side = random(0,2);
  int y    = side ? IB_Y-4 : FA_Y+4;
  int x=0, px=0, py=y;
  for(;x<SW;x+=random(10,25)) {
    int ny = y + random(-8,9);
    tft.drawLine(px,py,x,ny,C_LGTN);
    px=x; py=ny;
  }
  tft.drawLine(px,py,SW,y,C_LGTN);
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  INSULT / MOOD CHANGE
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void fetchInsult(Mood m, char* buf, uint8_t sz) {
  const char* const* pool;
  switch(m){
    case CALM:    pool=pool_calm; break;
    case ANNOYED: pool=pool_ann;  break;
    case FURIOUS: pool=pool_fur;  break;
    default:      pool=pool_sul;  break;
  }
  const char* s = (const char*)pgm_read_ptr(&pool[insultIdx[(int)m] % poolSz[(int)m]]);
  insultIdx[(int)m]++;
  strncpy_P(buf, s, sz-1); buf[sz-1]='\0';
}

void startInsult(Mood m) {
  fetchInsult(m, insultBuf, sizeof(insultBuf));
  revealed=0; twMs=millis();
  tft.fillRect(17,IB_Y+2,124,SH-IB_Y-4,C_BARBG);
  drawInsultBar();
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  MOOD STATE MACHINE
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void escalate(int w) {
  if(mood==SULKING) return;
  if(mood==FURIOUS) { if(++furiousHits>=SULK_AT){ furiousHits=0; changeMood(SULKING); return; } }
  else furiousHits=0;
  Mood next=(Mood)min((int)mood+w,(int)FURIOUS);
  if(next!=mood) changeMood(next);
  else { buzzPat(mood); armGo(92,92,100); armGo(90,90,100); } // same-mood react
}

void deEscalate() {
  Mood prev=mood;
  switch(mood){
    case SULKING: if(quietMs>T_SUL_CLM){ furiousHits=0; mood=CALM; }    break;
    case FURIOUS: if(quietMs>T_FUR_ANN) mood=ANNOYED; break;
    case ANNOYED: if(quietMs>T_ANN_CLM) mood=CALM;    break;
    default: break;
  }
  if(mood!=prev){ rageTarget=rTgt[(int)mood]; changeMood(mood); }
}

void changeMood(Mood m) {
  mood=m;
  rageTarget=rTgt[(int)m];
  // Screen flash
  const uint16_t fc[]={TC_CLM,TC_ANN,0xF800,TC_SUL};
  flashCol=fc[(int)m]; flashing=true; flashMs=millis();
  // Update top bar label
  tft.fillRect(68,0,140,TB_H-1,C_BARBG);
  tft.setTextSize(1); tft.setTextColor(tcC(m));
  tft.setCursor(34,2); tft.print(moodStr(m));
  // Reactions
  react(m);
  setLED(m);
  startInsult(m);
  // Reset mode-specific state
  if(m==CALM)    { initZzz(); browTwitch=0; pjX=0; pjY=0; }
  if(m==FURIOUS) { initSteam(); }
  if(m==SULKING) { tearY=FCY-12; sweatY=FCY-20; }
}

void react(Mood m) { buzzPat(m); armGo(90,90,0); delay(50); moveArms(m); }

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  IMU
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void calIMU() {
  long sx=0,sy=0,sz=0;
  for(int i=0;i<IMU_CAL;i++){
    int16_t ax,ay,az,gx,gy,gz;
    imu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
    sx+=ax;sy+=ay;sz+=az; delay(7);
  }
  bAx=(float)sx/IMU_CAL; bAy=(float)sy/IMU_CAL; bAz=(float)sz/IMU_CAL;
  Serial.printf("[IMU] Base Ax=%.0f Ay=%.0f Az=%.0f\n",bAx,bAy,bAz);
}

int evIMU() {
  int16_t ax,ay,az,gx,gy,gz;
  imu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
  float dAx=ax-bAx, dAy=ay-bAy, dAz=az-bAz;
  float mag=sqrtf(dAx*dAx+dAy*dAy+dAz*dAz);

  float hp=fabsf(mag-prevHP); prevHP=mag;
  if(hp>TAP_HP) return 1;

  static float win[8]={0}; static uint8_t wi=0;
  win[wi++%8]=mag;
  float mn=0; for(int i=0;i<8;i++) mn+=win[i]; mn/=8.0f;
  float vr=0; for(int i=0;i<8;i++) vr+=(win[i]-mn)*(win[i]-mn); vr/=8.0f;
  if(vr>SH_VAR) return 2;

  float zg=fabsf(dAz)/16384.0f;
  unsigned long now=millis();
  if(zg>PICK_G){ if(!inPick){inPick=true;pickMs=now;} else if(now-pickMs>PICK_MS){inPick=false;return 3;} }
  else inPick=false;

  float tilt=atan2f(dAy,dAz)*180.0f/PI;
  if(fabsf(tilt)>TILT_DEG) return 1;
  return 0;
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
//  HARDWARE HELPERS
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
void drawThickLine(int x0,int y0,int x1,int y1,int t,uint16_t c) {
  for(int i=-t/2;i<=t/2;i++) tft.drawLine(x0,y0+i,x1,y1+i,c);
}

void armGo(int l, int r, int ms) {
  sL.write(l); sR.write(180-r);
  if(ms>0) delay(ms);
}

void moveArms(Mood m) {
  switch(m){
    case CALM:    break;
    case ANNOYED: armGo(110,110,200); delay(80); armGo(90,90,280); break;
    case FURIOUS:
      for(int i=0;i<4;i++){armGo(140,90,80);armGo(90,140,80);}
      armGo(90,90,180); break;
    case SULKING: armGo(145,145,600); break;
  }
}

void buzz(int f,int d){ tone(BUZZ,f,d); delay(d+8); noTone(BUZZ); }

void buzzPat(Mood m){
  switch(m){
    case CALM:    buzz(180,65); break;
    case ANNOYED: buzz(440,85); delay(50); buzz(330,120); break;
    case FURIOUS:
      buzz(480,45);delay(20);buzz(600,45);delay(20);
      buzz(750,45);delay(20);buzz(920,90); break;
    case SULKING: buzz(200,480); break;
  }
}

void setLED(Mood m){
  digitalWrite(LED_G,LOW); digitalWrite(LED_Y,LOW); digitalWrite(LED_G,LOW);
  ledBlink=false;
  switch(m){
    case CALM:    digitalWrite(LED_G,HIGH); break;
    case ANNOYED: digitalWrite(LED_Y,HIGH); break;
    default: break; // FURIOUS/SULKING blinked in loop
  }
}
