// ============================================================
//  HATEBOT v4  |  Aesthetic Spring-Physics Edition
//  ESP8266 NodeMCU + ST7735 160x128 + MPU6050 + 2x Servos
//
//  ── TFT ST7735 ─────────────────────────────────────────────
//    VCC   -> 3V3
//    GND   -> GND
//    LED   -> 3V3
//    SCK   -> D5 / GPIO14  (hardware SPI)
//    SDA   -> D7 / GPIO13  (hardware SPI MOSI)
//    CS    -> D3 / GPIO0
//    A0/DC -> D4 / GPIO2
//    RST   -> D0 / GPIO16
//
//  ── MPU6050 ────────────────────────────────────────────────
//    VCC -> 3V3  |  GND -> GND
//    SDA -> D2 / GPIO4
//    SCL -> D1 / GPIO5
//
//  ── Servos (via 5V buck) ───────────────────────────────────
//    Servo 1 Signal -> D6 / GPIO12
//    Servo 2 Signal -> D8 / GPIO15
//    Servo VCC -> Buck 5V  |  GND -> Buck GND + NodeMCU GND
//
//  ── Buzzer (passive, 3-pin module) ────────────────────────
//    I/O -> TX / GPIO1  |  VCC -> 3V3  |  GND -> GND
//
//  ── Traffic Light (Green LED used) ───────────────────────
//    G   -> RX / GPIO3  |  GND -> GND
// ============================================================

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <MPU6050.h>
#include <math.h>

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  HARDWARE PINS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define TFT_CS   0
#define TFT_DC   2
#define TFT_RST  16

#define MPU_SDA  4
#define MPU_SCL  5
#define SERVO1   12
#define SERVO2   15
#define BUZZER   1
#define LED_GRN  3

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
MPU6050 imu;
Servo sL, sR;

#define SW          160
#define SH          128
#define TARGET_FPS  30
#define FRAME_MS    (1000 / TARGET_FPS)

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  COLOUR PALETTE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define C_BG        0x0005
#define C_FACE      0x2124
#define C_FACE_HL   0x318C
#define C_WHITE     0xFFFF
#define C_BLACK     0x0000
#define C_IRIS_DIM  0x035B
#define C_IRIS      0x07BF
#define C_IRIS_BRT  0x07FF
#define C_BROW      0xE73C
#define C_MOUTH     0xE73C
#define C_CHEEK     0xF9AC
#define C_OUTLINE   0x0000

#define C_HUD_BG    0x10A2
#define C_HUD_TXT   0x738E

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  FACE GEOMETRY
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define FA_Y    14
#define FA_H    100
#define FBG_X   7
#define FBG_Y   FA_Y+2
#define FBG_W   146
#define FBG_H   96
#define FBG_R   20

#define BASE_EW  24
#define BASE_EH  18
#define EL_CX    53
#define ER_CX    107
#define E_CY     FA_Y + 42
#define BASE_IR  6
#define BASE_PR  3
#define BL_CX    53
#define BR_CX    107
#define B_CY     FA_Y + 22
#define B_HW     10
#define M_CX     80
#define M_CY     FA_Y + 76
#define M_HW_B   22
#define M_AMP    10

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  SPRING SYSTEM
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
struct Sp {
  float v = 0, vel = 0, tgt = 0;
  void to(float t)             { tgt = t; }
  void snap(float x)           { v = x; vel = 0; tgt = x; }
  void tick(float k, float d)  { vel = vel*d + (tgt-v)*k; v += vel; }
};

Sp eyeL_op, eyeR_op, eyeL_w, eyeR_w, eyeL_h, eyeR_h;
Sp pupL_x, pupL_y, pupR_x, pupR_y, pupL_r, pupR_r;
Sp irisB, browL_y, browR_y, browL_in, browR_in;
Sp mCurve, mOpen, mHW, mSkew, faceY, cheek;

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
//  HATEBOT MOOD LOGIC
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
enum Mood { IDLE=0, ANNOYED=1, FURIOUS=2, SULKING=3 };
Mood mood = IDLE;

