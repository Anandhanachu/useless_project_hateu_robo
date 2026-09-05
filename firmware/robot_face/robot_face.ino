// ============================================================
//  ROBOT FACE ANIMATOR  v1.0
//  ESP8266 NodeMCU + Adafruit ST7735  128x160 -> landscape 160x128
//
//  Wiring:
//    TFT CS  -> D1 / GPIO5
//    TFT DC  -> D2 / GPIO4
//    TFT RST -> D0 / GPIO16
//    TFT SCK -> D5 / GPIO14  (hardware SPI)
//    TFT MOSI-> D7 / GPIO13  (hardware SPI)
//    VCC -> 5V  |  GND -> GND
//
//  Required libraries:  Adafruit GFX  +  Adafruit ST7735/ST7789
// ============================================================

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: INCLUDES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <math.h>

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: TFT CONFIGURATION
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define TFT_CS   5    // D1 / GPIO5
#define TFT_DC   4    // D2 / GPIO4
#define TFT_RST  16   // D0 / GPIO16

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define SW          160
#define SH          128
#define TARGET_FPS  30
#define FRAME_MS    (1000 / TARGET_FPS)

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: COLOUR PALETTE  (RGB565)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define C_BG        0x000D   // near-black background
#define C_FACE      0x2124   // dark slate face
#define C_FACE_HL   0x318C   // lighter face accent (mouth interior)
#define C_WHITE     0xFFFF
#define C_BLACK     0x0000
#define C_IRIS_DIM  0x035B   // dim iris - sad/angry/sleepy
#define C_IRIS      0x07BF   // normal cyan-blue iris
#define C_IRIS_BRT  0x07FF   // bright iris - happy/excited
#define C_BROW      0xE73C   // eyebrow colour
#define C_MOUTH     0xE73C   // mouth colour
#define C_CHEEK     0xF9AC   // blush pink
#define C_OUTLINE   0x0000   // black outline

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: FACE GEOMETRY CONSTANTS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Face background rounded-rect
#define FBG_X   7
#define FBG_Y   5
#define FBG_W   146
#define FBG_H   118
#define FBG_R   22

// Eyes base parameters
#define BASE_EW  26    // eye base width
#define BASE_EH  20    // eye base max-open height
#define EL_CX    53    // left  eye center X
#define ER_CX    107   // right eye center X
#define E_CY     54    // eye center Y

// Iris / pupil
#define BASE_IR  7     // iris radius
#define BASE_PR  4     // pupil radius

// Eyebrows
#define BL_CX    53    // left  brow center X
#define BR_CX    107   // right brow center X
#define B_CY     31    // brow base Y
#define B_HW     11    // brow half-width each side

// Mouth
#define M_CX     80    // mouth center X
#define M_CY     93    // mouth center Y
#define M_HW_B   26    // mouth base half-width
#define M_AMP    11    // max curve amplitude

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: FacePose STRUCTURE + SPRING SYSTEM
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
struct Sp {
  float v = 0, vel = 0, tgt = 0;
  void to(float t)             { tgt = t; }
  void snap(float x)           { v = x; vel = 0; tgt = x; }
  void tick(float k, float d)  { vel = vel*d + (tgt-v)*k; v += vel; }
};

// All animated face parameters
Sp eyeL_op, eyeR_op;   // openness 0-1
Sp eyeL_w,  eyeR_w;    // width px
Sp eyeL_h,  eyeR_h;    // max-height px
Sp pupL_x, pupL_y;     // left pupil offset -1..+1
Sp pupR_x, pupR_y;     // right pupil offset
Sp pupL_r, pupR_r;     // pupil radius
Sp irisB;               // iris brightness 0=dim 1=bright
Sp browL_y, browR_y;   // brow center-Y offset (neg=up)
Sp browL_in, browR_in; // brow inner-end Y offset (+ve=down=angry)
Sp mCurve;              // mouth curve -1=frown +1=smile
Sp mOpen;               // mouth openness 0-1
Sp mHW;                 // mouth half-width px
Sp mSkew;               // mouth X skew
Sp faceY;               // face vertical offset
Sp cheek;               // cheek intensity 0-1