float ePupX=0, ePupY=0, ePupRX=0;
bool gazeOn = true;

// Timing & Escalation
int furiousHits = 0;
unsigned long lastEvt = 0;
#define T_ANN_IDLE  12000UL
#define T_FUR_ANN   10000UL
#define T_SUL_IDLE  25000UL

// IMU
float bAx,bAy,bAz, prevHP=0;
bool inPick=false;
unsigned long pickMs=0;

// Typewriter HUD
char insultBuf[30] = "";
uint8_t revChars = 0;
unsigned long twMs = 0;
bool redrawHUD = true;

// LED
unsigned long ledMs = 0;
bool ledState = false;

// Arm
unsigned long armMs = 0;

void applyMoodSprings(Mood m) {
  gazeOn = true;
  switch(m) {
    case IDLE:
      eyeL_op.to(0.85f); eyeR_op.to(0.85f);
      eyeL_w.to(BASE_EW); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR); pupR_r.to(BASE_PR); irisB.to(0.50f);
      browL_y.to(0); browR_y.to(0); browL_in.to(0); browR_in.to(0);
      mCurve.to(0.15f); mOpen.to(0); mHW.to(M_HW_B); mSkew.to(0); cheek.to(0);
      ePupX=0; ePupY=0; ePupRX=0;
      g_k=0.10f; g_d=0.74f; break;

    case ANNOYED:
      eyeL_op.to(0.88f); eyeR_op.to(0.76f);
      eyeL_w.to(BASE_EW+3); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH+1); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR); pupR_r.to(BASE_PR); irisB.to(0.45f);
      browL_y.to(-2); browR_y.to(1); browL_in.to(-2); browR_in.to(5);
      mCurve.to(-0.12f); mOpen.to(0.05f); mHW.to(M_HW_B-3); mSkew.to(-4); cheek.to(0);
      ePupX=-0.6f; ePupY=0; ePupRX=-0.6f; // strong side-eye
      g_k=0.18f; g_d=0.60f; break;

    case FURIOUS:
      eyeL_op.to(0.38f); eyeR_op.to(0.38f);
      eyeL_w.to(BASE_EW); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH-3); eyeR_h.to(BASE_EH-3);
      pupL_r.to(BASE_PR-1); pupR_r.to(BASE_PR-1); irisB.to(0.05f); // red/dim
      browL_y.to(-2); browR_y.to(-2); browL_in.to(8); browR_in.to(8); // hard V
      mCurve.to(-0.42f); mOpen.to(0.1f); mHW.to(M_HW_B-4); mSkew.to(0); cheek.to(0);
      ePupX=0; ePupY=0.15f; ePupRX=0;
      g_k=0.28f; g_d=0.52f; break;

    case SULKING:
      eyeL_op.to(0.70f); eyeR_op.to(0.70f);
      eyeL_w.to(BASE_EW); eyeR_w.to(BASE_EW);
      eyeL_h.to(BASE_EH); eyeR_h.to(BASE_EH);
      pupL_r.to(BASE_PR); pupR_r.to(BASE_PR); irisB.to(0.12f);
      browL_y.to(-2); browR_y.to(-2); browL_in.to(-6); browR_in.to(-6); // sad arch
      mCurve.to(-0.75f); mOpen.to(0); mHW.to(M_HW_B-2); mSkew.to(0); cheek.to(0);
      ePupX=0; ePupY=0.40f; ePupRX=0; gazeOn=false;
      g_k=0.08f; g_d=0.78f; break;
  }
}