// Current spring params
float g_k = 0.12f, g_d = 0.70f;

void tickAllSprings() {
  float k = g_k, d = g_d;
  eyeL_op.tick(k,d); eyeR_op.tick(k,d);
  eyeL_w.tick(k,d);  eyeR_w.tick(k,d);
  eyeL_h.tick(k,d);  eyeR_h.tick(k,d);
  pupL_x.tick(0.10f,0.80f); pupL_y.tick(0.10f,0.80f);
  pupR_x.tick(0.10f,0.80f); pupR_y.tick(0.10f,0.80f);
  pupL_r.tick(k,d);  pupR_r.tick(k,d);
  irisB.tick(0.07f,0.86f);
  browL_y.tick(k,d); browR_y.tick(k,d);
  browL_in.tick(k,d);browR_in.tick(k,d);
  mCurve.tick(k,d);  mOpen.tick(k,d);
  mHW.tick(k,d);     mSkew.tick(k,d);
  faceY.tick(0.05f,0.90f);
  cheek.tick(0.07f,0.85f);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: EXPRESSION DEFINITIONS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
enum Expr {
  IDLE=0, HAPPY, CURIOUS, LISTENING, SURPRISED,
  SLEEPY, SLEEPING, CONFUSED, EXCITED, SAD, ANGRY, EXPR_N
};
Expr curExpr = IDLE;

// Pupil defaults for gaze engine
float ePupX = 0, ePupY = 0, ePupRX = 0;
bool  gazeOn = true;

void applyExpr(Expr e) {
  curExpr = e; gazeOn = true;
  switch(e) {
    case IDLE:
      eyeL_op.to(0.85f); eyeR_op.to(0.85f);
      eyeL_w.to(BASE_EW); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR); pupR_r.to(BASE_PR); irisB.to(0.50f);
      browL_y.to(0); browR_y.to(0); browL_in.to(0); browR_in.to(0);
      mCurve.to(0.15f); mOpen.to(0); mHW.to(M_HW_B); mSkew.to(0); cheek.to(0);
      ePupX=0; ePupY=0; ePupRX=0;
      g_k=0.10f; g_d=0.74f; break;

    case HAPPY:
      eyeL_op.to(0.62f); eyeR_op.to(0.62f);
      eyeL_w.to(BASE_EW+2); eyeR_w.to(BASE_EW+2);
      eyeL_h.to(BASE_EH); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR-1); pupR_r.to(BASE_PR-1); irisB.to(0.85f);
      browL_y.to(-3); browR_y.to(-3); browL_in.to(-2); browR_in.to(-2);
      mCurve.to(0.92f); mOpen.to(0.15f); mHW.to(M_HW_B+3); mSkew.to(0); cheek.to(0.75f);
      ePupX=0; ePupY=-0.1f; ePupRX=0;
      g_k=0.13f; g_d=0.67f; break;

    case CURIOUS:
      eyeL_op.to(0.92f); eyeR_op.to(0.82f);
      eyeL_w.to(BASE_EW+3); eyeR_w.to(BASE_EW-1);
      eyeL_h.to(BASE_EH+2); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR+1); pupR_r.to(BASE_PR); irisB.to(0.65f);
      browL_y.to(-5); browR_y.to(-1); browL_in.to(-2); browR_in.to(1);
      mCurve.to(0.28f); mOpen.to(0.08f); mHW.to(M_HW_B-3); mSkew.to(-2); cheek.to(0);
      ePupX=0.4f; ePupY=0; ePupRX=0.4f;
      g_k=0.12f; g_d=0.68f; break;

    case LISTENING:
      eyeL_op.to(0.92f); eyeR_op.to(0.92f);
      eyeL_w.to(BASE_EW); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR); pupR_r.to(BASE_PR); irisB.to(0.60f);
      browL_y.to(-2); browR_y.to(-2); browL_in.to(-1); browR_in.to(-1);
      mCurve.to(0.05f); mOpen.to(0.07f); mHW.to(M_HW_B-4); mSkew.to(0); cheek.to(0);
      ePupX=0; ePupY=0.1f; ePupRX=0;
      g_k=0.11f; g_d=0.73f; break;

    case SURPRISED:
      eyeL_op.to(1.0f); eyeR_op.to(1.0f);
      eyeL_w.to(BASE_EW+5); eyeR_w.to(BASE_EW+5);
      eyeL_h.to(BASE_EH+5); eyeR_h.to(BASE_EH+5);
      pupL_r.to(BASE_PR+2); pupR_r.to(BASE_PR+2); irisB.to(0.75f);
      browL_y.to(-7); browR_y.to(-7); browL_in.to(-3); browR_in.to(-3);
      mCurve.to(0); mOpen.to(0.88f); mHW.to(M_HW_B-5); mSkew.to(0); cheek.to(0.25f);
      ePupX=0; ePupY=0; ePupRX=0; gazeOn=false;
      g_k=0.28f; g_d=0.52f; break;

    case SLEEPY:
      eyeL_op.to(0.33f); eyeR_op.to(0.33f);
      eyeL_w.to(BASE_EW); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR); pupR_r.to(BASE_PR); irisB.to(0.18f);
      browL_y.to(2); browR_y.to(2); browL_in.to(2); browR_in.to(2);
      mCurve.to(0); mOpen.to(0.05f); mHW.to(M_HW_B-6); mSkew.to(0); cheek.to(0);
      ePupX=0; ePupY=0.25f; ePupRX=0;
      g_k=0.06f; g_d=0.84f; break;

    case SLEEPING:
      eyeL_op.to(0.05f); eyeR_op.to(0.05f);
      eyeL_w.to(BASE_EW); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR); pupR_r.to(BASE_PR); irisB.to(0);
      browL_y.to(3); browR_y.to(3); browL_in.to(3); browR_in.to(3);
      mCurve.to(0.06f); mOpen.to(0.03f); mHW.to(M_HW_B-7); mSkew.to(0); cheek.to(0);
      ePupX=0; ePupY=0; ePupRX=0; gazeOn=false;
      g_k=0.04f; g_d=0.91f; break;

    case CONFUSED:
      eyeL_op.to(0.88f); eyeR_op.to(0.76f);
      eyeL_w.to(BASE_EW+3); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH+1); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR); pupR_r.to(BASE_PR); irisB.to(0.45f);
      browL_y.to(-4); browR_y.to(1); browL_in.to(-2); browR_in.to(5);
      mCurve.to(-0.12f); mOpen.to(0.07f); mHW.to(M_HW_B-3); mSkew.to(-4); cheek.to(0);
      ePupX=-0.25f; ePupY=0; ePupRX=0.2f;
      g_k=0.11f; g_d=0.70f; break;

    case EXCITED:
      eyeL_op.to(1.0f); eyeR_op.to(1.0f);
      eyeL_w.to(BASE_EW+4); eyeR_w.to(BASE_EW+4);
      eyeL_h.to(BASE_EH+5); eyeR_h.to(BASE_EH+5);
      pupL_r.to(BASE_PR+2); pupR_r.to(BASE_PR+2); irisB.to(1.0f);
      browL_y.to(-6); browR_y.to(-6); browL_in.to(-3); browR_in.to(-3);
      mCurve.to(0.88f); mOpen.to(0.55f); mHW.to(M_HW_B+5); mSkew.to(0); cheek.to(0.90f);
      ePupX=0; ePupY=0; ePupRX=0;
      g_k=0.18f; g_d=0.60f; break;

    case SAD:
      eyeL_op.to(0.70f); eyeR_op.to(0.70f);
      eyeL_w.to(BASE_EW); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR); pupR_r.to(BASE_PR); irisB.to(0.12f);
      browL_y.to(-2); browR_y.to(-2); browL_in.to(-6); browR_in.to(-6); // inner UP = sad arch
      mCurve.to(-0.75f); mOpen.to(0); mHW.to(M_HW_B-2); mSkew.to(0); cheek.to(0);
      ePupX=0; ePupY=0.30f; ePupRX=0;
      g_k=0.08f; g_d=0.78f; break;

    case ANGRY:
      eyeL_op.to(0.38f); eyeR_op.to(0.38f);
      eyeL_w.to(BASE_EW); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR-1); pupR_r.to(BASE_PR-1); irisB.to(0.05f);
      browL_y.to(-1); browR_y.to(-1); browL_in.to(8); browR_in.to(8); // inner DOWN = angry V
      mCurve.to(-0.42f); mOpen.to(0); mHW.to(M_HW_B-4); mSkew.to(0); cheek.to(0);
      ePupX=0; ePupY=0.15f; ePupRX=0;
      g_k=0.20f; g_d=0.58f; break;

    default: break;
  }
}