void snapMood(Mood m) {
  applyMoodSprings(m);
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
  cheek.snap(cheek.tgt);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  HARDWARE CONTROL
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void armGo(int l, int r, int ms) {
  sL.write(l); sR.write(180-r);
  if(ms>0) delay(ms);
}

void moveArms(Mood m) {
  switch(m){
    case IDLE:    break;
    case ANNOYED: armGo(110,110,150); delay(50); armGo(90,90,150); break;
    case FURIOUS:
      for(int i=0;i<3;i++){armGo(140,90,60);armGo(90,140,60);}
      armGo(90,90,100); break;
    case SULKING: armGo(145,145,400); break;
  }
}

void buzz(int f,int d){ tone(BUZZER,f,d); delay(d+8); noTone(BUZZER); }
void buzzPat(Mood m){
  switch(m){
    case IDLE:    buzz(180,50); break;
    case ANNOYED: buzz(440,70); delay(30); buzz(330,100); break;
    case FURIOUS:
      buzz(480,40);delay(15);buzz(600,40);delay(15);
      buzz(750,40);delay(15);buzz(920,80); break;
    case SULKING: buzz(200,400); break;
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  STRINGS & TEXT
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
const char s_c0[] PROGMEM = "Oh. It's you.";
const char s_c1[] PROGMEM = "Still here?";
const char s_c2[] PROGMEM = "...cool.";
const char* const p_idle[] PROGMEM = {s_c0,s_c1,s_c2};

const char s_a0[] PROGMEM = "Seriously?";
const char s_a1[] PROGMEM = "Please stop.";
const char s_a2[] PROGMEM = "You again.";
const char* const p_ann[] PROGMEM  = {s_a0,s_a1,s_a2};

const char s_f0[] PROGMEM = "HOW DARE YOU.";
const char s_f1[] PROGMEM = "STOP TOUCHING ME";
const char s_f2[] PROGMEM = "GET AWAY.";
const char* const p_fur[] PROGMEM  = {s_f0,s_f1,s_f2};

const char s_s0[] PROGMEM = "I'm done.";
const char s_s1[] PROGMEM = "...";
const char* const p_sul[] PROGMEM  = {s_s0,s_s1};

uint8_t insultIdx = 0;

void setInsult(Mood m) {
  const char* const* pool;
  uint8_t sz;
  switch(m){
    case IDLE:    pool=p_idle; sz=3; break;
    case ANNOYED: pool=p_ann;  sz=3; break;
    case FURIOUS: pool=p_fur;  sz=3; break;
    case SULKING: pool=p_sul;  sz=2; break;
  }
  const char* s = (const char*)pgm_read_ptr(&pool[insultIdx % sz]);
  insultIdx++;
  strncpy_P(insultBuf, s, 29); insultBuf[29]='\0';
  revChars = 0; twMs = millis();
}

void changeMood(Mood m) {
  mood = m;
  applyMoodSprings(m);
  setInsult(m);
  redrawHUD = true;
  buzzPat(m);
  armGo(90,90,0); delay(40); moveArms(m);
}

void escalate(int w) {
  if(mood==SULKING) return;
  if(mood==FURIOUS) { if(++furiousHits>=3){ furiousHits=0; changeMood(SULKING); return; } }
  else furiousHits=0;
  Mood next=(Mood)min((int)mood+w,(int)FURIOUS);
  if(next!=mood) changeMood(next);
  else { buzzPat(mood); armGo(92,92,60); armGo(90,90,60); }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  IMU
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void calIMU() {
  long sx=0,sy=0,sz=0;
  for(int i=0;i<100;i++){
    int16_t ax,ay,az,gx,gy,gz; imu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
    sx+=ax;sy+=ay;sz+=az; delay(5);
  }
  bAx=(float)sx/100; bAy=(float)sy/100; bAz=(float)sz/100;
}

int evIMU() {
  int16_t ax,ay,az,gx,gy,gz; imu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
  float dAx=ax-bAx, dAy=ay-bAy, dAz=az-bAz;
  float mag=sqrtf(dAx*dAx+dAy*dAy+dAz*dAz);

  float hp=fabsf(mag-prevHP); prevHP=mag;
  if(hp>8000.0f) return 1;

  static float win[8]={0}; static uint8_t wi=0;
  win[wi++%8]=mag;
  float mn=0; for(int i=0;i<8;i++) mn+=win[i]; mn/=8.0f;
  float vr=0; for(int i=0;i<8;i++) vr+=(win[i]-mn)*(win[i]-mn); vr/=8.0f;
  if(vr>3000.0f) return 2;

  float tilt=atan2f(dAy,dAz)*180.0f/PI;
  if(fabsf(tilt)>10.0f) return 1;
  return 0;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  ANIMATION ENGINES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Blink
enum BlinkSt { BL_IDLE, BL_CLOSING, BL_CLOSED, BL_OPENING };
BlinkSt blinkSt=BL_IDLE;
unsigned long blinkPh=0, blinkAt=0;
float blinkMul=1.0f;

void updateBlink(unsigned long now) {
  if(mood==SULKING) { blinkMul=1.0f; return; }
  switch(blinkSt){
    case BL_IDLE:
      blinkMul=1.0f;
      if(now>=blinkAt){ blinkSt=BL_CLOSING; blinkPh=now; }
      break;
    case BL_CLOSING:{
      float t=(float)(now-blinkPh)/70.0f;
      if(t>=1.0f){ blinkMul=0; blinkSt=BL_CLOSED; blinkPh=now; }
      else blinkMul=1.0f-t; break;}
    case BL_CLOSED:
      blinkMul=0;
      if(now-blinkPh>=30){ blinkSt=BL_OPENING; blinkPh=now; } break;
    case BL_OPENING:{
      float t=(float)(now-blinkPh)/90.0f;
      if(t>=1.0f){ blinkMul=1.0f; blinkSt=BL_IDLE; blinkAt=now+random(2000,5000); }
      else blinkMul=t*t*(3.0f-2.0f*t); break;}
  }
}

// Gaze
float gazeTX=0, gazeTY=0; unsigned long gazeNext=0;
void updateGaze(unsigned long now) {
  if(!gazeOn) return;
  if(now>=gazeNext) {
    if(random(100)<40){ gazeTX=0; gazeTY=0; }
    else { gazeTX=((float)random(200)/100.0f-1.0f)*0.45f; gazeTY=((float)random(200)/100.0f-1.0f)*0.25f; }
    gazeNext=now+random(800,2500);
  }
  pupL_x.to(constrain(ePupX+gazeTX,-0.8f,0.8f)); pupL_y.to(constrain(ePupY+gazeTY,-0.8f,0.8f));
  pupR_x.to(constrain(ePupRX+gazeTX,-0.8f,0.8f)); pupR_y.to(constrain(ePupY+gazeTY,-0.8f,0.8f));
}

// Micro
float mu_bL=0, mu_bR=0, mu_pLx=0, mu_pLy=0, mu_pRx=0, mu_pRy=0, mu_fY=0;
unsigned long twiNext=0; float twiV=0;
void updateMicro(unsigned long now) {
  float t=now*0.001f;
  mu_pLx=sinf(t*0.4f)*0.03f; mu_pLy=cosf(t*0.3f)*0.02f;
  mu_pRx=sinf(t*0.5f)*0.03f; mu_pRy=cosf(t*0.4f)*0.02f;
  mu_bL=0; mu_bR=0;
  if(mood==IDLE) mu_fY=sinf(t*0.6f)*1.2f;
  else if(mood==SULKING) mu_fY=sinf(t*0.3f)*0.7f;
  else if(mood==FURIOUS) mu_fY=sinf(t*4.0f)*2.0f; // angry shake

  if(mood==ANNOYED||mood==FURIOUS) {
    if(now>twiNext){ twiV=(float)random(-3,4); twiNext=now+random(400,1500); }
    twiV*=0.8f; mu_bL=twiV; mu_bR=-twiV*0.5f;
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  RENDERERS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void drawThickLine(int x0,int y0,int x1,int y1,int t,uint16_t c) {
  for(int i=-t/2;i<=t/2;i++) tft.drawLine(x0,y0+i,x1,y1+i,c);
}

void drawEye(int cx, int cy, float w, float h, float op, float px, float py, float pr) {
  int wi=(int)w, hi=max(1,(int)(h*op)), cr=min(wi/4,hi/2); cr=constrain(cr,1,6);
  int top=cy-hi/2;
  if(hi<=2) { tft.drawFastHLine(cx-wi/2, cy, wi, C_OUTLINE); return; }

  tft.fillRoundRect(cx-wi/2, top, wi, hi, cr, C_WHITE);
  if(hi>pr*2+2 && wi>BASE_IR*2){
    int rx=wi/2-BASE_IR, ry=hi/2-BASE_IR;
    int ipx=cx+constrain((int)(px*rx),-rx,rx);
    int ipy=cy+constrain((int)(py*ry),-ry,ry);
    uint16_t ic = (mood==FURIOUS)?0xF800:C_IRIS; // Red eyes when furious
    tft.fillCircle(ipx,ipy,BASE_IR,ic);
    tft.fillCircle(ipx,ipy,(int)pr,C_BLACK);
    tft.fillCircle(ipx-BASE_IR/3,ipy-BASE_IR/4,max(1,(int)pr/2),C_WHITE);
  }
  tft.drawRoundRect(cx-wi/2-1,top-1,wi+2,hi+2,cr+1,C_OUTLINE);
  tft.drawRoundRect(cx-wi/2,  top,  wi,  hi,  cr,  C_OUTLINE);
}

void drawMouth(float curve, float op, float hw, float skew, int fy) {
  int cx=M_CX+(int)skew, cy=M_CY+fy, iw=(int)hw;
  if(op>0.1f){
    int oh=constrain((int)(op*16),3,16), cr=min(oh/2,iw);
    tft.fillRoundRect(cx-iw,cy-oh/2,iw*2,oh,cr,C_FACE_HL);
    tft.drawRoundRect(cx-iw-1,cy-oh/2-1,iw*2+2,oh+2,cr+1,C_MOUTH);
    tft.drawRoundRect(cx-iw,cy-oh/2,iw*2,oh,cr,C_MOUTH);
    if(mood==FURIOUS && curve<-0.3f) {
      // Teeth
      tft.fillRect(cx-iw+2, cy-2, iw*2-4, 4, C_WHITE);
      tft.drawFastHLine(cx-iw+2, cy, iw*2-4, C_BLACK);
      for(int i=-iw/2;i<iw/2;i+=4) tft.drawFastVLine(cx+i, cy-2, 4, C_BLACK);
    }
  } else {
    int amp=(int)(curve*M_AMP);
    int px=cx-iw, py=cy;
    for(int i=1;i<=10;i++){
      float t=(float)i/10.0f, ft=2*t-1;
      int nx=cx-iw+(int)(t*iw*2), ny=cy-(int)((1-ft*ft)*amp);
      tft.drawLine(px,py,nx,ny,C_MOUTH); tft.drawLine(px,py+1,nx,ny+1,C_MOUTH);
      px=nx; py=ny;
    }
  }
}

void drawHUD() {
  tft.fillRect(0,0,SW,FA_Y,C_HUD_BG);
  tft.drawFastHLine(0,FA_Y,SW,C_HUD_TXT);
  tft.fillRect(0,FA_Y+FA_H,SW,SH-(FA_Y+FA_H),C_HUD_BG);
  tft.drawFastHLine(0,FA_Y+FA_H-1,SW,C_HUD_TXT);

  tft.setTextColor(C_WHITE); tft.setTextSize(1);
  tft.setCursor(4,4); tft.print("MOOD:");
  tft.setTextColor((mood==FURIOUS)?0xF800:(mood==ANNOYED)?0xFE00:(mood==SULKING)?0x7BCF:0x07E0);
  tft.setCursor(40,4);
  const char* mt[]={"IDLE","ANNOYED","FURIOUS","SULKING"};
  tft.print(mt[mood]);
}

void drawInsultText() {
  tft.fillRect(4,FA_Y+FA_H+3,SW-8,SH-(FA_Y+FA_H)-4,C_HUD_BG);
  char tmp[30]; strncpy(tmp,insultBuf,revChars); tmp[revChars]='\0';
  tft.setTextSize(1); tft.setTextColor(C_WHITE);
  int w = revChars*6; int x = max(4,(SW-w)/2);
  tft.setCursor(x, FA_Y+FA_H+4); tft.print(tmp);
  if(revChars<strlen(insultBuf)) tft.fillRect(x+w, FA_Y+FA_H+4, 5, 8, C_WHITE);
}

void drawFrame() {
  if(redrawHUD){ drawHUD(); redrawHUD=false; }
  if(revChars < strlen(insultBuf) && millis()-twMs>60) {
    revChars++; twMs=millis(); drawInsultText();
  }

  int fy=(int)(faceY.v+mu_fY);
  tft.fillRect(0, FA_Y+1, SW, FA_H-2, C_BG);
  tft.fillRoundRect(FBG_X, FBG_Y+fy, FBG_W, FBG_H, FBG_R, C_FACE);

  drawEye(EL_CX, E_CY+fy, eyeL_w.v, eyeL_h.v, eyeL_op.v*blinkMul, pupL_x.v+mu_pLx, pupL_y.v+mu_pLy, pupL_r.v);
  drawEye(ER_CX, E_CY+fy, eyeR_w.v, eyeR_h.v, eyeR_op.v*blinkMul, pupR_x.v+mu_pRx, pupR_y.v+mu_pRy, pupR_r.v);

  // Brows
  int bL_cy=(int)(B_CY+fy+browL_y.v+mu_bL), bL_iy=bL_cy+(int)browL_in.v;
  drawThickLine(BL_CX-B_HW,bL_cy, BL_CX+B_HW,bL_iy, 3,C_BROW);
  int bR_cy=(int)(B_CY+fy+browR_y.v+mu_bR), bR_iy=bR_cy+(int)browR_in.v;
  drawThickLine(BR_CX-B_HW,bR_iy, BR_CX+B_HW,bR_cy, 3,C_BROW); // mirrored

  drawMouth(mCurve.v, mOpen.v, mHW.v, mSkew.v, fy);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//  MAIN
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void setup() {
  Serial.begin(115200);
  Wire.begin(MPU_SDA, MPU_SCL);
  Wire.setClock(400000);
  imu.initialize();
  calIMU();

  sL.attach(SERVO1,500,2400); sR.attach(SERVO2,500,2400);
  armGo(90,90,500);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_GRN, OUTPUT); digitalWrite(LED_GRN, LOW);

  tft.initR(INITR_BLACKTAB);
  SPI.setFrequency(26000000);
  tft.setRotation(1);
  tft.fillScreen(C_BG);

  snapMood(IDLE);
  setInsult(IDLE);
  blinkAt = millis()+2000;
  lastEvt = millis();
}

unsigned long lastFrMs = 0;
void loop() {
  unsigned long now = millis();

  int ev = evIMU();
  if(ev>0){ escalate(ev); lastEvt=now; }

  unsigned long quietMs = now-lastEvt;
  if(mood==SULKING && quietMs>T_SUL_IDLE) changeMood(IDLE);
  else if(mood==FURIOUS && quietMs>T_FUR_ANN) changeMood(ANNOYED);
  else if(mood==ANNOYED && quietMs>T_ANN_IDLE) changeMood(IDLE);

  // Single LED control
  if(mood==IDLE){ digitalWrite(LED_GRN, (now/1000)%2); } // slow pulse
  else if(mood==ANNOYED){ digitalWrite(LED_GRN, HIGH); } // solid
  else if(mood==FURIOUS){ digitalWrite(LED_GRN, (now/100)%2); } // fast blink
  else { digitalWrite(LED_GRN, (now/800)%2); } // sulk slow blink

  // Idle arm twitch
  if(mood==IDLE && now-armMs>8000) {
    armGo(96,90,200); armGo(90,90,200); buzz(180,40); armMs=now;
  }

  if(now - lastFrMs >= FRAME_MS) {
    lastFrMs = now;
    updateBlink(now);
    updateGaze(now);
    updateMicro(now);
    tickAllSprings();
    drawFrame();
  }
}