void snapToExpr(Expr e) {
  applyExpr(e);
  // Snap all springs to targets (no transition on first frame)
  eyeL_op.snap(eyeL_op.tgt); eyeR_op.snap(eyeR_op.tgt);
  eyeL_w.snap(eyeL_w.tgt);   eyeR_w.snap(eyeR_w.tgt);
  eyeL_h.snap(eyeL_h.tgt);   eyeR_h.snap(eyeR_h.tgt);
  pupL_x.snap(ePupX);  pupL_y.snap(ePupY);
  pupR_x.snap(ePupRX); pupR_y.snap(ePupY);
  pupL_r.snap(pupL_r.tgt); pupR_r.snap(pupR_r.tgt);
  irisB.snap(irisB.tgt);
  browL_y.snap(browL_y.tgt); browR_y.snap(browR_y.tgt);
  browL_in.snap(browL_in.tgt); browR_in.snap(browR_in.tgt);
  mCurve.snap(mCurve.tgt); mOpen.snap(mOpen.tgt);
  mHW.snap(mHW.tgt);     mSkew.snap(mSkew.tgt);
  faceY.snap(0); cheek.snap(cheek.tgt);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: EXPRESSION SELECTION (weighted random)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Weights sum to 100. Order matches Expr enum.
const int EXPR_W[EXPR_N]        = {25,18,15,12, 4, 6, 3, 7, 5, 3, 2};
const uint16_t EXPR_MIN_S[EXPR_N] = { 4, 3, 3, 3, 2, 5, 8,  3, 3, 4, 2};
const uint16_t EXPR_MAX_S[EXPR_N] = { 8, 6, 5, 6, 3,10,15,  5, 4, 8, 4};

unsigned long exprEndMs = 0;

Expr pickExpr() {
  int r = random(100), c = 0;
  for(int i=0;i<EXPR_N;i++){ c+=EXPR_W[i]; if(r<c) return (Expr)i; }
  return IDLE;
}

void switchExpr(Expr e) {
  applyExpr(e);
  exprEndMs = millis() + (unsigned long)random(EXPR_MIN_S[e]*1000, EXPR_MAX_S[e]*1000);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: POSE INTERPOLATION / EASING (spring tick above)
//  Expression timer check
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void checkExprTimer(unsigned long now) {
  if(now >= exprEndMs) switchExpr(pickExpr());
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: BLINK ENGINE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
enum BlinkSt { BL_IDLE, BL_CLOSING, BL_CLOSED, BL_OPENING };
BlinkSt   blinkSt     = BL_IDLE;
unsigned long blinkPh  = 0;
unsigned long blinkAt  = 0;
float         blinkMul = 1.0f;
bool          bDbl     = false;
bool          bLong    = false;

void scheduleBlink(unsigned long now) {
  int mn, mx;
  switch(curExpr){
    case SLEEPY:    mn=3000; mx=6000; break;
    case SURPRISED: mn=5000; mx=9000; break;
    case EXCITED:   mn=1200; mx=2800; break;
    default:        mn=2200; mx=5500; break;
  }
  blinkAt = now + random(mn, mx);
  int r = random(100);
  bDbl  = (r < 12);
  bLong = (!bDbl && r < 28);
}

void updateBlink(unsigned long now) {
  if(curExpr == SLEEPING){ blinkMul=1.0f; return; }
  switch(blinkSt){
    case BL_IDLE:
      blinkMul=1.0f;
      if(now>=blinkAt){ blinkSt=BL_CLOSING; blinkPh=now; }
      break;
    case BL_CLOSING:{
      float t=(float)(now-blinkPh)/80.0f;
      if(t>=1.0f){ blinkMul=0; blinkSt=BL_CLOSED; blinkPh=now; }
      else blinkMul=1.0f-t;
      break;}
    case BL_CLOSED:
      blinkMul=0;
      if(now-blinkPh>=(unsigned long)(bLong?180:25)){ blinkSt=BL_OPENING; blinkPh=now; }
      break;
    case BL_OPENING:{
      float t=(float)(now-blinkPh)/100.0f;
      if(t>=1.0f){
        blinkMul=1.0f; bLong=false;
        if(bDbl){ bDbl=false; blinkSt=BL_CLOSING; blinkPh=now; }
        else    { blinkSt=BL_IDLE; scheduleBlink(now); }
      } else blinkMul=t*t*(3.0f-2.0f*t); // smoothstep
      break;}
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: GAZE ENGINE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
float gazeTX=0, gazeTY=0;
unsigned long gazeNext=0;

void updateGaze(unsigned long now) {
  if(!gazeOn) return;
  if(now >= gazeNext) {
    float rng=0.45f;
    if(curExpr==CURIOUS||curExpr==LISTENING) rng=0.60f;
    if(curExpr==EXCITED)  rng=0.65f;
    if(curExpr==SLEEPY)   rng=0.20f;
    if(curExpr==ANGRY)    rng=0.25f;
    if(random(100)<30){ gazeTX=0; gazeTY=0; }
    else {
      gazeTX=((float)random(200)/100.0f-1.0f)*rng;
      gazeTY=((float)random(200)/100.0f-1.0f)*rng*0.55f;
    }
    int mn,mx;
    switch(curExpr){
      case EXCITED:   mn=300;  mx=1200; break;
      case SLEEPY:    mn=2000; mx=5000; break;
      case CURIOUS:   mn=500;  mx=1800; break;
      default:        mn=800;  mx=3000; break;
    }
    gazeNext=now+random(mn,mx);
  }
  pupL_x.to(constrain(ePupX +gazeTX,-0.85f,0.85f));
  pupL_y.to(constrain(ePupY +gazeTY,-0.85f,0.85f));
  pupR_x.to(constrain(ePupRX+gazeTX,-0.85f,0.85f));
  pupR_y.to(constrain(ePupY +gazeTY,-0.85f,0.85f));
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: MICRO-ANIMATION ENGINE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
float mu_bL=0, mu_bR=0;   // brow micro offset px
float mu_pLx=0, mu_pLy=0; // pupil micro offset
float mu_pRx=0, mu_pRy=0;
float mu_fY=0;             // face bob
float mu_mo=0;             // mouth micro

unsigned long angTwNext=0;
float         angTwV=0;

void updateMicro(unsigned long now) {
  float t = now*0.001f;
  // Always-on subtle pupil drift
  mu_pLx=sinf(t*0.47f)*0.05f+sinf(t*1.33f)*0.025f;
  mu_pLy=cosf(t*0.31f)*0.04f;
  mu_pRx=sinf(t*0.53f)*0.05f+sinf(t*1.19f)*0.025f;
  mu_pRy=cosf(t*0.37f)*0.04f;
  mu_bL=0; mu_bR=0; mu_mo=0;
  switch(curExpr){
    case IDLE:      mu_fY=sinf(t*0.65f)*1.4f; break;
    case HAPPY:
      mu_fY=sinf(t*1.8f)*2.3f;
      mu_bL=sinf(t*1.8f)*1.2f; mu_bR=mu_bL;
      mu_mo=sinf(t*1.8f)*0.04f; break;
    case EXCITED:
      mu_fY=sinf(t*4.2f)*3.2f;
      mu_bL=sinf(t*4.2f)*2.0f; mu_bR=mu_bL; break;
    case SLEEPY:  mu_fY=sinf(t*0.38f)*1.5f; break;
    case SLEEPING:mu_fY=sinf(t*0.26f)*1.8f; break;  // breathing
    case SAD:
      mu_fY=sinf(t*0.32f)*0.9f;
      mu_mo=sinf(t*0.55f)*0.03f; break;
    case CURIOUS:
      mu_fY=sinf(t*0.9f)*1.2f;
      mu_bL=sinf(t*0.7f)*0.8f; break;
    case LISTENING: mu_mo=sinf(t*1.1f)*0.05f; break;
    case CONFUSED:  mu_bL=sinf(t*0.72f)*1.4f; break;
    case ANGRY:
      if(now>angTwNext){ angTwV=(float)random(-3,4); angTwNext=now+random(600,2200); }
      angTwV*=0.82f;
      mu_bL=angTwV; mu_bR=-angTwV*0.6f; break;
    default: mu_fY=sinf(t*0.55f)*1.0f; break;
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: EXPRESSION-SPECIFIC ANIMATION ENGINE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void updateExprAnim(unsigned long now) {
  float t = now*0.001f;
  switch(curExpr){
    case CURIOUS:
      // Slightly oscillating larger left eye
      eyeL_h.to(BASE_EH+2+(int)(sinf(t*0.6f)*1.2f));
      break;
    case EXCITED:
      // Eye height pulses with bounce
      { float b=sinf(t*4.0f)*1.5f;
        eyeL_h.to(BASE_EH+5+b); eyeR_h.to(BASE_EH+5+b); }
      break;
    case SAD:
      // Very slow downward face drift
      faceY.to(sinf(t*0.22f)*2.0f);
      break;
    case CONFUSED:{
      // Occasionally diverge eyes dramatically
      static unsigned long cnNext=0; static bool cnMode=false; static unsigned long cnEnd=0;
      if(!cnMode && now>cnNext){ cnMode=true; cnEnd=now+random(800,2000); cnNext=cnEnd+random(1500,4000); }
      if(cnMode){ if(now>cnEnd) cnMode=false; else { pupL_x.to(-0.6f); pupR_x.to(0.5f); } }
      break;}
    default: break;
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: EYE RENDERER
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
uint16_t getIrisColor() {
  float b=constrain(irisB.v,0,1);
  uint16_t ca, cb; float t;
  if(b<0.5f){ ca=C_IRIS_DIM; cb=C_IRIS;     t=b*2.0f; }
  else       { ca=C_IRIS;     cb=C_IRIS_BRT; t=(b-0.5f)*2.0f; }
  uint8_t r1=(ca>>11)&0x1F,g1=(ca>>5)&0x3F,b1=ca&0x1F;
  uint8_t r2=(cb>>11)&0x1F,g2=(cb>>5)&0x3F,b2=cb&0x1F;
  return (uint16_t)(r1+(r2-r1)*t)<<11|(uint16_t)(g1+(g2-g1)*t)<<5|(uint16_t)(b1+(b2-b1)*t);
}

void drawEye(int cx, int cy, float bW, float bH, float op,
             float pX, float pY, float pR, uint16_t ic) {
  int w  = (int)bW;
  int h  = max(1,(int)(bH*op));
  int cr = constrain(min(w/4,h/2),1,6);
  int top= cy-h/2;

  // Closed-eye: just a line
  if(h<=2){
    tft.drawFastHLine(cx-w/2, cy, w, C_OUTLINE);
    return;
  }

  // Sclera
  tft.fillRoundRect(cx-w/2, top, w, h, cr, C_WHITE);

  // Iris + pupil
  int pr=max(1,(int)pR), ir=BASE_IR;
  if(h>pr*2+2 && w>ir*2){
    int pxR=max(1,w/2-ir), pyR=max(1,h/2-ir);
    int ipx=cx+constrain((int)(pX*pxR),-pxR,pxR);
    int ipy=cy+constrain((int)(pY*pyR),-pyR,pyR);
    ipx=constrain(ipx,cx-w/2+ir,cx+w/2-ir);
    ipy=constrain(ipy,top+ir,top+h-ir);
    tft.fillCircle(ipx,ipy,ir,ic);
    tft.fillCircle(ipx,ipy,pr,C_BLACK);
    tft.fillCircle(ipx-ir/3,ipy-ir/4,max(1,pr/2),C_WHITE); // highlight
  }

  // Outline (double = 2px thick)
  tft.drawRoundRect(cx-w/2-1,top-1,w+2,h+2,cr+1,C_OUTLINE);
  tft.drawRoundRect(cx-w/2,  top,  w,  h,  cr,  C_OUTLINE);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: EYEBROW RENDERER
//  Left brow : outer=left end, inner=right end
//  Right brow: inner=left end, outer=right end  (mirror)
//  browL_in>0 -> inner corner goes DOWN -> angry V
//  browL_in<0 -> inner corner goes UP   -> sad arch / raised
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void drawBrowL(float cyOff, float inOff, float mu, int fy) {
  int cy=(int)(B_CY+fy+cyOff+mu);
  int iy=cy+(int)inOff;
  for(int t=-1;t<=1;t++) tft.drawLine(BL_CX-B_HW,cy+t,BL_CX+B_HW,iy+t,C_BROW);
  tft.fillCircle(BL_CX-B_HW,cy,2,C_BROW);
  tft.fillCircle(BL_CX+B_HW,iy,2,C_BROW);
}

void drawBrowR(float cyOff, float inOff, float mu, int fy) {
  int cy=(int)(B_CY+fy+cyOff+mu);
  int iy=cy+(int)inOff;
  // Mirror: inner=left, outer=right
  for(int t=-1;t<=1;t++) tft.drawLine(BR_CX-B_HW,iy+t,BR_CX+B_HW,cy+t,C_BROW);
  tft.fillCircle(BR_CX-B_HW,iy,2,C_BROW);
  tft.fillCircle(BR_CX+B_HW,cy,2,C_BROW);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: MOUTH RENDERER
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void drawMouth(float curve, float op, float hw, float skew, float mmu, int fy) {
  int cx=M_CX+(int)skew;
  int cy=M_CY+fy;
  int iw=(int)hw;
  float ec=curve+mmu;

  if(op>0.12f){
    // Open mouth
    int oh=constrain((int)(op*24),3,24);
    int cr=min(oh/2,iw);
    tft.fillRoundRect(cx-iw,cy-oh/2,iw*2,oh,cr,C_FACE_HL);
    tft.drawRoundRect(cx-iw-1,cy-oh/2-1,iw*2+2,oh+2,cr+1,C_MOUTH);
    tft.drawRoundRect(cx-iw,  cy-oh/2,  iw*2,  oh,  cr,  C_MOUTH);
    // Smile arc on bottom for happy/excited
    if(ec>0.3f){
      int amp=(int)(ec*5), px=cx-iw, py=cy+oh/2-2;
      for(int i=1;i<=10;i++){
        float tf=(float)i/10.0f, ft=2*tf-1;
        int nx=cx-iw+(int)(tf*iw*2);
        int ny=(cy+oh/2-2)+(int)((1-ft*ft)*amp);
        tft.drawLine(px,py,nx,ny,C_MOUTH); px=nx; py=ny;
      }
    }
  } else {
    // Closed curve (parabola, 14 segments)
    int amp=(int)(ec*M_AMP);
    int px=cx-iw, py=cy;
    for(int i=1;i<=14;i++){
      float tf=(float)i/14.0f, ft=2*tf-1;
      int nx=cx-iw+(int)(tf*iw*2);
      int ny=cy-(int)((1-ft*ft)*amp);
      tft.drawLine(px,py,nx,ny,C_MOUTH);
      tft.drawLine(px,py+1,nx,ny+1,C_MOUTH); // 2px thick
      px=nx; py=ny;
    }
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: MAIN DRAW FUNCTION
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void drawFrame() {
  int fy=(int)(faceY.v+mu_fY);
  uint16_t ic=getIrisColor();
  float elOp=eyeL_op.v*blinkMul;
  float erOp=eyeR_op.v*blinkMul;
  float plx=constrain(pupL_x.v+mu_pLx,-0.95f,0.95f);
  float ply=constrain(pupL_y.v+mu_pLy,-0.95f,0.95f);
  float prx=constrain(pupR_x.v+mu_pRx,-0.95f,0.95f);
  float pry=constrain(pupR_y.v+mu_pRy,-0.95f,0.95f);

  // Background + face oval (overwrites previous frame)
  tft.fillScreen(C_BG);
  tft.fillRoundRect(FBG_X, FBG_Y+fy, FBG_W, FBG_H, FBG_R, C_FACE);

  // Cheeks (behind eyes so drawn first)
  if(cheek.v>0.05f){
    int cr=min(9,(int)(cheek.v*10));
    if(cr>2){
      tft.fillCircle(35,68+fy,cr,C_CHEEK);
      tft.fillCircle(125,68+fy,cr,C_CHEEK);
      tft.fillCircle(35,68+fy,cr-3,C_FACE); // soften center
      tft.fillCircle(125,68+fy,cr-3,C_FACE);
    }
  }

  // Eyes
  drawEye(EL_CX, E_CY+fy, eyeL_w.v, eyeL_h.v, elOp, plx, ply, pupL_r.v, ic);
  drawEye(ER_CX, E_CY+fy, eyeR_w.v, eyeR_h.v, erOp, prx, pry, pupR_r.v, ic);

  // Eyebrows
  drawBrowL(browL_y.v, browL_in.v, mu_bL, fy);
  drawBrowR(browR_y.v, browR_in.v, mu_bR, fy);

  // Mouth
  drawMouth(mCurve.v, mOpen.v, mHW.v, mSkew.v, mu_mo, fy);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: SETUP
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));

  // Init TFT
  // Change INITR_BLACKTAB to INITR_GREENTAB or INITR_REDTAB if colours look wrong
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);          // landscape: 160 wide x 128 tall
  SPI.setFrequency(26000000);  // 26 MHz - safe for most ST7735 modules
  tft.fillScreen(C_BG);

  // Start at IDLE, snapped (no lerp on first frame)
  snapToExpr(IDLE);
  exprEndMs = millis() + 4000;
  scheduleBlink(millis());
  gazeNext = millis() + 800;

  Serial.println(F("RobotFace ready"));
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SECTION: MAIN LOOP
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
unsigned long lastFrMs = 0;

void loop() {
  unsigned long now = millis();
  if(now - lastFrMs < FRAME_MS) return;
  lastFrMs = now;

  // Update all systems
  checkExprTimer(now);
  updateBlink(now);
  updateGaze(now);
  updateMicro(now);
  updateExprAnim(now);

  // Advance spring physics
  tickAllSprings();

  // Render
  drawFrame();
}
