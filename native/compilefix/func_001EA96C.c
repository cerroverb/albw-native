/* ===================================================================================================
 * WALL WALKER - what this function is (read this first)
 *
 * func_001EA96C(walker, input) moves a "wall walker": the part of an actor that can merge into a wall
 * and slide along it as a painting. Link has one (called from 00342C24, his movement step). Yuga, the
 * Eastern Palace boss, has his own (EnemyMedium +0x984, called every frame from 0015D6F4).
 *
 * Inputs
 *   walker  the walker object. Fields used most:
 *             +0x30  position (x, y, z)
 *             +0x3B40 position written back each frame (the owner reads it)
 *             +0x3BAE MODE: 0 = free (not in a wall), 1 = merged into a wall, 2 = leaving a wall
 *   input   a block the owner fills each frame (like a controller for the walker):
 *             +0x00  position          +0x0C..+0x2C  orientation rows (+0x24 = facing)
 *             +0x44  reach (how close a wall face must be)
 *             +0x68  owner's model      +0x90 / +0x9C / +0xA0  wall query settings and filters
 *             +0x95  "stay merged" flag (used when MODE is not 0)
 *
 * Flow (line markers "WALKER:" below)
 *   1. Already merged and +0x95 set: follow the wall, write the position, return.
 *   2. Otherwise MODE = 0, then ask the collision system for wall faces near the position.
 *   3. Test each face: is it close enough (reach), facing the right way, inside its edges?
 *   4. No face passed: return 0 (stay free). This is where Yuga's walker currently stops.
 *   5. A face passed: snap onto it, MODE = 1 (merged) or 2 (leaving), return.
 *
 * Painting strip (what is drawn on the wall; full story in native/PAINTING.md)
 *   While merged, the wall the actor covers is cut into segments (walker +0xA4 count, 0x9C bytes
 *   each, start +0x70 / end +0x74 along the wall). Each frame: add the segment under the actor,
 *   extend it over neighbouring faces (0064CB28), keep segments with length (WALK_SEG_HAS_LENGTH),
 *   then turn them into quads in the painting model's free strip buffer (002C1658 swaps buffers,
 *   0043285C finishes). The draw item 00171510 draws the strip with the flat sprite texture.
 *
 * The rest of this file is machine-converted (Ghidra) and hard to read; only the marked places are
 * explained. Known fixes in it are listed in the header comment below.
 * =================================================================================================== */
/* USA 001EA96C: converted stage-2 text (build/decomp/ghidra_fallback/ghidra_analyzed5_v4/001/func_001EA96C.c,
 * the player's per-frame update, 80 KB) with one fix: the lost FIRST store of a stack Vec3 passed by
 * address (class lostframe, BOOT_LOG 127). EU 001EAB14..001EAB38 (and the second, external-velocity
 * step after it): tmp = velocity * speed (00115D70 into sp+0x30), `ldm sp+0x30, {r0-r2}; stm sp+4`,
 * then position += sp+4 (00115C54). The text copied only y / z (local_1f28 = sp+8 / sp+0xC), so x at
 * sp+4 (stack0xffffe0d4) was stale: Link never moved along x under velocity (flow dash to Gulley,
 * BOOT_LOG 127). Confidence: high (instruction-level).
 * Second fix (BOOT_LOG 135): the ledge / step-down candidate (EU 1EC818..1EC844) read the x / y words
 * of sp+0 after 00115D70 / 00115C54 from stale registers: Link's position became (2e15, 13551, z)
 * at the yard's south ledge and he never dropped down. */
// MAPFIX-INTEGRATED v1
#include "ghidra_types.h"
extern void *stderr;
int fprintf(void *, const char *, ...);
char *getenv(const char *);

extern int func_00100118(int param_1,undefined *param_2,int param_3,int param_4);
extern int func_00100174(int *param_1);
extern int func_0010B3EC(undefined4 *param_1,int param_2,int param_3);
extern undefined8 func_0010E3C4(uint *param_1,uint *param_2,uint param_3);
extern int func_0010F1DC(int *param_1,int *param_2);
extern int func_00110490(undefined4 *param_1,uint param_2);
extern int func_00115C54(float *param_1,float *param_2,float *param_3);
extern int func_00115C70(float *param_1,float *param_2,float *param_3);
extern int func_00115D70(float *param_1,float *param_2,float fparam_0);
extern int func_00115D88(float *param_1,float *param_2,float fparam_0);
extern int func_00115DAC(float *param_1,float *param_2,float *param_3,float fparam_0);
extern float func_00115DC8(float *param_1);
extern int func_00115E10(float *param_1,float *param_2,float *param_3);
extern int func_001FE514(undefined4 *param_1,undefined4 *param_2);
extern int func_001FE680(undefined4 *param_1);
extern int func_002C1658(int param_1);
extern int func_002C1720(int param_1,uint *param_2,uint param_3);
extern float func_0042D5B0(float *param_1,float *param_2,float *param_3,float *param_4);
extern int func_004324A4(uint *param_1,float *param_2,uint param_3,float fparam_0);
extern int func_0043285C(uint *param_1,float fparam_0);
extern int func_00432DF0(uint *param_1,float *param_2);
extern int func_00432F74(undefined4 *param_1);
extern int func_0048C258(int *param_1,undefined *param_2);
extern int func_0048C320(int *param_1,int param_2,int param_3);
extern int func_0049F5FC(undefined4 *param_1);
extern int func_004A17A4(float *param_1);
extern int func_004A17E4(float *param_1);
extern int func_004AAD4C(int param_1,float *param_2,undefined4 *param_3,undefined4 param_4,uint param_5, uint param_6,float fparam_0,float fparam_1);
extern int func_004AAF44(int param_1,float *param_2,undefined4 *param_3,uint param_4,uint param_5, float fparam_0);
extern int func_00570D00(int param_1,int *param_2,int param_3,int param_4,int param_5,float fparam_0);
extern float func_0063AEA8(float ***********arg_r0,int arg_r1,int arg_r2,float fparam_0,float fparam_1);
extern int func_0064268C(float ***********arg_r0,int arg_r1,int arg_r2,float fparam_0,float fparam_1);
extern int func_00649E18(int param_1,float *param_2,float *param_3);
extern int func_0064B4A0(int param_1,float *param_2,float *param_3);
extern int func_0064CB28(int *param_1,int *param_2,int *param_3);
extern int func_0064E9F4(int *param_1,undefined4 *param_2,int *param_3);
extern int func_00650A24(int param_1,float *param_2,float *param_3);
extern float func_0068906C(float *param_1,float *param_2);
extern int func_0068B200(float *param_1,float *param_2);
extern undefined DAT_0000000f;
extern undefined DAT_00000030;
extern float *DAT_001eb3dc;
extern float * * * * * * *DAT_001eb3e0;
extern float * * * * * *DAT_001eb3e4;
extern float * * * * * *DAT_001eb3e8;
extern float DAT_001eb3ec;
extern int DAT_001eb3f0;
extern float DAT_001eb3f4;
extern uint DAT_001eb3f8;
extern undefined4 *DAT_001eb3fc;
extern undefined4 DAT_001eb400;
extern int *DAT_001eb404;
extern undefined4 *DAT_001ebb24;
extern float * * * * * * *DAT_001ebb28;
extern float * * * * * * *DAT_001ebb2c;
extern float DAT_001ebb30;
extern int DAT_001ebb34;
extern float DAT_001ebfa8;
extern float DAT_001ebfac;
extern float DAT_001ebfb0;
extern float *DAT_001ebfb4;
extern float DAT_001ebfb8;
extern float * * * * * * *DAT_001ebfbc;
extern float DAT_001ebfc0;
extern undefined *DAT_001ebfc4;
extern float DAT_001ec3b4;
extern float DAT_001ec3b8;
extern float DAT_001ec718;
extern float DAT_001ecc3c;
extern float DAT_001ecc40;
extern float DAT_001ecc44;
extern int DAT_001ecc48;
extern float DAT_001ed5b4;
extern float * * * * * * *DAT_001ed5b8;
extern float * * * * * * *DAT_001edb10;
extern int DAT_001edb14;
extern float * * * * * * *DAT_001edb18;
extern undefined *DAT_001edb1c;
extern int DAT_001eee2c;
extern float DAT_001eee30;
extern float * * * * * *DAT_001eee34;
extern uint *DAT_001ef398;
extern uint *DAT_001ef39c;
extern float DAT_001ef3a0;
extern undefined *DAT_001ef3a4;
extern uint *DAT_001ef3a8;
extern uint *DAT_001ef3ac;
extern undefined *DAT_001ef3b0;
extern int DAT_001ef3b4;
extern float DAT_001f0618;
extern float * * * * * * *DAT_001f061c;
extern uint *DAT_001f0620;
extern uint *DAT_001f0624;
extern uint *DAT_001f0628;
extern uint *DAT_001f062c;
extern float * * * * *DAT_001f0630;
extern int DAT_001f0634;
extern uint *DAT_001f0638;
extern uint *DAT_001f063c;
extern undefined *DAT_001f0640;
extern int DAT_001f0644;
extern int DAT_001f2904;
extern float * * * * * * *DAT_001f2908;
extern int DAT_001f290c;
extern int DAT_001f3a80;
extern float DAT_001f3a84;
extern int DAT_001f4bdc;
extern int DAT_001f4be0;
extern float * * * * * * *DAT_001f5fdc;
extern int DAT_001f5fe0;
extern float * * * * * * *DAT_001f5fe4;
extern undefined *DAT_001f5fe8;
extern int DAT_001f5fec;
extern float DAT_001f6730;
extern float * * * * * * *DAT_001f6734;
extern uint *DAT_001f6c00;
extern uint *DAT_001f6c04;
extern float DAT_001f6c08;
extern undefined *DAT_001f6c0c;
extern uint *DAT_001f6c10;
extern uint *DAT_001f6c14;
extern int DAT_001f6c18;
extern float DAT_001f7eb4;
extern float * * * * * * *DAT_001f7eb8;
extern uint *DAT_001f7ebc;
extern uint *DAT_001f7ec0;
extern uint *DAT_001f7ec4;
extern uint *DAT_001f7ec8;
extern undefined *DAT_001f7ecc;
extern float * * * * *DAT_001f7ed0;
extern int DAT_001f7ed4;
extern uint *DAT_001f7ed8;
extern uint *DAT_001f7edc;
extern int DAT_001f7ee0;
extern int DAT_001fa164;
extern float * * * * * * *DAT_001fa168;
extern int DAT_001fa16c;
extern int DAT_001fb1f4;
extern float DAT_001fb1f8;
extern int DAT_001fc480;
extern int DAT_001fc484;
extern float DAT_001fc488;
extern float DAT_001fccac;
extern float DAT_001fccb0;
extern float DAT_001fd184;
extern float DAT_001fd188;
extern float DAT_001fdb78;
extern int *DAT_001fdb7c;
extern uint *DAT_001fdb80;
extern uint *DAT_001fdb84;
extern uint *DAT_001fdb88;
extern uint DAT_001fdb8c;
extern undefined *DAT_001fdb90;
extern float DAT_001fdb94;
extern float *DAT_001fdb98;
extern uint *DAT_001fdb9c;
extern int *DAT_001fdba0;
extern float DAT_001fdba4;
extern float DAT_001fe138;
extern float DAT_001fe13c;
extern int DAT_001fe140;
extern float DAT_001fe144;
extern float DAT_001fe148;

// USA 001EA96C <- EU 001EA96C [EDITED REGION: check by hand] (Ghidra, non-matching)
// GLOBAL DAT_0000000f undefined
// GLOBAL DAT_00000030 undefined
// GLOBAL DAT_001eb3dc float *
// GLOBAL DAT_001eb3e0 float * * * * * * *
// GLOBAL DAT_001eb3e4 float * * * * * *
// GLOBAL DAT_001eb3e8 float * * * * * *
// GLOBAL DAT_001eb3ec float
// GLOBAL DAT_001eb3f0 int
// GLOBAL DAT_001eb3f4 float
// GLOBAL DAT_001eb3f8 uint
// GLOBAL DAT_001eb3fc undefined4 *
// GLOBAL DAT_001eb400 undefined4
// GLOBAL DAT_001eb404 int *
// GLOBAL DAT_001ebb24 undefined4 *
// GLOBAL DAT_001ebb28 float * * * * * * *
// GLOBAL DAT_001ebb2c float * * * * * * *
// GLOBAL DAT_001ebb30 float
// GLOBAL DAT_001ebb34 int
// GLOBAL DAT_001ebfa8 float
// GLOBAL DAT_001ebfac float
// GLOBAL DAT_001ebfb0 float
// GLOBAL DAT_001ebfb4 float *
// GLOBAL DAT_001ebfb8 float
// GLOBAL DAT_001ebfbc float * * * * * * *
// GLOBAL DAT_001ebfc0 float
// GLOBAL DAT_001ebfc4 undefined *
// GLOBAL DAT_001ec3b4 float
// GLOBAL DAT_001ec3b8 float
// GLOBAL DAT_001ec718 float
// GLOBAL DAT_001ecc3c float
// GLOBAL DAT_001ecc40 float
// GLOBAL DAT_001ecc44 float
// GLOBAL DAT_001ecc48 int
// GLOBAL DAT_001ed5b4 float
// GLOBAL DAT_001ed5b8 float * * * * * * *
// GLOBAL DAT_001edb10 float * * * * * * *
// GLOBAL DAT_001edb14 int
// GLOBAL DAT_001edb18 float * * * * * * *
// GLOBAL DAT_001edb1c undefined *
// GLOBAL DAT_001eee2c int
// GLOBAL DAT_001eee30 float
// GLOBAL DAT_001eee34 float * * * * * *
// GLOBAL DAT_001ef398 uint *
// GLOBAL DAT_001ef39c uint *
// GLOBAL DAT_001ef3a0 float
// GLOBAL DAT_001ef3a4 undefined *
// GLOBAL DAT_001ef3a8 uint *
// GLOBAL DAT_001ef3ac uint *
// GLOBAL DAT_001ef3b0 undefined *
// GLOBAL DAT_001ef3b4 int
// GLOBAL DAT_001f0618 float
// GLOBAL DAT_001f061c float * * * * * * *
// GLOBAL DAT_001f0620 uint *
// GLOBAL DAT_001f0624 uint *
// GLOBAL DAT_001f0628 uint *
// GLOBAL DAT_001f062c uint *
// GLOBAL DAT_001f0630 float * * * * *
// GLOBAL DAT_001f0634 int
// GLOBAL DAT_001f0638 uint *
// GLOBAL DAT_001f063c uint *
// GLOBAL DAT_001f0640 undefined *
// GLOBAL DAT_001f0644 int
// GLOBAL DAT_001f2904 int
// GLOBAL DAT_001f2908 float * * * * * * *
// GLOBAL DAT_001f290c int
// GLOBAL DAT_001f3a80 int
// GLOBAL DAT_001f3a84 float
// GLOBAL DAT_001f4bdc int
// GLOBAL DAT_001f4be0 int
// GLOBAL DAT_001f5fdc float * * * * * * *
// GLOBAL DAT_001f5fe0 int
// GLOBAL DAT_001f5fe4 float * * * * * * *
// GLOBAL DAT_001f5fe8 undefined *
// GLOBAL DAT_001f5fec int
// GLOBAL DAT_001f6730 float
// GLOBAL DAT_001f6734 float * * * * * * *
// GLOBAL DAT_001f6c00 uint *
// GLOBAL DAT_001f6c04 uint *
// GLOBAL DAT_001f6c08 float
// GLOBAL DAT_001f6c0c undefined *
// GLOBAL DAT_001f6c10 uint *
// GLOBAL DAT_001f6c14 uint *
// GLOBAL DAT_001f6c18 int
// GLOBAL DAT_001f7eb4 float
// GLOBAL DAT_001f7eb8 float * * * * * * *
// GLOBAL DAT_001f7ebc uint *
// GLOBAL DAT_001f7ec0 uint *
// GLOBAL DAT_001f7ec4 uint *
// GLOBAL DAT_001f7ec8 uint *
// GLOBAL DAT_001f7ecc undefined *
// GLOBAL DAT_001f7ed0 float * * * * *
// GLOBAL DAT_001f7ed4 int
// GLOBAL DAT_001f7ed8 uint *
// GLOBAL DAT_001f7edc uint *
// GLOBAL DAT_001f7ee0 int
// GLOBAL DAT_001fa164 int
// GLOBAL DAT_001fa168 float * * * * * * *
// GLOBAL DAT_001fa16c int
// GLOBAL DAT_001fb1f4 int
// GLOBAL DAT_001fb1f8 float
// GLOBAL DAT_001fc480 int
// GLOBAL DAT_001fc484 int
// GLOBAL DAT_001fc488 float
// GLOBAL DAT_001fccac float
// GLOBAL DAT_001fccb0 float
// GLOBAL DAT_001fd184 float
// GLOBAL DAT_001fd188 float
// GLOBAL DAT_001fdb78 float
// GLOBAL DAT_001fdb7c int *
// GLOBAL DAT_001fdb80 uint *
// GLOBAL DAT_001fdb84 uint *
// GLOBAL DAT_001fdb88 uint *
// GLOBAL DAT_001fdb8c uint
// GLOBAL DAT_001fdb90 undefined *
// GLOBAL DAT_001fdb94 float
// GLOBAL DAT_001fdb98 float *
// GLOBAL DAT_001fdb9c uint *
// GLOBAL DAT_001fdba0 int *
// GLOBAL DAT_001fdba4 float
// GLOBAL DAT_001fe138 float
// GLOBAL DAT_001fe13c float
// GLOBAL DAT_001fe140 int
// GLOBAL DAT_001fe144 float
// GLOBAL DAT_001fe148 float

/* WARNING: Removing unreachable block (ram,0x001f5dc4) */
/* WARNING: Removing unreachable block (ram,0x001ee644) */
/* WARNING: Removing unreachable block (ram,0x001ed37c) */
/* WARNING: Removing unreachable block (ram,0x001ed530) */
/* WARNING: Removing unreachable block (ram,0x001ee804) */
/* WARNING: Removing unreachable block (ram,0x001f5f78) */
/* WARNING: Type propagation algorithm not settling */
/* WARNING: Restarted to delay deadcode elimination for space: stack */

/* the player actor of the last update (debug: ALBW_ROUTE autopilot / ALBW_PLAYER_LOG in
 * platform.c read its position at +0x30) */
void *albw_dbg_player;

undefined4 func_001EA96C(float ***********param_1,float *param_2)

{
  albw_dbg_player = (void *)param_1;
  _Alignas(8) unsigned char albw_frame[0x1f30]; /* entry_sp - 0x1f30 .. entry_sp + 0x0 */
#define stack0xffffe0d0 (*(undefined4 *)(albw_frame + 0x0))
#define stack0xffffe0d4 (*(undefined4 *)(albw_frame + 0x4))
  char *pcVar1;
  float ******ppppppfVar2;
  float ******ppppppfVar3;
  uint *puVar4;
  float fVar5;
  int *piVar6;
  undefined4 *puVar7;
  float *pfVar8;
  float ***********pppppppppppfVar9;
  float **********ppppppppppfVar10;
  float **********ppppppppppfVar11;
  float **********ppppppppppfVar12;
  float ************ppppppppppppfVar13;
  float ********ppppppppfVar14;
  float ***********pppppppppppfVar15;
  uint uVar16;
  float ***********pppppppppppfVar17;
  undefined4 *puVar18;
  char cVar19;
  byte bVar20;
  int iVar21;
  float **********ppppppppppfVar22;
  float *********pppppppppfVar23;
  float *********pppppppppfVar24;
  float ***********extraout_r1;
  float ***********extraout_r1_00;
  float ***********extraout_r1_01;
  float ***********extraout_r1_02;
  float ***********extraout_r1_03;
  float ***********extraout_r1_04;
  float ***********extraout_r1_05;
  float ***********extraout_r1_06;
  float ***********extraout_r1_07;
  float ***********extraout_r1_08;
  float ***********extraout_r1_09;
  float ***********extraout_r1_10;
  float ***********extraout_r1_11;
  int *piVar25;
  uint *puVar26;
  uint *puVar27;
  float ***********pppppppppppfVar28;
  float ***********pppppppppppfVar29;
  float **********ppppppppppfVar30;
  uint uVar31;
  float *pfVar32;
  float ***********pppppppppppfVar33;
  float ***********pppppppppppfVar34;
  int iVar35;
  uint uVar36;
  int iVar37;
  float ***********pppppppppppfVar38;
  float *pfVar39;
  float ************ppppppppppppfVar40;
  float *****pppppfVar41;
  ushort uVar42;
  uint uVar43;
  bool bVar44;
  bool bVar45;
  bool bVar46;
  bool bVar47;
  float *********pppppppppfVar48;
  float ***********pppppppppppfVar49;
  float ***********pppppppppppfVar50;
  float *******pppppppfVar51;
  float ***********pppppppppppfVar52;
  float ***********pppppppppppfVar53;
  float fVar54;
  float fVar55;
  float **********ppppppppppfVar56;
  float fVar57;
  float fVar58;
  float fVar59;
  float fVar60;
  float ***********pppppppppppfVar61;
  float ***********pppppppppppfVar62;
  float ***********pppppppppppfVar63;
  float *********pppppppppfVar64;
  float fVar65;
  float *********pppppppppfVar66;
  undefined8 uVar67;
  float **********ppppppppppfVar73;
  float ***********pppppppppppfVar74;
  ulonglong uVar68;
  longlong lVar69;
  float ***********pppppppppppfVar75;
  ulonglong uVar70;
  undefined8 uVar71;
  undefined8 uVar72;
  float *********pppppppppfVar76;
  float ***********pppppppppppfVar78;
  undefined8 uVar77;
  float ***********pppppppppppfVar79;
  undefined4 uVar80;
  float ***********pppppppppppfVar81;
  undefined8 in_d11;
  float ***********pppppppppppfVar83;
  longlong lVar82;
  undefined8 uVar84;
  undefined8 in_d15;
  float ************ppppppppppppfVar85;
  #define in_stack_ffffe0d4 (*(float *********** *)(albw_frame + 0x4)) /* V4: frame slot (callee-filled) */
  /* sp+0: x of a vector a call just wrote with &stack0xffffe0d0 (y is in_stack_ffffe0d4 = sp+4, z is
   * local_1f28[0] = sp+8). The Ghidra text read x / y from stale variables after those calls. */
/* WALKER: a painting segment is kept only when its start (+0x70) and end (+0x74) differ by more
 * than 2^-23. The ARM tests the bits of (start - end) against 0xB4000000 / 0x34000000 (= -/+2^-23);
 * the converted text cast the float's value to an int instead, so every short segment read as 0 and
 * was dropped: Link's wall painting got no strip to draw. */
#define WALK_SEG_HAS_LENGTH(a, b) \
  ((ALBW_AS_F32((a)[0x1c]) - ALBW_AS_F32((b)[0x1d])) < -1.1920929e-7f || \
   (ALBW_AS_F32((a)[0x1c]) - ALBW_AS_F32((b)[0x1d])) > 1.1920929e-7f)
#define sp_x (*(float *********** *)(albw_frame + 0x0))
  #define local_1f28 ((float *********** *)(albw_frame + 0x8))
  #define local_1f14 (*(float *********** *)(albw_frame + 0x1c))
  #define local_1f10 (*(float *********** *)(albw_frame + 0x20))
  #define local_1f0c (*(float *********** *)(albw_frame + 0x24))
  #define local_1f08 (*(float *********** *)(albw_frame + 0x28))
  #define local_1f04 (*(float *********** *)(albw_frame + 0x2c))
  #define local_1f00 ((float *********** *)(albw_frame + 0x30))
  #define local_1ef4 (*(float ********** *)(albw_frame + 0x3c))
  #define local_1ef0 (*(float ************ *)(albw_frame + 0x40))
  #define local_1eec (*(float *********** *)(albw_frame + 0x44))
  #define local_1ee8 (*(float ************ *)(albw_frame + 0x48))
  #define local_1ee4 (*(float *********** *)(albw_frame + 0x4c))
  #define local_1ee0 (*(float ************ *)(albw_frame + 0x50))
  #define local_1edc (*(float *********** *)(albw_frame + 0x54))
  #define local_1ed8 (*(float *********** *)(albw_frame + 0x58))
  #define local_1ed4 (*(float *********** *)(albw_frame + 0x5c))
  #define local_1ed0 (*(float *********** *)(albw_frame + 0x60))
  #define local_1ecc (*(float ************ *)(albw_frame + 0x64))
  #define local_1ec8 (*(float *********** *)(albw_frame + 0x68))
  #define local_1ec4 (*(float ************ *)(albw_frame + 0x6c))
  #define local_1ec0 (*(float ************ *)(albw_frame + 0x70))
  #define local_1ebc (*(float *********** *)(albw_frame + 0x74))
  #define local_1eb8 (*(float *********** *)(albw_frame + 0x78))
  #define local_1eb4 (*(float ************ *)(albw_frame + 0x7c))
  #define local_1eb0 (*(float *********** *)(albw_frame + 0x80))
  #define local_1eac (*(float ************ *)(albw_frame + 0x84))
  #define local_1ea8 (*(float *********** *)(albw_frame + 0x88))
  #define local_1ea4 (*(float *********** *)(albw_frame + 0x8c))
  #define local_1ea0 (*(float *********** *)(albw_frame + 0x90))
  #define local_1e9c (*(float *********** *)(albw_frame + 0x94))
  #define local_1e98 (*(float *********** *)(albw_frame + 0x98))
  #define local_1e94 (*(float *********** *)(albw_frame + 0x9c))
  #define local_1e90 (*(float *********** *)(albw_frame + 0xa0))
  #define local_1e8c (*(float *********** *)(albw_frame + 0xa4))
  #define local_1e88 (*(float *********** *)(albw_frame + 0xa8))
  #define local_1e84 (*(float *********** *)(albw_frame + 0xac))
  #define local_1e80 (*(float *********** *)(albw_frame + 0xb0))
  #define local_1e7c (*(float *********** *)(albw_frame + 0xb4))
  #define local_1e78 (*(float *********** *)(albw_frame + 0xb8))
  #define local_1e74 (*(float *********** *)(albw_frame + 0xbc))
  #define local_1e70 (*(float *********** *)(albw_frame + 0xc0))
  #define local_1e6c (*(float *********** *)(albw_frame + 0xc4))
  #define local_1e68 (*(float *********** *)(albw_frame + 0xc8))
  #define local_1e64 (*(float *********** *)(albw_frame + 0xcc))
  #define local_1e60 (*(float *********** *)(albw_frame + 0xd0))
  #define local_1e5c (*(float *********** *)(albw_frame + 0xd4))
  #define local_1e58 (*(float ************ *)(albw_frame + 0xd8))
  #define local_1e54 (*(float *********** *)(albw_frame + 0xdc))
  #define local_1e50 (*(float *********** *)(albw_frame + 0xe0))
  #define local_1e4c (*(float *********** *)(albw_frame + 0xe4))
  #define local_1e48 (*(float *********** *)(albw_frame + 0xe8))
  #define local_1e44 (*(float *********** *)(albw_frame + 0xec))
  #define local_1e40 (*(float *********** *)(albw_frame + 0xf0))
  #define local_1e3c (*(float *********** *)(albw_frame + 0xf4))
  #define local_1e38 (*(float *********** *)(albw_frame + 0xf8))
  #define local_1e34 (*(float *********** *)(albw_frame + 0xfc))
  #define local_1e30 (*(float *********** *)(albw_frame + 0x100))
  #define local_1e2c (*(float *********** *)(albw_frame + 0x104))
  #define local_1e28 (*(uint * *)(albw_frame + 0x108))
  #define local_1e24 (*(float *********** *)(albw_frame + 0x10c))
  #define local_1e20 (*(float *********** *)(albw_frame + 0x110))
  #define local_1e1c (*(float *********** *)(albw_frame + 0x114))
  #define local_1e18 (*(float *********** *)(albw_frame + 0x118))
  #define local_1e14 (*(float ************ *)(albw_frame + 0x11c))
  #define local_1e10 (*(float ************ *)(albw_frame + 0x120))
  #define local_1e0c (*(float *********** *)(albw_frame + 0x124))
  #define local_1e08 (*(float *********** *)(albw_frame + 0x128))
  #define local_1e04 (*(float *********** *)(albw_frame + 0x12c))
  #define local_1e00 (*(float *********** *)(albw_frame + 0x130))
  #define local_1dfc (*(float *********** *)(albw_frame + 0x134))
  #define local_1df8 (*(float *********** *)(albw_frame + 0x138))
  #define local_1df4 (*(float *********** *)(albw_frame + 0x13c))
  #define local_1df0 (*(float *********** *)(albw_frame + 0x140))
  #define local_1dec (*(float *********** *)(albw_frame + 0x144))
  #define local_1de8 (*(float *********** *)(albw_frame + 0x148))
  #define local_1de4 (*(float ********** *)(albw_frame + 0x14c))
  #define local_1de0 (*(float *********** *)(albw_frame + 0x150))
  #define local_1ddc (*(float *********** *)(albw_frame + 0x154))
  #define local_1dd8 ((float *********** *)(albw_frame + 0x158))
  #define local_1dcc (*(float ********** *)(albw_frame + 0x164))
  #define local_1dc8 (*(float *********** *)(albw_frame + 0x168))
  #define local_1dc4 (*(float *********** *)(albw_frame + 0x16c))
  #define local_1dc0 (*(float *********** *)(albw_frame + 0x170))
  #define local_1dbc (*(float ************ *)(albw_frame + 0x174))
  #define local_1db8 (*(float ************ *)(albw_frame + 0x178))
  #define local_1db4 (*(float *********** *)(albw_frame + 0x17c))
  #define local_1db0 (*(float *********** *)(albw_frame + 0x180))
  #define local_1dac (*(float *********** *)(albw_frame + 0x184))
  #define local_1da8 (*(float *********** *)(albw_frame + 0x188))
  #define local_1da4 (*(float *********** *)(albw_frame + 0x18c))
  #define local_1da0 (*(float *********** *)(albw_frame + 0x190))
  #define local_1d9c (*(float *********** *)(albw_frame + 0x194))
  #define local_1d98 (*(float *********** *)(albw_frame + 0x198))
  #define local_1d94 (*(float ************ *)(albw_frame + 0x19c))
  #define local_1d90 (*(float *)(albw_frame + 0x1a0))
  #define local_1d8c (*(float *)(albw_frame + 0x1a4))
  #define local_1d88 (*(float *)(albw_frame + 0x1a8))
  #define local_1d84 (*(float *********** *)(albw_frame + 0x1ac))
  #define local_1d80 (*(float ********** *)(albw_frame + 0x1b0))
  #define local_1d7c (*(float ************ *)(albw_frame + 0x1b4))
  #define local_1d78 (*(float ************ *)(albw_frame + 0x1b8))
  #define local_1d74 (*(float ************ *)(albw_frame + 0x1bc))
  #define local_1d70 ((float *********** *)(albw_frame + 0x1c0))
  #define local_1d64 (*(float ********** *)(albw_frame + 0x1cc))
  #define local_1d60 (*(float ********** *)(albw_frame + 0x1d0))
  #define local_1d5c (*(float ********** *)(albw_frame + 0x1d4))
  #define local_1d58 (*(float ********** *)(albw_frame + 0x1d8))
  #define local_1d54 (*(float ********** *)(albw_frame + 0x1dc))
  #define local_1d50 (*(float ********** *)(albw_frame + 0x1e0))
  #define local_1d4c (*(float *********** *)(albw_frame + 0x1e4))
  #define local_1d48 (*(float *********** *)(albw_frame + 0x1e8))
  #define local_1d44 (*(float *********** *)(albw_frame + 0x1ec))
  #define local_1d40 (*(float *********** *)(albw_frame + 0x1f0))
  #define local_30 (*(float *********** *)(albw_frame + 0x1f00))
  #define local_2c (*(float * *)(albw_frame + 0x1f04))
  /* WALKER: local_1e28 points at MODE (+0x3BAE); local_2c is the input block, local_30 the walker. Step 1 follows: already merged -> follow the wall and return */
  local_1e28 = (uint *)((int)param_1 + 0x3bae);
  local_1e58 = (float ************)(param_1 + 0xed3);
  local_1de0 = param_1 + 0xed6;
  local_1dfc = (float ***********)(uint)*(byte *)local_1e28;
  local_30 = param_1;
  local_2c = param_2;
  if ((*(char *)((int)param_2 + 0x95) != '\0' && local_1dfc != (float ***********)0x0) &&
     (puVar4 = (uint *)func_002C1658((int)ALBW_AS_U32(param_2[0x1a])), pppppppppppfVar52 = local_30, *puVar4 != 0
     )) {
    if (getenv("ALBW_WALK_TRACE")) {   /* WALKER debug: step 1 (stay merged): position and step along the wall */
      extern long g_plat_frames;
      fprintf(stderr, "walk: f=%ld stay walker %p pos (%.3f %.3f %.3f) step %.4f %.4f vel (%.3f %.3f %.3f) row0 (%.3f %.3f %.3f)\n",
              g_plat_frames, (void *)local_30, local_2c[0], local_2c[1], local_2c[2], local_2c[0xc], local_2c[0xd],
              local_2c[0xe], local_2c[0xf], local_2c[0x10], local_2c[3], local_2c[4], local_2c[5]);
    }
    local_30[0x12] = (float **********)ALBW_PTR_BITS(local_2c[3]);
    local_30[0x13] = (float **********)ALBW_PTR_BITS(local_2c[4]);
    local_30[0x14] = (float **********)ALBW_PTR_BITS(local_2c[5]);
    param_1[0x15] = (float **********)ALBW_PTR_BITS(local_2c[6]);
    param_1[0x16] = (float **********)ALBW_PTR_BITS(local_2c[7]);
    param_1[0x17] = (float **********)ALBW_PTR_BITS(local_2c[8]);
    local_30[0x18] = (float **********)ALBW_PTR_BITS(local_2c[9]);
    local_30[0x19] = (float **********)ALBW_PTR_BITS(local_2c[10]);
    local_30[0x1a] = (float **********)ALBW_PTR_BITS(local_2c[0xb]);
    local_1f28[3] = (float ***********)local_30[0xc];
    local_1f28[4] = (float ***********)local_30[0xd];
    local_1f14 = (float ***********)local_30[0xe];
    func_00115C54((float *)local_1f00,local_2c,local_2c + 0xe);
    local_1f28[0] = local_1f00[1];
    local_1f28[1] = local_1f00[2];
    pppppppppppfVar52[0xc] = (float **********)local_1f00[0];
    pppppppppppfVar52[0xd] = (float **********)local_1f00[1];
    pppppppppppfVar52[0xe] = (float **********)local_1f00[2];
    pppppppppppfVar52 = local_30 + 0xc;
    func_00115D70((float *)local_1f00,(float *)(local_30 + 0x12),local_2c[0xc]);
    stack0xffffe0d4 = (undefined4)(uintptr_t)local_1f00[0];   /* lost x store (EU 001EAB24 ldm/stm) */
    local_1f28[0] = local_1f00[1];
    local_1f28[1] = local_1f00[2];
    func_00115C54((float *)pppppppppppfVar52,(float *)pppppppppppfVar52,(float *)&stack0xffffe0d4);
    pppppppppppfVar52 = local_30 + 0xc;
    func_00115D70((float *)local_1f00,(float *)(local_30 + 0x15),local_2c[0xd]);
    stack0xffffe0d4 = (undefined4)(uintptr_t)local_1f00[0];   /* lost x store (EU 001EAB24 ldm/stm) */
    local_1f28[0] = local_1f00[1];
    local_1f28[1] = local_1f00[2];
    func_00115C54((float *)pppppppppppfVar52,(float *)pppppppppppfVar52,(float *)&stack0xffffe0d4);
    local_30[0xed0] = local_30[0xc];
    local_30[0xed1] = local_30[0xd];
    local_30[0xed2] = local_30[0xe];
    *local_1e58 = (float ***********)local_30[0x12];
    local_1e58[1] = (float ***********)local_30[0x13];
    local_1e58[2] = (float ***********)local_30[0x14];
    *local_1de0 = local_30[0x15];
    local_1de0[1] = local_30[0x16];
    local_1de0[2] = local_30[0x17];
    local_1e58[6] = (float ***********)local_30[0x18];
    local_1e58[7] = (float ***********)local_30[0x19];
    local_1e58[8] = (float ***********)local_30[0x1a];
    local_30[0xee5] = local_30[0x18];
    local_30[0xee6] = local_30[0x19];
    local_30[0xee7] = local_30[0x1a];
    local_30[0xee2] = local_30[0xee5];
    local_30[0xee3] = local_30[0xee6];
    local_30[0xee4] = local_30[0xee7];
    fVar5 = local_2c[0x1a];
    *(uint *)((int)ALBW_AS_U32(fVar5) + 0x164) = *(uint *)((int)ALBW_AS_U32(fVar5) + 0x164) | 0x40000000;
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x80) = local_30[0x12];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x84) = local_30[0x15];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x88) = local_30[0x18];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x8c) = local_30[0xc];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x90) = local_30[0x13];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x94) = local_30[0x16];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x98) = local_30[0x19];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x9c) = local_30[0xd];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0xa0) = local_30[0x14];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0xa4) = local_30[0x17];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0xa8) = local_30[0x1a];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0xac) = local_30[0xe];
    func_00115C70((float *)local_1f00,(float *)(local_30 + 0xc),(float *)(local_1f28 + 3));
    local_1f10 = local_1f00[0];
    local_1f0c = local_1f00[1];
    local_1f08 = local_1f00[2];
    return func_00432DF0(puVar4,(float *)&local_1f10);
  }
  pppppppppppfVar15 = local_30;
  pfVar39 = DAT_001eb3dc;
  cVar19 = *(char *)(local_2c + 0x25);
  local_1e2c = (float ***********)((int)local_30 + 0x3dc3);
  pppppppppppfVar52 = local_1e2c;
  if (cVar19 != '\0') {
    pppppppppppfVar52 = local_1dfc;
  }
  if (cVar19 != '\0' && pppppppppppfVar52 != (float ***********)0x0) {
    *(char *)local_1e2c = cVar19;
  }
  piVar6 = (int *)ALBW_PTR_BITS(*pfVar39);
  iVar21 = 0;
  if (0 < *piVar6) {
    do {
      piVar25 = *(int **)(piVar6[2] + iVar21 * 4);
      iVar21 = iVar21 + 1;
      *piVar25 = piVar6[3];
      piVar6[3] = (int)piVar25;
    } while (iVar21 < *piVar6);
  }
  *piVar6 = 0;
  iVar21 = 0;
  if (0 < piVar6[5]) {
    do {
      piVar25 = *(int **)(piVar6[7] + iVar21 * 4);
      iVar21 = iVar21 + 1;
      *piVar25 = piVar6[8];
      piVar6[8] = (int)piVar25;
    } while (iVar21 < piVar6[5]);
  }
  piVar6[5] = 0;
  piVar6[10] = 0;
  local_1e10 = (float ************)local_30[0xed0];
  local_1de4 = local_30[0xed1];
  local_1e0c = (float ***********)local_30[0xed2];
  local_1e24 = (float ***********)local_30[0xed3];
  local_1df4 = (float ***********)local_30[0xed4];
  local_1e20 = (float ***********)local_30[0xed5];
  local_1df0 = (float ***********)local_30[0xed6];
  local_1dec = (float ***********)local_30[0xed7];
  local_1e1c = (float ***********)local_30[0xed8];
  local_1e18 = (float ***********)local_30[0xed9];
  local_1e14 = (float ************)local_30[0xeda];
  local_1de8 = (float ***********)local_30[0xedb];
  local_30[0x12] = (float **********)ALBW_PTR_BITS(local_2c[3]);
  local_30[0x13] = (float **********)ALBW_PTR_BITS(local_2c[4]);
  local_30[0x14] = (float **********)ALBW_PTR_BITS(local_2c[5]);
  param_1[0x15] = (float **********)ALBW_PTR_BITS(local_2c[6]);
  param_1[0x16] = (float **********)ALBW_PTR_BITS(local_2c[7]);
  param_1[0x17] = (float **********)ALBW_PTR_BITS(local_2c[8]);
  local_30[0x18] = (float **********)ALBW_PTR_BITS(local_2c[9]);
  local_30[0x19] = (float **********)ALBW_PTR_BITS(local_2c[10]);
  local_30[0x1a] = (float **********)ALBW_PTR_BITS(local_2c[0xb]);
  pppppppppppfVar52 = local_30 + 0xc;
  func_00115C54((float *)(local_1f28 + 1),local_2c,local_2c + 0xe);
  *pppppppppppfVar52 = (float **********)local_1f28[1];
  pppppppppppfVar15[0xd] = (float **********)local_1f28[2];
  pppppppppppfVar15[0xe] = (float **********)local_1f28[3];
  local_30[0x1b] = (float **********)ALBW_PTR_BITS((local_2c[0x14] + local_2c[0x15]));
  local_30[0x1c] = (float **********)ALBW_PTR_BITS(local_2c[0x13]);
  if (local_2c[0x1d] == 0.0) {
    ppppppppppfVar22 = (float **********)0x0;
  }
  else {
    ppppppppppfVar22 = (float **********)(local_2c + 0x1d);
  }
  local_30[0xeec] = ppppppppppfVar22;
  func_0049F5FC(local_30 + 2);
  func_0049F5FC(local_30 + 6);
  pppppppppppfVar52 = local_30;
  /* WALKER: step 2: not merged (or merge not kept): MODE = 0 (free), then look for a wall */
  *(byte *)local_1e28 = 0;
  local_1e50 = local_30 + 0xec0;
  ppppppppppfVar22 = (float **********)0x0;
  if ((float *)ALBW_PTR_BITS(local_2c[0x27]) != (float *)0x0) {
    ppppppppppfVar22 = *(float ***********)ALBW_PTR_BITS(local_2c[0x27]);
  }
  local_30[0xee9] = ppppppppppfVar22;
  if ((float *)ALBW_PTR_BITS(local_2c[0x28]) == (float *)0x0) {
    ppppppppppfVar22 = (float **********)0xffffffff;
  }
  else {
    ppppppppppfVar22 = *(float ***********)ALBW_PTR_BITS(local_2c[0x28]);
  }
  local_1ddc = local_30 + 0xeea;
  *local_1ddc = ppppppppppfVar22;
  *(char *)((int)local_30 + 0x3dc2) = '\0';
  fVar5 = DAT_001eb3ec;
  ppppppfVar3 = DAT_001eb3e8;
  ppppppfVar2 = DAT_001eb3e4;
  pppppppppppfVar15 = (float ***********)DAT_001eb3e0;
  if ((local_2c[0x27] == 0.0) || (local_2c[0x28] == 0.0)) {
    cVar19 = '\x01';
  }
  else {
    cVar19 = '\0';
  }
  lVar69 = ZEXT48(DAT_001eb3e8) << 0x20;
  uVar70 = ZEXT48(DAT_001eb3e4);
  *(char *)(local_30 + 0xf71) = cVar19;
  local_30[0x1e] = (float **********)pppppppppppfVar15;
  local_30[10] = (float **********)0x0;
  fVar55 = local_2c[0x18];
  local_30[0x1f] = (float **********)ppppppfVar2;
  local_30[0x20] = (float **********)ppppppfVar2;
  local_1f28[1] = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(local_30[0x1b]) * fVar55));
  uVar67 = CONCAT44(fVar5,local_1f28[1]);
  local_30[0x21] = (float **********)ppppppfVar2;
  local_30[0x22] = (float **********)ppppppfVar3;
  local_30[0x23] = (float **********)ppppppfVar3;
  local_30[0x24] = (float **********)ppppppfVar3;
  local_1f28[3] = local_1f28[1];
  if (local_2c[0xd] <= ALBW_AS_F32(pppppppppppfVar15)) {
    local_1f28[2] = (float ***********)ALBW_PTR_BITS(local_2c[0x13]);
    func_0068B200((float *)(local_30 + 0x1f),(float *)(local_1f28 + 1));
  }
  else {
    local_1f28[2] = (float ***********)ALBW_PTR_BITS((local_2c[0x13] + local_2c[0xd] * fVar5));
    func_0068B200((float *)(local_30 + 0x1f),(float *)(local_1f28 + 1));
  }
  if (ALBW_AS_F32(pppppppppppfVar15) <= local_2c[0xd]) {
    local_1f28[1] = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(uVar67));
    local_1f28[2] = pppppppppppfVar15;
    local_1f28[3] = local_1f28[1];
    func_0068B200((float *)(pppppppppppfVar52 + 0x1f),(float *)(local_1f28 + 1));
    fVar5 = ALBW_AS_F32(uVar67);
  }
  else {
    local_1f28[2] = (float ***********)ALBW_PTR_BITS((local_2c[0xd] * ALBW_AS_F32(((ulonglong)uVar67 >> 0x20))));
    local_1f28[1] = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(uVar67));
    local_1f28[3] = local_1f28[1];
    func_0068B200((float *)(pppppppppppfVar52 + 0x1f),(float *)(local_1f28 + 1));
    fVar5 = ALBW_AS_F32(uVar67);
  }
  iVar21 = DAT_001eb3f0;
  pfVar39 = DAT_001eb3dc;
  pppppppppppfVar81 = (float ***********)uVar70;
  pppppppppppfVar52[0x1f] =
       (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar52[0x1f]) + ALBW_AS_F32(pppppppppppfVar52[0xc])));
  pppppppppppfVar52[0x20] =
       (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar52[0x20]) + ALBW_AS_F32(pppppppppppfVar52[0xd])));
  pppppppppppfVar52[0x21] =
       (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar52[0x21]) + ALBW_AS_F32(pppppppppppfVar52[0xe])));
  fVar55 = DAT_001eb3f4;
  pppppppppppfVar52[0x22] =
       (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar52[0x22]) + ALBW_AS_F32(pppppppppppfVar52[0xc])));
  pppppppppppfVar52[0x23] =
       (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar52[0x23]) + ALBW_AS_F32(pppppppppppfVar52[0xd])));
  pppppppppppfVar52[0x24] =
       (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar52[0x24]) + ALBW_AS_F32(pppppppppppfVar52[0xe])));
  *pppppppppppfVar52 = (float **********)ALBW_PTR_BITS(*pfVar39);
  pppppppppppfVar52[1] = (float **********)((int)ALBW_AS_U32(*pfVar39) + 0x14);
  pcVar1 = (char *)(iVar21 + (int)pppppppppppfVar52);
  pcVar1[0] = '\0';
  pcVar1[1] = '\0';
  pcVar1[2] = '\0';
  pcVar1[3] = '\0';
  local_1f00[0] =
       (float ***********)
       ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar52[0x1f]) + ALBW_AS_F32(pppppppppppfVar52[0x22])) * fVar55));
  local_1f00[1] =
       (float ***********)
       ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar52[0x20]) + ALBW_AS_F32(pppppppppppfVar52[0x23])) * fVar55));
  local_1f00[2] =
       (float ***********)
       ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar52[0x21]) + ALBW_AS_F32(pppppppppppfVar52[0x24])) * fVar55));
  ppppppppppfVar73 =
       (float **********)
       ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar52[0x22]) - ALBW_AS_F32(pppppppppppfVar52[0x1f])) * fVar55));
  ppppppppppfVar22 =
       (float **********)
       ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar52[0x23]) - ALBW_AS_F32(pppppppppppfVar52[0x20])) * fVar55));
  ppppppppppfVar56 =
       (float **********)
       ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar52[0x24]) - ALBW_AS_F32(pppppppppppfVar52[0x21])) * fVar55));
  if (ALBW_AS_F32(ppppppppppfVar22) < ALBW_AS_F32(ppppppppppfVar73)) {
    ppppppppppfVar22 = ppppppppppfVar73;
  }
  if (ALBW_AS_F32(ppppppppppfVar22) <= ALBW_AS_F32(ppppppppppfVar56)) {
    ppppppppppfVar22 = ppppppppppfVar56;
  }
  pppppppppppfVar52[0x25] = (float **********)local_1f00[0];
  pppppppppppfVar52[0x26] = (float **********)local_1f00[1];
  pppppppppppfVar52[0x27] = (float **********)local_1f00[2];
  pppppppppppfVar52[0x28] = ppppppppppfVar22;
  /* WALKER: step 2: collect wall faces near the position (collision query, or the faces given by input +0x90) */
  if (local_2c[0x24] == 0.0) {
    local_1f28[4] = (float ***********)*DAT_001eb3fc;
    local_1f14 = (float ***********)DAT_001eb3fc[1];
    local_1f28[3] = pppppppppppfVar52;
    local_1f28[2] = (float ***********)DAT_001eb400;
    in_stack_ffffe0d4 = (float ***********)(int)*(char *)((int)local_2c + 0x97);
    func_004AAD4C(*DAT_001eb404,(float *)local_1f00,local_1f28 + 2,0x1f,0xf,(uint)in_stack_ffffe0d4,
                  ALBW_AS_F32(ppppppppppfVar22),fVar5);
  }
  else {
    uVar36 = 0;
    uVar68 = (ulonglong)DAT_001eb3f8;
    while( true ) {
      pppppppppppfVar81 = (float ***********)uVar70;
      puVar4 = (uint *)ALBW_PTR_BITS(local_2c[0x24]);
      if ((int)*puVar4 <= (int)uVar36) break;
      if (uVar36 < *puVar4) {
        pfVar39 = (float *)(puVar4[2] + uVar36 * 0x70);
      }
      else {
        pfVar39 = (float *)puVar4[2];
      }
      ppppppppppfVar56 = pppppppppppfVar52[0x28];
      func_00115C70((float *)&local_1ef4,(float *)(pppppppppppfVar52 + 0x25),pfVar39 + 7);
      if (((((ALBW_AS_F32(local_1ef4) * pfVar39[0xd] + ALBW_AS_F32(local_1ef0) * pfVar39[0xe] +
              ALBW_AS_F32(local_1eec) * pfVar39[0xf] <= ALBW_AS_F32(ppppppppppfVar56)) &&
            (ALBW_AS_F32(local_1ef4) * pfVar39[0x10] + ALBW_AS_F32(local_1ef0) * pfVar39[0x11] +
             ALBW_AS_F32(local_1eec) * pfVar39[0x12] <= ALBW_AS_F32(ppppppppppfVar56))) &&
           (ALBW_AS_F32(local_1ef4) * pfVar39[0x13] + ALBW_AS_F32(local_1ef0) * pfVar39[0x14] +
            ALBW_AS_F32(local_1eec) * pfVar39[0x15] <= pfVar39[0x18] + ALBW_AS_F32(ppppppppppfVar56))) &&
          ((fVar5 = -(ALBW_AS_F32(local_1ef4) * pfVar39[10] + ALBW_AS_F32(local_1ef0) * pfVar39[0xb]) +
                    -(ALBW_AS_F32(local_1eec) * pfVar39[0xc]), -ALBW_AS_F32(ppppppppppfVar56) <= fVar5 &&
           (fVar5 <= ALBW_AS_F32(ppppppppppfVar56))))) && (fVar5 != -3.4028235e+38)) {
        ppppppppppfVar56 = *pppppppppppfVar52;
        if ((int)*ppppppppppfVar56 < (int)ppppppppppfVar56[1]) {
          ppppppppppfVar73 = (float **********)ppppppppppfVar56[3];
          if (ppppppppppfVar73 != (float **********)0x0) {
            ppppppppppfVar56[3] = *ppppppppppfVar73;
            func_00110490(ppppppppppfVar73,0x80);
            *ppppppppppfVar73 = (float *********)0x0;
            ppppppppppfVar73[2] = (float *********)0x0;
            ppppppppppfVar73[3] = (float *********)0x0;
            ppppppppppfVar73[4] = (float *********)0x0;
            ppppppppppfVar73[8] = (float *********)uVar68;
            local_1f28[3] = (float ***********)0x1f;
            *(undefined2 *)(ppppppppppfVar73 + 0x1b) = 0x1f;
            ppppppppppfVar73[0x1c] = (float *********)0x0;
            ppppppppppfVar73[0x1d] = (float *********)0x0;
            ppppppppppfVar73[0x1e] = (float *********)0x0;
            ppppppppppfVar73[0x1f] = (float *********)0x0;
          }
          if ((int)*ppppppppppfVar56 < (int)ppppppppppfVar56[1]) {
            ppppppppppfVar56[2][(int)*ppppppppppfVar56] = (float ********)ppppppppppfVar73;
            *ppppppppppfVar56 = (float *********)((int)*ppppppppppfVar56 + 1);
          }
        }
        else {
          ppppppppppfVar73 = (float **********)0x0;
        }
        if (((pppppppppppfVar52[10] == (float **********)0x0) &&
            ((float **********)ALBW_PTR_BITS(pfVar39[0x1a]) == pppppppppppfVar52[0xee9])) &&
           ((float **********)ALBW_PTR_BITS(pfVar39[2]) == pppppppppppfVar52[0xeea])) {
          pppppppppppfVar52[10] = ppppppppppfVar73;
        }
        ppppppppppfVar73[2] = (float *********)ALBW_PTR_BITS(*pfVar39);
        ppppppppppfVar73[3] = (float *********)ALBW_PTR_BITS(pfVar39[1]);
        ppppppppppfVar73[4] = (float *********)ALBW_PTR_BITS(pfVar39[2]);
        ppppppppppfVar73[5] = (float *********)ALBW_PTR_BITS(pfVar39[3]);
        ppppppppppfVar73[6] = (float *********)ALBW_PTR_BITS(pfVar39[4]);
        ppppppppppfVar73[7] = (float *********)ALBW_PTR_BITS(pfVar39[5]);
        ppppppppppfVar73[8] = (float *********)ALBW_PTR_BITS(pfVar39[6]);
        ppppppppppfVar73[9] = (float *********)ALBW_PTR_BITS(pfVar39[7]);
        ppppppppppfVar73[10] = (float *********)ALBW_PTR_BITS(pfVar39[8]);
        ppppppppppfVar73[0xb] = (float *********)ALBW_PTR_BITS(pfVar39[9]);
        ppppppppppfVar73[0xc] = (float *********)ALBW_PTR_BITS(pfVar39[10]);
        ppppppppppfVar73[0xd] = (float *********)ALBW_PTR_BITS(pfVar39[0xb]);
        ppppppppppfVar73[0xe] = (float *********)ALBW_PTR_BITS(pfVar39[0xc]);
        ppppppppppfVar73[0xf] = (float *********)ALBW_PTR_BITS(pfVar39[0xd]);
        ppppppppppfVar73[0x10] = (float *********)ALBW_PTR_BITS(pfVar39[0xe]);
        ppppppppppfVar73[0x11] = (float *********)ALBW_PTR_BITS(pfVar39[0xf]);
        ppppppppppfVar73[0x12] = (float *********)ALBW_PTR_BITS(pfVar39[0x10]);
        ppppppppppfVar73[0x13] = (float *********)ALBW_PTR_BITS(pfVar39[0x11]);
        ppppppppppfVar73[0x14] = (float *********)ALBW_PTR_BITS(pfVar39[0x12]);
        ppppppppppfVar73[0x15] = (float *********)ALBW_PTR_BITS(pfVar39[0x13]);
        ppppppppppfVar73[0x16] = (float *********)ALBW_PTR_BITS(pfVar39[0x14]);
        ppppppppppfVar73[0x17] = (float *********)ALBW_PTR_BITS(pfVar39[0x15]);
        ppppppppppfVar73[0x18] = (float *********)ALBW_PTR_BITS(pfVar39[0x16]);
        ppppppppppfVar73[0x19] = (float *********)ALBW_PTR_BITS(pfVar39[0x17]);
        ppppppppppfVar73[0x1a] = (float *********)ALBW_PTR_BITS(pfVar39[0x18]);
        *(undefined2 *)(ppppppppppfVar73 + 0x1b) = *(undefined2 *)(pfVar39 + 0x19);
        *ppppppppppfVar73 = (float *********)ALBW_PTR_BITS(pfVar39[0x1a]);
        ppppppppppfVar73[1] = (float *********)ALBW_PTR_BITS(pfVar39[0x1b]);
      }
      uVar36 = uVar36 + 1;
    }
  }
  local_1f28[4] = (float ***********)*DAT_001ebb24;
  local_1f14 = (float ***********)DAT_001ebb24[1];
  local_1f28[3] = pppppppppppfVar52;
  local_1f28[2] = (float ***********)DAT_001ebb28;
  ppppppppppppfVar85 = (float ************)&DAT_0000000f;
  /* WALKER: step 2: second collision query; no faces at all -> step 4 (return) */
  func_004AAF44(*DAT_001eb404,(float *)local_1f00,local_1f28 + 2,0x1f,0xf,ALBW_AS_F32(ppppppppppfVar22));
  pppppppppppfVar83 = local_30;
  if (**pppppppppppfVar52 == (float *********)0x0) goto LAB_001fe470;
  func_00432F74(&local_1f08);
  pppppppppppfVar52 = (float ***********)DAT_001ebb2c;
  uVar80 = (undefined4)((ulonglong)lVar69 >> 0x20);
  local_1f04 = (float ***********)pppppppppppfVar83[0xc];
  local_1f00[0] = (float ***********)pppppppppppfVar83[0xd];
  local_1f00[1] = (float ***********)pppppppppppfVar83[0xe];
  local_1eb0 = (float ***********)ALBW_PTR_BITS(local_2c[0x11]);
  local_1e54 = (float ***********)0x0;
  ppppppppppfVar22 = pppppppppppfVar83[10];
  pppppppppppfVar53 = pppppppppppfVar15;
  local_1e30 = pppppppppppfVar15;
  if (ppppppppppfVar22 == (float **********)0x0) {
    if (((int *)ALBW_PTR_BITS(local_2c[0x27]) != (int *)0x0) && (*(int *)ALBW_PTR_BITS(local_2c[0x27]) != 0)) {
      pppppppppppfVar83[0x12] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x12]));
      pppppppppppfVar83[0x13] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x13]));
      pppppppppppfVar83[0x14] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x14]));
      pppppppppppfVar83[0x18] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x18]));
      pppppppppppfVar83[0x19] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x19]));
      pppppppppppfVar83[0x1a] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x1a]));
    }
LAB_001ebe40:
    lVar69 = CONCAT44(uVar80,pppppppppppfVar81);
    pppppppppppfVar79 = pppppppppppfVar81;
    func_00100118((int)local_1dd8,DAT_001ebfc4,0xc,3);
    local_1d40 = pppppppppppfVar83 + 0xec0;
    pppppppppfVar48 = (float *********)0x0;
    while( true ) {
      uVar80 = (undefined4)((ulonglong)in_d11 >> 0x20);
      pppppppppfVar23 = **pppppppppppfVar83;
      if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
      if (pppppppppfVar48 < pppppppppfVar23) {
        ppppppppfVar14 = (*pppppppppppfVar83)[2][(int)pppppppppfVar48];
      }
      else {
        ppppppppfVar14 = (float ********)0x0;
      }
      local_1e3c = (float ***********)(ppppppppfVar14 + 2);
      func_00115D70((float *)&local_1d4c,(float *)(ppppppppfVar14 + 0xc),ALBW_AS_F32(pppppppppppfVar52));
      local_1e84 = local_1d4c;
      local_1e80 = local_1d48;
      local_1e7c = local_1d44;
      func_00115C54((float *)local_1d70,(float *)(pppppppppppfVar83 + 0xc),(float *)&local_1e84);
      pppppppppppfVar74 = local_1e3c;
      local_1f04 = local_1d70[0];
      local_1f00[0] = local_1d70[1];
      local_1f00[1] = local_1d70[2];
      local_1db0 = local_1d70[0];
      local_1dac = local_1d70[1];
      local_1da8 = local_1d70[2];
      local_1da4 = local_1eb0;
      ppppppppppppfVar85 = &local_1db0;
      pppppppppppfVar75 = local_1e3c + 7;
      pfVar39 = (float *)ALBW_PTR_BITS(local_2c[0x26]);
      pppppppppppfVar78 = pppppppppppfVar15;
      func_00115C70((float *)&stack0xffffe0d4,(float *)ppppppppppppfVar85,(float *)pppppppppppfVar75
                   );
      pppppppppppfVar9 = pppppppppppfVar74 + 0x10;
      in_d11 = CONCAT44(uVar80,local_1f28[1]);
      local_1f14 = pppppppppppfVar74 + 0x13;
      pppppppppppfVar17 = pppppppppppfVar74 + 10;
      pppppppppppfVar49 = pppppppppppfVar74 + 0xd;
      ppppppppppfVar22 = pppppppppppfVar74[0x16];
      pppppppppppfVar50 = local_1da4;
      pppppppppppfVar34 = local_1f28[0];
      local_1e98 = pppppppppppfVar15;
      local_1e94 = pppppppppppfVar15;
      local_1e90 = pppppppppppfVar15;
      if (pfVar39 != (float *)0x0) {
        func_00115C70((float *)&local_1e98,(float *)ppppppppppppfVar85,pfVar39);
        func_00115C70((float *)&local_1e98,(float *)&local_1e98,(float *)(pppppppppppfVar74 + 7));
        if (ALBW_AS_F32(pppppppppppfVar15) <=
            ALBW_AS_F32(local_1e98) * ALBW_AS_F32(*pppppppppppfVar17) +
            ALBW_AS_F32(local_1e94) * ALBW_AS_F32(pppppppppppfVar74[0xb]) +
            ALBW_AS_F32(local_1e90) * ALBW_AS_F32(pppppppppppfVar74[0xc])) {
          ppppppppppfVar22 =
               (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar15) + ALBW_AS_F32(local_1da4) * DAT_001ec3b4));
          if (ALBW_AS_F32(ppppppppppfVar22) < ALBW_AS_F32(pppppppppppfVar74[0x16])) {
            ppppppppppfVar22 = pppppppppppfVar74[0x16];
          }
          goto LAB_001ebff4;
        }
        goto LAB_001ec3c4;
      }
LAB_001ebff4:
      fVar5 = ALBW_AS_F32(in_d11);
      fVar55 = ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(*pppppppppppfVar17) +
               ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(pppppppppppfVar74[0xb]) +
               fVar5 * ALBW_AS_F32(pppppppppppfVar74[0xc]);
      in_d11 = CONCAT44(fVar55,fVar5);
      pppppppppppfVar33 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar50) - fVar55));
      if (getenv("ALBW_WALK_TRACE")) {   /* WALKER debug: one line per wall face tested */
        extern long g_plat_frames;
        fprintf(stderr, "walk: f=%ld walker %p face %d v (%.3f %.3f %.3f) n (%.3f %.3f %.3f) reach %.3f dist %.3f limit %.3f n2 (%.3f %.3f %.3f) n3 (%.3f %.3f %.3f) pos (%.3f %.3f %.3f)\n",
                g_plat_frames, (void *)local_30, (int)(long)pppppppppfVar48,
                ALBW_AS_F32(in_stack_ffffe0d4), ALBW_AS_F32(pppppppppppfVar34), fVar5,
                ALBW_AS_F32(*pppppppppppfVar17), ALBW_AS_F32(pppppppppppfVar74[0xb]), ALBW_AS_F32(pppppppppppfVar74[0xc]),
                ALBW_AS_F32(pppppppppppfVar50), ALBW_AS_F32(pppppppppppfVar33), ALBW_AS_F32(ppppppppppfVar22),
                ALBW_AS_F32(*pppppppppppfVar49), ALBW_AS_F32(pppppppppppfVar74[0xe]), ALBW_AS_F32(pppppppppppfVar74[0xf]),
                ALBW_AS_F32(*local_1f14), ALBW_AS_F32(local_1f14[1]), ALBW_AS_F32(local_1f14[2]),
                ALBW_AS_F32(local_1f04), ALBW_AS_F32(local_1f00[0]), ALBW_AS_F32(local_1f00[1]));
      }
      if (ALBW_AS_F32(pppppppppppfVar33) == ALBW_AS_F32(pppppppppppfVar15)) {
LAB_001ec3a4:
        if (ALBW_AS_F32(pppppppppppfVar33) <= ALBW_AS_F32(pppppppppppfVar15)) goto LAB_001ec3c4;
        bVar44 = true;
        pppppppppppfVar53 = pppppppppppfVar33;
      }
      else {
        if ((ALBW_AS_F32(pppppppppppfVar33) < ALBW_AS_F32(pppppppppppfVar15)) ||
           (ALBW_AS_F32(ppppppppppfVar22) <= ALBW_AS_F32(pppppppppppfVar33))) goto LAB_001ec3c4;
        local_1e88 = (float ***********)
                     ALBW_PTR_BITS((ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(*pppppppppppfVar49) +
                      ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(pppppppppppfVar74[0xe]) +
                     fVar5 * ALBW_AS_F32(pppppppppppfVar74[0xf])));
        if (ALBW_AS_F32(pppppppppppfVar50) < ALBW_AS_F32(local_1e88)) {
          uVar36 = 1;
LAB_001ec0bc:
          if (pfVar39 != (float *)0x0) {
            func_00115C70((float *)&local_1e6c,(float *)&local_1e98,(float *)pppppppppppfVar75);
            func_00115C70((float *)&local_1d64,(float *)ppppppppppppfVar85,
                          (float *)pppppppppppfVar75);
            fVar5 = ALBW_AS_F32(local_1e6c) * ALBW_AS_F32(*pppppppppppfVar17) +
                    ALBW_AS_F32(local_1e68) * ALBW_AS_F32(pppppppppppfVar74[0xb]) +
                    ALBW_AS_F32(local_1e64) * ALBW_AS_F32(pppppppppppfVar74[0xc]);
            if (fVar5 <= ALBW_AS_F32(pppppppppppfVar15)) {
              fVar5 = -fVar5;
            }
            fVar55 = ALBW_AS_F32(local_1d64) * ALBW_AS_F32(*pppppppppppfVar17) +
                     ALBW_AS_F32(local_1d60) * ALBW_AS_F32(pppppppppppfVar74[0xb]) +
                     ALBW_AS_F32(local_1d5c) * ALBW_AS_F32(pppppppppppfVar74[0xc]);
            if (fVar55 <= ALBW_AS_F32(pppppppppppfVar15)) {
              fVar55 = -fVar55;
            }
            if (ALBW_AS_F32(pppppppppppfVar15) < fVar5 + fVar55) {
              local_1eac = (float ************)ALBW_PTR_BITS(*pfVar39);
              local_1ea8 = (float ***********)ALBW_PTR_BITS(pfVar39[1]);
              local_1ea4 = (float ***********)ALBW_PTR_BITS(pfVar39[2]);
              func_00115D70((float *)&local_1eac,(float *)&local_1eac,-(fVar55 / (fVar5 + fVar55)));
              func_00115C54((float *)&local_1eac,(float *)&local_1eac,(float *)ppppppppppppfVar85);
              func_00115C70((float *)&local_1d58,(float *)&local_1eac,(float *)pppppppppppfVar75);
              if (((((uVar36 & 1) != 0) &&
                   (local_1e88 = (float ***********)
                                 ALBW_PTR_BITS((ALBW_AS_F32(local_1d58) * ALBW_AS_F32(*pppppppppppfVar49) +
                                  ALBW_AS_F32(local_1d54) * ALBW_AS_F32(pppppppppppfVar74[0xe]) +
                                 ALBW_AS_F32(local_1d50) * ALBW_AS_F32(pppppppppppfVar74[0xf]))),
                   ALBW_AS_F32(pppppppppppfVar50) < ALBW_AS_F32(local_1e88))) ||
                  (((uVar36 & 3) != 0 &&
                   (local_1e8c = (float ***********)
                                 ALBW_PTR_BITS((ALBW_AS_F32(local_1d58) * ALBW_AS_F32(*pppppppppppfVar9) +
                                  ALBW_AS_F32(local_1d54) * ALBW_AS_F32(pppppppppppfVar9[1]) +
                                 ALBW_AS_F32(local_1d50) * ALBW_AS_F32(pppppppppppfVar9[2]))),
                   ALBW_AS_F32(pppppppppppfVar50) < ALBW_AS_F32(local_1e8c))))) ||
                 ((uVar36 != 0 &&
                  (local_1e60 = (float ***********)
                                ALBW_PTR_BITS(((ALBW_AS_F32(local_1d58) * ALBW_AS_F32(*local_1f14) +
                                  ALBW_AS_F32(local_1d54) * ALBW_AS_F32(local_1f14[1]) +
                                 ALBW_AS_F32(local_1d50) * ALBW_AS_F32(local_1f14[2])) -
                                ALBW_AS_F32(pppppppppppfVar74[0x18]))),
                  ALBW_AS_F32(pppppppppppfVar50) < ALBW_AS_F32(local_1e60))))) goto LAB_001ec3c4;
            }
            goto LAB_001ec224;
          }
          goto LAB_001ec3c4;
        }
        local_1e8c = (float ***********)
                     ALBW_PTR_BITS((ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(*pppppppppppfVar9) +
                      ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(pppppppppppfVar9[1]) +
                     fVar5 * ALBW_AS_F32(pppppppppppfVar9[2])));
        if (ALBW_AS_F32(pppppppppppfVar50) < ALBW_AS_F32(local_1e8c)) {
          uVar36 = 2;
          goto LAB_001ec0bc;
        }
        local_1e60 = (float ***********)
                     ALBW_PTR_BITS(((ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(*local_1f14) +
                       ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(local_1f14[1]) +
                      fVar5 * ALBW_AS_F32(local_1f14[2])) - ALBW_AS_F32(pppppppppppfVar74[0x18])));
        if (ALBW_AS_F32(pppppppppppfVar50) < ALBW_AS_F32(local_1e60)) {
          uVar36 = 4;
          goto LAB_001ec0bc;
        }
LAB_001ec224:
        pppppppppppfVar74 = pppppppppppfVar49;
        pppppppppppfVar34 = local_1e88;
        if (ALBW_AS_F32(local_1e88) <= ALBW_AS_F32(local_1e8c)) {
          pppppppppppfVar17 = pppppppppppfVar9;
          pppppppppppfVar75 = local_1e8c;
          if ((ALBW_AS_F32(local_1e88) <= ALBW_AS_F32(local_1e60)) &&
             (pppppppppppfVar17 = local_1f14, pppppppppppfVar49 = pppppppppppfVar9,
             pppppppppppfVar74 = pppppppppppfVar9, pppppppppppfVar75 = local_1e60,
             pppppppppppfVar29 = local_1e8c, pppppppppppfVar34 = local_1e8c,
             ALBW_AS_F32(local_1e60) < ALBW_AS_F32(local_1e8c))) goto LAB_001ec270;
        }
        else {
          pppppppppppfVar17 = local_1f14;
          pppppppppppfVar75 = local_1e60;
          if ((ALBW_AS_F32(local_1e60) < ALBW_AS_F32(local_1e88)) &&
             (pppppppppppfVar17 = pppppppppppfVar49, pppppppppppfVar74 = pppppppppppfVar9,
             pppppppppppfVar75 = local_1e88, pppppppppppfVar29 = local_1e88,
             pppppppppppfVar34 = local_1e8c, ALBW_AS_F32(local_1e8c) <= ALBW_AS_F32(local_1e60))) {
LAB_001ec270:
            pppppppppppfVar17 = pppppppppppfVar49;
            pppppppppppfVar74 = local_1f14;
            pppppppppppfVar75 = pppppppppppfVar29;
            pppppppppppfVar34 = local_1e60;
          }
        }
        if (ALBW_AS_F32(pppppppppppfVar75) <= ALBW_AS_F32(pppppppppppfVar15)) goto LAB_001ec3a4;
        fVar5 = ALBW_AS_F32(*pppppppppppfVar17) * ALBW_AS_F32(*pppppppppppfVar74) +
                ALBW_AS_F32(pppppppppppfVar17[1]) * ALBW_AS_F32(pppppppppppfVar74[1]) +
                ALBW_AS_F32(pppppppppppfVar17[2]) * ALBW_AS_F32(pppppppppppfVar74[2]);
        if (ALBW_AS_F32(pppppppppppfVar34) < fVar5 * ALBW_AS_F32(pppppppppppfVar75)) {
          fVar5 = ALBW_AS_F32(pppppppppppfVar50) * ALBW_AS_F32(pppppppppppfVar50) -
                  ALBW_AS_F32(pppppppppppfVar75) * ALBW_AS_F32(pppppppppppfVar75);
LAB_001ec38c:
          fVar55 = ALBW_AS_F32(((ulonglong)in_d11 >> 0x20));
          if (fVar55 * fVar55 <= fVar5) {
            pppppppppppfVar33 = (float ***********)ALBW_PTR_BITS((float)(SQRT(fVar5) - fVar55));
            goto LAB_001ec3a4;
          }
        }
        else {
          fVar55 = fVar5 * fVar5 - DAT_001ec3b8;
          if (0xb4000000 < (uint)ALBW_AS_U32(fVar55)) {
            fVar55 = (fVar5 * ALBW_AS_F32(pppppppppppfVar34) - ALBW_AS_F32(pppppppppppfVar75)) / fVar55;
            fVar5 = ALBW_AS_F32(pppppppppppfVar34) - fVar55 * fVar5;
            func_00115D70((float *)&local_1eac,(float *)pppppppppppfVar17,fVar55);
            local_1f28[2] = (float ***********)local_1eac;
            local_1f28[3] = local_1ea8;
            local_1f28[4] = local_1ea4;
            func_00115D70((float *)&local_1eac,(float *)pppppppppppfVar74,fVar5);
            local_1e6c = (float ***********)local_1eac;
            local_1e68 = local_1ea8;
            local_1e64 = local_1ea4;
            func_00115C54((float *)&stack0xffffe0d0,(float *)(local_1f28 + 2),(float *)&local_1e6c);
            fVar5 = ALBW_AS_F32(pppppppppppfVar50) * ALBW_AS_F32(pppppppppppfVar50) -
                    (ALBW_AS_F32(sp_x) * ALBW_AS_F32(sp_x) +
                     ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(in_stack_ffffe0d4) +
                    ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(local_1f28[0]));
            if (ALBW_AS_F32(pppppppppppfVar15) < fVar5) goto LAB_001ec38c;
          }
        }
LAB_001ec3c4:
        bVar44 = false;
      }
      if (getenv("ALBW_WALK_TRACE"))   /* WALKER debug: did the sphere test pass */
        fprintf(stderr, "walk:   face %d sphere %s pen %.3f gap-limit %.3f\n", (int)(long)pppppppppfVar48, bVar44 ? "PASS" : "fail",
                ALBW_AS_F32(pppppppppppfVar53), ALBW_AS_F32(pppppppppppfVar81));
      if (bVar44) {
        pppppppppppfVar53 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(local_1eb0) - ALBW_AS_F32(pppppppppppfVar53)));
        if (ALBW_AS_F32(pppppppppppfVar53) <= ALBW_AS_F32(pppppppppppfVar15)) {
          pppppppppppfVar53 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar53));
        }
        if (ALBW_AS_F32(pppppppppppfVar53) - DAT_001ec718 <= ALBW_AS_F32(pppppppppppfVar81)) {
          local_1d70[0] = (float ***********)local_1e3c[7];
          pppppppppppfVar74 = local_1e3c + 7;
          local_1d70[1] = (float ***********)local_1e3c[8];
          local_1d70[2] = (float ***********)local_1e3c[9];
          fVar55 = ALBW_AS_F32(local_1e3c[0xc]) * ALBW_AS_F32(local_1e3c[0xe]) -
                   ALBW_AS_F32(local_1e3c[0xb]) * ALBW_AS_F32(local_1e3c[0xf]);
          fVar57 = ALBW_AS_F32(local_1e3c[0xb]) * ALBW_AS_F32(local_1e3c[0xd]) -
                   ALBW_AS_F32(local_1e3c[10]) * ALBW_AS_F32(local_1e3c[0xe]);
          fVar58 = ALBW_AS_F32(local_1e3c[10]) * ALBW_AS_F32(local_1e3c[0xf]) -
                   ALBW_AS_F32(local_1e3c[0xc]) * ALBW_AS_F32(local_1e3c[0xd]);
          fVar5 = fVar55 * ALBW_AS_F32(local_1e3c[0x13]) + fVar58 * ALBW_AS_F32(local_1e3c[0x14]) +
                  fVar57 * ALBW_AS_F32(local_1e3c[0x15]);
          if (fVar5 <= ALBW_AS_F32(pppppppppppfVar15)) {
            local_1d64 = *pppppppppppfVar74;
            local_1d60 = local_1e3c[8];
            local_1d5c = local_1e3c[9];
          }
          else {
            fVar5 = ALBW_AS_F32(local_1e3c[0x18]) / fVar5;
            local_1d64 = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(*pppppppppppfVar74) + fVar55 * fVar5));
            local_1d60 = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(local_1e3c[8]) + fVar58 * fVar5));
            local_1d5c = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(local_1e3c[9]) + fVar57 * fVar5));
          }
          fVar55 = ALBW_AS_F32(local_1e3c[0x12]) * ALBW_AS_F32(local_1e3c[0xb]) -
                   ALBW_AS_F32(local_1e3c[0x11]) * ALBW_AS_F32(local_1e3c[0xc]);
          fVar57 = ALBW_AS_F32(local_1e3c[0x11]) * ALBW_AS_F32(local_1e3c[10]) -
                   ALBW_AS_F32(local_1e3c[0x10]) * ALBW_AS_F32(local_1e3c[0xb]);
          fVar58 = ALBW_AS_F32(local_1e3c[0x10]) * ALBW_AS_F32(local_1e3c[0xc]) -
                   ALBW_AS_F32(local_1e3c[0x12]) * ALBW_AS_F32(local_1e3c[10]);
          fVar5 = fVar55 * ALBW_AS_F32(local_1e3c[0x13]) + fVar58 * ALBW_AS_F32(local_1e3c[0x14]) +
                  fVar57 * ALBW_AS_F32(local_1e3c[0x15]);
          if (fVar5 <= ALBW_AS_F32(pppppppppppfVar15)) {
            local_1d58 = *pppppppppppfVar74;
            local_1d54 = local_1e3c[8];
            local_1d50 = local_1e3c[9];
          }
          else {
            fVar5 = ALBW_AS_F32(local_1e3c[0x18]) / fVar5;
            local_1d58 = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(*pppppppppppfVar74) + fVar55 * fVar5));
            local_1d54 = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(local_1e3c[8]) + fVar58 * fVar5));
            local_1d50 = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(local_1e3c[9]) + fVar57 * fVar5));
          }
          func_00115C70((float *)&stack0xffffe0d0,(float *)local_1d70,(float *)&local_1f04);
          iVar35 = 0;
          fVar5 = *DAT_001ebfb4 * ALBW_AS_F32(sp_x) +
                  DAT_001ebfb4[1] * ALBW_AS_F32(in_stack_ffffe0d4) + DAT_001ebfb4[2] * ALBW_AS_F32(local_1f28[0])
          ;
          uVar67 = CONCAT44(pppppppppppfVar15,fVar5);
          iVar21 = 0;
          do {
            ppppppppppppfVar13 = local_1d70;
            ppppppppppppfVar40 = ppppppppppppfVar13 + iVar35 * 3;
            if (iVar35 == 2) {
              fVar55 = ALBW_AS_F32(uVar67);
            }
            else {
              ppppppppppppfVar13 = ppppppppppppfVar13 + (iVar35 + 1) * 3;
              func_00115C70((float *)&local_1e6c,(float *)ppppppppppppfVar13,(float *)&local_1f04);
              fVar55 = *DAT_001ebfb4 * ALBW_AS_F32(local_1e6c) + DAT_001ebfb4[1] * ALBW_AS_F32(local_1e68) +
                       DAT_001ebfb4[2] * ALBW_AS_F32(local_1e64);
            }
            fVar58 = ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
            if (fVar55 == fVar58) {
              local_1dd8[iVar21 * 3] = *ppppppppppppfVar13;
              local_1dd8[iVar21 * 3 + 1] = ppppppppppppfVar13[1];
              local_1dd8[iVar21 * 3 + 2] = ppppppppppppfVar13[2];
              iVar37 = iVar21 + 1;
            }
            else {
              /* WALKER: slice the wall triangle at the walker's height. The edge from this corner
               * (height fVar5) to the next one (height fVar55) crosses the slice when the heights
               * have opposite signs: add the crossing point. EU 1EC620..1EC6EC. The Ghidra text had
               * this test garbled (NAN branches, one direction only): walls were never found. */
              iVar37 = iVar21;
              if ((fVar58 < fVar5 && fVar55 < fVar58) || (fVar5 < fVar58 && fVar58 < fVar55)) {
                local_1f14 = (float ***********)(local_1dd8 + iVar21 * 3);
                fVar5 = fVar5 / (fVar5 - fVar55);   /* 0..1 along the edge */
                func_00115C70((float *)&local_1e98,(float *)ppppppppppppfVar13,
                              (float *)ppppppppppppfVar40);   /* edge = next - this corner */
                local_1eac = (float ************)local_1e98;
                local_1ea8 = local_1e94;
                local_1ea4 = local_1e90;
                func_00115DAC((float *)local_1f14,(float *)&local_1eac,(float *)ppppppppppppfVar40,
                              fVar5);   /* point = this corner + edge * t */
                iVar37 = iVar21 + 1;
              }
            }
            fVar5 = fVar55;
          } while ((iVar37 < 2) && (iVar35 = iVar35 + 1, iVar21 = iVar37, iVar35 < 3));
          if (getenv("ALBW_WALK_TRACE"))   /* WALKER debug: crossing points of the face at the walker's height */
            fprintf(stderr, "walk:   face %d slice points %d tri (%.3f %.3f %.3f) (%.3f %.3f %.3f) (%.3f %.3f %.3f) pos (%.3f %.3f %.3f) up (%.3f %.3f %.3f)\n",
                    (int)(long)pppppppppfVar48, iVar37,
                    ALBW_AS_F32(local_1d70[0]), ALBW_AS_F32(local_1d70[1]), ALBW_AS_F32(local_1d70[2]),
                    ALBW_AS_F32(local_1d70[3]), ALBW_AS_F32(local_1d70[4]), ALBW_AS_F32(local_1d70[5]),
                    ALBW_AS_F32(local_1d70[6]), ALBW_AS_F32(local_1d70[7]), ALBW_AS_F32(local_1d70[8]),
                    ALBW_AS_F32(local_1f04), ALBW_AS_F32(local_1f00[0]), ALBW_AS_F32(local_1f00[1]),
                    DAT_001ebfb4[0], DAT_001ebfb4[1], DAT_001ebfb4[2]);
          if (iVar37 == 1) {
            fVar5 = (ALBW_AS_F32(pppppppppppfVar83[0xc]) - ALBW_AS_F32(local_1dd8[0])) *
                    (ALBW_AS_F32(pppppppppppfVar83[0xc]) - ALBW_AS_F32(local_1dd8[0])) +
                    (ALBW_AS_F32(pppppppppppfVar83[0xd]) - ALBW_AS_F32(local_1dd8[1])) *
                    (ALBW_AS_F32(pppppppppppfVar83[0xd]) - ALBW_AS_F32(local_1dd8[1])) +
                    (ALBW_AS_F32(pppppppppppfVar83[0xe]) - ALBW_AS_F32(local_1dd8[2])) *
                    (ALBW_AS_F32(pppppppppppfVar83[0xe]) - ALBW_AS_F32(local_1dd8[2]));
            pppppppppppfVar15 = local_1dd8[2];
            pppppppppppfVar74 = local_1dd8[1];
            pppppppppppfVar49 = local_1dd8[0];
          }
          else {
            if (iVar37 != 2) goto LAB_001ec928;
            func_00115C70((float *)&stack0xffffe0d0,(float *)&local_1dcc,(float *)local_1dd8);
            fVar5 = ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
            local_1f14 = local_1f28[0];
            local_1f28[3] = sp_x;
            local_1f28[4] = in_stack_ffffe0d4;
            fVar55 = func_0068906C((float *)&local_1e4c,(float *)(local_1f28 + 3));
            func_0042D5B0((float *)(pppppppppppfVar83 + 0xc),(float *)local_1dd8,
                          (float *)&local_1e4c,(float *)&local_1e00);
            if (fVar5 <= ALBW_AS_F32(local_1e00)) {
              if (ALBW_AS_F32(local_1e00) <= fVar55) {
                func_00115D70((float *)&stack0xffffe0d0,(float *)&local_1e4c,ALBW_AS_F32(local_1e00));
                /* EU 1EC824..1EC82C: ldm sp, {r0, r1, r3}; stm sp+0x84 - the whole result is copied;
                 * the text took x / y from stale registers (BOOT_LOG 135) */
                local_1ea4 = local_1f28[0];
                local_1eac = (float ************)(uintptr_t)stack0xffffe0d0;
                local_1ea8 = (float ***********)(uintptr_t)stack0xffffe0d4;
                func_00115C54((float *)&stack0xffffe0d0,(float *)local_1dd8,(float *)&local_1eac);
                /* EU 1EC83C..1EC844: vldr s2, [sp]; s1, [sp, #4]; s0, [sp, #8] */
                pppppppppppfVar15 = local_1f28[0];
                pppppppppppfVar74 = (float ***********)(uintptr_t)stack0xffffe0d4;
                pppppppppppfVar49 = (float ***********)(uintptr_t)stack0xffffe0d0;
              }
              else {
                fVar5 = (ALBW_AS_F32(pppppppppppfVar83[0xc]) - ALBW_AS_F32(local_1dcc)) *
                        (ALBW_AS_F32(pppppppppppfVar83[0xc]) - ALBW_AS_F32(local_1dcc)) +
                        (ALBW_AS_F32(pppppppppppfVar83[0xd]) - ALBW_AS_F32(local_1dc8)) *
                        (ALBW_AS_F32(pppppppppppfVar83[0xd]) - ALBW_AS_F32(local_1dc8)) +
                        (ALBW_AS_F32(pppppppppppfVar83[0xe]) - ALBW_AS_F32(local_1dc4)) *
                        (ALBW_AS_F32(pppppppppppfVar83[0xe]) - ALBW_AS_F32(local_1dc4));
                pppppppppppfVar15 = local_1dc4;
                pppppppppppfVar74 = local_1dc8;
                pppppppppppfVar49 = (float ***********)local_1dcc;
              }
            }
            else {
              fVar5 = (ALBW_AS_F32(pppppppppppfVar83[0xc]) - ALBW_AS_F32(local_1dd8[0])) *
                      (ALBW_AS_F32(pppppppppppfVar83[0xc]) - ALBW_AS_F32(local_1dd8[0])) +
                      (ALBW_AS_F32(pppppppppppfVar83[0xd]) - ALBW_AS_F32(local_1dd8[1])) *
                      (ALBW_AS_F32(pppppppppppfVar83[0xd]) - ALBW_AS_F32(local_1dd8[1])) +
                      (ALBW_AS_F32(pppppppppppfVar83[0xe]) - ALBW_AS_F32(local_1dd8[2])) *
                      (ALBW_AS_F32(pppppppppppfVar83[0xe]) - ALBW_AS_F32(local_1dd8[2]));
              pppppppppppfVar15 = local_1dd8[2];
              pppppppppppfVar74 = local_1dd8[1];
              pppppppppppfVar49 = local_1dd8[0];
            }
          }
          if (fVar5 <= ALBW_AS_F32(lVar69)) {
            pppppppppppfVar34 = (float ***********)local_1e3c[10];
            pppppppppppfVar17 = (float ***********)local_1e3c[0xb];
            fVar55 = ALBW_AS_F32(lVar69) - fVar5;
            pppppppppppfVar50 =
                 (float ***********)
                 ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar83[0x18]) * ALBW_AS_F32(pppppppppppfVar34) +
                  ALBW_AS_F32(pppppppppppfVar83[0x19]) * ALBW_AS_F32(pppppppppppfVar17) +
                 ALBW_AS_F32(pppppppppppfVar83[0x1a]) * ALBW_AS_F32(local_1e3c[0xc])));
            if (((fVar55 < DAT_001ecc40) || (DAT_001ecc3c < fVar55)) ||
               (ALBW_AS_F32(pppppppppppfVar50) <= ALBW_AS_F32(local_1e30))) {
              lVar69 = CONCAT44((int)((ulonglong)lVar69 >> 0x20),fVar5);
              in_d15 = CONCAT44(pppppppppppfVar74,pppppppppppfVar15);
              if (pppppppppfVar48 < **pppppppppppfVar83) {
                local_1e54 = (float ***********)(*pppppppppppfVar83)[2][(int)pppppppppfVar48];
              }
              else {
                local_1e54 = (float ***********)0x0;
              }
              local_1e70 = (float ***********)local_1e3c[0xc];
              if (pppppppppfVar48 < **pppppppppppfVar83) {
                ppppppppfVar14 = (*pppppppppppfVar83)[2][(int)pppppppppfVar48];
              }
              else {
                ppppppppfVar14 = (float ********)0x0;
              }
              local_1d40[0xf] = (float **********)*ppppppppfVar14;
              pppppppppppfVar81 = pppppppppppfVar53;
              local_1e78 = pppppppppppfVar34;
              local_1e74 = pppppppppppfVar17;
              local_1e34 = pppppppppppfVar49;
              local_1e30 = pppppppppppfVar50;
            }
          }
        }
      }
LAB_001ec928:
      pppppppppppfVar15 = pppppppppppfVar78;
      pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
      in_stack_ffffe0d4 = pppppppppppfVar9;
    }
    /* WALKER: step 4: no face passed the tests (close enough, facing, inside edges) -> stay free, return 0 */
    if (local_1e54 == (float ***********)0x0) goto LAB_001fe470;
    pppppppppppfVar83[10] = (float **********)local_1e54;
  }
  else {
    pppppppppppfVar79 = pppppppppppfVar15;
    pppppppfVar51 = DAT_001ebb2c;
    func_00115D70((float *)local_1f28,(float *)(ppppppppppfVar22 + 0xc),ALBW_AS_F32(DAT_001ebb2c));
    local_1e6c = local_1f28[0];
    local_1e68 = local_1f28[1];
    local_1e64 = local_1f28[2];
    func_00115C54((float *)&stack0xffffe0d0,(float *)(pppppppppppfVar83 + 0xc),(float *)&local_1e6c)
    ;
    local_1f00[1] = local_1f28[0];
    local_1e80 = local_1f28[0];
    local_1e7c = local_1eb0;
    local_1f04 = sp_x;
    local_1f00[0] = in_stack_ffffe0d4;
    local_1e88 = sp_x;
    local_1e84 = in_stack_ffffe0d4;
    func_00115C70((float *)&stack0xffffe0d4,(float *)&local_1e88,(float *)(ppppppppppfVar22 + 9));
    ppppppppppfVar56 = ppppppppppfVar22 + 0xf;
    ppppppppppfVar30 = ppppppppppfVar22 + 0x12;
    ppppppppppfVar73 = ppppppppppfVar22 + 0x15;
    fVar55 = ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(ppppppppppfVar22[0xc]) +
             ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
             ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(ppppppppppfVar22[0xe]);
    fVar5 = ALBW_AS_F32(local_1e7c) - fVar55;
    if (fVar5 == ALBW_AS_F32(pppppppppppfVar15)) {
LAB_001eba64:
      if (fVar5 <= ALBW_AS_F32(pppppppppppfVar15)) goto LAB_001eba78;
      in_d11 = CONCAT44((int)((ulonglong)in_d11 >> 0x20),fVar5);
      bVar44 = true;
    }
    else {
      if ((((fVar5 < ALBW_AS_F32(pppppppppppfVar15)) || (ALBW_AS_F32(ppppppppppfVar22[0x18]) <= fVar5)) ||
          (fVar58 = ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(*ppppppppppfVar56) +
                    ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(ppppppppppfVar22[0x10]) +
                    ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(ppppppppppfVar22[0x11]), ALBW_AS_F32(local_1e7c) < fVar58
          )) || ((fVar57 = ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(*ppppppppppfVar30) +
                           ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(ppppppppppfVar22[0x13]) +
                           ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(ppppppppppfVar22[0x14]),
                 ALBW_AS_F32(local_1e7c) < fVar57 ||
                 (fVar60 = (ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(*ppppppppppfVar73) +
                            ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(ppppppppppfVar22[0x16]) +
                           ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(ppppppppppfVar22[0x17])) -
                           ALBW_AS_F32(ppppppppppfVar22[0x1a]), ALBW_AS_F32(local_1e7c) < fVar60))))
      goto LAB_001eba78;
      if (fVar58 <= fVar57) {
        ppppppppppfVar10 = ppppppppppfVar30;
        fVar65 = fVar57;
        if ((fVar58 <= fVar60) &&
           (ppppppppppfVar10 = ppppppppppfVar73, ppppppppppfVar56 = ppppppppppfVar30,
           fVar65 = fVar60, fVar58 = fVar57, fVar60 < fVar57)) {
          ppppppppppfVar10 = ppppppppppfVar30;
          ppppppppppfVar56 = ppppppppppfVar73;
          fVar65 = fVar57;
          fVar58 = fVar60;
        }
      }
      else {
        ppppppppppfVar10 = ppppppppppfVar73;
        fVar65 = fVar60;
        if ((fVar60 < fVar58) &&
           (ppppppppppfVar10 = ppppppppppfVar56, ppppppppppfVar56 = ppppppppppfVar30,
           fVar65 = fVar58, fVar58 = fVar57, fVar57 <= fVar60)) {
          ppppppppppfVar56 = ppppppppppfVar73;
          fVar58 = fVar60;
        }
      }
      if (fVar65 <= ALBW_AS_F32(pppppppppppfVar15)) goto LAB_001eba64;
      fVar5 = ALBW_AS_F32(*ppppppppppfVar10) * ALBW_AS_F32(*ppppppppppfVar56) +
              ALBW_AS_F32(ppppppppppfVar10[1]) * ALBW_AS_F32(ppppppppppfVar56[1]) +
              ALBW_AS_F32(ppppppppppfVar10[2]) * ALBW_AS_F32(ppppppppppfVar56[2]);
      if (fVar58 < fVar5 * fVar65) {
        fVar5 = ALBW_AS_F32(local_1e7c) * ALBW_AS_F32(local_1e7c) - fVar65 * fVar65;
LAB_001eba4c:
        if (fVar55 * fVar55 <= fVar5) {
          fVar5 = SQRT(fVar5) - fVar55;
          goto LAB_001eba64;
        }
      }
      else {
        fVar57 = fVar5 * fVar5 - DAT_001ebb30;
        if (0xb4000000 < (uint)ALBW_AS_U32(fVar57)) {
          fVar57 = (fVar5 * fVar58 - fVar65) / fVar57;
          fVar58 = fVar58 - fVar57 * fVar5;
          pppppppppppfVar74 = local_1e7c;
          func_00115D70((float *)&local_1dcc,(float *)ppppppppppfVar10,fVar57);
          local_1e08 = (float ***********)local_1dcc;
          local_1e04 = local_1dc8;
          local_1e00 = local_1dc4;
          func_00115D70((float *)local_1f28,(float *)ppppppppppfVar56,fVar58);
          local_1dd8[0] = local_1f28[0];
          local_1dd8[1] = local_1f28[1];
          local_1dd8[2] = local_1f28[2];
          func_00115C54((float *)&stack0xffffe0d0,(float *)&local_1e08,(float *)local_1dd8);
          fVar5 = ALBW_AS_F32(pppppppppppfVar74) * ALBW_AS_F32(pppppppppppfVar74) -
                  (ALBW_AS_F32(sp_x) * ALBW_AS_F32(sp_x) +
                   ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(in_stack_ffffe0d4) +
                  ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(local_1f28[0]));
          if (ALBW_AS_F32(pppppppppppfVar15) < fVar5) goto LAB_001eba4c;
        }
      }
LAB_001eba78:
      bVar44 = false;
    }
    uVar80 = (undefined4)((ulonglong)lVar69 >> 0x20);
    if (!bVar44) {
LAB_001ebd94:
      pppppppppppfVar83[10] = (float **********)0x0;
      pppppppppppfVar83[0x12] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x12]));
      pppppppppppfVar83[0x13] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x13]));
      pppppppppppfVar83[0x14] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x14]));
      pppppppppppfVar83[0x18] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x18]));
      pppppppppppfVar83[0x19] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x19]));
      pppppppppppfVar83[0x1a] = (float **********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar83[0x1a]));
      goto LAB_001ebe40;
    }
    pppppppppppfVar74 =
         (float ***********)ALBW_PTR_BITS(((ALBW_AS_F32(local_1eb0) - ALBW_AS_F32(in_d11)) - ALBW_AS_F32(pppppppfVar51)));
    pppppppppfVar48 = ppppppppppfVar22[0xd];
    if (ALBW_AS_F32(pppppppppfVar48) <= ALBW_AS_F32(pppppppppppfVar15)) {
      pppppppppfVar48 = (float *********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppfVar48));
    }
    if ((int)pppppppppfVar48 <= DAT_001ebb34) {
      func_00115C70((float *)&local_1e9c,(float *)&local_1f04,(float *)(ppppppppppfVar22 + 9));
      func_00115D70((float *)&local_1e9c,(float *)(ppppppppppfVar22 + 0xc),
                    ALBW_AS_F32(ppppppppppfVar22[0xc]) * ALBW_AS_F32(local_1e9c) +
                    ALBW_AS_F32(ppppppppppfVar22[0xd]) * ALBW_AS_F32(local_1e98) +
                    ALBW_AS_F32(ppppppppppfVar22[0xe]) * ALBW_AS_F32(local_1e94));
      local_1f28[3] = local_1e9c;
      local_1f28[4] = local_1e98;
      local_1f14 = local_1e94;
      func_00115C70((float *)&stack0xffffe0d0,(float *)&local_1f04,(float *)(local_1f28 + 3));
      local_1ea4 = local_1f28[0];
      local_1eac = (float ************)sp_x;
      local_1ea8 = in_stack_ffffe0d4;
      func_00115C70((float *)local_1f28,(float *)&local_1eac,(float *)(ppppppppppfVar22 + 9));
      uVar80 = (undefined4)((ulonglong)lVar69 >> 0x20);
      if ((((ALBW_AS_F32(pppppppfVar51) <
             ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(ppppppppppfVar22[0xf]) +
             ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(ppppppppppfVar22[0x10]) +
             ALBW_AS_F32(local_1f28[2]) * ALBW_AS_F32(ppppppppppfVar22[0x11])) ||
           (ALBW_AS_F32(pppppppfVar51) <
            ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(ppppppppppfVar22[0x12]) +
            ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(ppppppppppfVar22[0x13]) +
            ALBW_AS_F32(local_1f28[2]) * ALBW_AS_F32(ppppppppppfVar22[0x14]))) ||
          (ALBW_AS_F32(ppppppppppfVar22[0x1a]) + ALBW_AS_F32(pppppppfVar51) <
           ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(ppppppppppfVar22[0x15]) +
           ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(ppppppppppfVar22[0x16]) +
           ALBW_AS_F32(local_1f28[2]) * ALBW_AS_F32(ppppppppppfVar22[0x17]))) ||
         (((fVar5 = -(ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(ppppppppppfVar22[0xc]) +
                      ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
                     ALBW_AS_F32(local_1f28[2]) * ALBW_AS_F32(ppppppppppfVar22[0xe])), fVar5 < DAT_001ebfa8 ||
           (ALBW_AS_F32(pppppppfVar51) < fVar5)) || (fVar5 == -3.4028235e+38)))) goto LAB_001ebd94;
    }
    uVar80 = (undefined4)((ulonglong)lVar69 >> 0x20);
    pppppppppppfVar49 = pppppppppppfVar74;
    if (ALBW_AS_F32(pppppppppppfVar74) <= ALBW_AS_F32(pppppppppppfVar15)) {
      pppppppppppfVar49 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar74));
    }
    if (ALBW_AS_F32(pppppppppppfVar81) < ALBW_AS_F32(pppppppppppfVar49)) goto LAB_001ebd94;
    if (((DAT_001ebfb0 <= ALBW_AS_F32(pppppppppppfVar81) - ALBW_AS_F32(pppppppppppfVar49)) &&
        (ALBW_AS_F32(pppppppppppfVar81) - ALBW_AS_F32(pppppppppppfVar49) <= DAT_001ebfac)) &&
       (ALBW_AS_F32(pppppppppppfVar83[0x18]) * ALBW_AS_F32(ppppppppppfVar22[0xc]) +
        ALBW_AS_F32(pppppppppppfVar83[0x19]) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
        ALBW_AS_F32(pppppppppppfVar83[0x1a]) * ALBW_AS_F32(ppppppppppfVar22[0xe]) < ALBW_AS_F32(pppppppppppfVar79)))
    goto LAB_001ebd94;
    local_1e78 = (float ***********)ppppppppppfVar22[0xc];
    local_1e74 = (float ***********)ppppppppppfVar22[0xd];
    local_1e70 = (float ***********)ppppppppppfVar22[0xe];
    func_00115D70((float *)(local_1f28 + 2),(float *)&local_1e78,ALBW_AS_F32(pppppppppppfVar74));
    local_1f28[0] = local_1f28[3];
    local_1f28[1] = local_1f28[4];
    pppppppppppfVar79 = pppppppppppfVar81;
    in_stack_ffffe0d4 = local_1f28[2];
    func_00115C70((float *)(local_1f28 + 2),(float *)(pppppppppppfVar83 + 0xc),
                  (float *)&stack0xffffe0d4);
    uVar80 = (undefined4)((ulonglong)lVar69 >> 0x20);
    local_1e34 = local_1f28[2];
    in_d15 = CONCAT44(local_1f28[3],local_1f28[4]);
    local_1e50[0xf] = (float **********)*pppppppppppfVar83[10];
    pppppppppppfVar53 = pppppppppppfVar74;
    pppppppppppfVar81 = pppppppppppfVar79;
    if (pppppppppppfVar83[10] == (float **********)0x0) goto LAB_001ebe40;
  }
  pppppppppppfVar81 = (float ***********)((ulonglong)lVar69 >> 0x20);
  pppppppppppfVar83[0xc] = (float **********)local_1e34;
  pppppppppppfVar83[0xd] = (float **********)((ulonglong)in_d15 >> 0x20);
  pppppppppppfVar83[0xe] = (float **********)in_d15;
  pppppppppppfVar83[0x18] = (float **********)local_1e78;
  pppppppppppfVar83[0x19] = (float **********)local_1e74;
  pppppppppppfVar83[0x1a] = (float **********)local_1e70;
  pfVar39 = DAT_001ebfb4;
  pppppppppppfVar83[0x15] = (float **********)ALBW_PTR_BITS(*DAT_001ebfb4);
  pppppppppppfVar83[0x16] = (float **********)ALBW_PTR_BITS(pfVar39[1]);
  pppppppppppfVar83[0x17] = (float **********)ALBW_PTR_BITS(pfVar39[2]);
  func_004A17E4((float *)(pppppppppppfVar83 + 0x12));
  fVar5 = local_2c[0xd];
  uVar67 = CONCAT44(DAT_001ebfb8,fVar5);
  bVar44 = DAT_001ebfb8 == fVar5;
  bVar47 = fVar5 <= DAT_001ebfb8;
  if (!bVar47 || bVar44) {
    bVar44 = fVar5 == ALBW_AS_F32(DAT_001ebfbc);
    bVar47 = ALBW_AS_F32(DAT_001ebfbc) <= fVar5;
  }
  fVar5 = DAT_001ebfc0;
  pppppppppppfVar53 = (float ***********)DAT_001ebfbc;
  if (bVar47 && !bVar44) {
    func_0049F5FC(pppppppppppfVar83 + 6);
    pppppppppfVar48 = (float *********)0x0;
    while( true ) {
      pppppppppfVar23 = **pppppppppppfVar83;
      if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
      if (pppppppppfVar48 < pppppppppfVar23) {
        ppppppppfVar14 = (*pppppppppppfVar83)[2][(int)pppppppppfVar48];
      }
      else {
        ppppppppfVar14 = (float ********)0x0;
      }
      func_0010F1DC((int *)(pppppppppppfVar83 + 6),
                    (int *)((int)pppppppppppfVar83[9] + (int)ppppppppfVar14));
      pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
      pppppppppppfVar83[8] = (float **********)((int)pppppppppppfVar83[8] + 1);
    }
    func_001FE680(&local_1ea8);
    local_1dd8[0] = (float ***********)pppppppppppfVar83[10];
    local_1dd8[1] = (float ***********)pppppppppppfVar83[0xc];
    local_1dd8[2] = (float ***********)pppppppppppfVar83[0xd];
    fVar55 = fVar5;
    if (ALBW_AS_F32(pppppppppppfVar15) <= ALBW_AS_F32(uVar67)) {
      fVar55 = DAT_001ecc44;
    }
    local_1dcc = pppppppppppfVar83[0xe];
    func_00115D70((float *)(local_1f28 + 2),(float *)(pppppppppppfVar83 + 0x15),fVar55);
    local_1dc8 = local_1f28[2];
    local_1dc4 = local_1f28[3];
    local_1dc0 = local_1f28[4];
    ppppppppppfVar22 = pppppppppppfVar83[10];
    if (ALBW_AS_F32(uVar67) <= ALBW_AS_F32(pppppppppppfVar15)) {
      uVar67 = CONCAT44((int)((ulonglong)uVar67 >> 0x20),-ALBW_AS_F32(uVar67));
    }
    local_1dbc = (float ************)ppppppppppfVar22[0xc];
    local_1db8 = (float ************)ppppppppppfVar22[0xd];
    local_1db4 = (float ***********)ppppppppppfVar22[0xe];
    local_1db0 = pppppppppppfVar83 + 0x12;
    local_1da8 = pppppppppppfVar83 + 6;
    local_1dac = (float ***********)uVar67;
    local_1da4 = (float ***********)0xffffffff;
    func_00650A24((int)pppppppppppfVar83,(float *)&local_1ea8,(float *)local_1dd8);
    if (local_1e88 != (float ***********)0x0) {
      *(float ***********)(DAT_001ecc48 + (int)pppppppppppfVar83) = *local_1e88;
      pppppppppppfVar83[0xc] = (float **********)local_1ea4;
      pppppppppppfVar83[0xd] = (float **********)local_1ea0;
      pppppppppppfVar83[0xe] = (float **********)local_1e9c;
      if ((float ***********)pppppppppppfVar83[10] != local_1e88) {
        pppppppppppfVar83[10] = (float **********)local_1e88;
        pppppppppppfVar83[0x18] = local_1e88[0xc];
        pppppppppppfVar83[0x19] = local_1e88[0xd];
        pppppppppppfVar83[0x1a] = local_1e88[0xe];
        pfVar39 = DAT_001ebfb4;
        pppppppppppfVar83[0x15] = (float **********)ALBW_PTR_BITS(*DAT_001ebfb4);
        pppppppppppfVar83[0x16] = (float **********)ALBW_PTR_BITS(pfVar39[1]);
        pppppppppppfVar83[0x17] = (float **********)ALBW_PTR_BITS(pfVar39[2]);
        func_004A17E4((float *)(pppppppppppfVar83 + 0x12));
      }
    }
  }
  func_001FE680(&local_1da0);
  local_1d84 = pppppppppppfVar83 + 0xf;
  pppppppppppfVar74 = (float ***********)pppppppppppfVar83[10];
  func_0049F5FC(pppppppppppfVar83 + 6);
  pppppppppfVar48 = (float *********)0x0;
  while( true ) {
    fVar55 = ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
    pppppppppfVar23 = **pppppppppppfVar83;
    if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
    if (pppppppppfVar48 < pppppppppfVar23) {
      ppppppppfVar14 = (*pppppppppppfVar83)[2][(int)pppppppppfVar48];
    }
    else {
      ppppppppfVar14 = (float ********)0x0;
    }
    func_0010F1DC((int *)(pppppppppppfVar83 + 6),
                  (int *)((int)pppppppppppfVar83[9] + (int)ppppppppfVar14));
    pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
    pppppppppppfVar83[8] = (float **********)((int)pppppppppppfVar83[8] + 1);
  }
  local_1dd8[1] = (float ***********)pppppppppppfVar83[0xc];
  local_1da8 = pppppppppppfVar83 + 6;
  local_1dd8[2] = (float ***********)pppppppppppfVar83[0xd];
  local_1dcc = pppppppppppfVar83[0xe];
  local_1dc8 = (float ***********)pppppppppppfVar83[0x15];
  local_1dc4 = (float ***********)pppppppppppfVar83[0x16];
  local_1dc0 = (float ***********)pppppppppppfVar83[0x17];
  local_1dbc = (float ************)pppppppppppfVar74[0xc];
  local_1db8 = (float ************)pppppppppppfVar74[0xd];
  local_1db4 = (float ***********)pppppppppppfVar74[0xe];
  local_1db0 = pppppppppppfVar83 + 0x12;
  local_1dac = (float ***********)pppppppppppfVar83[0x1c];
  if (local_1d7c == (float ************)0x0) {
    local_1da4 = (float ***********)0xffffffff;
  }
  else {
    local_1da4 = (float ***********)((int)*local_1d7c + -1);
  }
  local_1dd8[0] = pppppppppppfVar74;
  uVar36 = func_00650A24((int)pppppppppppfVar83,(float *)&local_1da0,(float *)local_1dd8);
  pppppppppppfVar74 = local_30;
  if (1 < uVar36) goto LAB_001fe470;
  if (local_1d84 != (float ***********)0x0) {
    *local_1d84 = (float **********)local_1d9c;
    local_1d84[1] = (float **********)local_1d98;
    local_1d84[2] = (float **********)local_1d94;
    pppppppppppfVar83[0xb] = local_1d80;
  }
  uVar70 = (ulonglong)(uint)ALBW_AS_U32(fVar55);
  fVar58 = local_2c[0xf];
  bVar44 = fVar55 == fVar58;
  bVar47 = fVar58 <= fVar55;
  if (!bVar47 || bVar44) {
    bVar44 = fVar58 == ALBW_AS_F32(pppppppppppfVar53);
    bVar47 = ALBW_AS_F32(pppppppppppfVar53) <= fVar58;
  }
  *(bool *)((int)pppppppppppfVar83 + 0x3dc2) = bVar47 && !bVar44;
  pppppppppppfVar83 = pppppppppppfVar81;
  if (**local_30 != (float *********)0x0) {
    pppppppppfVar48 = (float *********)0x0;
    pppppppppppfVar49 = pppppppppppfVar15;
    while( true ) {
      pppppppppfVar23 = **pppppppppppfVar74;
      pppppppppppfVar52 = pppppppppppfVar81;
      if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
      if (pppppppppfVar48 < pppppppppfVar23) {
        ppppppppppfVar22 = (float **********)(*pppppppppppfVar74)[2][(int)pppppppppfVar48];
      }
      else {
        ppppppppppfVar22 = (float **********)0x0;
      }
      pppppppppfVar23 = ppppppppppfVar22[0xc];
      fVar57 = ALBW_AS_F32(pppppppppfVar23) * ALBW_AS_F32(ppppppppppfVar22[0x11]) -
               ALBW_AS_F32(ppppppppppfVar22[0xe]) * ALBW_AS_F32(ppppppppppfVar22[0xf]);
      pppppppppppfVar9 = (float ***********)ppppppppppfVar22[9];
      pppppppppppfVar17 = (float ***********)ppppppppppfVar22[10];
      pppppppppppfVar52 = (float ***********)ppppppppppfVar22[0xb];
      fVar60 = ALBW_AS_F32(ppppppppppfVar22[0xd]) * ALBW_AS_F32(ppppppppppfVar22[0xf]) -
               ALBW_AS_F32(pppppppppfVar23) * ALBW_AS_F32(ppppppppppfVar22[0x10]);
      fVar58 = ALBW_AS_F32(ppppppppppfVar22[0xe]) * ALBW_AS_F32(ppppppppppfVar22[0x10]) -
               ALBW_AS_F32(ppppppppppfVar22[0xd]) * ALBW_AS_F32(ppppppppppfVar22[0x11]);
      fVar55 = fVar58 * ALBW_AS_F32(ppppppppppfVar22[0x15]) + fVar57 * ALBW_AS_F32(ppppppppppfVar22[0x16]) +
               fVar60 * ALBW_AS_F32(ppppppppppfVar22[0x17]);
      pppppppppppfVar50 = pppppppppppfVar17;
      pppppppppppfVar34 = pppppppppppfVar52;
      pppppppppppfVar78 = pppppppppppfVar9;
      if (ALBW_AS_F32(pppppppppppfVar49) < fVar55) {
        fVar55 = ALBW_AS_F32(ppppppppppfVar22[0x1a]) / fVar55;
        pppppppppppfVar50 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar17) + fVar57 * fVar55));
        pppppppppppfVar34 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar52) + fVar60 * fVar55));
        pppppppppppfVar78 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar9) + fVar58 * fVar55));
      }
      fVar58 = ALBW_AS_F32(ppppppppppfVar22[0x14]) * ALBW_AS_F32(ppppppppppfVar22[0xd]) -
               ALBW_AS_F32(ppppppppppfVar22[0x13]) * ALBW_AS_F32(ppppppppppfVar22[0xe]);
      fVar57 = ALBW_AS_F32(ppppppppppfVar22[0x12]) * ALBW_AS_F32(ppppppppppfVar22[0xe]) -
               ALBW_AS_F32(ppppppppppfVar22[0x14]) * ALBW_AS_F32(pppppppppfVar23);
      fVar60 = ALBW_AS_F32(ppppppppppfVar22[0x13]) * ALBW_AS_F32(pppppppppfVar23) -
               ALBW_AS_F32(ppppppppppfVar22[0x12]) * ALBW_AS_F32(ppppppppppfVar22[0xd]);
      in_d11 = CONCAT44(fVar60,fVar57);
      fVar55 = fVar58 * *(float *)(ppppppppppfVar22 + 0x15) +
               fVar57 * *(float *)((char *)(ppppppppppfVar22 + 0x15) + 4) +
               fVar60 * ALBW_AS_F32(ppppppppppfVar22[0x17]);
      pppppppppppfVar75 = pppppppppppfVar9;
      pppppppppppfVar33 = pppppppppppfVar52;
      pppppppppppfVar29 = pppppppppppfVar17;
      if (ALBW_AS_F32(pppppppppppfVar49) < fVar55) {
        fVar55 = ALBW_AS_F32(ppppppppppfVar22[0x1a]) / fVar55;
        pppppppppppfVar75 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar9) + fVar58 * fVar55));
        pppppppppppfVar33 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar52) + fVar60 * fVar55));
        pppppppppppfVar29 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar17) + fVar57 * fVar55));
      }
      pppppppppppfVar38 = pppppppppppfVar79;
      if (ALBW_AS_F32(pppppppppppfVar9) < ALBW_AS_F32(pppppppppppfVar79)) {
        pppppppppppfVar38 = pppppppppppfVar9;
      }
      pppppppppppfVar28 = pppppppppppfVar79;
      if (ALBW_AS_F32(pppppppppppfVar17) < ALBW_AS_F32(pppppppppppfVar79)) {
        pppppppppppfVar28 = pppppppppppfVar17;
      }
      pppppppppppfVar62 = pppppppppppfVar79;
      if (ALBW_AS_F32(pppppppppppfVar52) < ALBW_AS_F32(pppppppppppfVar79)) {
        pppppppppppfVar62 = pppppppppppfVar52;
      }
      pppppppppppfVar61 = pppppppppppfVar81;
      if (ALBW_AS_F32(pppppppppppfVar81) <= ALBW_AS_F32(pppppppppppfVar9)) {
        pppppppppppfVar61 = pppppppppppfVar9;
      }
      pppppppppppfVar9 = pppppppppppfVar81;
      if (ALBW_AS_F32(pppppppppppfVar81) <= ALBW_AS_F32(pppppppppppfVar17)) {
        pppppppppppfVar9 = pppppppppppfVar17;
      }
      if (ALBW_AS_F32(pppppppppppfVar52) < ALBW_AS_F32(pppppppppppfVar81)) {
        pppppppppppfVar52 = pppppppppppfVar81;
      }
      if (ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar38)) {
        pppppppppppfVar38 = pppppppppppfVar78;
      }
      pppppppppppfVar17 = pppppppppppfVar50;
      if (ALBW_AS_F32(pppppppppppfVar28) <= ALBW_AS_F32(pppppppppppfVar50)) {
        pppppppppppfVar17 = pppppppppppfVar28;
      }
      if (ALBW_AS_F32(pppppppppppfVar34) < ALBW_AS_F32(pppppppppppfVar62)) {
        pppppppppppfVar62 = pppppppppppfVar34;
      }
      if (ALBW_AS_F32(pppppppppppfVar61) <= ALBW_AS_F32(pppppppppppfVar78)) {
        pppppppppppfVar61 = pppppppppppfVar78;
      }
      if (ALBW_AS_F32(pppppppppppfVar9) <= ALBW_AS_F32(pppppppppppfVar50)) {
        pppppppppppfVar9 = pppppppppppfVar50;
      }
      if (ALBW_AS_F32(pppppppppppfVar52) <= ALBW_AS_F32(pppppppppppfVar34)) {
        pppppppppppfVar52 = pppppppppppfVar34;
      }
      if (ALBW_AS_F32(pppppppppppfVar75) < ALBW_AS_F32(pppppppppppfVar38)) {
        pppppppppppfVar38 = pppppppppppfVar75;
      }
      if (ALBW_AS_F32(pppppppppppfVar29) < ALBW_AS_F32(pppppppppppfVar17)) {
        pppppppppppfVar17 = pppppppppppfVar29;
      }
      if (ALBW_AS_F32(pppppppppppfVar33) < ALBW_AS_F32(pppppppppppfVar62)) {
        pppppppppppfVar62 = pppppppppppfVar33;
      }
      if (ALBW_AS_F32(pppppppppppfVar61) <= ALBW_AS_F32(pppppppppppfVar75)) {
        pppppppppppfVar61 = pppppppppppfVar75;
      }
      if (ALBW_AS_F32(pppppppppppfVar9) <= ALBW_AS_F32(pppppppppppfVar29)) {
        pppppppppppfVar9 = pppppppppppfVar29;
      }
      if (ALBW_AS_F32(pppppppppppfVar52) <= ALBW_AS_F32(pppppppppppfVar33)) {
        pppppppppppfVar52 = pppppppppppfVar33;
      }
      bVar44 = ALBW_AS_F32(pppppppppppfVar74[0x1f]) == ALBW_AS_F32(pppppppppppfVar61);
      bVar47 = ALBW_AS_F32(pppppppppppfVar61) <= ALBW_AS_F32(pppppppppppfVar74[0x1f]);
      if (!bVar47 || bVar44) {
        bVar44 = ALBW_AS_F32(pppppppppppfVar38) == ALBW_AS_F32(pppppppppppfVar74[0x22]);
        bVar47 = ALBW_AS_F32(pppppppppppfVar74[0x22]) <= ALBW_AS_F32(pppppppppppfVar38);
      }
      if (((bVar47 && !bVar44) || (ALBW_AS_F32(pppppppppppfVar9) < ALBW_AS_F32(pppppppppppfVar74[0x20]))) ||
         ((ALBW_AS_F32(pppppppppppfVar74[0x23]) < ALBW_AS_F32(pppppppppppfVar17) ||
          ((ALBW_AS_F32(pppppppppppfVar52) < ALBW_AS_F32(pppppppppppfVar74[0x21]) ||
           (ALBW_AS_F32(pppppppppppfVar74[0x24]) < ALBW_AS_F32(pppppppppppfVar62))))))) {
        if (pppppppppppfVar74[10] == ppppppppppfVar22) goto LAB_001fe470;
        ppppppppppfVar22 = *pppppppppppfVar74;
        pppppppppfVar23 = pppppppppfVar48;
        if ((int)pppppppppfVar48 + 1 <= (int)*ppppppppppfVar22) {
          do {
            pppppppppfVar24 = (float *********)ppppppppppfVar22[2][(int)pppppppppfVar23];
            pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
            *pppppppppfVar24 = (float ********)ppppppppppfVar22[3];
            ppppppppppfVar22[3] = pppppppppfVar24;
          } while ((int)pppppppppfVar23 < (int)pppppppppfVar48 + 1);
        }
        func_0048C320((int *)ppppppppppfVar22,(int)pppppppppfVar48,1);
      }
      else {
        pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
      }
    }
  }
  if (*pppppppppppfVar74[1] != (float *********)0x0) {
    in_d11 = CONCAT44((int)((ulonglong)in_d11 >> 0x20),pppppppppppfVar83);
    pppppppppfVar48 = (float *********)0x0;
    pppppppppppfVar81 = pppppppppppfVar15;
    while( true ) {
      pppppppppfVar23 = *pppppppppppfVar74[1];
      if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
      pppppppppppfVar49 = (float ***********)in_d11;
      if (pppppppppfVar48 < pppppppppfVar23) {
        ppppppppfVar14 = pppppppppppfVar74[1][2][(int)pppppppppfVar48];
      }
      else {
        ppppppppfVar14 = (float ********)0x0;
      }
      pppppppfVar51 = ppppppppfVar14[0xc];
      fVar58 = ALBW_AS_F32(ppppppppfVar14[0xe]) * ALBW_AS_F32(ppppppppfVar14[0x10]) -
               ALBW_AS_F32(ppppppppfVar14[0xd]) * ALBW_AS_F32(ppppppppfVar14[0x11]);
      fVar57 = ALBW_AS_F32(pppppppfVar51) * ALBW_AS_F32(ppppppppfVar14[0x11]) -
               ALBW_AS_F32(ppppppppfVar14[0xe]) * ALBW_AS_F32(ppppppppfVar14[0xf]);
      pppppppppppfVar50 = (float ***********)ppppppppfVar14[9];
      pppppppppppfVar75 = (float ***********)ppppppppfVar14[10];
      pppppppppppfVar78 = (float ***********)ppppppppfVar14[0xb];
      fVar60 = ALBW_AS_F32(ppppppppfVar14[0xd]) * ALBW_AS_F32(ppppppppfVar14[0xf]) -
               ALBW_AS_F32(pppppppfVar51) * ALBW_AS_F32(ppppppppfVar14[0x10]);
      fVar55 = fVar58 * ALBW_AS_F32(ppppppppfVar14[0x15]) + fVar57 * ALBW_AS_F32(ppppppppfVar14[0x16]) +
               fVar60 * ALBW_AS_F32(ppppppppfVar14[0x17]);
      pppppppppppfVar34 = pppppppppppfVar50;
      pppppppppppfVar9 = pppppppppppfVar78;
      pppppppppppfVar17 = pppppppppppfVar75;
      if (ALBW_AS_F32(pppppppppppfVar81) < fVar55) {
        fVar55 = ALBW_AS_F32(ppppppppfVar14[0x1a]) / fVar55;
        pppppppppppfVar34 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar50) + fVar58 * fVar55));
        pppppppppppfVar9 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar78) + fVar60 * fVar55));
        pppppppppppfVar17 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) + fVar57 * fVar55));
      }
      fVar58 = ALBW_AS_F32(ppppppppfVar14[0x14]) * ALBW_AS_F32(ppppppppfVar14[0xd]) -
               ALBW_AS_F32(ppppppppfVar14[0x13]) * ALBW_AS_F32(ppppppppfVar14[0xe]);
      fVar60 = ALBW_AS_F32(ppppppppfVar14[0x13]) * ALBW_AS_F32(pppppppfVar51) -
               ALBW_AS_F32(ppppppppfVar14[0x12]) * ALBW_AS_F32(ppppppppfVar14[0xd]);
      fVar57 = ALBW_AS_F32(ppppppppfVar14[0x12]) * ALBW_AS_F32(ppppppppfVar14[0xe]) -
               ALBW_AS_F32(ppppppppfVar14[0x14]) * ALBW_AS_F32(pppppppfVar51);
      fVar55 = fVar58 * ALBW_AS_F32(ppppppppfVar14[0x15]) + fVar57 * ALBW_AS_F32(ppppppppfVar14[0x16]) +
               fVar60 * ALBW_AS_F32(ppppppppfVar14[0x17]);
      pppppppppppfVar33 = pppppppppppfVar75;
      pppppppppppfVar29 = pppppppppppfVar78;
      pppppppppppfVar38 = pppppppppppfVar50;
      if (ALBW_AS_F32(pppppppppppfVar81) < fVar55) {
        fVar55 = ALBW_AS_F32(ppppppppfVar14[0x1a]) / fVar55;
        pppppppppppfVar33 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) + fVar57 * fVar55));
        pppppppppppfVar29 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar78) + fVar60 * fVar55));
        pppppppppppfVar38 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar50) + fVar58 * fVar55));
      }
      pppppppppppfVar28 = pppppppppppfVar50;
      if (ALBW_AS_F32(pppppppppppfVar79) <= ALBW_AS_F32(pppppppppppfVar50)) {
        pppppppppppfVar28 = pppppppppppfVar79;
      }
      pppppppppppfVar62 = pppppppppppfVar79;
      if (ALBW_AS_F32(pppppppppppfVar75) < ALBW_AS_F32(pppppppppppfVar79)) {
        pppppppppppfVar62 = pppppppppppfVar75;
      }
      pppppppppppfVar61 = pppppppppppfVar79;
      if (ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar79)) {
        pppppppppppfVar61 = pppppppppppfVar78;
      }
      if (ALBW_AS_F32(pppppppppppfVar50) < ALBW_AS_F32(pppppppppppfVar49)) {
        pppppppppppfVar50 = pppppppppppfVar49;
      }
      pppppppppppfVar63 = pppppppppppfVar49;
      if (ALBW_AS_F32(pppppppppppfVar49) <= ALBW_AS_F32(pppppppppppfVar75)) {
        pppppppppppfVar63 = pppppppppppfVar75;
      }
      if (ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar49)) {
        pppppppppppfVar78 = pppppppppppfVar49;
      }
      pppppppppppfVar49 = pppppppppppfVar34;
      if (ALBW_AS_F32(pppppppppppfVar28) <= ALBW_AS_F32(pppppppppppfVar34)) {
        pppppppppppfVar49 = pppppppppppfVar28;
      }
      pppppppppppfVar75 = pppppppppppfVar17;
      if (ALBW_AS_F32(pppppppppppfVar62) <= ALBW_AS_F32(pppppppppppfVar17)) {
        pppppppppppfVar75 = pppppppppppfVar62;
      }
      pppppppppppfVar28 = pppppppppppfVar9;
      if (ALBW_AS_F32(pppppppppppfVar61) <= ALBW_AS_F32(pppppppppppfVar9)) {
        pppppppppppfVar28 = pppppppppppfVar61;
      }
      if (ALBW_AS_F32(pppppppppppfVar34) < ALBW_AS_F32(pppppppppppfVar50)) {
        pppppppppppfVar34 = pppppppppppfVar50;
      }
      if (ALBW_AS_F32(pppppppppppfVar63) <= ALBW_AS_F32(pppppppppppfVar17)) {
        pppppppppppfVar63 = pppppppppppfVar17;
      }
      if (ALBW_AS_F32(pppppppppppfVar78) <= ALBW_AS_F32(pppppppppppfVar9)) {
        pppppppppppfVar78 = pppppppppppfVar9;
      }
      if (ALBW_AS_F32(pppppppppppfVar38) < ALBW_AS_F32(pppppppppppfVar49)) {
        pppppppppppfVar49 = pppppppppppfVar38;
      }
      if (ALBW_AS_F32(pppppppppppfVar33) < ALBW_AS_F32(pppppppppppfVar75)) {
        pppppppppppfVar75 = pppppppppppfVar33;
      }
      if (ALBW_AS_F32(pppppppppppfVar29) < ALBW_AS_F32(pppppppppppfVar28)) {
        pppppppppppfVar28 = pppppppppppfVar29;
      }
      if (ALBW_AS_F32(pppppppppppfVar34) <= ALBW_AS_F32(pppppppppppfVar38)) {
        pppppppppppfVar34 = pppppppppppfVar38;
      }
      if (ALBW_AS_F32(pppppppppppfVar63) <= ALBW_AS_F32(pppppppppppfVar33)) {
        pppppppppppfVar63 = pppppppppppfVar33;
      }
      if (ALBW_AS_F32(pppppppppppfVar78) <= ALBW_AS_F32(pppppppppppfVar29)) {
        pppppppppppfVar78 = pppppppppppfVar29;
      }
      bVar44 = ALBW_AS_F32(pppppppppppfVar74[0x1f]) == ALBW_AS_F32(pppppppppppfVar34);
      bVar47 = ALBW_AS_F32(pppppppppppfVar34) <= ALBW_AS_F32(pppppppppppfVar74[0x1f]);
      if (!bVar47 || bVar44) {
        bVar44 = ALBW_AS_F32(pppppppppppfVar49) == ALBW_AS_F32(pppppppppppfVar74[0x22]);
        bVar47 = ALBW_AS_F32(pppppppppppfVar74[0x22]) <= ALBW_AS_F32(pppppppppppfVar49);
      }
      if ((((bVar47 && !bVar44) || (ALBW_AS_F32(pppppppppppfVar63) < ALBW_AS_F32(pppppppppppfVar74[0x20]))) ||
          (ALBW_AS_F32(pppppppppppfVar74[0x23]) < ALBW_AS_F32(pppppppppppfVar75))) ||
         ((ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar74[0x21]) ||
          (ALBW_AS_F32(pppppppppppfVar74[0x24]) < ALBW_AS_F32(pppppppppppfVar28))))) {
        ppppppppppfVar22 = pppppppppppfVar74[1];
        pppppppppfVar23 = pppppppppfVar48;
        if ((int)pppppppppfVar48 + 1 <= (int)*ppppppppppfVar22) {
          do {
            pppppppppfVar24 = (float *********)ppppppppppfVar22[2][(int)pppppppppfVar23];
            pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
            *pppppppppfVar24 = (float ********)ppppppppppfVar22[3];
            ppppppppppfVar22[3] = pppppppppfVar24;
          } while ((int)pppppppppfVar23 < (int)pppppppppfVar48 + 1);
        }
        func_0048C320((int *)ppppppppppfVar22,(int)pppppppppfVar48,1);
      }
      else {
        pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
      }
    }
  }
  pppppppppppfVar49 = local_30;
  pppppppppppfVar81 = (float ***********)uVar70;
  pppppppppppfVar74 = (float ***********)((ulonglong)in_d11 >> 0x20);
  fVar55 = local_2c[0xc];
  uVar68 = (ulonglong)(uint)ALBW_AS_U32(fVar55);
  bVar44 = ALBW_AS_F32(pppppppppppfVar81) == fVar55;
  bVar47 = fVar55 <= ALBW_AS_F32(pppppppppppfVar81);
  pppppppppppfVar50 = local_30 + 2;
  if (!bVar47 || bVar44) {
    bVar44 = fVar55 == ALBW_AS_F32(pppppppppppfVar53);
    bVar47 = ALBW_AS_F32(pppppppppppfVar53) <= fVar55;
  }
  fVar55 = DAT_001ed5b4;
  if (bVar47 && !bVar44) {
    func_001FE680((undefined4 *)&stack0xffffe0d0);
    local_1ebc = (float ***********)uVar68;
    local_1f14 = (float ***********)&local_1e98;
    if (ALBW_AS_F32(local_1ebc) <= ALBW_AS_F32(pppppppppppfVar15)) {
      local_1ea4 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(local_1ebc));
      lVar69 = ZEXT48(local_1ea4) << 0x20;
      pppppppppppfVar49[0x1d] = (float **********)ALBW_PTR_BITS(-(fVar55 * ALBW_AS_F32(local_1ebc)));
      local_1ed0 = (float ***********)pppppppppppfVar49[10];
      local_1ecc = (float ************)pppppppppppfVar49[0xc];
      local_1ec8 = (float ***********)pppppppppppfVar49[0xd];
      local_1ec4 = (float ************)pppppppppppfVar49[0xe];
      local_1ec0 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar49[0x12]));
      local_1eb8 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar49[0x14]));
      local_1ebc = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar49[0x13]));
      ppppppppppfVar22 = pppppppppppfVar49[10];
      local_1eb4 = (float ************)ppppppppppfVar22[0xc];
      local_1eb0 = (float ***********)ppppppppppfVar22[0xd];
      local_1eac = (float ************)ppppppppppfVar22[0xe];
      local_1ea8 = (float ***********)DAT_001ed5b8;
      local_1f08 = (float ***********)0x0;
      local_1f0c = (float ***********)0x0;
      local_1ea0 = pppppppppppfVar50;
      func_0049F5FC(pppppppppppfVar49 + 2);
      func_0049F5FC(pppppppppppfVar49 + 6);
      pppppppppfVar48 = (float *********)0x0;
      while( true ) {
        pppppppppppfVar81 = (float ***********)uVar70;
        fVar58 = ALBW_AS_F32(((ulonglong)lVar69 >> 0x20));
        ppppppppppfVar22 = *pppppppppppfVar49;
        pppppppppfVar23 = *ppppppppppfVar22;
        if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
        if (pppppppppfVar48 < pppppppppfVar23) {
          ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
        }
        if (pppppppppfVar48 < pppppppppfVar23) {
          pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
        }
        else {
          pppppppppfVar23 = (float *********)0x0;
        }
        func_0010F1DC((int *)(pppppppppppfVar49 + 2),
                      (int *)((int)pppppppppppfVar49[5] + (int)pppppppppfVar23));
        pppppppppppfVar49[4] = (float **********)((int)pppppppppppfVar49[4] + 1);
        ppppppppppfVar22 = *pppppppppppfVar49;
        pppppppppfVar23 = *ppppppppppfVar22;
        if (pppppppppfVar48 < pppppppppfVar23) {
          ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
        }
        if (pppppppppfVar48 < pppppppppfVar23) {
          pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
        }
        else {
          pppppppppfVar23 = (float *********)0x0;
        }
        func_0010F1DC((int *)(pppppppppppfVar49 + 6),
                      (int *)((int)pppppppppppfVar49[9] + (int)pppppppppfVar23));
        pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
        pppppppppppfVar49[8] = (float **********)((int)pppppppppppfVar49[8] + 1);
      }
      func_0064E9F4((int *)pppppppppppfVar49,(undefined4 *)&stack0xffffe0d0,(int *)&local_1ed0);
      ppppppppppfVar22 =
           (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar49[0x1e]) - (fVar58 - ALBW_AS_F32(ppppppppppppfVar85))))
      ;
    }
    else {
      pppppppppppfVar49[0x1d] = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(local_1ebc) * fVar55));
      local_1ee8 = (float ************)pppppppppppfVar49[10];
      local_1ee4 = (float ***********)pppppppppppfVar49[0xc];
      local_1ee0 = (float ************)pppppppppppfVar49[0xd];
      local_1edc = (float ***********)pppppppppppfVar49[0xe];
      local_1ed8 = (float ***********)pppppppppppfVar49[0x12];
      local_1ed4 = (float ***********)pppppppppppfVar49[0x13];
      local_1ed0 = (float ***********)pppppppppppfVar49[0x14];
      ppppppppppfVar22 = pppppppppppfVar49[10];
      local_1ecc = (float ************)ppppppppppfVar22[0xc];
      local_1ec8 = (float ***********)ppppppppppfVar22[0xd];
      local_1ec4 = (float ************)ppppppppppfVar22[0xe];
      local_1ec0 = (float ************)DAT_001ed5b8;
      local_1f08 = (float ***********)0x0;
      local_1f0c = (float ***********)0x0;
      local_1eb8 = pppppppppppfVar50;
      func_0049F5FC(pppppppppppfVar49 + 2);
      func_0049F5FC(pppppppppppfVar49 + 6);
      pppppppppfVar48 = (float *********)0x0;
      while( true ) {
        pppppppppppfVar81 = (float ***********)uVar70;
        fVar58 = ALBW_AS_F32(uVar68);
        ppppppppppfVar22 = *pppppppppppfVar49;
        pppppppppfVar23 = *ppppppppppfVar22;
        if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
        if (pppppppppfVar48 < pppppppppfVar23) {
          ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
        }
        if (pppppppppfVar48 < pppppppppfVar23) {
          pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
        }
        else {
          pppppppppfVar23 = (float *********)0x0;
        }
        func_0010F1DC((int *)(pppppppppppfVar49 + 2),
                      (int *)((int)pppppppppppfVar49[5] + (int)pppppppppfVar23));
        pppppppppppfVar49[4] = (float **********)((int)pppppppppppfVar49[4] + 1);
        ppppppppppfVar22 = *pppppppppppfVar49;
        pppppppppfVar23 = *ppppppppppfVar22;
        if (pppppppppfVar48 < pppppppppfVar23) {
          ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
        }
        if (pppppppppfVar48 < pppppppppfVar23) {
          pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
        }
        else {
          pppppppppfVar23 = (float *********)0x0;
        }
        func_0010F1DC((int *)(pppppppppppfVar49 + 6),
                      (int *)((int)pppppppppppfVar49[9] + (int)pppppppppfVar23));
        pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
        pppppppppppfVar49[8] = (float **********)((int)pppppppppppfVar49[8] + 1);
      }
      func_0064CB28((int *)pppppppppppfVar49,(int *)&stack0xffffe0d0,(int *)&local_1ee8);
      ppppppppppfVar22 =
           (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar49[0x1e]) + (fVar58 - ALBW_AS_F32(ppppppppppppfVar85))))
      ;
    }
    pppppppppppfVar49[0x1e] = ppppppppppfVar22;
    if (local_1f10 != (float ***********)0x0) {
      *(float ***********)(DAT_001ecc48 + (int)pppppppppppfVar49) = *local_1f10;
      pppppppppppfVar49[0xc] = (float **********)in_stack_ffffe0d4;
      pppppppppppfVar49[0xd] = (float **********)local_1f28[0];
      pppppppppppfVar49[0xe] = (float **********)local_1f28[1];
      if ((float ***********)pppppppppppfVar49[10] != local_1f10) {
        pppppppppppfVar49[10] = (float **********)local_1f10;
        pppppppppppfVar49[0x18] = local_1f10[0xc];
        pppppppppppfVar49[0x19] = local_1f10[0xd];
        pppppppppppfVar49[0x1a] = local_1f10[0xe];
        pppppppfVar51 = DAT_001ed5b8;
        pppppppppppfVar49[0x15] = (float **********)*DAT_001ed5b8;
        pppppppppppfVar49[0x16] = (float **********)pppppppfVar51[1];
        pppppppppppfVar49[0x17] = (float **********)pppppppfVar51[2];
        func_004A17E4((float *)(pppppppppppfVar49 + 0x12));
      }
      pppppppppppfVar49[0xf] = (float **********)local_1e98;
      pppppppppppfVar49[0x10] = (float **********)local_1e94;
      pppppppppppfVar49[0x11] = (float **********)local_1e90;
    }
    *(bool *)(local_30 + 0xf70) = ALBW_AS_F32(pppppppppppfVar15) < local_2c[0xc];
  }
  pppppppppppfVar34 = local_30;
  pppppppppppfVar49 = (float ***********)DAT_001edb10;
  local_30[0x29] = (float **********)0x0;
  local_30[0x77c] = (float **********)0x0;
  pppppppppppfVar78 = local_30 + 0x29;
  if (getenv("ALBW_WALK_TRACE"))   /* WALKER debug: is the painting segment list rebuilt? */
    { extern long g_plat_frames;
      fprintf(stderr, "walk: f=%ld seglist walker %p skip %d dist %g limit %g mode %d pos (%.3f %.3f %.3f) row0 (%.3f %.3f %.3f) row2 (%.3f %.3f %.3f) segs %d\n",
            g_plat_frames, (void *)local_30, *(char *)(local_30 + 0xf70), ALBW_AS_F32(pppppppppppfVar15), local_2c[0xc], *(byte *)local_1e28,
            ALBW_AS_F32(local_30[0xc]), ALBW_AS_F32(local_30[0xd]), ALBW_AS_F32(local_30[0xe]),
            ALBW_AS_F32(local_30[0x12]), ALBW_AS_F32(local_30[0x13]), ALBW_AS_F32(local_30[0x14]),
            ALBW_AS_F32(local_30[0x18]), ALBW_AS_F32(local_30[0x19]), ALBW_AS_F32(local_30[0x1a]), (int)(long)local_30[0x29]); }
  if (*(char *)(local_30 + 0xf70) == '\0') {
    lVar69 = ZEXT48(pppppppppppfVar15) << 0x20;
    pppppppppppfVar75 = (float ***********)ALBW_PTR_BITS(local_2c[0x15]);
    pppppppppfVar48 = (float *********)ALBW_PTR_BITS(local_2c[0x14]);
    pppppppppppfVar9 = local_30;
    local_1df8 = (float ***********)(uint)(local_2c[0x1a] != 0.0);
    func_001FE680(&local_1ec8);
    ppppppppppfVar22 = *pppppppppppfVar78;
    if ((int)ppppppppppfVar22 < (int)pppppppppppfVar34[0x2a]) {
      ppppppppppfVar56 = pppppppppppfVar34[0x2b];
      *pppppppppppfVar78 = (float **********)((int)ppppppppppfVar22 + 1);
      ppppppppppfVar56 = ppppppppppfVar56 + (int)ppppppppppfVar22 * 0x27;
    }
    else {
      ppppppppppfVar56 = (float **********)0x0;
    }
    ppppppppppfVar56[0x1c] = (float *********)pppppppppppfVar75;
    ppppppppppfVar56[9] = (float *********)pppppppppppfVar9[0xc];
    ppppppppppfVar56[10] = (float *********)pppppppppppfVar9[0xd];
    ppppppppppfVar56[0xb] = (float *********)pppppppppppfVar9[0xe];
    *(undefined1 *)(ppppppppppfVar56 + 0x24) = 0;
    pppppppppppfVar9[0x1d] = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) * fVar55));
    local_1ea0 = (float ***********)0x0;
    local_1e98 = (float ***********)pppppppppppfVar9[10];
    local_1e94 = (float ***********)pppppppppppfVar9[0xc];
    local_1e90 = (float ***********)pppppppppppfVar9[0xd];
    local_1e8c = (float ***********)pppppppppppfVar9[0xe];
    local_1e88 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar9[0x12]));
    local_1e84 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar9[0x13]));
    local_1e80 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar9[0x14]));
    ppppppppppfVar22 = pppppppppppfVar9[10];
    local_1e7c = (float ***********)ppppppppppfVar22[0xc];
    local_1e78 = (float ***********)ppppppppppfVar22[0xd];
    local_1e74 = (float ***********)ppppppppppfVar22[0xe];
    local_1e70 = (float ***********)DAT_001f5fdc;
    pppppppppppfVar17 = pppppppppppfVar9;
    local_1ea4 = pppppppppppfVar78;
    local_1e6c = pppppppppppfVar75;
    local_1e68 = pppppppppppfVar50;
    local_1e30 = pppppppppppfVar78;
    local_1e04 = pppppppppppfVar50;
    if (pppppppppppfVar78 != (float ***********)0x0) {
      ppppppppppfVar22 = pppppppppppfVar34[0x2b] + (int)*pppppppppppfVar78 * 0x27 + -0x27;
      *ppppppppppfVar22 = (float *********)local_1e88;
      ppppppppppfVar22[1] = (float *********)local_1e84;
      ppppppppppfVar22[2] = (float *********)local_1e80;
      ppppppppppfVar22[6] = (float *********)local_1e7c;
      ppppppppppfVar22[7] = (float *********)local_1e78;
      ppppppppppfVar22[8] = (float *********)local_1e74;
      func_004A17A4((float *)ppppppppppfVar22);
      local_1e64 = (float ***********)((int)*local_1ea4 + -1);
    }
    func_0049F5FC(pppppppppppfVar9 + 2);
    func_0049F5FC(pppppppppppfVar9 + 6);
    pppppppppfVar23 = (float *********)0x0;
    while( true ) {
      ppppppppppfVar22 = *pppppppppppfVar9;
      pppppppppfVar24 = *ppppppppppfVar22;
      if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
      if (pppppppppfVar23 < pppppppppfVar24) {
        ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
      }
      if (pppppppppfVar23 < pppppppppfVar24) {
        pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
      }
      else {
        pppppppppfVar24 = (float *********)0x0;
      }
      func_0010F1DC((int *)(pppppppppppfVar9 + 2),
                    (int *)((int)pppppppppppfVar9[5] + (int)pppppppppfVar24));
      pppppppppppfVar9[4] = (float **********)((int)pppppppppppfVar9[4] + 1);
      ppppppppppfVar22 = *pppppppppppfVar9;
      pppppppppfVar24 = *ppppppppppfVar22;
      if (pppppppppfVar23 < pppppppppfVar24) {
        ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
      }
      if (pppppppppfVar23 < pppppppppfVar24) {
        pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
      }
      else {
        pppppppppfVar24 = (float *********)0x0;
      }
      func_0010F1DC((int *)(pppppppppppfVar9 + 6),
                    (int *)((int)pppppppppppfVar9[9] + (int)pppppppppfVar24));
      pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
      pppppppppppfVar9[8] = (float **********)((int)pppppppppppfVar9[8] + 1);
    }
    func_0064E9F4((int *)pppppppppppfVar9,&local_1ec8,(int *)&local_1e98);
    if (local_1ea8 != (float ***********)0x0) {
      pppppppppppfVar17[0xedf] = (float **********)local_1ec4;
      pppppppppppfVar17[0xee0] = (float **********)local_1ec0;
      pppppppppppfVar17[0xee1] = (float **********)local_1ebc;
      pppppppppppfVar17[0xee5] = local_1ea8[0xc];
      pppppppppppfVar17[0xee6] = local_1ea8[0xd];
      pppppppppppfVar17[0xee7] = local_1ea8[0xe];
      pppppppppppfVar50 = pppppppppppfVar15;
      fVar58 = fVar5;
      if (DAT_001f5fe0 < (int)local_1ec8) {
        ppppppppppfVar22 = pppppppppppfVar17[0x1e];
        bVar44 = ALBW_AS_F32(ppppppppppfVar22) < ALBW_AS_F32(pppppppppppfVar15);
        bVar47 = NAN(ALBW_AS_F32(ppppppppppfVar22)) || NAN(ALBW_AS_F32(pppppppppppfVar15));
        if (ALBW_AS_F32(ppppppppppfVar22) != ALBW_AS_F32(pppppppppppfVar15)) {
          bVar47 = SBORROW4((int)local_1ec8,(int)DAT_001f5fe4);
          bVar44 = (int)local_1ec8 - (int)DAT_001f5fe4 < 0;
        }
        pppppppppppfVar34 = local_1ec8;
        if ((ALBW_AS_F32(ppppppppppfVar22) == ALBW_AS_F32(pppppppppppfVar15) ||
            local_1ec8 == (float ***********)DAT_001f5fe4) || bVar44 != bVar47) {
          lVar69 = ZEXT48(local_1ec8) << 0x20;
          local_1dc4 = (float ***********)&local_1dc0;
          local_1dc8 = (float ***********)&DAT_00000030;
          local_1dcc = (float **********)0x0;
          func_00100118((int)&local_1dc0,DAT_001f5fe8,0x9c,0x30);
          if ((int)local_1dcc < (int)local_1dc8) {
            ppppppppppppfVar85 = (float ************)(local_1dc4 + (int)local_1dcc * 0x27);
            local_1dcc = (float **********)((int)local_1dcc + 1);
          }
          else {
            ppppppppppppfVar85 = (float ************)0x0;
          }
          ppppppppppppfVar85[0x1c] = (float ***********)((ulonglong)lVar69 >> 0x20);
          ppppppppppppfVar85[9] = (float ***********)pppppppppppfVar17[0xc];
          ppppppppppppfVar85[10] = (float ***********)pppppppppppfVar17[0xd];
          ppppppppppppfVar85[0xb] = (float ***********)pppppppppppfVar17[0xe];
          *(undefined1 *)(ppppppppppppfVar85 + 0x24) = 1;
          pppppppppppfVar34 = local_1ec8;
          pppppppppppfVar78 = pppppppppppfVar17;
          func_001FE680(local_1f28);
          local_1f0c = (float ***********)&local_1e3c;
          local_1f04 = &local_1dcc;
          if (ALBW_AS_F32(pppppppppppfVar34) <= ALBW_AS_F32(pppppppppppfVar15)) {
            local_1e6c = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar34));
            lVar69 = CONCAT44((int)((ulonglong)lVar69 >> 0x20),local_1e6c);
            pppppppppppfVar17[0x1d] = (float **********)ALBW_PTR_BITS(-(fVar55 * ALBW_AS_F32(pppppppppppfVar34)));
            local_1f00[0] = (float ***********)0x0;
            local_1e98 = (float ***********)pppppppppppfVar17[10];
            local_1e94 = (float ***********)pppppppppppfVar17[0xc];
            local_1e90 = (float ***********)pppppppppppfVar17[0xd];
            local_1e8c = (float ***********)pppppppppppfVar17[0xe];
            local_1e88 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar17[0x12]));
            local_1e80 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar17[0x14]));
            local_1e84 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar17[0x13]));
            ppppppppppfVar22 = pppppppppppfVar17[10];
            local_1e7c = (float ***********)ppppppppppfVar22[0xc];
            local_1e78 = (float ***********)ppppppppppfVar22[0xd];
            local_1e74 = (float ***********)ppppppppppfVar22[0xe];
            local_1e70 = (float ***********)DAT_001f5fdc;
            local_1e68 = local_1e04;
            if (&local_1dcc != (float ***********)0x0) {
              ppppppppppppfVar85 = (float ************)(local_1dc4 + (int)local_1dcc * 0x27 + -0x27)
              ;
              *ppppppppppppfVar85 = local_1e88;
              ppppppppppppfVar85[1] = local_1e84;
              ppppppppppppfVar85[2] = local_1e80;
              ppppppppppppfVar85[6] = local_1e7c;
              ppppppppppppfVar85[7] = local_1e78;
              ppppppppppppfVar85[8] = local_1e74;
              func_004A17A4((float *)ppppppppppppfVar85);
              local_1e64 = (float ***********)((int)*local_1f04 + -1);
            }
            func_0049F5FC(pppppppppppfVar17 + 2);
            func_0049F5FC(pppppppppppfVar17 + 6);
            pppppppppfVar23 = (float *********)0x0;
            while( true ) {
              ppppppppppfVar22 = *pppppppppppfVar17;
              pppppppppfVar24 = *ppppppppppfVar22;
              if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
              if (pppppppppfVar23 < pppppppppfVar24) {
                ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
              }
              if (pppppppppfVar23 < pppppppppfVar24) {
                pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
              }
              else {
                pppppppppfVar24 = (float *********)0x0;
              }
              func_0010F1DC((int *)(pppppppppppfVar17 + 2),
                            (int *)((int)pppppppppppfVar17[5] + (int)pppppppppfVar24));
              pppppppppppfVar17[4] = (float **********)((int)pppppppppppfVar17[4] + 1);
              ppppppppppfVar22 = *pppppppppppfVar17;
              pppppppppfVar24 = *ppppppppppfVar22;
              if (pppppppppfVar23 < pppppppppfVar24) {
                ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
              }
              if (pppppppppfVar23 < pppppppppfVar24) {
                pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
              }
              else {
                pppppppppfVar24 = (float *********)0x0;
              }
              func_0010F1DC((int *)(pppppppppppfVar17 + 6),
                            (int *)((int)pppppppppppfVar17[9] + (int)pppppppppfVar24));
              pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
              pppppppppppfVar17[8] = (float **********)((int)pppppppppppfVar17[8] + 1);
            }
            func_0064E9F4((int *)pppppppppppfVar17,local_1f28,(int *)&local_1e98);
            ppppppppppfVar22 =
                 (float **********)
                 ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar17[0x1e]) - (ALBW_AS_F32(lVar69) - ALBW_AS_F32(local_1f28[0]))));
          }
          else {
            pppppppppppfVar17[0x1d] = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar34) * fVar55));
            local_1f00[0] = (float ***********)0x0;
            local_1e98 = (float ***********)pppppppppppfVar17[10];
            local_1e94 = (float ***********)pppppppppppfVar17[0xc];
            local_1e90 = (float ***********)pppppppppppfVar17[0xd];
            local_1e8c = (float ***********)pppppppppppfVar17[0xe];
            local_1e88 = (float ***********)pppppppppppfVar17[0x12];
            local_1e84 = (float ***********)pppppppppppfVar17[0x13];
            local_1e80 = (float ***********)pppppppppppfVar17[0x14];
            ppppppppppfVar22 = pppppppppppfVar17[10];
            local_1e7c = (float ***********)ppppppppppfVar22[0xc];
            local_1e78 = (float ***********)ppppppppppfVar22[0xd];
            local_1e74 = (float ***********)ppppppppppfVar22[0xe];
            local_1e70 = (float ***********)DAT_001f5fdc;
            local_1e68 = local_1e04;
            local_1e6c = pppppppppppfVar34;
            if (&local_1dcc != (float ***********)0x0) {
              ppppppppppppfVar85 = (float ************)(local_1dc4 + (int)local_1dcc * 0x27 + -0x27)
              ;
              *ppppppppppppfVar85 = local_1e88;
              ppppppppppppfVar85[1] = local_1e84;
              ppppppppppppfVar85[2] = local_1e80;
              ppppppppppppfVar85[6] = local_1e7c;
              ppppppppppppfVar85[7] = local_1e78;
              ppppppppppppfVar85[8] = local_1e74;
              func_004A17A4((float *)ppppppppppppfVar85);
              local_1e64 = (float ***********)((int)*local_1f04 + -1);
            }
            func_0049F5FC(pppppppppppfVar17 + 2);
            func_0049F5FC(pppppppppppfVar17 + 6);
            pppppppppfVar23 = (float *********)0x0;
            while( true ) {
              ppppppppppfVar22 = *pppppppppppfVar17;
              pppppppppfVar24 = *ppppppppppfVar22;
              if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
              if (pppppppppfVar23 < pppppppppfVar24) {
                ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
              }
              if (pppppppppfVar23 < pppppppppfVar24) {
                pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
              }
              else {
                pppppppppfVar24 = (float *********)0x0;
              }
              func_0010F1DC((int *)(pppppppppppfVar17 + 2),
                            (int *)((int)pppppppppppfVar17[5] + (int)pppppppppfVar24));
              pppppppppppfVar17[4] = (float **********)((int)pppppppppppfVar17[4] + 1);
              ppppppppppfVar22 = *pppppppppppfVar17;
              pppppppppfVar24 = *ppppppppppfVar22;
              if (pppppppppfVar23 < pppppppppfVar24) {
                ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
              }
              if (pppppppppfVar23 < pppppppppfVar24) {
                pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
              }
              else {
                pppppppppfVar24 = (float *********)0x0;
              }
              func_0010F1DC((int *)(pppppppppppfVar17 + 6),
                            (int *)((int)pppppppppppfVar17[9] + (int)pppppppppfVar24));
              pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
              pppppppppppfVar17[8] = (float **********)((int)pppppppppppfVar17[8] + 1);
            }
            func_0064CB28((int *)pppppppppppfVar17,(int *)local_1f28,(int *)&local_1e98);
            ppppppppppfVar22 =
                 (float **********)
                 ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar17[0x1e]) + (ALBW_AS_F32(pppppppppppfVar34) - ALBW_AS_F32(local_1f28[0]))
                 ));
          }
          pppppppppppfVar17[0x1e] = ppppppppppfVar22;
          if (local_1f08 != (float ***********)0x0) {
            *(float ***********)(DAT_001f5fec + (int)pppppppppppfVar17) = *local_1f08;
            pppppppppppfVar17[0xc] = (float **********)local_1f28[1];
            pppppppppppfVar17[0xd] = (float **********)local_1f28[2];
            pppppppppppfVar17[0xe] = (float **********)local_1f28[3];
            if ((float ***********)pppppppppppfVar17[10] != local_1f08) {
              pppppppppppfVar17[10] = (float **********)local_1f08;
              pppppppppppfVar17[0x18] = local_1f08[0xc];
              pppppppppppfVar17[0x19] = local_1f08[0xd];
              pppppppppppfVar17[0x1a] = local_1f08[0xe];
              pppppppfVar51 = DAT_001f5fdc;
              pppppppppppfVar17[0x15] = (float **********)*DAT_001f5fdc;
              pppppppppppfVar17[0x16] = (float **********)pppppppfVar51[1];
              pppppppppppfVar17[0x17] = (float **********)pppppppfVar51[2];
              func_004A17E4((float *)(pppppppppppfVar17 + 0x12));
            }
            pppppppppppfVar17[0xf] = (float **********)local_1e3c;
            pppppppppppfVar17[0x10] = (float **********)local_1e38;
            pppppppppppfVar17[0x11] = (float **********)local_1e34;
          }
          pppppppppppfVar17 = pppppppppppfVar78;
          local_1ec8 = local_1f28[0];
          pppppppppppfVar34 = local_1f28[0];
          if ((int)local_1f28[0] <= DAT_001f4bdc) {
            fVar58 = fVar5;
            if ((float ***********)local_1dcc == (float ***********)0x1) {
              if (*local_1ea4 == (float **********)0x0) {
                ppppppppppfVar22 = (float **********)0x0;
              }
              else {
                ppppppppppfVar22 = local_1ea4[2];
              }
              ppppppppppfVar22[9] = (float *********)pppppppppppfVar17[0xc];
              ppppppppppfVar22[10] = (float *********)pppppppppppfVar17[0xd];
              ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar17[0xe];
              ppppppppppfVar22[0x1c] =
                   (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) + ALBW_AS_F32(((ulonglong)lVar69 >> 0x20))));
            }
            else {
              ppppppppppppfVar85 = (float ************)0x0;
              pppppppppppfVar34 = (float ***********)0x0;
              if (*local_1ea4 == (float **********)0x0) {
                ppppppppppfVar22 = (float **********)0x0;
              }
              else {
                ppppppppppfVar22 = local_1ea4[2];
              }
              if (0 < (int)local_1dcc) {
                do {
                  ppppppppppppfVar13 = (float ************)local_1dc4;
                  if (pppppppppppfVar34 < local_1dcc) {
                    ppppppppppppfVar13 =
                         (float ************)(local_1dc4 + (int)pppppppppppfVar34 * 0x27);
                  }
                  if (((ALBW_AS_F32(ppppppppppfVar22[6]) == ALBW_AS_F32(ppppppppppppfVar13[6])) &&
                      (ALBW_AS_F32(ppppppppppfVar22[7]) == ALBW_AS_F32(ppppppppppppfVar13[7]))) &&
                     (ALBW_AS_F32(ppppppppppfVar22[8]) == ALBW_AS_F32(ppppppppppppfVar13[8]))) {
                    ppppppppppfVar22[9] = (float *********)ppppppppppppfVar13[0xc];
                    ppppppppppfVar22[10] = (float *********)ppppppppppppfVar13[0xd];
                    ppppppppppfVar22[0xb] = (float *********)ppppppppppppfVar13[0xe];
                    pppppppppppfVar15 =
                         (float ***********)
                         ALBW_PTR_BITS((ALBW_AS_F32(ppppppppppppfVar13[0x1c]) - ALBW_AS_F32(ppppppppppppfVar13[0x1d])));
                    ppppppppppfVar22[0x1c] =
                         (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) + ALBW_AS_F32(pppppppppppfVar15)));
                    ppppppppppppfVar13 = ppppppppppppfVar85;
                  }
                  else {
                    *(undefined1 *)(ppppppppppppfVar13 + 0x24) = 0;
                    pppppppppppfVar15 =
                         (float ***********)
                         ALBW_PTR_BITS(((ALBW_AS_F32(ppppppppppppfVar13[0x1c]) - ALBW_AS_F32(ppppppppppppfVar13[0x1d])) +
                         ALBW_AS_F32(pppppppppppfVar15)));
                    if (ppppppppppppfVar85 == (float ************)0x0) {
                      pppppppppppfVar78 = (float ***********)ppppppppppfVar22[0x1c];
                    }
                    else {
                      pppppppppppfVar78 = ppppppppppppfVar85[0x1c];
                    }
                    ppppppppppppfVar13[0x1d] = pppppppppppfVar78;
                    ppppppppppppfVar13[0x1c] =
                         (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) + ALBW_AS_F32(pppppppppppfVar15)));
                    pppppppppppfVar78 = ppppppppppppfVar13[9];
                    pppppppppppfVar9 = ppppppppppppfVar13[10];
                    pppppppppppfVar33 = ppppppppppppfVar13[0xb];
                    ppppppppppppfVar13[9] = ppppppppppppfVar13[0xc];
                    ppppppppppppfVar13[10] = ppppppppppppfVar13[0xd];
                    ppppppppppppfVar13[0xb] = ppppppppppppfVar13[0xe];
                    ppppppppppppfVar13[0xc] = pppppppppppfVar78;
                    ppppppppppppfVar13[0xd] = pppppppppppfVar9;
                    ppppppppppppfVar13[0xe] = pppppppppppfVar33;
                    func_00115D70((float *)ppppppppppppfVar13,(float *)ppppppppppppfVar13,fVar5);
                    func_00115D70((float *)(ppppppppppppfVar13 + 3),
                                  (float *)(ppppppppppppfVar13 + 3),fVar5);
                    ppppppppppfVar56 = *local_1ea4;
                    if ((int)ppppppppppfVar56 < (int)local_1ea4[1]) {
                      *local_1ea4 = (float **********)((int)ppppppppppfVar56 + 1);
                      ppppppppppfVar56 = local_1ea4[2] + (int)ppppppppppfVar56 * 0x27;
                      local_1f08 = (float ***********)(ppppppppppppfVar13 + 0xc);
                      *ppppppppppfVar56 = (float *********)*ppppppppppppfVar13;
                      ppppppppppfVar56[1] = (float *********)ppppppppppppfVar13[1];
                      local_1f28[4] = (float ***********)(ppppppppppfVar56 + 0xf);
                      ppppppppppfVar56[2] = (float *********)ppppppppppppfVar13[2];
                      ppppppppppfVar56[3] = (float *********)ppppppppppppfVar13[3];
                      ppppppppppfVar56[4] = (float *********)ppppppppppppfVar13[4];
                      ppppppppppfVar56[5] = (float *********)ppppppppppppfVar13[5];
                      ppppppppppfVar56[6] = (float *********)ppppppppppppfVar13[6];
                      ppppppppppfVar56[7] = (float *********)ppppppppppppfVar13[7];
                      ppppppppppfVar56[8] = (float *********)ppppppppppppfVar13[8];
                      ppppppppppfVar56[9] = (float *********)ppppppppppppfVar13[9];
                      ppppppppppfVar56[10] = (float *********)ppppppppppppfVar13[10];
                      ppppppppppfVar56[0xb] = (float *********)ppppppppppppfVar13[0xb];
                      ppppppppppfVar56[0xc] = (float *********)*local_1f08;
                      ppppppppppfVar56[0xd] = (float *********)ppppppppppppfVar13[0xd];
                      ppppppppppfVar56[0xe] = (float *********)ppppppppppppfVar13[0xe];
                      *local_1f28[4] = (float **********)ppppppppppppfVar13[0xf];
                      ppppppppppfVar56[0x10] = (float *********)ppppppppppppfVar13[0x10];
                      ppppppppppfVar56[0x11] = (float *********)ppppppppppppfVar13[0x11];
                      ppppppppppfVar56[0x12] = (float *********)ppppppppppppfVar13[0x12];
                      ppppppppppfVar56[0x13] = (float *********)ppppppppppppfVar13[0x13];
                      ppppppppppfVar56[0x14] = (float *********)ppppppppppppfVar13[0x14];
                      ppppppppppfVar56[0x15] = (float *********)ppppppppppppfVar13[0x15];
                      ppppppppppfVar56[0x16] = (float *********)ppppppppppppfVar13[0x16];
                      ppppppppppfVar56[0x17] = (float *********)ppppppppppppfVar13[0x17];
                      ppppppppppfVar56[0x18] = (float *********)ppppppppppppfVar13[0x18];
                      ppppppppppfVar56[0x19] = (float *********)ppppppppppppfVar13[0x19];
                      ppppppppppfVar56[0x1a] = (float *********)ppppppppppppfVar13[0x1a];
                      ppppppppppfVar56[0x1b] = (float *********)ppppppppppppfVar13[0x1b];
                      ppppppppppfVar56[0x1c] = (float *********)ppppppppppppfVar13[0x1c];
                      ppppppppppfVar56[0x1d] = (float *********)ppppppppppppfVar13[0x1d];
                      ppppppppppfVar56[0x1e] = (float *********)ppppppppppppfVar13[0x1e];
                      ppppppppppfVar56[0x1f] = (float *********)ppppppppppppfVar13[0x1f];
                      ppppppppppfVar56[0x20] = (float *********)ppppppppppppfVar13[0x20];
                      ppppppppppfVar56[0x21] = (float *********)ppppppppppppfVar13[0x21];
                      ppppppppppfVar56[0x22] = (float *********)ppppppppppppfVar13[0x22];
                      ppppppppppfVar56[0x23] = (float *********)ppppppppppppfVar13[0x23];
                      *(undefined1 *)(ppppppppppfVar56 + 0x24) =
                           *(undefined1 *)(ppppppppppppfVar13 + 0x24);
                      *(undefined1 *)((int)ppppppppppfVar56 + 0x91) =
                           *(undefined1 *)((int)ppppppppppppfVar13 + 0x91);
                      *(undefined1 *)((int)ppppppppppfVar56 + 0x92) =
                           *(undefined1 *)((int)ppppppppppppfVar13 + 0x92);
                      *(undefined1 *)((int)ppppppppppfVar56 + 0x93) =
                           *(undefined1 *)((int)ppppppppppppfVar13 + 0x93);
                      *(undefined1 *)(ppppppppppfVar56 + 0x25) =
                           *(undefined1 *)(ppppppppppppfVar13 + 0x25);
                      *(undefined1 *)((int)ppppppppppfVar56 + 0x95) =
                           *(undefined1 *)((int)ppppppppppppfVar13 + 0x95);
                      *(undefined1 *)((int)ppppppppppfVar56 + 0x96) =
                           *(undefined1 *)((int)ppppppppppppfVar13 + 0x96);
                      *(undefined1 *)((int)ppppppppppfVar56 + 0x97) =
                           *(undefined1 *)((int)ppppppppppppfVar13 + 0x97);
                      *(undefined1 *)(ppppppppppfVar56 + 0x26) =
                           *(undefined1 *)(ppppppppppppfVar13 + 0x26);
                    }
                  }
                  pppppppppppfVar34 = (float ***********)((int)pppppppppppfVar34 + 1);
                  ppppppppppppfVar85 = ppppppppppppfVar13;
                } while ((int)pppppppppppfVar34 < (int)local_1dcc);
              }
            }
            goto LAB_001f5a48;
          }
        }
      }
      else {
LAB_001f5a48:
        func_001FE680(local_1dd8);
        local_1db4 = local_1e30;
        ppppppppppfVar22 = *local_1e30;
        if ((int)ppppppppppfVar22 < (int)local_1e30[1]) {
          *local_1e30 = (float **********)((int)ppppppppppfVar22 + 1);
          ppppppppppfVar56 = local_1e30[2] + (int)ppppppppppfVar22 * 0x27;
        }
        else {
          ppppppppppfVar56 = (float **********)0x0;
        }
        local_1e9c = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar48) + ALBW_AS_F32(local_1ec8)));
        ppppppppppfVar56[0x1c] = pppppppppfVar48;
        pppppppfVar51 = DAT_001f5fdc;
        ppppppppppfVar56[9] = (float *********)pppppppppppfVar17[0xc];
        ppppppppppfVar56[10] = (float *********)pppppppppppfVar17[0xd];
        ppppppppppfVar56[0xb] = (float *********)pppppppppppfVar17[0xe];
        *(undefined1 *)(ppppppppppfVar56 + 0x24) = 1;
        pppppppppppfVar17[0x1d] = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(local_1e9c) * fVar55));
        local_1db0 = (float ***********)0x0;
        local_1ec8 = (float ***********)pppppppppppfVar17[10];
        local_1ec4 = (float ************)pppppppppppfVar17[0xc];
        local_1ec0 = (float ************)pppppppppppfVar17[0xd];
        local_1ebc = (float ***********)pppppppppppfVar17[0xe];
        local_1eb8 = (float ***********)pppppppppppfVar17[0x12];
        local_1eb4 = (float ************)pppppppppppfVar17[0x13];
        local_1eb0 = (float ***********)pppppppppppfVar17[0x14];
        ppppppppppfVar56 = pppppppppppfVar17[10];
        local_1eac = (float ************)ppppppppppfVar56[0xc];
        local_1ea8 = (float ***********)ppppppppppfVar56[0xd];
        local_1ea4 = (float ***********)ppppppppppfVar56[0xe];
        local_1ea0 = (float ***********)pppppppfVar51;
        local_1e98 = local_1e04;
        pppppppppppfVar15 = pppppppppppfVar17;
        if (local_1e30 != (float ***********)0x0) {
          ppppppppppfVar56 = local_1e30[2] + (int)*local_1e30 * 0x27 + -0x27;
          *ppppppppppfVar56 = (float *********)local_1eb8;
          ppppppppppfVar56[1] = (float *********)local_1eb4;
          ppppppppppfVar56[2] = (float *********)local_1eb0;
          ppppppppppfVar56[6] = (float *********)local_1eac;
          ppppppppppfVar56[7] = (float *********)local_1ea8;
          ppppppppppfVar56[8] = (float *********)local_1ea4;
          func_004A17A4((float *)ppppppppppfVar56);
          local_1e94 = (float ***********)((int)*local_1db4 + -1);
        }
        local_1f28[2] = pppppppppppfVar17;
        func_0049F5FC(pppppppppppfVar17 + 2);
        func_0049F5FC(pppppppppppfVar17 + 6);
        pppppppppfVar23 = (float *********)0x0;
        while( true ) {
          uVar80 = (undefined4)((ulonglong)lVar69 >> 0x20);
          ppppppppppfVar56 = *pppppppppppfVar17;
          pppppppppfVar24 = *ppppppppppfVar56;
          if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
          if (pppppppppfVar23 < pppppppppfVar24) {
            ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
          }
          if (pppppppppfVar23 < pppppppppfVar24) {
            pppppppppfVar24 = ppppppppppfVar56[(int)pppppppppfVar23];
          }
          else {
            pppppppppfVar24 = (float *********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar17 + 2),
                        (int *)((int)pppppppppppfVar17[5] + (int)pppppppppfVar24));
          pppppppppppfVar17[4] = (float **********)((int)pppppppppppfVar17[4] + 1);
          ppppppppppfVar56 = *pppppppppppfVar17;
          pppppppppfVar24 = *ppppppppppfVar56;
          if (pppppppppfVar23 < pppppppppfVar24) {
            ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
          }
          if (pppppppppfVar23 < pppppppppfVar24) {
            pppppppppfVar24 = ppppppppppfVar56[(int)pppppppppfVar23];
          }
          else {
            pppppppppfVar24 = (float *********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar17 + 6),
                        (int *)((int)pppppppppppfVar17[9] + (int)pppppppppfVar24));
          pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
          pppppppppppfVar17[8] = (float **********)((int)pppppppppppfVar17[8] + 1);
        }
        func_0064CB28((int *)local_1f28[2],(int *)local_1dd8,(int *)&local_1ec8);
        if (local_1db8 == (float ************)0x0) goto LAB_001f6210;
        uVar67 = CONCAT44(uVar80,pppppppppppfVar50);
        pppppppppppfVar15[0xedc] = (float **********)local_1dd8[1];
        pppppppppppfVar15[0xedd] = (float **********)local_1dd8[2];
        pppppppppppfVar15[0xede] = local_1dcc;
        pppppppppppfVar15[0xee2] = (float **********)local_1db8[0xc];
        pppppppppppfVar15[0xee3] = (float **********)local_1db8[0xd];
        pppppppppppfVar15[0xee4] = (float **********)local_1db8[0xe];
        pppppppppppfVar17 = pppppppppppfVar15;
        if ((int)local_1dd8[0] <= DAT_001f5fe0) {
          if (local_1df8 == (float ***********)0x0) goto LAB_001fc7a8;
          pfVar39 = (float *)(local_1f00 + 2);
          func_001FE680(pfVar39);
          local_1f28[2] = (float ***********)uVar67;
          uVar72 = CONCAT44(local_1f28[2],pppppppppppfVar79);
          local_1ed4 = pppppppppppfVar15 + 0x77c;
          *local_1ed4 = (float **********)0x0;
          ppppppppppfVar22 = *local_1ed4;
          if ((int)ppppppppppfVar22 < (int)pppppppppppfVar15[0x77d]) {
            *local_1ed4 = (float **********)((int)ppppppppppfVar22 + 1);
            ppppppppppfVar22 = pppppppppppfVar15[0x77e] + (int)ppppppppppfVar22 * 0x27;
          }
          else {
            ppppppppppfVar22 = (float **********)0x0;
          }
          local_1f28[1] = (float ***********)DAT_001f6734;
          uVar71 = CONCAT44(DAT_001f6734,pppppppppfVar48);
          ppppppppppfVar22[0x1c] = (float *********)pppppppppppfVar75;
          ppppppppppfVar22[9] = (float *********)pppppppppppfVar15[0xf];
          ppppppppppfVar22[10] = (float *********)pppppppppppfVar15[0x10];
          ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar15[0x11];
          *(undefined1 *)(ppppppppppfVar22 + 0x24) = 0;
          local_1ed0 = (float ***********)0x0;
          local_1ec8 = (float ***********)pppppppppppfVar15[0xb];
          local_1ec4 = (float ************)pppppppppppfVar15[0xf];
          local_1ec0 = (float ************)pppppppppppfVar15[0x10];
          local_1ebc = (float ***********)pppppppppppfVar15[0x11];
          local_1f0c = (float ***********)local_1ec8[0xc];
          local_1f08 = (float ***********)local_1ec8[0xd];
          local_1f04 = (float ***********)local_1ec8[0xe];
          local_1f28[4] = (float ***********)*pppppppfVar51;
          local_1f14 = (float ***********)pppppppfVar51[1];
          local_1f10 = (float ***********)pppppppfVar51[2];
          pppppppppppfVar52 = pppppppppppfVar15;
          local_1f28[3] = local_1f28[2];
          local_1e88 = local_1ed4;
          func_004A17E4((float *)(local_1f28 + 1));
          local_1eb8 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(local_1f28[1]));
          local_1eb0 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(local_1f28[3]));
          local_1eb4 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(local_1f28[2]));
          ppppppppppfVar22 = pppppppppppfVar15[0xb];
          local_1eac = (float ************)ppppppppppfVar22[0xc];
          local_1ea8 = (float ***********)ppppppppppfVar22[0xd];
          local_1ea4 = (float ***********)ppppppppppfVar22[0xe];
          local_1ea0 = (float ***********)DAT_001f5fdc;
          local_1e98 = local_1e04;
          local_1e9c = pppppppppppfVar75;
          if (local_1ed4 != (float ***********)0x0) {
            ppppppppppfVar22 = local_1ed4[2] + (int)*local_1ed4 * 0x27 + -0x27;
            *ppppppppppfVar22 = (float *********)local_1eb8;
            ppppppppppfVar22[1] = (float *********)local_1eb4;
            ppppppppppfVar22[2] = (float *********)local_1eb0;
            ppppppppppfVar22[6] = (float *********)local_1eac;
            ppppppppppfVar22[7] = (float *********)local_1ea8;
            ppppppppppfVar22[8] = (float *********)local_1ea4;
            func_004A17A4((float *)ppppppppppfVar22);
            local_1e94 = (float ***********)((int)*local_1ed4 + -1);
          }
          func_0049F5FC(pppppppppppfVar15 + 2);
          func_0049F5FC(pppppppppppfVar15 + 6);
          pppppppppfVar48 = (float *********)0x0;
          while( true ) {
            ppppppppppfVar22 = *pppppppppppfVar15;
            pppppppppfVar23 = *ppppppppppfVar22;
            if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
            if (pppppppppfVar48 < pppppppppfVar23) {
              ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
            }
            if (pppppppppfVar48 < pppppppppfVar23) {
              pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
            }
            else {
              pppppppppfVar23 = (float *********)0x0;
            }
            func_0010F1DC((int *)(pppppppppppfVar15 + 2),
                          (int *)((int)pppppppppppfVar15[5] + (int)pppppppppfVar23));
            pppppppppppfVar15[4] = (float **********)((int)pppppppppppfVar15[4] + 1);
            ppppppppppfVar22 = *pppppppppppfVar15;
            pppppppppfVar23 = *ppppppppppfVar22;
            if (pppppppppfVar48 < pppppppppfVar23) {
              ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
            }
            if (pppppppppfVar48 < pppppppppfVar23) {
              pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
            }
            else {
              pppppppppfVar23 = (float *********)0x0;
            }
            func_0010F1DC((int *)(pppppppppppfVar15 + 6),
                          (int *)((int)pppppppppppfVar15[9] + (int)pppppppppfVar23));
            pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
            pppppppppppfVar15[8] = (float **********)((int)pppppppppppfVar15[8] + 1);
          }
          func_0064B4A0((int)pppppppppppfVar15,pfVar39,(float *)&local_1ec8);
          uVar70 = ZEXT48(local_1f00[2]);
          ppppppppppfVar22 = *local_1ed4;
          if ((int)ppppppppppfVar22 < (int)local_1ed4[1]) {
            *local_1ed4 = (float **********)((int)ppppppppppfVar22 + 1);
            ppppppppppfVar22 = local_1ed4[2] + (int)ppppppppppfVar22 * 0x27;
          }
          else {
            ppppppppppfVar22 = (float **********)0x0;
          }
          local_1f28[2] = (float ***********)((ulonglong)uVar71 >> 0x20);
          uVar84 = CONCAT44(pppppppppppfVar83,local_1f28[2]);
          pppppppppfVar48 = (float *********)uVar71;
          ppppppppppfVar22[0x1c] = pppppppppfVar48;
          pppppppfVar51 = DAT_001f5fdc;
          local_1f28[3] = (float ***********)((ulonglong)uVar72 >> 0x20);
          uVar77 = CONCAT44(local_1f28[3],pppppppppfVar48);
          ppppppppppfVar22[9] = (float *********)pppppppppppfVar52[0xf];
          ppppppppppfVar22[10] = (float *********)pppppppppppfVar52[0x10];
          ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar52[0x11];
          *(undefined1 *)(ppppppppppfVar22 + 0x24) = 1;
          local_1ed0 = (float ***********)0x0;
          local_1ec8 = (float ***********)pppppppppppfVar52[0xb];
          uVar71 = CONCAT44(pppppppppppfVar75,ALBW_AS_F32(local_1f00[2]) + ALBW_AS_F32(pppppppppfVar48));
          local_1ec4 = (float ************)pppppppppppfVar52[0xf];
          local_1ec0 = (float ************)pppppppppppfVar52[0x10];
          local_1ebc = (float ***********)pppppppppppfVar52[0x11];
          local_1f08 = (float ***********)local_1ec8[0xc];
          local_1f04 = (float ***********)local_1ec8[0xd];
          local_1f00[0] = (float ***********)local_1ec8[0xe];
          local_1f14 = (float ***********)*pppppppfVar51;
          local_1f10 = (float ***********)pppppppfVar51[1];
          local_1f0c = (float ***********)pppppppfVar51[2];
          pppppppppppfVar15 = pppppppppppfVar52;
          local_1f28[4] = local_1f28[3];
          func_004A17E4((float *)(local_1f28 + 2));
          local_1eb8 = local_1f28[2];
          local_1eb4 = (float ************)local_1f28[3];
          local_1eb0 = local_1f28[4];
          ppppppppppfVar22 = pppppppppppfVar52[0xb];
          local_1eac = (float ************)ppppppppppfVar22[0xc];
          local_1ea8 = (float ***********)ppppppppppfVar22[0xd];
          local_1ea4 = (float ***********)ppppppppppfVar22[0xe];
          local_1ea0 = (float ***********)pppppppfVar51;
          local_1e9c = (float ***********)uVar71;
          local_1e98 = local_1e04;
          if (local_1ed4 != (float ***********)0x0) {
            ppppppppppfVar22 = local_1ed4[2] + (int)*local_1ed4 * 0x27 + -0x27;
            *ppppppppppfVar22 = (float *********)local_1f28[2];
            ppppppppppfVar22[1] = (float *********)local_1f28[3];
            ppppppppppfVar22[2] = (float *********)local_1f28[4];
            ppppppppppfVar22[6] = (float *********)local_1eac;
            ppppppppppfVar22[7] = (float *********)local_1ea8;
            ppppppppppfVar22[8] = (float *********)local_1ea4;
            func_004A17A4((float *)ppppppppppfVar22);
            local_1e94 = (float ***********)((int)*local_1ed4 + -1);
          }
          func_0049F5FC(pppppppppppfVar52 + 2);
          func_0049F5FC(pppppppppppfVar52 + 6);
          pppppppppfVar48 = (float *********)0x0;
          while( true ) {
            pppppppppppfVar79 = (float ***********)uVar72;
            fVar55 = ALBW_AS_F32(uVar70);
            fVar5 = ALBW_AS_F32(((ulonglong)uVar71 >> 0x20));
            ppppppppppfVar22 = *pppppppppppfVar52;
            pppppppppfVar23 = *ppppppppppfVar22;
            if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
            if (pppppppppfVar48 < pppppppppfVar23) {
              ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
            }
            if (pppppppppfVar48 < pppppppppfVar23) {
              pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
            }
            else {
              pppppppppfVar23 = (float *********)0x0;
            }
            func_0010F1DC((int *)(pppppppppppfVar52 + 2),
                          (int *)((int)pppppppppppfVar52[5] + (int)pppppppppfVar23));
            pppppppppppfVar52[4] = (float **********)((int)pppppppppppfVar52[4] + 1);
            ppppppppppfVar22 = *pppppppppppfVar52;
            pppppppppfVar23 = *ppppppppppfVar22;
            if (pppppppppfVar48 < pppppppppfVar23) {
              ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
            }
            if (pppppppppfVar48 < pppppppppfVar23) {
              pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
            }
            else {
              pppppppppfVar23 = (float *********)0x0;
            }
            func_0010F1DC((int *)(pppppppppppfVar52 + 6),
                          (int *)((int)pppppppppppfVar52[9] + (int)pppppppppfVar23));
            pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
            pppppppppppfVar52[8] = (float **********)((int)pppppppppppfVar52[8] + 1);
          }
          func_00649E18((int)pppppppppppfVar52,pfVar39,(float *)&local_1ec8);
          uVar72 = CONCAT44(fVar5,local_1f00[2]);
          local_1de8 = (float ***********)0x0;
          fVar5 = fVar5 + ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
          pppppppppppfVar52 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(uVar77) + ALBW_AS_F32(uVar67)));
          pppppppppppfVar74 = pppppppppppfVar15 + 0x29;
          pppppppppppfVar34 = pppppppppppfVar15;
          if (((*DAT_001f6c00 & 1) == 0) &&
             (iVar21 = func_00100174((int *)DAT_001f6c00), puVar4 = DAT_001f6c04, iVar21 != 0)) {
            puVar26 = DAT_001f6c04 + 3;
            *DAT_001f6c04 = 0;
            puVar4[1] = 0;
            puVar4[2] = 0;
            func_0010B3EC(puVar4,0x20,(int)puVar26);
          }
          fVar57 = DAT_001f6c08;
          puVar4 = DAT_001f6c04;
          pppppppppppfVar81 = (float ***********)uVar84;
          pppppppppppfVar83 = (float ***********)((ulonglong)uVar84 >> 0x20);
          *DAT_001f6c04 = 0;
          ppppppppppfVar22 = (float **********)0x0;
          fVar60 = ALBW_AS_F32(pppppppppppfVar81) / (ALBW_AS_F32(((ulonglong)uVar67 >> 0x20)) - fVar5);
          local_1f14 = (float ***********)ALBW_PTR_BITS((fVar60 * fVar57));
          local_1f28[3] = (float ***********)ALBW_PTR_BITS((fVar57 + fVar5 * fVar60 * ALBW_AS_F32(pppppppppppfVar49)));
          fVar5 = ALBW_AS_F32(pppppppppppfVar81) / (ALBW_AS_F32(uVar67) - ALBW_AS_F32(pppppppppppfVar52));
          local_1f28[4] = (float ***********)ALBW_PTR_BITS((fVar5 * ALBW_AS_F32(pppppppppppfVar49)));
          local_1f28[2] = (float ***********)ALBW_PTR_BITS((fVar57 + ALBW_AS_F32(pppppppppppfVar52) * fVar5 * fVar57));
          pppppppppppfVar53 = pppppppppppfVar49;
          if (0 < (int)*pppppppppppfVar74) {
            do {
              if (ppppppppppfVar22 < *pppppppppppfVar74) {
                ppppppppppfVar56 = pppppppppppfVar15[0x2b] + (int)ppppppppppfVar22 * 0x27;
              }
              else {
                ppppppppppfVar56 = pppppppppppfVar15[0x2b];
              }
              ppppppppppfVar56[0x1c] =
                   (float *********)
                   ALBW_PTR_BITS((ALBW_AS_F32(local_1f28[*(char *)(ppppppppppfVar56 + 0x24) + 2]) +
                   ALBW_AS_F32(local_1f28[(int)*(char *)(ppppppppppfVar56 + 0x24) + 4]) *
                   ALBW_AS_F32(ppppppppppfVar56[0x1c])));
              pppppppppfVar48 =
                   (float *********)
                   ALBW_PTR_BITS((ALBW_AS_F32(local_1f28[*(char *)(ppppppppppfVar56 + 0x24) + 2]) +
                   ALBW_AS_F32(local_1f28[(int)*(char *)(ppppppppppfVar56 + 0x24) + 4]) *
                   ALBW_AS_F32(ppppppppppfVar56[0x1d])));
              ppppppppppfVar56[0x1d] = pppppppppfVar48;
              if (ALBW_AS_F32(pppppppppfVar48) < ALBW_AS_F32(ppppppppppfVar56[0x1c])) {
                pppppppppfVar48 = ppppppppppfVar56[9];
                pppppppppfVar23 = ppppppppppfVar56[10];
                pppppppppfVar24 = ppppppppppfVar56[0xb];
                ppppppppppfVar56[9] = ppppppppppfVar56[0xc];
                ppppppppppfVar56[10] = ppppppppppfVar56[0xd];
                ppppppppppfVar56[0xb] = ppppppppppfVar56[0xe];
                ppppppppppfVar56[0xc] = pppppppppfVar48;
                ppppppppppfVar56[0xd] = pppppppppfVar23;
                ppppppppppfVar56[0xe] = pppppppppfVar24;
                pppppppppfVar48 = ppppppppppfVar56[0x1c];
                ppppppppppfVar56[0x1c] = ppppppppppfVar56[0x1d];
                ppppppppppfVar56[0x1d] = pppppppppfVar48;
              }
              ppppppppppfVar56[0xf] = ppppppppppfVar56[9];
              ppppppppppfVar56[0x10] = ppppppppppfVar56[10];
              ppppppppppfVar56[0x11] = ppppppppppfVar56[0xb];
              ppppppppppfVar56[0x12] = ppppppppppfVar56[0xc];
              ppppppppppfVar56[0x13] = ppppppppppfVar56[0xd];
              ppppppppppfVar56[0x14] = ppppppppppfVar56[0xe];
              *(undefined1 *)((int)ppppppppppfVar56 + 0x92) = 0;
              *(undefined1 *)((int)ppppppppppfVar56 + 0x91) = 0;
              ppppppppppfVar56[0x1b] = (float *********)((ulonglong)uVar77 >> 0x20);
              *(undefined1 *)(ppppppppppfVar56 + 0x26) = 0;
              *(undefined1 *)((int)ppppppppppfVar56 + 0x97) = 0;
              *(undefined1 *)((int)ppppppppppfVar56 + 0x95) = 0;
              *(undefined1 *)(ppppppppppfVar56 + 0x25) = 0;
              if (*(char *)(ppppppppppfVar56 + 0x24) == '\0') {
                func_00115D70((float *)ppppppppppfVar56,(float *)ppppppppppfVar56,fVar58);
                func_00115D70((float *)(ppppppppppfVar56 + 3),(float *)(ppppppppppfVar56 + 3),fVar58
                             );
              }
              ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
            } while ((int)ppppppppppfVar22 < (int)*pppppppppppfVar74);
          }
          ppppppppppfVar22 = (float **********)0x0;
          if (0 < (int)*pppppppppppfVar74) {
            do {
              puVar26 = DAT_001f6c04;
              ppppppppppfVar73 = *pppppppppppfVar74;
              ppppppppppfVar56 = pppppppppppfVar15[0x2b];
              if (ppppppppppfVar22 < ppppppppppfVar73) {
                ppppppppppfVar56 = ppppppppppfVar56 + (int)ppppppppppfVar22 * 0x27;
              }
              ppppppppppfVar30 = pppppppppppfVar15[0x2b];
              if (ppppppppppfVar22 < ppppppppppfVar73) {
                ppppppppppfVar30 = ppppppppppfVar30 + (int)ppppppppppfVar22 * 0x27;
              }
              if (WALK_SEG_HAS_LENGTH(ppppppppppfVar56, ppppppppppfVar30)) {
                if (ppppppppppfVar22 < ppppppppppfVar73) {
                  ppppppppppfVar56 = pppppppppppfVar15[0x2b] + (int)ppppppppppfVar22 * 0x27;
                }
                else {
                  ppppppppppfVar56 = pppppppppppfVar15[0x2b];
                }
                if ((int)*DAT_001f6c04 < (int)DAT_001f6c04[1]) {
                  *(float ***********)(DAT_001f6c04[2] + *DAT_001f6c04 * 4) = ppppppppppfVar56;
                  *puVar26 = *puVar26 + 1;
                }
              }
              ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
            } while ((int)ppppppppppfVar22 < (int)*pppppppppppfVar74);
          }
          *pppppppppppfVar74 = (float **********)0x0;
          if (*puVar4 != 0) {
            func_0048C258((int *)DAT_001f6c04,DAT_001f6c0c);
            if (((*DAT_001f6c10 & 1) == 0) &&
               (iVar21 = func_00100174((int *)DAT_001f6c10), puVar26 = DAT_001f6c14, iVar21 != 0)) {
              puVar27 = DAT_001f6c14 + 3;
              *DAT_001f6c14 = 0;
              puVar26[1] = 0x30;
              puVar26[2] = (uint)puVar27;
              func_00100118((int)(puVar26 + 3),DAT_001f5fe8,0x9c,0x30);
            }
            puVar26 = DAT_001f6c14;
            *DAT_001f6c14 = 0;
            if (*puVar4 == 0) {
              puVar18 = (undefined4 *)0x0;
            }
            else {
              puVar18 = *(undefined4 **)puVar4[2];
            }
            if (0 < (int)puVar26[1]) {
              *puVar26 = 1;
              func_001FE514((undefined4 *)puVar26[2],puVar18);
            }
            iVar21 = *puVar26 * 0x9c + -0x9c + puVar26[2];
            uVar36 = 1;
            if (1 < (int)*puVar4) {
              do {
                uVar43 = *puVar4;
                uVar16 = uVar43;
                if (uVar36 < uVar43) {
                  uVar16 = puVar4[2];
                }
                if (uVar36 < uVar43) {
                  puVar18 = *(undefined4 **)(uVar16 + uVar36 * 4);
                }
                else {
                  puVar18 = (undefined4 *)0x0;
                }
                fVar5 = *(float *)(iVar21 + 0x18) * ALBW_AS_F32(puVar18[6]) +
                        *(float *)(iVar21 + 0x1c) * ALBW_AS_F32(puVar18[7]) +
                        *(float *)(iVar21 + 0x20) * ALBW_AS_F32(puVar18[8]);
                if (fVar5 <= ALBW_AS_F32(((ulonglong)uVar77 >> 0x20))) {
                  fVar5 = -fVar5;
                }
                if (DAT_001f6c18 < (int)ALBW_AS_U32(fVar5)) {
                  if (*(float *)(iVar21 + 0x74) < ALBW_AS_F32(puVar18[0x1d])) {
                    *(undefined4 *)(iVar21 + 0x30) = puVar18[0xc];
                    *(undefined4 *)(iVar21 + 0x34) = puVar18[0xd];
                    *(undefined4 *)(iVar21 + 0x38) = puVar18[0xe];
                    *(undefined4 *)(iVar21 + 0x74) = puVar18[0x1d];
                    *(undefined4 *)(iVar21 + 0x48) = puVar18[0x12];
                    *(undefined4 *)(iVar21 + 0x4c) = puVar18[0x13];
                    *(undefined4 *)(iVar21 + 0x50) = puVar18[0x14];
                  }
                }
                else {
                  uVar16 = *puVar26;
                  if ((int)uVar16 < (int)puVar26[1]) {
                    *puVar26 = uVar16 + 1;
                    puVar7 = (undefined4 *)(puVar26[2] + uVar16 * 0x9c);
                    local_1f00[1] = (float ***********)(puVar18 + 0xc);
                    *puVar7 = *puVar18;
                    puVar7[1] = puVar18[1];
                    puVar7[2] = puVar18[2];
                    puVar7[3] = puVar18[3];
                    puVar7[4] = puVar18[4];
                    local_1eb8 = (float ***********)(puVar18 + 0x12);
                    puVar7[5] = puVar18[5];
                    puVar7[6] = puVar18[6];
                    puVar7[7] = puVar18[7];
                    local_1ebc = (float ***********)(puVar18 + 0x15);
                    puVar7[8] = puVar18[8];
                    puVar7[9] = puVar18[9];
                    puVar7[10] = puVar18[10];
                    local_1f08 = (float ***********)(puVar18 + 0x18);
                    puVar7[0xb] = puVar18[0xb];
                    puVar7[0xc] = *local_1f00[1];
                    puVar7[0xd] = puVar18[0xd];
                    puVar7[0xe] = puVar18[0xe];
                    puVar7[0xf] = puVar18[0xf];
                    puVar7[0x10] = puVar18[0x10];
                    puVar7[0x11] = puVar18[0x11];
                    puVar7[0x12] = (float **********)puVar18[0x12];
                    puVar7[0x13] = puVar18[0x13];
                    puVar7[0x14] = puVar18[0x14];
                    puVar7[0x15] = (float **********)puVar18[0x15];
                    puVar7[0x16] = puVar18[0x16];
                    puVar7[0x17] = puVar18[0x17];
                    puVar7[0x18] = (float **********)puVar18[0x18];
                    puVar7[0x19] = puVar18[0x19];
                    puVar7[0x1a] = puVar18[0x1a];
                    puVar7[0x1b] = puVar18[0x1b];
                    puVar7[0x1c] = puVar18[0x1c];
                    puVar7[0x1d] = puVar18[0x1d];
                    puVar7[0x1e] = puVar18[0x1e];
                    puVar7[0x1f] = puVar18[0x1f];
                    puVar7[0x20] = puVar18[0x20];
                    puVar7[0x21] = puVar18[0x21];
                    puVar7[0x22] = puVar18[0x22];
                    puVar7[0x23] = puVar18[0x23];
                    *(undefined1 *)(puVar7 + 0x24) = *(undefined1 *)(puVar18 + 0x24);
                    *(undefined1 *)((int)puVar7 + 0x91) = *(undefined1 *)((int)puVar18 + 0x91);
                    *(undefined1 *)((int)puVar7 + 0x92) = *(undefined1 *)((int)puVar18 + 0x92);
                    *(undefined1 *)((int)puVar7 + 0x93) = *(undefined1 *)((int)puVar18 + 0x93);
                    *(undefined1 *)(puVar7 + 0x25) = *(undefined1 *)(puVar18 + 0x25);
                    *(undefined1 *)((int)puVar7 + 0x95) = *(undefined1 *)((int)puVar18 + 0x95);
                    *(undefined1 *)((int)puVar7 + 0x96) = *(undefined1 *)((int)puVar18 + 0x96);
                    *(undefined1 *)((int)puVar7 + 0x97) = *(undefined1 *)((int)puVar18 + 0x97);
                    *(undefined1 *)(puVar7 + 0x26) = *(undefined1 *)(puVar18 + 0x26);
                  }
                  iVar21 = *puVar26 * 0x9c + -0x9c + puVar26[2];
                }
                uVar36 = uVar36 + 1;
              } while ((int)uVar36 < (int)*puVar4);
            }
            uVar16 = *puVar26;
            local_1f28[1] = (float ***********)0x0;
            uVar36 = 1;
            if (1 < (int)uVar16) {
              pfVar39 = (float *)puVar26[2];
              fVar5 = ABS(DAT_001f7eb4);
              do {
                pfVar8 = pfVar39;
                if (uVar36 - 1 < uVar16) {
                  pfVar8 = pfVar39 + (uVar36 - 1) * 0x27;
                }
                uVar43 = uVar36;
                if (uVar36 < uVar16) {
                  uVar43 = uVar36 * 0x27;
                }
                fVar60 = pfVar8[7];
                pfVar32 = pfVar39;
                if (uVar36 < uVar16) {
                  pfVar32 = pfVar39 + uVar43;
                }
                iVar21 = (int)ALBW_AS_U32(fVar60) - (int)ALBW_AS_U32(fVar5);
                if ((int)ALBW_AS_U32(fVar60) <= (int)ALBW_AS_U32(fVar5)) {
                  fVar60 = pfVar32[7];
                  iVar21 = (int)ALBW_AS_U32(fVar60) - (int)ALBW_AS_U32(fVar5);
                }
                if (fVar60 == fVar5 || iVar21 < 0 != SBORROW4((int)ALBW_AS_U32(fVar60),(int)ALBW_AS_U32(fVar5))) break;
                if ((uint)ALBW_AS_U32(DAT_001f7eb4) <
                    (uint)ALBW_AS_U32((*pfVar8 * pfVar32[6] + pfVar8[1] * pfVar32[7] + pfVar8[2] * pfVar32[8])))
                goto LAB_001f6f60;
                uVar36 = uVar36 + 1;
              } while ((int)uVar36 < (int)uVar16);
            }
            if (((int)uVar36 < (int)uVar16) && (uVar43 = uVar16 - 1, (int)uVar36 <= (int)uVar43)) {
              pfVar39 = (float *)puVar26[2];
              do {
                pfVar8 = pfVar39;
                if (uVar43 - 1 < uVar16) {
                  pfVar8 = pfVar39 + (uVar43 - 1) * 0x27;
                }
                uVar31 = uVar43;
                if (uVar43 < uVar16) {
                  uVar31 = uVar43 * 0x27;
                }
                fVar5 = pfVar8[7];
                pfVar32 = pfVar39;
                if (uVar43 < uVar16) {
                  pfVar32 = pfVar39 + uVar31;
                }
                iVar21 = (int)ALBW_AS_U32(fVar5) - (int)ALBW_AS_U32(DAT_001f6730);
                if ((int)ALBW_AS_U32(fVar5) <= (int)ALBW_AS_U32(DAT_001f6730)) {
                  fVar5 = pfVar32[7];
                  iVar21 = (int)ALBW_AS_U32(fVar5) - (int)ALBW_AS_U32(DAT_001f6730);
                }
                if (fVar5 == DAT_001f6730 || iVar21 < 0 != SBORROW4((int)ALBW_AS_U32(fVar5),(int)ALBW_AS_U32(DAT_001f6730)))
                break;
                if (DAT_001f7eb8 <
                    (float *******)
                    ALBW_PTR_BITS((*pfVar8 * pfVar32[6] + pfVar8[1] * pfVar32[7] + pfVar8[2] * pfVar32[8])))
                goto LAB_001f6f60;
                uVar43 = uVar43 - 1;
              } while ((int)uVar36 <= (int)uVar43);
            }
            goto LAB_001f7014;
          }
          goto LAB_001f72a4;
        }
        if ((ALBW_AS_F32(pppppppppppfVar15[0x1e]) == ALBW_AS_F32(pppppppppppfVar50)) ||
           (pppppppppppfVar34 = local_1dd8[0], (int)local_1dd8[0] <= (int)DAT_001f5fe4)) {
          uVar72 = CONCAT44(local_1dd8[0],pppppppppfVar48);
          pppppppppppfVar49 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(local_1dd8[0]));
          func_001FE680(&local_1ec8);
          local_1eac = &local_1e4c;
          if (ALBW_AS_F32(pppppppppppfVar49) <= ALBW_AS_F32(uVar67)) {
            local_1e6c = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar49));
            uVar70 = ZEXT48(local_1e6c);
            pppppppppppfVar15[0x1d] = (float **********)ALBW_PTR_BITS(-(fVar55 * ALBW_AS_F32(pppppppppppfVar49)));
            local_1e98 = (float ***********)pppppppppppfVar15[10];
            local_1e94 = (float ***********)pppppppppppfVar15[0xc];
            local_1e90 = (float ***********)pppppppppppfVar15[0xd];
            local_1e8c = (float ***********)pppppppppppfVar15[0xe];
            local_1e88 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar15[0x12]));
            local_1e80 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar15[0x14]));
            local_1e84 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar15[0x13]));
            ppppppppppfVar56 = pppppppppppfVar15[10];
            local_1e7c = (float ***********)ppppppppppfVar56[0xc];
            local_1e78 = (float ***********)ppppppppppfVar56[0xd];
            local_1e74 = (float ***********)ppppppppppfVar56[0xe];
            local_1e68 = local_1e04;
            local_1e70 = (float ***********)pppppppfVar51;
            local_1ea0 = (float ***********)0x0;
            local_1ea4 = (float ***********)0x0;
            local_1f28[3] = pppppppppppfVar15;
            local_1f28[4] = (float ***********)&local_1ec8;
            func_0049F5FC(pppppppppppfVar15 + 2);
            func_0049F5FC(pppppppppppfVar15 + 6);
            pppppppppfVar48 = (float *********)0x0;
            while( true ) {
              fVar5 = ALBW_AS_F32(uVar70);
              ppppppppppfVar56 = *pppppppppppfVar15;
              pppppppppfVar23 = *ppppppppppfVar56;
              if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
              if (pppppppppfVar48 < pppppppppfVar23) {
                ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
              }
              if (pppppppppfVar48 < pppppppppfVar23) {
                pppppppppfVar23 = ppppppppppfVar56[(int)pppppppppfVar48];
              }
              else {
                pppppppppfVar23 = (float *********)0x0;
              }
              func_0010F1DC((int *)(pppppppppppfVar15 + 2),
                            (int *)((int)pppppppppppfVar15[5] + (int)pppppppppfVar23));
              pppppppppppfVar15[4] = (float **********)((int)pppppppppppfVar15[4] + 1);
              ppppppppppfVar56 = *pppppppppppfVar15;
              pppppppppfVar23 = *ppppppppppfVar56;
              if (pppppppppfVar48 < pppppppppfVar23) {
                ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
              }
              if (pppppppppfVar48 < pppppppppfVar23) {
                pppppppppfVar23 = ppppppppppfVar56[(int)pppppppppfVar48];
              }
              else {
                pppppppppfVar23 = (float *********)0x0;
              }
              func_0010F1DC((int *)(pppppppppppfVar15 + 6),
                            (int *)((int)pppppppppppfVar15[9] + (int)pppppppppfVar23));
              pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
              pppppppppppfVar15[8] = (float **********)((int)pppppppppppfVar15[8] + 1);
            }
            func_0064E9F4((int *)local_1f28[3],local_1f28[4],(int *)&local_1e98);
            ppppppppppfVar56 =
                 (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar15[0x1e]) - (fVar5 - ALBW_AS_F32(local_1ec8))));
          }
          else {
            pppppppppppfVar15[0x1d] = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar49) * fVar55));
            local_1e98 = (float ***********)pppppppppppfVar15[10];
            local_1e94 = (float ***********)pppppppppppfVar15[0xc];
            local_1e90 = (float ***********)pppppppppppfVar15[0xd];
            local_1e8c = (float ***********)pppppppppppfVar15[0xe];
            local_1e88 = (float ***********)pppppppppppfVar15[0x12];
            local_1e84 = (float ***********)pppppppppppfVar15[0x13];
            local_1e80 = (float ***********)pppppppppppfVar15[0x14];
            ppppppppppfVar56 = pppppppppppfVar15[10];
            local_1e7c = (float ***********)ppppppppppfVar56[0xc];
            local_1e78 = (float ***********)ppppppppppfVar56[0xd];
            local_1e74 = (float ***********)ppppppppppfVar56[0xe];
            local_1e68 = local_1e04;
            local_1e70 = (float ***********)pppppppfVar51;
            local_1ea0 = (float ***********)0x0;
            local_1ea4 = (float ***********)0x0;
            local_1f28[1] = (float ***********)&local_1ec8;
            local_1f28[2] = pppppppppppfVar15;
            local_1e6c = pppppppppppfVar49;
            func_0049F5FC(pppppppppppfVar15 + 2);
            func_0049F5FC(pppppppppppfVar15 + 6);
            pppppppppfVar48 = (float *********)0x0;
            while( true ) {
              ppppppppppfVar56 = *pppppppppppfVar15;
              pppppppppfVar23 = *ppppppppppfVar56;
              if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
              if (pppppppppfVar48 < pppppppppfVar23) {
                ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
              }
              if (pppppppppfVar48 < pppppppppfVar23) {
                pppppppppfVar23 = ppppppppppfVar56[(int)pppppppppfVar48];
              }
              else {
                pppppppppfVar23 = (float *********)0x0;
              }
              func_0010F1DC((int *)(pppppppppppfVar15 + 2),
                            (int *)((int)pppppppppppfVar15[5] + (int)pppppppppfVar23));
              pppppppppppfVar15[4] = (float **********)((int)pppppppppppfVar15[4] + 1);
              ppppppppppfVar56 = *pppppppppppfVar15;
              pppppppppfVar23 = *ppppppppppfVar56;
              if (pppppppppfVar48 < pppppppppfVar23) {
                ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
              }
              if (pppppppppfVar48 < pppppppppfVar23) {
                pppppppppfVar23 = ppppppppppfVar56[(int)pppppppppfVar48];
              }
              else {
                pppppppppfVar23 = (float *********)0x0;
              }
              func_0010F1DC((int *)(pppppppppppfVar15 + 6),
                            (int *)((int)pppppppppppfVar15[9] + (int)pppppppppfVar23));
              pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
              pppppppppppfVar15[8] = (float **********)((int)pppppppppppfVar15[8] + 1);
            }
            func_0064CB28((int *)local_1f28[2],(int *)local_1f28[1],(int *)&local_1e98);
            ppppppppppfVar56 =
                 (float **********)
                 ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar15[0x1e]) + (ALBW_AS_F32(pppppppppppfVar49) - ALBW_AS_F32(local_1ec8))));
          }
          pppppppppppfVar15[0x1e] = ppppppppppfVar56;
          if (local_1ea8 != (float ***********)0x0) {
            *(float ***********)(DAT_001f5fec + (int)pppppppppppfVar15) = *local_1ea8;
            pppppppppppfVar15[0xc] = (float **********)local_1ec4;
            pppppppppppfVar15[0xd] = (float **********)local_1ec0;
            pppppppppppfVar15[0xe] = (float **********)local_1ebc;
            if ((float ***********)pppppppppppfVar15[10] != local_1ea8) {
              pppppppppppfVar15[10] = (float **********)local_1ea8;
              pppppppppppfVar15[0x18] = local_1ea8[0xc];
              pppppppppppfVar15[0x19] = local_1ea8[0xd];
              pppppppppppfVar15[0x1a] = local_1ea8[0xe];
              pppppppppppfVar15[0x15] = (float **********)*pppppppfVar51;
              pppppppppppfVar15[0x16] = (float **********)pppppppfVar51[1];
              pppppppppppfVar15[0x17] = (float **********)pppppppfVar51[2];
              func_004A17E4((float *)(pppppppppppfVar15 + 0x12));
            }
            pppppppppppfVar15[0xf] = (float **********)local_1e4c;
            pppppppppppfVar15[0x10] = (float **********)local_1e48;
            pppppppppppfVar15[0x11] = (float **********)local_1e44;
          }
          fVar5 = DAT_001f6730;
          local_1dd8[0] = local_1ec8;
          if (ppppppppppfVar22 < *local_1db4) {
            ppppppppppfVar22 = local_1db4[2] + (int)ppppppppppfVar22 * 0x27;
          }
          else {
            ppppppppppfVar22 = (float **********)0x0;
          }
          ppppppppppfVar22[9] = (float *********)pppppppppppfVar17[0xc];
          ppppppppppfVar22[10] = (float *********)pppppppppppfVar17[0xd];
          ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar17[0xe];
          ppppppppppfVar22[0x1c] =
               (float *********)ALBW_PTR_BITS((ALBW_AS_F32(uVar72) + ALBW_AS_F32(((ulonglong)uVar72 >> 0x20))));
          pppppppppppfVar34 = local_1ec8;
          if ((int)local_1ec8 <= (int)ALBW_AS_U32(fVar5)) {
            /* WALKER: reset the painting strip: clear the "skip segment rebuild" flag (+0x3DC0) and the
             * segment count. The ARM (EU 1EEA3C) stores the same zero register to both; the converted
             * text stored 1 here, so the strip was never rebuilt and Link's wall painting stayed empty. */
            *(char *)(pppppppppppfVar17 + 0xf70) = '\0';
            pppppppppppfVar17[0x29] = (float **********)0x0;
            pppppppppppfVar17[0x77c] = (float **********)0x0;
            pppppppppppfVar34 =
                 (float ***********)ALBW_PTR_BITS(func_0063AEA8((float ***********)param_1,1,(int)(uintptr_t)local_1df8,
                     param_2[0x15],param_2[0x14]));   /* EU 001F61C8..001F61FC: this branch's flag [sp,#0x138]; s0 = [0x15], s1 = [0x14] (BOOT_LOG 166) */
          }
        }
      }
LAB_001f6204:
      if ((int)pppppppppppfVar34 <= (int)ALBW_AS_U32(DAT_001f6730)) goto LAB_001fc7a8;
    }
LAB_001f6210:
    local_30[0xed0] = (float **********)local_1e10;
    local_30[0xed1] = local_1de4;
    local_30[0xed2] = (float **********)local_1e0c;
    *local_1e58 = local_1e24;
    local_1e58[1] = local_1df4;
    local_1e58[2] = local_1e20;
    *local_1de0 = (float **********)local_1df0;
    local_1de0[1] = (float **********)local_1dec;
    local_1de0[2] = (float **********)local_1e1c;
    local_1e58[6] = local_1e18;
    local_1e58[7] = (float ***********)local_1e14;
    local_1e58[8] = local_1de8;
    if (local_1dfc == (float ***********)0x0) {
      bVar20 = 0;
    }
    else {
      bVar20 = 2;
    }
    /* WALKER: step 5: MODE = 2 (leaving the wall) when we were merged, else 0 */
    *(byte *)local_1e28 = bVar20;
    ppppppppppfVar22 = (float **********)0x0;
    if ((float *)ALBW_PTR_BITS(local_2c[0x27]) != (float *)0x0) {
      ppppppppppfVar22 = *(float ***********)ALBW_PTR_BITS(local_2c[0x27]);
    }
    local_1e50[0x29] = ppppppppppfVar22;
    if ((float *)ALBW_PTR_BITS(local_2c[0x28]) == (float *)0x0) {
      ppppppppppfVar22 = (float **********)0xffffffff;
    }
    else {
      ppppppppppfVar22 = *(float ***********)ALBW_PTR_BITS(local_2c[0x28]);
    }
    *local_1ddc = ppppppppppfVar22;
    goto LAB_001fe470;
  }
  pppppppppppfVar75 = (float ***********)ALBW_PTR_BITS(local_2c[0x14]);
  pppppppppfVar48 = (float *********)ALBW_PTR_BITS(local_2c[0x15]);
  pppppppppppfVar9 = pppppppppppfVar15;
  pppppppppppfVar17 = local_30;
  local_1e48 = (float ***********)(uint)(local_2c[0x1a] != 0.0);
  func_001FE680(&local_1e88);
  ppppppppppfVar22 = *pppppppppppfVar78;
  if ((int)ppppppppppfVar22 < (int)pppppppppppfVar34[0x2a]) {
    ppppppppppfVar56 = pppppppppppfVar34[0x2b];
    *pppppppppppfVar78 = (float **********)((int)ppppppppppfVar22 + 1);
    ppppppppppfVar56 = ppppppppppfVar56 + (int)ppppppppppfVar22 * 0x27;
  }
  else {
    ppppppppppfVar56 = (float **********)0x0;
  }
  ppppppppppfVar56[0x1c] = (float *********)pppppppppppfVar75;
  ppppppppppfVar56[9] = (float *********)pppppppppppfVar17[0xc];
  ppppppppppfVar56[10] = (float *********)pppppppppppfVar17[0xd];
  ppppppppppfVar56[0xb] = (float *********)pppppppppppfVar17[0xe];
  *(undefined1 *)(ppppppppppfVar56 + 0x24) = 1;
  pppppppppppfVar17[0x1d] = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) * fVar55));
  local_1e60 = (float ***********)0x0;
  local_1ec8 = (float ***********)pppppppppppfVar17[10];
  local_1ec4 = (float ************)pppppppppppfVar17[0xc];
  local_1ec0 = (float ************)pppppppppppfVar17[0xd];
  local_1ebc = (float ***********)pppppppppppfVar17[0xe];
  local_1eb8 = (float ***********)pppppppppppfVar17[0x12];
  local_1eb4 = (float ************)pppppppppppfVar17[0x13];
  local_1eb0 = (float ***********)pppppppppppfVar17[0x14];
  ppppppppppfVar22 = pppppppppppfVar17[10];
  local_1eac = (float ************)ppppppppppfVar22[0xc];
  local_1ea8 = (float ***********)ppppppppppfVar22[0xd];
  local_1ea4 = (float ***********)ppppppppppfVar22[0xe];
  local_1ea0 = (float ***********)DAT_001ed5b8;
  pppppppppppfVar33 = pppppppppppfVar17;
  local_1e9c = pppppppppppfVar75;
  local_1e98 = pppppppppppfVar50;
  local_1e64 = pppppppppppfVar78;
  local_1e08 = pppppppppppfVar78;
  local_1dd8[0] = pppppppppppfVar50;
  if (pppppppppppfVar78 != (float ***********)0x0) {
    ppppppppppfVar22 = pppppppppppfVar34[0x2b] + (int)*pppppppppppfVar78 * 0x27 + -0x27;
    *ppppppppppfVar22 = (float *********)local_1eb8;
    ppppppppppfVar22[1] = (float *********)local_1eb4;
    ppppppppppfVar22[2] = (float *********)local_1eb0;
    ppppppppppfVar22[6] = (float *********)local_1eac;
    ppppppppppfVar22[7] = (float *********)local_1ea8;
    ppppppppppfVar22[8] = (float *********)local_1ea4;
    func_004A17A4((float *)ppppppppppfVar22);
    local_1e94 = (float ***********)((int)*local_1e64 + -1);
  }
  func_0049F5FC(pppppppppppfVar17 + 2);
  func_0049F5FC(pppppppppppfVar17 + 6);
  pppppppppfVar23 = (float *********)0x0;
  while( true ) {
    ppppppppppfVar22 = *pppppppppppfVar17;
    pppppppppfVar24 = *ppppppppppfVar22;
    if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
    if (pppppppppfVar23 < pppppppppfVar24) {
      ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
    }
    if (pppppppppfVar23 < pppppppppfVar24) {
      pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
    }
    else {
      pppppppppfVar24 = (float *********)0x0;
    }
    func_0010F1DC((int *)(pppppppppppfVar17 + 2),
                  (int *)((int)pppppppppppfVar17[5] + (int)pppppppppfVar24));
    pppppppppppfVar17[4] = (float **********)((int)pppppppppppfVar17[4] + 1);
    ppppppppppfVar22 = *pppppppppppfVar17;
    pppppppppfVar24 = *ppppppppppfVar22;
    if (pppppppppfVar23 < pppppppppfVar24) {
      ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
    }
    if (pppppppppfVar23 < pppppppppfVar24) {
      pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
    }
    else {
      pppppppppfVar24 = (float *********)0x0;
    }
    func_0010F1DC((int *)(pppppppppppfVar17 + 6),
                  (int *)((int)pppppppppppfVar17[9] + (int)pppppppppfVar24));
    pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
    pppppppppppfVar17[8] = (float **********)((int)pppppppppppfVar17[8] + 1);
  }
  func_0064CB28((int *)pppppppppppfVar17,(int *)&local_1e88,(int *)&local_1ec8);
  if (local_1e68 == (float ***********)0x0) goto LAB_001f6210;
  pppppppppppfVar33[0xedc] = (float **********)local_1e84;
  pppppppppppfVar33[0xedd] = (float **********)local_1e80;
  pppppppppppfVar33[0xede] = (float **********)local_1e7c;
  pppppppppppfVar33[0xee2] = local_1e68[0xc];
  pppppppppppfVar33[0xee3] = local_1e68[0xd];
  pppppppppppfVar33[0xee4] = local_1e68[0xe];
  pppppppppppfVar50 = pppppppppppfVar15;
  fVar58 = fVar5;
  if (DAT_001edb14 < (int)local_1e88) {
    ppppppppppfVar22 = pppppppppppfVar33[0x1e];
    bVar44 = ALBW_AS_F32(ppppppppppfVar22) < ALBW_AS_F32(pppppppppppfVar15);
    bVar47 = NAN(ALBW_AS_F32(ppppppppppfVar22)) || NAN(ALBW_AS_F32(pppppppppppfVar15));
    if (ALBW_AS_F32(ppppppppppfVar22) != ALBW_AS_F32(pppppppppppfVar15)) {
      bVar47 = SBORROW4((int)local_1e88,(int)DAT_001edb18);
      bVar44 = (int)local_1e88 - (int)DAT_001edb18 < 0;
    }
    pppppppppppfVar34 = local_1e88;
    pppppppppppfVar17 = pppppppppppfVar33;
    if ((ALBW_AS_F32(ppppppppppfVar22) == ALBW_AS_F32(pppppppppppfVar15) ||
        local_1e88 == (float ***********)DAT_001edb18) || bVar44 != bVar47) {
      local_1dc4 = (float ***********)&local_1dc0;
      local_1dc8 = (float ***********)&DAT_00000030;
      local_1dcc = (float **********)0x0;
      pppppppppppfVar9 = local_1e88;
      func_00100118((int)local_1dc4,DAT_001edb1c,0x9c,0x30);
      if ((int)local_1dcc < (int)local_1dc8) {
        ppppppppppppfVar85 = (float ************)(local_1dc4 + (int)local_1dcc * 0x27);
        local_1dcc = (float **********)((int)local_1dcc + 1);
      }
      else {
        ppppppppppppfVar85 = (float ************)0x0;
      }
      ppppppppppppfVar85[0x1c] = pppppppppppfVar9;
      ppppppppppppfVar85[9] = (float ***********)pppppppppppfVar33[0xc];
      ppppppppppppfVar85[10] = (float ***********)pppppppppppfVar33[0xd];
      ppppppppppppfVar85[0xb] = (float ***********)pppppppppppfVar33[0xe];
      *(undefined1 *)(ppppppppppppfVar85 + 0x24) = 0;
      pppppppppppfVar78 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(local_1e88));
      pppppppppppfVar34 = pppppppppppfVar33;
      func_001FE680(local_1f28);
      local_1f0c = (float ***********)&local_1e38;
      local_1f04 = &local_1dcc;
      if (ALBW_AS_F32(pppppppppppfVar78) <= ALBW_AS_F32(pppppppppppfVar15)) {
        pppppppppppfVar17 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar78));
        pppppppppppfVar33[0x1d] = (float **********)ALBW_PTR_BITS(-(fVar55 * ALBW_AS_F32(pppppppppppfVar78)));
        local_1f00[0] = (float ***********)0x0;
        local_1ec8 = (float ***********)pppppppppppfVar33[10];
        local_1ec4 = (float ************)pppppppppppfVar33[0xc];
        local_1ec0 = (float ************)pppppppppppfVar33[0xd];
        local_1ebc = (float ***********)pppppppppppfVar33[0xe];
        local_1eb8 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar33[0x12]));
        local_1eb4 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar33[0x13]));
        local_1eb0 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar33[0x14]));
        ppppppppppfVar22 = pppppppppppfVar33[10];
        local_1eac = (float ************)ppppppppppfVar22[0xc];
        local_1ea8 = (float ***********)ppppppppppfVar22[0xd];
        local_1ea4 = (float ***********)ppppppppppfVar22[0xe];
        local_1ea0 = (float ***********)DAT_001ed5b8;
        local_1e98 = local_1dd8[0];
        local_1e9c = pppppppppppfVar17;
        if (&local_1dcc != (float ***********)0x0) {
          ppppppppppppfVar85 = (float ************)(local_1dc4 + (int)local_1dcc * 0x27 + -0x27);
          *ppppppppppppfVar85 = local_1eb8;
          ppppppppppppfVar85[1] = (float ***********)local_1eb4;
          ppppppppppppfVar85[2] = local_1eb0;
          ppppppppppppfVar85[6] = (float ***********)local_1eac;
          ppppppppppppfVar85[7] = local_1ea8;
          ppppppppppppfVar85[8] = local_1ea4;
          func_004A17A4((float *)ppppppppppppfVar85);
          local_1e94 = (float ***********)((int)*local_1f04 + -1);
        }
        func_0049F5FC(pppppppppppfVar33 + 2);
        func_0049F5FC(pppppppppppfVar33 + 6);
        pppppppppfVar23 = (float *********)0x0;
        while( true ) {
          ppppppppppfVar22 = *pppppppppppfVar33;
          pppppppppfVar24 = *ppppppppppfVar22;
          if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
          if (pppppppppfVar23 < pppppppppfVar24) {
            ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
          }
          if (pppppppppfVar23 < pppppppppfVar24) {
            pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
          }
          else {
            pppppppppfVar24 = (float *********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar33 + 2),
                        (int *)((int)pppppppppppfVar33[5] + (int)pppppppppfVar24));
          pppppppppppfVar33[4] = (float **********)((int)pppppppppppfVar33[4] + 1);
          if (pppppppppfVar23 < **pppppppppppfVar33) {
            ppppppppfVar14 = (*pppppppppppfVar33)[2][(int)pppppppppfVar23];
          }
          else {
            ppppppppfVar14 = (float ********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar33 + 6),
                        (int *)((int)ppppppppfVar14 + (int)pppppppppppfVar33[9]));
          pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
          pppppppppppfVar33[8] = (float **********)((int)pppppppppppfVar33[8] + 1);
        }
        func_0064E9F4((int *)pppppppppppfVar33,local_1f28,(int *)&local_1ec8);
        ppppppppppfVar22 =
             (float **********)
             ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar33[0x1e]) - (ALBW_AS_F32(pppppppppppfVar17) - ALBW_AS_F32(local_1f28[0]))));
      }
      else {
        pppppppppppfVar33[0x1d] = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar78) * fVar55));
        local_1f00[0] = (float ***********)0x0;
        local_1ec8 = (float ***********)pppppppppppfVar33[10];
        local_1ec4 = (float ************)pppppppppppfVar33[0xc];
        local_1ec0 = (float ************)pppppppppppfVar33[0xd];
        local_1ebc = (float ***********)pppppppppppfVar33[0xe];
        local_1eb8 = (float ***********)pppppppppppfVar33[0x12];
        local_1eb4 = (float ************)pppppppppppfVar33[0x13];
        local_1eb0 = (float ***********)pppppppppppfVar33[0x14];
        ppppppppppfVar22 = pppppppppppfVar33[10];
        local_1eac = (float ************)ppppppppppfVar22[0xc];
        local_1ea8 = (float ***********)ppppppppppfVar22[0xd];
        local_1ea4 = (float ***********)ppppppppppfVar22[0xe];
        local_1ea0 = (float ***********)DAT_001ed5b8;
        local_1e98 = local_1dd8[0];
        local_1e9c = pppppppppppfVar78;
        if (&local_1dcc != (float ***********)0x0) {
          ppppppppppppfVar85 = (float ************)(local_1dc4 + (int)local_1dcc * 0x27 + -0x27);
          *ppppppppppppfVar85 = local_1eb8;
          ppppppppppppfVar85[1] = (float ***********)local_1eb4;
          ppppppppppppfVar85[2] = local_1eb0;
          ppppppppppppfVar85[6] = (float ***********)local_1eac;
          ppppppppppppfVar85[7] = local_1ea8;
          ppppppppppppfVar85[8] = local_1ea4;
          func_004A17A4((float *)ppppppppppppfVar85);
          local_1e94 = (float ***********)((int)*local_1f04 + -1);
        }
        func_0049F5FC(pppppppppppfVar33 + 2);
        func_0049F5FC(pppppppppppfVar33 + 6);
        pppppppppfVar23 = (float *********)0x0;
        while( true ) {
          ppppppppppfVar22 = *pppppppppppfVar33;
          pppppppppfVar24 = *ppppppppppfVar22;
          if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
          if (pppppppppfVar23 < pppppppppfVar24) {
            ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
          }
          if (pppppppppfVar23 < pppppppppfVar24) {
            pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
          }
          else {
            pppppppppfVar24 = (float *********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar33 + 2),
                        (int *)((int)pppppppppppfVar33[5] + (int)pppppppppfVar24));
          pppppppppppfVar33[4] = (float **********)((int)pppppppppppfVar33[4] + 1);
          ppppppppppfVar22 = *pppppppppppfVar33;
          pppppppppfVar24 = *ppppppppppfVar22;
          if (pppppppppfVar23 < pppppppppfVar24) {
            ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
          }
          if (pppppppppfVar23 < pppppppppfVar24) {
            pppppppppfVar24 = ppppppppppfVar22[(int)pppppppppfVar23];
          }
          else {
            pppppppppfVar24 = (float *********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar33 + 6),
                        (int *)((int)pppppppppppfVar33[9] + (int)pppppppppfVar24));
          pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
          pppppppppppfVar33[8] = (float **********)((int)pppppppppppfVar33[8] + 1);
        }
        func_0064CB28((int *)pppppppppppfVar33,(int *)local_1f28,(int *)&local_1ec8);
        ppppppppppfVar22 =
             (float **********)
             ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar33[0x1e]) + (ALBW_AS_F32(pppppppppppfVar78) - ALBW_AS_F32(local_1f28[0]))));
      }
      pppppppppppfVar33[0x1e] = ppppppppppfVar22;
      if (local_1f08 != (float ***********)0x0) {
        *(float ***********)(DAT_001eee2c + (int)pppppppppppfVar33) = *local_1f08;
        pppppppppppfVar33[0xc] = (float **********)local_1f28[1];
        pppppppppppfVar33[0xd] = (float **********)local_1f28[2];
        pppppppppppfVar33[0xe] = (float **********)local_1f28[3];
        if ((float ***********)pppppppppppfVar33[10] != local_1f08) {
          pppppppppppfVar33[10] = (float **********)local_1f08;
          pppppppppppfVar33[0x18] = local_1f08[0xc];
          pppppppppppfVar33[0x19] = local_1f08[0xd];
          pppppppppppfVar33[0x1a] = local_1f08[0xe];
          pppppppfVar51 = DAT_001ed5b8;
          pppppppppppfVar33[0x15] = (float **********)*DAT_001ed5b8;
          pppppppppppfVar33[0x16] = (float **********)pppppppfVar51[1];
          pppppppppppfVar33[0x17] = (float **********)pppppppfVar51[2];
          func_004A17E4((float *)(pppppppppppfVar33 + 0x12));
        }
        pppppppppppfVar33[0xf] = (float **********)local_1e38;
        pppppppppppfVar33[0x10] = (float **********)local_1e34;
        pppppppppppfVar33[0x11] = (float **********)local_1e30;
      }
      pppppppppppfVar33 = pppppppppppfVar34;
      local_1e88 = local_1f28[0];
      pppppppppppfVar34 = local_1f28[0];
      pppppppppppfVar17 = pppppppppppfVar33;
      if ((int)local_1f28[0] <= (int)ALBW_AS_U32(DAT_001eee30)) {
        fVar58 = fVar5;
        if ((float ***********)local_1dcc == (float ***********)0x1) {
          if (*local_1e64 == (float **********)0x0) {
            ppppppppppfVar22 = (float **********)0x0;
          }
          else {
            ppppppppppfVar22 = local_1e64[2];
          }
          ppppppppppfVar22[9] = (float *********)pppppppppppfVar33[0xc];
          ppppppppppfVar22[10] = (float *********)pppppppppppfVar33[0xd];
          ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar33[0xe];
          ppppppppppfVar22[0x1c] =
               (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) + ALBW_AS_F32(pppppppppppfVar9)));
        }
        else {
          ppppppppppppfVar85 = (float ************)0x0;
          pppppppppppfVar34 = (float ***********)0x0;
          if (*local_1e64 == (float **********)0x0) {
            ppppppppppfVar22 = (float **********)0x0;
          }
          else {
            ppppppppppfVar22 = local_1e64[2];
          }
          if (0 < (int)local_1dcc) {
            do {
              ppppppppppppfVar13 = (float ************)local_1dc4;
              if (pppppppppppfVar34 < local_1dcc) {
                ppppppppppppfVar13 =
                     (float ************)(local_1dc4 + (int)pppppppppppfVar34 * 0x27);
              }
              if (((ALBW_AS_F32(ppppppppppfVar22[6]) == ALBW_AS_F32(ppppppppppppfVar13[6])) &&
                  (ALBW_AS_F32(ppppppppppfVar22[7]) == ALBW_AS_F32(ppppppppppppfVar13[7]))) &&
                 (ALBW_AS_F32(ppppppppppfVar22[8]) == ALBW_AS_F32(ppppppppppppfVar13[8]))) {
                ppppppppppfVar22[9] = (float *********)ppppppppppppfVar13[0xc];
                ppppppppppfVar22[10] = (float *********)ppppppppppppfVar13[0xd];
                ppppppppppfVar22[0xb] = (float *********)ppppppppppppfVar13[0xe];
                pppppppppppfVar15 =
                     (float ***********)
                     ALBW_PTR_BITS((ALBW_AS_F32(ppppppppppppfVar13[0x1c]) - ALBW_AS_F32(ppppppppppppfVar13[0x1d])));
                ppppppppppfVar22[0x1c] =
                     (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) + ALBW_AS_F32(pppppppppppfVar15)));
                ppppppppppppfVar13 = ppppppppppppfVar85;
              }
              else {
                *(undefined1 *)(ppppppppppppfVar13 + 0x24) = 1;
                pppppppppppfVar15 =
                     (float ***********)
                     ALBW_PTR_BITS(((ALBW_AS_F32(ppppppppppppfVar13[0x1c]) - ALBW_AS_F32(ppppppppppppfVar13[0x1d])) +
                     ALBW_AS_F32(pppppppppppfVar15)));
                if (ppppppppppppfVar85 == (float ************)0x0) {
                  pppppppppppfVar78 = (float ***********)ppppppppppfVar22[0x1c];
                }
                else {
                  pppppppppppfVar78 = ppppppppppppfVar85[0x1c];
                }
                ppppppppppppfVar13[0x1d] = pppppppppppfVar78;
                ppppppppppppfVar13[0x1c] =
                     (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar75) + ALBW_AS_F32(pppppppppppfVar15)));
                pppppppppppfVar78 = ppppppppppppfVar13[9];
                pppppppppppfVar17 = ppppppppppppfVar13[10];
                pppppppppppfVar29 = ppppppppppppfVar13[0xb];
                ppppppppppppfVar13[9] = ppppppppppppfVar13[0xc];
                ppppppppppppfVar13[10] = ppppppppppppfVar13[0xd];
                ppppppppppppfVar13[0xb] = ppppppppppppfVar13[0xe];
                ppppppppppppfVar13[0xc] = pppppppppppfVar78;
                ppppppppppppfVar13[0xd] = pppppppppppfVar17;
                ppppppppppppfVar13[0xe] = pppppppppppfVar29;
                func_00115D70((float *)ppppppppppppfVar13,(float *)ppppppppppppfVar13,fVar5);
                func_00115D70((float *)(ppppppppppppfVar13 + 3),(float *)(ppppppppppppfVar13 + 3),
                              fVar5);
                ppppppppppfVar56 = *local_1e64;
                if ((int)ppppppppppfVar56 < (int)local_1e64[1]) {
                  *local_1e64 = (float **********)((int)ppppppppppfVar56 + 1);
                  ppppppppppfVar56 = local_1e64[2] + (int)ppppppppppfVar56 * 0x27;
                  local_1f00[1] = (float ***********)(ppppppppppppfVar13 + 0xc);
                  *ppppppppppfVar56 = (float *********)*ppppppppppppfVar13;
                  ppppppppppfVar56[1] = (float *********)ppppppppppppfVar13[1];
                  local_1f14 = (float ***********)(ppppppppppfVar56 + 0xf);
                  ppppppppppfVar56[2] = (float *********)ppppppppppppfVar13[2];
                  ppppppppppfVar56[3] = (float *********)ppppppppppppfVar13[3];
                  ppppppppppfVar56[4] = (float *********)ppppppppppppfVar13[4];
                  ppppppppppfVar56[5] = (float *********)ppppppppppppfVar13[5];
                  ppppppppppfVar56[6] = (float *********)ppppppppppppfVar13[6];
                  ppppppppppfVar56[7] = (float *********)ppppppppppppfVar13[7];
                  ppppppppppfVar56[8] = (float *********)ppppppppppppfVar13[8];
                  ppppppppppfVar56[9] = (float *********)ppppppppppppfVar13[9];
                  ppppppppppfVar56[10] = (float *********)ppppppppppppfVar13[10];
                  ppppppppppfVar56[0xb] = (float *********)ppppppppppppfVar13[0xb];
                  ppppppppppfVar56[0xc] = (float *********)*local_1f00[1];
                  ppppppppppfVar56[0xd] = (float *********)ppppppppppppfVar13[0xd];
                  ppppppppppfVar56[0xe] = (float *********)ppppppppppppfVar13[0xe];
                  *local_1f14 = (float **********)ppppppppppppfVar13[0xf];
                  ppppppppppfVar56[0x10] = (float *********)ppppppppppppfVar13[0x10];
                  ppppppppppfVar56[0x11] = (float *********)ppppppppppppfVar13[0x11];
                  ppppppppppfVar56[0x12] = (float *********)ppppppppppppfVar13[0x12];
                  ppppppppppfVar56[0x13] = (float *********)ppppppppppppfVar13[0x13];
                  ppppppppppfVar56[0x14] = (float *********)ppppppppppppfVar13[0x14];
                  ppppppppppfVar56[0x15] = (float *********)ppppppppppppfVar13[0x15];
                  ppppppppppfVar56[0x16] = (float *********)ppppppppppppfVar13[0x16];
                  ppppppppppfVar56[0x17] = (float *********)ppppppppppppfVar13[0x17];
                  ppppppppppfVar56[0x18] = (float *********)ppppppppppppfVar13[0x18];
                  ppppppppppfVar56[0x19] = (float *********)ppppppppppppfVar13[0x19];
                  ppppppppppfVar56[0x1a] = (float *********)ppppppppppppfVar13[0x1a];
                  ppppppppppfVar56[0x1b] = (float *********)ppppppppppppfVar13[0x1b];
                  ppppppppppfVar56[0x1c] = (float *********)ppppppppppppfVar13[0x1c];
                  ppppppppppfVar56[0x1d] = (float *********)ppppppppppppfVar13[0x1d];
                  ppppppppppfVar56[0x1e] = (float *********)ppppppppppppfVar13[0x1e];
                  ppppppppppfVar56[0x1f] = (float *********)ppppppppppppfVar13[0x1f];
                  ppppppppppfVar56[0x20] = (float *********)ppppppppppppfVar13[0x20];
                  ppppppppppfVar56[0x21] = (float *********)ppppppppppppfVar13[0x21];
                  ppppppppppfVar56[0x22] = (float *********)ppppppppppppfVar13[0x22];
                  ppppppppppfVar56[0x23] = (float *********)ppppppppppppfVar13[0x23];
                  *(undefined1 *)(ppppppppppfVar56 + 0x24) =
                       *(undefined1 *)(ppppppppppppfVar13 + 0x24);
                  *(undefined1 *)((int)ppppppppppfVar56 + 0x91) =
                       *(undefined1 *)((int)ppppppppppppfVar13 + 0x91);
                  *(undefined1 *)((int)ppppppppppfVar56 + 0x92) =
                       *(undefined1 *)((int)ppppppppppppfVar13 + 0x92);
                  *(undefined1 *)((int)ppppppppppfVar56 + 0x93) =
                       *(undefined1 *)((int)ppppppppppppfVar13 + 0x93);
                  *(undefined1 *)(ppppppppppfVar56 + 0x25) =
                       *(undefined1 *)(ppppppppppppfVar13 + 0x25);
                  *(undefined1 *)((int)ppppppppppfVar56 + 0x95) =
                       *(undefined1 *)((int)ppppppppppppfVar13 + 0x95);
                  *(undefined1 *)((int)ppppppppppfVar56 + 0x96) =
                       *(undefined1 *)((int)ppppppppppppfVar13 + 0x96);
                  *(undefined1 *)((int)ppppppppppfVar56 + 0x97) =
                       *(undefined1 *)((int)ppppppppppppfVar13 + 0x97);
                  *(undefined1 *)(ppppppppppfVar56 + 0x26) =
                       *(undefined1 *)(ppppppppppppfVar13 + 0x26);
                }
              }
              pppppppppppfVar34 = (float ***********)((int)pppppppppppfVar34 + 1);
              ppppppppppppfVar85 = ppppppppppppfVar13;
            } while ((int)pppppppppppfVar34 < (int)local_1dcc);
          }
        }
        goto LAB_001ee2c4;
      }
    }
    goto LAB_001f6204;
  }
LAB_001ee2c4:
  uVar67 = CONCAT44(pppppppppppfVar75,local_1e88);
  func_001FE680(&local_1ec8);
  local_1ea4 = local_1e08;
  ppppppppppfVar22 = *local_1e08;
  if ((int)ppppppppppfVar22 < (int)local_1e08[1]) {
    *local_1e08 = (float **********)((int)ppppppppppfVar22 + 1);
    ppppppppppfVar56 = local_1e08[2] + (int)ppppppppppfVar22 * 0x27;
  }
  else {
    ppppppppppfVar56 = (float **********)0x0;
  }
  local_1da4 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar48) + ALBW_AS_F32(uVar67)));
  ppppppppppfVar56[0x1c] = pppppppppfVar48;
  pppppppfVar51 = DAT_001ed5b8;
  ppppppppppfVar56[9] = (float *********)pppppppppppfVar33[0xc];
  ppppppppppfVar56[10] = (float *********)pppppppppppfVar33[0xd];
  ppppppppppfVar56[0xb] = (float *********)pppppppppppfVar33[0xe];
  *(undefined1 *)(ppppppppppfVar56 + 0x24) = 0;
  pppppppppppfVar33[0x1d] = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(local_1da4) * fVar55));
  local_1ea0 = (float ***********)0x0;
  local_1dd8[2] = (float ***********)pppppppppppfVar33[10];
  local_1dcc = pppppppppppfVar33[0xc];
  local_1dc8 = (float ***********)pppppppppppfVar33[0xd];
  local_1dc4 = (float ***********)pppppppppppfVar33[0xe];
  local_1dc0 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar33[0x12]));
  local_1db8 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar33[0x14]));
  local_1dbc = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar33[0x13]));
  ppppppppppfVar56 = pppppppppppfVar33[10];
  local_1db4 = (float ***********)ppppppppppfVar56[0xc];
  local_1db0 = (float ***********)ppppppppppfVar56[0xd];
  local_1dac = (float ***********)ppppppppppfVar56[0xe];
  local_1da8 = (float ***********)pppppppfVar51;
  local_1da0 = local_1dd8[0];
  pppppppppppfVar17 = pppppppppppfVar33;
  if (local_1e08 != (float ***********)0x0) {
    ppppppppppfVar56 = local_1e08[2] + (int)*local_1e08 * 0x27 + -0x27;
    *ppppppppppfVar56 = (float *********)local_1dc0;
    ppppppppppfVar56[1] = (float *********)local_1dbc;
    ppppppppppfVar56[2] = (float *********)local_1db8;
    ppppppppppfVar56[6] = (float *********)local_1db4;
    ppppppppppfVar56[7] = (float *********)local_1db0;
    ppppppppppfVar56[8] = (float *********)local_1dac;
    func_004A17A4((float *)ppppppppppfVar56);
    local_1d9c = (float ***********)((int)*local_1ea4 + -1);
  }
  local_1f28[3] = (float ***********)&local_1ec8;
  func_0049F5FC(pppppppppppfVar33 + 2);
  func_0049F5FC(pppppppppppfVar33 + 6);
  pppppppppfVar23 = (float *********)0x0;
  while( true ) {
    pppppppppppfVar15 = (float ***********)((ulonglong)uVar67 >> 0x20);
    ppppppppppfVar56 = *pppppppppppfVar33;
    pppppppppfVar24 = *ppppppppppfVar56;
    if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
    if (pppppppppfVar23 < pppppppppfVar24) {
      ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
    }
    if (pppppppppfVar23 < pppppppppfVar24) {
      pppppppppfVar24 = ppppppppppfVar56[(int)pppppppppfVar23];
    }
    else {
      pppppppppfVar24 = (float *********)0x0;
    }
    func_0010F1DC((int *)(pppppppppppfVar33 + 2),
                  (int *)((int)pppppppppppfVar33[5] + (int)pppppppppfVar24));
    pppppppppppfVar33[4] = (float **********)((int)pppppppppppfVar33[4] + 1);
    ppppppppppfVar56 = *pppppppppppfVar33;
    pppppppppfVar24 = *ppppppppppfVar56;
    if (pppppppppfVar23 < pppppppppfVar24) {
      ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
    }
    if (pppppppppfVar23 < pppppppppfVar24) {
      pppppppppfVar24 = ppppppppppfVar56[(int)pppppppppfVar23];
    }
    else {
      pppppppppfVar24 = (float *********)0x0;
    }
    func_0010F1DC((int *)(pppppppppppfVar33 + 6),
                  (int *)((int)pppppppppppfVar33[9] + (int)pppppppppfVar24));
    pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
    pppppppppppfVar33[8] = (float **********)((int)pppppppppppfVar33[8] + 1);
  }
  func_0064E9F4((int *)pppppppppppfVar33,local_1f28[3],(int *)(local_1dd8 + 2));
  if (local_1ea8 == (float ***********)0x0) goto LAB_001f6210;
  pppppppppppfVar17[0xedf] = (float **********)local_1ec4;
  pppppppppppfVar17[0xee0] = (float **********)local_1ec0;
  pppppppppppfVar17[0xee1] = (float **********)local_1ebc;
  pppppppppppfVar17[0xee5] = local_1ea8[0xc];
  pppppppppppfVar17[0xee6] = local_1ea8[0xd];
  pppppppppppfVar17[0xee7] = local_1ea8[0xe];
  if (DAT_001edb14 < (int)local_1ec8) {
    if ((ALBW_AS_F32(pppppppppppfVar17[0x1e]) == ALBW_AS_F32(pppppppppppfVar50)) ||
       (pppppppppppfVar34 = local_1ec8, (int)local_1ec8 <= (int)DAT_001edb18)) {
      pppppppppppfVar49 = local_1ec8;
      pppppppppppfVar74 = local_1ec8;
      pppppppppppfVar78 = pppppppppppfVar50;
      pppppppppppfVar9 = pppppppppppfVar17;
      func_001FE680(local_1f28);
      local_1f0c = (float ***********)&local_1e88;
      local_1e6c = (float ***********)local_1f28;
      local_1e68 = pppppppppppfVar17;
      if (ALBW_AS_F32(pppppppppppfVar49) <= ALBW_AS_F32(pppppppppppfVar50)) {
        ppppppppppppfVar85 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar49));
        pppppppppppfVar17[0x1d] = (float **********)ALBW_PTR_BITS(-(fVar55 * ALBW_AS_F32(pppppppppppfVar49)));
        local_1dd8[2] = (float ***********)pppppppppppfVar17[10];
        local_1dcc = pppppppppppfVar17[0xc];
        local_1dc8 = (float ***********)pppppppppppfVar17[0xd];
        local_1dc4 = (float ***********)pppppppppppfVar17[0xe];
        local_1dc0 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar17[0x12]));
        local_1dbc = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar17[0x13]));
        local_1db8 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(pppppppppppfVar17[0x14]));
        ppppppppppfVar56 = pppppppppppfVar17[10];
        local_1db4 = (float ***********)ppppppppppfVar56[0xc];
        local_1db0 = (float ***********)ppppppppppfVar56[0xd];
        local_1dac = (float ***********)ppppppppppfVar56[0xe];
        local_1da0 = local_1dd8[0];
        local_1da8 = (float ***********)pppppppfVar51;
        local_1f00[0] = (float ***********)0x0;
        local_1f04 = (float ***********)0x0;
        local_1da4 = (float ***********)ppppppppppppfVar85;
        func_0049F5FC(pppppppppppfVar17 + 2);
        func_0049F5FC(pppppppppppfVar17 + 6);
        pppppppppfVar23 = (float *********)0x0;
        while( true ) {
          ppppppppppfVar56 = *pppppppppppfVar17;
          pppppppppfVar24 = *ppppppppppfVar56;
          if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
          if (pppppppppfVar23 < pppppppppfVar24) {
            ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
          }
          if (pppppppppfVar23 < pppppppppfVar24) {
            pppppppppfVar24 = ppppppppppfVar56[(int)pppppppppfVar23];
          }
          else {
            pppppppppfVar24 = (float *********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar17 + 2),
                        (int *)((int)pppppppppppfVar17[5] + (int)pppppppppfVar24));
          pppppppppppfVar17[4] = (float **********)((int)pppppppppppfVar17[4] + 1);
          ppppppppppfVar56 = *pppppppppppfVar17;
          pppppppppfVar24 = *ppppppppppfVar56;
          if (pppppppppfVar23 < pppppppppfVar24) {
            ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
          }
          if (pppppppppfVar23 < pppppppppfVar24) {
            pppppppppfVar24 = ppppppppppfVar56[(int)pppppppppfVar23];
          }
          else {
            pppppppppfVar24 = (float *********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar17 + 6),
                        (int *)((int)pppppppppppfVar17[9] + (int)pppppppppfVar24));
          pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
          pppppppppppfVar17[8] = (float **********)((int)pppppppppppfVar17[8] + 1);
        }
        func_0064E9F4((int *)local_1e68,local_1e6c,(int *)(local_1dd8 + 2));
        ppppppppppfVar56 =
             (float **********)
             ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar17[0x1e]) - (ALBW_AS_F32(ppppppppppppfVar85) - ALBW_AS_F32(local_1f28[0]))));
      }
      else {
        pppppppppppfVar17[0x1d] = (float **********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar49) * fVar55));
        local_1dd8[2] = (float ***********)pppppppppppfVar17[10];
        local_1dcc = pppppppppppfVar17[0xc];
        local_1dc8 = (float ***********)pppppppppppfVar17[0xd];
        local_1dc4 = (float ***********)pppppppppppfVar17[0xe];
        local_1dc0 = (float ***********)pppppppppppfVar17[0x12];
        local_1dbc = (float ************)pppppppppppfVar17[0x13];
        local_1db8 = (float ************)pppppppppppfVar17[0x14];
        ppppppppppfVar56 = pppppppppppfVar17[10];
        local_1db4 = (float ***********)ppppppppppfVar56[0xc];
        local_1db0 = (float ***********)ppppppppppfVar56[0xd];
        local_1dac = (float ***********)ppppppppppfVar56[0xe];
        local_1da0 = local_1dd8[0];
        local_1da8 = (float ***********)pppppppfVar51;
        local_1f00[0] = (float ***********)0x0;
        local_1f04 = (float ***********)0x0;
        local_1da4 = pppppppppppfVar49;
        func_0049F5FC(pppppppppppfVar17 + 2);
        func_0049F5FC(pppppppppppfVar17 + 6);
        pppppppppfVar23 = (float *********)0x0;
        while( true ) {
          ppppppppppfVar56 = *pppppppppppfVar17;
          pppppppppfVar24 = *ppppppppppfVar56;
          if ((int)pppppppppfVar24 <= (int)pppppppppfVar23) break;
          if (pppppppppfVar23 < pppppppppfVar24) {
            ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
          }
          if (pppppppppfVar23 < pppppppppfVar24) {
            pppppppppfVar24 = ppppppppppfVar56[(int)pppppppppfVar23];
          }
          else {
            pppppppppfVar24 = (float *********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar17 + 2),
                        (int *)((int)pppppppppppfVar17[5] + (int)pppppppppfVar24));
          pppppppppppfVar17[4] = (float **********)((int)pppppppppppfVar17[4] + 1);
          ppppppppppfVar56 = *pppppppppppfVar17;
          pppppppppfVar24 = *ppppppppppfVar56;
          if (pppppppppfVar23 < pppppppppfVar24) {
            ppppppppppfVar56 = (float **********)ppppppppppfVar56[2];
          }
          if (pppppppppfVar23 < pppppppppfVar24) {
            pppppppppfVar24 = ppppppppppfVar56[(int)pppppppppfVar23];
          }
          else {
            pppppppppfVar24 = (float *********)0x0;
          }
          func_0010F1DC((int *)(pppppppppppfVar17 + 6),
                        (int *)((int)pppppppppppfVar17[9] + (int)pppppppppfVar24));
          pppppppppfVar23 = (float *********)((int)pppppppppfVar23 + 1);
          pppppppppppfVar17[8] = (float **********)((int)pppppppppppfVar17[8] + 1);
        }
        func_0064CB28((int *)local_1e68,(int *)local_1e6c,(int *)(local_1dd8 + 2));
        ppppppppppfVar56 =
             (float **********)
             ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar17[0x1e]) + (ALBW_AS_F32(pppppppppppfVar49) - ALBW_AS_F32(local_1f28[0]))));
      }
      pppppppppppfVar17[0x1e] = ppppppppppfVar56;
      if (local_1f08 != (float ***********)0x0) {
        *(float ***********)(DAT_001eee2c + (int)pppppppppppfVar17) = *local_1f08;
        pppppppppppfVar17[0xc] = (float **********)local_1f28[1];
        pppppppppppfVar17[0xd] = (float **********)local_1f28[2];
        pppppppppppfVar17[0xe] = (float **********)local_1f28[3];
        if ((float ***********)pppppppppppfVar17[10] != local_1f08) {
          pppppppppppfVar17[10] = (float **********)local_1f08;
          pppppppppppfVar17[0x18] = local_1f08[0xc];
          pppppppppppfVar17[0x19] = local_1f08[0xd];
          pppppppppppfVar17[0x1a] = local_1f08[0xe];
          pppppppppppfVar17[0x15] = (float **********)*pppppppfVar51;
          pppppppppppfVar17[0x16] = (float **********)pppppppfVar51[1];
          pppppppppppfVar17[0x17] = (float **********)pppppppfVar51[2];
          func_004A17E4((float *)(pppppppppppfVar17 + 0x12));
        }
        pppppppppppfVar17[0xf] = (float **********)local_1e88;
        pppppppppppfVar17[0x10] = (float **********)local_1e84;
        pppppppppppfVar17[0x11] = (float **********)local_1e80;
      }
      fVar5 = DAT_001eee30;
      local_1ec8 = local_1f28[0];
      if (ppppppppppfVar22 < *local_1ea4) {
        ppppppppppfVar22 = local_1ea4[2] + (int)ppppppppppfVar22 * 0x27;
      }
      else {
        ppppppppppfVar22 = (float **********)0x0;
      }
      ppppppppppfVar22[9] = (float *********)pppppppppppfVar9[0xc];
      ppppppppppfVar22[10] = (float *********)pppppppppppfVar9[0xd];
      ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar9[0xe];
      ppppppppppfVar22[0x1c] = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar48) + ALBW_AS_F32(pppppppppppfVar74)));
      pppppppppppfVar34 = local_1f28[0];
      pppppppppppfVar50 = pppppppppppfVar78;
      pppppppppppfVar17 = pppppppppppfVar9;
      if ((int)local_1f28[0] <= (int)ALBW_AS_U32(fVar5)) {
        *(char *)(pppppppppppfVar9 + 0xf70) = '\0';
        pppppppppppfVar9[0x29] = (float **********)0x0;
        pppppppppppfVar9[0x77c] = (float **********)0x0;
        pppppppppppfVar34 = pppppppppppfVar15;
        /* EU 001EEA30..001EEA64: r0 = param_1, r1 = 1, r2 = this branch's flag (param_2[0x1a] != 0, [sp,#0xe8]),
         * s0 / s1 = param_2[0x14] / [0x15] (BOOT_LOG 166) */
        func_0064268C((float ***********)param_1,1,(int)(uintptr_t)local_1e48,param_2[0x14],param_2[0x15]);
        pppppppppppfVar50 = pppppppppppfVar78;
        pppppppppppfVar17 = pppppppppppfVar9;
      }
    }
    goto LAB_001f6204;
  }
  if (local_1e48 != (float ***********)0x0) {
    pfVar39 = (float *)(local_1f00 + 2);
    pppppppppppfVar53 = pppppppppppfVar50;
    func_001FE680(pfVar39);
    uVar67 = CONCAT44(pppppppppppfVar50,pppppppppppfVar79);
    local_1ed4 = pppppppppppfVar17 + 0x77c;
    *local_1ed4 = (float **********)0x0;
    ppppppppppfVar22 = *local_1ed4;
    if ((int)ppppppppppfVar22 < (int)pppppppppppfVar17[0x77d]) {
      *local_1ed4 = (float **********)((int)ppppppppppfVar22 + 1);
      ppppppppppfVar22 = pppppppppppfVar17[0x77e] + (int)ppppppppppfVar22 * 0x27;
    }
    else {
      ppppppppppfVar22 = (float **********)0x0;
    }
    local_1f28[0] = (float ***********)DAT_001eee34;
    uVar72 = CONCAT44(DAT_001eee34,pppppppppfVar48);
    ppppppppppfVar22[0x1c] = (float *********)pppppppppppfVar15;
    ppppppppppfVar22[9] = (float *********)pppppppppppfVar17[0xf];
    ppppppppppfVar22[10] = (float *********)pppppppppppfVar17[0x10];
    ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar17[0x11];
    *(undefined1 *)(ppppppppppfVar22 + 0x24) = 1;
    local_1ed0 = (float ***********)0x0;
    local_1ec8 = (float ***********)pppppppppppfVar17[0xb];
    local_1ec4 = (float ************)pppppppppppfVar17[0xf];
    local_1ec0 = (float ************)pppppppppppfVar17[0x10];
    local_1ebc = (float ***********)pppppppppppfVar17[0x11];
    local_1f10 = (float ***********)local_1ec8[0xc];
    local_1f0c = (float ***********)local_1ec8[0xd];
    local_1f08 = (float ***********)local_1ec8[0xe];
    local_1f28[3] = (float ***********)*pppppppfVar51;
    local_1f28[4] = (float ***********)pppppppfVar51[1];
    local_1f14 = (float ***********)pppppppfVar51[2];
    pppppppppppfVar52 = pppppppppppfVar17;
    local_1f28[1] = pppppppppppfVar50;
    local_1f28[2] = pppppppppppfVar50;
    local_1e88 = local_1ed4;
    func_004A17E4((float *)local_1f28);
    local_1eb8 = local_1f28[0];
    local_1eb4 = (float ************)local_1f28[1];
    local_1eb0 = local_1f28[2];
    ppppppppppfVar22 = pppppppppppfVar17[0xb];
    local_1eac = (float ************)ppppppppppfVar22[0xc];
    local_1ea8 = (float ***********)ppppppppppfVar22[0xd];
    local_1ea4 = (float ***********)ppppppppppfVar22[0xe];
    local_1ea0 = (float ***********)pppppppfVar51;
    local_1e98 = local_1dd8[0];
    local_1e9c = pppppppppppfVar15;
    if (local_1ed4 != (float ***********)0x0) {
      ppppppppppfVar22 = local_1ed4[2] + (int)*local_1ed4 * 0x27 + -0x27;
      *ppppppppppfVar22 = (float *********)local_1f28[0];
      ppppppppppfVar22[1] = (float *********)local_1f28[1];
      ppppppppppfVar22[2] = (float *********)local_1f28[2];
      ppppppppppfVar22[6] = (float *********)local_1eac;
      ppppppppppfVar22[7] = (float *********)local_1ea8;
      ppppppppppfVar22[8] = (float *********)local_1ea4;
      func_004A17A4((float *)ppppppppppfVar22);
      local_1e94 = (float ***********)((int)*local_1ed4 + -1);
    }
    func_0049F5FC(pppppppppppfVar17 + 2);
    func_0049F5FC(pppppppppppfVar17 + 6);
    pppppppppfVar48 = (float *********)0x0;
    while( true ) {
      ppppppppppfVar22 = *pppppppppppfVar17;
      pppppppppfVar23 = *ppppppppppfVar22;
      if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
      if (pppppppppfVar48 < pppppppppfVar23) {
        ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
      }
      if (pppppppppfVar48 < pppppppppfVar23) {
        pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
      }
      else {
        pppppppppfVar23 = (float *********)0x0;
      }
      func_0010F1DC((int *)(pppppppppppfVar17 + 2),
                    (int *)((int)pppppppppppfVar17[5] + (int)pppppppppfVar23));
      pppppppppppfVar17[4] = (float **********)((int)pppppppppppfVar17[4] + 1);
      ppppppppppfVar22 = *pppppppppppfVar17;
      pppppppppfVar23 = *ppppppppppfVar22;
      if (pppppppppfVar48 < pppppppppfVar23) {
        ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
      }
      if (pppppppppfVar48 < pppppppppfVar23) {
        pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
      }
      else {
        pppppppppfVar23 = (float *********)0x0;
      }
      func_0010F1DC((int *)(pppppppppppfVar17 + 6),
                    (int *)((int)pppppppppppfVar17[9] + (int)pppppppppfVar23));
      pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
      pppppppppppfVar17[8] = (float **********)((int)pppppppppppfVar17[8] + 1);
    }
    func_00649E18((int)pppppppppppfVar17,pfVar39,(float *)&local_1ec8);
    uVar70 = ZEXT48(local_1f00[2]);
    ppppppppppfVar22 = *local_1ed4;
    if ((int)ppppppppppfVar22 < (int)local_1ed4[1]) {
      *local_1ed4 = (float **********)((int)ppppppppppfVar22 + 1);
      ppppppppppfVar22 = local_1ed4[2] + (int)ppppppppppfVar22 * 0x27;
    }
    else {
      ppppppppppfVar22 = (float **********)0x0;
    }
    local_1e24 = (float ***********)((ulonglong)uVar72 >> 0x20);
    uVar71 = CONCAT44(pppppppppppfVar83,local_1e24);
    pppppppppfVar48 = (float *********)uVar72;
    ppppppppppfVar22[0x1c] = pppppppppfVar48;
    local_1e20 = (float ***********)((ulonglong)uVar67 >> 0x20);
    uVar72 = CONCAT44(local_1e20,pppppppppfVar48);
    ppppppppppfVar22[9] = (float *********)pppppppppppfVar52[0xf];
    ppppppppppfVar22[10] = (float *********)pppppppppppfVar52[0x10];
    ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar52[0x11];
    *(undefined1 *)(ppppppppppfVar22 + 0x24) = 0;
    local_1ed0 = (float ***********)0x0;
    local_1ec8 = (float ***********)pppppppppppfVar52[0xb];
    pppppppppppfVar81 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(local_1f00[2]) + ALBW_AS_F32(pppppppppfVar48)));
    local_1ec4 = (float ************)pppppppppppfVar52[0xf];
    local_1ec0 = (float ************)pppppppppppfVar52[0x10];
    local_1ebc = (float ***********)pppppppppppfVar52[0x11];
    local_1e0c = (float ***********)local_1ec8[0xc];
    local_1e08 = (float ***********)local_1ec8[0xd];
    local_1e04 = (float ***********)local_1ec8[0xe];
    local_1e18 = (float ***********)*pppppppfVar51;
    local_1e14 = (float ************)pppppppfVar51[1];
    local_1e10 = (float ************)pppppppfVar51[2];
    pppppppppppfVar74 = pppppppppppfVar52;
    local_1e1c = local_1e20;
    func_004A17E4((float *)&local_1e24);
    local_1eb8 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(local_1e24));
    local_1eb4 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(local_1e20));
    local_1eb0 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(local_1e1c));
    ppppppppppfVar22 = pppppppppppfVar52[0xb];
    local_1eac = (float ************)ppppppppppfVar22[0xc];
    local_1ea8 = (float ***********)ppppppppppfVar22[0xd];
    local_1ea4 = (float ***********)ppppppppppfVar22[0xe];
    local_1ea0 = (float ***********)pppppppfVar51;
    local_1e98 = local_1dd8[0];
    local_1e9c = pppppppppppfVar81;
    if (local_1ed4 != (float ***********)0x0) {
      ppppppppppfVar22 = local_1ed4[2] + (int)*local_1ed4 * 0x27 + -0x27;
      *ppppppppppfVar22 = (float *********)local_1eb8;
      ppppppppppfVar22[1] = (float *********)local_1eb4;
      ppppppppppfVar22[2] = (float *********)local_1eb0;
      ppppppppppfVar22[6] = (float *********)local_1eac;
      ppppppppppfVar22[7] = (float *********)local_1ea8;
      ppppppppppfVar22[8] = (float *********)local_1ea4;
      func_004A17A4((float *)ppppppppppfVar22);
      local_1e94 = (float ***********)((int)*local_1ed4 + -1);
    }
    func_0049F5FC(pppppppppppfVar52 + 2);
    func_0049F5FC(pppppppppppfVar52 + 6);
    pppppppppfVar48 = (float *********)0x0;
    while( true ) {
      pppppppppppfVar79 = (float ***********)uVar67;
      fVar5 = ALBW_AS_F32(uVar70);
      ppppppppppfVar22 = *pppppppppppfVar52;
      pppppppppfVar23 = *ppppppppppfVar22;
      if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
      if (pppppppppfVar48 < pppppppppfVar23) {
        ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
      }
      if (pppppppppfVar48 < pppppppppfVar23) {
        pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
      }
      else {
        pppppppppfVar23 = (float *********)0x0;
      }
      func_0010F1DC((int *)(pppppppppppfVar52 + 2),
                    (int *)((int)pppppppppppfVar52[5] + (int)pppppppppfVar23));
      pppppppppppfVar52[4] = (float **********)((int)pppppppppppfVar52[4] + 1);
      ppppppppppfVar22 = *pppppppppppfVar52;
      pppppppppfVar23 = *ppppppppppfVar22;
      if (pppppppppfVar48 < pppppppppfVar23) {
        ppppppppppfVar22 = (float **********)ppppppppppfVar22[2];
      }
      if (pppppppppfVar48 < pppppppppfVar23) {
        pppppppppfVar23 = ppppppppppfVar22[(int)pppppppppfVar48];
      }
      else {
        pppppppppfVar23 = (float *********)0x0;
      }
      func_0010F1DC((int *)(pppppppppppfVar52 + 6),
                    (int *)((int)pppppppppppfVar52[9] + (int)pppppppppfVar23));
      pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
      pppppppppppfVar52[8] = (float **********)((int)pppppppppppfVar52[8] + 1);
    }
    func_0064B4A0((int)pppppppppppfVar52,pfVar39,(float *)&local_1ec8);
    local_1e30 = (float ***********)0x0;
    fVar55 = ALBW_AS_F32(pppppppppppfVar15) + ALBW_AS_F32(pppppppppppfVar9);
    pppppppppppfVar52 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(uVar72) + ALBW_AS_F32(pppppppppppfVar50)));
    pppppppppppfVar17 = pppppppppppfVar74 + 0x29;
    pppppppppppfVar34 = local_1f00[2];
    pppppppppppfVar78 = pppppppppppfVar74;
    if (((*DAT_001ef398 & 1) == 0) &&
       (iVar21 = func_00100174((int *)DAT_001ef398), puVar4 = DAT_001ef39c, iVar21 != 0)) {
      puVar26 = DAT_001ef39c + 3;
      *DAT_001ef39c = 0;
      puVar4[1] = 0;
      puVar4[2] = 0;
      func_0010B3EC(puVar4,0x20,(int)puVar26);
    }
    fVar57 = DAT_001ef3a0;
    puVar4 = DAT_001ef39c;
    pppppppppppfVar81 = (float ***********)uVar71;
    pppppppppppfVar83 = (float ***********)((ulonglong)uVar71 >> 0x20);
    uVar67 = CONCAT44(pppppppppppfVar34,DAT_001ef3a0);
    *DAT_001ef39c = 0;
    ppppppppppfVar22 = (float **********)0x0;
    fVar60 = ALBW_AS_F32(pppppppppppfVar81) / (ALBW_AS_F32(pppppppppppfVar9) - fVar55);
    local_1f28[1] = (float ***********)ALBW_PTR_BITS((fVar60 * fVar57));
    local_1ec4 = (float ************)ALBW_PTR_BITS((fVar57 + fVar55 * fVar60 * ALBW_AS_F32(pppppppppppfVar49)));
    fVar55 = ALBW_AS_F32(pppppppppppfVar81) / (ALBW_AS_F32(pppppppppppfVar50) - ALBW_AS_F32(pppppppppppfVar52));
    local_1f28[0] = (float ***********)ALBW_PTR_BITS((fVar55 * ALBW_AS_F32(pppppppppppfVar49)));
    local_1ec8 = (float ***********)ALBW_PTR_BITS((fVar57 + ALBW_AS_F32(pppppppppppfVar52) * fVar55 * fVar57));
    pppppppppppfVar50 = pppppppppppfVar53;
    pppppppppppfVar53 = pppppppppppfVar49;
    if (0 < (int)*pppppppppppfVar17) {
      do {
        if (ppppppppppfVar22 < *pppppppppppfVar17) {
          ppppppppppfVar56 = pppppppppppfVar74[0x2b] + (int)ppppppppppfVar22 * 0x27;
        }
        else {
          ppppppppppfVar56 = pppppppppppfVar74[0x2b];
        }
        ppppppppppfVar56[0x1c] =
             (float *********)
             ALBW_PTR_BITS((ALBW_AS_F32((&local_1ec8)[*(char *)(ppppppppppfVar56 + 0x24)]) +
             ALBW_AS_F32(local_1f28[*(char *)(ppppppppppfVar56 + 0x24)]) * ALBW_AS_F32(ppppppppppfVar56[0x1c])));
        pppppppppfVar48 =
             (float *********)
             ALBW_PTR_BITS((ALBW_AS_F32((&local_1ec8)[*(char *)(ppppppppppfVar56 + 0x24)]) +
             ALBW_AS_F32(local_1f28[*(char *)(ppppppppppfVar56 + 0x24)]) * ALBW_AS_F32(ppppppppppfVar56[0x1d])));
        ppppppppppfVar56[0x1d] = pppppppppfVar48;
        if (ALBW_AS_F32(pppppppppfVar48) < ALBW_AS_F32(ppppppppppfVar56[0x1c])) {
          pppppppppfVar48 = ppppppppppfVar56[9];
          pppppppppfVar23 = ppppppppppfVar56[10];
          pppppppppfVar24 = ppppppppppfVar56[0xb];
          ppppppppppfVar56[9] = ppppppppppfVar56[0xc];
          ppppppppppfVar56[10] = ppppppppppfVar56[0xd];
          ppppppppppfVar56[0xb] = ppppppppppfVar56[0xe];
          ppppppppppfVar56[0xc] = pppppppppfVar48;
          ppppppppppfVar56[0xd] = pppppppppfVar23;
          ppppppppppfVar56[0xe] = pppppppppfVar24;
          pppppppppfVar48 = ppppppppppfVar56[0x1c];
          ppppppppppfVar56[0x1c] = ppppppppppfVar56[0x1d];
          ppppppppppfVar56[0x1d] = pppppppppfVar48;
        }
        ppppppppppfVar56[0xf] = ppppppppppfVar56[9];
        ppppppppppfVar56[0x10] = ppppppppppfVar56[10];
        ppppppppppfVar56[0x11] = ppppppppppfVar56[0xb];
        ppppppppppfVar56[0x12] = ppppppppppfVar56[0xc];
        ppppppppppfVar56[0x13] = ppppppppppfVar56[0xd];
        ppppppppppfVar56[0x14] = ppppppppppfVar56[0xe];
        *(undefined1 *)((int)ppppppppppfVar56 + 0x92) = 0;
        *(undefined1 *)((int)ppppppppppfVar56 + 0x91) = 0;
        ppppppppppfVar56[0x1b] = (float *********)((ulonglong)uVar72 >> 0x20);
        *(undefined1 *)(ppppppppppfVar56 + 0x26) = 0;
        *(undefined1 *)((int)ppppppppppfVar56 + 0x97) = 0;
        *(undefined1 *)((int)ppppppppppfVar56 + 0x95) = 0;
        *(undefined1 *)(ppppppppppfVar56 + 0x25) = 0;
        if (*(char *)(ppppppppppfVar56 + 0x24) == '\0') {
          func_00115D70((float *)ppppppppppfVar56,(float *)ppppppppppfVar56,fVar58);
          func_00115D70((float *)(ppppppppppfVar56 + 3),(float *)(ppppppppppfVar56 + 3),fVar58);
        }
        ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
      } while ((int)ppppppppppfVar22 < (int)*pppppppppppfVar17);
    }
    ppppppppppfVar22 = (float **********)0x0;
    if (0 < (int)*pppppppppppfVar17) {
      do {
        puVar26 = DAT_001ef39c;
        ppppppppppfVar73 = *pppppppppppfVar17;
        ppppppppppfVar56 = pppppppppppfVar74[0x2b];
        if (ppppppppppfVar22 < ppppppppppfVar73) {
          ppppppppppfVar56 = ppppppppppfVar56 + (int)ppppppppppfVar22 * 0x27;
        }
        ppppppppppfVar30 = pppppppppppfVar74[0x2b];
        if (ppppppppppfVar22 < ppppppppppfVar73) {
          ppppppppppfVar30 = ppppppppppfVar30 + (int)ppppppppppfVar22 * 0x27;
        }
        if (WALK_SEG_HAS_LENGTH(ppppppppppfVar56, ppppppppppfVar30)) {
          if (ppppppppppfVar22 < ppppppppppfVar73) {
            ppppppppppfVar56 = pppppppppppfVar74[0x2b] + (int)ppppppppppfVar22 * 0x27;
          }
          else {
            ppppppppppfVar56 = pppppppppppfVar74[0x2b];
          }
          if ((int)*DAT_001ef39c < (int)DAT_001ef39c[1]) {
            *(float ***********)(DAT_001ef39c[2] + *DAT_001ef39c * 4) = ppppppppppfVar56;
            *puVar26 = *puVar26 + 1;
          }
        }
        ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
      } while ((int)ppppppppppfVar22 < (int)*pppppppppppfVar17);
    }
    *pppppppppppfVar17 = (float **********)0x0;
    if (*puVar4 != 0) {
      func_0048C258((int *)DAT_001ef39c,DAT_001ef3a4);
      if (((*DAT_001ef3a8 & 1) == 0) &&
         (iVar21 = func_00100174((int *)DAT_001ef3a8), puVar26 = DAT_001ef3ac, iVar21 != 0)) {
        puVar27 = DAT_001ef3ac + 3;
        *DAT_001ef3ac = 0;
        puVar26[1] = 0x30;
        puVar26[2] = (uint)puVar27;
        func_00100118((int)(puVar26 + 3),DAT_001ef3b0,0x9c,0x30);
      }
      puVar26 = DAT_001ef3ac;
      *DAT_001ef3ac = 0;
      if (*puVar4 == 0) {
        puVar18 = (undefined4 *)0x0;
      }
      else {
        puVar18 = *(undefined4 **)puVar4[2];
      }
      if (0 < (int)puVar26[1]) {
        *puVar26 = 1;
        func_001FE514((undefined4 *)puVar26[2],puVar18);
      }
      iVar21 = *puVar26 * 0x9c + -0x9c + puVar26[2];
      uVar36 = 1;
      if (1 < (int)*puVar4) {
        do {
          uVar43 = *puVar4;
          uVar16 = uVar43;
          if (uVar36 < uVar43) {
            uVar16 = puVar4[2];
          }
          if (uVar36 < uVar43) {
            puVar18 = *(undefined4 **)(uVar16 + uVar36 * 4);
          }
          else {
            puVar18 = (undefined4 *)0x0;
          }
          fVar55 = *(float *)(iVar21 + 0x18) * ALBW_AS_F32(puVar18[6]) +
                   *(float *)(iVar21 + 0x1c) * ALBW_AS_F32(puVar18[7]) +
                   *(float *)(iVar21 + 0x20) * ALBW_AS_F32(puVar18[8]);
          if (fVar55 <= ALBW_AS_F32(((ulonglong)uVar72 >> 0x20))) {
            fVar55 = -fVar55;
          }
          if (DAT_001ef3b4 < (int)ALBW_AS_U32(fVar55)) {
            if (*(float *)(iVar21 + 0x74) < ALBW_AS_F32(puVar18[0x1d])) {
              *(undefined4 *)(iVar21 + 0x30) = puVar18[0xc];
              *(undefined4 *)(iVar21 + 0x34) = puVar18[0xd];
              *(undefined4 *)(iVar21 + 0x38) = puVar18[0xe];
              *(undefined4 *)(iVar21 + 0x74) = puVar18[0x1d];
              *(undefined4 *)(iVar21 + 0x48) = puVar18[0x12];
              *(undefined4 *)(iVar21 + 0x4c) = puVar18[0x13];
              *(undefined4 *)(iVar21 + 0x50) = puVar18[0x14];
            }
          }
          else {
            uVar16 = *puVar26;
            if ((int)uVar16 < (int)puVar26[1]) {
              *puVar26 = uVar16 + 1;
              local_1f00[0] = (float ***********)(puVar18 + 6);
              puVar7 = (undefined4 *)(puVar26[2] + uVar16 * 0x9c);
              local_1ebc = (float ***********)(puVar18 + 0xc);
              *puVar7 = *puVar18;
              puVar7[1] = puVar18[1];
              puVar7[2] = puVar18[2];
              local_1eb4 = (float ************)(puVar7 + 0xf);
              puVar7[3] = puVar18[3];
              puVar7[4] = puVar18[4];
              local_1eac = (float ************)(puVar18 + 0x12);
              puVar7[5] = puVar18[5];
              local_1eb0 = (float ***********)(puVar7 + 0x12);
              puVar7[6] = (float **********)puVar18[6];
              puVar7[7] = puVar18[7];
              puVar7[8] = puVar18[8];
              puVar7[9] = puVar18[9];
              puVar7[10] = puVar18[10];
              local_1f04 = (float ***********)(puVar18 + 0x18);
              puVar7[0xb] = puVar18[0xb];
              puVar7[0xc] = (float **********)puVar18[0xc];
              puVar7[0xd] = puVar18[0xd];
              puVar7[0xe] = puVar18[0xe];
              puVar7[0xf] = (float **********)puVar18[0xf];
              puVar7[0x10] = puVar18[0x10];
              puVar7[0x11] = puVar18[0x11];
              puVar7[0x12] = (float **********)puVar18[0x12];
              puVar7[0x13] = puVar18[0x13];
              puVar7[0x14] = puVar18[0x14];
              puVar7[0x15] = puVar18[0x15];
              puVar7[0x16] = puVar18[0x16];
              puVar7[0x17] = puVar18[0x17];
              puVar7[0x18] = (float **********)puVar18[0x18];
              puVar7[0x19] = puVar18[0x19];
              puVar7[0x1a] = puVar18[0x1a];
              puVar7[0x1b] = puVar18[0x1b];
              puVar7[0x1c] = puVar18[0x1c];
              puVar7[0x1d] = puVar18[0x1d];
              puVar7[0x1e] = puVar18[0x1e];
              puVar7[0x1f] = puVar18[0x1f];
              puVar7[0x20] = puVar18[0x20];
              puVar7[0x21] = puVar18[0x21];
              puVar7[0x22] = puVar18[0x22];
              puVar7[0x23] = puVar18[0x23];
              *(undefined1 *)(puVar7 + 0x24) = *(undefined1 *)(puVar18 + 0x24);
              *(undefined1 *)((int)puVar7 + 0x91) = *(undefined1 *)((int)puVar18 + 0x91);
              *(undefined1 *)((int)puVar7 + 0x92) = *(undefined1 *)((int)puVar18 + 0x92);
              *(undefined1 *)((int)puVar7 + 0x93) = *(undefined1 *)((int)puVar18 + 0x93);
              *(undefined1 *)(puVar7 + 0x25) = *(undefined1 *)(puVar18 + 0x25);
              *(undefined1 *)((int)puVar7 + 0x95) = *(undefined1 *)((int)puVar18 + 0x95);
              *(undefined1 *)((int)puVar7 + 0x96) = *(undefined1 *)((int)puVar18 + 0x96);
              *(undefined1 *)((int)puVar7 + 0x97) = *(undefined1 *)((int)puVar18 + 0x97);
              *(undefined1 *)(puVar7 + 0x26) = *(undefined1 *)(puVar18 + 0x26);
            }
            iVar21 = *puVar26 * 0x9c + -0x9c + puVar26[2];
          }
          uVar36 = uVar36 + 1;
        } while ((int)uVar36 < (int)*puVar4);
      }
      uVar16 = *puVar26;
      local_1f08 = (float ***********)0x0;
      uVar36 = 1;
      if (1 < (int)uVar16) {
        pfVar39 = (float *)puVar26[2];
        fVar55 = ABS(DAT_001f0618);
        do {
          pfVar8 = pfVar39;
          if (uVar36 - 1 < uVar16) {
            pfVar8 = pfVar39 + (uVar36 - 1) * 0x27;
          }
          uVar43 = uVar36;
          if (uVar36 < uVar16) {
            uVar43 = uVar36 * 0x27;
          }
          fVar57 = pfVar8[7];
          pfVar32 = pfVar39;
          if (uVar36 < uVar16) {
            pfVar32 = pfVar39 + uVar43;
          }
          iVar21 = (int)ALBW_AS_U32(fVar57) - (int)ALBW_AS_U32(fVar55);
          if ((int)ALBW_AS_U32(fVar57) <= (int)ALBW_AS_U32(fVar55)) {
            fVar57 = pfVar32[7];
            iVar21 = (int)ALBW_AS_U32(fVar57) - (int)ALBW_AS_U32(fVar55);
          }
          if (fVar57 == fVar55 || iVar21 < 0 != SBORROW4((int)ALBW_AS_U32(fVar57),(int)ALBW_AS_U32(fVar55))) break;
          if ((uint)ALBW_AS_U32(DAT_001f0618) <
              (uint)ALBW_AS_U32((*pfVar8 * pfVar32[6] + pfVar8[1] * pfVar32[7] + pfVar8[2] * pfVar32[8])))
          goto LAB_001ef6fc;
          uVar36 = uVar36 + 1;
        } while ((int)uVar36 < (int)uVar16);
      }
      if (((int)uVar36 < (int)uVar16) && (uVar43 = uVar16 - 1, (int)uVar36 <= (int)uVar43)) {
        pfVar39 = (float *)puVar26[2];
        do {
          pfVar8 = pfVar39;
          if (uVar43 - 1 < uVar16) {
            pfVar8 = pfVar39 + (uVar43 - 1) * 0x27;
          }
          uVar31 = uVar43;
          if (uVar43 < uVar16) {
            uVar31 = uVar43 * 0x27;
          }
          fVar55 = pfVar8[7];
          pfVar32 = pfVar39;
          if (uVar43 < uVar16) {
            pfVar32 = pfVar39 + uVar31;
          }
          iVar21 = (int)ALBW_AS_U32(fVar55) - (int)ALBW_AS_U32(DAT_001eee30);
          if ((int)ALBW_AS_U32(fVar55) <= (int)ALBW_AS_U32(DAT_001eee30)) {
            fVar55 = pfVar32[7];
            iVar21 = (int)ALBW_AS_U32(fVar55) - (int)ALBW_AS_U32(DAT_001eee30);
          }
          if (fVar55 == DAT_001eee30 || iVar21 < 0 != SBORROW4((int)ALBW_AS_U32(fVar55),(int)ALBW_AS_U32(DAT_001eee30)))
          break;
          if (DAT_001f061c <
              (float *******)
              ALBW_PTR_BITS((*pfVar8 * pfVar32[6] + pfVar8[1] * pfVar32[7] + pfVar8[2] * pfVar32[8])))
          goto LAB_001ef6fc;
          uVar43 = uVar43 - 1;
        } while ((int)uVar36 <= (int)uVar43);
      }
      goto LAB_001ef7b0;
    }
    goto LAB_001efa3c;
  }
  goto LAB_001fc7a8;
LAB_001ef6fc:
  local_1f08 = (float ***********)0x1;
LAB_001ef7b0:
  uVar36 = 0;
  if (0 < (int)*DAT_001ef3ac) {
    do {
      puVar4 = DAT_001ef3ac;
      if (uVar36 < *DAT_001ef3ac) {
        uVar16 = DAT_001ef3ac[2] + uVar36 * 0x9c;
      }
      else {
        uVar16 = DAT_001ef3ac[2];
      }
      *(undefined1 *)(uVar16 + 0x93) = 1;
      if (uVar36 < *puVar4) {
        puVar18 = (undefined4 *)(puVar4[2] + uVar36 * 0x9c);
      }
      else {
        puVar18 = (undefined4 *)puVar4[2];
      }
      ppppppppppfVar22 = *pppppppppppfVar17;
      if ((int)ppppppppppfVar22 < (int)pppppppppppfVar74[0x2a]) {
        *pppppppppppfVar17 = (float **********)((int)ppppppppppfVar22 + 1);
        ppppppppppfVar22 = pppppppppppfVar74[0x2b] + (int)ppppppppppfVar22 * 0x27;
        *ppppppppppfVar22 = (float *********)*puVar18;
        local_1f0c = (float ***********)(ppppppppppfVar22 + 9);
        ppppppppppfVar22[1] = (float *********)puVar18[1];
        local_1ebc = (float ***********)(puVar18 + 0xf);
        ppppppppppfVar22[2] = (float *********)puVar18[2];
        ppppppppppfVar22[3] = (float *********)puVar18[3];
        ppppppppppfVar22[4] = (float *********)puVar18[4];
        local_1eb4 = (float ************)(puVar18 + 0x12);
        ppppppppppfVar22[5] = (float *********)puVar18[5];
        ppppppppppfVar22[6] = (float *********)puVar18[6];
        ppppppppppfVar22[7] = (float *********)puVar18[7];
        ppppppppppfVar22[8] = (float *********)puVar18[8];
        ppppppppppfVar22[9] = (float *********)puVar18[9];
        ppppppppppfVar22[10] = (float *********)puVar18[10];
        ppppppppppfVar22[0xb] = (float *********)puVar18[0xb];
        ppppppppppfVar22[0xc] = (float *********)puVar18[0xc];
        ppppppppppfVar22[0xd] = (float *********)puVar18[0xd];
        ppppppppppfVar22[0xe] = (float *********)puVar18[0xe];
        ppppppppppfVar22[0xf] = (float *********)puVar18[0xf];
        ppppppppppfVar22[0x10] = (float *********)puVar18[0x10];
        ppppppppppfVar22[0x11] = (float *********)puVar18[0x11];
        ppppppppppfVar22[0x12] = (float *********)puVar18[0x12];
        ppppppppppfVar22[0x13] = (float *********)puVar18[0x13];
        ppppppppppfVar22[0x14] = (float *********)puVar18[0x14];
        ppppppppppfVar22[0x15] = (float *********)puVar18[0x15];
        ppppppppppfVar22[0x16] = (float *********)puVar18[0x16];
        ppppppppppfVar22[0x17] = (float *********)puVar18[0x17];
        ppppppppppfVar22[0x18] = (float *********)puVar18[0x18];
        ppppppppppfVar22[0x19] = (float *********)puVar18[0x19];
        ppppppppppfVar22[0x1a] = (float *********)puVar18[0x1a];
        ppppppppppfVar22[0x1b] = (float *********)puVar18[0x1b];
        ppppppppppfVar22[0x1c] = (float *********)puVar18[0x1c];
        ppppppppppfVar22[0x1d] = (float *********)puVar18[0x1d];
        ppppppppppfVar22[0x1e] = (float *********)puVar18[0x1e];
        ppppppppppfVar22[0x1f] = (float *********)puVar18[0x1f];
        ppppppppppfVar22[0x20] = (float *********)puVar18[0x20];
        ppppppppppfVar22[0x21] = (float *********)puVar18[0x21];
        ppppppppppfVar22[0x22] = (float *********)puVar18[0x22];
        ppppppppppfVar22[0x23] = (float *********)puVar18[0x23];
        *(undefined1 *)(ppppppppppfVar22 + 0x24) = *(undefined1 *)(puVar18 + 0x24);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x91) = *(undefined1 *)((int)puVar18 + 0x91);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x92) = *(undefined1 *)((int)puVar18 + 0x92);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x93) = *(undefined1 *)((int)puVar18 + 0x93);
        *(undefined1 *)(ppppppppppfVar22 + 0x25) = *(undefined1 *)(puVar18 + 0x25);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = *(undefined1 *)((int)puVar18 + 0x95);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x96) = *(undefined1 *)((int)puVar18 + 0x96);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x97) = *(undefined1 *)((int)puVar18 + 0x97);
        *(undefined1 *)(ppppppppppfVar22 + 0x26) = *(undefined1 *)(puVar18 + 0x26);
      }
      uVar36 = uVar36 + 1;
    } while ((int)uVar36 < (int)*DAT_001ef3ac);
  }
  if (local_1f08 != (float ***********)0x0) {
    local_1e30 = (float ***********)0x1;
  }
LAB_001efa3c:
  pppppppppppfVar49 = local_1e88;
  fVar55 = ALBW_AS_F32(pppppppppppfVar15) + fVar5;
  fVar57 = ALBW_AS_F32(uVar72) + ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
  if (((*DAT_001f0620 & 1) == 0) &&
     (iVar21 = func_00100174((int *)DAT_001f0620), puVar4 = DAT_001f0624, iVar21 != 0)) {
    puVar26 = DAT_001f0624 + 3;
    *DAT_001f0624 = 0;
    puVar4[1] = 0;
    puVar4[2] = 0;
    func_0010B3EC(puVar4,0x20,(int)puVar26);
  }
  puVar4 = DAT_001f0624;
  *DAT_001f0624 = 0;
  ppppppppppfVar22 = (float **********)0x0;
  fVar5 = ALBW_AS_F32(pppppppppppfVar81) / (fVar5 - fVar55);
  fVar60 = ALBW_AS_F32(uVar67);
  local_1f14 = (float ***********)ALBW_PTR_BITS((fVar5 * fVar60));
  local_1f28[3] = (float ***********)ALBW_PTR_BITS((fVar60 + fVar55 * fVar5 * ALBW_AS_F32(pppppppppppfVar53)));
  fVar5 = ALBW_AS_F32(pppppppppppfVar81) / (ALBW_AS_F32(((ulonglong)uVar67 >> 0x20)) - fVar57);
  local_1f28[4] = (float ***********)ALBW_PTR_BITS((fVar5 * ALBW_AS_F32(pppppppppppfVar53)));
  local_1f28[2] = (float ***********)ALBW_PTR_BITS((fVar60 + fVar57 * fVar5 * fVar60));
  pppppppppppfVar74 = pppppppppppfVar81;
  if (0 < (int)*pppppppppppfVar49) {
    do {
      if (ppppppppppfVar22 < *pppppppppppfVar49) {
        ppppppppppfVar56 = pppppppppppfVar49[2] + (int)ppppppppppfVar22 * 0x27;
      }
      else {
        ppppppppppfVar56 = pppppppppppfVar49[2];
      }
      ppppppppppfVar56[0x1c] =
           (float *********)
           ALBW_PTR_BITS((ALBW_AS_F32(local_1f28[*(char *)(ppppppppppfVar56 + 0x24) + 2]) +
           ALBW_AS_F32(local_1f28[(int)*(char *)(ppppppppppfVar56 + 0x24) + 4]) *
           ALBW_AS_F32(ppppppppppfVar56[0x1c])));
      pppppppppfVar48 =
           (float *********)
           ALBW_PTR_BITS((ALBW_AS_F32(local_1f28[*(char *)(ppppppppppfVar56 + 0x24) + 2]) +
           ALBW_AS_F32(local_1f28[(int)*(char *)(ppppppppppfVar56 + 0x24) + 4]) *
           ALBW_AS_F32(ppppppppppfVar56[0x1d])));
      ppppppppppfVar56[0x1d] = pppppppppfVar48;
      if (ALBW_AS_F32(pppppppppfVar48) < ALBW_AS_F32(ppppppppppfVar56[0x1c])) {
        pppppppppfVar48 = ppppppppppfVar56[9];
        pppppppppfVar23 = ppppppppppfVar56[10];
        pppppppppfVar24 = ppppppppppfVar56[0xb];
        ppppppppppfVar56[9] = ppppppppppfVar56[0xc];
        ppppppppppfVar56[10] = ppppppppppfVar56[0xd];
        ppppppppppfVar56[0xb] = ppppppppppfVar56[0xe];
        ppppppppppfVar56[0xc] = pppppppppfVar48;
        ppppppppppfVar56[0xd] = pppppppppfVar23;
        ppppppppppfVar56[0xe] = pppppppppfVar24;
        pppppppppfVar48 = ppppppppppfVar56[0x1c];
        ppppppppppfVar56[0x1c] = ppppppppppfVar56[0x1d];
        ppppppppppfVar56[0x1d] = pppppppppfVar48;
      }
      ppppppppppfVar56[0xf] = ppppppppppfVar56[9];
      ppppppppppfVar56[0x10] = ppppppppppfVar56[10];
      ppppppppppfVar56[0x11] = ppppppppppfVar56[0xb];
      ppppppppppfVar56[0x12] = ppppppppppfVar56[0xc];
      ppppppppppfVar56[0x13] = ppppppppppfVar56[0xd];
      ppppppppppfVar56[0x14] = ppppppppppfVar56[0xe];
      *(undefined1 *)((int)ppppppppppfVar56 + 0x92) = 0;
      *(undefined1 *)((int)ppppppppppfVar56 + 0x91) = 0;
      ppppppppppfVar56[0x1b] = (float *********)((ulonglong)uVar72 >> 0x20);
      *(undefined1 *)(ppppppppppfVar56 + 0x26) = 0;
      *(undefined1 *)((int)ppppppppppfVar56 + 0x97) = 0;
      *(undefined1 *)((int)ppppppppppfVar56 + 0x95) = 0;
      *(undefined1 *)(ppppppppppfVar56 + 0x25) = 0;
      if (*(char *)(ppppppppppfVar56 + 0x24) == '\0') {
        func_00115D70((float *)ppppppppppfVar56,(float *)ppppppppppfVar56,fVar58);
        func_00115D70((float *)(ppppppppppfVar56 + 3),(float *)(ppppppppppfVar56 + 3),fVar58);
      }
      ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
    } while ((int)ppppppppppfVar22 < (int)*pppppppppppfVar49);
  }
  ppppppppppfVar22 = (float **********)0x0;
  if (0 < (int)*pppppppppppfVar49) {
    do {
      puVar26 = DAT_001f0624;
      ppppppppppfVar73 = *pppppppppppfVar49;
      ppppppppppfVar56 = pppppppppppfVar49[2];
      if (ppppppppppfVar22 < ppppppppppfVar73) {
        ppppppppppfVar56 = ppppppppppfVar56 + (int)ppppppppppfVar22 * 0x27;
      }
      ppppppppppfVar30 = pppppppppppfVar49[2];
      if (ppppppppppfVar22 < ppppppppppfVar73) {
        ppppppppppfVar30 = ppppppppppfVar30 + (int)ppppppppppfVar22 * 0x27;
      }
      if (WALK_SEG_HAS_LENGTH(ppppppppppfVar56, ppppppppppfVar30)) {
        if (ppppppppppfVar22 < ppppppppppfVar73) {
          ppppppppppfVar56 = pppppppppppfVar49[2] + (int)ppppppppppfVar22 * 0x27;
        }
        else {
          ppppppppppfVar56 = pppppppppppfVar49[2];
        }
        if ((int)*DAT_001f0624 < (int)DAT_001f0624[1]) {
          *(float ***********)(DAT_001f0624[2] + *DAT_001f0624 * 4) = ppppppppppfVar56;
          *puVar26 = *puVar26 + 1;
        }
      }
      ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
    } while ((int)ppppppppppfVar22 < (int)*pppppppppppfVar49);
  }
  *pppppppppppfVar49 = (float **********)0x0;
  if (*puVar4 != 0) {
    func_0048C258((int *)DAT_001f0624,DAT_001ef3a4);
    if (((*DAT_001f0628 & 1) == 0) &&
       (iVar21 = func_00100174((int *)DAT_001f0628), puVar26 = DAT_001f062c, iVar21 != 0)) {
      puVar27 = DAT_001f062c + 3;
      *DAT_001f062c = 0;
      puVar26[1] = 0x30;
      puVar26[2] = (uint)puVar27;
      func_00100118((int)(puVar26 + 3),DAT_001ef3b0,0x9c,0x30);
    }
    puVar26 = DAT_001f062c;
    *DAT_001f062c = 0;
    if (*puVar4 == 0) {
      puVar18 = (undefined4 *)0x0;
    }
    else {
      puVar18 = *(undefined4 **)puVar4[2];
    }
    if (0 < (int)puVar26[1]) {
      *puVar26 = 1;
      func_001FE514((undefined4 *)puVar26[2],puVar18);
    }
    iVar21 = *puVar26 * 0x9c + -0x9c + puVar26[2];
    uVar36 = 1;
    if (1 < (int)*puVar4) {
      do {
        uVar43 = *puVar4;
        uVar16 = uVar43;
        if (uVar36 < uVar43) {
          uVar16 = puVar4[2];
        }
        if (uVar36 < uVar43) {
          puVar18 = *(undefined4 **)(uVar16 + uVar36 * 4);
        }
        else {
          puVar18 = (undefined4 *)0x0;
        }
        fVar5 = *(float *)(iVar21 + 0x18) * ALBW_AS_F32(puVar18[6]) +
                *(float *)(iVar21 + 0x1c) * ALBW_AS_F32(puVar18[7]) +
                *(float *)(iVar21 + 0x20) * ALBW_AS_F32(puVar18[8]);
        if (fVar5 <= ALBW_AS_F32(((ulonglong)uVar72 >> 0x20))) {
          fVar5 = -fVar5;
        }
        if (DAT_001ef3b4 < (int)ALBW_AS_U32(fVar5)) {
          if (*(float *)(iVar21 + 0x74) < ALBW_AS_F32(puVar18[0x1d])) {
            *(undefined4 *)(iVar21 + 0x30) = puVar18[0xc];
            *(undefined4 *)(iVar21 + 0x34) = puVar18[0xd];
            *(undefined4 *)(iVar21 + 0x38) = puVar18[0xe];
            *(undefined4 *)(iVar21 + 0x74) = puVar18[0x1d];
            *(undefined4 *)(iVar21 + 0x48) = puVar18[0x12];
            *(undefined4 *)(iVar21 + 0x4c) = puVar18[0x13];
            *(undefined4 *)(iVar21 + 0x50) = puVar18[0x14];
          }
        }
        else {
          uVar16 = *puVar26;
          if ((int)uVar16 < (int)puVar26[1]) {
            *puVar26 = uVar16 + 1;
            local_1eb4 = (float ************)(puVar18 + 6);
            local_1ec0 = (float ************)(puVar18 + 9);
            puVar7 = (undefined4 *)(puVar26[2] + uVar16 * 0x9c);
            *puVar7 = *puVar18;
            local_1f04 = (float ***********)(puVar7 + 0xf);
            puVar7[1] = puVar18[1];
            puVar7[2] = puVar18[2];
            puVar7[3] = puVar18[3];
            puVar7[4] = puVar18[4];
            puVar7[5] = puVar18[5];
            puVar7[6] = *local_1eb4;
            puVar7[7] = puVar18[7];
            puVar7[8] = puVar18[8];
            puVar7[9] = *local_1ec0;
            puVar7[10] = puVar18[10];
            puVar7[0xb] = puVar18[0xb];
            puVar7[0xc] = puVar18[0xc];
            puVar7[0xd] = puVar18[0xd];
            puVar7[0xe] = puVar18[0xe];
            *local_1f04 = (float **********)puVar18[0xf];
            puVar7[0x10] = puVar18[0x10];
            puVar7[0x11] = puVar18[0x11];
            puVar7[0x12] = puVar18[0x12];
            puVar7[0x13] = puVar18[0x13];
            puVar7[0x14] = puVar18[0x14];
            puVar7[0x15] = puVar18[0x15];
            puVar7[0x16] = puVar18[0x16];
            puVar7[0x17] = puVar18[0x17];
            puVar7[0x18] = puVar18[0x18];
            puVar7[0x19] = puVar18[0x19];
            puVar7[0x1a] = puVar18[0x1a];
            puVar7[0x1b] = puVar18[0x1b];
            puVar7[0x1c] = puVar18[0x1c];
            puVar7[0x1d] = puVar18[0x1d];
            puVar7[0x1e] = puVar18[0x1e];
            puVar7[0x1f] = puVar18[0x1f];
            puVar7[0x20] = puVar18[0x20];
            puVar7[0x21] = puVar18[0x21];
            puVar7[0x22] = puVar18[0x22];
            puVar7[0x23] = puVar18[0x23];
            *(undefined1 *)(puVar7 + 0x24) = *(undefined1 *)(puVar18 + 0x24);
            *(undefined1 *)((int)puVar7 + 0x91) = *(undefined1 *)((int)puVar18 + 0x91);
            *(undefined1 *)((int)puVar7 + 0x92) = *(undefined1 *)((int)puVar18 + 0x92);
            *(undefined1 *)((int)puVar7 + 0x93) = *(undefined1 *)((int)puVar18 + 0x93);
            *(undefined1 *)(puVar7 + 0x25) = *(undefined1 *)(puVar18 + 0x25);
            *(undefined1 *)((int)puVar7 + 0x95) = *(undefined1 *)((int)puVar18 + 0x95);
            *(undefined1 *)((int)puVar7 + 0x96) = *(undefined1 *)((int)puVar18 + 0x96);
            *(undefined1 *)((int)puVar7 + 0x97) = *(undefined1 *)((int)puVar18 + 0x97);
            *(undefined1 *)(puVar7 + 0x26) = *(undefined1 *)(puVar18 + 0x26);
          }
          iVar21 = *puVar26 * 0x9c + -0x9c + puVar26[2];
        }
        uVar36 = uVar36 + 1;
      } while ((int)uVar36 < (int)*puVar4);
    }
    uVar36 = *puVar26;
    local_1f28[1] = (float ***********)0x0;
    uVar16 = 1;
    if (1 < (int)uVar36) {
      pfVar39 = (float *)puVar26[2];
      do {
        pfVar8 = pfVar39;
        if (uVar16 - 1 < uVar36) {
          pfVar8 = pfVar39 + (uVar16 - 1) * 0x27;
        }
        pfVar32 = pfVar39;
        if (uVar16 < uVar36) {
          pfVar32 = pfVar39 + uVar16 * 0x27;
        }
        pppppfVar41 = (float *****)ALBW_PTR_BITS(pfVar8[7]);
        iVar21 = (int)pppppfVar41 - (int)DAT_001f0630;
        if ((int)pppppfVar41 <= (int)DAT_001f0630) {
          pppppfVar41 = (float *****)ALBW_PTR_BITS(pfVar32[7]);
          iVar21 = (int)pppppfVar41 - (int)DAT_001f0630;
        }
        if (pppppfVar41 == DAT_001f0630 ||
            iVar21 < 0 != SBORROW4((int)pppppfVar41,(int)DAT_001f0630)) break;
        if ((int)DAT_001f0630 <
            (int)ALBW_AS_U32((*pfVar8 * pfVar32[6] + pfVar8[1] * pfVar32[7] + pfVar8[2] * pfVar32[8])))
        goto LAB_001f0154;
        uVar16 = uVar16 + 1;
      } while ((int)uVar16 < (int)uVar36);
    }
    if (((int)uVar16 < (int)uVar36) && (uVar43 = uVar36 - 1, (int)uVar16 <= (int)uVar43)) {
      pfVar39 = (float *)puVar26[2];
      do {
        pfVar8 = pfVar39;
        if (uVar43 - 1 < uVar36) {
          pfVar8 = pfVar39 + (uVar43 - 1) * 0x27;
        }
        pfVar32 = pfVar39;
        if (uVar43 < uVar36) {
          pfVar32 = pfVar39 + uVar43 * 0x27;
        }
        pppppfVar41 = (float *****)ALBW_PTR_BITS(pfVar8[7]);
        iVar21 = (int)pppppfVar41 - (int)DAT_001f0630;
        if ((int)pppppfVar41 <= (int)DAT_001f0630) {
          pppppfVar41 = (float *****)ALBW_PTR_BITS(pfVar32[7]);
          iVar21 = (int)pppppfVar41 - (int)DAT_001f0630;
        }
        if (pppppfVar41 == DAT_001f0630 ||
            iVar21 < 0 != SBORROW4((int)pppppfVar41,(int)DAT_001f0630)) break;
        if (DAT_001f0634 <
            (int)ALBW_AS_U32((*pfVar8 * pfVar32[6] + pfVar8[1] * pfVar32[7] + pfVar8[2] * pfVar32[8])))
        goto LAB_001f0154;
        uVar43 = uVar43 - 1;
      } while ((int)uVar16 <= (int)uVar43);
    }
    goto LAB_001f0208;
  }
  goto LAB_001f0488;
LAB_001f0154:
  local_1f28[1] = (float ***********)0x1;
LAB_001f0208:
  uVar36 = 0;
  if (0 < (int)*DAT_001f062c) {
    do {
      puVar4 = DAT_001f062c;
      if (uVar36 < *DAT_001f062c) {
        uVar16 = DAT_001f062c[2] + uVar36 * 0x9c;
      }
      else {
        uVar16 = DAT_001f062c[2];
      }
      *(undefined1 *)(uVar16 + 0x93) = 0;
      puVar18 = (undefined4 *)puVar4[2];
      if (uVar36 < *puVar4) {
        puVar18 = puVar18 + uVar36 * 0x27;
      }
      ppppppppppfVar22 = *pppppppppppfVar49;
      if ((int)ppppppppppfVar22 < (int)pppppppppppfVar49[1]) {
        *pppppppppppfVar49 = (float **********)((int)ppppppppppfVar22 + 1);
        ppppppppppfVar22 = pppppppppppfVar49[2] + (int)ppppppppppfVar22 * 0x27;
        *ppppppppppfVar22 = (float *********)*puVar18;
        ppppppppppfVar22[1] = (float *********)puVar18[1];
        local_1f0c = (float ***********)(puVar18 + 0xf);
        ppppppppppfVar22[2] = (float *********)puVar18[2];
        local_1ec4 = (float ************)(ppppppppppfVar22 + 0xf);
        ppppppppppfVar22[3] = (float *********)puVar18[3];
        ppppppppppfVar22[4] = (float *********)puVar18[4];
        ppppppppppfVar22[5] = (float *********)puVar18[5];
        ppppppppppfVar22[6] = (float *********)puVar18[6];
        ppppppppppfVar22[7] = (float *********)puVar18[7];
        ppppppppppfVar22[8] = (float *********)puVar18[8];
        ppppppppppfVar22[9] = (float *********)puVar18[9];
        ppppppppppfVar22[10] = (float *********)puVar18[10];
        ppppppppppfVar22[0xb] = (float *********)puVar18[0xb];
        ppppppppppfVar22[0xc] = (float *********)puVar18[0xc];
        ppppppppppfVar22[0xd] = (float *********)puVar18[0xd];
        ppppppppppfVar22[0xe] = (float *********)puVar18[0xe];
        *local_1ec4 = (float ***********)*local_1f0c;
        ppppppppppfVar22[0x10] = (float *********)puVar18[0x10];
        ppppppppppfVar22[0x11] = (float *********)puVar18[0x11];
        ppppppppppfVar22[0x12] = (float *********)puVar18[0x12];
        ppppppppppfVar22[0x13] = (float *********)puVar18[0x13];
        ppppppppppfVar22[0x14] = (float *********)puVar18[0x14];
        ppppppppppfVar22[0x15] = (float *********)puVar18[0x15];
        ppppppppppfVar22[0x16] = (float *********)puVar18[0x16];
        ppppppppppfVar22[0x17] = (float *********)puVar18[0x17];
        ppppppppppfVar22[0x18] = (float *********)puVar18[0x18];
        ppppppppppfVar22[0x19] = (float *********)puVar18[0x19];
        ppppppppppfVar22[0x1a] = (float *********)puVar18[0x1a];
        ppppppppppfVar22[0x1b] = (float *********)puVar18[0x1b];
        ppppppppppfVar22[0x1c] = (float *********)puVar18[0x1c];
        ppppppppppfVar22[0x1d] = (float *********)puVar18[0x1d];
        ppppppppppfVar22[0x1e] = (float *********)puVar18[0x1e];
        ppppppppppfVar22[0x1f] = (float *********)puVar18[0x1f];
        ppppppppppfVar22[0x20] = (float *********)puVar18[0x20];
        ppppppppppfVar22[0x21] = (float *********)puVar18[0x21];
        ppppppppppfVar22[0x22] = (float *********)puVar18[0x22];
        ppppppppppfVar22[0x23] = (float *********)puVar18[0x23];
        *(undefined1 *)(ppppppppppfVar22 + 0x24) = *(undefined1 *)(puVar18 + 0x24);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x91) = *(undefined1 *)((int)puVar18 + 0x91);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x92) = *(undefined1 *)((int)puVar18 + 0x92);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x93) = *(undefined1 *)((int)puVar18 + 0x93);
        *(undefined1 *)(ppppppppppfVar22 + 0x25) = *(undefined1 *)(puVar18 + 0x25);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = *(undefined1 *)((int)puVar18 + 0x95);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x96) = *(undefined1 *)((int)puVar18 + 0x96);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x97) = *(undefined1 *)((int)puVar18 + 0x97);
        *(undefined1 *)(ppppppppppfVar22 + 0x26) = *(undefined1 *)(puVar18 + 0x26);
      }
      uVar36 = uVar36 + 1;
    } while ((int)uVar36 < (int)*DAT_001f062c);
  }
  if (local_1f28[1] != (float ***********)0x0) {
    local_1e30 = (float ***********)((uint)local_1e30 | 2);
  }
LAB_001f0488:
  pppppppppppfVar15 = local_1e88;
  local_1f00[1] = local_1e30;
  local_1f10 = pppppppppppfVar78 + 0x29;
  local_1f00[0] = pppppppppppfVar78;
  if (((*DAT_001f0638 & 1) == 0) &&
     (iVar21 = func_00100174((int *)DAT_001f0638), puVar4 = DAT_001f063c, iVar21 != 0)) {
    puVar26 = DAT_001f063c + 3;
    *DAT_001f063c = 0;
    puVar4[1] = 0x30;
    puVar4[2] = (uint)puVar26;
    func_00100118((int)(puVar4 + 3),DAT_001f0640,0x9c,0x30);
  }
  pppppppppppfVar49 = (float ***********)0x0;
  *DAT_001f063c = 0;
  local_1f14 = (float ***********)0x0;
  if (0 < (int)*local_1f10) {
LAB_001f0528:
    iVar21 = DAT_001f0644;
    local_1f0c = local_1f00[1];
    if (local_1f14 < *local_1f10) {
      ppppppppppfVar22 = local_1f10[2] + (int)local_1f14 * 0x27;
    }
    else {
      ppppppppppfVar22 = local_1f10[2];
    }
    pppppppppppfVar34 = (float ***********)*pppppppppppfVar15;
    if ((int)pppppppppppfVar49 < (int)pppppppppppfVar34) {
      fVar5 = ALBW_AS_F32(((ulonglong)uVar72 >> 0x20));
      pppppppppppfVar9 = pppppppppppfVar49;
      do {
        ppppppppppfVar56 = pppppppppppfVar15[2];
        if (pppppppppppfVar9 < pppppppppppfVar34) {
          ppppppppppfVar56 = ppppppppppfVar56 + (int)pppppppppppfVar9 * 0x27;
        }
        fVar55 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(ppppppppppfVar56[6]) +
                 ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(ppppppppppfVar56[7]) +
                 ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(ppppppppppfVar56[8]);
        if (fVar55 <= fVar5) {
          fVar55 = -fVar55;
        }
        if (iVar21 < (int)ALBW_AS_U32(fVar55)) {
          func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar22 + 9),
                        (float *)(ppppppppppfVar56 + 9));
          fVar55 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1eb0) +
                   ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1eac) +
                   ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1ea8);
          if (fVar55 <= fVar5) {
            fVar55 = -fVar55;
          }
          if ((int)ALBW_AS_U32(fVar55) < DAT_001f0634) {
            local_1f28[1] = pppppppppppfVar9;
            if (-1 < (int)pppppppppppfVar9) goto joined_r0x001f07b0;
            break;
          }
        }
        pppppppppppfVar34 = (float ***********)*pppppppppppfVar15;
        pppppppppppfVar9 = (float ***********)((int)pppppppppppfVar9 + 1);
      } while ((int)pppppppppppfVar9 < (int)pppppppppppfVar34);
    }
    pppppppppfVar48 = (float *********)((ulonglong)uVar72 >> 0x20);
    ppppppppppfVar22[0x1f] = pppppppppfVar48;
    ppppppppppfVar22[0x1e] = pppppppppfVar48;
    func_00115D70((float *)&local_1e80,(float *)(ppppppppppfVar22 + 3),ALBW_AS_F32(local_1f00[0][0x1c]));
    local_1ec8 = local_1e80;
    local_1ec4 = (float ************)local_1e7c;
    local_1ec0 = (float ************)local_1e78;
    func_00115D88((float *)&local_1e8c,(float *)&local_1ec8,ALBW_AS_F32(ppppppppppfVar22[4]));
    local_1ebc = local_1e8c;
    local_1eb8 = local_1e88;
    local_1eb4 = (float ************)local_1e84;
    func_00115C54((float *)&local_1e98,(float *)(ppppppppppfVar22 + 9),(float *)&local_1ebc);
    local_1f0c = local_1e98;
    local_1f08 = local_1e94;
    local_1f04 = local_1e90;
    ppppppppppfVar22[0x15] = (float *********)local_1e98;
    ppppppppppfVar22[0x16] = (float *********)local_1e94;
    ppppppppppfVar22[0x17] = (float *********)local_1e90;
    func_00115D70((float *)&local_1e74,(float *)(ppppppppppfVar22 + 3),ALBW_AS_F32(local_1f00[0][0x1c]));
    local_1ec8 = local_1e74;
    local_1ec4 = (float ************)local_1e70;
    local_1ec0 = (float ************)local_1e6c;
    func_00115D88((float *)&local_1e64,(float *)&local_1ec8,ALBW_AS_F32(ppppppppppfVar22[4]));
    local_1f0c = local_1e64;
    local_1f08 = local_1e60;
    local_1f04 = local_1e5c;
    func_00115C54((float *)&local_1ea4,(float *)(ppppppppppfVar22 + 0xc),(float *)&local_1f0c);
    local_1ebc = local_1ea4;
    local_1eb8 = local_1ea0;
    local_1eb4 = (float ************)local_1e9c;
    ppppppppppfVar22[0x18] = (float *********)local_1ea4;
    ppppppppppfVar22[0x19] = (float *********)local_1ea0;
    ppppppppppfVar22[0x1a] = (float *********)local_1e9c;
    *(undefined1 *)(ppppppppppfVar22 + 0x26) = 1;
    uVar36 = *DAT_001f063c;
    if ((int)uVar36 < (int)DAT_001f063c[1]) {
      puVar4 = DAT_001f063c + 2;
      *DAT_001f063c = uVar36 + 1;
      func_001FE514((undefined4 *)(*puVar4 + uVar36 * 0x9c),ppppppppppfVar22);
    }
    goto LAB_001f0f9c;
  }
LAB_001f0fb8:
  ppppppppppfVar22 = *local_1f10;
  ppppppppppfVar56 = local_1f10[2];
  pppppppppppfVar34 = (float ***********)((int)ppppppppppfVar22 * 7);
  pppppppppppfVar9 = (float ***********)*pppppppppppfVar15;
  bVar47 = pppppppppppfVar49 <= pppppppppppfVar9;
  bVar44 = pppppppppppfVar9 == pppppppppppfVar49;
  if ((int)pppppppppppfVar49 < (int)pppppppppppfVar9) {
    do {
      ppppppppppfVar73 = pppppppppppfVar15[2];
      if (bVar47 && !bVar44) {
        ppppppppppfVar73 = ppppppppppfVar73 + (int)pppppppppppfVar49 * 0x27;
      }
      ppppppppppfVar73[0x1e] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -10];
      ppppppppppfVar73[0x1f] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -10];
      ppppppppppfVar73[0x15] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x15];
      ppppppppppfVar73[0x16] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x14];
      ppppppppppfVar73[0x17] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x13];
      ppppppppppfVar73[0x18] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x15];
      ppppppppppfVar73[0x19] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x14];
      ppppppppppfVar73[0x1a] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x13];
      *(undefined1 *)((int)ppppppppppfVar73 + 0x97) = 1;
      if (pppppppppppfVar49 < *pppppppppppfVar15) {
        ppppppppppfVar73 = pppppppppppfVar15[2] + (int)pppppppppppfVar49 * 0x27;
      }
      else {
        ppppppppppfVar73 = pppppppppppfVar15[2];
      }
      pppppppppppfVar34 = (float ***********)*DAT_001f063c;
      if ((int)pppppppppppfVar34 < (int)DAT_001f063c[1]) {
        puVar4 = DAT_001f063c + 2;
        *DAT_001f063c = (uint)((int)pppppppppppfVar34 + 1);
        puVar18 = (undefined4 *)(*puVar4 + (int)pppppppppppfVar34 * 0x9c);
        local_1f28[2] = (float ***********)(ppppppppppfVar73 + 0xc);
        *puVar18 = *ppppppppppfVar73;
        local_1f00[1] = (float ***********)(puVar18 + 0xc);
        puVar18[1] = ppppppppppfVar73[1];
        local_1f10 = (float ***********)(puVar18 + 0xf);
        puVar18[2] = ppppppppppfVar73[2];
        local_1ec4 = (float ************)(ppppppppppfVar73 + 0x12);
        puVar18[3] = ppppppppppfVar73[3];
        puVar18[4] = ppppppppppfVar73[4];
        local_1ec8 = (float ***********)(puVar18 + 0x12);
        puVar18[5] = ppppppppppfVar73[5];
        puVar18[6] = ppppppppppfVar73[6];
        puVar18[7] = ppppppppppfVar73[7];
        puVar18[8] = ppppppppppfVar73[8];
        puVar18[9] = ppppppppppfVar73[9];
        puVar18[10] = ppppppppppfVar73[10];
        puVar18[0xb] = ppppppppppfVar73[0xb];
        *local_1f00[1] = *local_1f28[2];
        puVar18[0xd] = ppppppppppfVar73[0xd];
        puVar18[0xe] = ppppppppppfVar73[0xe];
        *local_1f10 = (float **********)ppppppppppfVar73[0xf];
        puVar18[0x10] = ppppppppppfVar73[0x10];
        puVar18[0x11] = ppppppppppfVar73[0x11];
        *local_1ec8 = (float **********)*local_1ec4;
        puVar18[0x13] = ppppppppppfVar73[0x13];
        puVar18[0x14] = ppppppppppfVar73[0x14];
        puVar18[0x15] = ppppppppppfVar73[0x15];
        puVar18[0x16] = ppppppppppfVar73[0x16];
        puVar18[0x17] = ppppppppppfVar73[0x17];
        puVar18[0x18] = ppppppppppfVar73[0x18];
        puVar18[0x19] = ppppppppppfVar73[0x19];
        puVar18[0x1a] = ppppppppppfVar73[0x1a];
        puVar18[0x1b] = ppppppppppfVar73[0x1b];
        puVar18[0x1c] = ppppppppppfVar73[0x1c];
        puVar18[0x1d] = ppppppppppfVar73[0x1d];
        puVar18[0x1e] = ppppppppppfVar73[0x1e];
        puVar18[0x1f] = ppppppppppfVar73[0x1f];
        puVar18[0x20] = ppppppppppfVar73[0x20];
        puVar18[0x21] = ppppppppppfVar73[0x21];
        puVar18[0x22] = ppppppppppfVar73[0x22];
        puVar18[0x23] = ppppppppppfVar73[0x23];
        *(undefined1 *)(puVar18 + 0x24) = *(undefined1 *)(ppppppppppfVar73 + 0x24);
        *(undefined1 *)((int)puVar18 + 0x91) = *(undefined1 *)((int)ppppppppppfVar73 + 0x91);
        *(undefined1 *)((int)puVar18 + 0x92) = *(undefined1 *)((int)ppppppppppfVar73 + 0x92);
        *(undefined1 *)((int)puVar18 + 0x93) = *(undefined1 *)((int)ppppppppppfVar73 + 0x93);
        *(undefined1 *)(puVar18 + 0x25) = *(undefined1 *)(ppppppppppfVar73 + 0x25);
        *(undefined1 *)((int)puVar18 + 0x95) = *(undefined1 *)((int)ppppppppppfVar73 + 0x95);
        *(undefined1 *)((int)puVar18 + 0x96) = *(undefined1 *)((int)ppppppppppfVar73 + 0x96);
        pppppppppppfVar34 = (float ***********)(uint)*(byte *)((int)ppppppppppfVar73 + 0x97);
        *(byte *)((int)puVar18 + 0x97) = *(byte *)((int)ppppppppppfVar73 + 0x97);
        *(undefined1 *)(puVar18 + 0x26) = *(undefined1 *)(ppppppppppfVar73 + 0x26);
      }
      pppppppppppfVar9 = (float ***********)*pppppppppppfVar15;
      pppppppppppfVar49 = (float ***********)((int)pppppppppppfVar49 + 1);
      bVar47 = pppppppppppfVar49 <= pppppppppppfVar9;
      bVar44 = pppppppppppfVar9 == pppppppppppfVar49;
    } while (!bVar44 && (int)pppppppppppfVar49 <= (int)pppppppppppfVar9);
  }
  uVar36 = 0;
  if (getenv("ALBW_WALK_TRACE"))   /* WALKER debug: painting segments copied from the work list */
    fprintf(stderr, "walk: seg-copy A walker %p target %p work-count %d\n", (void *)local_30, (void *)local_1f00[0], (int)*DAT_001f063c);
  local_1f00[0][0x29] = (float **********)0x0;
  pppppppppppfVar15 = (float ***********)*DAT_001f063c;
  if (0 < (int)pppppppppppfVar15) {
    do {
      if (uVar36 < *DAT_001f063c) {
        pppppppppppfVar15 = (float ***********)(DAT_001f063c[2] + uVar36 * 0x9c);
      }
      else {
        pppppppppppfVar15 = (float ***********)DAT_001f063c[2];
      }
      pppppppppppfVar34 = (float ***********)local_1f00[0][0x29];
      if ((int)pppppppppppfVar34 < (int)local_1f00[0][0x2a]) {
        local_1f00[0][0x29] = (float **********)((int)pppppppppppfVar34 + 1);
        ppppppppppfVar22 = local_1f00[0][0x2b] + (int)pppppppppppfVar34 * 0x27;
        local_1f04 = pppppppppppfVar15 + 0xf;
        *ppppppppppfVar22 = (float *********)*pppppppppppfVar15;
        local_1f00[1] = (float ***********)(ppppppppppfVar22 + 0xf);
        ppppppppppfVar22[1] = (float *********)pppppppppppfVar15[1];
        ppppppppppfVar22[2] = (float *********)pppppppppppfVar15[2];
        ppppppppppfVar22[3] = (float *********)pppppppppppfVar15[3];
        ppppppppppfVar22[4] = (float *********)pppppppppppfVar15[4];
        ppppppppppfVar22[5] = (float *********)pppppppppppfVar15[5];
        ppppppppppfVar22[6] = (float *********)pppppppppppfVar15[6];
        ppppppppppfVar22[7] = (float *********)pppppppppppfVar15[7];
        ppppppppppfVar22[8] = (float *********)pppppppppppfVar15[8];
        ppppppppppfVar22[9] = (float *********)pppppppppppfVar15[9];
        ppppppppppfVar22[10] = (float *********)pppppppppppfVar15[10];
        ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar15[0xb];
        ppppppppppfVar22[0xc] = (float *********)pppppppppppfVar15[0xc];
        ppppppppppfVar22[0xd] = (float *********)pppppppppppfVar15[0xd];
        ppppppppppfVar22[0xe] = (float *********)pppppppppppfVar15[0xe];
        *local_1f00[1] = *local_1f04;
        ppppppppppfVar22[0x10] = (float *********)pppppppppppfVar15[0x10];
        ppppppppppfVar22[0x11] = (float *********)pppppppppppfVar15[0x11];
        ppppppppppfVar22[0x12] = (float *********)pppppppppppfVar15[0x12];
        ppppppppppfVar22[0x13] = (float *********)pppppppppppfVar15[0x13];
        ppppppppppfVar22[0x14] = (float *********)pppppppppppfVar15[0x14];
        ppppppppppfVar22[0x15] = (float *********)pppppppppppfVar15[0x15];
        ppppppppppfVar22[0x16] = (float *********)pppppppppppfVar15[0x16];
        ppppppppppfVar22[0x17] = (float *********)pppppppppppfVar15[0x17];
        ppppppppppfVar22[0x18] = (float *********)pppppppppppfVar15[0x18];
        ppppppppppfVar22[0x19] = (float *********)pppppppppppfVar15[0x19];
        ppppppppppfVar22[0x1a] = (float *********)pppppppppppfVar15[0x1a];
        ppppppppppfVar22[0x1b] = (float *********)pppppppppppfVar15[0x1b];
        ppppppppppfVar22[0x1c] = (float *********)pppppppppppfVar15[0x1c];
        ppppppppppfVar22[0x1d] = (float *********)pppppppppppfVar15[0x1d];
        ppppppppppfVar22[0x1e] = (float *********)pppppppppppfVar15[0x1e];
        ppppppppppfVar22[0x1f] = (float *********)pppppppppppfVar15[0x1f];
        ppppppppppfVar22[0x20] = (float *********)pppppppppppfVar15[0x20];
        ppppppppppfVar22[0x21] = (float *********)pppppppppppfVar15[0x21];
        ppppppppppfVar22[0x22] = (float *********)pppppppppppfVar15[0x22];
        ppppppppppfVar22[0x23] = (float *********)pppppppppppfVar15[0x23];
        *(char *)(ppppppppppfVar22 + 0x24) = *(char *)(pppppppppppfVar15 + 0x24);
        *(char *)((int)ppppppppppfVar22 + 0x91) = *(char *)((int)pppppppppppfVar15 + 0x91);
        *(char *)((int)ppppppppppfVar22 + 0x92) = *(char *)((int)pppppppppppfVar15 + 0x92);
        *(char *)((int)ppppppppppfVar22 + 0x93) = *(char *)((int)pppppppppppfVar15 + 0x93);
        *(char *)(ppppppppppfVar22 + 0x25) = *(char *)(pppppppppppfVar15 + 0x25);
        *(char *)((int)ppppppppppfVar22 + 0x95) = *(char *)((int)pppppppppppfVar15 + 0x95);
        *(char *)((int)ppppppppppfVar22 + 0x96) = *(char *)((int)pppppppppppfVar15 + 0x96);
        pppppppppppfVar34 = (float ***********)(uint)*(byte *)((int)pppppppppppfVar15 + 0x97);
        *(byte *)((int)ppppppppppfVar22 + 0x97) = *(byte *)((int)pppppppppppfVar15 + 0x97);
        pppppppppppfVar49 = pppppppppppfVar15 + 0x26;
        pppppppppppfVar15 = (float ***********)(uint)*(byte *)pppppppppppfVar49;
        *(byte *)(ppppppppppfVar22 + 0x26) = *(byte *)pppppppppppfVar49;
      }
      uVar36 = uVar36 + 1;
    } while ((int)uVar36 < (int)*DAT_001f063c);
  }
  pppppppppppfVar49 = pppppppppppfVar78;
  if (((uint)local_1e30 & 1) != 0) {
    fVar5 = ALBW_AS_F32(((ulonglong)uVar72 >> 0x20));
    ppppppppppfVar22 = (float **********)0x0;
    pppppppppppfVar75 = (float ***********)0x1;
    pppppppppppfVar17 = pppppppppppfVar78 + 0x29;
    pppppppppppfVar9 = (float ***********)*pppppppppppfVar17;
    local_1eb8 = (float ***********)0x0;
    local_1e54 = (float ***********)0x0;
    if (0 < (int)pppppppppppfVar9) {
      do {
        pppppppppppfVar15 = (float ***********)pppppppppppfVar78[0x2b];
        if (ppppppppppfVar22 < *pppppppppppfVar17) {
          pppppppppppfVar15 = pppppppppppfVar15 + (int)ppppppppppfVar22 * 0x27;
        }
        pppppppppppfVar34 = (float ***********)pppppppppppfVar78[0x2b];
        if (ppppppppppfVar22 < *pppppppppppfVar17) {
          pppppppppppfVar34 = pppppppppppfVar34 + (int)ppppppppppfVar22 * 0x27;
        }
        ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
        *(char *)(pppppppppppfVar34 + 0x25) = '\0';
        *(char *)((int)pppppppppppfVar15 + 0x95) = '\0';
      } while ((int)ppppppppppfVar22 < (int)pppppppppppfVar9);
    }
    if (1 < (int)pppppppppppfVar9) {
      do {
        pppppfVar41 = DAT_001f0630;
        pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar75 + -1);
        if (pppppppppppfVar15 < *pppppppppppfVar17) {
          pppppppppppfVar34 = (float ***********)((int)pppppppppppfVar15 * 7);
          ppppppppppfVar22 = pppppppppppfVar78[0x2b] + (int)pppppppppppfVar15 * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar78[0x2b];
        }
        if (pppppppppppfVar75 < *pppppppppppfVar17) {
          pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar75 * 0x27);
          ppppppppppfVar56 = pppppppppppfVar78[0x2b] + (int)pppppppppppfVar75 * 0x27;
        }
        else {
          ppppppppppfVar56 = pppppppppppfVar78[0x2b];
          pppppppppppfVar15 = pppppppppppfVar75;
        }
        if ((((int)ppppppppppfVar22[7] <= (int)DAT_001f0630) &&
            ((int)ppppppppppfVar56[7] <= (int)DAT_001f0630)) ||
           (pppppppppppfVar15 = (float ***********)DAT_001f061c,
           (float *******)
           ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar56[6]) +
            ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar56[7]) +
           ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar56[8]))) <= DAT_001f061c)) break;
        fVar55 = ALBW_AS_F32(pppppppppppfVar49[0x1c]) / ALBW_AS_F32(ppppppppppfVar22[4]);
        uVar67 = CONCAT44((int)((ulonglong)uVar72 >> 0x20),fVar55);
        if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
          pppppppppppfVar15 = (float ***********)(ppppppppppfVar22 + 0x18);
        }
        else {
          pppppppppppfVar15 = (float ***********)(ppppppppppfVar22 + 0xc);
        }
        fVar57 = ALBW_AS_F32(pppppppppppfVar49[0x1c]) / ALBW_AS_F32(ppppppppppfVar56[4]);
        local_1e84 = pppppppppppfVar15;
        func_00115D70((float *)&local_1e9c,(float *)(ppppppppppfVar22 + 3),fVar55);
        local_1f0c = local_1e9c;
        local_1f08 = local_1e98;
        local_1f04 = local_1e94;
        func_00115C54((float *)&local_1e00,(float *)pppppppppppfVar15,(float *)&local_1f0c);
        local_1e70 = local_1e00;
        local_1e6c = local_1dfc;
        local_1e68 = local_1df8;
        local_1ec8 = local_1e00;
        local_1ec4 = (float ************)local_1dfc;
        local_1ec0 = (float ************)local_1df8;
        if (*(char *)((int)ppppppppppfVar56 + 0x93) == '\0') {
          ppppppppppfVar73 = ppppppppppfVar56 + 0x15;
        }
        else {
          ppppppppppfVar73 = ppppppppppfVar56 + 9;
        }
        func_00115D70((float *)&local_1e9c,(float *)(ppppppppppfVar56 + 3),fVar57);
        local_1e70 = local_1e9c;
        local_1e6c = local_1e98;
        local_1e68 = local_1e94;
        pppppppppppfVar34 = local_1e94;
        func_00115C54((float *)&local_1df4,(float *)ppppppppppfVar73,(float *)&local_1e70);
        local_1f0c = local_1df4;
        local_1f08 = local_1df0;
        local_1f04 = local_1dec;
        local_1ea8 = local_1df4;
        local_1ea4 = local_1df0;
        local_1ea0 = local_1dec;
        fVar55 = ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar56[6]) +
                 ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar56[7]) +
                 ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar56[8]);
        func_00115C70((float *)&local_1e90,(float *)&local_1ec8,(float *)ppppppppppfVar73);
        fVar55 = (-(ALBW_AS_F32(ppppppppppfVar56[6]) * ALBW_AS_F32(local_1e90) +
                   ALBW_AS_F32(ppppppppppfVar56[7]) * ALBW_AS_F32(local_1e8c)) +
                 -(ALBW_AS_F32(ppppppppppfVar56[8]) * ALBW_AS_F32(local_1e88))) / fVar55;
        uVar70 = CONCAT44(fVar55,fVar5);
        if ((int)pppppfVar41 < (int)ALBW_AS_U32(fVar55)) {
          func_00115D70((float *)&local_1e4c,(float *)ppppppppppfVar22,fVar55);
          local_1e7c = local_1e4c;
          local_1e78 = local_1e48;
          local_1e74 = local_1e44;
          pppppppppppfVar34 = local_1e44;
          func_00115C54((float *)&local_1e18,(float *)&local_1ec8,(float *)&local_1e7c);
          ppppppppppfVar22[0x12] = (float *********)local_1e18;
          ppppppppppfVar22[0x13] = (float *********)local_1e14;
          ppppppppppfVar22[0x14] = (float *********)local_1e10;
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\x01') {
            func_00115D70((float *)&local_1e40,(float *)(ppppppppppfVar22 + 3),fVar58);
            local_1f10 = local_1e40;
            local_1f0c = local_1e3c;
            local_1f08 = local_1e38;
            func_00115D70((float *)&local_1e0c,(float *)&local_1f10,ALBW_AS_F32(uVar67));
            local_1e64 = local_1e0c;
            local_1e60 = local_1e08;
            local_1e5c = local_1e04;
            pppppppppppfVar34 = local_1e04;
            func_00115C54((float *)(ppppppppppfVar22 + 0x12),(float *)(ppppppppppfVar22 + 0x12),
                          (float *)&local_1e64);
          }
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
            func_00115C70((float *)&local_1e9c,(float *)(ppppppppppfVar22 + 0x12),
                          (float *)(ppppppppppfVar22 + 0xc));
            ppppppppppfVar22[0x1b] =
                 (float *********)
                 ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1e9c) +
                  ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1e98) +
                 ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1e94)));
          }
          uVar70 = (ulonglong)(uint)ALBW_AS_U32((ALBW_AS_F32(uVar70) + ALBW_AS_F32((uVar70 >> 0x20))));
          *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
        }
        fVar5 = ALBW_AS_F32(uVar70);
        uVar72 = CONCAT44((int)((ulonglong)uVar67 >> 0x20),
                          -(ALBW_AS_F32(*ppppppppppfVar56) * ALBW_AS_F32(ppppppppppfVar22[6]) +
                            ALBW_AS_F32(ppppppppppfVar56[1]) * ALBW_AS_F32(ppppppppppfVar22[7]) +
                           ALBW_AS_F32(ppppppppppfVar56[2]) * ALBW_AS_F32(ppppppppppfVar22[8])));
        func_00115C70((float *)&local_1f0c,(float *)&local_1ea8,(float *)local_1e84);
        fVar55 = (-(ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f0c) +
                   ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f08)) +
                 -(ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f04))) / ALBW_AS_F32(uVar72);
        pppppppppppfVar15 = extraout_r1;
        if ((int)pppppfVar41 < (int)ALBW_AS_U32(fVar55)) {
          func_00115D70((float *)&local_1e90,(float *)ppppppppppfVar56,fVar55);
          local_1e9c = local_1e90;
          local_1e98 = local_1e8c;
          local_1e94 = local_1e88;
          func_00115C70((float *)&local_1e90,(float *)&local_1ea8,(float *)&local_1e9c);
          pppppppppppfVar34 = (float ***********)&local_1e90;
          ppppppppppfVar56[0xf] = (float *********)local_1e90;
          ppppppppppfVar56[0x10] = (float *********)local_1e8c;
          ppppppppppfVar56[0x11] = (float *********)local_1e88;
          pppppppppppfVar15 = local_1e8c;
          if (*(char *)((int)ppppppppppfVar56 + 0x93) == '\x01') {
            func_00115D70((float *)&local_1e9c,(float *)(ppppppppppfVar56 + 3),fVar58);
            local_1f08 = local_1e9c;
            local_1f04 = local_1e98;
            local_1f00[0] = local_1e94;
            func_00115D70((float *)&local_1e9c,(float *)&local_1f08,fVar57);
            local_1e90 = local_1e9c;
            local_1e8c = local_1e98;
            local_1e88 = local_1e94;
            pppppppppppfVar34 = local_1e94;
            func_00115C54((float *)(ppppppppppfVar56 + 0xf),(float *)(ppppppppppfVar56 + 0xf),
                          (float *)&local_1e90);
            pppppppppppfVar15 = extraout_r1_00;
          }
          if (*(char *)((int)ppppppppppfVar56 + 0x93) == '\0') {
            func_00115C70((float *)&local_1f08,(float *)(ppppppppppfVar56 + 9),
                          (float *)(ppppppppppfVar56 + 0xf));
            ppppppppppfVar56[0x1b] =
                 (float *********)
                 ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar56) * ALBW_AS_F32(local_1f08) +
                  ALBW_AS_F32(ppppppppppfVar56[1]) * ALBW_AS_F32(local_1f04) +
                 ALBW_AS_F32(ppppppppppfVar56[2]) * ALBW_AS_F32(local_1f00[0])));
            pppppppppppfVar15 = extraout_r1_01;
          }
          fVar5 = fVar5 + fVar55;
          *(undefined1 *)(ppppppppppfVar56 + 0x25) = 1;
        }
        pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar75 + 1);
        local_1e54 = (float ***********)((int)local_1e54 + 1);
      } while ((int)pppppppppppfVar75 < (int)pppppppppppfVar9);
    }
    iVar21 = DAT_001f2904;
    pppppppppppfVar33 = pppppppppppfVar9;
    if ((int)pppppppppppfVar75 < (int)pppppppppppfVar9) {
      while (pppppppppppfVar29 = (float ***********)((int)pppppppppppfVar33 + -1),
            (void) iVar21, (int)pppppppppppfVar75 <= (int)pppppppppppfVar29) {
        pppppppppppfVar15 = (float ***********)*pppppppppppfVar17;
        if ((float ***********)((int)pppppppppppfVar33 + -2) < pppppppppppfVar15) {
          pppppppppppfVar34 = (float ***********)pppppppppppfVar78[0x2b];
          pppppppppppfVar33 = pppppppppppfVar34 + ((int)pppppppppppfVar33 + -2) * 0x27;
        }
        else {
          pppppppppppfVar33 = (float ***********)pppppppppppfVar78[0x2b];
        }
        if (pppppppppppfVar29 < pppppppppppfVar15) {
          pppppppppppfVar15 = (float ***********)pppppppppppfVar78[0x2b];
          pppppppppppfVar38 = pppppppppppfVar15 + (int)pppppppppppfVar29 * 0x27;
        }
        else {
          pppppppppppfVar38 = (float ***********)pppppppppppfVar78[0x2b];
        }
        if ((((int)pppppppppppfVar33[7] <= iVar21) && ((int)pppppppppppfVar38[7] <= iVar21)) ||
           (pppppppppppfVar15 = (float ***********)DAT_001f2908,
           (float ***********)
           ALBW_PTR_BITS((ALBW_AS_F32(*pppppppppppfVar33) * ALBW_AS_F32(pppppppppppfVar38[6]) +
            ALBW_AS_F32(pppppppppppfVar33[1]) * ALBW_AS_F32(pppppppppppfVar38[7]) +
           ALBW_AS_F32(pppppppppppfVar33[2]) * ALBW_AS_F32(pppppppppppfVar38[8]))) <= DAT_001f2908)) break;
        fVar55 = ALBW_AS_F32(pppppppppppfVar49[0x1c]) / ALBW_AS_F32(pppppppppppfVar33[4]);
        uVar67 = CONCAT44((int)((ulonglong)uVar72 >> 0x20),fVar55);
        if (*(char *)((int)pppppppppppfVar33 + 0x93) == '\0') {
          pppppppppppfVar15 = pppppppppppfVar33 + 0x18;
        }
        else {
          pppppppppppfVar15 = pppppppppppfVar33 + 0xc;
        }
        fVar57 = ALBW_AS_F32(pppppppppppfVar49[0x1c]) / ALBW_AS_F32(pppppppppppfVar38[4]);
        local_1e88 = pppppppppppfVar15;
        func_00115D70((float *)&local_1e80,(float *)(pppppppppppfVar33 + 3),fVar55);
        local_1ec8 = local_1e80;
        local_1ec4 = (float ************)local_1e7c;
        local_1ec0 = (float ************)local_1e78;
        func_00115C54((float *)&local_1df4,(float *)pppppppppppfVar15,(float *)&local_1ec8);
        local_1f10 = local_1df4;
        local_1f0c = local_1df0;
        local_1f08 = local_1dec;
        local_1ea8 = local_1df4;
        local_1ea4 = local_1df0;
        local_1ea0 = local_1dec;
        if (*(char *)((int)pppppppppppfVar38 + 0x93) == '\0') {
          pppppppppppfVar15 = pppppppppppfVar38 + 0x15;
        }
        else {
          pppppppppppfVar15 = pppppppppppfVar38 + 9;
        }
        local_1f00[0] = pppppppppppfVar15;
        func_00115D70((float *)&local_1e18,(float *)(pppppppppppfVar38 + 3),fVar57);
        pppppppppppfVar34 = (float ***********)&local_1f10;
        local_1f10 = local_1e18;
        local_1f0c = (float ***********)local_1e14;
        local_1f08 = (float ***********)local_1e10;
        func_00115C54((float *)local_1dd8,(float *)pppppppppppfVar15,(float *)&local_1f10);
        local_1ec8 = local_1dd8[0];
        local_1ec4 = (float ************)local_1dd8[1];
        local_1ec0 = (float ************)local_1dd8[2];
        local_1e9c = local_1dd8[0];
        local_1e98 = local_1dd8[1];
        local_1e94 = local_1dd8[2];
        fVar55 = ALBW_AS_F32(*pppppppppppfVar33) * ALBW_AS_F32(pppppppppppfVar38[6]) +
                 ALBW_AS_F32(pppppppppppfVar33[1]) * ALBW_AS_F32(pppppppppppfVar38[7]) +
                 ALBW_AS_F32(pppppppppppfVar33[2]) * ALBW_AS_F32(pppppppppppfVar38[8]);
        func_00115C70((float *)&local_1e0c,(float *)&local_1ea8,(float *)local_1f00[0]);
        fVar55 = -(ALBW_AS_F32(pppppppppppfVar38[6]) * ALBW_AS_F32(local_1e0c) +
                   ALBW_AS_F32(pppppppppppfVar38[7]) * ALBW_AS_F32(local_1e08) +
                  ALBW_AS_F32(pppppppppppfVar38[8]) * ALBW_AS_F32(local_1e04)) / fVar55;
        uVar70 = CONCAT44(fVar55,fVar5);
        if (iVar21 < (int)ALBW_AS_U32(fVar55)) {
          func_00115D70((float *)&local_1e74,(float *)pppppppppppfVar33,fVar55);
          local_1e80 = local_1e74;
          local_1e7c = local_1e70;
          local_1e78 = local_1e6c;
          pppppppppppfVar34 = local_1e6c;
          func_00115C54((float *)&local_1e4c,(float *)&local_1ea8,(float *)&local_1e80);
          pppppppppppfVar33[0x12] = (float **********)local_1e4c;
          pppppppppppfVar33[0x13] = (float **********)local_1e48;
          pppppppppppfVar33[0x14] = (float **********)local_1e44;
          if (*(char *)((int)pppppppppppfVar33 + 0x93) == '\x01') {
            func_00115D70((float *)&local_1e68,(float *)(pppppppppppfVar33 + 3),fVar58);
            local_1ec8 = local_1e68;
            local_1ec4 = (float ************)local_1e64;
            local_1ec0 = (float ************)local_1e60;
            func_00115D70((float *)&local_1e00,(float *)&local_1ec8,ALBW_AS_F32(uVar67));
            local_1e40 = local_1e00;
            local_1e3c = local_1dfc;
            local_1e38 = local_1df8;
            pppppppppppfVar34 = local_1df8;
            func_00115C54((float *)(pppppppppppfVar33 + 0x12),(float *)(pppppppppppfVar33 + 0x12),
                          (float *)&local_1e40);
          }
          if (*(char *)((int)pppppppppppfVar33 + 0x93) == '\0') {
            func_00115C70((float *)&local_1ec8,(float *)(pppppppppppfVar33 + 0x12),
                          (float *)(pppppppppppfVar33 + 0xc));
            pppppppppppfVar33[0x1b] =
                 (float **********)
                 ALBW_PTR_BITS((ALBW_AS_F32(*pppppppppppfVar33) * ALBW_AS_F32(local_1ec8) +
                  ALBW_AS_F32(pppppppppppfVar33[1]) * ALBW_AS_F32(local_1ec4) +
                 ALBW_AS_F32(pppppppppppfVar33[2]) * ALBW_AS_F32(local_1ec0)));
          }
          uVar70 = (ulonglong)(uint)ALBW_AS_U32((ALBW_AS_F32(uVar70) + ALBW_AS_F32((uVar70 >> 0x20))));
          *(undefined1 *)((int)pppppppppppfVar33 + 0x95) = 1;
        }
        fVar5 = ALBW_AS_F32(uVar70);
        uVar72 = CONCAT44((int)((ulonglong)uVar67 >> 0x20),
                          -(ALBW_AS_F32(*pppppppppppfVar38) * ALBW_AS_F32(pppppppppppfVar33[6]) +
                            ALBW_AS_F32(pppppppppppfVar38[1]) * ALBW_AS_F32(pppppppppppfVar33[7]) +
                           ALBW_AS_F32(pppppppppppfVar38[2]) * ALBW_AS_F32(pppppppppppfVar33[8])));
        func_00115C70((float *)&local_1f10,(float *)&local_1e9c,(float *)local_1e88);
        fVar55 = (-(ALBW_AS_F32(pppppppppppfVar33[6]) * ALBW_AS_F32(local_1f10) +
                   ALBW_AS_F32(pppppppppppfVar33[7]) * ALBW_AS_F32(local_1f0c)) +
                 -(ALBW_AS_F32(pppppppppppfVar33[8]) * ALBW_AS_F32(local_1f08))) / ALBW_AS_F32(uVar72);
        pppppppppppfVar15 = extraout_r1_02;
        if (iVar21 < (int)ALBW_AS_U32(fVar55)) {
          func_00115D70((float *)&local_1e88,(float *)pppppppppppfVar38,fVar55);
          local_1ec8 = local_1e88;
          local_1ec4 = (float ************)local_1e84;
          local_1ec0 = (float ************)local_1e80;
          func_00115C70((float *)&local_1e88,(float *)&local_1e9c,(float *)&local_1ec8);
          pppppppppppfVar34 = (float ***********)&local_1e88;
          pppppppppppfVar38[0xf] = (float **********)local_1e88;
          pppppppppppfVar38[0x10] = (float **********)local_1e84;
          pppppppppppfVar38[0x11] = (float **********)local_1e80;
          pppppppppppfVar15 = local_1e84;
          if (*(char *)((int)pppppppppppfVar38 + 0x93) == '\x01') {
            func_00115D70((float *)&local_1e88,(float *)(pppppppppppfVar38 + 3),fVar58);
            local_1f10 = local_1e88;
            local_1f0c = local_1e84;
            local_1f08 = local_1e80;
            func_00115D70((float *)&local_1e8c,(float *)&local_1f10,fVar57);
            local_1ec8 = local_1e8c;
            local_1ec4 = (float ************)local_1e88;
            local_1ec0 = (float ************)local_1e84;
            pppppppppppfVar34 = local_1e84;
            func_00115C54((float *)(pppppppppppfVar38 + 0xf),(float *)(pppppppppppfVar38 + 0xf),
                          (float *)&local_1ec8);
            pppppppppppfVar15 = extraout_r1_03;
          }
          if (*(char *)((int)pppppppppppfVar38 + 0x93) == '\0') {
            func_00115C70((float *)&local_1f10,(float *)(pppppppppppfVar38 + 9),
                          (float *)(pppppppppppfVar38 + 0xf));
            pppppppppppfVar38[0x1b] =
                 (float **********)
                 ALBW_PTR_BITS((ALBW_AS_F32(*pppppppppppfVar38) * ALBW_AS_F32(local_1f10) +
                  ALBW_AS_F32(pppppppppppfVar38[1]) * ALBW_AS_F32(local_1f0c) +
                 ALBW_AS_F32(pppppppppppfVar38[2]) * ALBW_AS_F32(local_1f08)));
            pppppppppppfVar15 = extraout_r1_04;
          }
          fVar5 = fVar5 + fVar55;
          *(undefined1 *)(pppppppppppfVar38 + 0x25) = 1;
        }
        local_1eb8 = (float ***********)((int)local_1eb8 + 1);
        iVar21 = DAT_001f2904;
        pppppppppppfVar33 = pppppppppppfVar29;
      }
    }
    fVar55 = ALBW_AS_F32(((ulonglong)uVar72 >> 0x20));
    if (fVar55 < fVar5) {
      pppppppppppfVar33 = (float ***********)((int)pppppppppppfVar9 + -1);
      pppppppppppfVar75 = local_1e54;
      if (local_1e54 == pppppppppppfVar33) {
        pppppppppppfVar75 = local_1eb8;
      }
      if (local_1e54 == pppppppppppfVar33 && pppppppppppfVar75 == (float ***********)0x0) {
        local_1eb8 = (float ***********)0x1;
        local_1e54 = (float ***********)((int)local_1e54 + -1);
      }
      fVar58 = fVar55;
      fVar57 = fVar55;
      if (0 < (int)local_1e54) {
        if (0 < (int)pppppppppppfVar9) {
          pppppppppppfVar15 = (float ***********)pppppppppppfVar78[0x2b];
          pppppppppppfVar34 = (float ***********)*pppppppppppfVar17;
        }
        pppppppppppfVar75 = (float ***********)0x0;
        if (0 < (int)pppppppppppfVar9) {
          do {
            pppppppppppfVar29 = pppppppppppfVar15;
            if (pppppppppppfVar75 < pppppppppppfVar34) {
              pppppppppppfVar29 = pppppppppppfVar15 + (int)pppppppppppfVar75 * 0x27;
            }
            if ((0x3effffff < (int)pppppppppppfVar29[0x1d]) &&
               (ppppppppppfVar22 = pppppppppppfVar29[0x1c], (int)ppppppppppfVar22 < 0x3f000001)) {
              if (pppppppppppfVar75 == (float ***********)0x0) {
                if ((float ***********)0x1 < pppppppppppfVar34) {
                  pppppppppppfVar15 = pppppppppppfVar15 + 0x27;
                }
                fVar58 = (ALBW_AS_F32(pppppppppppfVar15[0x1d]) - ALBW_AS_F32(pppppppppppfVar15[0x1c])) + fVar55;
                fVar57 = (ALBW_AS_F32(pppppppppppfVar29[0x1d]) - ALBW_AS_F32(ppppppppppfVar22)) + fVar55;
              }
              else {
                pppppppppppfVar28 = (float ***********)((int)pppppppppppfVar75 + -1);
                pppppppppppfVar38 = pppppppppppfVar15;
                if (pppppppppppfVar28 < pppppppppppfVar34) {
                  pppppppppppfVar38 = pppppppppppfVar15 + (int)pppppppppppfVar28 * 0x27;
                }
                if (pppppppppppfVar28 < pppppppppppfVar34) {
                  pppppppppppfVar15 = pppppppppppfVar15 + (int)((int)pppppppppppfVar75 + -1) * 0x27;
                }
                fVar58 = (ALBW_AS_F32(pppppppppppfVar29[0x1d]) - ALBW_AS_F32(ppppppppppfVar22)) + fVar55;
                fVar57 = (ALBW_AS_F32(pppppppppppfVar38[0x1d]) - ALBW_AS_F32(pppppppppppfVar15[0x1c])) + fVar55;
              }
              break;
            }
            pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar75 + 1);
          } while ((int)pppppppppppfVar75 < (int)pppppppppppfVar9);
        }
      }
      if ((0 < (int)local_1eb8) && (-1 < (int)pppppppppppfVar33)) {
        ppppppppppfVar22 = pppppppppppfVar78[0x2b];
        pppppppppppfVar34 = (float ***********)*pppppppppppfVar17;
        pppppppppppfVar15 = pppppppppppfVar33;
        do {
          ppppppppppfVar56 = ppppppppppfVar22;
          if (pppppppppppfVar15 < pppppppppppfVar34) {
            ppppppppppfVar56 = ppppppppppfVar22 + (int)pppppppppppfVar15 * 0x27;
          }
          if ((0x3effffff < (int)ppppppppppfVar56[0x1d]) &&
             ((int)ppppppppppfVar56[0x1c] < 0x3f000001)) {
            if (pppppppppppfVar15 == pppppppppppfVar33) {
              pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar15 + -1);
              ppppppppppfVar73 = ppppppppppfVar22;
              if (pppppppppppfVar75 < pppppppppppfVar34) {
                ppppppppppfVar73 = ppppppppppfVar22 + (int)pppppppppppfVar75 * 0x27;
              }
              fVar58 = (ALBW_AS_F32(ppppppppppfVar56[0x1d]) - ALBW_AS_F32(ppppppppppfVar56[0x1c])) + fVar58;
              if (pppppppppppfVar75 < pppppppppppfVar34) {
                ppppppppppfVar22 = ppppppppppfVar22 + (int)((int)pppppppppppfVar15 + -1) * 0x27;
              }
              fVar57 = (ALBW_AS_F32(ppppppppppfVar73[0x1d]) - ALBW_AS_F32(ppppppppppfVar22[0x1c])) + fVar57;
            }
            else {
              pppppppppppfVar29 = (float ***********)((int)pppppppppppfVar15 + 1);
              pppppppppppfVar75 = pppppppppppfVar29;
              if (pppppppppppfVar29 < pppppppppppfVar34) {
                pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar29 * 0x27);
              }
              ppppppppppfVar73 = ppppppppppfVar22;
              if (pppppppppppfVar29 < pppppppppppfVar34) {
                ppppppppppfVar73 = ppppppppppfVar22 + (int)pppppppppppfVar75;
              }
              fVar57 = (ALBW_AS_F32(ppppppppppfVar56[0x1d]) - ALBW_AS_F32(ppppppppppfVar56[0x1c])) + fVar57;
              if (pppppppppppfVar29 < pppppppppppfVar34) {
                ppppppppppfVar22 = ppppppppppfVar22 + (int)((int)pppppppppppfVar15 + 1) * 0x27;
              }
              fVar58 = (ALBW_AS_F32(ppppppppppfVar73[0x1d]) - ALBW_AS_F32(ppppppppppfVar22[0x1c])) + fVar58;
            }
            break;
          }
          pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar15 + -1);
        } while (-1 < (int)pppppppppppfVar15);
      }
      uVar72 = CONCAT44(fVar55,fVar55);
      fVar60 = fVar55;
      if (fVar55 < fVar57 + fVar58) {
        fVar55 = fVar5 * fVar58 * (ALBW_AS_F32(pppppppppppfVar74) / (fVar57 + fVar58));
        fVar60 = fVar5 - fVar55;
      }
      pppppppppppfVar15 = (float ***********)0x0;
      for (; ((int)pppppppppppfVar15 < (int)pppppppppppfVar9 && (ALBW_AS_F32(uVar72) < fVar55));
          fVar55 = fVar55 - fVar5) {
        if (pppppppppppfVar15 < *pppppppppppfVar17) {
          ppppppppppfVar22 = pppppppppppfVar78[0x2b] + (int)pppppppppppfVar15 * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar78[0x2b];
        }
        func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x12),
                      (float *)(ppppppppppfVar22 + 0xf));
        fVar5 = ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1ec8) +
                ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1ec4) +
                ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1ec0);
        if (fVar55 < fVar5) {
          bVar44 = false;
          fVar58 = ALBW_AS_F32(uVar72);
          fVar5 = fVar55 - ALBW_AS_F32(ppppppppppfVar22[0x1b]);
          if (fVar55 - ALBW_AS_F32(ppppppppppfVar22[0x1b]) < fVar58) {
            fVar5 = fVar58;
          }
          if (fVar5 == fVar58) {
            local_1f28[0] = (float ***********)ppppppppppfVar22[9];
            local_1f28[1] = (float ***********)ppppppppppfVar22[10];
            local_1f28[2] = (float ***********)ppppppppppfVar22[0xb];
LAB_001f2210:
            if (pppppppppppfVar15 != (float ***********)0x0) goto LAB_001f2234;
          }
          else {
            if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
              func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar22,fVar5);
              local_1ea8 = local_1ec8;
              local_1ea4 = (float ***********)local_1ec4;
              local_1ea0 = (float ***********)local_1ec0;
              func_00115C54((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0xf),
                            (float *)&local_1ea8);
              local_1f28[0] = local_1ec8;
              local_1f28[1] = (float ***********)local_1ec4;
              local_1f28[2] = (float ***********)local_1ec0;
              goto LAB_001f2210;
            }
            func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar22,fVar5);
            local_1e84 = local_1ec8;
            local_1e80 = (float ***********)local_1ec4;
            local_1e7c = (float ***********)local_1ec0;
            func_00115C54((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0xf),
                          (float *)&local_1e84);
            local_1e90 = local_1ec8;
            local_1e8c = (float ***********)local_1ec4;
            local_1e88 = (float ***********)local_1ec0;
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 3),
                          ALBW_AS_F32(pppppppppppfVar49[0x1c]));
            local_1ea8 = local_1ec8;
            local_1ea4 = (float ***********)local_1ec4;
            local_1ea0 = (float ***********)local_1ec0;
            func_00115D88((float *)&local_1ec8,(float *)&local_1ea8,ALBW_AS_F32(ppppppppppfVar22[4]));
            local_1e78 = local_1ec8;
            local_1e74 = (float ***********)local_1ec4;
            local_1e70 = (float ***********)local_1ec0;
            func_00115C54((float *)&local_1ec8,(float *)&local_1e90,(float *)&local_1e78);
            local_1f28[0] = local_1ec8;
            local_1f28[1] = (float ***********)local_1ec4;
            local_1f28[2] = (float ***********)local_1ec0;
            if (pppppppppppfVar15 != (float ***********)0x0) {
              if (*(char *)(ppppppppppfVar22 + 0x25) != '\0') {
                func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar22,fVar5);
                local_1e6c = local_1ec8;
                local_1e68 = (float ***********)local_1ec4;
                local_1e64 = (float ***********)local_1ec0;
                func_00115C54((float *)(ppppppppppfVar22 + 0xf),(float *)(ppppppppppfVar22 + 0xf),
                              (float *)&local_1e6c);
                func_00115C70((float *)&local_1ec8,(float *)local_1f28,
                              (float *)(ppppppppppfVar22 + 0xf));
                local_1e6c = local_1ec8;
                local_1e68 = (float ***********)local_1ec4;
                local_1e64 = (float ***********)local_1ec0;
                func_0068906C((float *)&local_1e9c,(float *)&local_1e6c);
                bVar44 = true;
              }
              goto LAB_001f2234;
            }
          }
          if (*(char *)(ppppppppppfVar22 + 0x26) == '\0') {
LAB_001f2234:
            pppppppppppfVar34 = (float ***********)0x0;
            local_1f04 = pppppppppppfVar15;
            goto LAB_001f24a4;
          }
          pppppppppppfVar34 = (float ***********)0x0;
          local_1f04 = (float ***********)0x1;
          goto LAB_001f24b0;
        }
        pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar15 + 1);
      }
LAB_001f24b8:
      while( true ) {
        pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar9 + -1);
        bVar44 = (int)pppppppppppfVar15 < 0;
        bVar47 = pppppppppppfVar15 == (float ***********)0x0;
        bVar45 = false;
        if (!bVar44) {
          fVar5 = ALBW_AS_F32(uVar72);
          bVar44 = fVar60 < fVar5;
          bVar47 = fVar60 == fVar5;
          bVar45 = NAN(fVar60) || NAN(fVar5);
        }
        if (bVar47 || bVar44 != bVar45) goto LAB_001f298c;
        if (pppppppppppfVar15 < *pppppppppppfVar17) {
          ppppppppppfVar22 = pppppppppppfVar78[0x2b] + (int)pppppppppppfVar15 * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar78[0x2b];
        }
        func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x12),
                      (float *)(ppppppppppfVar22 + 0xf));
        fVar5 = ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1ec8) +
                ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1ec4) +
                ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1ec0);
        if (fVar60 < fVar5) break;
        fVar60 = fVar60 - fVar5;
        pppppppppppfVar9 = pppppppppppfVar15;
      }
      bVar44 = false;
      fVar55 = ALBW_AS_F32(uVar72);
      fVar5 = fVar60 - ALBW_AS_F32(ppppppppppfVar22[0x1b]);
      if (fVar60 - ALBW_AS_F32(ppppppppppfVar22[0x1b]) < fVar55) {
        fVar5 = fVar55;
      }
      if (fVar5 == fVar55) {
        local_1f28[0] = (float ***********)ppppppppppfVar22[0xc];
        local_1f28[1] = (float ***********)ppppppppppfVar22[0xd];
        local_1f28[2] = (float ***********)ppppppppppfVar22[0xe];
      }
      else if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
        func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar22,fVar5);
        local_1ea8 = local_1ec8;
        local_1ea4 = (float ***********)local_1ec4;
        local_1ea0 = (float ***********)local_1ec0;
        func_00115C70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar22 + 0x12),
                      (float *)&local_1ea8);
        local_1f28[0] = local_1f28[3];
        local_1f28[1] = local_1f28[4];
        local_1f28[2] = local_1f14;
      }
      else {
        func_00115D70((float *)&local_1ea8,(float *)ppppppppppfVar22,fVar5);
        local_1e84 = local_1ea8;
        local_1e80 = local_1ea4;
        local_1e7c = local_1ea0;
        func_00115C70((float *)&local_1ea8,(float *)(ppppppppppfVar22 + 0x12),(float *)&local_1e84);
        local_1e90 = local_1ea8;
        local_1e8c = local_1ea4;
        local_1e88 = local_1ea0;
        func_00115D70((float *)&local_1ea8,(float *)(ppppppppppfVar22 + 3),
                      ALBW_AS_F32(pppppppppppfVar49[0x1c]));
        local_1ec8 = local_1ea8;
        local_1ec4 = (float ************)local_1ea4;
        local_1ec0 = (float ************)local_1ea0;
        func_00115D88((float *)&local_1ea8,(float *)&local_1ec8,ALBW_AS_F32(ppppppppppfVar22[4]));
        local_1e78 = local_1ea8;
        local_1e74 = local_1ea4;
        local_1e70 = local_1ea0;
        func_00115C54((float *)&local_1ea8,(float *)&local_1e90,(float *)&local_1e78);
        local_1f28[0] = local_1ea8;
        local_1f28[1] = local_1ea4;
        local_1f28[2] = local_1ea0;
        pppppppppppfVar34 = local_1ea4;
        if (pppppppppppfVar15 != pppppppppppfVar33) {
          pppppppppppfVar34 = (float ***********)(uint)*(byte *)((int)ppppppppppfVar22 + 0x95);
        }
        if (pppppppppppfVar15 != pppppppppppfVar33 && pppppppppppfVar34 != (float ***********)0x0) {
          func_00115D70((float *)&local_1f14,(float *)ppppppppppfVar22,fVar5);
          local_1ea8 = local_1f14;
          local_1ea4 = local_1f10;
          local_1ea0 = local_1f0c;
          func_00115C70((float *)(ppppppppppfVar22 + 0x12),(float *)(ppppppppppfVar22 + 0x12),
                        (float *)&local_1ea8);
          func_00115C70((float *)(local_1f28 + 3),(float *)local_1f28,
                        (float *)(ppppppppppfVar22 + 0x12));
          local_1ea8 = local_1f28[3];
          local_1ea4 = local_1f28[4];
          local_1ea0 = local_1f14;
          func_0068906C((float *)&local_1e9c,(float *)&local_1ea8);
          bVar44 = true;
        }
      }
      pppppppppppfVar34 = pppppppppppfVar15;
      if ((pppppppppppfVar15 == pppppppppppfVar33) && (*(char *)(ppppppppppfVar22 + 0x26) != '\0'))
      {
        pppppppppppfVar34 = (float ***********)((int)pppppppppppfVar9 + -2);
      }
      pppppppppppfVar9 = (float ***********)*pppppppppppfVar17;
      while( true ) {
        pppppppppppfVar9 = (float ***********)((int)pppppppppppfVar9 + -1);
        pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar9 - (int)pppppppppppfVar34);
        if ((int)pppppppppppfVar9 >= (int)pppppppppppfVar34) {
          pppppppppppfVar75 = pppppppppppfVar9;
        }
        if ((int)pppppppppppfVar75 < 0 !=
            ((int)pppppppppppfVar9 < (int)pppppppppppfVar34 &&
            SBORROW4((int)pppppppppppfVar9,(int)pppppppppppfVar34))) break;
        if (pppppppppppfVar9 < *pppppppppppfVar17) {
          ppppppppppfVar22 = pppppppppppfVar78[0x2b] + (int)pppppppppppfVar9 * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar78[0x2b];
        }
        *(undefined1 *)((int)ppppppppppfVar22 + 0x92) = 1;
        if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
          func_00115C70((float *)&local_1f04,(float *)local_1f28,(float *)(ppppppppppfVar22 + 0x12))
          ;
          func_00115D70((float *)&local_1f04,(float *)(ppppppppppfVar22 + 6),
                        ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f04) +
                        ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f00[0]) +
                        ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f00[1]));
          local_1ea8 = local_1f04;
          local_1ea4 = local_1f00[0];
          local_1ea0 = local_1f00[1];
          func_00115C70((float *)&local_1f04,(float *)local_1f28,(float *)&local_1ea8);
          ppppppppppfVar22[0x12] = (float *********)local_1f04;
          ppppppppppfVar22[0x13] = (float *********)local_1f00[0];
          ppppppppppfVar22[0x14] = (float *********)local_1f00[1];
        }
        else if (pppppppppppfVar9 != pppppppppppfVar15 && bVar44) {
          fVar55 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1e9c) +
                   ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1e98) +
                   ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1e94);
          fVar5 = fVar55;
          if (fVar55 <= ALBW_AS_F32(uVar72)) {
            fVar5 = -fVar55;
          }
          if ((int)ALBW_AS_U32(fVar5) <= DAT_001f290c) goto LAB_001f2898;
          func_00115C70((float *)&local_1e40,(float *)local_1f28,(float *)(ppppppppppfVar22 + 0xc));
          local_1e90 = local_1e40;
          local_1e8c = local_1e3c;
          local_1e88 = local_1e38;
          func_00115D70((float *)&local_1e18,(float *)&local_1e9c,
                        ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1e40) +
                        ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1e3c) +
                        ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1e38));
          local_1f04 = local_1e18;
          local_1f00[0] = (float ***********)local_1e14;
          local_1f00[1] = (float ***********)local_1e10;
          func_00115D88((float *)&local_1e0c,(float *)&local_1f04,fVar55);
          local_1e90 = local_1e0c;
          local_1e8c = local_1e08;
          local_1e88 = local_1e04;
          func_00115C70((float *)&local_1eb4,(float *)local_1f28,(float *)&local_1e90);
          local_1ec8 = (float ***********)local_1eb4;
          local_1ec4 = (float ************)local_1eb0;
          local_1ec0 = local_1eac;
          func_00115C70((float *)&local_1e78,(float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x12)
                       );
          local_1f04 = local_1e78;
          local_1f00[0] = local_1e74;
          local_1f00[1] = local_1e70;
          if (ALBW_AS_F32(local_1e78) * ALBW_AS_F32(ppppppppppfVar22[3]) +
              ALBW_AS_F32(local_1e74) * ALBW_AS_F32(ppppppppppfVar22[4]) +
              ALBW_AS_F32(local_1e70) * ALBW_AS_F32(ppppppppppfVar22[5]) <= ALBW_AS_F32(uVar72)) goto LAB_001f2898;
          ppppppppppfVar22[0x18] = (float *********)local_1ec8;
          ppppppppppfVar22[0x19] = (float *********)local_1ec4;
          ppppppppppfVar22[0x1a] = (float *********)local_1ec0;
        }
        else {
LAB_001f2898:
          func_00115C70((float *)&local_1e00,(float *)local_1f28,(float *)(ppppppppppfVar22 + 0x12))
          ;
          func_00115D70((float *)&local_1e6c,(float *)(ppppppppppfVar22 + 6),
                        ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1e00) +
                        ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1dfc) +
                        ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1df8));
          local_1e84 = local_1e6c;
          local_1e80 = local_1e68;
          local_1e7c = local_1e64;
          func_00115C70((float *)&local_1e4c,(float *)local_1f28,(float *)&local_1e84);
          ppppppppppfVar22[0x18] = (float *********)local_1e4c;
          ppppppppppfVar22[0x19] = (float *********)local_1e48;
          ppppppppppfVar22[0x1a] = (float *********)local_1e44;
        }
      }
    }
  }
LAB_001f298c:
  pppppppppppfVar17 = pppppppppppfVar49;
  if (((uint)local_1e30 & 2) != 0) {
    fVar5 = ALBW_AS_F32(((ulonglong)uVar72 >> 0x20));
    ppppppppppfVar56 = (float **********)0x1;
    pppppppppppfVar15 = pppppppppppfVar49 + 0x29;
    local_1f04 = (float ***********)*pppppppppppfVar15;
    local_1e40 = (float ***********)0x0;
    local_1eb0 = (float ***********)0x0;
    ppppppppppfVar22 = (float **********)0x0;
    if (0 < (int)local_1f04) {
      do {
        ppppppppppfVar73 = pppppppppppfVar49[0x2b];
        if (ppppppppppfVar22 < *pppppppppppfVar15) {
          ppppppppppfVar73 = ppppppppppfVar73 + (int)ppppppppppfVar22 * 0x27;
        }
        ppppppppppfVar30 = pppppppppppfVar49[0x2b];
        if (ppppppppppfVar22 < *pppppppppppfVar15) {
          ppppppppppfVar30 = ppppppppppfVar30 + (int)ppppppppppfVar22 * 0x27;
        }
        ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
        *(undefined1 *)(ppppppppppfVar30 + 0x25) = 0;
        *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 0;
      } while ((int)ppppppppppfVar22 < (int)local_1f04);
    }
    pppppppppppfVar34 = pppppppppppfVar74;
    if (1 < (int)local_1f04) {
      do {
        if ((float **********)((int)ppppppppppfVar56 + -1) < *pppppppppppfVar15) {
          ppppppppppfVar22 = pppppppppppfVar49[0x2b] + ((int)ppppppppppfVar56 + -1) * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar49[0x2b];
        }
        if (ppppppppppfVar56 < *pppppppppppfVar15) {
          ppppppppppfVar73 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar56 * 0x27;
        }
        else {
          ppppppppppfVar73 = pppppppppppfVar49[0x2b];
        }
        if ((((int)ppppppppppfVar22[7] <= DAT_001f2904) &&
            ((int)ppppppppppfVar73[7] <= DAT_001f2904)) ||
           ((int)ALBW_AS_U32((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar73[6]) +
                  ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar73[7]) +
                 ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar73[8]))) <= DAT_001f3a80)) break;
        fVar55 = ALBW_AS_F32(pppppppppppfVar17[0x1c]) / ALBW_AS_F32(ppppppppppfVar22[4]);
        if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar22 + 0xc);
        }
        else {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar22 + 0x15);
        }
        uVar67 = CONCAT44(fVar55,ALBW_AS_F32(pppppppppppfVar17[0x1c]) / ALBW_AS_F32(ppppppppppfVar73[4]));
        local_1e38 = pppppppppppfVar78;
        func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 3),fVar55);
        local_1ec8 = local_1ebc;
        local_1ec4 = (float ************)local_1eb8;
        local_1ec0 = local_1eb4;
        func_00115C70((float *)&local_1ebc,(float *)pppppppppppfVar78,(float *)&local_1ec8);
        local_1f28[3] = local_1ebc;
        local_1f28[4] = local_1eb8;
        local_1f14 = (float ***********)local_1eb4;
        local_1e78 = local_1ebc;
        local_1e74 = local_1eb8;
        local_1e70 = (float ***********)local_1eb4;
        if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar73 + 9);
        }
        else {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar73 + 0x15);
        }
        local_1f10 = pppppppppppfVar78;
        func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 3),ALBW_AS_F32(uVar67));
        local_1f28[3] = local_1ebc;
        local_1f28[4] = local_1eb8;
        local_1f14 = (float ***********)local_1eb4;
        func_00115C70((float *)&local_1ebc,(float *)pppppppppppfVar78,(float *)(local_1f28 + 3));
        local_1ec8 = local_1ebc;
        local_1ec4 = (float ************)local_1eb8;
        local_1ec0 = local_1eb4;
        local_1e4c = local_1ebc;
        local_1e48 = local_1eb8;
        local_1e44 = (float ***********)local_1eb4;
        fVar55 = ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar73[6]) +
                 ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar73[7]) +
                 ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar73[8]);
        func_00115C70((float *)(local_1f28 + 1),(float *)&local_1e78,(float *)local_1f10);
        fVar55 = (-(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[1]) +
                   ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[2])) +
                 -(ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[3]))) / fVar55;
        lVar69 = (ulonglong)(uint)ALBW_AS_U32(fVar55) << 0x20;
        if (DAT_001f2904 < (int)ALBW_AS_U32(fVar55)) {
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
            ppppppppppfVar10 = *pppppppppppfVar15;
            bVar47 = ppppppppppfVar56 <= ppppppppppfVar10;
            bVar44 = ppppppppppfVar10 == ppppppppppfVar56;
            ppppppppppfVar30 = ppppppppppfVar56;
            fVar58 = fVar5;
            if ((int)ppppppppppfVar56 < (int)ppppppppppfVar10) {
              do {
                if (!bVar47 || bVar44) {
                  ppppppppppfVar10 = pppppppppppfVar49[0x2b];
                }
                else {
                  ppppppppppfVar10 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar30 * 0x27;
                }
                func_00115C70((float *)(local_1f28 + 4),(float *)(ppppppppppfVar10 + 0xc),
                              (float *)(ppppppppppfVar10 + 9));
                fVar55 = ALBW_AS_F32(((ulonglong)lVar69 >> 0x20));
                ppppppppppfVar30 = (float **********)((int)ppppppppppfVar30 + 1);
                ppppppppppfVar11 = *pppppppppppfVar15;
                fVar57 = ALBW_AS_F32(*ppppppppppfVar10) * ALBW_AS_F32(local_1f28[4]) +
                         ALBW_AS_F32(ppppppppppfVar10[1]) * ALBW_AS_F32(local_1f14) +
                         ALBW_AS_F32(ppppppppppfVar10[2]) * ALBW_AS_F32(local_1f10);
                if (fVar57 <= fVar5) {
                  fVar57 = -fVar57;
                }
                bVar47 = ppppppppppfVar30 <= ppppppppppfVar11;
                bVar44 = ppppppppppfVar11 == ppppppppppfVar30;
                fVar58 = fVar57 + fVar58;
              } while (!bVar44 && (int)ppppppppppfVar30 <= (int)ppppppppppfVar11);
            }
            if (fVar58 < fVar55) {
              fVar55 = fVar58;
            }
            if (*(char *)((int)ppppppppppfVar22 + 0x93) != '\0') goto LAB_001f2c8c;
            func_00115D70((float *)&local_1e90,(float *)ppppppppppfVar22,fVar55);
            local_1eac = (float ************)local_1e90;
            local_1ea8 = local_1e8c;
            local_1ea4 = local_1e88;
            func_00115C54((float *)&local_1e18,(float *)&local_1e78,(float *)&local_1eac);
            ppppppppppfVar22[0x12] = (float *********)local_1e18;
            ppppppppppfVar22[0x13] = (float *********)local_1e14;
            ppppppppppfVar22[0x14] = (float *********)local_1e10;
          }
          else {
LAB_001f2c8c:
            func_00115D70((float *)&local_1e84,(float *)ppppppppppfVar22,fVar55);
            local_1e0c = local_1e84;
            local_1e08 = local_1e80;
            local_1e04 = local_1e7c;
            func_00115C54((float *)(ppppppppppfVar22 + 0x18),(float *)(ppppppppppfVar22 + 0x18),
                          (float *)&local_1e0c);
          }
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 3),
                          ALBW_AS_F32(pppppppppppfVar34));
            local_1f28[2] = local_1ec8;
            local_1f28[3] = (float ***********)local_1ec4;
            local_1f28[4] = (float ***********)local_1ec0;
            func_00115D70((float *)&local_1ec8,(float *)(local_1f28 + 2),
                          ALBW_AS_F32(((ulonglong)uVar67 >> 0x20)));
            local_1e24 = local_1ec8;
            local_1e20 = (float ***********)local_1ec4;
            local_1e1c = (float ***********)local_1ec0;
            func_00115C54((float *)(ppppppppppfVar22 + 0x12),(float *)(ppppppppppfVar22 + 0x12),
                          (float *)&local_1e24);
          }
          *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
              local_1f08 = (float ***********)(ppppppppppfVar22 + 0x18);
              func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 0x18),
                            (float *)(ppppppppppfVar22 + 0x12));
              func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 6),
                            ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1ebc) +
                            ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1eb8) +
                            ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1eb4));
              local_1eac = (float ************)local_1ebc;
              local_1ea8 = local_1eb8;
              local_1ea4 = (float ***********)local_1eb4;
              func_00115C70((float *)(local_1f28 + 1),(float *)local_1f08,(float *)&local_1eac);
              local_1ec8 = local_1f28[1];
              local_1ec4 = (float ************)local_1f28[2];
              local_1ec0 = (float ************)local_1f28[3];
            }
            else {
              local_1ec8 = (float ***********)ppppppppppfVar22[0x12];
              local_1ec4 = (float ************)ppppppppppfVar22[0x13];
              local_1ec0 = (float ************)ppppppppppfVar22[0x14];
            }
            if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
              func_00115C70((float *)(local_1f28 + 1),(float *)(ppppppppppfVar22 + 0x12),
                            (float *)&local_1ec8);
              local_1ebc = local_1f28[1];
              local_1eb8 = local_1f28[2];
              local_1eb4 = (float ************)local_1f28[3];
              func_0068906C((float *)(local_1f28 + 4),(float *)&local_1ebc);
            }
            else {
              func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 0x18),
                            (float *)&local_1ec8);
              local_1f28[1] = local_1ebc;
              local_1f28[2] = local_1eb8;
              local_1f28[3] = (float ***********)local_1eb4;
              func_0068906C((float *)(local_1f28 + 4),(float *)(local_1f28 + 1));
            }
            fVar58 = ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[4]) +
                     ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f14) +
                     ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f10);
            fVar55 = fVar58;
            if (fVar58 <= fVar5) {
              fVar55 = -fVar58;
            }
            if (DAT_001f290c < (int)ALBW_AS_U32(fVar55)) {
              func_00115C70((float *)&local_1ebc,(float *)&local_1ec8,
                            (float *)(ppppppppppfVar73 + 9));
              fVar55 = -(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ebc) +
                         ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1eb8) +
                        ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1eb4));
              if (fVar55 <= fVar5) {
                if ((uint)ALBW_AS_U32(DAT_001f3a84) < (uint)ALBW_AS_U32(fVar55)) {
                  func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 0xc),
                                (float *)(ppppppppppfVar22 + 9));
                  func_00115D70((float *)&local_1ebc,(float *)ppppppppppfVar73,
                                ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1ebc) +
                                ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1eb8) +
                                ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1eb4));
                  local_1e84 = local_1ebc;
                  local_1e80 = local_1eb8;
                  local_1e7c = (float ***********)local_1eb4;
                  func_00115C70((float *)&local_1ebc,(float *)&local_1e4c,(float *)&local_1e84);
                  ppppppppppfVar73[0xf] = (float *********)local_1ebc;
                  ppppppppppfVar73[0x10] = (float *********)local_1eb8;
                  ppppppppppfVar73[0x11] = (float *********)local_1eb4;
                  func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 3),
                                ALBW_AS_F32(pppppppppppfVar34));
                  local_1e90 = local_1ebc;
                  local_1e8c = local_1eb8;
                  local_1e88 = (float ***********)local_1eb4;
                  func_00115D70((float *)&local_1ebc,(float *)&local_1e90,ALBW_AS_F32(uVar67));
                  local_1e84 = local_1ebc;
                  local_1e80 = local_1eb8;
                  local_1e7c = (float ***********)local_1eb4;
                  func_00115C54((float *)(ppppppppppfVar73 + 0xf),(float *)(ppppppppppfVar73 + 0xf),
                                (float *)&local_1e84);
                  *(undefined1 *)(ppppppppppfVar73 + 0x25) = 1;
                  if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
                    local_1f08 = (float ***********)(ppppppppppfVar73 + 0x15);
                    local_1f28[1] = (float ***********)(ppppppppppfVar73 + 6);
                    func_00115C70((float *)&local_1eac,(float *)(ppppppppppfVar73 + 0x15),
                                  (float *)(ppppppppppfVar73 + 0xf));
                    func_00115D70((float *)&local_1eac,(float *)local_1f28[1],
                                  ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1eac) +
                                  ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ea8) +
                                  ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ea4));
                    local_1ebc = (float ***********)local_1eac;
                    local_1eb8 = local_1ea8;
                    local_1eb4 = (float ************)local_1ea4;
                    func_00115C70((float *)(local_1f28 + 1),(float *)local_1f08,(float *)&local_1ebc
                                 );
                    local_1ec8 = local_1f28[1];
                    local_1ec4 = (float ************)local_1f28[2];
                    local_1ec0 = (float ************)local_1f28[3];
                  }
                  else {
                    local_1ec8 = (float ***********)ppppppppppfVar73[0xf];
                    local_1ec4 = (float ************)ppppppppppfVar73[0x10];
                    local_1ec0 = (float ************)ppppppppppfVar73[0x11];
                  }
                  if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
                    func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 0xf),
                                  (float *)&local_1ec8);
                    local_1f28[1] = local_1ebc;
                    local_1f28[2] = local_1eb8;
                    local_1f28[3] = (float ***********)local_1eb4;
                    func_0068906C((float *)(local_1f28 + 4),(float *)(local_1f28 + 1));
                  }
                  else {
                    func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 0x15),
                                  (float *)&local_1ec8);
                    local_1f28[1] = local_1ebc;
                    local_1f28[2] = local_1eb8;
                    local_1f28[3] = (float ***********)local_1eb4;
                    func_0068906C((float *)(local_1f28 + 4),(float *)(local_1f28 + 1));
                  }
                  fVar58 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[4]) +
                           ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f14) +
                           ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f10);
                  fVar55 = fVar58;
                  if (fVar58 <= fVar5) {
                    fVar55 = -fVar58;
                  }
                  if (DAT_001f290c < (int)ALBW_AS_U32(fVar55)) {
                    func_00115C70((float *)&local_1ebc,(float *)&local_1ec8,
                                  (float *)(ppppppppppfVar22 + 0xc));
                    fVar55 = -(ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1ebc) +
                               ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1eb8) +
                              ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1eb4));
                    if (fVar5 < fVar55) {
                      func_00115D70((float *)&local_1e24,(float *)(local_1f28 + 4),fVar55);
                      local_1eac = (float ************)local_1e24;
                      local_1ea8 = local_1e20;
                      local_1ea4 = local_1e1c;
                      func_00115D88((float *)&local_1e24,(float *)&local_1eac,fVar58);
                      local_1ebc = local_1e24;
                      local_1eb8 = local_1e20;
                      local_1eb4 = (float ************)local_1e1c;
                      func_00115C54((float *)(local_1f28 + 1),(float *)&local_1ec8,
                                    (float *)&local_1ebc);
                      local_1ea0 = local_1f28[1];
                      local_1e9c = local_1f28[2];
                      local_1e98 = local_1f28[3];
                      func_00115C70((float *)(local_1f28 + 1),(float *)(ppppppppppfVar22 + 0x12),
                                    (float *)&local_1ea0);
                      local_1eac = (float ************)local_1f28[1];
                      local_1ea8 = local_1f28[2];
                      local_1ea4 = local_1f28[3];
                      if (fVar5 < ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(ppppppppppfVar22[3]) +
                                  ALBW_AS_F32(local_1f28[2]) * ALBW_AS_F32(ppppppppppfVar22[4]) +
                                  ALBW_AS_F32(local_1f28[3]) * ALBW_AS_F32(ppppppppppfVar22[5])) {
                        ppppppppppfVar22[0x18] = (float *********)local_1ea0;
                        ppppppppppfVar22[0x19] = (float *********)local_1e9c;
                        ppppppppppfVar22[0x1a] = (float *********)local_1e98;
                        *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
                        if ((*(char *)((int)ppppppppppfVar22 + 0x97) != '\0') &&
                           (*(char *)(ppppppppppfVar22 + 0x25) == '\0')) {
                          ppppppppppfVar30 = ppppppppppfVar22 + 0x15;
                          ppppppppppfVar22[0x15] = ppppppppppfVar22[0x18];
                          ppppppppppfVar22[0x16] = ppppppppppfVar22[0x19];
                          pppppppppfVar48 = ppppppppppfVar22[0x1a];
                          goto LAB_001f334c;
                        }
                      }
                    }
                  }
                }
              }
              else {
                func_00115D70((float *)&local_1ea0,(float *)(local_1f28 + 4),fVar55);
                local_1eac = (float ************)local_1ea0;
                local_1ea8 = local_1e9c;
                local_1ea4 = local_1e98;
                func_00115D88((float *)&local_1ea0,(float *)&local_1eac,fVar58);
                local_1e90 = local_1ea0;
                local_1e8c = local_1e9c;
                local_1e88 = local_1e98;
                func_00115C54((float *)(local_1f28 + 1),(float *)&local_1ec8,(float *)&local_1e90);
                local_1ebc = local_1f28[1];
                local_1eb8 = local_1f28[2];
                local_1eb4 = (float ************)local_1f28[3];
                func_00115C70((float *)&local_1ea0,(float *)(ppppppppppfVar73 + 0xf),
                              (float *)&local_1ebc);
                local_1eac = (float ************)local_1ea0;
                local_1ea8 = local_1e9c;
                local_1ea4 = local_1e98;
                if (fVar5 < ALBW_AS_F32(local_1ea0) * ALBW_AS_F32(ppppppppppfVar73[3]) +
                            ALBW_AS_F32(local_1e9c) * ALBW_AS_F32(ppppppppppfVar73[4]) +
                            ALBW_AS_F32(local_1e98) * ALBW_AS_F32(ppppppppppfVar73[5])) {
                  ppppppppppfVar73[0x15] = (float *********)local_1ebc;
                  ppppppppppfVar73[0x16] = (float *********)local_1eb8;
                  ppppppppppfVar73[0x17] = (float *********)local_1eb4;
                  *(undefined1 *)(ppppppppppfVar73 + 0x25) = 1;
                  if ((*(char *)((int)ppppppppppfVar73 + 0x97) != '\0') &&
                     (*(char *)((int)ppppppppppfVar73 + 0x95) == '\0')) {
                    ppppppppppfVar30 = ppppppppppfVar73 + 0x18;
                    ppppppppppfVar73[0x18] = ppppppppppfVar73[0x15];
                    ppppppppppfVar73[0x19] = ppppppppppfVar73[0x16];
                    pppppppppfVar48 = ppppppppppfVar73[0x17];
LAB_001f334c:
                    ppppppppppfVar30[2] = pppppppppfVar48;
                  }
                }
              }
            }
          }
        }
        fVar55 = -(ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(ppppppppppfVar22[6]) +
                   ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(ppppppppppfVar22[7]) +
                  ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(ppppppppppfVar22[8]));
        func_00115C70((float *)(local_1f28 + 1),(float *)&local_1e4c,(float *)local_1e38);
        fVar55 = (-(ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[1]) +
                   ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[2])) +
                 -(ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f28[3]))) / fVar55;
        lVar69 = (ulonglong)(uint)ALBW_AS_U32(fVar55) << 0x20;
        if (DAT_001f2904 < (int)ALBW_AS_U32(fVar55)) {
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            ppppppppppfVar30 = (float **********)0x0;
            fVar58 = fVar5;
            if (0 < (int)ppppppppppfVar56) {
              do {
                if (ppppppppppfVar30 < *pppppppppppfVar15) {
                  ppppppppppfVar10 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar30 * 0x27;
                }
                else {
                  ppppppppppfVar10 = pppppppppppfVar49[0x2b];
                }
                func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar10 + 0xc),
                              (float *)(ppppppppppfVar10 + 9));
                fVar55 = ALBW_AS_F32(((ulonglong)lVar69 >> 0x20));
                ppppppppppfVar30 = (float **********)((int)ppppppppppfVar30 + 1);
                fVar57 = ALBW_AS_F32(*ppppppppppfVar10) * ALBW_AS_F32(local_1ec8) +
                         ALBW_AS_F32(ppppppppppfVar10[1]) * ALBW_AS_F32(local_1ec4) +
                         ALBW_AS_F32(ppppppppppfVar10[2]) * ALBW_AS_F32(local_1ec0);
                if (fVar57 <= fVar5) {
                  fVar57 = -fVar57;
                }
                fVar58 = fVar57 + fVar58;
              } while ((int)ppppppppppfVar30 < (int)ppppppppppfVar56);
            }
            if (fVar58 < fVar55) {
              fVar55 = fVar58;
            }
            if (*(char *)((int)ppppppppppfVar73 + 0x93) != '\0') goto LAB_001f3450;
            func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar73,fVar55);
            local_1ebc = local_1ec8;
            local_1eb8 = (float ***********)local_1ec4;
            local_1eb4 = local_1ec0;
            func_00115C70((float *)&local_1ec8,(float *)&local_1e4c,(float *)&local_1ebc);
            ppppppppppfVar73[0xf] = (float *********)local_1ec8;
            ppppppppppfVar73[0x10] = (float *********)local_1ec4;
            ppppppppppfVar73[0x11] = (float *********)local_1ec0;
          }
          else {
LAB_001f3450:
            local_1ebc = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(*ppppppppppfVar73));
            local_1eb8 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar73[1]));
            local_1eb4 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar73[2]));
            func_00115D70((float *)(local_1f28 + 1),(float *)&local_1ebc,fVar55);
            local_1ec8 = local_1f28[1];
            local_1ec4 = (float ************)local_1f28[2];
            local_1ec0 = (float ************)local_1f28[3];
            func_00115C54((float *)(ppppppppppfVar73 + 0x15),(float *)(ppppppppppfVar73 + 0x15),
                          (float *)&local_1ec8);
          }
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 3),
                          ALBW_AS_F32(pppppppppppfVar34));
            local_1f28[1] = local_1ec8;
            local_1f28[2] = (float ***********)local_1ec4;
            local_1f28[3] = (float ***********)local_1ec0;
            func_00115D70((float *)&local_1ec8,(float *)(local_1f28 + 1),ALBW_AS_F32(uVar67));
            local_1ebc = local_1ec8;
            local_1eb8 = (float ***********)local_1ec4;
            local_1eb4 = local_1ec0;
            func_00115C54((float *)(ppppppppppfVar73 + 0xf),(float *)(ppppppppppfVar73 + 0xf),
                          (float *)&local_1ebc);
          }
          fVar55 = ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
          *(undefined1 *)(ppppppppppfVar73 + 0x25) = 1;
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
            if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
              func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 0x15),
                            (float *)(ppppppppppfVar73 + 0xf));
              func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 6),
                            ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ebc) +
                            ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1eb8) +
                            ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1eb4));
              local_1ec8 = local_1ebc;
              local_1ec4 = (float ************)local_1eb8;
              local_1ec0 = local_1eb4;
              func_00115C70((float *)(local_1f28 + 4),(float *)(ppppppppppfVar73 + 0x15),
                            (float *)&local_1ec8);
              local_1ea0 = local_1f28[4];
              local_1e9c = local_1f14;
              local_1e98 = local_1f10;
            }
            else {
              local_1ea0 = (float ***********)ppppppppppfVar73[0xf];
              local_1e9c = (float ***********)ppppppppppfVar73[0x10];
              local_1e98 = (float ***********)ppppppppppfVar73[0x11];
            }
            if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
              func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 0xf),
                            (float *)&local_1ea0);
              local_1f28[4] = local_1ec8;
              local_1f14 = (float ***********)local_1ec4;
              local_1f10 = (float ***********)local_1ec0;
              func_0068906C((float *)(local_1f28 + 1),(float *)(local_1f28 + 4));
            }
            else {
              func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 0x15),
                            (float *)&local_1ea0);
              local_1f28[4] = local_1ec8;
              local_1f14 = (float ***********)local_1ec4;
              local_1f10 = (float ***********)local_1ec0;
              func_0068906C((float *)(local_1f28 + 1),(float *)(local_1f28 + 4));
            }
            iVar21 = DAT_001f290c;
            fVar57 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[1]) +
                     ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[2]) +
                     ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f28[3]);
            fVar58 = fVar57;
            if (fVar57 <= fVar5) {
              fVar58 = -fVar57;
            }
            if (DAT_001f290c < (int)ALBW_AS_U32(fVar58)) {
              func_00115C70((float *)(local_1f28 + 4),(float *)&local_1ea0,
                            (float *)(ppppppppppfVar22 + 0xc));
              fVar58 = -(ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[4]) +
                        ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f14)) +
                       -(ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f10));
              if (fVar58 <= fVar5) {
                if ((uint)ALBW_AS_U32(DAT_001f3a84) < (uint)ALBW_AS_U32(fVar58)) {
                  func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 0xc),
                                (float *)(ppppppppppfVar73 + 9));
                  func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar22,
                                ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(local_1ec8) +
                                ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(local_1ec4) +
                                ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(local_1ec0));
                  local_1e94 = local_1ec8;
                  local_1e90 = (float ***********)local_1ec4;
                  local_1e8c = (float ***********)local_1ec0;
                  func_00115C54((float *)&local_1ec8,(float *)&local_1e78,(float *)&local_1e94);
                  ppppppppppfVar22[0x12] = (float *********)local_1ec8;
                  ppppppppppfVar22[0x13] = (float *********)local_1ec4;
                  ppppppppppfVar22[0x14] = (float *********)local_1ec0;
                  func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 3),
                                ALBW_AS_F32(pppppppppppfVar34));
                  local_1eac = (float ************)local_1ec8;
                  local_1ea8 = (float ***********)local_1ec4;
                  local_1ea4 = (float ***********)local_1ec0;
                  func_00115D70((float *)(local_1f28 + 4),(float *)&local_1eac,fVar55);
                  local_1e94 = local_1f28[4];
                  local_1e90 = local_1f14;
                  local_1e8c = local_1f10;
                  func_00115C54((float *)(ppppppppppfVar22 + 0x12),
                                (float *)(ppppppppppfVar22 + 0x12),(float *)&local_1e94);
                  *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
                  if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
                    func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x18),
                                  (float *)(ppppppppppfVar22 + 0x12));
                    func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 6),
                                  ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1ec8) +
                                  ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1ec4) +
                                  ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1ec0));
                    local_1ebc = local_1ec8;
                    local_1eb8 = (float ***********)local_1ec4;
                    local_1eb4 = local_1ec0;
                    func_00115C70((float *)(local_1f28 + 4),(float *)(ppppppppppfVar22 + 0x18),
                                  (float *)&local_1ebc);
                    local_1ea0 = local_1f28[4];
                    local_1e9c = local_1f14;
                    local_1e98 = local_1f10;
                  }
                  else {
                    local_1ea0 = (float ***********)ppppppppppfVar22[0x12];
                    local_1e9c = (float ***********)ppppppppppfVar22[0x13];
                    local_1e98 = (float ***********)ppppppppppfVar22[0x14];
                  }
                  if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
                    func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x12),
                                  (float *)&local_1ea0);
                    local_1f28[4] = local_1ec8;
                    local_1f14 = (float ***********)local_1ec4;
                    local_1f10 = (float ***********)local_1ec0;
                    func_0068906C((float *)(local_1f28 + 1),(float *)(local_1f28 + 4));
                  }
                  else {
                    func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x18),
                                  (float *)&local_1ea0);
                    local_1f28[4] = local_1ec8;
                    local_1f14 = (float ***********)local_1ec4;
                    local_1f10 = (float ***********)local_1ec0;
                    func_0068906C((float *)(local_1f28 + 1),(float *)(local_1f28 + 4));
                  }
                  fVar58 = ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[1]) +
                           ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[2]) +
                           ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[3]);
                  fVar55 = fVar58;
                  if (fVar58 <= fVar5) {
                    fVar55 = -fVar58;
                  }
                  if (iVar21 < (int)ALBW_AS_U32(fVar55)) {
                    func_00115C70((float *)(local_1f28 + 4),(float *)&local_1ea0,
                                  (float *)(ppppppppppfVar73 + 9));
                    fVar55 = -(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[4]) +
                              ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f14)) +
                             -(ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f10));
                    if (fVar5 < fVar55) {
                      func_00115D70((float *)(local_1f28 + 4),(float *)(local_1f28 + 1),fVar55);
                      local_1ebc = local_1f28[4];
                      local_1eb8 = local_1f14;
                      local_1eb4 = (float ************)local_1f10;
                      func_00115D88((float *)(local_1f28 + 4),(float *)&local_1ebc,fVar58);
                      local_1e3c = local_1f28[4];
                      local_1e38 = local_1f14;
                      local_1e34 = local_1f10;
                      func_00115C54((float *)(local_1f28 + 4),(float *)&local_1ea0,
                                    (float *)&local_1e3c);
                      local_1ec8 = local_1f28[4];
                      local_1ec4 = (float ************)local_1f14;
                      local_1ec0 = (float ************)local_1f10;
                      func_00115C70((float *)&local_1e88,(float *)(ppppppppppfVar73 + 0xf),
                                    (float *)&local_1ec8);
                      local_1ebc = local_1e88;
                      local_1eb8 = local_1e84;
                      local_1eb4 = (float ************)local_1e80;
                      if (fVar5 < ALBW_AS_F32(local_1e88) * ALBW_AS_F32(ppppppppppfVar73[3]) +
                                  ALBW_AS_F32(local_1e84) * ALBW_AS_F32(ppppppppppfVar73[4]) +
                                  ALBW_AS_F32(local_1e80) * ALBW_AS_F32(ppppppppppfVar73[5])) {
                        ppppppppppfVar73[0x15] = (float *********)local_1ec8;
                        ppppppppppfVar73[0x16] = (float *********)local_1ec4;
                        ppppppppppfVar73[0x17] = (float *********)local_1ec0;
                        *(undefined1 *)(ppppppppppfVar73 + 0x25) = 1;
                        if ((*(char *)((int)ppppppppppfVar73 + 0x97) != '\0') &&
                           (*(char *)((int)ppppppppppfVar73 + 0x95) == '\0')) {
                          ppppppppppfVar30 = ppppppppppfVar73 + 0x18;
                          ppppppppppfVar73[0x18] = ppppppppppfVar73[0x15];
                          ppppppppppfVar73[0x19] = ppppppppppfVar73[0x16];
                          pppppppppfVar48 = ppppppppppfVar73[0x17];
                          goto LAB_001f3b00;
                        }
                      }
                    }
                  }
                }
              }
              else {
                func_00115D70((float *)&local_1ebc,(float *)(local_1f28 + 1),fVar58);
                local_1f28[4] = local_1ebc;
                local_1f14 = local_1eb8;
                local_1f10 = (float ***********)local_1eb4;
                func_00115D88((float *)&local_1ebc,(float *)(local_1f28 + 4),fVar57);
                local_1eac = (float ************)local_1ebc;
                local_1ea8 = local_1eb8;
                local_1ea4 = (float ***********)local_1eb4;
                func_00115C54((float *)&local_1ebc,(float *)&local_1ea0,(float *)&local_1eac);
                local_1ec8 = local_1ebc;
                local_1ec4 = (float ************)local_1eb8;
                local_1ec0 = local_1eb4;
                func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 0x12),
                              (float *)&local_1ec8);
                local_1f28[4] = local_1ebc;
                local_1f14 = local_1eb8;
                local_1f10 = (float ***********)local_1eb4;
                if (fVar5 < ALBW_AS_F32(local_1ebc) * ALBW_AS_F32(ppppppppppfVar22[3]) +
                            ALBW_AS_F32(local_1eb8) * ALBW_AS_F32(ppppppppppfVar22[4]) +
                            ALBW_AS_F32(local_1eb4) * ALBW_AS_F32(ppppppppppfVar22[5])) {
                  ppppppppppfVar22[0x18] = (float *********)local_1ec8;
                  ppppppppppfVar22[0x19] = (float *********)local_1ec4;
                  ppppppppppfVar22[0x1a] = (float *********)local_1ec0;
                  *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
                  if ((*(char *)((int)ppppppppppfVar22 + 0x97) != '\0') &&
                     (*(char *)(ppppppppppfVar22 + 0x25) == '\0')) {
                    ppppppppppfVar30 = ppppppppppfVar22 + 0x15;
                    ppppppppppfVar22[0x15] = ppppppppppfVar22[0x18];
                    ppppppppppfVar22[0x16] = ppppppppppfVar22[0x19];
                    pppppppppfVar48 = ppppppppppfVar22[0x1a];
LAB_001f3b00:
                    ppppppppppfVar30[2] = pppppppppfVar48;
                  }
                }
              }
            }
          }
        }
        ppppppppppfVar56 = (float **********)((int)ppppppppppfVar56 + 1);
        local_1eb0 = (float ***********)((int)local_1eb0 + 1);
      } while ((int)ppppppppppfVar56 < (int)local_1f04);
      if (0 < (int)local_1eb0) {
        ppppppppppfVar22 = (float **********)0x0;
        do {
          if (ppppppppppfVar22 < *pppppppppppfVar15) {
            ppppppppppfVar73 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar22 * 0x27;
          }
          else {
            ppppppppppfVar73 = pppppppppppfVar49[0x2b];
          }
          *(undefined1 *)((int)ppppppppppfVar73 + 0x91) = 1;
          *(undefined1 *)((int)ppppppppppfVar73 + 0x92) = 1;
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            local_1e6c = (float ***********)ppppppppppfVar73[0x15];
            local_1e68 = (float ***********)ppppppppppfVar73[0x16];
            local_1e64 = (float ***********)ppppppppppfVar73[0x17];
            func_00115C70((float *)&local_1e94,(float *)&local_1e6c,
                          (float *)(ppppppppppfVar73 + 0xf));
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 6),
                          ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1e94) +
                          ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1e90) +
                          ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1e8c));
            local_1f28[2] = local_1ec8;
            local_1f28[3] = (float ***********)local_1ec4;
            local_1f28[4] = (float ***********)local_1ec0;
            func_00115C70((float *)&local_1ea0,(float *)&local_1e6c,(float *)(local_1f28 + 2));
            ppppppppppfVar73[0x15] = (float *********)local_1ea0;
            ppppppppppfVar73[0x16] = (float *********)local_1e9c;
            ppppppppppfVar73[0x17] = (float *********)local_1e98;
            local_1e6c = (float ***********)ppppppppppfVar73[0x18];
            local_1e68 = (float ***********)ppppppppppfVar73[0x19];
            local_1e64 = (float ***********)ppppppppppfVar73[0x1a];
            func_00115C70((float *)&local_1eac,(float *)&local_1e6c,
                          (float *)(ppppppppppfVar73 + 0x12));
            func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 6),
                          ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1eac) +
                          ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ea8) +
                          ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ea4));
            local_1f28[2] = local_1ebc;
            local_1f28[3] = local_1eb8;
            local_1f28[4] = (float ***********)local_1eb4;
            func_00115C70((float *)&local_1e88,(float *)&local_1e6c,(float *)(local_1f28 + 2));
            ppppppppppfVar73[0x18] = (float *********)local_1e88;
            ppppppppppfVar73[0x19] = (float *********)local_1e84;
            ppppppppppfVar73[0x1a] = (float *********)local_1e80;
          }
          ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
        } while ((int)ppppppppppfVar22 <= (int)local_1eb0);
      }
    }
    ppppppppppfVar22 = (float **********)((int)local_1f04 + -1);
    if (((int)ppppppppppfVar56 < (int)local_1f04) &&
       (pppppppppppfVar34 = pppppppppppfVar74, (int)ppppppppppfVar56 <= (int)ppppppppppfVar22)) {
      do {
        if ((float **********)((int)ppppppppppfVar22 + -1) < *pppppppppppfVar15) {
          ppppppppppfVar73 = pppppppppppfVar49[0x2b] + ((int)ppppppppppfVar22 + -1) * 0x27;
        }
        else {
          ppppppppppfVar73 = pppppppppppfVar49[0x2b];
        }
        if (ppppppppppfVar22 < *pppppppppppfVar15) {
          ppppppppppfVar30 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar22 * 0x27;
        }
        else {
          ppppppppppfVar30 = pppppppppppfVar49[0x2b];
        }
        if ((((int)ppppppppppfVar73[7] <= DAT_001f4bdc) &&
            ((int)ppppppppppfVar30[7] <= DAT_001f4bdc)) ||
           ((int)ALBW_AS_U32((ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(ppppppppppfVar30[6]) +
                  ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(ppppppppppfVar30[7]) +
                 ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(ppppppppppfVar30[8]))) <= DAT_001f3a80)) break;
        fVar55 = ALBW_AS_F32(pppppppppppfVar17[0x1c]) / ALBW_AS_F32(ppppppppppfVar73[4]);
        if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar73 + 0xc);
        }
        else {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar73 + 0x15);
        }
        uVar67 = CONCAT44(fVar55,ALBW_AS_F32(pppppppppppfVar17[0x1c]) / ALBW_AS_F32(ppppppppppfVar30[4]));
        local_1e4c = pppppppppppfVar78;
        func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 3),fVar55);
        local_1ec8 = local_1ebc;
        local_1ec4 = (float ************)local_1eb8;
        local_1ec0 = local_1eb4;
        func_00115C70((float *)&local_1ebc,(float *)pppppppppppfVar78,(float *)&local_1ec8);
        local_1f28[3] = local_1ebc;
        local_1f28[4] = local_1eb8;
        local_1f14 = (float ***********)local_1eb4;
        local_1e98 = local_1ebc;
        local_1e94 = local_1eb8;
        local_1e90 = (float ***********)local_1eb4;
        if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar30 + 9);
        }
        else {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar30 + 0x15);
        }
        local_1f10 = pppppppppppfVar78;
        func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar30 + 3),ALBW_AS_F32(uVar67));
        local_1f28[3] = local_1ebc;
        local_1f28[4] = local_1eb8;
        local_1f14 = (float ***********)local_1eb4;
        func_00115C70((float *)&local_1ebc,(float *)pppppppppppfVar78,(float *)(local_1f28 + 3));
        local_1ec8 = local_1ebc;
        local_1ec4 = (float ************)local_1eb8;
        local_1ec0 = local_1eb4;
        local_1e8c = local_1ebc;
        local_1e88 = local_1eb8;
        local_1e84 = (float ***********)local_1eb4;
        fVar55 = ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(ppppppppppfVar30[6]) +
                 ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(ppppppppppfVar30[7]) +
                 ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(ppppppppppfVar30[8]);
        func_00115C70((float *)(local_1f28 + 1),(float *)&local_1e98,(float *)local_1f10);
        fVar55 = (-(ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1f28[1]) +
                   ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1f28[2])) +
                 -(ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1f28[3]))) / fVar55;
        lVar69 = (ulonglong)(uint)ALBW_AS_U32(fVar55) << 0x20;
        if (DAT_001f4bdc < (int)ALBW_AS_U32(fVar55)) {
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            ppppppppppfVar11 = *pppppppppppfVar15;
            bVar47 = ppppppppppfVar22 <= ppppppppppfVar11;
            bVar44 = ppppppppppfVar11 == ppppppppppfVar22;
            ppppppppppfVar10 = ppppppppppfVar22;
            fVar58 = fVar5;
            if ((int)ppppppppppfVar22 < (int)ppppppppppfVar11) {
              do {
                if (!bVar47 || bVar44) {
                  ppppppppppfVar11 = pppppppppppfVar49[0x2b];
                }
                else {
                  ppppppppppfVar11 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar10 * 0x27;
                }
                func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar11 + 0xc),
                              (float *)(ppppppppppfVar11 + 9));
                fVar55 = ALBW_AS_F32(((ulonglong)lVar69 >> 0x20));
                ppppppppppfVar10 = (float **********)((int)ppppppppppfVar10 + 1);
                ppppppppppfVar12 = *pppppppppppfVar15;
                fVar57 = ALBW_AS_F32(*ppppppppppfVar11) * ALBW_AS_F32(local_1ec8) +
                         ALBW_AS_F32(ppppppppppfVar11[1]) * ALBW_AS_F32(local_1ec4) +
                         ALBW_AS_F32(ppppppppppfVar11[2]) * ALBW_AS_F32(local_1ec0);
                if (fVar57 <= fVar5) {
                  fVar57 = -fVar57;
                }
                bVar47 = ppppppppppfVar10 <= ppppppppppfVar12;
                bVar44 = ppppppppppfVar12 == ppppppppppfVar10;
                fVar58 = fVar57 + fVar58;
              } while (!bVar44 && (int)ppppppppppfVar10 <= (int)ppppppppppfVar12);
            }
            if (fVar58 < fVar55) {
              fVar55 = fVar58;
            }
            if (*(char *)((int)ppppppppppfVar73 + 0x93) != '\0') goto LAB_001f3f08;
            func_00115D70((float *)&local_1e18,(float *)ppppppppppfVar73,fVar55);
            local_1eb0 = local_1e18;
            local_1eac = local_1e14;
            local_1ea8 = (float ***********)local_1e10;
            func_00115C54((float *)&local_1e00,(float *)&local_1e98,(float *)&local_1eb0);
            ppppppppppfVar73[0x12] = (float *********)local_1e00;
            ppppppppppfVar73[0x13] = (float *********)local_1dfc;
            ppppppppppfVar73[0x14] = (float *********)local_1df8;
          }
          else {
LAB_001f3f08:
            func_00115D70((float *)&local_1e0c,(float *)ppppppppppfVar73,fVar55);
            local_1ea4 = local_1e0c;
            local_1ea0 = local_1e08;
            local_1e9c = local_1e04;
            func_00115C54((float *)(ppppppppppfVar73 + 0x18),(float *)(ppppppppppfVar73 + 0x18),
                          (float *)&local_1ea4);
          }
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 3),
                          ALBW_AS_F32(pppppppppppfVar34));
            local_1f28[2] = local_1ec8;
            local_1f28[3] = (float ***********)local_1ec4;
            local_1f28[4] = (float ***********)local_1ec0;
            func_00115D70((float *)&local_1ec8,(float *)(local_1f28 + 2),
                          ALBW_AS_F32(((ulonglong)uVar67 >> 0x20)));
            local_1e24 = local_1ec8;
            local_1e20 = (float ***********)local_1ec4;
            local_1e1c = (float ***********)local_1ec0;
            func_00115C54((float *)(ppppppppppfVar73 + 0x12),(float *)(ppppppppppfVar73 + 0x12),
                          (float *)&local_1e24);
          }
          *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 1;
          if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
            if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
              local_1f10 = (float ***********)(ppppppppppfVar73 + 0x18);
              func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar73 + 0x18),
                            (float *)(ppppppppppfVar73 + 0x12));
              func_00115D70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar73 + 6),
                            ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1eb0) +
                            ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1eac) +
                            ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ea8));
              local_1ebc = local_1f28[3];
              local_1eb8 = local_1f28[4];
              local_1eb4 = (float ************)local_1f14;
              func_00115C70((float *)(local_1f28 + 1),(float *)local_1f10,(float *)&local_1ebc);
              local_1e78 = local_1f28[1];
              local_1e74 = local_1f28[2];
              local_1e70 = local_1f28[3];
            }
            else {
              local_1e78 = (float ***********)ppppppppppfVar73[0x12];
              local_1e74 = (float ***********)ppppppppppfVar73[0x13];
              local_1e70 = (float ***********)ppppppppppfVar73[0x14];
            }
            if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
              func_00115C70((float *)(local_1f28 + 4),(float *)(ppppppppppfVar73 + 0x12),
                            (float *)&local_1e78);
              local_1f28[1] = local_1f28[4];
              local_1f28[2] = local_1f14;
              local_1f28[3] = local_1f10;
              func_0068906C((float *)&local_1ec8,(float *)(local_1f28 + 1));
            }
            else {
              func_00115C70((float *)(local_1f28 + 4),(float *)(ppppppppppfVar73 + 0x18),
                            (float *)&local_1e78);
              local_1f28[1] = local_1f28[4];
              local_1f28[2] = local_1f14;
              local_1f28[3] = local_1f10;
              func_0068906C((float *)&local_1ec8,(float *)(local_1f28 + 1));
            }
            fVar58 = ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1ec8) +
                     ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1ec4) +
                     ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1ec0);
            fVar55 = fVar58;
            if (fVar58 <= fVar5) {
              fVar55 = -fVar58;
            }
            if (DAT_001f4be0 < (int)ALBW_AS_U32(fVar55)) {
              func_00115C70((float *)(local_1f28 + 1),(float *)&local_1e78,
                            (float *)(ppppppppppfVar30 + 9));
              fVar55 = -(ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1f28[1]) +
                         ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1f28[2]) +
                        ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1f28[3]));
              if (fVar55 <= fVar5) {
                if ((uint)ALBW_AS_U32(DAT_001f3a84) < (uint)ALBW_AS_U32(fVar55)) {
                  func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 0xc),
                                (float *)(ppppppppppfVar73 + 9));
                  func_00115D70((float *)&local_1ebc,(float *)ppppppppppfVar30,
                                ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(local_1ebc) +
                                ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(local_1eb8) +
                                ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(local_1eb4));
                  local_1ea4 = local_1ebc;
                  local_1ea0 = local_1eb8;
                  local_1e9c = (float ***********)local_1eb4;
                  func_00115C70((float *)&local_1ebc,(float *)&local_1e8c,(float *)&local_1ea4);
                  ppppppppppfVar30[0xf] = (float *********)local_1ebc;
                  ppppppppppfVar30[0x10] = (float *********)local_1eb8;
                  ppppppppppfVar30[0x11] = (float *********)local_1eb4;
                  func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar30 + 3),
                                ALBW_AS_F32(pppppppppppfVar34));
                  local_1f28[1] = local_1ebc;
                  local_1f28[2] = local_1eb8;
                  local_1f28[3] = (float ***********)local_1eb4;
                  func_00115D70((float *)&local_1ebc,(float *)(local_1f28 + 1),ALBW_AS_F32(uVar67));
                  local_1ea4 = local_1ebc;
                  local_1ea0 = local_1eb8;
                  local_1e9c = (float ***********)local_1eb4;
                  func_00115C54((float *)(ppppppppppfVar30 + 0xf),(float *)(ppppppppppfVar30 + 0xf),
                                (float *)&local_1ea4);
                  *(undefined1 *)(ppppppppppfVar30 + 0x25) = 1;
                  if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
                    local_1f08 = (float ***********)(ppppppppppfVar30 + 0x15);
                    local_1f14 = (float ***********)(ppppppppppfVar30 + 6);
                    func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar30 + 0x15),
                                  (float *)(ppppppppppfVar30 + 0xf));
                    func_00115D70((float *)&local_1eb0,(float *)local_1f14,
                                  ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1eb0) +
                                  ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1eac) +
                                  ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1ea8));
                    local_1ebc = local_1eb0;
                    local_1eb8 = (float ***********)local_1eac;
                    local_1eb4 = (float ************)local_1ea8;
                    func_00115C70((float *)(local_1f28 + 4),(float *)local_1f08,(float *)&local_1ebc
                                 );
                    local_1e78 = local_1f28[4];
                    local_1e74 = local_1f14;
                    local_1e70 = local_1f10;
                  }
                  else {
                    local_1e78 = (float ***********)ppppppppppfVar30[0xf];
                    local_1e74 = (float ***********)ppppppppppfVar30[0x10];
                    local_1e70 = (float ***********)ppppppppppfVar30[0x11];
                  }
                  if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
                    func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar30 + 0xf),
                                  (float *)&local_1e78);
                    local_1f28[4] = local_1ebc;
                    local_1f14 = local_1eb8;
                    local_1f10 = (float ***********)local_1eb4;
                    func_0068906C((float *)&local_1ec8,(float *)(local_1f28 + 4));
                  }
                  else {
                    func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar30 + 0x15),
                                  (float *)&local_1e78);
                    local_1f28[4] = local_1ebc;
                    local_1f14 = local_1eb8;
                    local_1f10 = (float ***********)local_1eb4;
                    func_0068906C((float *)&local_1ec8,(float *)(local_1f28 + 4));
                  }
                  fVar58 = ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ec8) +
                           ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ec4) +
                           ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ec0);
                  fVar55 = fVar58;
                  if (fVar58 <= fVar5) {
                    fVar55 = -fVar58;
                  }
                  if (DAT_001f4be0 < (int)ALBW_AS_U32(fVar55)) {
                    func_00115C70((float *)&local_1ebc,(float *)&local_1e78,
                                  (float *)(ppppppppppfVar73 + 0xc));
                    fVar55 = -(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ebc) +
                               ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1eb8) +
                              ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1eb4));
                    if (fVar5 < fVar55) {
                      func_00115D70((float *)&local_1e24,(float *)&local_1ec8,fVar55);
                      local_1ebc = local_1e24;
                      local_1eb8 = local_1e20;
                      local_1eb4 = (float ************)local_1e1c;
                      func_00115D88((float *)(local_1f28 + 4),(float *)&local_1ebc,fVar58);
                      local_1e3c = local_1f28[4];
                      local_1e38 = local_1f14;
                      local_1e34 = local_1f10;
                      func_00115C54((float *)(local_1f28 + 4),(float *)&local_1e78,
                                    (float *)&local_1e3c);
                      local_1eb0 = local_1f28[4];
                      local_1eac = (float ************)local_1f14;
                      local_1ea8 = local_1f10;
                      func_00115C70((float *)&local_1e24,(float *)(ppppppppppfVar73 + 0x12),
                                    (float *)&local_1eb0);
                      local_1ebc = local_1e24;
                      local_1eb8 = local_1e20;
                      local_1eb4 = (float ************)local_1e1c;
                      if (fVar5 < ALBW_AS_F32(local_1e24) * ALBW_AS_F32(ppppppppppfVar73[3]) +
                                  ALBW_AS_F32(local_1e20) * ALBW_AS_F32(ppppppppppfVar73[4]) +
                                  ALBW_AS_F32(local_1e1c) * ALBW_AS_F32(ppppppppppfVar73[5])) {
                        ppppppppppfVar73[0x18] = (float *********)local_1eb0;
                        ppppppppppfVar73[0x19] = (float *********)local_1eac;
                        ppppppppppfVar73[0x1a] = (float *********)local_1ea8;
                        *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 1;
                        if ((*(char *)((int)ppppppppppfVar73 + 0x97) != '\0') &&
                           (*(char *)(ppppppppppfVar73 + 0x25) == '\0')) {
                          ppppppppppfVar10 = ppppppppppfVar73 + 0x15;
                          ppppppppppfVar73[0x15] = ppppppppppfVar73[0x18];
                          ppppppppppfVar73[0x16] = ppppppppppfVar73[0x19];
                          pppppppppfVar48 = ppppppppppfVar73[0x1a];
                          goto LAB_001f45b8;
                        }
                      }
                    }
                  }
                }
              }
              else {
                func_00115D70((float *)&local_1ea4,(float *)&local_1ec8,fVar55);
                local_1f28[2] = local_1ea4;
                local_1f28[3] = local_1ea0;
                local_1f28[4] = local_1e9c;
                func_00115D88((float *)&local_1ea4,(float *)(local_1f28 + 2),fVar58);
                local_1eb0 = local_1ea4;
                local_1eac = (float ************)local_1ea0;
                local_1ea8 = local_1e9c;
                func_00115C54((float *)&local_1ea4,(float *)&local_1e78,(float *)&local_1eb0);
                local_1ebc = local_1ea4;
                local_1eb8 = local_1ea0;
                local_1eb4 = (float ************)local_1e9c;
                func_00115C70((float *)&local_1ea4,(float *)(ppppppppppfVar30 + 0xf),
                              (float *)&local_1ebc);
                local_1f28[2] = local_1ea4;
                local_1f28[3] = local_1ea0;
                local_1f28[4] = local_1e9c;
                if (fVar5 < ALBW_AS_F32(local_1ea4) * ALBW_AS_F32(ppppppppppfVar30[3]) +
                            ALBW_AS_F32(local_1ea0) * ALBW_AS_F32(ppppppppppfVar30[4]) +
                            ALBW_AS_F32(local_1e9c) * ALBW_AS_F32(ppppppppppfVar30[5])) {
                  ppppppppppfVar30[0x15] = (float *********)local_1ebc;
                  ppppppppppfVar30[0x16] = (float *********)local_1eb8;
                  ppppppppppfVar30[0x17] = (float *********)local_1eb4;
                  *(undefined1 *)(ppppppppppfVar30 + 0x25) = 1;
                  if ((*(char *)((int)ppppppppppfVar30 + 0x97) != '\0') &&
                     (*(char *)((int)ppppppppppfVar30 + 0x95) == '\0')) {
                    ppppppppppfVar10 = ppppppppppfVar30 + 0x18;
                    ppppppppppfVar30[0x18] = ppppppppppfVar30[0x15];
                    ppppppppppfVar30[0x19] = ppppppppppfVar30[0x16];
                    pppppppppfVar48 = ppppppppppfVar30[0x17];
LAB_001f45b8:
                    ppppppppppfVar10[2] = pppppppppfVar48;
                  }
                }
              }
            }
          }
        }
        fVar55 = -(ALBW_AS_F32(*ppppppppppfVar30) * ALBW_AS_F32(ppppppppppfVar73[6]) +
                   ALBW_AS_F32(ppppppppppfVar30[1]) * ALBW_AS_F32(ppppppppppfVar73[7]) +
                  ALBW_AS_F32(ppppppppppfVar30[2]) * ALBW_AS_F32(ppppppppppfVar73[8]));
        func_00115C70((float *)(local_1f28 + 1),(float *)&local_1e8c,(float *)local_1e4c);
        fVar55 = (-(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[1]) +
                   ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[2])) +
                 -(ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[3]))) / fVar55;
        lVar69 = (ulonglong)(uint)ALBW_AS_U32(fVar55) << 0x20;
        if (DAT_001f4bdc < (int)ALBW_AS_U32(fVar55)) {
          if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
            ppppppppppfVar10 = (float **********)0x0;
            fVar58 = fVar5;
            if (0 < (int)ppppppppppfVar22) {
              do {
                if (ppppppppppfVar10 < *pppppppppppfVar15) {
                  ppppppppppfVar11 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar10 * 0x27;
                }
                else {
                  ppppppppppfVar11 = pppppppppppfVar49[0x2b];
                }
                func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar11 + 0xc),
                              (float *)(ppppppppppfVar11 + 9));
                fVar55 = ALBW_AS_F32(((ulonglong)lVar69 >> 0x20));
                ppppppppppfVar10 = (float **********)((int)ppppppppppfVar10 + 1);
                fVar57 = ALBW_AS_F32(*ppppppppppfVar11) * ALBW_AS_F32(local_1ec8) +
                         ALBW_AS_F32(ppppppppppfVar11[1]) * ALBW_AS_F32(local_1ec4) +
                         ALBW_AS_F32(ppppppppppfVar11[2]) * ALBW_AS_F32(local_1ec0);
                if (fVar57 <= fVar5) {
                  fVar57 = -fVar57;
                }
                fVar58 = fVar57 + fVar58;
              } while ((int)ppppppppppfVar10 < (int)ppppppppppfVar22);
            }
            if (fVar58 < fVar55) {
              fVar55 = fVar58;
            }
            if (*(char *)((int)ppppppppppfVar30 + 0x93) != '\0') goto LAB_001f46bc;
            func_00115D70((float *)(local_1f28 + 4),(float *)ppppppppppfVar30,fVar55);
            local_1ec8 = local_1f28[4];
            local_1ec4 = (float ************)local_1f14;
            local_1ec0 = (float ************)local_1f10;
            func_00115C70((float *)(local_1f28 + 3),(float *)&local_1e8c,(float *)&local_1ec8);
            ppppppppppfVar30[0xf] = (float *********)local_1f28[3];
            ppppppppppfVar30[0x10] = (float *********)local_1f28[4];
            ppppppppppfVar30[0x11] = (float *********)local_1f14;
          }
          else {
LAB_001f46bc:
            local_1ec8 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(*ppppppppppfVar30));
            local_1ec4 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar30[1]));
            local_1ec0 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar30[2]));
            func_00115D70((float *)(local_1f28 + 1),(float *)&local_1ec8,fVar55);
            local_1f28[4] = local_1f28[1];
            local_1f14 = local_1f28[2];
            local_1f10 = local_1f28[3];
            func_00115C54((float *)(ppppppppppfVar30 + 0x15),(float *)(ppppppppppfVar30 + 0x15),
                          (float *)(local_1f28 + 4));
          }
          if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
            func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar30 + 3),
                          ALBW_AS_F32(pppppppppppfVar34));
            local_1f28[2] = local_1ebc;
            local_1f28[3] = local_1eb8;
            local_1f28[4] = (float ***********)local_1eb4;
            func_00115D70((float *)&local_1ebc,(float *)(local_1f28 + 2),ALBW_AS_F32(uVar67));
            local_1ec8 = local_1ebc;
            local_1ec4 = (float ************)local_1eb8;
            local_1ec0 = local_1eb4;
            func_00115C54((float *)(ppppppppppfVar30 + 0xf),(float *)(ppppppppppfVar30 + 0xf),
                          (float *)&local_1ec8);
          }
          fVar55 = ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
          *(undefined1 *)(ppppppppppfVar30 + 0x25) = 1;
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
              func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar30 + 0x15),
                            (float *)(ppppppppppfVar30 + 0xf));
              func_00115D70((float *)&local_1eb0,(float *)(ppppppppppfVar30 + 6),
                            ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1eb0) +
                            ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1eac) +
                            ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1ea8));
              local_1ec8 = local_1eb0;
              local_1ec4 = local_1eac;
              local_1ec0 = (float ************)local_1ea8;
              func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar30 + 0x15),
                            (float *)&local_1ec8);
              local_1ebc = local_1eb0;
              local_1eb8 = (float ***********)local_1eac;
              local_1eb4 = (float ************)local_1ea8;
            }
            else {
              local_1ebc = (float ***********)ppppppppppfVar30[0xf];
              local_1eb8 = (float ***********)ppppppppppfVar30[0x10];
              local_1eb4 = (float ************)ppppppppppfVar30[0x11];
            }
            if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
              func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar30 + 0xf),
                            (float *)&local_1ebc);
              local_1ec8 = local_1eb0;
              local_1ec4 = local_1eac;
              local_1ec0 = (float ************)local_1ea8;
              func_0068906C((float *)(local_1f28 + 3),(float *)&local_1ec8);
            }
            else {
              func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar30 + 0x15),
                            (float *)&local_1ebc);
              local_1ec8 = local_1eb0;
              local_1ec4 = local_1eac;
              local_1ec0 = (float ************)local_1ea8;
              func_0068906C((float *)(local_1f28 + 3),(float *)&local_1ec8);
            }
            iVar21 = DAT_001f4be0;
            fVar57 = ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[3]) +
                     ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[4]) +
                     ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f14);
            fVar58 = fVar57;
            if (fVar57 <= fVar5) {
              fVar58 = -fVar57;
            }
            if (DAT_001f4be0 < (int)ALBW_AS_U32(fVar58)) {
              func_00115C70((float *)&local_1ec8,(float *)&local_1ebc,
                            (float *)(ppppppppppfVar73 + 0xc));
              fVar58 = -(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ec8) +
                         ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ec4) +
                        ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ec0));
              if (fVar58 <= fVar5) {
                if ((uint)ALBW_AS_U32(DAT_001f3a84) < (uint)ALBW_AS_U32(fVar58)) {
                  func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar30 + 0xc),
                                (float *)(ppppppppppfVar30 + 9));
                  func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar73,
                                ALBW_AS_F32(*ppppppppppfVar30) * ALBW_AS_F32(local_1ec8) +
                                ALBW_AS_F32(ppppppppppfVar30[1]) * ALBW_AS_F32(local_1ec4) +
                                ALBW_AS_F32(ppppppppppfVar30[2]) * ALBW_AS_F32(local_1ec0));
                  local_1e7c = local_1ec8;
                  local_1e78 = (float ***********)local_1ec4;
                  local_1e74 = (float ***********)local_1ec0;
                  func_00115C54((float *)&local_1ec8,(float *)&local_1e98,(float *)&local_1e7c);
                  ppppppppppfVar73[0x12] = (float *********)local_1ec8;
                  ppppppppppfVar73[0x13] = (float *********)local_1ec4;
                  ppppppppppfVar73[0x14] = (float *********)local_1ec0;
                  func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 3),
                                ALBW_AS_F32(pppppppppppfVar34));
                  local_1eb0 = local_1ec8;
                  local_1eac = local_1ec4;
                  local_1ea8 = (float ***********)local_1ec0;
                  func_00115D70((float *)&local_1ec8,(float *)&local_1eb0,fVar55);
                  local_1e7c = local_1ec8;
                  local_1e78 = (float ***********)local_1ec4;
                  local_1e74 = (float ***********)local_1ec0;
                  func_00115C54((float *)(ppppppppppfVar73 + 0x12),
                                (float *)(ppppppppppfVar73 + 0x12),(float *)&local_1e7c);
                  *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 1;
                  if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
                    local_1f00[1] = (float ***********)(ppppppppppfVar73 + 0x18);
                    func_00115C70((float *)&local_1ea4,(float *)(ppppppppppfVar73 + 0x18),
                                  (float *)(ppppppppppfVar73 + 0x12));
                    func_00115D70((float *)&local_1ea4,(float *)(ppppppppppfVar73 + 6),
                                  ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ea4) +
                                  ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ea0) +
                                  ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1e9c));
                    local_1ec8 = local_1ea4;
                    local_1ec4 = (float ************)local_1ea0;
                    local_1ec0 = (float ************)local_1e9c;
                    func_00115C70((float *)&local_1ea4,(float *)local_1f00[1],(float *)&local_1ec8);
                    local_1ebc = local_1ea4;
                    local_1eb8 = local_1ea0;
                    local_1eb4 = (float ************)local_1e9c;
                  }
                  else {
                    local_1ebc = (float ***********)ppppppppppfVar73[0x12];
                    local_1eb8 = (float ***********)ppppppppppfVar73[0x13];
                    local_1eb4 = (float ************)ppppppppppfVar73[0x14];
                  }
                  if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
                    func_00115C70((float *)local_1f28,(float *)(ppppppppppfVar73 + 0x12),
                                  (float *)&local_1ebc);
                    local_1ec8 = local_1f28[0];
                    local_1ec4 = (float ************)local_1f28[1];
                    local_1ec0 = (float ************)local_1f28[2];
                    func_0068906C((float *)(local_1f28 + 3),(float *)&local_1ec8);
                  }
                  else {
                    func_00115C70((float *)local_1f28,(float *)(ppppppppppfVar73 + 0x18),
                                  (float *)&local_1ebc);
                    local_1ec8 = local_1f28[0];
                    local_1ec4 = (float ************)local_1f28[1];
                    local_1ec0 = (float ************)local_1f28[2];
                    func_0068906C((float *)(local_1f28 + 3),(float *)&local_1ec8);
                  }
                  fVar58 = ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1f28[3]) +
                           ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1f28[4]) +
                           ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1f14);
                  fVar55 = fVar58;
                  if (fVar58 <= fVar5) {
                    fVar55 = -fVar58;
                  }
                  if (iVar21 < (int)ALBW_AS_U32(fVar55)) {
                    func_00115C70((float *)&local_1ec8,(float *)&local_1ebc,
                                  (float *)(ppppppppppfVar30 + 9));
                    fVar55 = -(ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1ec8) +
                              ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1ec4)) +
                             -(ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1ec0));
                    if (fVar5 < fVar55) {
                      func_00115D70((float *)&local_1e3c,(float *)(local_1f28 + 3),fVar55);
                      local_1ec8 = local_1e3c;
                      local_1ec4 = (float ************)local_1e38;
                      local_1ec0 = (float ************)local_1e34;
                      func_00115D88((float *)local_1f28,(float *)&local_1ec8,fVar58);
                      local_1e4c = local_1f28[0];
                      local_1e48 = local_1f28[1];
                      local_1e44 = local_1f28[2];
                      func_00115C54((float *)local_1f28,(float *)&local_1ebc,(float *)&local_1e4c);
                      local_1ea4 = local_1f28[0];
                      local_1ea0 = local_1f28[1];
                      local_1e9c = local_1f28[2];
                      func_00115C70((float *)&local_1e3c,(float *)(ppppppppppfVar30 + 0xf),
                                    (float *)&local_1ea4);
                      local_1ec8 = local_1e3c;
                      local_1ec4 = (float ************)local_1e38;
                      local_1ec0 = (float ************)local_1e34;
                      if (fVar5 < ALBW_AS_F32(local_1e3c) * ALBW_AS_F32(ppppppppppfVar30[3]) +
                                  ALBW_AS_F32(local_1e38) * ALBW_AS_F32(ppppppppppfVar30[4]) +
                                  ALBW_AS_F32(local_1e34) * ALBW_AS_F32(ppppppppppfVar30[5])) {
                        ppppppppppfVar30[0x15] = (float *********)local_1ea4;
                        ppppppppppfVar30[0x16] = (float *********)local_1ea0;
                        ppppppppppfVar30[0x17] = (float *********)local_1e9c;
                        *(undefined1 *)(ppppppppppfVar30 + 0x25) = 1;
                        if ((*(char *)((int)ppppppppppfVar30 + 0x97) != '\0') &&
                           (*(char *)((int)ppppppppppfVar30 + 0x95) == '\0')) {
                          ppppppppppfVar10 = ppppppppppfVar30 + 0x18;
                          ppppppppppfVar30[0x18] = ppppppppppfVar30[0x15];
                          ppppppppppfVar30[0x19] = ppppppppppfVar30[0x16];
                          pppppppppfVar48 = ppppppppppfVar30[0x17];
                          goto LAB_001f4d74;
                        }
                      }
                    }
                  }
                }
              }
              else {
                func_00115D70((float *)&local_1e7c,(float *)(local_1f28 + 3),fVar58);
                local_1ec8 = local_1e7c;
                local_1ec4 = (float ************)local_1e78;
                local_1ec0 = (float ************)local_1e74;
                func_00115D88((float *)&local_1e7c,(float *)&local_1ec8,fVar57);
                local_1ea4 = local_1e7c;
                local_1ea0 = local_1e78;
                local_1e9c = local_1e74;
                func_00115C54((float *)&local_1e7c,(float *)&local_1ebc,(float *)&local_1ea4);
                local_1eb0 = local_1e7c;
                local_1eac = (float ************)local_1e78;
                local_1ea8 = local_1e74;
                func_00115C70((float *)&local_1e7c,(float *)(ppppppppppfVar73 + 0x12),
                              (float *)&local_1eb0);
                local_1ec8 = local_1e7c;
                local_1ec4 = (float ************)local_1e78;
                local_1ec0 = (float ************)local_1e74;
                if (fVar5 < ALBW_AS_F32(local_1e7c) * ALBW_AS_F32(ppppppppppfVar73[3]) +
                            ALBW_AS_F32(local_1e78) * ALBW_AS_F32(ppppppppppfVar73[4]) +
                            ALBW_AS_F32(local_1e74) * ALBW_AS_F32(ppppppppppfVar73[5])) {
                  ppppppppppfVar73[0x18] = (float *********)local_1eb0;
                  ppppppppppfVar73[0x19] = (float *********)local_1eac;
                  ppppppppppfVar73[0x1a] = (float *********)local_1ea8;
                  *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 1;
                  if ((*(char *)((int)ppppppppppfVar73 + 0x97) != '\0') &&
                     (*(char *)(ppppppppppfVar73 + 0x25) == '\0')) {
                    ppppppppppfVar10 = ppppppppppfVar73 + 0x15;
                    ppppppppppfVar73[0x15] = ppppppppppfVar73[0x18];
                    ppppppppppfVar73[0x16] = ppppppppppfVar73[0x19];
                    pppppppppfVar48 = ppppppppppfVar73[0x1a];
LAB_001f4d74:
                    ppppppppppfVar10[2] = pppppppppfVar48;
                  }
                }
              }
            }
          }
        }
        ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + -1);
        local_1e40 = (float ***********)((int)local_1e40 + 1);
      } while ((int)ppppppppppfVar56 <= (int)ppppppppppfVar22);
      if ((0 < (int)local_1e40) &&
         (ppppppppppfVar56 = (float **********)((int)local_1f04 + -1),
         (int)ppppppppppfVar22 <= (int)ppppppppppfVar56)) {
        do {
          if (ppppppppppfVar56 < *pppppppppppfVar15) {
            ppppppppppfVar73 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar56 * 0x27;
          }
          else {
            ppppppppppfVar73 = pppppppppppfVar49[0x2b];
          }
          *(undefined1 *)((int)ppppppppppfVar73 + 0x91) = 1;
          *(undefined1 *)((int)ppppppppppfVar73 + 0x92) = 1;
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            local_1e6c = (float ***********)ppppppppppfVar73[0x15];
            local_1e68 = (float ***********)ppppppppppfVar73[0x16];
            local_1e64 = (float ***********)ppppppppppfVar73[0x17];
            func_00115C70((float *)&local_1eb0,(float *)&local_1e6c,
                          (float *)(ppppppppppfVar73 + 0xf));
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 6),
                          ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1eb0) +
                          ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1eac) +
                          ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ea8));
            local_1f08 = local_1ec8;
            local_1f04 = (float ***********)local_1ec4;
            local_1f00[0] = (float ***********)local_1ec0;
            func_00115C70((float *)&local_1ebc,(float *)&local_1e6c,(float *)&local_1f08);
            ppppppppppfVar73[0x15] = (float *********)local_1ebc;
            ppppppppppfVar73[0x16] = (float *********)local_1eb8;
            ppppppppppfVar73[0x17] = (float *********)local_1eb4;
            local_1e6c = (float ***********)ppppppppppfVar73[0x18];
            local_1e68 = (float ***********)ppppppppppfVar73[0x19];
            local_1e64 = (float ***********)ppppppppppfVar73[0x1a];
            func_00115C70((float *)&stack0xffffe0d4,(float *)&local_1e6c,
                          (float *)(ppppppppppfVar73 + 0x12));
            func_00115D70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar73 + 6),
                          ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(in_stack_ffffe0d4) +
                          ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[0]) +
                          ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[1]));
            local_1f08 = local_1f28[3];
            local_1f04 = local_1f28[4];
            local_1f00[0] = local_1f14;
            func_00115C70((float *)&local_1ea4,(float *)&local_1e6c,(float *)&local_1f08);
            ppppppppppfVar73[0x18] = (float *********)local_1ea4;
            ppppppppppfVar73[0x19] = (float *********)local_1ea0;
            ppppppppppfVar73[0x1a] = (float *********)local_1e9c;
          }
          ppppppppppfVar56 = (float **********)((int)ppppppppppfVar56 + -1);
        } while ((int)ppppppppppfVar22 <= (int)ppppppppppfVar56);
      }
    }
  }
  goto LAB_001fc7a8;
joined_r0x001f07b0:
  for (; (int)pppppppppppfVar49 < (int)pppppppppppfVar9;
      pppppppppppfVar49 = (float ***********)((int)pppppppppppfVar49 + 1)) {
    if (pppppppppppfVar49 < *pppppppppppfVar15) {
      ppppppppppfVar56 = pppppppppppfVar15[2] + (int)pppppppppppfVar49 * 0x27;
    }
    else {
      ppppppppppfVar56 = pppppppppppfVar15[2];
    }
    ppppppppppfVar56[0x1e] = ppppppppppfVar22[0x1c];
    ppppppppppfVar56[0x1f] = ppppppppppfVar22[0x1c];
    ppppppppppfVar56[0x15] = ppppppppppfVar22[0xf];
    ppppppppppfVar56[0x16] = ppppppppppfVar22[0x10];
    ppppppppppfVar56[0x17] = ppppppppppfVar22[0x11];
    ppppppppppfVar56[0x18] = ppppppppppfVar22[0xf];
    ppppppppppfVar56[0x19] = ppppppppppfVar22[0x10];
    ppppppppppfVar56[0x1a] = ppppppppppfVar22[0x11];
    *(undefined1 *)((int)ppppppppppfVar56 + 0x97) = 1;
    if (pppppppppppfVar49 < *pppppppppppfVar15) {
      ppppppppppfVar56 = pppppppppppfVar15[2] + (int)pppppppppppfVar49 * 0x27;
    }
    else {
      ppppppppppfVar56 = pppppppppppfVar15[2];
    }
    uVar36 = *DAT_001f063c;
    if ((int)uVar36 < (int)DAT_001f063c[1]) {
      puVar4 = DAT_001f063c + 2;
      *DAT_001f063c = uVar36 + 1;
      puVar18 = (undefined4 *)(*puVar4 + uVar36 * 0x9c);
      local_1f08 = (float ***********)(ppppppppppfVar56 + 0xc);
      *puVar18 = *ppppppppppfVar56;
      local_1ebc = (float ***********)(puVar18 + 9);
      local_1ec8 = (float ***********)(puVar18 + 0xc);
      puVar18[1] = ppppppppppfVar56[1];
      local_1eb0 = (float ***********)(ppppppppppfVar56 + 0xf);
      puVar18[2] = ppppppppppfVar56[2];
      puVar18[3] = ppppppppppfVar56[3];
      local_1ec4 = (float ************)(ppppppppppfVar56 + 0x12);
      puVar18[4] = ppppppppppfVar56[4];
      puVar18[5] = ppppppppppfVar56[5];
      puVar18[6] = ppppppppppfVar56[6];
      puVar18[7] = ppppppppppfVar56[7];
      puVar18[8] = ppppppppppfVar56[8];
      *local_1ebc = (float **********)ppppppppppfVar56[9];
      puVar18[10] = ppppppppppfVar56[10];
      puVar18[0xb] = ppppppppppfVar56[0xb];
      *local_1ec8 = *local_1f08;
      puVar18[0xd] = ppppppppppfVar56[0xd];
      puVar18[0xe] = ppppppppppfVar56[0xe];
      puVar18[0xf] = *local_1eb0;
      puVar18[0x10] = ppppppppppfVar56[0x10];
      puVar18[0x11] = ppppppppppfVar56[0x11];
      puVar18[0x12] = *local_1ec4;
      puVar18[0x13] = ppppppppppfVar56[0x13];
      puVar18[0x14] = ppppppppppfVar56[0x14];
      puVar18[0x15] = ppppppppppfVar56[0x15];
      puVar18[0x16] = ppppppppppfVar56[0x16];
      puVar18[0x17] = ppppppppppfVar56[0x17];
      puVar18[0x18] = ppppppppppfVar56[0x18];
      puVar18[0x19] = ppppppppppfVar56[0x19];
      puVar18[0x1a] = ppppppppppfVar56[0x1a];
      puVar18[0x1b] = ppppppppppfVar56[0x1b];
      puVar18[0x1c] = ppppppppppfVar56[0x1c];
      puVar18[0x1d] = ppppppppppfVar56[0x1d];
      puVar18[0x1e] = ppppppppppfVar56[0x1e];
      puVar18[0x1f] = ppppppppppfVar56[0x1f];
      puVar18[0x20] = ppppppppppfVar56[0x20];
      puVar18[0x21] = ppppppppppfVar56[0x21];
      puVar18[0x22] = ppppppppppfVar56[0x22];
      puVar18[0x23] = ppppppppppfVar56[0x23];
      *(undefined1 *)(puVar18 + 0x24) = *(undefined1 *)(ppppppppppfVar56 + 0x24);
      *(undefined1 *)((int)puVar18 + 0x91) = *(undefined1 *)((int)ppppppppppfVar56 + 0x91);
      *(undefined1 *)((int)puVar18 + 0x92) = *(undefined1 *)((int)ppppppppppfVar56 + 0x92);
      *(undefined1 *)((int)puVar18 + 0x93) = *(undefined1 *)((int)ppppppppppfVar56 + 0x93);
      *(undefined1 *)(puVar18 + 0x25) = *(undefined1 *)(ppppppppppfVar56 + 0x25);
      *(undefined1 *)((int)puVar18 + 0x95) = *(undefined1 *)((int)ppppppppppfVar56 + 0x95);
      *(undefined1 *)((int)puVar18 + 0x96) = *(undefined1 *)((int)ppppppppppfVar56 + 0x96);
      *(undefined1 *)((int)puVar18 + 0x97) = *(undefined1 *)((int)ppppppppppfVar56 + 0x97);
      *(undefined1 *)(puVar18 + 0x26) = *(undefined1 *)(ppppppppppfVar56 + 0x26);
    }
  }
  uVar36 = *DAT_001f063c;
  if ((int)uVar36 < 1) {
    if ((int)local_1f14 + 1 < (int)*local_1f10) {
      ppppppppppfVar56 = ppppppppppfVar22;
      if ((float **********)((int)local_1f14 + 1) < *local_1f10) {
        ppppppppppfVar73 = local_1f10[2] + (int)((int)local_1f14 + 1) * 0x27;
      }
      else {
        ppppppppppfVar73 = local_1f10[2];
      }
      goto LAB_001f0ae8;
    }
  }
  else {
    ppppppppppfVar73 = ppppppppppfVar22;
    ppppppppppfVar56 = (float **********)(DAT_001f063c[2] + uVar36 * 0x9c + -0x9c);
LAB_001f0ae8:
    pppppppppfVar48 = ppppppppppfVar56[7];
    iVar21 = (int)pppppppppfVar48 - (int)DAT_001f0630;
    if ((int)pppppppppfVar48 <= (int)DAT_001f0630) {
      pppppppppfVar48 = ppppppppppfVar73[7];
      iVar21 = (int)pppppppppfVar48 - (int)DAT_001f0630;
    }
    if (pppppppppfVar48 != (float *********)DAT_001f0630 &&
        iVar21 < 0 == SBORROW4((int)pppppppppfVar48,(int)DAT_001f0630)) {
      pppppppfVar51 =
           (float *******)
           ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar56) * ALBW_AS_F32(ppppppppppfVar73[6]) +
            ALBW_AS_F32(ppppppppppfVar56[1]) * ALBW_AS_F32(ppppppppppfVar73[7]) +
           ALBW_AS_F32(ppppppppppfVar56[2]) * ALBW_AS_F32(ppppppppppfVar73[8])));
      if (DAT_001f061c < pppppppfVar51) {
LAB_001f0b34:
        local_1f0c = (float ***********)0x0;
      }
      else if (DAT_001f0634 < (int)pppppppfVar51) {
        local_1f0c = (float ***********)0x2;
        if ((int)local_1f14 + 1 < (int)*local_1f10) {
          if ((float **********)((int)local_1f14 + 1) < *local_1f10) {
            ppppppppppfVar56 = local_1f10[2] + (int)((int)local_1f14 + 1) * 0x27;
          }
          else {
            ppppppppppfVar56 = local_1f10[2];
          }
          if (DAT_001f061c <
              (float *******)
              ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar56[6]) +
               ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar56[7]) +
              ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar56[8])))) goto LAB_001f0b34;
        }
      }
    }
  }
  if ((int)uVar36 < (int)DAT_001f063c[1]) {
    puVar18 = (undefined4 *)(DAT_001f063c[2] + uVar36 * 0x9c);
    *DAT_001f063c = uVar36 + 1;
  }
  else {
    puVar18 = (undefined4 *)0x0;
  }
  if (pppppppppppfVar9 < *pppppppppppfVar15) {
    ppppppppppfVar56 = pppppppppppfVar15[2] + (int)pppppppppppfVar9 * 0x27;
  }
  else {
    ppppppppppfVar56 = pppppppppppfVar15[2];
  }
  if (local_1f0c < (float ***********)0x2) {
    *puVar18 = *ppppppppppfVar22;
    puVar18[1] = ppppppppppfVar22[1];
    puVar18[2] = ppppppppppfVar22[2];
    puVar18[3] = ppppppppppfVar22[3];
    puVar18[4] = ppppppppppfVar22[4];
    puVar18[5] = ppppppppppfVar22[5];
    puVar18[6] = ppppppppppfVar22[6];
    puVar18[7] = ppppppppppfVar22[7];
    puVar18[8] = ppppppppppfVar22[8];
    puVar18[9] = ppppppppppfVar22[9];
    puVar18[10] = ppppppppppfVar22[10];
    puVar18[0xb] = ppppppppppfVar22[0xb];
    puVar18[0xc] = ppppppppppfVar22[0xc];
    puVar18[0xd] = ppppppppppfVar22[0xd];
    puVar18[0xe] = ppppppppppfVar22[0xe];
    puVar18[0xf] = ppppppppppfVar22[0xf];
    puVar18[0x10] = ppppppppppfVar22[0x10];
    puVar18[0x11] = ppppppppppfVar22[0x11];
    puVar18[0x12] = ppppppppppfVar22[0x12];
    puVar18[0x13] = ppppppppppfVar22[0x13];
    puVar18[0x14] = ppppppppppfVar22[0x14];
    puVar18[0x15] = ppppppppppfVar22[0x15];
    puVar18[0x16] = ppppppppppfVar22[0x16];
    puVar18[0x17] = ppppppppppfVar22[0x17];
    puVar18[0x18] = ppppppppppfVar22[0x18];
    puVar18[0x19] = ppppppppppfVar22[0x19];
    puVar18[0x1a] = ppppppppppfVar22[0x1a];
    puVar18[0x1b] = ppppppppppfVar22[0x1b];
    puVar18[0x1c] = ppppppppppfVar22[0x1c];
    puVar18[0x1d] = ppppppppppfVar22[0x1d];
    puVar18[0x1e] = ppppppppppfVar22[0x1e];
    puVar18[0x1f] = ppppppppppfVar22[0x1f];
    puVar18[0x20] = ppppppppppfVar22[0x20];
    puVar18[0x21] = ppppppppppfVar22[0x21];
    puVar18[0x22] = ppppppppppfVar22[0x22];
    puVar18[0x23] = ppppppppppfVar22[0x23];
    *(undefined1 *)(puVar18 + 0x24) = *(undefined1 *)(ppppppppppfVar22 + 0x24);
    *(undefined1 *)((int)puVar18 + 0x91) = *(undefined1 *)((int)ppppppppppfVar22 + 0x91);
    *(undefined1 *)((int)puVar18 + 0x92) = *(undefined1 *)((int)ppppppppppfVar22 + 0x92);
    *(undefined1 *)((int)puVar18 + 0x93) = *(undefined1 *)((int)ppppppppppfVar22 + 0x93);
    *(undefined1 *)(puVar18 + 0x25) = *(undefined1 *)(ppppppppppfVar22 + 0x25);
    *(undefined1 *)((int)puVar18 + 0x95) = *(undefined1 *)((int)ppppppppppfVar22 + 0x95);
    *(undefined1 *)((int)puVar18 + 0x96) = *(undefined1 *)((int)ppppppppppfVar22 + 0x96);
    *(undefined1 *)((int)puVar18 + 0x97) = *(undefined1 *)((int)ppppppppppfVar22 + 0x97);
    *(undefined1 *)(puVar18 + 0x26) = *(undefined1 *)(ppppppppppfVar22 + 0x26);
    puVar18[0x1e] = ppppppppppfVar56[0x1c];
    puVar18[0x1f] = ppppppppppfVar56[0x1d];
    puVar18[0x15] = ppppppppppfVar56[0xf];
    puVar18[0x16] = ppppppppppfVar56[0x10];
    puVar18[0x17] = ppppppppppfVar56[0x11];
    puVar18[0x18] = ppppppppppfVar56[0x12];
    puVar18[0x19] = ppppppppppfVar56[0x13];
    pppppppppfVar48 = ppppppppppfVar56[0x14];
  }
  else {
    if (local_1f0c != (float ***********)0x2 && local_1f0c != (float ***********)0x3)
    goto LAB_001f0f94;
    *puVar18 = *ppppppppppfVar56;
    puVar18[1] = ppppppppppfVar56[1];
    puVar18[2] = ppppppppppfVar56[2];
    puVar18[3] = ppppppppppfVar56[3];
    puVar18[4] = ppppppppppfVar56[4];
    puVar18[5] = ppppppppppfVar56[5];
    puVar18[6] = ppppppppppfVar56[6];
    puVar18[7] = ppppppppppfVar56[7];
    puVar18[8] = ppppppppppfVar56[8];
    puVar18[9] = ppppppppppfVar56[9];
    puVar18[10] = ppppppppppfVar56[10];
    puVar18[0xb] = ppppppppppfVar56[0xb];
    puVar18[0xc] = ppppppppppfVar56[0xc];
    puVar18[0xd] = ppppppppppfVar56[0xd];
    puVar18[0xe] = ppppppppppfVar56[0xe];
    puVar18[0xf] = ppppppppppfVar56[0xf];
    puVar18[0x10] = ppppppppppfVar56[0x10];
    puVar18[0x11] = ppppppppppfVar56[0x11];
    puVar18[0x12] = ppppppppppfVar56[0x12];
    puVar18[0x13] = ppppppppppfVar56[0x13];
    puVar18[0x14] = ppppppppppfVar56[0x14];
    puVar18[0x15] = ppppppppppfVar56[0x15];
    puVar18[0x16] = ppppppppppfVar56[0x16];
    puVar18[0x17] = ppppppppppfVar56[0x17];
    puVar18[0x18] = ppppppppppfVar56[0x18];
    puVar18[0x19] = ppppppppppfVar56[0x19];
    puVar18[0x1a] = ppppppppppfVar56[0x1a];
    puVar18[0x1b] = ppppppppppfVar56[0x1b];
    puVar18[0x1c] = ppppppppppfVar56[0x1c];
    puVar18[0x1d] = ppppppppppfVar56[0x1d];
    puVar18[0x1e] = ppppppppppfVar56[0x1e];
    puVar18[0x1f] = ppppppppppfVar56[0x1f];
    puVar18[0x20] = ppppppppppfVar56[0x20];
    puVar18[0x21] = ppppppppppfVar56[0x21];
    puVar18[0x22] = ppppppppppfVar56[0x22];
    puVar18[0x23] = ppppppppppfVar56[0x23];
    *(undefined1 *)(puVar18 + 0x24) = *(undefined1 *)(ppppppppppfVar56 + 0x24);
    *(undefined1 *)((int)puVar18 + 0x91) = *(undefined1 *)((int)ppppppppppfVar56 + 0x91);
    *(undefined1 *)((int)puVar18 + 0x92) = *(undefined1 *)((int)ppppppppppfVar56 + 0x92);
    *(undefined1 *)((int)puVar18 + 0x93) = *(undefined1 *)((int)ppppppppppfVar56 + 0x93);
    *(undefined1 *)(puVar18 + 0x25) = *(undefined1 *)(ppppppppppfVar56 + 0x25);
    *(undefined1 *)((int)puVar18 + 0x95) = *(undefined1 *)((int)ppppppppppfVar56 + 0x95);
    *(undefined1 *)((int)puVar18 + 0x96) = *(undefined1 *)((int)ppppppppppfVar56 + 0x96);
    *(undefined1 *)((int)puVar18 + 0x97) = *(undefined1 *)((int)ppppppppppfVar56 + 0x97);
    *(undefined1 *)(puVar18 + 0x26) = *(undefined1 *)(ppppppppppfVar56 + 0x26);
    puVar18[0x1e] = ppppppppppfVar22[0x1c];
    puVar18[0x1f] = ppppppppppfVar22[0x1d];
    puVar18[0x15] = ppppppppppfVar22[0xf];
    puVar18[0x16] = ppppppppppfVar22[0x10];
    puVar18[0x17] = ppppppppppfVar22[0x11];
    puVar18[0x18] = ppppppppppfVar22[0x12];
    puVar18[0x19] = ppppppppppfVar22[0x13];
    pppppppppfVar48 = ppppppppppfVar22[0x14];
  }
  puVar18[0x1a] = pppppppppfVar48;
LAB_001f0f94:
  pppppppppppfVar49 = (float ***********)((int)pppppppppppfVar9 + 1);
LAB_001f0f9c:
  local_1f14 = (float ***********)((int)local_1f14 + 1);
  if ((int)*local_1f10 <= (int)local_1f14) goto LAB_001f0fb8;
  goto LAB_001f0528;
LAB_001f24b0:
  do {
    if ((int)pppppppppppfVar9 <= (int)pppppppppppfVar34) break;
    if (pppppppppppfVar34 < *pppppppppppfVar17) {
      ppppppppppfVar22 = pppppppppppfVar78[0x2b] + (int)pppppppppppfVar34 * 0x27;
    }
    else {
      ppppppppppfVar22 = pppppppppppfVar78[0x2b];
    }
    *(undefined1 *)((int)ppppppppppfVar22 + 0x91) = 1;
    if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
      func_00115C70((float *)&local_1ec8,(float *)local_1f28,(float *)(ppppppppppfVar22 + 0xf));
      func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 6),
                    ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1ec8) +
                    ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1ec4) +
                    ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1ec0));
      local_1e90 = local_1ec8;
      local_1e8c = (float ***********)local_1ec4;
      local_1e88 = (float ***********)local_1ec0;
      func_00115C70((float *)&local_1ea8,(float *)local_1f28,(float *)&local_1e90);
      ppppppppppfVar22[0xf] = (float *********)local_1ea8;
      ppppppppppfVar22[0x10] = (float *********)local_1ea4;
      ppppppppppfVar22[0x11] = (float *********)local_1ea0;
    }
    else {
      if (pppppppppppfVar34 != pppppppppppfVar15 && bVar44) {
        fVar55 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1e9c) +
                 ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1e98) +
                 ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1e94);
        fVar5 = fVar55;
        if (fVar55 <= ALBW_AS_F32(uVar72)) {
          fVar5 = -fVar55;
        }
        if (DAT_001f290c < (int)ALBW_AS_U32(fVar5)) {
          func_00115C70((float *)&local_1e90,(float *)local_1f28,(float *)(ppppppppppfVar22 + 9));
          local_1e84 = local_1e90;
          local_1e80 = local_1e8c;
          local_1e7c = local_1e88;
          func_00115D70((float *)&local_1e90,(float *)&local_1e9c,
                        ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1e90) +
                        ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1e8c) +
                        ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1e88));
          local_1ec8 = local_1e90;
          local_1ec4 = (float ************)local_1e8c;
          local_1ec0 = (float ************)local_1e88;
          func_00115D88((float *)&local_1e90,(float *)&local_1ec8,fVar55);
          local_1e84 = local_1e90;
          local_1e80 = local_1e8c;
          local_1e7c = local_1e88;
          func_00115C70((float *)&local_1e24,(float *)local_1f28,(float *)&local_1e84);
          local_1ea8 = local_1e24;
          local_1ea4 = local_1e20;
          local_1ea0 = local_1e1c;
          func_00115C70((float *)&local_1e90,(float *)&local_1ea8,(float *)(ppppppppppfVar22 + 0xf))
          ;
          local_1ec8 = local_1e90;
          local_1ec4 = (float ************)local_1e8c;
          local_1ec0 = (float ************)local_1e88;
          if (ALBW_AS_F32(uVar72) <
              ALBW_AS_F32(local_1e90) * ALBW_AS_F32(ppppppppppfVar22[3]) +
              ALBW_AS_F32(local_1e8c) * ALBW_AS_F32(ppppppppppfVar22[4]) +
              ALBW_AS_F32(local_1e88) * ALBW_AS_F32(ppppppppppfVar22[5])) {
            ppppppppppfVar22[0x15] = (float *********)local_1ea8;
            ppppppppppfVar22[0x16] = (float *********)local_1ea4;
            ppppppppppfVar22[0x17] = (float *********)local_1ea0;
            goto LAB_001f24a0;
          }
        }
      }
      func_00115C70((float *)&local_1ec8,(float *)local_1f28,(float *)(ppppppppppfVar22 + 0xf));
      func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 6),
                    ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1ec8) +
                    ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1ec4) +
                    ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1ec0));
      local_1ea8 = local_1ec8;
      local_1ea4 = (float ***********)local_1ec4;
      local_1ea0 = (float ***********)local_1ec0;
      func_00115C70((float *)&local_1ec8,(float *)local_1f28,(float *)&local_1ea8);
      ppppppppppfVar22[0x15] = (float *********)local_1ec8;
      ppppppppppfVar22[0x16] = (float *********)local_1ec4;
      ppppppppppfVar22[0x17] = (float *********)local_1ec0;
    }
LAB_001f24a0:
    pppppppppppfVar34 = (float ***********)((int)pppppppppppfVar34 + 1);
LAB_001f24a4:
  } while ((int)pppppppppppfVar34 <= (int)local_1f04);
  goto LAB_001f24b8;
LAB_001f6f60:
  local_1f28[1] = (float ***********)0x1;
LAB_001f7014:
  uVar36 = 0;
  if (0 < (int)*DAT_001f6c14) {
    do {
      puVar4 = DAT_001f6c14;
      if (uVar36 < *DAT_001f6c14) {
        uVar16 = DAT_001f6c14[2] + uVar36 * 0x9c;
      }
      else {
        uVar16 = DAT_001f6c14[2];
      }
      *(undefined1 *)(uVar16 + 0x93) = 1;
      if (uVar36 < *puVar4) {
        puVar18 = (undefined4 *)(puVar4[2] + uVar36 * 0x9c);
      }
      else {
        puVar18 = (undefined4 *)puVar4[2];
      }
      ppppppppppfVar22 = *pppppppppppfVar74;
      if ((int)ppppppppppfVar22 < (int)pppppppppppfVar15[0x2a]) {
        *pppppppppppfVar74 = (float **********)((int)ppppppppppfVar22 + 1);
        ppppppppppfVar22 = pppppppppppfVar15[0x2b] + (int)ppppppppppfVar22 * 0x27;
        *ppppppppppfVar22 = (float *********)*puVar18;
        local_1ec0 = (float ************)(ppppppppppfVar22 + 9);
        ppppppppppfVar22[1] = (float *********)puVar18[1];
        ppppppppppfVar22[2] = (float *********)puVar18[2];
        local_1f04 = (float ***********)(ppppppppppfVar22 + 0xf);
        ppppppppppfVar22[3] = (float *********)puVar18[3];
        ppppppppppfVar22[4] = (float *********)puVar18[4];
        local_1eac = (float ************)(puVar18 + 0x12);
        ppppppppppfVar22[5] = (float *********)puVar18[5];
        ppppppppppfVar22[6] = (float *********)puVar18[6];
        ppppppppppfVar22[7] = (float *********)puVar18[7];
        ppppppppppfVar22[8] = (float *********)puVar18[8];
        ppppppppppfVar22[9] = (float *********)puVar18[9];
        ppppppppppfVar22[10] = (float *********)puVar18[10];
        ppppppppppfVar22[0xb] = (float *********)puVar18[0xb];
        ppppppppppfVar22[0xc] = (float *********)puVar18[0xc];
        ppppppppppfVar22[0xd] = (float *********)puVar18[0xd];
        ppppppppppfVar22[0xe] = (float *********)puVar18[0xe];
        ppppppppppfVar22[0xf] = (float *********)puVar18[0xf];
        ppppppppppfVar22[0x10] = (float *********)puVar18[0x10];
        ppppppppppfVar22[0x11] = (float *********)puVar18[0x11];
        ppppppppppfVar22[0x12] = (float *********)puVar18[0x12];
        ppppppppppfVar22[0x13] = (float *********)puVar18[0x13];
        ppppppppppfVar22[0x14] = (float *********)puVar18[0x14];
        ppppppppppfVar22[0x15] = (float *********)puVar18[0x15];
        ppppppppppfVar22[0x16] = (float *********)puVar18[0x16];
        ppppppppppfVar22[0x17] = (float *********)puVar18[0x17];
        ppppppppppfVar22[0x18] = (float *********)puVar18[0x18];
        ppppppppppfVar22[0x19] = (float *********)puVar18[0x19];
        ppppppppppfVar22[0x1a] = (float *********)puVar18[0x1a];
        ppppppppppfVar22[0x1b] = (float *********)puVar18[0x1b];
        ppppppppppfVar22[0x1c] = (float *********)puVar18[0x1c];
        ppppppppppfVar22[0x1d] = (float *********)puVar18[0x1d];
        ppppppppppfVar22[0x1e] = (float *********)puVar18[0x1e];
        ppppppppppfVar22[0x1f] = (float *********)puVar18[0x1f];
        ppppppppppfVar22[0x20] = (float *********)puVar18[0x20];
        ppppppppppfVar22[0x21] = (float *********)puVar18[0x21];
        ppppppppppfVar22[0x22] = (float *********)puVar18[0x22];
        ppppppppppfVar22[0x23] = (float *********)puVar18[0x23];
        *(undefined1 *)(ppppppppppfVar22 + 0x24) = *(undefined1 *)(puVar18 + 0x24);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x91) = *(undefined1 *)((int)puVar18 + 0x91);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x92) = *(undefined1 *)((int)puVar18 + 0x92);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x93) = *(undefined1 *)((int)puVar18 + 0x93);
        *(undefined1 *)(ppppppppppfVar22 + 0x25) = *(undefined1 *)(puVar18 + 0x25);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = *(undefined1 *)((int)puVar18 + 0x95);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x96) = *(undefined1 *)((int)puVar18 + 0x96);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x97) = *(undefined1 *)((int)puVar18 + 0x97);
        *(undefined1 *)(ppppppppppfVar22 + 0x26) = *(undefined1 *)(puVar18 + 0x26);
      }
      uVar36 = uVar36 + 1;
    } while ((int)uVar36 < (int)*DAT_001f6c14);
  }
  if (local_1f28[1] != (float ***********)0x0) {
    local_1de8 = (float ***********)0x1;
  }
LAB_001f72a4:
  pppppppppppfVar15 = local_1e88;
  fVar5 = ALBW_AS_F32(uVar72);
  fVar60 = ALBW_AS_F32(((ulonglong)uVar72 >> 0x20)) + fVar55;
  pppppppppfVar48 = (float *********)((ulonglong)uVar77 >> 0x20);
  fVar65 = ALBW_AS_F32(uVar77) + fVar5;
  if (((*DAT_001f7ebc & 1) == 0) &&
     (iVar21 = func_00100174((int *)DAT_001f7ebc), puVar4 = DAT_001f7ec0, iVar21 != 0)) {
    puVar26 = DAT_001f7ec0 + 3;
    *DAT_001f7ec0 = 0;
    puVar4[1] = 0;
    puVar4[2] = 0;
    func_0010B3EC(puVar4,0x20,(int)puVar26);
  }
  puVar4 = DAT_001f7ec0;
  *DAT_001f7ec0 = 0;
  ppppppppppfVar22 = (float **********)0x0;
  fVar55 = ALBW_AS_F32(pppppppppppfVar81) / (fVar55 - fVar60);
  local_1f00[1] = (float ***********)ALBW_PTR_BITS((fVar55 * fVar57));
  local_1f28[3] = (float ***********)ALBW_PTR_BITS((fVar57 + fVar60 * fVar55 * ALBW_AS_F32(pppppppppppfVar53)));
  fVar5 = ALBW_AS_F32(pppppppppppfVar81) / (fVar5 - fVar65);
  local_1f00[0] = (float ***********)ALBW_PTR_BITS((fVar5 * ALBW_AS_F32(pppppppppppfVar53)));
  local_1f28[2] = (float ***********)ALBW_PTR_BITS((fVar57 + fVar65 * fVar5 * fVar57));
  pppppppppppfVar74 = pppppppppppfVar81;
  if (0 < (int)*pppppppppppfVar15) {
    do {
      if (ppppppppppfVar22 < *pppppppppppfVar15) {
        ppppppppppfVar56 = pppppppppppfVar15[2] + (int)ppppppppppfVar22 * 0x27;
      }
      else {
        ppppppppppfVar56 = pppppppppppfVar15[2];
      }
      ppppppppppfVar56[0x1c] =
           (float *********)
           ALBW_PTR_BITS((ALBW_AS_F32(local_1f28[*(char *)(ppppppppppfVar56 + 0x24) + 2]) +
           ALBW_AS_F32(local_1f00[*(char *)(ppppppppppfVar56 + 0x24)]) * ALBW_AS_F32(ppppppppppfVar56[0x1c])));
      pppppppppfVar23 =
           (float *********)
           ALBW_PTR_BITS((ALBW_AS_F32(local_1f28[*(char *)(ppppppppppfVar56 + 0x24) + 2]) +
           ALBW_AS_F32(local_1f00[*(char *)(ppppppppppfVar56 + 0x24)]) * ALBW_AS_F32(ppppppppppfVar56[0x1d])));
      ppppppppppfVar56[0x1d] = pppppppppfVar23;
      if (ALBW_AS_F32(pppppppppfVar23) < ALBW_AS_F32(ppppppppppfVar56[0x1c])) {
        pppppppppfVar23 = ppppppppppfVar56[9];
        pppppppppfVar24 = ppppppppppfVar56[10];
        pppppppppfVar64 = ppppppppppfVar56[0xb];
        ppppppppppfVar56[9] = ppppppppppfVar56[0xc];
        ppppppppppfVar56[10] = ppppppppppfVar56[0xd];
        ppppppppppfVar56[0xb] = ppppppppppfVar56[0xe];
        ppppppppppfVar56[0xc] = pppppppppfVar23;
        ppppppppppfVar56[0xd] = pppppppppfVar24;
        ppppppppppfVar56[0xe] = pppppppppfVar64;
        pppppppppfVar23 = ppppppppppfVar56[0x1c];
        ppppppppppfVar56[0x1c] = ppppppppppfVar56[0x1d];
        ppppppppppfVar56[0x1d] = pppppppppfVar23;
      }
      ppppppppppfVar56[0xf] = ppppppppppfVar56[9];
      ppppppppppfVar56[0x10] = ppppppppppfVar56[10];
      ppppppppppfVar56[0x11] = ppppppppppfVar56[0xb];
      ppppppppppfVar56[0x12] = ppppppppppfVar56[0xc];
      ppppppppppfVar56[0x13] = ppppppppppfVar56[0xd];
      ppppppppppfVar56[0x14] = ppppppppppfVar56[0xe];
      *(undefined1 *)((int)ppppppppppfVar56 + 0x92) = 0;
      *(undefined1 *)((int)ppppppppppfVar56 + 0x91) = 0;
      ppppppppppfVar56[0x1b] = pppppppppfVar48;
      *(undefined1 *)(ppppppppppfVar56 + 0x26) = 0;
      *(undefined1 *)((int)ppppppppppfVar56 + 0x97) = 0;
      *(undefined1 *)((int)ppppppppppfVar56 + 0x95) = 0;
      *(undefined1 *)(ppppppppppfVar56 + 0x25) = 0;
      if (*(char *)(ppppppppppfVar56 + 0x24) == '\0') {
        func_00115D70((float *)ppppppppppfVar56,(float *)ppppppppppfVar56,fVar58);
        func_00115D70((float *)(ppppppppppfVar56 + 3),(float *)(ppppppppppfVar56 + 3),fVar58);
      }
      ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
    } while ((int)ppppppppppfVar22 < (int)*pppppppppppfVar15);
  }
  ppppppppppfVar22 = (float **********)0x0;
  if (0 < (int)*pppppppppppfVar15) {
    do {
      puVar26 = DAT_001f7ec0;
      ppppppppppfVar73 = *pppppppppppfVar15;
      ppppppppppfVar56 = pppppppppppfVar15[2];
      if (ppppppppppfVar22 < ppppppppppfVar73) {
        ppppppppppfVar56 = ppppppppppfVar56 + (int)ppppppppppfVar22 * 0x27;
      }
      ppppppppppfVar30 = pppppppppppfVar15[2];
      if (ppppppppppfVar22 < ppppppppppfVar73) {
        ppppppppppfVar30 = ppppppppppfVar30 + (int)ppppppppppfVar22 * 0x27;
      }
      if (WALK_SEG_HAS_LENGTH(ppppppppppfVar56, ppppppppppfVar30)) {
        if (ppppppppppfVar22 < ppppppppppfVar73) {
          ppppppppppfVar56 = pppppppppppfVar15[2] + (int)ppppppppppfVar22 * 0x27;
        }
        else {
          ppppppppppfVar56 = pppppppppppfVar15[2];
        }
        if ((int)*DAT_001f7ec0 < (int)DAT_001f7ec0[1]) {
          *(float ***********)(DAT_001f7ec0[2] + *DAT_001f7ec0 * 4) = ppppppppppfVar56;
          *puVar26 = *puVar26 + 1;
        }
      }
      ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
    } while ((int)ppppppppppfVar22 < (int)*pppppppppppfVar15);
  }
  *pppppppppppfVar15 = (float **********)0x0;
  if (*puVar4 != 0) {
    func_0048C258((int *)DAT_001f7ec0,DAT_001f6c0c);
    if (((*DAT_001f7ec4 & 1) == 0) &&
       (iVar21 = func_00100174((int *)DAT_001f7ec4), puVar26 = DAT_001f7ec8, iVar21 != 0)) {
      puVar27 = DAT_001f7ec8 + 3;
      *DAT_001f7ec8 = 0;
      puVar26[1] = 0x30;
      puVar26[2] = (uint)puVar27;
      func_00100118((int)(puVar26 + 3),DAT_001f7ecc,0x9c,0x30);
    }
    puVar26 = DAT_001f7ec8;
    *DAT_001f7ec8 = 0;
    if (*puVar4 == 0) {
      puVar18 = (undefined4 *)0x0;
    }
    else {
      puVar18 = *(undefined4 **)puVar4[2];
    }
    if (0 < (int)puVar26[1]) {
      *puVar26 = 1;
      func_001FE514((undefined4 *)puVar26[2],puVar18);
    }
    iVar21 = puVar26[2] + *puVar26 * 0x9c + -0x9c;
    uVar36 = 1;
    if (1 < (int)*puVar4) {
      do {
        uVar43 = *puVar4;
        uVar16 = uVar43;
        if (uVar36 < uVar43) {
          uVar16 = puVar4[2];
        }
        if (uVar36 < uVar43) {
          puVar18 = *(undefined4 **)(uVar16 + uVar36 * 4);
        }
        else {
          puVar18 = (undefined4 *)0x0;
        }
        fVar5 = *(float *)(iVar21 + 0x18) * ALBW_AS_F32(puVar18[6]) +
                *(float *)(iVar21 + 0x1c) * ALBW_AS_F32(puVar18[7]) +
                *(float *)(iVar21 + 0x20) * ALBW_AS_F32(puVar18[8]);
        if (fVar5 <= ALBW_AS_F32(pppppppppfVar48)) {
          fVar5 = -fVar5;
        }
        if (DAT_001f6c18 < (int)ALBW_AS_U32(fVar5)) {
          if (*(float *)(iVar21 + 0x74) < ALBW_AS_F32(puVar18[0x1d])) {
            *(undefined4 *)(iVar21 + 0x30) = puVar18[0xc];
            *(undefined4 *)(iVar21 + 0x34) = puVar18[0xd];
            *(undefined4 *)(iVar21 + 0x38) = puVar18[0xe];
            *(undefined4 *)(iVar21 + 0x74) = puVar18[0x1d];
            *(undefined4 *)(iVar21 + 0x48) = puVar18[0x12];
            *(undefined4 *)(iVar21 + 0x4c) = puVar18[0x13];
            *(undefined4 *)(iVar21 + 0x50) = puVar18[0x14];
          }
        }
        else {
          uVar16 = *puVar26;
          if ((int)uVar16 < (int)puVar26[1]) {
            *puVar26 = uVar16 + 1;
            local_1ebc = (float ***********)(puVar18 + 6);
            puVar7 = (undefined4 *)(puVar26[2] + uVar16 * 0x9c);
            local_1ec8 = (float ***********)(puVar18 + 0xc);
            *puVar7 = *puVar18;
            local_1ec0 = (float ************)(puVar7 + 0xf);
            puVar7[1] = puVar18[1];
            local_1ec4 = (float ************)(puVar18 + 0xf);
            puVar7[2] = puVar18[2];
            puVar7[3] = puVar18[3];
            puVar7[4] = puVar18[4];
            local_1eb8 = (float ***********)(puVar18 + 0x12);
            puVar7[5] = puVar18[5];
            puVar7[6] = *local_1ebc;
            puVar7[7] = puVar18[7];
            puVar7[8] = puVar18[8];
            puVar7[9] = puVar18[9];
            puVar7[10] = puVar18[10];
            puVar7[0xb] = puVar18[0xb];
            puVar7[0xc] = *local_1ec8;
            puVar7[0xd] = puVar18[0xd];
            puVar7[0xe] = puVar18[0xe];
            *local_1ec0 = *local_1ec4;
            puVar7[0x10] = puVar18[0x10];
            puVar7[0x11] = puVar18[0x11];
            puVar7[0x12] = *local_1eb8;
            puVar7[0x13] = puVar18[0x13];
            puVar7[0x14] = puVar18[0x14];
            puVar7[0x15] = puVar18[0x15];
            puVar7[0x16] = puVar18[0x16];
            puVar7[0x17] = puVar18[0x17];
            puVar7[0x18] = puVar18[0x18];
            puVar7[0x19] = puVar18[0x19];
            puVar7[0x1a] = puVar18[0x1a];
            puVar7[0x1b] = puVar18[0x1b];
            puVar7[0x1c] = puVar18[0x1c];
            puVar7[0x1d] = puVar18[0x1d];
            puVar7[0x1e] = puVar18[0x1e];
            puVar7[0x1f] = puVar18[0x1f];
            puVar7[0x20] = puVar18[0x20];
            puVar7[0x21] = puVar18[0x21];
            puVar7[0x22] = puVar18[0x22];
            puVar7[0x23] = puVar18[0x23];
            *(undefined1 *)(puVar7 + 0x24) = *(undefined1 *)(puVar18 + 0x24);
            *(undefined1 *)((int)puVar7 + 0x91) = *(undefined1 *)((int)puVar18 + 0x91);
            *(undefined1 *)((int)puVar7 + 0x92) = *(undefined1 *)((int)puVar18 + 0x92);
            *(undefined1 *)((int)puVar7 + 0x93) = *(undefined1 *)((int)puVar18 + 0x93);
            *(undefined1 *)(puVar7 + 0x25) = *(undefined1 *)(puVar18 + 0x25);
            *(undefined1 *)((int)puVar7 + 0x95) = *(undefined1 *)((int)puVar18 + 0x95);
            *(undefined1 *)((int)puVar7 + 0x96) = *(undefined1 *)((int)puVar18 + 0x96);
            *(undefined1 *)((int)puVar7 + 0x97) = *(undefined1 *)((int)puVar18 + 0x97);
            *(undefined1 *)(puVar7 + 0x26) = *(undefined1 *)(puVar18 + 0x26);
          }
          iVar21 = *puVar26 * 0x9c + -0x9c + puVar26[2];
        }
        uVar36 = uVar36 + 1;
      } while ((int)uVar36 < (int)*puVar4);
    }
    uVar36 = *puVar26;
    local_1f0c = (float ***********)0x0;
    uVar16 = 1;
    if (1 < (int)uVar36) {
      pfVar39 = (float *)puVar26[2];
      do {
        pfVar8 = pfVar39;
        if (uVar16 - 1 < uVar36) {
          pfVar8 = pfVar39 + (uVar16 - 1) * 0x27;
        }
        pfVar32 = pfVar39;
        if (uVar16 < uVar36) {
          pfVar32 = pfVar39 + uVar16 * 0x27;
        }
        pppppfVar41 = (float *****)ALBW_PTR_BITS(pfVar8[7]);
        iVar21 = (int)pppppfVar41 - (int)DAT_001f7ed0;
        if ((int)pppppfVar41 <= (int)DAT_001f7ed0) {
          pppppfVar41 = (float *****)ALBW_PTR_BITS(pfVar32[7]);
          iVar21 = (int)pppppfVar41 - (int)DAT_001f7ed0;
        }
        if (pppppfVar41 == DAT_001f7ed0 ||
            iVar21 < 0 != SBORROW4((int)pppppfVar41,(int)DAT_001f7ed0)) break;
        if ((int)DAT_001f7ed0 <
            (int)ALBW_AS_U32((*pfVar8 * pfVar32[6] + pfVar8[1] * pfVar32[7] + pfVar8[2] * pfVar32[8])))
        goto LAB_001f79e4;
        uVar16 = uVar16 + 1;
      } while ((int)uVar16 < (int)uVar36);
    }
    if (((int)uVar16 < (int)uVar36) && (uVar43 = uVar36 - 1, (int)uVar16 <= (int)uVar43)) {
      pfVar39 = (float *)puVar26[2];
      do {
        pfVar8 = pfVar39;
        if (uVar43 - 1 < uVar36) {
          pfVar8 = pfVar39 + (uVar43 - 1) * 0x27;
        }
        pfVar32 = pfVar39;
        if (uVar43 < uVar36) {
          pfVar32 = pfVar39 + uVar43 * 0x27;
        }
        pppppfVar41 = (float *****)ALBW_PTR_BITS(pfVar8[7]);
        iVar21 = (int)pppppfVar41 - (int)DAT_001f7ed0;
        if ((int)pppppfVar41 <= (int)DAT_001f7ed0) {
          pppppfVar41 = (float *****)ALBW_PTR_BITS(pfVar32[7]);
          iVar21 = (int)pppppfVar41 - (int)DAT_001f7ed0;
        }
        if (pppppfVar41 == DAT_001f7ed0 ||
            iVar21 < 0 != SBORROW4((int)pppppfVar41,(int)DAT_001f7ed0)) break;
        if (DAT_001f7ed4 <
            (int)ALBW_AS_U32((*pfVar8 * pfVar32[6] + pfVar8[1] * pfVar32[7] + pfVar8[2] * pfVar32[8])))
        goto LAB_001f79e4;
        uVar43 = uVar43 - 1;
      } while ((int)uVar16 <= (int)uVar43);
    }
    goto LAB_001f7a98;
  }
  goto LAB_001f7d24;
joined_r0x001f8050:
  for (; (int)pppppppppppfVar49 < (int)pppppppppppfVar9;
      pppppppppppfVar49 = (float ***********)((int)pppppppppppfVar49 + 1)) {
    if (pppppppppppfVar49 < *pppppppppppfVar15) {
      ppppppppppfVar56 = pppppppppppfVar15[2] + (int)pppppppppppfVar49 * 0x27;
    }
    else {
      ppppppppppfVar56 = pppppppppppfVar15[2];
    }
    ppppppppppfVar56[0x1e] = ppppppppppfVar22[0x1c];
    ppppppppppfVar56[0x1f] = ppppppppppfVar22[0x1c];
    ppppppppppfVar56[0x15] = ppppppppppfVar22[0xf];
    ppppppppppfVar56[0x16] = ppppppppppfVar22[0x10];
    ppppppppppfVar56[0x17] = ppppppppppfVar22[0x11];
    ppppppppppfVar56[0x18] = ppppppppppfVar22[0xf];
    ppppppppppfVar56[0x19] = ppppppppppfVar22[0x10];
    ppppppppppfVar56[0x1a] = ppppppppppfVar22[0x11];
    *(undefined1 *)((int)ppppppppppfVar56 + 0x97) = 1;
    if (pppppppppppfVar49 < *pppppppppppfVar15) {
      ppppppppppfVar56 = pppppppppppfVar15[2] + (int)pppppppppppfVar49 * 0x27;
    }
    else {
      ppppppppppfVar56 = pppppppppppfVar15[2];
    }
    uVar36 = *DAT_001f7edc;
    if ((int)uVar36 < (int)DAT_001f7edc[1]) {
      puVar4 = DAT_001f7edc + 2;
      *DAT_001f7edc = uVar36 + 1;
      puVar18 = (undefined4 *)(*puVar4 + uVar36 * 0x9c);
      local_1ebc = (float ***********)(puVar18 + 9);
      *puVar18 = *ppppppppppfVar56;
      local_1ec0 = (float ************)(puVar18 + 0xc);
      puVar18[1] = ppppppppppfVar56[1];
      local_1f04 = (float ***********)(ppppppppppfVar56 + 0xf);
      puVar18[2] = ppppppppppfVar56[2];
      local_1ec8 = (float ***********)(puVar18 + 0xf);
      puVar18[3] = ppppppppppfVar56[3];
      puVar18[4] = ppppppppppfVar56[4];
      local_1eb0 = (float ***********)(ppppppppppfVar56 + 0x12);
      puVar18[5] = ppppppppppfVar56[5];
      puVar18[6] = ppppppppppfVar56[6];
      puVar18[7] = ppppppppppfVar56[7];
      puVar18[8] = ppppppppppfVar56[8];
      *local_1ebc = (float **********)ppppppppppfVar56[9];
      puVar18[10] = ppppppppppfVar56[10];
      puVar18[0xb] = ppppppppppfVar56[0xb];
      *local_1ec0 = (float ***********)ppppppppppfVar56[0xc];
      puVar18[0xd] = ppppppppppfVar56[0xd];
      puVar18[0xe] = ppppppppppfVar56[0xe];
      *local_1ec8 = *local_1f04;
      puVar18[0x10] = ppppppppppfVar56[0x10];
      puVar18[0x11] = ppppppppppfVar56[0x11];
      puVar18[0x12] = *local_1eb0;
      puVar18[0x13] = ppppppppppfVar56[0x13];
      puVar18[0x14] = ppppppppppfVar56[0x14];
      puVar18[0x15] = ppppppppppfVar56[0x15];
      puVar18[0x16] = ppppppppppfVar56[0x16];
      puVar18[0x17] = ppppppppppfVar56[0x17];
      puVar18[0x18] = ppppppppppfVar56[0x18];
      puVar18[0x19] = ppppppppppfVar56[0x19];
      puVar18[0x1a] = ppppppppppfVar56[0x1a];
      puVar18[0x1b] = ppppppppppfVar56[0x1b];
      puVar18[0x1c] = ppppppppppfVar56[0x1c];
      puVar18[0x1d] = ppppppppppfVar56[0x1d];
      puVar18[0x1e] = ppppppppppfVar56[0x1e];
      puVar18[0x1f] = ppppppppppfVar56[0x1f];
      puVar18[0x20] = ppppppppppfVar56[0x20];
      puVar18[0x21] = ppppppppppfVar56[0x21];
      puVar18[0x22] = ppppppppppfVar56[0x22];
      puVar18[0x23] = ppppppppppfVar56[0x23];
      *(undefined1 *)(puVar18 + 0x24) = *(undefined1 *)(ppppppppppfVar56 + 0x24);
      *(undefined1 *)((int)puVar18 + 0x91) = *(undefined1 *)((int)ppppppppppfVar56 + 0x91);
      *(undefined1 *)((int)puVar18 + 0x92) = *(undefined1 *)((int)ppppppppppfVar56 + 0x92);
      *(undefined1 *)((int)puVar18 + 0x93) = *(undefined1 *)((int)ppppppppppfVar56 + 0x93);
      *(undefined1 *)(puVar18 + 0x25) = *(undefined1 *)(ppppppppppfVar56 + 0x25);
      *(undefined1 *)((int)puVar18 + 0x95) = *(undefined1 *)((int)ppppppppppfVar56 + 0x95);
      *(undefined1 *)((int)puVar18 + 0x96) = *(undefined1 *)((int)ppppppppppfVar56 + 0x96);
      *(undefined1 *)((int)puVar18 + 0x97) = *(undefined1 *)((int)ppppppppppfVar56 + 0x97);
      *(undefined1 *)(puVar18 + 0x26) = *(undefined1 *)(ppppppppppfVar56 + 0x26);
    }
  }
  uVar36 = *DAT_001f7edc;
  if ((int)uVar36 < 1) {
    if ((int)local_1f00[0] + 1 < (int)*local_1f14) {
      ppppppppppfVar56 = ppppppppppfVar22;
      if ((float **********)((int)local_1f00[0] + 1) < *local_1f14) {
        ppppppppppfVar73 = local_1f14[2] + (int)((int)local_1f00[0] + 1) * 0x27;
      }
      else {
        ppppppppppfVar73 = local_1f14[2];
      }
      goto LAB_001f8388;
    }
  }
  else {
    ppppppppppfVar73 = ppppppppppfVar22;
    ppppppppppfVar56 = (float **********)(DAT_001f7edc[2] + uVar36 * 0x9c + -0x9c);
LAB_001f8388:
    pppppppppfVar23 = ppppppppppfVar56[7];
    iVar21 = (int)pppppppppfVar23 - (int)DAT_001f7ed0;
    if ((int)pppppppppfVar23 <= (int)DAT_001f7ed0) {
      pppppppppfVar23 = ppppppppppfVar73[7];
      iVar21 = (int)pppppppppfVar23 - (int)DAT_001f7ed0;
    }
    if (pppppppppfVar23 != (float *********)DAT_001f7ed0 &&
        iVar21 < 0 == SBORROW4((int)pppppppppfVar23,(int)DAT_001f7ed0)) {
      pppppppfVar51 =
           (float *******)
           ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar56) * ALBW_AS_F32(ppppppppppfVar73[6]) +
            ALBW_AS_F32(ppppppppppfVar56[1]) * ALBW_AS_F32(ppppppppppfVar73[7]) +
           ALBW_AS_F32(ppppppppppfVar56[2]) * ALBW_AS_F32(ppppppppppfVar73[8])));
      if (DAT_001f7eb8 < pppppppfVar51) {
LAB_001f83d4:
        local_1f0c = (float ***********)0x0;
      }
      else if (DAT_001f7ed4 < (int)pppppppfVar51) {
        local_1f0c = (float ***********)0x2;
        if ((int)local_1f00[0] + 1 < (int)*local_1f14) {
          if ((float **********)((int)local_1f00[0] + 1) < *local_1f14) {
            ppppppppppfVar56 = local_1f14[2] + (int)((int)local_1f00[0] + 1) * 0x27;
          }
          else {
            ppppppppppfVar56 = local_1f14[2];
          }
          if (DAT_001f7eb8 <
              (float *******)
              ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar56[6]) +
               ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar56[7]) +
              ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar56[8])))) goto LAB_001f83d4;
        }
      }
    }
  }
  if ((int)uVar36 < (int)DAT_001f7edc[1]) {
    puVar18 = (undefined4 *)(DAT_001f7edc[2] + uVar36 * 0x9c);
    *DAT_001f7edc = uVar36 + 1;
  }
  else {
    puVar18 = (undefined4 *)0x0;
  }
  if (pppppppppppfVar9 < *pppppppppppfVar15) {
    ppppppppppfVar56 = pppppppppppfVar15[2] + (int)pppppppppppfVar9 * 0x27;
  }
  else {
    ppppppppppfVar56 = pppppppppppfVar15[2];
  }
  if (local_1f0c < (float ***********)0x2) {
    *puVar18 = *ppppppppppfVar22;
    puVar18[1] = ppppppppppfVar22[1];
    puVar18[2] = ppppppppppfVar22[2];
    puVar18[3] = ppppppppppfVar22[3];
    puVar18[4] = ppppppppppfVar22[4];
    puVar18[5] = ppppppppppfVar22[5];
    puVar18[6] = ppppppppppfVar22[6];
    puVar18[7] = ppppppppppfVar22[7];
    puVar18[8] = ppppppppppfVar22[8];
    puVar18[9] = ppppppppppfVar22[9];
    puVar18[10] = ppppppppppfVar22[10];
    puVar18[0xb] = ppppppppppfVar22[0xb];
    puVar18[0xc] = ppppppppppfVar22[0xc];
    puVar18[0xd] = ppppppppppfVar22[0xd];
    puVar18[0xe] = ppppppppppfVar22[0xe];
    puVar18[0xf] = ppppppppppfVar22[0xf];
    puVar18[0x10] = ppppppppppfVar22[0x10];
    puVar18[0x11] = ppppppppppfVar22[0x11];
    puVar18[0x12] = ppppppppppfVar22[0x12];
    puVar18[0x13] = ppppppppppfVar22[0x13];
    puVar18[0x14] = ppppppppppfVar22[0x14];
    puVar18[0x15] = ppppppppppfVar22[0x15];
    puVar18[0x16] = ppppppppppfVar22[0x16];
    puVar18[0x17] = ppppppppppfVar22[0x17];
    puVar18[0x18] = ppppppppppfVar22[0x18];
    puVar18[0x19] = ppppppppppfVar22[0x19];
    puVar18[0x1a] = ppppppppppfVar22[0x1a];
    puVar18[0x1b] = ppppppppppfVar22[0x1b];
    puVar18[0x1c] = ppppppppppfVar22[0x1c];
    puVar18[0x1d] = ppppppppppfVar22[0x1d];
    puVar18[0x1e] = ppppppppppfVar22[0x1e];
    puVar18[0x1f] = ppppppppppfVar22[0x1f];
    puVar18[0x20] = ppppppppppfVar22[0x20];
    puVar18[0x21] = ppppppppppfVar22[0x21];
    puVar18[0x22] = ppppppppppfVar22[0x22];
    puVar18[0x23] = ppppppppppfVar22[0x23];
    *(undefined1 *)(puVar18 + 0x24) = *(undefined1 *)(ppppppppppfVar22 + 0x24);
    *(undefined1 *)((int)puVar18 + 0x91) = *(undefined1 *)((int)ppppppppppfVar22 + 0x91);
    *(undefined1 *)((int)puVar18 + 0x92) = *(undefined1 *)((int)ppppppppppfVar22 + 0x92);
    *(undefined1 *)((int)puVar18 + 0x93) = *(undefined1 *)((int)ppppppppppfVar22 + 0x93);
    *(undefined1 *)(puVar18 + 0x25) = *(undefined1 *)(ppppppppppfVar22 + 0x25);
    *(undefined1 *)((int)puVar18 + 0x95) = *(undefined1 *)((int)ppppppppppfVar22 + 0x95);
    *(undefined1 *)((int)puVar18 + 0x96) = *(undefined1 *)((int)ppppppppppfVar22 + 0x96);
    *(undefined1 *)((int)puVar18 + 0x97) = *(undefined1 *)((int)ppppppppppfVar22 + 0x97);
    *(undefined1 *)(puVar18 + 0x26) = *(undefined1 *)(ppppppppppfVar22 + 0x26);
    puVar18[0x1e] = ppppppppppfVar56[0x1c];
    puVar18[0x1f] = ppppppppppfVar56[0x1d];
    puVar18[0x15] = ppppppppppfVar56[0xf];
    puVar18[0x16] = ppppppppppfVar56[0x10];
    puVar18[0x17] = ppppppppppfVar56[0x11];
    puVar18[0x18] = ppppppppppfVar56[0x12];
    puVar18[0x19] = ppppppppppfVar56[0x13];
    pppppppppfVar23 = ppppppppppfVar56[0x14];
  }
  else {
    if (local_1f0c != (float ***********)0x2 && local_1f0c != (float ***********)0x3)
    goto LAB_001f8834;
    *puVar18 = *ppppppppppfVar56;
    puVar18[1] = ppppppppppfVar56[1];
    puVar18[2] = ppppppppppfVar56[2];
    puVar18[3] = ppppppppppfVar56[3];
    puVar18[4] = ppppppppppfVar56[4];
    puVar18[5] = ppppppppppfVar56[5];
    puVar18[6] = ppppppppppfVar56[6];
    puVar18[7] = ppppppppppfVar56[7];
    puVar18[8] = ppppppppppfVar56[8];
    puVar18[9] = ppppppppppfVar56[9];
    puVar18[10] = ppppppppppfVar56[10];
    puVar18[0xb] = ppppppppppfVar56[0xb];
    puVar18[0xc] = ppppppppppfVar56[0xc];
    puVar18[0xd] = ppppppppppfVar56[0xd];
    puVar18[0xe] = ppppppppppfVar56[0xe];
    puVar18[0xf] = ppppppppppfVar56[0xf];
    puVar18[0x10] = ppppppppppfVar56[0x10];
    puVar18[0x11] = ppppppppppfVar56[0x11];
    puVar18[0x12] = ppppppppppfVar56[0x12];
    puVar18[0x13] = ppppppppppfVar56[0x13];
    puVar18[0x14] = ppppppppppfVar56[0x14];
    puVar18[0x15] = ppppppppppfVar56[0x15];
    puVar18[0x16] = ppppppppppfVar56[0x16];
    puVar18[0x17] = ppppppppppfVar56[0x17];
    puVar18[0x18] = ppppppppppfVar56[0x18];
    puVar18[0x19] = ppppppppppfVar56[0x19];
    puVar18[0x1a] = ppppppppppfVar56[0x1a];
    puVar18[0x1b] = ppppppppppfVar56[0x1b];
    puVar18[0x1c] = ppppppppppfVar56[0x1c];
    puVar18[0x1d] = ppppppppppfVar56[0x1d];
    puVar18[0x1e] = ppppppppppfVar56[0x1e];
    puVar18[0x1f] = ppppppppppfVar56[0x1f];
    puVar18[0x20] = ppppppppppfVar56[0x20];
    puVar18[0x21] = ppppppppppfVar56[0x21];
    puVar18[0x22] = ppppppppppfVar56[0x22];
    puVar18[0x23] = ppppppppppfVar56[0x23];
    *(undefined1 *)(puVar18 + 0x24) = *(undefined1 *)(ppppppppppfVar56 + 0x24);
    *(undefined1 *)((int)puVar18 + 0x91) = *(undefined1 *)((int)ppppppppppfVar56 + 0x91);
    *(undefined1 *)((int)puVar18 + 0x92) = *(undefined1 *)((int)ppppppppppfVar56 + 0x92);
    *(undefined1 *)((int)puVar18 + 0x93) = *(undefined1 *)((int)ppppppppppfVar56 + 0x93);
    *(undefined1 *)(puVar18 + 0x25) = *(undefined1 *)(ppppppppppfVar56 + 0x25);
    *(undefined1 *)((int)puVar18 + 0x95) = *(undefined1 *)((int)ppppppppppfVar56 + 0x95);
    *(undefined1 *)((int)puVar18 + 0x96) = *(undefined1 *)((int)ppppppppppfVar56 + 0x96);
    *(undefined1 *)((int)puVar18 + 0x97) = *(undefined1 *)((int)ppppppppppfVar56 + 0x97);
    *(undefined1 *)(puVar18 + 0x26) = *(undefined1 *)(ppppppppppfVar56 + 0x26);
    puVar18[0x1e] = ppppppppppfVar22[0x1c];
    puVar18[0x1f] = ppppppppppfVar22[0x1d];
    puVar18[0x15] = ppppppppppfVar22[0xf];
    puVar18[0x16] = ppppppppppfVar22[0x10];
    puVar18[0x17] = ppppppppppfVar22[0x11];
    puVar18[0x18] = ppppppppppfVar22[0x12];
    puVar18[0x19] = ppppppppppfVar22[0x13];
    pppppppppfVar23 = ppppppppppfVar22[0x14];
  }
  puVar18[0x1a] = pppppppppfVar23;
LAB_001f8834:
  pppppppppppfVar49 = (float ***********)((int)pppppppppppfVar9 + 1);
LAB_001f883c:
  local_1f00[0] = (float ***********)((int)local_1f00[0] + 1);
  pppppppppfVar23 = pppppppppfVar48;
  if ((int)*local_1f14 <= (int)local_1f00[0]) goto LAB_001f8858;
  goto LAB_001f7dc4;
LAB_001f9d14:
  do {
    if ((int)pppppppppppfVar9 <= (int)pppppppppppfVar78) break;
    if (pppppppppppfVar78 < *pppppppppppfVar17) {
      ppppppppppfVar22 = pppppppppppfVar34[0x2b] + (int)pppppppppppfVar78 * 0x27;
    }
    else {
      ppppppppppfVar22 = pppppppppppfVar34[0x2b];
    }
    *(undefined1 *)((int)ppppppppppfVar22 + 0x91) = 1;
    if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
      func_00115C70((float *)(local_1f28 + 3),(float *)&local_1ea4,(float *)(ppppppppppfVar22 + 0xf)
                   );
      func_00115D70((float *)&local_1e98,(float *)(ppppppppppfVar22 + 6),
                    ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[3]) +
                    ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[4]) +
                    ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f14));
      local_1f10 = local_1e98;
      local_1f0c = local_1e94;
      local_1f08 = local_1e90;
      func_00115C70((float *)&local_1e98,(float *)&local_1ea4,(float *)&local_1f10);
      ppppppppppfVar22[0xf] = (float *********)local_1e98;
      ppppppppppfVar22[0x10] = (float *********)local_1e94;
      ppppppppppfVar22[0x11] = (float *********)local_1e90;
    }
    else {
      if (pppppppppppfVar78 != pppppppppppfVar15 && bVar44) {
        fVar55 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1eb8) +
                 ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1eb4) +
                 ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1eb0);
        fVar5 = fVar55;
        if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
          fVar5 = -fVar55;
        }
        if (DAT_001fa16c < (int)ALBW_AS_U32(fVar5)) {
          func_00115C70((float *)&local_1e80,(float *)&local_1ea4,(float *)(ppppppppppfVar22 + 9));
          local_1e8c = local_1e80;
          local_1e88 = local_1e7c;
          local_1e84 = local_1e78;
          func_00115D70((float *)(local_1f28 + 4),(float *)&local_1eb8,
                        ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1e80) +
                        ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1e7c) +
                        ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1e78));
          local_1f0c = local_1f28[4];
          local_1f08 = local_1f14;
          local_1f04 = local_1f10;
          func_00115D88((float *)(local_1f28 + 3),(float *)&local_1f0c,fVar55);
          local_1e8c = local_1f28[3];
          local_1e88 = local_1f28[4];
          local_1e84 = local_1f14;
          func_00115C70((float *)local_1dd8,(float *)&local_1ea4,(float *)&local_1e8c);
          local_1e98 = local_1dd8[0];
          local_1e94 = local_1dd8[1];
          local_1e90 = local_1dd8[2];
          func_00115C70((float *)&local_1e80,(float *)&local_1e98,(float *)(ppppppppppfVar22 + 0xf))
          ;
          local_1f0c = local_1e80;
          local_1f08 = local_1e7c;
          local_1f04 = local_1e78;
          if (ALBW_AS_F32(pppppppppfVar23) <
              ALBW_AS_F32(local_1e80) * ALBW_AS_F32(ppppppppppfVar22[3]) +
              ALBW_AS_F32(local_1e7c) * ALBW_AS_F32(ppppppppppfVar22[4]) +
              ALBW_AS_F32(local_1e78) * ALBW_AS_F32(ppppppppppfVar22[5])) {
            ppppppppppfVar22[0x15] = (float *********)local_1e98;
            ppppppppppfVar22[0x16] = (float *********)local_1e94;
            ppppppppppfVar22[0x17] = (float *********)local_1e90;
            goto LAB_001f9d04;
          }
        }
      }
      func_00115C70((float *)(local_1f28 + 3),(float *)&local_1ea4,(float *)(ppppppppppfVar22 + 0xf)
                   );
      func_00115D70((float *)&local_1e98,(float *)(ppppppppppfVar22 + 6),
                    ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[3]) +
                    ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[4]) +
                    ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f14));
      local_1f10 = local_1e98;
      local_1f0c = local_1e94;
      local_1f08 = local_1e90;
      func_00115C70((float *)&local_1e98,(float *)&local_1ea4,(float *)&local_1f10);
      ppppppppppfVar22[0x15] = (float *********)local_1e98;
      ppppppppppfVar22[0x16] = (float *********)local_1e94;
      ppppppppppfVar22[0x17] = (float *********)local_1e90;
    }
LAB_001f9d04:
    pppppppppppfVar78 = (float ***********)((int)pppppppppppfVar78 + 1);
LAB_001f9d08:
  } while ((int)pppppppppppfVar78 <= (int)local_1ec8);
  goto LAB_001f9d1c;
LAB_001f79e4:
  local_1f0c = (float ***********)0x1;
LAB_001f7a98:
  uVar36 = 0;
  if (0 < (int)*DAT_001f7ec8) {
    do {
      puVar4 = DAT_001f7ec8;
      uVar16 = DAT_001f7ec8[2];
      if (uVar36 < *DAT_001f7ec8) {
        uVar16 = uVar16 + uVar36 * 0x9c;
      }
      *(undefined1 *)(uVar16 + 0x93) = 0;
      puVar18 = (undefined4 *)puVar4[2];
      if (uVar36 < *puVar4) {
        puVar18 = puVar18 + uVar36 * 0x27;
      }
      ppppppppppfVar22 = *pppppppppppfVar15;
      if ((int)ppppppppppfVar22 < (int)pppppppppppfVar15[1]) {
        *pppppppppppfVar15 = (float **********)((int)ppppppppppfVar22 + 1);
        ppppppppppfVar22 = pppppppppppfVar15[2] + (int)ppppppppppfVar22 * 0x27;
        local_1f28[1] = (float ***********)(ppppppppppfVar22 + 9);
        *ppppppppppfVar22 = (float *********)*puVar18;
        local_1ec4 = (float ************)(puVar18 + 0xf);
        ppppppppppfVar22[1] = (float *********)puVar18[1];
        local_1ec8 = (float ***********)(ppppppppppfVar22 + 0xf);
        ppppppppppfVar22[2] = (float *********)puVar18[2];
        ppppppppppfVar22[3] = (float *********)puVar18[3];
        ppppppppppfVar22[4] = (float *********)puVar18[4];
        ppppppppppfVar22[5] = (float *********)puVar18[5];
        ppppppppppfVar22[6] = (float *********)puVar18[6];
        ppppppppppfVar22[7] = (float *********)puVar18[7];
        ppppppppppfVar22[8] = (float *********)puVar18[8];
        *local_1f28[1] = (float **********)puVar18[9];
        ppppppppppfVar22[10] = (float *********)puVar18[10];
        ppppppppppfVar22[0xb] = (float *********)puVar18[0xb];
        ppppppppppfVar22[0xc] = (float *********)puVar18[0xc];
        ppppppppppfVar22[0xd] = (float *********)puVar18[0xd];
        ppppppppppfVar22[0xe] = (float *********)puVar18[0xe];
        *local_1ec8 = (float **********)*local_1ec4;
        ppppppppppfVar22[0x10] = (float *********)puVar18[0x10];
        ppppppppppfVar22[0x11] = (float *********)puVar18[0x11];
        ppppppppppfVar22[0x12] = (float *********)puVar18[0x12];
        ppppppppppfVar22[0x13] = (float *********)puVar18[0x13];
        ppppppppppfVar22[0x14] = (float *********)puVar18[0x14];
        ppppppppppfVar22[0x15] = (float *********)puVar18[0x15];
        ppppppppppfVar22[0x16] = (float *********)puVar18[0x16];
        ppppppppppfVar22[0x17] = (float *********)puVar18[0x17];
        ppppppppppfVar22[0x18] = (float *********)puVar18[0x18];
        ppppppppppfVar22[0x19] = (float *********)puVar18[0x19];
        ppppppppppfVar22[0x1a] = (float *********)puVar18[0x1a];
        ppppppppppfVar22[0x1b] = (float *********)puVar18[0x1b];
        ppppppppppfVar22[0x1c] = (float *********)puVar18[0x1c];
        ppppppppppfVar22[0x1d] = (float *********)puVar18[0x1d];
        ppppppppppfVar22[0x1e] = (float *********)puVar18[0x1e];
        ppppppppppfVar22[0x1f] = (float *********)puVar18[0x1f];
        ppppppppppfVar22[0x20] = (float *********)puVar18[0x20];
        ppppppppppfVar22[0x21] = (float *********)puVar18[0x21];
        ppppppppppfVar22[0x22] = (float *********)puVar18[0x22];
        ppppppppppfVar22[0x23] = (float *********)puVar18[0x23];
        *(undefined1 *)(ppppppppppfVar22 + 0x24) = *(undefined1 *)(puVar18 + 0x24);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x91) = *(undefined1 *)((int)puVar18 + 0x91);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x92) = *(undefined1 *)((int)puVar18 + 0x92);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x93) = *(undefined1 *)((int)puVar18 + 0x93);
        *(undefined1 *)(ppppppppppfVar22 + 0x25) = *(undefined1 *)(puVar18 + 0x25);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = *(undefined1 *)((int)puVar18 + 0x95);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x96) = *(undefined1 *)((int)puVar18 + 0x96);
        *(undefined1 *)((int)ppppppppppfVar22 + 0x97) = *(undefined1 *)((int)puVar18 + 0x97);
        *(undefined1 *)(ppppppppppfVar22 + 0x26) = *(undefined1 *)(puVar18 + 0x26);
      }
      uVar36 = uVar36 + 1;
    } while ((int)uVar36 < (int)*DAT_001f7ec8);
  }
  if (local_1f0c != (float ***********)0x0) {
    local_1de8 = (float ***********)((uint)local_1de8 | 2);
  }
LAB_001f7d24:
  pppppppppppfVar15 = local_1e88;
  local_1eac = (float ************)local_1de8;
  local_1f14 = pppppppppppfVar34 + 0x29;
  local_1f00[1] = pppppppppppfVar34;
  if (((*DAT_001f7ed8 & 1) == 0) &&
     (iVar21 = func_00100174((int *)DAT_001f7ed8), puVar4 = DAT_001f7edc, iVar21 != 0)) {
    puVar26 = DAT_001f7edc + 3;
    *DAT_001f7edc = 0;
    puVar4[1] = 0x30;
    puVar4[2] = (uint)puVar26;
    func_00100118((int)(puVar4 + 3),DAT_001f7ecc,0x9c,0x30);
  }
  pppppppppppfVar49 = (float ***********)0x0;
  *DAT_001f7edc = 0;
  local_1f00[0] = (float ***********)0x0;
  pppppppppfVar23 = pppppppppfVar48;
  if (0 < (int)*local_1f14) {
LAB_001f7dc4:
    iVar21 = DAT_001f7ee0;
    local_1f0c = (float ***********)local_1eac;
    if (local_1f00[0] < *local_1f14) {
      ppppppppppfVar22 = local_1f14[2] + (int)local_1f00[0] * 0x27;
    }
    else {
      ppppppppppfVar22 = local_1f14[2];
    }
    pppppppppppfVar78 = (float ***********)*pppppppppppfVar15;
    pppppppppfVar48 = pppppppppfVar23;
    if ((int)pppppppppppfVar49 < (int)pppppppppppfVar78) {
      pppppppppppfVar9 = pppppppppppfVar49;
      do {
        ppppppppppfVar56 = pppppppppppfVar15[2];
        if (pppppppppppfVar9 < pppppppppppfVar78) {
          ppppppppppfVar56 = ppppppppppfVar56 + (int)pppppppppppfVar9 * 0x27;
        }
        fVar5 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(ppppppppppfVar56[6]) +
                ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(ppppppppppfVar56[7]) +
                ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(ppppppppppfVar56[8]);
        if (fVar5 <= ALBW_AS_F32(pppppppppfVar23)) {
          fVar5 = -fVar5;
        }
        if (iVar21 < (int)ALBW_AS_U32(fVar5)) {
          func_00115C70((float *)(local_1f28 + 2),(float *)(ppppppppppfVar22 + 9),
                        (float *)(ppppppppppfVar56 + 9));
          fVar5 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[2]) +
                  ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[3]) +
                  ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f28[4]);
          if (fVar5 <= ALBW_AS_F32(pppppppppfVar23)) {
            fVar5 = -fVar5;
          }
          if ((int)ALBW_AS_U32(fVar5) < DAT_001f7ed4) {
            local_1f28[2] = pppppppppppfVar9;
            if (-1 < (int)pppppppppppfVar9) goto joined_r0x001f8050;
            break;
          }
        }
        pppppppppppfVar78 = (float ***********)*pppppppppppfVar15;
        pppppppppppfVar9 = (float ***********)((int)pppppppppppfVar9 + 1);
      } while ((int)pppppppppppfVar9 < (int)pppppppppppfVar78);
    }
    ppppppppppfVar22[0x1f] = pppppppppfVar48;
    ppppppppppfVar22[0x1e] = pppppppppfVar48;
    func_00115D70((float *)&local_1ea8,(float *)(ppppppppppfVar22 + 3),ALBW_AS_F32(local_1f00[1][0x1c]));
    local_1ebc = local_1ea8;
    local_1eb8 = local_1ea4;
    local_1eb4 = (float ************)local_1ea0;
    func_00115D88((float *)&local_1e90,(float *)&local_1ebc,ALBW_AS_F32(ppppppppppfVar22[4]));
    local_1ec8 = local_1e90;
    local_1ec4 = (float ************)local_1e8c;
    local_1ec0 = (float ************)local_1e88;
    func_00115C54((float *)&local_1e84,(float *)(ppppppppppfVar22 + 9),(float *)&local_1ec8);
    local_1f0c = local_1e84;
    local_1f08 = local_1e80;
    local_1f04 = local_1e7c;
    ppppppppppfVar22[0x15] = (float *********)local_1e84;
    ppppppppppfVar22[0x16] = (float *********)local_1e80;
    ppppppppppfVar22[0x17] = (float *********)local_1e7c;
    func_00115D70((float *)&local_1e9c,(float *)(ppppppppppfVar22 + 3),ALBW_AS_F32(local_1f00[1][0x1c]));
    local_1ebc = local_1e9c;
    local_1eb8 = local_1e98;
    local_1eb4 = (float ************)local_1e94;
    func_00115D88((float *)&local_1e78,(float *)&local_1ebc,ALBW_AS_F32(ppppppppppfVar22[4]));
    local_1f0c = local_1e78;
    local_1f08 = local_1e74;
    local_1f04 = local_1e70;
    func_00115C54((float *)&local_1e6c,(float *)(ppppppppppfVar22 + 0xc),(float *)&local_1f0c);
    local_1ec8 = local_1e6c;
    local_1ec4 = (float ************)local_1e68;
    local_1ec0 = (float ************)local_1e64;
    ppppppppppfVar22[0x18] = (float *********)local_1e6c;
    ppppppppppfVar22[0x19] = (float *********)local_1e68;
    ppppppppppfVar22[0x1a] = (float *********)local_1e64;
    *(undefined1 *)(ppppppppppfVar22 + 0x26) = 1;
    uVar36 = *DAT_001f7edc;
    if ((int)uVar36 < (int)DAT_001f7edc[1]) {
      puVar4 = DAT_001f7edc + 2;
      *DAT_001f7edc = uVar36 + 1;
      func_001FE514((undefined4 *)(*puVar4 + uVar36 * 0x9c),ppppppppppfVar22);
    }
    goto LAB_001f883c;
  }
LAB_001f8858:
  ppppppppppfVar22 = *local_1f14;
  ppppppppppfVar56 = local_1f14[2];
  pppppppppppfVar78 = (float ***********)((int)ppppppppppfVar22 * 7);
  pppppppppppfVar9 = (float ***********)*pppppppppppfVar15;
  bVar47 = pppppppppppfVar49 <= pppppppppppfVar9;
  bVar44 = pppppppppppfVar9 == pppppppppppfVar49;
  if ((int)pppppppppppfVar49 < (int)pppppppppppfVar9) {
    do {
      ppppppppppfVar73 = pppppppppppfVar15[2];
      if (bVar47 && !bVar44) {
        ppppppppppfVar73 = ppppppppppfVar73 + (int)pppppppppppfVar49 * 0x27;
      }
      ppppppppppfVar73[0x1e] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -10];
      ppppppppppfVar73[0x1f] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -10];
      ppppppppppfVar73[0x15] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x15];
      ppppppppppfVar73[0x16] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x14];
      ppppppppppfVar73[0x17] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x13];
      ppppppppppfVar73[0x18] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x15];
      ppppppppppfVar73[0x19] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x14];
      ppppppppppfVar73[0x1a] = ppppppppppfVar56[(int)ppppppppppfVar22 * 0x27 + -0x13];
      *(undefined1 *)((int)ppppppppppfVar73 + 0x97) = 1;
      ppppppppppfVar73 = pppppppppppfVar15[2];
      if (pppppppppppfVar49 < *pppppppppppfVar15) {
        ppppppppppfVar73 = ppppppppppfVar73 + (int)pppppppppppfVar49 * 0x27;
      }
      pppppppppppfVar78 = (float ***********)*DAT_001f7edc;
      if ((int)pppppppppppfVar78 < (int)DAT_001f7edc[1]) {
        puVar4 = DAT_001f7edc + 2;
        *DAT_001f7edc = (uint)((int)pppppppppppfVar78 + 1);
        local_1ec8 = (float ***********)(ppppppppppfVar73 + 6);
        local_1f0c = (float ***********)(ppppppppppfVar73 + 9);
        puVar18 = (undefined4 *)(*puVar4 + (int)pppppppppppfVar78 * 0x9c);
        local_1f08 = (float ***********)(ppppppppppfVar73 + 0xc);
        *puVar18 = *ppppppppppfVar73;
        local_1f28[4] = (float ***********)(puVar18 + 0xc);
        puVar18[1] = ppppppppppfVar73[1];
        puVar18[2] = ppppppppppfVar73[2];
        local_1f28[1] = (float ***********)(puVar18 + 0x15);
        puVar18[3] = ppppppppppfVar73[3];
        puVar18[4] = ppppppppppfVar73[4];
        puVar18[5] = ppppppppppfVar73[5];
        puVar18[6] = *local_1ec8;
        puVar18[7] = ppppppppppfVar73[7];
        puVar18[8] = ppppppppppfVar73[8];
        puVar18[9] = *local_1f0c;
        puVar18[10] = ppppppppppfVar73[10];
        puVar18[0xb] = ppppppppppfVar73[0xb];
        *local_1f28[4] = *local_1f08;
        puVar18[0xd] = ppppppppppfVar73[0xd];
        puVar18[0xe] = ppppppppppfVar73[0xe];
        puVar18[0xf] = ppppppppppfVar73[0xf];
        puVar18[0x10] = ppppppppppfVar73[0x10];
        puVar18[0x11] = ppppppppppfVar73[0x11];
        puVar18[0x12] = ppppppppppfVar73[0x12];
        puVar18[0x13] = ppppppppppfVar73[0x13];
        puVar18[0x14] = ppppppppppfVar73[0x14];
        *local_1f28[1] = (float **********)ppppppppppfVar73[0x15];
        puVar18[0x16] = ppppppppppfVar73[0x16];
        puVar18[0x17] = ppppppppppfVar73[0x17];
        puVar18[0x18] = ppppppppppfVar73[0x18];
        puVar18[0x19] = ppppppppppfVar73[0x19];
        puVar18[0x1a] = ppppppppppfVar73[0x1a];
        puVar18[0x1b] = ppppppppppfVar73[0x1b];
        puVar18[0x1c] = ppppppppppfVar73[0x1c];
        puVar18[0x1d] = ppppppppppfVar73[0x1d];
        puVar18[0x1e] = ppppppppppfVar73[0x1e];
        puVar18[0x1f] = ppppppppppfVar73[0x1f];
        puVar18[0x20] = ppppppppppfVar73[0x20];
        puVar18[0x21] = ppppppppppfVar73[0x21];
        puVar18[0x22] = ppppppppppfVar73[0x22];
        puVar18[0x23] = ppppppppppfVar73[0x23];
        *(undefined1 *)(puVar18 + 0x24) = *(undefined1 *)(ppppppppppfVar73 + 0x24);
        *(undefined1 *)((int)puVar18 + 0x91) = *(undefined1 *)((int)ppppppppppfVar73 + 0x91);
        *(undefined1 *)((int)puVar18 + 0x92) = *(undefined1 *)((int)ppppppppppfVar73 + 0x92);
        *(undefined1 *)((int)puVar18 + 0x93) = *(undefined1 *)((int)ppppppppppfVar73 + 0x93);
        *(undefined1 *)(puVar18 + 0x25) = *(undefined1 *)(ppppppppppfVar73 + 0x25);
        *(undefined1 *)((int)puVar18 + 0x95) = *(undefined1 *)((int)ppppppppppfVar73 + 0x95);
        *(undefined1 *)((int)puVar18 + 0x96) = *(undefined1 *)((int)ppppppppppfVar73 + 0x96);
        pppppppppppfVar78 = (float ***********)(uint)*(byte *)((int)ppppppppppfVar73 + 0x97);
        *(byte *)((int)puVar18 + 0x97) = *(byte *)((int)ppppppppppfVar73 + 0x97);
        *(undefined1 *)(puVar18 + 0x26) = *(undefined1 *)(ppppppppppfVar73 + 0x26);
      }
      pppppppppppfVar9 = (float ***********)*pppppppppppfVar15;
      pppppppppppfVar49 = (float ***********)((int)pppppppppppfVar49 + 1);
      bVar47 = pppppppppppfVar49 <= pppppppppppfVar9;
      bVar44 = pppppppppppfVar9 == pppppppppppfVar49;
    } while (!bVar44 && (int)pppppppppppfVar49 <= (int)pppppppppppfVar9);
  }
  uVar36 = 0;
  if (getenv("ALBW_WALK_TRACE"))   /* WALKER debug: second segment copy */
    fprintf(stderr, "walk: seg-copy B walker %p target %p\n", (void *)local_30, (void *)local_1f00[1]);
  local_1f00[1][0x29] = (float **********)0x0;
  pppppppppppfVar15 = (float ***********)*DAT_001f7edc;
  if (0 < (int)pppppppppppfVar15) {
    do {
      pppppppppppfVar15 = (float ***********)DAT_001f7edc[2];
      if (uVar36 < *DAT_001f7edc) {
        pppppppppppfVar15 = pppppppppppfVar15 + uVar36 * 0x27;
      }
      pppppppppppfVar78 = (float ***********)local_1f00[1][0x29];
      if ((int)pppppppppppfVar78 < (int)local_1f00[1][0x2a]) {
        local_1f00[1][0x29] = (float **********)((int)pppppppppppfVar78 + 1);
        local_1f28[4] = pppppppppppfVar15 + 9;
        ppppppppppfVar22 = local_1f00[1][0x2b] + (int)pppppppppppfVar78 * 0x27;
        *ppppppppppfVar22 = (float *********)*pppppppppppfVar15;
        ppppppppppfVar22[1] = (float *********)pppppppppppfVar15[1];
        ppppppppppfVar22[2] = (float *********)pppppppppppfVar15[2];
        ppppppppppfVar22[3] = (float *********)pppppppppppfVar15[3];
        ppppppppppfVar22[4] = (float *********)pppppppppppfVar15[4];
        ppppppppppfVar22[5] = (float *********)pppppppppppfVar15[5];
        ppppppppppfVar22[6] = (float *********)pppppppppppfVar15[6];
        ppppppppppfVar22[7] = (float *********)pppppppppppfVar15[7];
        ppppppppppfVar22[8] = (float *********)pppppppppppfVar15[8];
        ppppppppppfVar22[9] = (float *********)*local_1f28[4];
        ppppppppppfVar22[10] = (float *********)pppppppppppfVar15[10];
        ppppppppppfVar22[0xb] = (float *********)pppppppppppfVar15[0xb];
        ppppppppppfVar22[0xc] = (float *********)pppppppppppfVar15[0xc];
        ppppppppppfVar22[0xd] = (float *********)pppppppppppfVar15[0xd];
        ppppppppppfVar22[0xe] = (float *********)pppppppppppfVar15[0xe];
        ppppppppppfVar22[0xf] = (float *********)pppppppppppfVar15[0xf];
        ppppppppppfVar22[0x10] = (float *********)pppppppppppfVar15[0x10];
        ppppppppppfVar22[0x11] = (float *********)pppppppppppfVar15[0x11];
        ppppppppppfVar22[0x12] = (float *********)pppppppppppfVar15[0x12];
        ppppppppppfVar22[0x13] = (float *********)pppppppppppfVar15[0x13];
        ppppppppppfVar22[0x14] = (float *********)pppppppppppfVar15[0x14];
        ppppppppppfVar22[0x15] = (float *********)pppppppppppfVar15[0x15];
        ppppppppppfVar22[0x16] = (float *********)pppppppppppfVar15[0x16];
        ppppppppppfVar22[0x17] = (float *********)pppppppppppfVar15[0x17];
        ppppppppppfVar22[0x18] = (float *********)pppppppppppfVar15[0x18];
        ppppppppppfVar22[0x19] = (float *********)pppppppppppfVar15[0x19];
        ppppppppppfVar22[0x1a] = (float *********)pppppppppppfVar15[0x1a];
        ppppppppppfVar22[0x1b] = (float *********)pppppppppppfVar15[0x1b];
        ppppppppppfVar22[0x1c] = (float *********)pppppppppppfVar15[0x1c];
        ppppppppppfVar22[0x1d] = (float *********)pppppppppppfVar15[0x1d];
        ppppppppppfVar22[0x1e] = (float *********)pppppppppppfVar15[0x1e];
        ppppppppppfVar22[0x1f] = (float *********)pppppppppppfVar15[0x1f];
        ppppppppppfVar22[0x20] = (float *********)pppppppppppfVar15[0x20];
        ppppppppppfVar22[0x21] = (float *********)pppppppppppfVar15[0x21];
        ppppppppppfVar22[0x22] = (float *********)pppppppppppfVar15[0x22];
        ppppppppppfVar22[0x23] = (float *********)pppppppppppfVar15[0x23];
        *(char *)(ppppppppppfVar22 + 0x24) = *(char *)(pppppppppppfVar15 + 0x24);
        *(char *)((int)ppppppppppfVar22 + 0x91) = *(char *)((int)pppppppppppfVar15 + 0x91);
        *(char *)((int)ppppppppppfVar22 + 0x92) = *(char *)((int)pppppppppppfVar15 + 0x92);
        *(char *)((int)ppppppppppfVar22 + 0x93) = *(char *)((int)pppppppppppfVar15 + 0x93);
        *(char *)(ppppppppppfVar22 + 0x25) = *(char *)(pppppppppppfVar15 + 0x25);
        *(char *)((int)ppppppppppfVar22 + 0x95) = *(char *)((int)pppppppppppfVar15 + 0x95);
        *(char *)((int)ppppppppppfVar22 + 0x96) = *(char *)((int)pppppppppppfVar15 + 0x96);
        pppppppppppfVar78 = (float ***********)(uint)*(byte *)((int)pppppppppppfVar15 + 0x97);
        *(byte *)((int)ppppppppppfVar22 + 0x97) = *(byte *)((int)pppppppppppfVar15 + 0x97);
        pppppppppppfVar49 = pppppppppppfVar15 + 0x26;
        pppppppppppfVar15 = (float ***********)(uint)*(byte *)pppppppppppfVar49;
        *(byte *)(ppppppppppfVar22 + 0x26) = *(byte *)pppppppppppfVar49;
      }
      uVar36 = uVar36 + 1;
    } while ((int)uVar36 < (int)*DAT_001f7edc);
  }
  pppppppppfVar23 = pppppppppfVar48;
  pppppppppppfVar49 = pppppppppppfVar34;
  if (((uint)local_1de8 & 1) != 0) {
    ppppppppppfVar22 = (float **********)0x0;
    pppppppppppfVar75 = (float ***********)0x1;
    pppppppppppfVar17 = pppppppppppfVar34 + 0x29;
    pppppppppppfVar9 = (float ***********)*pppppppppppfVar17;
    local_1e54 = (float ***********)0x0;
    local_1e34 = (float ***********)0x0;
    if (0 < (int)pppppppppppfVar9) {
      do {
        pppppppppppfVar15 = (float ***********)pppppppppppfVar34[0x2b];
        if (ppppppppppfVar22 < *pppppppppppfVar17) {
          pppppppppppfVar15 = pppppppppppfVar15 + (int)ppppppppppfVar22 * 0x27;
        }
        pppppppppppfVar78 = (float ***********)pppppppppppfVar34[0x2b];
        if (ppppppppppfVar22 < *pppppppppppfVar17) {
          pppppppppppfVar78 = pppppppppppfVar78 + (int)ppppppppppfVar22 * 0x27;
        }
        ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
        *(char *)(pppppppppppfVar78 + 0x25) = '\0';
        *(char *)((int)pppppppppppfVar15 + 0x95) = '\0';
      } while ((int)ppppppppppfVar22 < (int)pppppppppppfVar9);
    }
    if (1 < (int)pppppppppppfVar9) {
      do {
        pppppfVar41 = DAT_001f7ed0;
        pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar75 + -1);
        if (pppppppppppfVar15 < *pppppppppppfVar17) {
          pppppppppppfVar78 = (float ***********)((int)pppppppppppfVar15 * 7);
          ppppppppppfVar22 = pppppppppppfVar34[0x2b] + (int)pppppppppppfVar15 * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar34[0x2b];
        }
        if (pppppppppppfVar75 < *pppppppppppfVar17) {
          pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar75 * 0x27);
          ppppppppppfVar56 = pppppppppppfVar34[0x2b] + (int)pppppppppppfVar75 * 0x27;
        }
        else {
          ppppppppppfVar56 = pppppppppppfVar34[0x2b];
          pppppppppppfVar15 = pppppppppppfVar75;
        }
        if ((((int)ppppppppppfVar22[7] <= (int)DAT_001f7ed0) &&
            ((int)ppppppppppfVar56[7] <= (int)DAT_001f7ed0)) ||
           (pppppppppppfVar15 = (float ***********)DAT_001f7eb8,
           (float *******)
           ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar56[6]) +
            ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar56[7]) +
           ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar56[8]))) <= DAT_001f7eb8)) break;
        fVar5 = ALBW_AS_F32(pppppppppppfVar49[0x1c]) / ALBW_AS_F32(ppppppppppfVar22[4]);
        uVar67 = CONCAT44(pppppppppfVar23,fVar5);
        if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
          pppppppppppfVar15 = (float ***********)(ppppppppppfVar22 + 0x18);
        }
        else {
          pppppppppppfVar15 = (float ***********)(ppppppppppfVar22 + 0xc);
        }
        fVar55 = ALBW_AS_F32(pppppppppppfVar49[0x1c]) / ALBW_AS_F32(ppppppppppfVar56[4]);
        local_1eac = (float ************)pppppppppppfVar15;
        func_00115D70((float *)&local_1e98,(float *)(ppppppppppfVar22 + 3),fVar5);
        local_1eb8 = local_1e98;
        local_1eb4 = (float ************)local_1e94;
        local_1eb0 = local_1e90;
        func_00115C54((float *)&local_1e00,(float *)pppppppppppfVar15,(float *)&local_1eb8);
        local_1f28[2] = local_1e00;
        local_1f28[3] = local_1dfc;
        local_1f28[4] = local_1df8;
        local_1e8c = local_1e00;
        local_1e88 = local_1dfc;
        local_1e84 = local_1df8;
        if (*(char *)((int)ppppppppppfVar56 + 0x93) == '\0') {
          ppppppppppfVar73 = ppppppppppfVar56 + 0x15;
        }
        else {
          ppppppppppfVar73 = ppppppppppfVar56 + 9;
        }
        func_00115D70((float *)&local_1e4c,(float *)(ppppppppppfVar56 + 3),fVar55);
        local_1f28[2] = local_1e4c;
        local_1f28[3] = local_1e48;
        local_1f28[4] = local_1e44;
        pppppppppppfVar78 = local_1e44;
        func_00115C54((float *)&local_1e18,(float *)ppppppppppfVar73,(float *)(local_1f28 + 2));
        local_1eb8 = local_1e18;
        local_1eb4 = local_1e14;
        local_1eb0 = (float ***********)local_1e10;
        local_1f14 = local_1e18;
        local_1f10 = (float ***********)local_1e14;
        local_1f0c = (float ***********)local_1e10;
        fVar5 = ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar56[6]) +
                ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar56[7]) +
                ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar56[8]);
        func_00115C70((float *)&local_1e6c,(float *)&local_1e8c,(float *)ppppppppppfVar73);
        fVar5 = (-(ALBW_AS_F32(ppppppppppfVar56[6]) * ALBW_AS_F32(local_1e6c) +
                  ALBW_AS_F32(ppppppppppfVar56[7]) * ALBW_AS_F32(local_1e68)) +
                -(ALBW_AS_F32(ppppppppppfVar56[8]) * ALBW_AS_F32(local_1e64))) / fVar5;
        uVar70 = CONCAT44(fVar5,pppppppppfVar48);
        if ((int)pppppfVar41 < (int)ALBW_AS_U32(fVar5)) {
          func_00115D70((float *)&local_1e78,(float *)ppppppppppfVar22,fVar5);
          local_1f08 = local_1e78;
          local_1f04 = local_1e74;
          local_1f00[0] = local_1e70;
          pppppppppppfVar78 = local_1e70;
          func_00115C54((float *)&local_1e40,(float *)&local_1e8c,(float *)&local_1f08);
          ppppppppppfVar22[0x12] = (float *********)local_1e40;
          ppppppppppfVar22[0x13] = (float *********)local_1e3c;
          ppppppppppfVar22[0x14] = (float *********)local_1e38;
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\x01') {
            func_00115D70((float *)&local_1e24,(float *)(ppppppppppfVar22 + 3),fVar58);
            local_1eb8 = local_1e24;
            local_1eb4 = (float ************)local_1e20;
            local_1eb0 = local_1e1c;
            func_00115D70((float *)&local_1e0c,(float *)&local_1eb8,ALBW_AS_F32(uVar67));
            local_1e98 = local_1e0c;
            local_1e94 = local_1e08;
            local_1e90 = local_1e04;
            pppppppppppfVar78 = local_1e04;
            func_00115C54((float *)(ppppppppppfVar22 + 0x12),(float *)(ppppppppppfVar22 + 0x12),
                          (float *)&local_1e98);
          }
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
            func_00115C70((float *)&local_1f08,(float *)(ppppppppppfVar22 + 0x12),
                          (float *)(ppppppppppfVar22 + 0xc));
            ppppppppppfVar22[0x1b] =
                 (float *********)
                 ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1f08) +
                  ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1f04) +
                 ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1f00[0])));
          }
          uVar70 = (ulonglong)(uint)ALBW_AS_U32((ALBW_AS_F32(uVar70) + ALBW_AS_F32((uVar70 >> 0x20))));
          *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
        }
        pppppppppfVar48 = (float *********)uVar70;
        pppppppppfVar23 = (float *********)((ulonglong)uVar67 >> 0x20);
        fVar5 = -(ALBW_AS_F32(*ppppppppppfVar56) * ALBW_AS_F32(ppppppppppfVar22[6]) +
                  ALBW_AS_F32(ppppppppppfVar56[1]) * ALBW_AS_F32(ppppppppppfVar22[7]) +
                 ALBW_AS_F32(ppppppppppfVar56[2]) * ALBW_AS_F32(ppppppppppfVar22[8]));
        func_00115C70((float *)(local_1f28 + 2),(float *)&local_1f14,(float *)local_1eac);
        fVar5 = (-(ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[2]) +
                  ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[3])) +
                -(ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f28[4]))) / fVar5;
        pppppppppppfVar15 = extraout_r1_05;
        if ((int)pppppfVar41 < (int)ALBW_AS_U32(fVar5)) {
          func_00115D70((float *)&local_1f08,(float *)ppppppppppfVar56,fVar5);
          local_1eb8 = local_1f08;
          local_1eb4 = (float ************)local_1f04;
          local_1eb0 = local_1f00[0];
          pppppppppppfVar78 = local_1f00[0];
          func_00115C70((float *)&local_1f08,(float *)&local_1f14,(float *)&local_1eb8);
          ppppppppppfVar56[0xf] = (float *********)local_1f08;
          ppppppppppfVar56[0x10] = (float *********)local_1f04;
          ppppppppppfVar56[0x11] = (float *********)local_1f00[0];
          pppppppppppfVar15 = extraout_r1_06;
          if (*(char *)((int)ppppppppppfVar56 + 0x93) == '\x01') {
            func_00115D70((float *)&local_1e98,(float *)(ppppppppppfVar56 + 3),fVar58);
            local_1f28[2] = local_1e98;
            local_1f28[3] = local_1e94;
            local_1f28[4] = local_1e90;
            func_00115D70((float *)&local_1e98,(float *)(local_1f28 + 2),fVar55);
            local_1eb8 = local_1e98;
            local_1eb4 = (float ************)local_1e94;
            local_1eb0 = local_1e90;
            pppppppppppfVar78 = local_1e90;
            func_00115C54((float *)(ppppppppppfVar56 + 0xf),(float *)(ppppppppppfVar56 + 0xf),
                          (float *)&local_1eb8);
            pppppppppppfVar15 = extraout_r1_07;
          }
          if (*(char *)((int)ppppppppppfVar56 + 0x93) == '\0') {
            func_00115C70((float *)&local_1f08,(float *)(ppppppppppfVar56 + 9),
                          (float *)(ppppppppppfVar56 + 0xf));
            ppppppppppfVar56[0x1b] =
                 (float *********)
                 ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar56) * ALBW_AS_F32(local_1f08) +
                  ALBW_AS_F32(ppppppppppfVar56[1]) * ALBW_AS_F32(local_1f04) +
                 ALBW_AS_F32(ppppppppppfVar56[2]) * ALBW_AS_F32(local_1f00[0])));
            pppppppppppfVar15 = extraout_r1_08;
          }
          pppppppppfVar48 = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar48) + fVar5));
          *(undefined1 *)(ppppppppppfVar56 + 0x25) = 1;
        }
        pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar75 + 1);
        local_1e34 = (float ***********)((int)local_1e34 + 1);
      } while ((int)pppppppppppfVar75 < (int)pppppppppppfVar9);
    }
    iVar21 = DAT_001fa164;
    pppppppppppfVar33 = pppppppppppfVar9;
    if ((int)pppppppppppfVar75 < (int)pppppppppppfVar9) {
      while (pppppppppppfVar29 = (float ***********)((int)pppppppppppfVar33 + -1),
            (void) iVar21, (int)pppppppppppfVar75 <= (int)pppppppppppfVar29) {
        pppppppppppfVar15 = (float ***********)*pppppppppppfVar17;
        if ((float ***********)((int)pppppppppppfVar33 + -2) < pppppppppppfVar15) {
          pppppppppppfVar78 = (float ***********)pppppppppppfVar34[0x2b];
          pppppppppppfVar33 = pppppppppppfVar78 + ((int)pppppppppppfVar33 + -2) * 0x27;
        }
        else {
          pppppppppppfVar33 = (float ***********)pppppppppppfVar34[0x2b];
        }
        if (pppppppppppfVar29 < pppppppppppfVar15) {
          pppppppppppfVar15 = (float ***********)pppppppppppfVar34[0x2b];
          pppppppppppfVar38 = pppppppppppfVar15 + (int)pppppppppppfVar29 * 0x27;
        }
        else {
          pppppppppppfVar38 = (float ***********)pppppppppppfVar34[0x2b];
        }
        if ((((int)pppppppppppfVar33[7] <= iVar21) && ((int)pppppppppppfVar38[7] <= iVar21)) ||
           (pppppppppppfVar15 = (float ***********)DAT_001fa168,
           (float ***********)
           ALBW_PTR_BITS((ALBW_AS_F32(*pppppppppppfVar33) * ALBW_AS_F32(pppppppppppfVar38[6]) +
            ALBW_AS_F32(pppppppppppfVar33[1]) * ALBW_AS_F32(pppppppppppfVar38[7]) +
           ALBW_AS_F32(pppppppppppfVar33[2]) * ALBW_AS_F32(pppppppppppfVar38[8]))) <= DAT_001fa168)) break;
        fVar5 = ALBW_AS_F32(pppppppppppfVar49[0x1c]) / ALBW_AS_F32(pppppppppppfVar33[4]);
        uVar67 = CONCAT44(pppppppppfVar23,fVar5);
        if (*(char *)((int)pppppppppppfVar33 + 0x93) == '\0') {
          pppppppppppfVar15 = pppppppppppfVar33 + 0x18;
        }
        else {
          pppppppppppfVar15 = pppppppppppfVar33 + 0xc;
        }
        fVar55 = ALBW_AS_F32(pppppppppppfVar49[0x1c]) / ALBW_AS_F32(pppppppppppfVar38[4]);
        local_1e64 = pppppppppppfVar15;
        func_00115D70((float *)&local_1e40,(float *)(pppppppppppfVar33 + 3),fVar5);
        local_1ec8 = local_1e40;
        local_1ec4 = (float ************)local_1e3c;
        local_1ec0 = (float ************)local_1e38;
        func_00115C54((float *)&local_1e24,(float *)pppppppppppfVar15,(float *)&local_1ec8);
        local_1e0c = local_1e24;
        local_1e08 = local_1e20;
        local_1e04 = local_1e1c;
        local_1f08 = local_1e24;
        local_1f04 = local_1e20;
        local_1f00[0] = local_1e1c;
        if (*(char *)((int)pppppppppppfVar38 + 0x93) == '\0') {
          pppppppppppfVar15 = pppppppppppfVar38 + 0x15;
        }
        else {
          pppppppppppfVar15 = pppppppppppfVar38 + 9;
        }
        local_1f28[2] = pppppppppppfVar15;
        func_00115D70((float *)&local_1e8c,(float *)(pppppppppppfVar38 + 3),fVar55);
        local_1e0c = local_1e8c;
        local_1e08 = local_1e88;
        local_1e04 = local_1e84;
        pppppppppppfVar78 = local_1e84;
        func_00115C54((float *)&local_1e98,(float *)pppppppppppfVar15,(float *)&local_1e0c);
        local_1ec8 = local_1e98;
        local_1ec4 = (float ************)local_1e94;
        local_1ec0 = (float ************)local_1e90;
        local_1f14 = local_1e98;
        local_1f10 = local_1e94;
        local_1f0c = local_1e90;
        fVar5 = ALBW_AS_F32(*pppppppppppfVar33) * ALBW_AS_F32(pppppppppppfVar38[6]) +
                ALBW_AS_F32(pppppppppppfVar33[1]) * ALBW_AS_F32(pppppppppppfVar38[7]) +
                ALBW_AS_F32(pppppppppppfVar33[2]) * ALBW_AS_F32(pppppppppppfVar38[8]);
        func_00115C70((float *)&local_1e80,(float *)&local_1f08,(float *)local_1f28[2]);
        fVar5 = -(ALBW_AS_F32(pppppppppppfVar38[6]) * ALBW_AS_F32(local_1e80) +
                  ALBW_AS_F32(pppppppppppfVar38[7]) * ALBW_AS_F32(local_1e7c) +
                 ALBW_AS_F32(pppppppppppfVar38[8]) * ALBW_AS_F32(local_1e78)) / fVar5;
        uVar70 = CONCAT44(fVar5,pppppppppfVar48);
        if (iVar21 < (int)ALBW_AS_U32(fVar5)) {
          func_00115D70((float *)&local_1ec8,(float *)pppppppppppfVar33,fVar5);
          local_1e18 = local_1ec8;
          local_1e14 = local_1ec4;
          local_1e10 = local_1ec0;
          pppppppppppfVar78 = (float ***********)local_1ec0;
          func_00115C54((float *)&local_1e74,(float *)&local_1f08,(float *)&local_1e18);
          pppppppppppfVar33[0x12] = (float **********)local_1e74;
          pppppppppppfVar33[0x13] = (float **********)local_1e70;
          pppppppppppfVar33[0x14] = (float **********)local_1e6c;
          if (*(char *)((int)pppppppppppfVar33 + 0x93) == '\x01') {
            func_00115D70((float *)(local_1f28 + 2),(float *)(pppppppppppfVar33 + 3),fVar58);
            local_1eb8 = local_1f28[2];
            local_1eb4 = (float ************)local_1f28[3];
            local_1eb0 = local_1f28[4];
            func_00115D70((float *)&local_1e98,(float *)&local_1eb8,ALBW_AS_F32(uVar67));
            local_1e4c = local_1e98;
            local_1e48 = local_1e94;
            local_1e44 = local_1e90;
            pppppppppppfVar78 = local_1e90;
            func_00115C54((float *)(pppppppppppfVar33 + 0x12),(float *)(pppppppppppfVar33 + 0x12),
                          (float *)&local_1e4c);
          }
          if (*(char *)((int)pppppppppppfVar33 + 0x93) == '\0') {
            func_00115C70((float *)&local_1ec8,(float *)(pppppppppppfVar33 + 0x12),
                          (float *)(pppppppppppfVar33 + 0xc));
            pppppppppppfVar33[0x1b] =
                 (float **********)
                 ALBW_PTR_BITS((ALBW_AS_F32(*pppppppppppfVar33) * ALBW_AS_F32(local_1ec8) +
                  ALBW_AS_F32(pppppppppppfVar33[1]) * ALBW_AS_F32(local_1ec4) +
                 ALBW_AS_F32(pppppppppppfVar33[2]) * ALBW_AS_F32(local_1ec0)));
          }
          uVar70 = (ulonglong)(uint)ALBW_AS_U32((ALBW_AS_F32(uVar70) + ALBW_AS_F32((uVar70 >> 0x20))));
          *(undefined1 *)((int)pppppppppppfVar33 + 0x95) = 1;
        }
        pppppppppfVar48 = (float *********)uVar70;
        pppppppppfVar23 = (float *********)((ulonglong)uVar67 >> 0x20);
        fVar5 = -(ALBW_AS_F32(*pppppppppppfVar38) * ALBW_AS_F32(pppppppppppfVar33[6]) +
                  ALBW_AS_F32(pppppppppppfVar38[1]) * ALBW_AS_F32(pppppppppppfVar33[7]) +
                 ALBW_AS_F32(pppppppppppfVar38[2]) * ALBW_AS_F32(pppppppppppfVar33[8]));
        func_00115C70((float *)(local_1f28 + 2),(float *)&local_1f14,(float *)local_1e64);
        fVar5 = (-(ALBW_AS_F32(pppppppppppfVar33[6]) * ALBW_AS_F32(local_1f28[2]) +
                  ALBW_AS_F32(pppppppppppfVar33[7]) * ALBW_AS_F32(local_1f28[3])) +
                -(ALBW_AS_F32(pppppppppppfVar33[8]) * ALBW_AS_F32(local_1f28[4]))) / fVar5;
        pppppppppppfVar15 = extraout_r1_09;
        if (iVar21 < (int)ALBW_AS_U32(fVar5)) {
          func_00115D70((float *)&local_1e98,(float *)pppppppppppfVar38,fVar5);
          local_1eb8 = local_1e98;
          local_1eb4 = (float ************)local_1e94;
          local_1eb0 = local_1e90;
          pppppppppppfVar78 = local_1e90;
          func_00115C70((float *)&local_1e98,(float *)&local_1f14,(float *)&local_1eb8);
          pppppppppppfVar38[0xf] = (float **********)local_1e98;
          pppppppppppfVar38[0x10] = (float **********)local_1e94;
          pppppppppppfVar38[0x11] = (float **********)local_1e90;
          pppppppppppfVar15 = local_1e98;
          if (*(char *)((int)pppppppppppfVar38 + 0x93) == '\x01') {
            func_00115D70((float *)&local_1e98,(float *)(pppppppppppfVar38 + 3),fVar58);
            local_1f28[2] = local_1e98;
            local_1f28[3] = local_1e94;
            local_1f28[4] = local_1e90;
            func_00115D70((float *)&local_1e98,(float *)(local_1f28 + 2),fVar55);
            local_1eb8 = local_1e98;
            local_1eb4 = (float ************)local_1e94;
            local_1eb0 = local_1e90;
            pppppppppppfVar78 = local_1e90;
            func_00115C54((float *)(pppppppppppfVar38 + 0xf),(float *)(pppppppppppfVar38 + 0xf),
                          (float *)&local_1eb8);
            pppppppppppfVar15 = extraout_r1_10;
          }
          if (*(char *)((int)pppppppppppfVar38 + 0x93) == '\0') {
            func_00115C70((float *)&local_1eb8,(float *)(pppppppppppfVar38 + 9),
                          (float *)(pppppppppppfVar38 + 0xf));
            pppppppppppfVar38[0x1b] =
                 (float **********)
                 ALBW_PTR_BITS((ALBW_AS_F32(*pppppppppppfVar38) * ALBW_AS_F32(local_1eb8) +
                  ALBW_AS_F32(pppppppppppfVar38[1]) * ALBW_AS_F32(local_1eb4) +
                 ALBW_AS_F32(pppppppppppfVar38[2]) * ALBW_AS_F32(local_1eb0)));
            pppppppppppfVar15 = extraout_r1_11;
          }
          pppppppppfVar48 = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar48) + fVar5));
          *(undefined1 *)(pppppppppppfVar38 + 0x25) = 1;
        }
        local_1e54 = (float ***********)((int)local_1e54 + 1);
        iVar21 = DAT_001fa164;
        pppppppppppfVar33 = pppppppppppfVar29;
      }
    }
    if (ALBW_AS_F32(pppppppppfVar23) < ALBW_AS_F32(pppppppppfVar48)) {
      pppppppppppfVar33 = (float ***********)((int)pppppppppppfVar9 + -1);
      pppppppppppfVar75 = local_1e34;
      if (local_1e34 == pppppppppppfVar33) {
        pppppppppppfVar75 = local_1e54;
      }
      if (local_1e34 == pppppppppppfVar33 && pppppppppppfVar75 == (float ***********)0x0) {
        local_1e54 = (float ***********)0x1;
        local_1e34 = (float ***********)((int)local_1e34 + -1);
      }
      pppppppppfVar24 = pppppppppfVar23;
      pppppppppfVar64 = pppppppppfVar23;
      if (0 < (int)local_1e34) {
        if (0 < (int)pppppppppppfVar9) {
          pppppppppppfVar15 = (float ***********)pppppppppppfVar34[0x2b];
          pppppppppppfVar78 = (float ***********)*pppppppppppfVar17;
        }
        pppppppppppfVar75 = (float ***********)0x0;
        if (0 < (int)pppppppppppfVar9) {
          do {
            pppppppppppfVar29 = pppppppppppfVar15;
            if (pppppppppppfVar75 < pppppppppppfVar78) {
              pppppppppppfVar29 = pppppppppppfVar15 + (int)pppppppppppfVar75 * 0x27;
            }
            if ((0x3effffff < (int)pppppppppppfVar29[0x1d]) &&
               (ppppppppppfVar22 = pppppppppppfVar29[0x1c], (int)ppppppppppfVar22 < 0x3f000001)) {
              if (pppppppppppfVar75 == (float ***********)0x0) {
                if ((float ***********)0x1 < pppppppppppfVar78) {
                  pppppppppppfVar15 = pppppppppppfVar15 + 0x27;
                }
                pppppppppfVar24 =
                     (float *********)
                     ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar15[0x1d]) - ALBW_AS_F32(pppppppppppfVar15[0x1c])) +
                     ALBW_AS_F32(pppppppppfVar23)));
                pppppppppfVar64 =
                     (float *********)
                     ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar29[0x1d]) - ALBW_AS_F32(ppppppppppfVar22)) +
                     ALBW_AS_F32(pppppppppfVar23)));
              }
              else {
                pppppppppppfVar28 = (float ***********)((int)pppppppppppfVar75 + -1);
                pppppppppppfVar38 = pppppppppppfVar15;
                if (pppppppppppfVar28 < pppppppppppfVar78) {
                  pppppppppppfVar38 = pppppppppppfVar15 + (int)pppppppppppfVar28 * 0x27;
                }
                if (pppppppppppfVar28 < pppppppppppfVar78) {
                  pppppppppppfVar15 = pppppppppppfVar15 + (int)((int)pppppppppppfVar75 + -1) * 0x27;
                }
                pppppppppfVar24 =
                     (float *********)
                     ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar29[0x1d]) - ALBW_AS_F32(ppppppppppfVar22)) +
                     ALBW_AS_F32(pppppppppfVar23)));
                pppppppppfVar64 =
                     (float *********)
                     ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar38[0x1d]) - ALBW_AS_F32(pppppppppppfVar15[0x1c])) +
                     ALBW_AS_F32(pppppppppfVar23)));
              }
              break;
            }
            pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar75 + 1);
          } while ((int)pppppppppppfVar75 < (int)pppppppppppfVar9);
        }
      }
      if ((0 < (int)local_1e54) && (-1 < (int)pppppppppppfVar33)) {
        ppppppppppfVar22 = pppppppppppfVar34[0x2b];
        pppppppppppfVar78 = (float ***********)*pppppppppppfVar17;
        pppppppppppfVar15 = pppppppppppfVar33;
        do {
          ppppppppppfVar56 = ppppppppppfVar22;
          if (pppppppppppfVar15 < pppppppppppfVar78) {
            ppppppppppfVar56 = ppppppppppfVar22 + (int)pppppppppppfVar15 * 0x27;
          }
          if ((0x3effffff < (int)ppppppppppfVar56[0x1d]) &&
             ((int)ppppppppppfVar56[0x1c] < 0x3f000001)) {
            if (pppppppppppfVar15 == pppppppppppfVar33) {
              pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar15 + -1);
              ppppppppppfVar73 = ppppppppppfVar22;
              if (pppppppppppfVar75 < pppppppppppfVar78) {
                ppppppppppfVar73 = ppppppppppfVar22 + (int)pppppppppppfVar75 * 0x27;
              }
              pppppppppfVar24 =
                   (float *********)
                   ALBW_PTR_BITS(((ALBW_AS_F32(ppppppppppfVar56[0x1d]) - ALBW_AS_F32(ppppppppppfVar56[0x1c])) +
                   ALBW_AS_F32(pppppppppfVar24)));
              if (pppppppppppfVar75 < pppppppppppfVar78) {
                ppppppppppfVar22 = ppppppppppfVar22 + (int)((int)pppppppppppfVar15 + -1) * 0x27;
              }
              pppppppppfVar64 =
                   (float *********)
                   ALBW_PTR_BITS(((ALBW_AS_F32(ppppppppppfVar73[0x1d]) - ALBW_AS_F32(ppppppppppfVar22[0x1c])) +
                   ALBW_AS_F32(pppppppppfVar64)));
            }
            else {
              pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar15 + 1);
              ppppppppppfVar73 = ppppppppppfVar22;
              if (pppppppppppfVar75 < pppppppppppfVar78) {
                ppppppppppfVar73 = ppppppppppfVar22 + (int)pppppppppppfVar75 * 0x27;
              }
              pppppppppfVar64 =
                   (float *********)
                   ALBW_PTR_BITS(((ALBW_AS_F32(ppppppppppfVar56[0x1d]) - ALBW_AS_F32(ppppppppppfVar56[0x1c])) +
                   ALBW_AS_F32(pppppppppfVar64)));
              if (pppppppppppfVar75 < pppppppppppfVar78) {
                ppppppppppfVar22 = ppppppppppfVar22 + (int)((int)pppppppppppfVar15 + 1) * 0x27;
              }
              pppppppppfVar24 =
                   (float *********)
                   ALBW_PTR_BITS(((ALBW_AS_F32(ppppppppppfVar73[0x1d]) - ALBW_AS_F32(ppppppppppfVar22[0x1c])) +
                   ALBW_AS_F32(pppppppppfVar24)));
            }
            break;
          }
          pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar15 + -1);
        } while (-1 < (int)pppppppppppfVar15);
      }
      pppppppppfVar66 = pppppppppfVar23;
      pppppppppfVar76 = pppppppppfVar23;
      if (ALBW_AS_F32(pppppppppfVar23) < ALBW_AS_F32(pppppppppfVar64) + ALBW_AS_F32(pppppppppfVar24)) {
        pppppppppfVar66 =
             (float *********)
             ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar48) * ALBW_AS_F32(pppppppppfVar24) *
             (ALBW_AS_F32(pppppppppppfVar74) / (ALBW_AS_F32(pppppppppfVar64) + ALBW_AS_F32(pppppppppfVar24)))));
        pppppppppfVar76 = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar48) - ALBW_AS_F32(pppppppppfVar66)));
      }
      pppppppppppfVar15 = (float ***********)0x0;
      for (; ((int)pppppppppppfVar15 < (int)pppppppppppfVar9 &&
             (ALBW_AS_F32(pppppppppfVar23) < ALBW_AS_F32(pppppppppfVar66)));
          pppppppppfVar66 = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar66) - fVar5))) {
        if (pppppppppppfVar15 < *pppppppppppfVar17) {
          ppppppppppfVar22 = pppppppppppfVar34[0x2b] + (int)pppppppppppfVar15 * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar34[0x2b];
        }
        func_00115C70((float *)&local_1f14,(float *)(ppppppppppfVar22 + 0x12),
                      (float *)(ppppppppppfVar22 + 0xf));
        fVar5 = ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1f14) +
                ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1f10) +
                ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1f0c);
        if (ALBW_AS_F32(pppppppppfVar66) < fVar5) {
          bVar44 = false;
          pppppppppfVar48 =
               (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar66) - ALBW_AS_F32(ppppppppppfVar22[0x1b])));
          if (ALBW_AS_F32(pppppppppfVar66) - ALBW_AS_F32(ppppppppppfVar22[0x1b]) < ALBW_AS_F32(pppppppppfVar23)) {
            pppppppppfVar48 = pppppppppfVar23;
          }
          if (ALBW_AS_F32(pppppppppfVar48) == ALBW_AS_F32(pppppppppfVar23)) {
            local_1ea4 = (float ***********)ppppppppppfVar22[9];
            local_1ea0 = (float ***********)ppppppppppfVar22[10];
            local_1e9c = (float ***********)ppppppppppfVar22[0xb];
LAB_001f9a68:
            if (pppppppppppfVar15 != (float ***********)0x0) goto LAB_001f9a8c;
          }
          else {
            if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
              func_00115D70((float *)&local_1f0c,(float *)ppppppppppfVar22,ALBW_AS_F32(pppppppppfVar48));
              local_1f28[4] = local_1f0c;
              local_1f14 = local_1f08;
              local_1f10 = local_1f04;
              func_00115C54((float *)&local_1f0c,(float *)(ppppppppppfVar22 + 0xf),
                            (float *)(local_1f28 + 4));
              local_1ea4 = local_1f0c;
              local_1ea0 = local_1f08;
              local_1e9c = local_1f04;
              goto LAB_001f9a68;
            }
            func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar22,ALBW_AS_F32(pppppppppfVar48));
            local_1e8c = local_1ec8;
            local_1e88 = (float ***********)local_1ec4;
            local_1e84 = (float ***********)local_1ec0;
            func_00115C54((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0xf),
                          (float *)&local_1e8c);
            local_1e98 = local_1ec8;
            local_1e94 = (float ***********)local_1ec4;
            local_1e90 = (float ***********)local_1ec0;
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 3),
                          ALBW_AS_F32(pppppppppppfVar49[0x1c]));
            local_1f14 = local_1ec8;
            local_1f10 = (float ***********)local_1ec4;
            local_1f0c = (float ***********)local_1ec0;
            func_00115D88((float *)&local_1ec8,(float *)&local_1f14,ALBW_AS_F32(ppppppppppfVar22[4]));
            local_1e80 = local_1ec8;
            local_1e7c = (float ***********)local_1ec4;
            local_1e78 = (float ***********)local_1ec0;
            func_00115C54((float *)&local_1ec8,(float *)&local_1e98,(float *)&local_1e80);
            local_1ea4 = local_1ec8;
            local_1ea0 = (float ***********)local_1ec4;
            local_1e9c = (float ***********)local_1ec0;
            if (pppppppppppfVar15 != (float ***********)0x0) {
              if (*(char *)(ppppppppppfVar22 + 0x25) != '\0') {
                func_00115D70((float *)&local_1e74,(float *)ppppppppppfVar22,ALBW_AS_F32(pppppppppfVar48))
                ;
                local_1ec8 = local_1e74;
                local_1ec4 = (float ************)local_1e70;
                local_1ec0 = (float ************)local_1e6c;
                func_00115C54((float *)(ppppppppppfVar22 + 0xf),(float *)(ppppppppppfVar22 + 0xf),
                              (float *)&local_1ec8);
                func_00115C70((float *)&local_1e74,(float *)&local_1ea4,
                              (float *)(ppppppppppfVar22 + 0xf));
                local_1ec8 = local_1e74;
                local_1ec4 = (float ************)local_1e70;
                local_1ec0 = (float ************)local_1e6c;
                func_0068906C((float *)&local_1eb8,(float *)&local_1ec8);
                bVar44 = true;
              }
              goto LAB_001f9a8c;
            }
          }
          if (*(char *)(ppppppppppfVar22 + 0x26) == '\0') {
LAB_001f9a8c:
            pppppppppppfVar78 = (float ***********)0x0;
            local_1ec8 = pppppppppppfVar15;
            goto LAB_001f9d08;
          }
          pppppppppppfVar78 = (float ***********)0x0;
          local_1ec8 = (float ***********)0x1;
          goto LAB_001f9d14;
        }
        pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar15 + 1);
      }
LAB_001f9d1c:
      while( true ) {
        pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar9 + -1);
        bVar44 = pppppppppppfVar15 == (float ***********)0x0;
        if ((int)pppppppppppfVar15 >= 0) {
          bVar44 = ALBW_AS_F32(pppppppppfVar76) == ALBW_AS_F32(pppppppppfVar23);
        }
        if (bVar44 || ((int)pppppppppppfVar15 < 0 || ALBW_AS_F32(pppppppppfVar76) < ALBW_AS_F32(pppppppppfVar23)
                      )) goto LAB_001fa1f0;
        if (pppppppppppfVar15 < *pppppppppppfVar17) {
          ppppppppppfVar22 = pppppppppppfVar34[0x2b] + (int)pppppppppppfVar15 * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar34[0x2b];
        }
        func_00115C70((float *)(local_1f28 + 2),(float *)(ppppppppppfVar22 + 0x12),
                      (float *)(ppppppppppfVar22 + 0xf));
        fVar5 = ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1f28[2]) +
                ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1f28[3]) +
                ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1f28[4]);
        if (ALBW_AS_F32(pppppppppfVar76) < fVar5) break;
        pppppppppfVar76 = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar76) - fVar5));
        pppppppppppfVar9 = pppppppppppfVar15;
      }
      bVar44 = false;
      pppppppppfVar48 = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppfVar76) - ALBW_AS_F32(ppppppppppfVar22[0x1b])));
      if (ALBW_AS_F32(pppppppppfVar76) - ALBW_AS_F32(ppppppppppfVar22[0x1b]) < ALBW_AS_F32(pppppppppfVar23)) {
        pppppppppfVar48 = pppppppppfVar23;
      }
      if (ALBW_AS_F32(pppppppppfVar48) == ALBW_AS_F32(pppppppppfVar23)) {
        local_1ea4 = (float ***********)ppppppppppfVar22[0xc];
        local_1ea0 = (float ***********)ppppppppppfVar22[0xd];
        local_1e9c = (float ***********)ppppppppppfVar22[0xe];
      }
      else if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
        func_00115D70((float *)(local_1f28 + 1),(float *)ppppppppppfVar22,ALBW_AS_F32(pppppppppfVar48));
        local_1ec8 = local_1f28[1];
        local_1ec4 = (float ************)local_1f28[2];
        local_1ec0 = (float ************)local_1f28[3];
        func_00115C70((float *)&stack0xffffe0d4,(float *)(ppppppppppfVar22 + 0x12),
                      (float *)&local_1ec8);
        local_1ea0 = local_1f28[0];
        local_1e9c = local_1f28[1];
        local_1ea4 = in_stack_ffffe0d4;
      }
      else {
        func_00115D70((float *)(local_1f28 + 2),(float *)ppppppppppfVar22,ALBW_AS_F32(pppppppppfVar48));
        local_1e98 = local_1f28[2];
        local_1e94 = local_1f28[3];
        local_1e90 = local_1f28[4];
        func_00115C70((float *)(local_1f28 + 1),(float *)(ppppppppppfVar22 + 0x12),
                      (float *)&local_1e98);
        local_1ec8 = local_1f28[1];
        local_1ec4 = (float ************)local_1f28[2];
        local_1ec0 = (float ************)local_1f28[3];
        func_00115D70((float *)&local_1e74,(float *)(ppppppppppfVar22 + 3),
                      ALBW_AS_F32(pppppppppppfVar49[0x1c]));
        local_1e80 = local_1e74;
        local_1e7c = local_1e70;
        local_1e78 = local_1e6c;
        func_00115D88((float *)(local_1f28 + 1),(float *)&local_1e80,ALBW_AS_F32(ppppppppppfVar22[4]));
        local_1e8c = local_1f28[1];
        local_1e88 = local_1f28[2];
        local_1e84 = local_1f28[3];
        func_00115C54((float *)(local_1f28 + 1),(float *)&local_1ec8,(float *)&local_1e8c);
        ppppppppppppfVar85 = local_1f28 + 1;
        local_1ea4 = local_1f28[1];
        local_1ea0 = local_1f28[2];
        local_1e9c = local_1f28[3];
        if (pppppppppppfVar15 != pppppppppppfVar33) {
          ppppppppppppfVar85 = (float ************)(uint)*(byte *)((int)ppppppppppfVar22 + 0x95);
        }
        if (pppppppppppfVar15 != pppppppppppfVar33 && ppppppppppppfVar85 != (float ************)0x0)
        {
          func_00115D70((float *)(local_1f28 + 1),(float *)ppppppppppfVar22,ALBW_AS_F32(pppppppppfVar48));
          local_1e74 = local_1f28[1];
          local_1e70 = local_1f28[2];
          local_1e6c = local_1f28[3];
          func_00115C70((float *)(ppppppppppfVar22 + 0x12),(float *)(ppppppppppfVar22 + 0x12),
                        (float *)&local_1e74);
          func_00115C70((float *)&stack0xffffe0d4,(float *)&local_1ea4,
                        (float *)(ppppppppppfVar22 + 0x12));
          local_1e70 = local_1f28[0];
          local_1e6c = local_1f28[1];
          local_1e74 = in_stack_ffffe0d4;
          func_0068906C((float *)&local_1eb8,(float *)&local_1e74);
          bVar44 = true;
        }
      }
      pppppppppppfVar78 = pppppppppppfVar15;
      if ((pppppppppppfVar15 == pppppppppppfVar33) && (*(char *)(ppppppppppfVar22 + 0x26) != '\0'))
      {
        pppppppppppfVar78 = (float ***********)((int)pppppppppppfVar9 + -2);
      }
      pppppppppppfVar9 = (float ***********)*pppppppppppfVar17;
      while( true ) {
        pppppppppppfVar9 = (float ***********)((int)pppppppppppfVar9 + -1);
        pppppppppppfVar75 = (float ***********)((int)pppppppppppfVar9 - (int)pppppppppppfVar78);
        if ((int)pppppppppppfVar9 >= (int)pppppppppppfVar78) {
          pppppppppppfVar75 = pppppppppppfVar9;
        }
        if ((int)pppppppppppfVar75 < 0 !=
            ((int)pppppppppppfVar9 < (int)pppppppppppfVar78 &&
            SBORROW4((int)pppppppppppfVar9,(int)pppppppppppfVar78))) break;
        if (pppppppppppfVar9 < *pppppppppppfVar17) {
          ppppppppppfVar22 = pppppppppppfVar34[0x2b] + (int)pppppppppppfVar9 * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar34[0x2b];
        }
        *(undefined1 *)((int)ppppppppppfVar22 + 0x92) = 1;
        if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
          func_00115C70((float *)&stack0xffffe0d4,(float *)&local_1ea4,
                        (float *)(ppppppppppfVar22 + 0x12));
          func_00115D70((float *)(local_1f28 + 1),(float *)(ppppppppppfVar22 + 6),
                        ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(in_stack_ffffe0d4) +
                        ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[0]) +
                        ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f28[1]));
          local_1e80 = local_1f28[1];
          local_1e7c = local_1f28[2];
          local_1e78 = local_1f28[3];
          func_00115C70((float *)local_1f28,(float *)&local_1ea4,(float *)&local_1e80);
          ppppppppppfVar22[0x12] = (float *********)local_1f28[0];
          ppppppppppfVar22[0x13] = (float *********)local_1f28[1];
          ppppppppppfVar22[0x14] = (float *********)local_1f28[2];
        }
        else if (pppppppppppfVar9 != pppppppppppfVar15 && bVar44) {
          fVar55 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1eb8) +
                   ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1eb4) +
                   ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1eb0);
          fVar5 = fVar55;
          if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
            fVar5 = -fVar55;
          }
          if ((int)ALBW_AS_U32(fVar5) <= DAT_001fa16c) goto LAB_001fa0f0;
          func_00115C70((float *)&local_1e40,(float *)&local_1ea4,(float *)(ppppppppppfVar22 + 0xc))
          ;
          local_1e98 = local_1e40;
          local_1e94 = local_1e3c;
          local_1e90 = local_1e38;
          func_00115D70((float *)&local_1e24,(float *)&local_1eb8,
                        ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1e40) +
                        ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1e3c) +
                        ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1e38));
          local_1f28[1] = local_1e24;
          local_1f28[2] = local_1e20;
          local_1f28[3] = local_1e1c;
          func_00115D88((float *)&local_1e18,(float *)(local_1f28 + 1),fVar55);
          local_1e98 = local_1e18;
          local_1e94 = (float ***********)local_1e14;
          local_1e90 = (float ***********)local_1e10;
          func_00115C70((float *)&local_1df4,(float *)&local_1ea4,(float *)&local_1e98);
          local_1ec8 = local_1df4;
          local_1ec4 = (float ************)local_1df0;
          local_1ec0 = (float ************)local_1dec;
          func_00115C70((float *)&local_1e74,(float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x12)
                       );
          local_1f28[1] = local_1e74;
          local_1f28[2] = local_1e70;
          local_1f28[3] = local_1e6c;
          if (ALBW_AS_F32(local_1e74) * ALBW_AS_F32(ppppppppppfVar22[3]) +
              ALBW_AS_F32(local_1e70) * ALBW_AS_F32(ppppppppppfVar22[4]) +
              ALBW_AS_F32(local_1e6c) * ALBW_AS_F32(ppppppppppfVar22[5]) <= ALBW_AS_F32(pppppppppfVar23))
          goto LAB_001fa0f0;
          ppppppppppfVar22[0x18] = (float *********)local_1ec8;
          ppppppppppfVar22[0x19] = (float *********)local_1ec4;
          ppppppppppfVar22[0x1a] = (float *********)local_1ec0;
        }
        else {
LAB_001fa0f0:
          func_00115C70((float *)&local_1e0c,(float *)&local_1ea4,(float *)(ppppppppppfVar22 + 0x12)
                       );
          func_00115D70((float *)&local_1e68,(float *)(ppppppppppfVar22 + 6),
                        ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1e0c) +
                        ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1e08) +
                        ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1e04));
          local_1e8c = local_1e68;
          local_1e88 = local_1e64;
          local_1e84 = local_1e60;
          func_00115C70((float *)&local_1e4c,(float *)&local_1ea4,(float *)&local_1e8c);
          ppppppppppfVar22[0x18] = (float *********)local_1e4c;
          ppppppppppfVar22[0x19] = (float *********)local_1e48;
          ppppppppppfVar22[0x1a] = (float *********)local_1e44;
        }
      }
    }
  }
LAB_001fa1f0:
  pppppppppppfVar17 = pppppppppppfVar49;
  if (((uint)local_1de8 & 2) != 0) {
    ppppppppppfVar56 = (float **********)0x1;
    pppppppppppfVar15 = pppppppppppfVar49 + 0x29;
    local_1ea0 = (float ***********)*pppppppppppfVar15;
    local_1e54 = (float ***********)0x0;
    local_1eb0 = (float ***********)0x0;
    ppppppppppfVar22 = (float **********)0x0;
    if (0 < (int)local_1ea0) {
      do {
        ppppppppppfVar73 = pppppppppppfVar49[0x2b];
        if (ppppppppppfVar22 < *pppppppppppfVar15) {
          ppppppppppfVar73 = ppppppppppfVar73 + (int)ppppppppppfVar22 * 0x27;
        }
        ppppppppppfVar30 = pppppppppppfVar49[0x2b];
        if (ppppppppppfVar22 < *pppppppppppfVar15) {
          ppppppppppfVar30 = ppppppppppfVar30 + (int)ppppppppppfVar22 * 0x27;
        }
        ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
        *(undefined1 *)(ppppppppppfVar30 + 0x25) = 0;
        *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 0;
      } while ((int)ppppppppppfVar22 < (int)local_1ea0);
    }
    pppppppppppfVar34 = pppppppppppfVar74;
    if (1 < (int)local_1ea0) {
      do {
        if ((float **********)((int)ppppppppppfVar56 + -1) < *pppppppppppfVar15) {
          ppppppppppfVar22 = pppppppppppfVar49[0x2b] + ((int)ppppppppppfVar56 + -1) * 0x27;
        }
        else {
          ppppppppppfVar22 = pppppppppppfVar49[0x2b];
        }
        if (ppppppppppfVar56 < *pppppppppppfVar15) {
          ppppppppppfVar73 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar56 * 0x27;
        }
        else {
          ppppppppppfVar73 = pppppppppppfVar49[0x2b];
        }
        if ((((int)ppppppppppfVar22[7] <= DAT_001fa164) &&
            ((int)ppppppppppfVar73[7] <= DAT_001fa164)) ||
           ((int)ALBW_AS_U32((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar73[6]) +
                  ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar73[7]) +
                 ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar73[8]))) <= DAT_001fb1f4)) break;
        fVar5 = ALBW_AS_F32(pppppppppppfVar17[0x1c]) / ALBW_AS_F32(ppppppppppfVar22[4]);
        if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar22 + 0xc);
        }
        else {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar22 + 0x15);
        }
        uVar67 = CONCAT44(fVar5,ALBW_AS_F32(pppppppppppfVar17[0x1c]) / ALBW_AS_F32(ppppppppppfVar73[4]));
        local_1e5c = pppppppppppfVar78;
        func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 3),fVar5);
        local_1ec8 = local_1ebc;
        local_1ec4 = (float ************)local_1eb8;
        local_1ec0 = local_1eb4;
        func_00115C70((float *)&local_1ebc,(float *)pppppppppppfVar78,(float *)&local_1ec8);
        local_1f28[1] = local_1ebc;
        local_1f28[2] = local_1eb8;
        local_1f28[3] = (float ***********)local_1eb4;
        local_1e80 = local_1ebc;
        local_1e7c = local_1eb8;
        local_1e78 = (float ***********)local_1eb4;
        if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar73 + 9);
        }
        else {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar73 + 0x15);
        }
        local_1f08 = pppppppppppfVar78;
        func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 3),ALBW_AS_F32(uVar67));
        local_1f28[1] = local_1ebc;
        local_1f28[2] = local_1eb8;
        local_1f28[3] = (float ***********)local_1eb4;
        func_00115C70((float *)&local_1ebc,(float *)pppppppppppfVar78,(float *)(local_1f28 + 1));
        local_1ec8 = local_1ebc;
        local_1ec4 = (float ************)local_1eb8;
        local_1ec0 = local_1eb4;
        local_1e74 = local_1ebc;
        local_1e70 = local_1eb8;
        local_1e6c = (float ***********)local_1eb4;
        fVar5 = ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(ppppppppppfVar73[6]) +
                ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(ppppppppppfVar73[7]) +
                ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(ppppppppppfVar73[8]);
        func_00115C70((float *)(local_1f28 + 1),(float *)&local_1e80,(float *)local_1f08);
        pppppppppfVar48 =
             (float *********)
             ALBW_PTR_BITS(((-(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[1]) +
                ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[2])) +
              -(ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[3]))) / fVar5));
        lVar69 = ZEXT48(pppppppppfVar48) << 0x20;
        if (DAT_001fa164 < (int)pppppppppfVar48) {
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
            ppppppppppfVar10 = *pppppppppppfVar15;
            bVar47 = ppppppppppfVar56 <= ppppppppppfVar10;
            bVar44 = ppppppppppfVar10 == ppppppppppfVar56;
            ppppppppppfVar30 = ppppppppppfVar56;
            pppppppppfVar24 = pppppppppfVar23;
            if ((int)ppppppppppfVar56 < (int)ppppppppppfVar10) {
              do {
                if (!bVar47 || bVar44) {
                  ppppppppppfVar10 = pppppppppppfVar49[0x2b];
                }
                else {
                  ppppppppppfVar10 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar30 * 0x27;
                }
                func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar10 + 0xc),
                              (float *)(ppppppppppfVar10 + 9));
                pppppppppfVar48 = (float *********)((ulonglong)lVar69 >> 0x20);
                ppppppppppfVar30 = (float **********)((int)ppppppppppfVar30 + 1);
                ppppppppppfVar11 = *pppppppppppfVar15;
                fVar5 = ALBW_AS_F32(*ppppppppppfVar10) * ALBW_AS_F32(local_1ec8) +
                        ALBW_AS_F32(ppppppppppfVar10[1]) * ALBW_AS_F32(local_1ec4) +
                        ALBW_AS_F32(ppppppppppfVar10[2]) * ALBW_AS_F32(local_1ec0);
                if (fVar5 <= ALBW_AS_F32(pppppppppfVar23)) {
                  fVar5 = -fVar5;
                }
                bVar47 = ppppppppppfVar30 <= ppppppppppfVar11;
                bVar44 = ppppppppppfVar11 == ppppppppppfVar30;
                pppppppppfVar24 = (float *********)ALBW_PTR_BITS((fVar5 + ALBW_AS_F32(pppppppppfVar24)));
              } while (!bVar44 && (int)ppppppppppfVar30 <= (int)ppppppppppfVar11);
            }
            if (ALBW_AS_F32(pppppppppfVar24) < ALBW_AS_F32(pppppppppfVar48)) {
              pppppppppfVar48 = pppppppppfVar24;
            }
            if (*(char *)((int)ppppppppppfVar22 + 0x93) != '\0') goto LAB_001fa504;
            func_00115D70((float *)&local_1e4c,(float *)ppppppppppfVar22,ALBW_AS_F32(pppppppppfVar48));
            local_1ebc = local_1e4c;
            local_1eb8 = local_1e48;
            local_1eb4 = (float ************)local_1e44;
            func_00115C54((float *)&local_1e18,(float *)&local_1e80,(float *)&local_1ebc);
            ppppppppppfVar22[0x12] = (float *********)local_1e18;
            ppppppppppfVar22[0x13] = (float *********)local_1e14;
            ppppppppppfVar22[0x14] = (float *********)local_1e10;
          }
          else {
LAB_001fa504:
            func_00115D70((float *)&local_1e24,(float *)ppppppppppfVar22,ALBW_AS_F32(pppppppppfVar48));
            local_1e0c = local_1e24;
            local_1e08 = local_1e20;
            local_1e04 = local_1e1c;
            func_00115C54((float *)(ppppppppppfVar22 + 0x18),(float *)(ppppppppppfVar22 + 0x18),
                          (float *)&local_1e0c);
          }
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
            func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 3),
                          ALBW_AS_F32(pppppppppppfVar34));
            local_1f28[1] = local_1ebc;
            local_1f28[2] = local_1eb8;
            local_1f28[3] = (float ***********)local_1eb4;
            func_00115D70((float *)&local_1ebc,(float *)(local_1f28 + 1),
                          ALBW_AS_F32(((ulonglong)uVar67 >> 0x20)));
            local_1ec8 = local_1ebc;
            local_1ec4 = (float ************)local_1eb8;
            local_1ec0 = local_1eb4;
            func_00115C54((float *)(ppppppppppfVar22 + 0x12),(float *)(ppppppppppfVar22 + 0x12),
                          (float *)&local_1ec8);
          }
          *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
              local_1f00[1] = (float ***********)(ppppppppppfVar22 + 0x18);
              func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 0x18),
                            (float *)(ppppppppppfVar22 + 0x12));
              func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 6),
                            ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1ebc) +
                            ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1eb8) +
                            ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1eb4));
              local_1ec8 = local_1ebc;
              local_1ec4 = (float ************)local_1eb8;
              local_1ec0 = local_1eb4;
              func_00115C70((float *)&local_1ebc,(float *)local_1f00[1],(float *)&local_1ec8);
              local_1eac = (float ************)local_1ebc;
              local_1ea8 = local_1eb8;
              local_1ea4 = (float ***********)local_1eb4;
            }
            else {
              local_1eac = (float ************)ppppppppppfVar22[0x12];
              local_1ea8 = (float ***********)ppppppppppfVar22[0x13];
              local_1ea4 = (float ***********)ppppppppppfVar22[0x14];
            }
            if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
              func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 0x12),
                            (float *)&local_1eac);
              local_1ec8 = local_1ebc;
              local_1ec4 = (float ************)local_1eb8;
              local_1ec0 = local_1eb4;
              func_0068906C((float *)(local_1f28 + 1),(float *)&local_1ec8);
            }
            else {
              func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 0x18),
                            (float *)&local_1eac);
              local_1ec8 = local_1ebc;
              local_1ec4 = (float ************)local_1eb8;
              local_1ec0 = local_1eb4;
              func_0068906C((float *)(local_1f28 + 1),(float *)&local_1ec8);
            }
            fVar55 = ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[1]) +
                     ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[2]) +
                     ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[3]);
            fVar5 = fVar55;
            if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
              fVar5 = -fVar55;
            }
            if (DAT_001fa16c < (int)ALBW_AS_U32(fVar5)) {
              func_00115C70((float *)&local_1ec8,(float *)&local_1eac,
                            (float *)(ppppppppppfVar73 + 9));
              fVar5 = -(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ec8) +
                        ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ec4) +
                       ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ec0));
              if (fVar5 <= ALBW_AS_F32(pppppppppfVar23)) {
                if ((uint)ALBW_AS_U32(DAT_001fb1f8) < (uint)ALBW_AS_U32(fVar5)) {
                  func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0xc),
                                (float *)(ppppppppppfVar22 + 9));
                  func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar73,
                                ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1ec8) +
                                ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1ec4) +
                                ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1ec0));
                  local_1e4c = local_1ec8;
                  local_1e48 = (float ***********)local_1ec4;
                  local_1e44 = (float ***********)local_1ec0;
                  func_00115C70((float *)&local_1ec8,(float *)&local_1e74,(float *)&local_1e4c);
                  ppppppppppfVar73[0xf] = (float *********)local_1ec8;
                  ppppppppppfVar73[0x10] = (float *********)local_1ec4;
                  ppppppppppfVar73[0x11] = (float *********)local_1ec0;
                  func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 3),
                                ALBW_AS_F32(pppppppppppfVar34));
                  local_1e90 = local_1ec8;
                  local_1e8c = (float ***********)local_1ec4;
                  local_1e88 = (float ***********)local_1ec0;
                  func_00115D70((float *)&local_1ec8,(float *)&local_1e90,ALBW_AS_F32(uVar67));
                  local_1e4c = local_1ec8;
                  local_1e48 = (float ***********)local_1ec4;
                  local_1e44 = (float ***********)local_1ec0;
                  func_00115C54((float *)(ppppppppppfVar73 + 0xf),(float *)(ppppppppppfVar73 + 0xf),
                                (float *)&local_1e4c);
                  *(undefined1 *)(ppppppppppfVar73 + 0x25) = 1;
                  if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
                    local_1f04 = (float ***********)(ppppppppppfVar73 + 0x15);
                    local_1f14 = (float ***********)(ppppppppppfVar73 + 6);
                    func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 0x15),
                                  (float *)(ppppppppppfVar73 + 0xf));
                    func_00115D70((float *)&local_1ec8,(float *)local_1f14,
                                  ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ec8) +
                                  ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ec4) +
                                  ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ec0));
                    local_1ebc = local_1ec8;
                    local_1eb8 = (float ***********)local_1ec4;
                    local_1eb4 = local_1ec0;
                    func_00115C70((float *)&local_1ec8,(float *)local_1f04,(float *)&local_1ebc);
                    local_1eac = (float ************)local_1ec8;
                    local_1ea8 = (float ***********)local_1ec4;
                    local_1ea4 = (float ***********)local_1ec0;
                  }
                  else {
                    local_1eac = (float ************)ppppppppppfVar73[0xf];
                    local_1ea8 = (float ***********)ppppppppppfVar73[0x10];
                    local_1ea4 = (float ***********)ppppppppppfVar73[0x11];
                  }
                  if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
                    func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 0xf),
                                  (float *)&local_1eac);
                    local_1ebc = local_1ec8;
                    local_1eb8 = (float ***********)local_1ec4;
                    local_1eb4 = local_1ec0;
                    func_0068906C((float *)(local_1f28 + 1),(float *)&local_1ebc);
                  }
                  else {
                    func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 0x15),
                                  (float *)&local_1eac);
                    local_1ebc = local_1ec8;
                    local_1eb8 = (float ***********)local_1ec4;
                    local_1eb4 = local_1ec0;
                    func_0068906C((float *)(local_1f28 + 1),(float *)&local_1ebc);
                  }
                  fVar55 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[1]) +
                           ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[2]) +
                           ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f28[3]);
                  fVar5 = fVar55;
                  if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
                    fVar5 = -fVar55;
                  }
                  if (DAT_001fa16c < (int)ALBW_AS_U32(fVar5)) {
                    func_00115C70((float *)&local_1ec8,(float *)&local_1eac,
                                  (float *)(ppppppppppfVar22 + 0xc));
                    fVar5 = -(ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1ec8) +
                              ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1ec4) +
                             ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1ec0));
                    if (ALBW_AS_F32(pppppppppfVar23) < fVar5) {
                      func_00115D70((float *)&local_1e24,(float *)(local_1f28 + 1),fVar5);
                      local_1ec8 = local_1e24;
                      local_1ec4 = (float ************)local_1e20;
                      local_1ec0 = (float ************)local_1e1c;
                      func_00115D88((float *)&local_1e24,(float *)&local_1ec8,fVar55);
                      local_1e40 = local_1e24;
                      local_1e3c = local_1e20;
                      local_1e38 = local_1e1c;
                      func_00115C54((float *)&local_1e24,(float *)&local_1eac,(float *)&local_1e40);
                      local_1ebc = local_1e24;
                      local_1eb8 = local_1e20;
                      local_1eb4 = (float ************)local_1e1c;
                      func_00115C70((float *)&local_1e24,(float *)(ppppppppppfVar22 + 0x12),
                                    (float *)&local_1ebc);
                      local_1ec8 = local_1e24;
                      local_1ec4 = (float ************)local_1e20;
                      local_1ec0 = (float ************)local_1e1c;
                      if (ALBW_AS_F32(pppppppppfVar23) <
                          ALBW_AS_F32(local_1e24) * ALBW_AS_F32(ppppppppppfVar22[3]) +
                          ALBW_AS_F32(local_1e20) * ALBW_AS_F32(ppppppppppfVar22[4]) +
                          ALBW_AS_F32(local_1e1c) * ALBW_AS_F32(ppppppppppfVar22[5])) {
                        ppppppppppfVar22[0x18] = (float *********)local_1ebc;
                        ppppppppppfVar22[0x19] = (float *********)local_1eb8;
                        ppppppppppfVar22[0x1a] = (float *********)local_1eb4;
                        *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
                        if ((*(char *)((int)ppppppppppfVar22 + 0x97) != '\0') &&
                           (*(char *)(ppppppppppfVar22 + 0x25) == '\0')) {
                          ppppppppppfVar30 = ppppppppppfVar22 + 0x15;
                          ppppppppppfVar22[0x15] = ppppppppppfVar22[0x18];
                          ppppppppppfVar22[0x16] = ppppppppppfVar22[0x19];
                          pppppppppfVar48 = ppppppppppfVar22[0x1a];
                          goto LAB_001fabc0;
                        }
                      }
                    }
                  }
                }
              }
              else {
                func_00115D70((float *)&local_1e4c,(float *)(local_1f28 + 1),fVar5);
                local_1ebc = local_1e4c;
                local_1eb8 = local_1e48;
                local_1eb4 = (float ************)local_1e44;
                func_00115D88((float *)&local_1e4c,(float *)&local_1ebc,fVar55);
                local_1e90 = local_1e4c;
                local_1e8c = local_1e48;
                local_1e88 = local_1e44;
                func_00115C54((float *)&local_1e4c,(float *)&local_1eac,(float *)&local_1e90);
                local_1ec8 = local_1e4c;
                local_1ec4 = (float ************)local_1e48;
                local_1ec0 = (float ************)local_1e44;
                func_00115C70((float *)&local_1e4c,(float *)(ppppppppppfVar73 + 0xf),
                              (float *)&local_1ec8);
                local_1ebc = local_1e4c;
                local_1eb8 = local_1e48;
                local_1eb4 = (float ************)local_1e44;
                if (ALBW_AS_F32(pppppppppfVar23) <
                    ALBW_AS_F32(local_1e4c) * ALBW_AS_F32(ppppppppppfVar73[3]) +
                    ALBW_AS_F32(local_1e48) * ALBW_AS_F32(ppppppppppfVar73[4]) +
                    ALBW_AS_F32(local_1e44) * ALBW_AS_F32(ppppppppppfVar73[5])) {
                  ppppppppppfVar73[0x15] = (float *********)local_1ec8;
                  ppppppppppfVar73[0x16] = (float *********)local_1ec4;
                  ppppppppppfVar73[0x17] = (float *********)local_1ec0;
                  *(undefined1 *)(ppppppppppfVar73 + 0x25) = 1;
                  if ((*(char *)((int)ppppppppppfVar73 + 0x97) != '\0') &&
                     (*(char *)((int)ppppppppppfVar73 + 0x95) == '\0')) {
                    ppppppppppfVar30 = ppppppppppfVar73 + 0x18;
                    ppppppppppfVar73[0x18] = ppppppppppfVar73[0x15];
                    ppppppppppfVar73[0x19] = ppppppppppfVar73[0x16];
                    pppppppppfVar48 = ppppppppppfVar73[0x17];
LAB_001fabc0:
                    ppppppppppfVar30[2] = pppppppppfVar48;
                  }
                }
              }
            }
          }
        }
        fVar5 = -(ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(ppppppppppfVar22[6]) +
                  ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(ppppppppppfVar22[7]) +
                 ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(ppppppppppfVar22[8]));
        func_00115C70((float *)(local_1f28 + 1),(float *)&local_1e74,(float *)local_1e5c);
        pppppppppfVar48 =
             (float *********)
             ALBW_PTR_BITS(((-(ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[1]) +
                ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[2])) +
              -(ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f28[3]))) / fVar5));
        lVar69 = ZEXT48(pppppppppfVar48) << 0x20;
        if (DAT_001fa164 < (int)pppppppppfVar48) {
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            ppppppppppfVar30 = (float **********)0x0;
            pppppppppfVar24 = pppppppppfVar23;
            if (0 < (int)ppppppppppfVar56) {
              do {
                if (ppppppppppfVar30 < *pppppppppppfVar15) {
                  ppppppppppfVar10 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar30 * 0x27;
                }
                else {
                  ppppppppppfVar10 = pppppppppppfVar49[0x2b];
                }
                func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar10 + 0xc),
                              (float *)(ppppppppppfVar10 + 9));
                pppppppppfVar48 = (float *********)((ulonglong)lVar69 >> 0x20);
                ppppppppppfVar30 = (float **********)((int)ppppppppppfVar30 + 1);
                fVar5 = ALBW_AS_F32(*ppppppppppfVar10) * ALBW_AS_F32(local_1ec8) +
                        ALBW_AS_F32(ppppppppppfVar10[1]) * ALBW_AS_F32(local_1ec4) +
                        ALBW_AS_F32(ppppppppppfVar10[2]) * ALBW_AS_F32(local_1ec0);
                if (fVar5 <= ALBW_AS_F32(pppppppppfVar23)) {
                  fVar5 = -fVar5;
                }
                pppppppppfVar24 = (float *********)ALBW_PTR_BITS((fVar5 + ALBW_AS_F32(pppppppppfVar24)));
              } while ((int)ppppppppppfVar30 < (int)ppppppppppfVar56);
            }
            if (ALBW_AS_F32(pppppppppfVar24) < ALBW_AS_F32(pppppppppfVar48)) {
              pppppppppfVar48 = pppppppppfVar24;
            }
            if (*(char *)((int)ppppppppppfVar73 + 0x93) != '\0') goto LAB_001facc4;
            func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar73,ALBW_AS_F32(pppppppppfVar48));
            local_1f0c = local_1ec8;
            local_1f08 = (float ***********)local_1ec4;
            local_1f04 = (float ***********)local_1ec0;
            func_00115C70((float *)&local_1ec8,(float *)&local_1e74,(float *)&local_1f0c);
            ppppppppppfVar73[0xf] = (float *********)local_1ec8;
            ppppppppppfVar73[0x10] = (float *********)local_1ec4;
            ppppppppppfVar73[0x11] = (float *********)local_1ec0;
          }
          else {
LAB_001facc4:
            local_1ec8 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(*ppppppppppfVar73));
            local_1ec4 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar73[1]));
            local_1ec0 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar73[2]));
            func_00115D70((float *)(local_1f28 + 1),(float *)&local_1ec8,ALBW_AS_F32(pppppppppfVar48));
            local_1f0c = local_1f28[1];
            local_1f08 = local_1f28[2];
            local_1f04 = local_1f28[3];
            func_00115C54((float *)(ppppppppppfVar73 + 0x15),(float *)(ppppppppppfVar73 + 0x15),
                          (float *)&local_1f0c);
          }
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 3),
                          ALBW_AS_F32(pppppppppppfVar34));
            local_1f0c = local_1ebc;
            local_1f08 = local_1eb8;
            local_1f04 = (float ***********)local_1eb4;
            func_00115D70((float *)&local_1ebc,(float *)&local_1f0c,ALBW_AS_F32(uVar67));
            local_1ec8 = local_1ebc;
            local_1ec4 = (float ************)local_1eb8;
            local_1ec0 = local_1eb4;
            func_00115C54((float *)(ppppppppppfVar73 + 0xf),(float *)(ppppppppppfVar73 + 0xf),
                          (float *)&local_1ec8);
          }
          fVar5 = ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
          *(undefined1 *)(ppppppppppfVar73 + 0x25) = 1;
          if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
            if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
              func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 0x15),
                            (float *)(ppppppppppfVar73 + 0xf));
              func_00115D70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar73 + 6),
                            ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ec8) +
                            ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ec4) +
                            ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ec0));
              local_1ebc = local_1f28[3];
              local_1eb8 = local_1f28[4];
              local_1eb4 = (float ************)local_1f14;
              func_00115C70((float *)(local_1f28 + 1),(float *)(ppppppppppfVar73 + 0x15),
                            (float *)&local_1ebc);
              local_1e90 = local_1f28[1];
              local_1e8c = local_1f28[2];
              local_1e88 = local_1f28[3];
            }
            else {
              local_1e90 = (float ***********)ppppppppppfVar73[0xf];
              local_1e8c = (float ***********)ppppppppppfVar73[0x10];
              local_1e88 = (float ***********)ppppppppppfVar73[0x11];
            }
            if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
              func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 0xf),
                            (float *)&local_1e90);
              local_1f28[1] = local_1ec8;
              local_1f28[2] = (float ***********)local_1ec4;
              local_1f28[3] = (float ***********)local_1ec0;
              func_0068906C((float *)&local_1f0c,(float *)(local_1f28 + 1));
            }
            else {
              func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 0x15),
                            (float *)&local_1e90);
              local_1f28[1] = local_1ec8;
              local_1f28[2] = (float ***********)local_1ec4;
              local_1f28[3] = (float ***********)local_1ec0;
              func_0068906C((float *)&local_1f0c,(float *)(local_1f28 + 1));
            }
            iVar21 = DAT_001fa16c;
            fVar58 = ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f0c) +
                     ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f08) +
                     ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f04);
            fVar55 = fVar58;
            if (fVar58 <= ALBW_AS_F32(pppppppppfVar23)) {
              fVar55 = -fVar58;
            }
            if (DAT_001fa16c < (int)ALBW_AS_U32(fVar55)) {
              func_00115C70((float *)(local_1f28 + 1),(float *)&local_1e90,
                            (float *)(ppppppppppfVar22 + 0xc));
              fVar55 = -(ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1f28[1]) +
                         ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1f28[2]) +
                        ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1f28[3]));
              if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
                if ((uint)ALBW_AS_U32(DAT_001fb1f8) < (uint)ALBW_AS_U32(fVar55)) {
                  func_00115C70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar73 + 0xc),
                                (float *)(ppppppppppfVar73 + 9));
                  func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar22,
                                ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(local_1f28[3]) +
                                ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(local_1f28[4]) +
                                ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(local_1f14));
                  local_1eac = (float ************)local_1ec8;
                  local_1ea8 = (float ***********)local_1ec4;
                  local_1ea4 = (float ***********)local_1ec0;
                  func_00115C54((float *)&local_1ec8,(float *)&local_1e80,(float *)&local_1eac);
                  ppppppppppfVar22[0x12] = (float *********)local_1ec8;
                  ppppppppppfVar22[0x13] = (float *********)local_1ec4;
                  ppppppppppfVar22[0x14] = (float *********)local_1ec0;
                  func_00115D70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar22 + 3),
                                ALBW_AS_F32(pppppppppppfVar34));
                  local_1ebc = local_1f28[3];
                  local_1eb8 = local_1f28[4];
                  local_1eb4 = (float ************)local_1f14;
                  func_00115D70((float *)(local_1f28 + 1),(float *)&local_1ebc,fVar5);
                  local_1eac = (float ************)local_1f28[1];
                  local_1ea8 = local_1f28[2];
                  local_1ea4 = local_1f28[3];
                  func_00115C54((float *)(ppppppppppfVar22 + 0x12),
                                (float *)(ppppppppppfVar22 + 0x12),(float *)&local_1eac);
                  *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
                  if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
                    func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x18),
                                  (float *)(ppppppppppfVar22 + 0x12));
                    func_00115D70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar22 + 6),
                                  ALBW_AS_F32(ppppppppppfVar22[6]) * ALBW_AS_F32(local_1ec8) +
                                  ALBW_AS_F32(ppppppppppfVar22[7]) * ALBW_AS_F32(local_1ec4) +
                                  ALBW_AS_F32(ppppppppppfVar22[8]) * ALBW_AS_F32(local_1ec0));
                    local_1e64 = local_1f28[3];
                    local_1e60 = local_1f28[4];
                    local_1e5c = local_1f14;
                    func_00115C70((float *)(local_1f28 + 1),(float *)(ppppppppppfVar22 + 0x18),
                                  (float *)&local_1e64);
                    local_1e90 = local_1f28[1];
                    local_1e8c = local_1f28[2];
                    local_1e88 = local_1f28[3];
                  }
                  else {
                    local_1e90 = (float ***********)ppppppppppfVar22[0x12];
                    local_1e8c = (float ***********)ppppppppppfVar22[0x13];
                    local_1e88 = (float ***********)ppppppppppfVar22[0x14];
                  }
                  if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
                    func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x12),
                                  (float *)&local_1e90);
                    local_1f28[1] = local_1ec8;
                    local_1f28[2] = (float ***********)local_1ec4;
                    local_1f28[3] = (float ***********)local_1ec0;
                    func_0068906C((float *)&local_1f0c,(float *)(local_1f28 + 1));
                  }
                  else {
                    func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar22 + 0x18),
                                  (float *)&local_1e90);
                    local_1f28[1] = local_1ec8;
                    local_1f28[2] = (float ***********)local_1ec4;
                    local_1f28[3] = (float ***********)local_1ec0;
                    func_0068906C((float *)&local_1f0c,(float *)(local_1f28 + 1));
                  }
                  fVar55 = ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f0c) +
                           ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f08) +
                           ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f04);
                  fVar5 = fVar55;
                  if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
                    fVar5 = -fVar55;
                  }
                  if (iVar21 < (int)ALBW_AS_U32(fVar5)) {
                    func_00115C70((float *)local_1f28,(float *)&local_1e90,
                                  (float *)(ppppppppppfVar73 + 9));
                    fVar5 = -(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[0]) +
                             ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[1])) +
                            -(ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[2]));
                    if (ALBW_AS_F32(pppppppppfVar23) < fVar5) {
                      func_00115D70((float *)&local_1e64,(float *)&local_1f0c,fVar5);
                      local_1f28[1] = local_1e64;
                      local_1f28[2] = local_1e60;
                      local_1f28[3] = local_1e5c;
                      func_00115D88((float *)&local_1e64,(float *)(local_1f28 + 1),fVar55);
                      local_1e4c = local_1e64;
                      local_1e48 = local_1e60;
                      local_1e44 = local_1e5c;
                      func_00115C54((float *)&local_1e64,(float *)&local_1e90,(float *)&local_1e4c);
                      local_1ec8 = local_1e64;
                      local_1ec4 = (float ************)local_1e60;
                      local_1ec0 = (float ************)local_1e5c;
                      func_00115C70((float *)&local_1e64,(float *)(ppppppppppfVar73 + 0xf),
                                    (float *)&local_1ec8);
                      local_1f28[1] = local_1e64;
                      local_1f28[2] = local_1e60;
                      local_1f28[3] = local_1e5c;
                      if (ALBW_AS_F32(pppppppppfVar23) <
                          ALBW_AS_F32(local_1e64) * ALBW_AS_F32(ppppppppppfVar73[3]) +
                          ALBW_AS_F32(local_1e60) * ALBW_AS_F32(ppppppppppfVar73[4]) +
                          ALBW_AS_F32(local_1e5c) * ALBW_AS_F32(ppppppppppfVar73[5])) {
                        ppppppppppfVar73[0x15] = (float *********)local_1ec8;
                        ppppppppppfVar73[0x16] = (float *********)local_1ec4;
                        ppppppppppfVar73[0x17] = (float *********)local_1ec0;
                        *(undefined1 *)(ppppppppppfVar73 + 0x25) = 1;
                        if ((*(char *)((int)ppppppppppfVar73 + 0x97) != '\0') &&
                           (*(char *)((int)ppppppppppfVar73 + 0x95) == '\0')) {
                          ppppppppppfVar30 = ppppppppppfVar73 + 0x18;
                          ppppppppppfVar73[0x18] = ppppppppppfVar73[0x15];
                          ppppppppppfVar73[0x19] = ppppppppppfVar73[0x16];
                          pppppppppfVar48 = ppppppppppfVar73[0x17];
                          goto LAB_001fb38c;
                        }
                      }
                    }
                  }
                }
              }
              else {
                func_00115D70((float *)&local_1ebc,(float *)&local_1f0c,fVar55);
                local_1f28[2] = local_1ebc;
                local_1f28[3] = local_1eb8;
                local_1f28[4] = (float ***********)local_1eb4;
                func_00115D88((float *)&local_1ebc,(float *)(local_1f28 + 2),fVar58);
                local_1eac = (float ************)local_1ebc;
                local_1ea8 = local_1eb8;
                local_1ea4 = (float ***********)local_1eb4;
                func_00115C54((float *)&local_1ebc,(float *)&local_1e90,(float *)&local_1eac);
                local_1ec8 = local_1ebc;
                local_1ec4 = (float ************)local_1eb8;
                local_1ec0 = local_1eb4;
                func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar22 + 0x12),
                              (float *)&local_1ec8);
                local_1f28[2] = local_1ebc;
                local_1f28[3] = local_1eb8;
                local_1f28[4] = (float ***********)local_1eb4;
                if (ALBW_AS_F32(pppppppppfVar23) <
                    ALBW_AS_F32(local_1ebc) * ALBW_AS_F32(ppppppppppfVar22[3]) +
                    ALBW_AS_F32(local_1eb8) * ALBW_AS_F32(ppppppppppfVar22[4]) +
                    ALBW_AS_F32(local_1eb4) * ALBW_AS_F32(ppppppppppfVar22[5])) {
                  ppppppppppfVar22[0x18] = (float *********)local_1ec8;
                  ppppppppppfVar22[0x19] = (float *********)local_1ec4;
                  ppppppppppfVar22[0x1a] = (float *********)local_1ec0;
                  *(undefined1 *)((int)ppppppppppfVar22 + 0x95) = 1;
                  if ((*(char *)((int)ppppppppppfVar22 + 0x97) != '\0') &&
                     (*(char *)(ppppppppppfVar22 + 0x25) == '\0')) {
                    ppppppppppfVar30 = ppppppppppfVar22 + 0x15;
                    ppppppppppfVar22[0x15] = ppppppppppfVar22[0x18];
                    ppppppppppfVar22[0x16] = ppppppppppfVar22[0x19];
                    pppppppppfVar48 = ppppppppppfVar22[0x1a];
LAB_001fb38c:
                    ppppppppppfVar30[2] = pppppppppfVar48;
                  }
                }
              }
            }
          }
        }
        ppppppppppfVar56 = (float **********)((int)ppppppppppfVar56 + 1);
        local_1eb0 = (float ***********)((int)local_1eb0 + 1);
      } while ((int)ppppppppppfVar56 < (int)local_1ea0);
      if (0 < (int)local_1eb0) {
        ppppppppppfVar22 = (float **********)0x0;
        do {
          if (ppppppppppfVar22 < *pppppppppppfVar15) {
            ppppppppppfVar73 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar22 * 0x27;
          }
          else {
            ppppppppppfVar73 = pppppppppppfVar49[0x2b];
          }
          *(undefined1 *)((int)ppppppppppfVar73 + 0x91) = 1;
          *(undefined1 *)((int)ppppppppppfVar73 + 0x92) = 1;
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            local_1e9c = (float ***********)ppppppppppfVar73[0x15];
            local_1e98 = (float ***********)ppppppppppfVar73[0x16];
            local_1e94 = (float ***********)ppppppppppfVar73[0x17];
            func_00115C70((float *)&local_1e84,(float *)&local_1e9c,
                          (float *)(ppppppppppfVar73 + 0xf));
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 6),
                          ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1e84) +
                          ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1e80) +
                          ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1e7c));
            local_1f28[3] = local_1ec8;
            local_1f28[4] = (float ***********)local_1ec4;
            local_1f14 = (float ***********)local_1ec0;
            func_00115C70((float *)&local_1e90,(float *)&local_1e9c,(float *)(local_1f28 + 3));
            ppppppppppfVar73[0x15] = (float *********)local_1e90;
            ppppppppppfVar73[0x16] = (float *********)local_1e8c;
            ppppppppppfVar73[0x17] = (float *********)local_1e88;
            local_1e9c = (float ***********)ppppppppppfVar73[0x18];
            local_1e98 = (float ***********)ppppppppppfVar73[0x19];
            local_1e94 = (float ***********)ppppppppppfVar73[0x1a];
            func_00115C70((float *)&stack0xffffe0d4,(float *)&local_1e9c,
                          (float *)(ppppppppppfVar73 + 0x12));
            func_00115D70((float *)&local_1eac,(float *)(ppppppppppfVar73 + 6),
                          ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(in_stack_ffffe0d4) +
                          ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[0]) +
                          ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[1]));
            local_1f28[3] = (float ***********)local_1eac;
            local_1f28[4] = local_1ea8;
            local_1f14 = local_1ea4;
            func_00115C70((float *)&local_1e78,(float *)&local_1e9c,(float *)(local_1f28 + 3));
            ppppppppppfVar73[0x18] = (float *********)local_1e78;
            ppppppppppfVar73[0x19] = (float *********)local_1e74;
            ppppppppppfVar73[0x1a] = (float *********)local_1e70;
          }
          ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + 1);
        } while ((int)ppppppppppfVar22 <= (int)local_1eb0);
      }
    }
    ppppppppppfVar22 = (float **********)((int)local_1ea0 + -1);
    if (((int)ppppppppppfVar56 < (int)local_1ea0) &&
       (pppppppppppfVar34 = pppppppppppfVar74, (int)ppppppppppfVar56 <= (int)ppppppppppfVar22)) {
      do {
        if ((float **********)((int)ppppppppppfVar22 + -1) < *pppppppppppfVar15) {
          ppppppppppfVar73 = pppppppppppfVar49[0x2b] + ((int)ppppppppppfVar22 + -1) * 0x27;
        }
        else {
          ppppppppppfVar73 = pppppppppppfVar49[0x2b];
        }
        if (ppppppppppfVar22 < *pppppppppppfVar15) {
          ppppppppppfVar30 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar22 * 0x27;
        }
        else {
          ppppppppppfVar30 = pppppppppppfVar49[0x2b];
        }
        if ((((int)ppppppppppfVar73[7] <= DAT_001fc480) &&
            ((int)ppppppppppfVar30[7] <= DAT_001fc480)) ||
           ((int)ALBW_AS_U32((ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(ppppppppppfVar30[6]) +
                  ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(ppppppppppfVar30[7]) +
                 ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(ppppppppppfVar30[8]))) <= DAT_001fb1f4)) break;
        fVar5 = ALBW_AS_F32(pppppppppppfVar17[0x1c]) / ALBW_AS_F32(ppppppppppfVar73[4]);
        if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar73 + 0xc);
        }
        else {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar73 + 0x15);
        }
        uVar67 = CONCAT44(fVar5,ALBW_AS_F32(pppppppppppfVar17[0x1c]) / ALBW_AS_F32(ppppppppppfVar30[4]));
        local_1e64 = pppppppppppfVar78;
        func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 3),fVar5);
        local_1ec8 = local_1ebc;
        local_1ec4 = (float ************)local_1eb8;
        local_1ec0 = local_1eb4;
        func_00115C70((float *)&local_1ebc,(float *)pppppppppppfVar78,(float *)&local_1ec8);
        local_1f28[1] = local_1ebc;
        local_1f28[2] = local_1eb8;
        local_1f28[3] = (float ***********)local_1eb4;
        local_1e4c = local_1ebc;
        local_1e48 = local_1eb8;
        local_1e44 = (float ***********)local_1eb4;
        if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar30 + 9);
        }
        else {
          pppppppppppfVar78 = (float ***********)(ppppppppppfVar30 + 0x15);
        }
        local_1f08 = pppppppppppfVar78;
        func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar30 + 3),ALBW_AS_F32(uVar67));
        local_1f28[1] = local_1ebc;
        local_1f28[2] = local_1eb8;
        local_1f28[3] = (float ***********)local_1eb4;
        func_00115C70((float *)&local_1ebc,(float *)pppppppppppfVar78,(float *)(local_1f28 + 1));
        local_1ec8 = local_1ebc;
        local_1ec4 = (float ************)local_1eb8;
        local_1ec0 = local_1eb4;
        local_1e84 = local_1ebc;
        local_1e80 = local_1eb8;
        local_1e7c = (float ***********)local_1eb4;
        fVar5 = ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(ppppppppppfVar30[6]) +
                ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(ppppppppppfVar30[7]) +
                ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(ppppppppppfVar30[8]);
        func_00115C70((float *)&local_1e0c,(float *)&local_1e4c,(float *)local_1f08);
        pppppppppfVar48 =
             (float *********)
             ALBW_PTR_BITS(((-(ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1e0c) +
                ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1e08)) +
              -(ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1e04))) / fVar5));
        lVar69 = ZEXT48(pppppppppfVar48) << 0x20;
        if (DAT_001fc480 < (int)pppppppppfVar48) {
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            ppppppppppfVar11 = *pppppppppppfVar15;
            bVar47 = ppppppppppfVar22 <= ppppppppppfVar11;
            bVar44 = ppppppppppfVar11 == ppppppppppfVar22;
            ppppppppppfVar10 = ppppppppppfVar22;
            pppppppppfVar24 = pppppppppfVar23;
            if ((int)ppppppppppfVar22 < (int)ppppppppppfVar11) {
              do {
                if (!bVar47 || bVar44) {
                  ppppppppppfVar11 = pppppppppppfVar49[0x2b];
                }
                else {
                  ppppppppppfVar11 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar10 * 0x27;
                }
                func_00115C70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar11 + 0xc),
                              (float *)(ppppppppppfVar11 + 9));
                pppppppppfVar48 = (float *********)((ulonglong)lVar69 >> 0x20);
                ppppppppppfVar10 = (float **********)((int)ppppppppppfVar10 + 1);
                ppppppppppfVar12 = *pppppppppppfVar15;
                fVar5 = ALBW_AS_F32(*ppppppppppfVar11) * ALBW_AS_F32(local_1f28[3]) +
                        ALBW_AS_F32(ppppppppppfVar11[1]) * ALBW_AS_F32(local_1f28[4]) +
                        ALBW_AS_F32(ppppppppppfVar11[2]) * ALBW_AS_F32(local_1f14);
                if (fVar5 <= ALBW_AS_F32(pppppppppfVar23)) {
                  fVar5 = -fVar5;
                }
                bVar47 = ppppppppppfVar10 <= ppppppppppfVar12;
                bVar44 = ppppppppppfVar12 == ppppppppppfVar10;
                pppppppppfVar24 = (float *********)ALBW_PTR_BITS((fVar5 + ALBW_AS_F32(pppppppppfVar24)));
              } while (!bVar44 && (int)ppppppppppfVar10 <= (int)ppppppppppfVar12);
            }
            if (ALBW_AS_F32(pppppppppfVar24) < ALBW_AS_F32(pppppppppfVar48)) {
              pppppppppfVar48 = pppppppppfVar24;
            }
            if (*(char *)((int)ppppppppppfVar73 + 0x93) != '\0') goto LAB_001fb7a0;
            func_00115D70((float *)&local_1e3c,(float *)ppppppppppfVar73,ALBW_AS_F32(pppppppppfVar48));
            local_1eb0 = local_1e3c;
            local_1eac = (float ************)local_1e38;
            local_1ea8 = local_1e34;
            func_00115C54((float *)&local_1e18,(float *)&local_1e4c,(float *)&local_1eb0);
            ppppppppppfVar73[0x12] = (float *********)local_1e18;
            ppppppppppfVar73[0x13] = (float *********)local_1e14;
            ppppppppppfVar73[0x14] = (float *********)local_1e10;
          }
          else {
LAB_001fb7a0:
            func_00115D70((float *)&local_1e24,(float *)ppppppppppfVar73,ALBW_AS_F32(pppppppppfVar48));
            local_1e74 = local_1e24;
            local_1e70 = local_1e20;
            local_1e6c = local_1e1c;
            func_00115C54((float *)(ppppppppppfVar73 + 0x18),(float *)(ppppppppppfVar73 + 0x18),
                          (float *)&local_1e74);
          }
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 3),
                          ALBW_AS_F32(pppppppppppfVar34));
            local_1f28[2] = local_1ebc;
            local_1f28[3] = local_1eb8;
            local_1f28[4] = (float ***********)local_1eb4;
            func_00115D70((float *)&local_1ebc,(float *)(local_1f28 + 2),
                          ALBW_AS_F32(((ulonglong)uVar67 >> 0x20)));
            local_1ec8 = local_1ebc;
            local_1ec4 = (float ************)local_1eb8;
            local_1ec0 = local_1eb4;
            func_00115C54((float *)(ppppppppppfVar73 + 0x12),(float *)(ppppppppppfVar73 + 0x12),
                          (float *)&local_1ec8);
          }
          *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 1;
          if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
            if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
              local_1f0c = (float ***********)(ppppppppppfVar73 + 0x18);
              func_00115C70((float *)&local_1e90,(float *)(ppppppppppfVar73 + 0x18),
                            (float *)(ppppppppppfVar73 + 0x12));
              func_00115D70((float *)(local_1f28 + 2),(float *)(ppppppppppfVar73 + 6),
                            ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1e90) +
                            ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1e8c) +
                            ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1e88));
              local_1eb0 = local_1f28[2];
              local_1eac = (float ************)local_1f28[3];
              local_1ea8 = local_1f28[4];
              func_00115C70((float *)(local_1f28 + 1),(float *)local_1f0c,(float *)&local_1eb0);
              local_1ebc = local_1f28[1];
              local_1eb8 = local_1f28[2];
              local_1eb4 = (float ************)local_1f28[3];
            }
            else {
              local_1ebc = (float ***********)ppppppppppfVar73[0x12];
              local_1eb8 = (float ***********)ppppppppppfVar73[0x13];
              local_1eb4 = (float ************)ppppppppppfVar73[0x14];
            }
            if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
              func_00115C70((float *)(local_1f28 + 1),(float *)(ppppppppppfVar73 + 0x12),
                            (float *)&local_1ebc);
              local_1eb0 = local_1f28[1];
              local_1eac = (float ************)local_1f28[2];
              local_1ea8 = local_1f28[3];
              func_0068906C((float *)&local_1ec8,(float *)&local_1eb0);
            }
            else {
              func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar73 + 0x18),
                            (float *)&local_1ebc);
              local_1f28[1] = local_1eb0;
              local_1f28[2] = (float ***********)local_1eac;
              local_1f28[3] = local_1ea8;
              func_0068906C((float *)&local_1ec8,(float *)(local_1f28 + 1));
            }
            fVar55 = ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1ec8) +
                     ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1ec4) +
                     ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1ec0);
            fVar5 = fVar55;
            if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
              fVar5 = -fVar55;
            }
            if (DAT_001fc484 < (int)ALBW_AS_U32(fVar5)) {
              func_00115C70((float *)(local_1f28 + 2),(float *)&local_1ebc,
                            (float *)(ppppppppppfVar30 + 9));
              fVar5 = -(ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1f28[2]) +
                        ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1f28[3]) +
                       ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1f28[4]));
              if (fVar5 <= ALBW_AS_F32(pppppppppfVar23)) {
                if ((uint)ALBW_AS_U32(DAT_001fb1f8) < (uint)ALBW_AS_U32(fVar5)) {
                  func_00115C70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar73 + 0xc),
                                (float *)(ppppppppppfVar73 + 9));
                  func_00115D70((float *)&local_1eb0,(float *)ppppppppppfVar30,
                                ALBW_AS_F32(*ppppppppppfVar73) * ALBW_AS_F32(local_1f28[3]) +
                                ALBW_AS_F32(ppppppppppfVar73[1]) * ALBW_AS_F32(local_1f28[4]) +
                                ALBW_AS_F32(ppppppppppfVar73[2]) * ALBW_AS_F32(local_1f14));
                  local_1e3c = local_1eb0;
                  local_1e38 = (float ***********)local_1eac;
                  local_1e34 = local_1ea8;
                  func_00115C70((float *)&local_1eb0,(float *)&local_1e84,(float *)&local_1e3c);
                  ppppppppppfVar30[0xf] = (float *********)local_1eb0;
                  ppppppppppfVar30[0x10] = (float *********)local_1eac;
                  ppppppppppfVar30[0x11] = (float *********)local_1ea8;
                  func_00115D70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar30 + 3),
                                ALBW_AS_F32(pppppppppppfVar34));
                  local_1e74 = local_1f28[3];
                  local_1e70 = local_1f28[4];
                  local_1e6c = local_1f14;
                  func_00115D70((float *)(local_1f28 + 2),(float *)&local_1e74,ALBW_AS_F32(uVar67));
                  local_1e3c = local_1f28[2];
                  local_1e38 = local_1f28[3];
                  local_1e34 = local_1f28[4];
                  func_00115C54((float *)(ppppppppppfVar30 + 0xf),(float *)(ppppppppppfVar30 + 0xf),
                                (float *)&local_1e3c);
                  *(undefined1 *)(ppppppppppfVar30 + 0x25) = 1;
                  if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
                    local_1f0c = (float ***********)(ppppppppppfVar30 + 0x15);
                    local_1f28[1] = (float ***********)(ppppppppppfVar30 + 6);
                    func_00115C70((float *)&local_1e90,(float *)(ppppppppppfVar30 + 0x15),
                                  (float *)(ppppppppppfVar30 + 0xf));
                    func_00115D70((float *)(local_1f28 + 3),(float *)local_1f28[1],
                                  ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1e90) +
                                  ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1e8c) +
                                  ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1e88));
                    local_1eb0 = local_1f28[3];
                    local_1eac = (float ************)local_1f28[4];
                    local_1ea8 = local_1f14;
                    func_00115C70((float *)&local_1e90,(float *)local_1f0c,(float *)&local_1eb0);
                    local_1ebc = local_1e90;
                    local_1eb8 = local_1e8c;
                    local_1eb4 = (float ************)local_1e88;
                  }
                  else {
                    local_1ebc = (float ***********)ppppppppppfVar30[0xf];
                    local_1eb8 = (float ***********)ppppppppppfVar30[0x10];
                    local_1eb4 = (float ************)ppppppppppfVar30[0x11];
                  }
                  if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
                    func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar30 + 0xf),
                                  (float *)&local_1ebc);
                    local_1f28[1] = local_1eb0;
                    local_1f28[2] = (float ***********)local_1eac;
                    local_1f28[3] = local_1ea8;
                    func_0068906C((float *)&local_1ec8,(float *)(local_1f28 + 1));
                  }
                  else {
                    func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar30 + 0x15),
                                  (float *)&local_1ebc);
                    local_1f28[1] = local_1eb0;
                    local_1f28[2] = (float ***********)local_1eac;
                    local_1f28[3] = local_1ea8;
                    func_0068906C((float *)&local_1ec8,(float *)(local_1f28 + 1));
                  }
                  fVar55 = ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ec8) +
                           ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ec4) +
                           ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ec0);
                  fVar5 = fVar55;
                  if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
                    fVar5 = -fVar55;
                  }
                  if (DAT_001fc484 < (int)ALBW_AS_U32(fVar5)) {
                    func_00115C70((float *)(local_1f28 + 1),(float *)&local_1ebc,
                                  (float *)(ppppppppppfVar73 + 0xc));
                    fVar5 = -(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[1]) +
                              ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[2]) +
                             ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[3]));
                    if (ALBW_AS_F32(pppppppppfVar23) < fVar5) {
                      func_00115D70((float *)&local_1e24,(float *)&local_1ec8,fVar5);
                      local_1f28[2] = local_1e24;
                      local_1f28[3] = local_1e20;
                      local_1f28[4] = local_1e1c;
                      func_00115D88((float *)&local_1e24,(float *)(local_1f28 + 2),fVar55);
                      local_1e90 = local_1e24;
                      local_1e8c = local_1e20;
                      local_1e88 = local_1e1c;
                      func_00115C54((float *)&local_1e24,(float *)&local_1ebc,(float *)&local_1e90);
                      local_1eb0 = local_1e24;
                      local_1eac = (float ************)local_1e20;
                      local_1ea8 = local_1e1c;
                      func_00115C70((float *)&local_1e24,(float *)(ppppppppppfVar73 + 0x12),
                                    (float *)&local_1eb0);
                      local_1f28[2] = local_1e24;
                      local_1f28[3] = local_1e20;
                      local_1f28[4] = local_1e1c;
                      if (ALBW_AS_F32(pppppppppfVar23) <
                          ALBW_AS_F32(local_1e24) * ALBW_AS_F32(ppppppppppfVar73[3]) +
                          ALBW_AS_F32(local_1e20) * ALBW_AS_F32(ppppppppppfVar73[4]) +
                          ALBW_AS_F32(local_1e1c) * ALBW_AS_F32(ppppppppppfVar73[5])) {
                        ppppppppppfVar73[0x18] = (float *********)local_1eb0;
                        ppppppppppfVar73[0x19] = (float *********)local_1eac;
                        ppppppppppfVar73[0x1a] = (float *********)local_1ea8;
                        *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 1;
                        if ((*(char *)((int)ppppppppppfVar73 + 0x97) != '\0') &&
                           (*(char *)(ppppppppppfVar73 + 0x25) == '\0')) {
                          ppppppppppfVar10 = ppppppppppfVar73 + 0x15;
                          ppppppppppfVar73[0x15] = ppppppppppfVar73[0x18];
                          ppppppppppfVar73[0x16] = ppppppppppfVar73[0x19];
                          pppppppppfVar48 = ppppppppppfVar73[0x1a];
                          goto LAB_001fbe64;
                        }
                      }
                    }
                  }
                }
              }
              else {
                func_00115D70((float *)&local_1e90,(float *)&local_1ec8,fVar5);
                local_1f28[2] = local_1e90;
                local_1f28[3] = local_1e8c;
                local_1f28[4] = local_1e88;
                func_00115D88((float *)&local_1e90,(float *)(local_1f28 + 2),fVar55);
                local_1e74 = local_1e90;
                local_1e70 = local_1e8c;
                local_1e6c = local_1e88;
                func_00115C54((float *)&local_1e90,(float *)&local_1ebc,(float *)&local_1e74);
                local_1eb0 = local_1e90;
                local_1eac = (float ************)local_1e8c;
                local_1ea8 = local_1e88;
                func_00115C70((float *)&local_1e90,(float *)(ppppppppppfVar30 + 0xf),
                              (float *)&local_1eb0);
                local_1f28[2] = local_1e90;
                local_1f28[3] = local_1e8c;
                local_1f28[4] = local_1e88;
                if (ALBW_AS_F32(pppppppppfVar23) <
                    ALBW_AS_F32(local_1e90) * ALBW_AS_F32(ppppppppppfVar30[3]) +
                    ALBW_AS_F32(local_1e8c) * ALBW_AS_F32(ppppppppppfVar30[4]) +
                    ALBW_AS_F32(local_1e88) * ALBW_AS_F32(ppppppppppfVar30[5])) {
                  ppppppppppfVar30[0x15] = (float *********)local_1eb0;
                  ppppppppppfVar30[0x16] = (float *********)local_1eac;
                  ppppppppppfVar30[0x17] = (float *********)local_1ea8;
                  *(undefined1 *)(ppppppppppfVar30 + 0x25) = 1;
                  if ((*(char *)((int)ppppppppppfVar30 + 0x97) != '\0') &&
                     (*(char *)((int)ppppppppppfVar30 + 0x95) == '\0')) {
                    ppppppppppfVar10 = ppppppppppfVar30 + 0x18;
                    ppppppppppfVar30[0x18] = ppppppppppfVar30[0x15];
                    ppppppppppfVar30[0x19] = ppppppppppfVar30[0x16];
                    pppppppppfVar48 = ppppppppppfVar30[0x17];
LAB_001fbe64:
                    ppppppppppfVar10[2] = pppppppppfVar48;
                  }
                }
              }
            }
          }
        }
        fVar5 = -(ALBW_AS_F32(*ppppppppppfVar30) * ALBW_AS_F32(ppppppppppfVar73[6]) +
                  ALBW_AS_F32(ppppppppppfVar30[1]) * ALBW_AS_F32(ppppppppppfVar73[7]) +
                 ALBW_AS_F32(ppppppppppfVar30[2]) * ALBW_AS_F32(ppppppppppfVar73[8]));
        func_00115C70((float *)(local_1f28 + 1),(float *)&local_1e84,(float *)local_1e64);
        pppppppppfVar48 =
             (float *********)
             ALBW_PTR_BITS(((-(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[1]) +
                ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[2])) +
              -(ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[3]))) / fVar5));
        lVar69 = ZEXT48(pppppppppfVar48) << 0x20;
        if (DAT_001fc480 < (int)pppppppppfVar48) {
          if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
            ppppppppppfVar10 = (float **********)0x0;
            pppppppppfVar24 = pppppppppfVar23;
            if (0 < (int)ppppppppppfVar22) {
              do {
                if (ppppppppppfVar10 < *pppppppppppfVar15) {
                  ppppppppppfVar11 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar10 * 0x27;
                }
                else {
                  ppppppppppfVar11 = pppppppppppfVar49[0x2b];
                }
                func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar11 + 0xc),
                              (float *)(ppppppppppfVar11 + 9));
                pppppppppfVar48 = (float *********)((ulonglong)lVar69 >> 0x20);
                ppppppppppfVar10 = (float **********)((int)ppppppppppfVar10 + 1);
                fVar5 = ALBW_AS_F32(*ppppppppppfVar11) * ALBW_AS_F32(local_1ec8) +
                        ALBW_AS_F32(ppppppppppfVar11[1]) * ALBW_AS_F32(local_1ec4) +
                        ALBW_AS_F32(ppppppppppfVar11[2]) * ALBW_AS_F32(local_1ec0);
                if (fVar5 <= ALBW_AS_F32(pppppppppfVar23)) {
                  fVar5 = -fVar5;
                }
                pppppppppfVar24 = (float *********)ALBW_PTR_BITS((fVar5 + ALBW_AS_F32(pppppppppfVar24)));
              } while ((int)ppppppppppfVar10 < (int)ppppppppppfVar22);
            }
            if (ALBW_AS_F32(pppppppppfVar24) < ALBW_AS_F32(pppppppppfVar48)) {
              pppppppppfVar48 = pppppppppfVar24;
            }
            if (*(char *)((int)ppppppppppfVar30 + 0x93) != '\0') goto LAB_001fbf68;
            func_00115D70((float *)&local_1ebc,(float *)ppppppppppfVar30,ALBW_AS_F32(pppppppppfVar48));
            local_1ec8 = local_1ebc;
            local_1ec4 = (float ************)local_1eb8;
            local_1ec0 = local_1eb4;
            func_00115C70((float *)&local_1ebc,(float *)&local_1e84,(float *)&local_1ec8);
            ppppppppppfVar30[0xf] = (float *********)local_1ebc;
            ppppppppppfVar30[0x10] = (float *********)local_1eb8;
            ppppppppppfVar30[0x11] = (float *********)local_1eb4;
          }
          else {
LAB_001fbf68:
            local_1eb4 = (float ************)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar30[2]));
            local_1ebc = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(*ppppppppppfVar30));
            local_1eb8 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar30[1]));
            func_00115D70((float *)(local_1f28 + 1),(float *)&local_1ebc,ALBW_AS_F32(pppppppppfVar48));
            local_1ec8 = local_1f28[1];
            local_1ec4 = (float ************)local_1f28[2];
            local_1ec0 = (float ************)local_1f28[3];
            func_00115C54((float *)(ppppppppppfVar30 + 0x15),(float *)(ppppppppppfVar30 + 0x15),
                          (float *)&local_1ec8);
          }
          if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar30 + 3),
                          ALBW_AS_F32(pppppppppppfVar34));
            local_1f28[2] = local_1ec8;
            local_1f28[3] = (float ***********)local_1ec4;
            local_1f28[4] = (float ***********)local_1ec0;
            func_00115D70((float *)&local_1ec8,(float *)(local_1f28 + 2),ALBW_AS_F32(uVar67));
            local_1ebc = local_1ec8;
            local_1eb8 = (float ***********)local_1ec4;
            local_1eb4 = local_1ec0;
            func_00115C54((float *)(ppppppppppfVar30 + 0xf),(float *)(ppppppppppfVar30 + 0xf),
                          (float *)&local_1ebc);
          }
          fVar5 = ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
          *(undefined1 *)(ppppppppppfVar30 + 0x25) = 1;
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
              func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar30 + 0x15),
                            (float *)(ppppppppppfVar30 + 0xf));
              func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar30 + 6),
                            ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1ec8) +
                            ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1ec4) +
                            ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1ec0));
              local_1ebc = local_1ec8;
              local_1eb8 = (float ***********)local_1ec4;
              local_1eb4 = local_1ec0;
              func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar30 + 0x15),
                            (float *)&local_1ebc);
              local_1e64 = local_1ec8;
              local_1e60 = (float ***********)local_1ec4;
              local_1e5c = (float ***********)local_1ec0;
            }
            else {
              local_1e64 = (float ***********)ppppppppppfVar30[0xf];
              local_1e60 = (float ***********)ppppppppppfVar30[0x10];
              local_1e5c = (float ***********)ppppppppppfVar30[0x11];
            }
            if (*(char *)((int)ppppppppppfVar30 + 0x93) == '\0') {
              func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar30 + 0xf),
                            (float *)&local_1e64);
              local_1ec8 = local_1ebc;
              local_1ec4 = (float ************)local_1eb8;
              local_1ec0 = local_1eb4;
              func_0068906C((float *)(local_1f28 + 1),(float *)&local_1ec8);
            }
            else {
              func_00115C70((float *)&local_1ebc,(float *)(ppppppppppfVar30 + 0x15),
                            (float *)&local_1e64);
              local_1ec8 = local_1ebc;
              local_1ec4 = (float ************)local_1eb8;
              local_1ec0 = local_1eb4;
              func_0068906C((float *)(local_1f28 + 1),(float *)&local_1ec8);
            }
            iVar21 = DAT_001fc484;
            fVar58 = ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1f28[1]) +
                     ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1f28[2]) +
                     ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[3]);
            fVar55 = fVar58;
            if (fVar58 <= ALBW_AS_F32(pppppppppfVar23)) {
              fVar55 = -fVar58;
            }
            if (DAT_001fc484 < (int)ALBW_AS_U32(fVar55)) {
              func_00115C70((float *)&local_1ec8,(float *)&local_1e64,
                            (float *)(ppppppppppfVar73 + 0xc));
              fVar55 = -(ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1ec8) +
                         ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1ec4) +
                        ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ec0));
              if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
                if ((uint)ALBW_AS_U32(DAT_001fc488) < (uint)ALBW_AS_U32(fVar55)) {
                  func_00115C70((float *)&local_1ec8,(float *)(ppppppppppfVar30 + 0xc),
                                (float *)(ppppppppppfVar30 + 9));
                  func_00115D70((float *)&local_1ec8,(float *)ppppppppppfVar73,
                                ALBW_AS_F32(*ppppppppppfVar30) * ALBW_AS_F32(local_1ec8) +
                                ALBW_AS_F32(ppppppppppfVar30[1]) * ALBW_AS_F32(local_1ec4) +
                                ALBW_AS_F32(ppppppppppfVar30[2]) * ALBW_AS_F32(local_1ec0));
                  local_1e90 = local_1ec8;
                  local_1e8c = (float ***********)local_1ec4;
                  local_1e88 = (float ***********)local_1ec0;
                  func_00115C54((float *)&local_1ec8,(float *)&local_1e4c,(float *)&local_1e90);
                  ppppppppppfVar73[0x12] = (float *********)local_1ec8;
                  ppppppppppfVar73[0x13] = (float *********)local_1ec4;
                  ppppppppppfVar73[0x14] = (float *********)local_1ec0;
                  func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 3),
                                ALBW_AS_F32(pppppppppppfVar34));
                  local_1ebc = local_1ec8;
                  local_1eb8 = (float ***********)local_1ec4;
                  local_1eb4 = local_1ec0;
                  func_00115D70((float *)&local_1ec8,(float *)&local_1ebc,fVar5);
                  local_1e90 = local_1ec8;
                  local_1e8c = (float ***********)local_1ec4;
                  local_1e88 = (float ***********)local_1ec0;
                  func_00115C54((float *)(ppppppppppfVar73 + 0x12),
                                (float *)(ppppppppppfVar73 + 0x12),(float *)&local_1e90);
                  *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 1;
                  if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
                    local_1f00[1] = (float ***********)(ppppppppppfVar73 + 0x18);
                    func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar73 + 0x18),
                                  (float *)(ppppppppppfVar73 + 0x12));
                    func_00115D70((float *)&local_1eb0,(float *)(ppppppppppfVar73 + 6),
                                  ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1eb0) +
                                  ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1eac) +
                                  ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1ea8));
                    local_1ec8 = local_1eb0;
                    local_1ec4 = local_1eac;
                    local_1ec0 = (float ************)local_1ea8;
                    func_00115C70((float *)&local_1eb0,(float *)local_1f00[1],(float *)&local_1ec8);
                    local_1e64 = local_1eb0;
                    local_1e60 = (float ***********)local_1eac;
                    local_1e5c = local_1ea8;
                  }
                  else {
                    local_1e64 = (float ***********)ppppppppppfVar73[0x12];
                    local_1e60 = (float ***********)ppppppppppfVar73[0x13];
                    local_1e5c = (float ***********)ppppppppppfVar73[0x14];
                  }
                  if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
                    func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar73 + 0x12),
                                  (float *)&local_1e64);
                    local_1ec8 = local_1eb0;
                    local_1ec4 = local_1eac;
                    local_1ec0 = (float ************)local_1ea8;
                    func_0068906C((float *)(local_1f28 + 1),(float *)&local_1ec8);
                  }
                  else {
                    func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar73 + 0x18),
                                  (float *)&local_1e64);
                    local_1ec8 = local_1eb0;
                    local_1ec4 = local_1eac;
                    local_1ec0 = (float ************)local_1ea8;
                    func_0068906C((float *)(local_1f28 + 1),(float *)&local_1ec8);
                  }
                  fVar55 = ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1f28[1]) +
                           ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1f28[2]) +
                           ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1f28[3]);
                  fVar5 = fVar55;
                  if (fVar55 <= ALBW_AS_F32(pppppppppfVar23)) {
                    fVar5 = -fVar55;
                  }
                  if (iVar21 < (int)ALBW_AS_U32(fVar5)) {
                    func_00115C70((float *)&local_1ec8,(float *)&local_1e64,
                                  (float *)(ppppppppppfVar30 + 9));
                    fVar5 = -(ALBW_AS_F32(ppppppppppfVar30[6]) * ALBW_AS_F32(local_1ec8) +
                             ALBW_AS_F32(ppppppppppfVar30[7]) * ALBW_AS_F32(local_1ec4)) +
                            -(ALBW_AS_F32(ppppppppppfVar30[8]) * ALBW_AS_F32(local_1ec0));
                    if (ALBW_AS_F32(pppppppppfVar23) < fVar5) {
                      func_00115D70((float *)&local_1e78,(float *)(local_1f28 + 1),fVar5);
                      local_1ec8 = local_1e78;
                      local_1ec4 = (float ************)local_1e74;
                      local_1ec0 = (float ************)local_1e70;
                      func_00115D88((float *)&local_1e78,(float *)&local_1ec8,fVar55);
                      local_1e40 = local_1e78;
                      local_1e3c = local_1e74;
                      local_1e38 = local_1e70;
                      func_00115C54((float *)&local_1e78,(float *)&local_1e64,(float *)&local_1e40);
                      local_1eb0 = local_1e78;
                      local_1eac = (float ************)local_1e74;
                      local_1ea8 = local_1e70;
                      func_00115C70((float *)&local_1e78,(float *)(ppppppppppfVar30 + 0xf),
                                    (float *)&local_1eb0);
                      local_1ec8 = local_1e78;
                      local_1ec4 = (float ************)local_1e74;
                      local_1ec0 = (float ************)local_1e70;
                      if (ALBW_AS_F32(pppppppppfVar23) <
                          ALBW_AS_F32(local_1e78) * ALBW_AS_F32(ppppppppppfVar30[3]) +
                          ALBW_AS_F32(local_1e74) * ALBW_AS_F32(ppppppppppfVar30[4]) +
                          ALBW_AS_F32(local_1e70) * ALBW_AS_F32(ppppppppppfVar30[5])) {
                        ppppppppppfVar30[0x15] = (float *********)local_1eb0;
                        ppppppppppfVar30[0x16] = (float *********)local_1eac;
                        ppppppppppfVar30[0x17] = (float *********)local_1ea8;
                        *(undefined1 *)(ppppppppppfVar30 + 0x25) = 1;
                        if ((*(char *)((int)ppppppppppfVar30 + 0x97) != '\0') &&
                           (*(char *)((int)ppppppppppfVar30 + 0x95) == '\0')) {
                          ppppppppppfVar10 = ppppppppppfVar30 + 0x18;
                          ppppppppppfVar30[0x18] = ppppppppppfVar30[0x15];
                          ppppppppppfVar30[0x19] = ppppppppppfVar30[0x16];
                          pppppppppfVar48 = ppppppppppfVar30[0x17];
                          goto LAB_001fc61c;
                        }
                      }
                    }
                  }
                }
              }
              else {
                func_00115D70((float *)&local_1eb0,(float *)(local_1f28 + 1),fVar55);
                local_1ec8 = local_1eb0;
                local_1ec4 = local_1eac;
                local_1ec0 = (float ************)local_1ea8;
                func_00115D88((float *)&local_1eb0,(float *)&local_1ec8,fVar58);
                local_1e90 = local_1eb0;
                local_1e8c = (float ***********)local_1eac;
                local_1e88 = local_1ea8;
                func_00115C54((float *)&local_1eb0,(float *)&local_1e64,(float *)&local_1e90);
                local_1ebc = local_1eb0;
                local_1eb8 = (float ***********)local_1eac;
                local_1eb4 = (float ************)local_1ea8;
                func_00115C70((float *)&local_1eb0,(float *)(ppppppppppfVar73 + 0x12),
                              (float *)&local_1ebc);
                local_1ec8 = local_1eb0;
                local_1ec4 = local_1eac;
                local_1ec0 = (float ************)local_1ea8;
                if (ALBW_AS_F32(pppppppppfVar23) <
                    ALBW_AS_F32(local_1eb0) * ALBW_AS_F32(ppppppppppfVar73[3]) +
                    ALBW_AS_F32(local_1eac) * ALBW_AS_F32(ppppppppppfVar73[4]) +
                    ALBW_AS_F32(local_1ea8) * ALBW_AS_F32(ppppppppppfVar73[5])) {
                  ppppppppppfVar73[0x18] = (float *********)local_1ebc;
                  ppppppppppfVar73[0x19] = (float *********)local_1eb8;
                  ppppppppppfVar73[0x1a] = (float *********)local_1eb4;
                  *(undefined1 *)((int)ppppppppppfVar73 + 0x95) = 1;
                  if ((*(char *)((int)ppppppppppfVar73 + 0x97) != '\0') &&
                     (*(char *)(ppppppppppfVar73 + 0x25) == '\0')) {
                    ppppppppppfVar10 = ppppppppppfVar73 + 0x15;
                    ppppppppppfVar73[0x15] = ppppppppppfVar73[0x18];
                    ppppppppppfVar73[0x16] = ppppppppppfVar73[0x19];
                    pppppppppfVar48 = ppppppppppfVar73[0x1a];
LAB_001fc61c:
                    ppppppppppfVar10[2] = pppppppppfVar48;
                  }
                }
              }
            }
          }
        }
        ppppppppppfVar22 = (float **********)((int)ppppppppppfVar22 + -1);
        local_1e54 = (float ***********)((int)local_1e54 + 1);
      } while ((int)ppppppppppfVar56 <= (int)ppppppppppfVar22);
      if ((0 < (int)local_1e54) &&
         (ppppppppppfVar56 = (float **********)((int)local_1ea0 + -1),
         (int)ppppppppppfVar22 <= (int)ppppppppppfVar56)) {
        do {
          if (ppppppppppfVar56 < *pppppppppppfVar15) {
            ppppppppppfVar73 = pppppppppppfVar49[0x2b] + (int)ppppppppppfVar56 * 0x27;
          }
          else {
            ppppppppppfVar73 = pppppppppppfVar49[0x2b];
          }
          *(undefined1 *)((int)ppppppppppfVar73 + 0x91) = 1;
          *(undefined1 *)((int)ppppppppppfVar73 + 0x92) = 1;
          if (*(char *)((int)ppppppppppfVar73 + 0x93) == '\0') {
            local_1e9c = (float ***********)ppppppppppfVar73[0x15];
            local_1e98 = (float ***********)ppppppppppfVar73[0x16];
            local_1e94 = (float ***********)ppppppppppfVar73[0x17];
            func_00115C70((float *)&stack0xffffe0d0,(float *)&local_1e9c,
                          (float *)(ppppppppppfVar73 + 0xf));
            func_00115D70((float *)&local_1ec8,(float *)(ppppppppppfVar73 + 6),
                          ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(sp_x) +
                          ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(in_stack_ffffe0d4) +
                          ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1f28[0]));
            local_1f28[3] = local_1ec8;
            local_1f28[4] = (float ***********)local_1ec4;
            local_1f14 = (float ***********)local_1ec0;
            func_00115C70((float *)&local_1eb0,(float *)&local_1e9c,(float *)(local_1f28 + 3));
            ppppppppppfVar73[0x15] = (float *********)local_1eb0;
            ppppppppppfVar73[0x16] = (float *********)local_1eac;
            ppppppppppfVar73[0x17] = (float *********)local_1ea8;
            local_1e9c = (float ***********)ppppppppppfVar73[0x18];
            local_1e98 = (float ***********)ppppppppppfVar73[0x19];
            local_1e94 = (float ***********)ppppppppppfVar73[0x1a];
            func_00115C70((float *)&local_1e90,(float *)&local_1e9c,
                          (float *)(ppppppppppfVar73 + 0x12));
            func_00115D70((float *)&local_1ebc,(float *)(ppppppppppfVar73 + 6),
                          ALBW_AS_F32(ppppppppppfVar73[6]) * ALBW_AS_F32(local_1e90) +
                          ALBW_AS_F32(ppppppppppfVar73[7]) * ALBW_AS_F32(local_1e8c) +
                          ALBW_AS_F32(ppppppppppfVar73[8]) * ALBW_AS_F32(local_1e88));
            local_1f28[3] = local_1ebc;
            local_1f28[4] = local_1eb8;
            local_1f14 = (float ***********)local_1eb4;
            func_00115C70((float *)&local_1e84,(float *)&local_1e9c,(float *)(local_1f28 + 3));
            ppppppppppfVar73[0x18] = (float *********)local_1e84;
            ppppppppppfVar73[0x19] = (float *********)local_1e80;
            ppppppppppfVar73[0x1a] = (float *********)local_1e7c;
          }
          ppppppppppfVar56 = (float **********)((int)ppppppppppfVar56 + -1);
        } while ((int)ppppppppppfVar22 <= (int)ppppppppppfVar56);
      }
    }
  }
LAB_001fc7a8:
  pppppppppppfVar15 = local_30;
  /* WALKER: step 5: a face was found: MODE = 1 (merged into the wall) */
  *(byte *)local_1e28 = 1;
  local_1e50[0x29] = local_1e50[0xf];
  *local_1ddc = (float **********)local_30[10][4];
  local_30[0xed0] = local_30[0xc];
  local_30[0xed1] = local_30[0xd];
  local_30[0xed2] = local_30[0xe];
  *local_1e58 = (float ***********)local_30[0x12];
  local_1e58[1] = (float ***********)local_30[0x13];
  local_1e58[2] = (float ***********)local_30[0x14];
  *local_1de0 = local_30[0x15];
  local_1de0[1] = local_30[0x16];
  local_1de0[2] = local_30[0x17];
  local_1e58[6] = (float ***********)local_30[0x18];
  local_1e58[7] = (float ***********)local_30[0x19];
  local_1e58[8] = (float ***********)local_30[0x1a];
  pppppppppfVar48 = local_30[10][0x1b];
  uVar42 = (ushort)pppppppppfVar48;
  local_1eb8 = (float ***********)((uint)pppppppppfVar48 & 0xffff);
  if (((uint)pppppppppfVar48 & 0x7e0) != 0x1e0) {
    fVar5 = DAT_001fccac;
    func_00115D70((float *)&stack0xffffe0d4,(float *)(local_30 + 0x18),DAT_001fccac);
    local_1e58 = (float ************)local_1f28[0];
    local_1e54 = local_1f28[1];
    local_1e5c = in_stack_ffffe0d4;
    func_00432F74(&local_1e28);
    func_00115D70((float *)&stack0xffffe0d4,(float *)(pppppppppppfVar15 + 0x18),DAT_001fccac);
    local_1dbc = (float ************)local_1f28[0];
    local_1db8 = (float ************)local_1f28[1];
    local_1dc0 = in_stack_ffffe0d4;
    func_00115C70((float *)&stack0xffffe0d0,(float *)(pppppppppppfVar15 + 0xc),(float *)&local_1dc0)
    ;
    local_1dd8[2] = (float ***********)ALBW_PTR_BITS(local_2c[0x1b]);
    local_1e1c = local_1f28[0];
    fVar55 = 4.34403e-44;
    pppppppppfVar48 = (float *********)0x0;
    local_1ed0 = pppppppppppfVar50;
    local_1ecc = (float ************)pppppppppppfVar50;
    local_1ec8 = pppppppppppfVar50;
    local_1ec4 = (float ************)pppppppppppfVar50;
    local_1ec0 = (float ************)pppppppppppfVar50;
    local_1ebc = pppppppppppfVar50;
    local_1e24 = pppppppppppfVar17;
    local_1e20 = in_stack_ffffe0d4;
    while( true ) {
      ppppppppppfVar22 = *pppppppppppfVar15;
      pppppppppfVar23 = *ppppppppppfVar22;
      if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
      if (pppppppppfVar48 < pppppppppfVar23) {
        ppppppppppfVar56 = (float **********)ppppppppppfVar22[2][(int)pppppppppfVar48];
      }
      else {
        ppppppppppfVar56 = (float **********)0x0;
      }
      pppppppppppfVar49 = local_1ebc;
      if (pppppppppppfVar15[10] != ppppppppppfVar56) {
        if (pppppppppfVar48 < pppppppppfVar23) {
          pppppppppfVar24 = ppppppppppfVar22[2];
        }
        else {
          ppppppppppfVar22 = (float **********)0x0;
          pppppppppfVar24 = pppppppppfVar23;
        }
        if (pppppppppfVar48 < pppppppppfVar23) {
          ppppppppppfVar22 = (float **********)pppppppppfVar24[(int)pppppppppfVar48];
        }
        if (ALBW_AS_F32(ppppppppppfVar22[0xc]) * ALBW_AS_F32(pppppppppppfVar15[0x18]) +
            ALBW_AS_F32(ppppppppppfVar22[0xd]) * ALBW_AS_F32(pppppppppppfVar15[0x19]) +
            ALBW_AS_F32(ppppppppppfVar22[0xe]) * ALBW_AS_F32(pppppppppppfVar15[0x1a]) < ALBW_AS_F32(pppppppppppfVar50)
           ) {
          local_1eb4 = &local_1ea0;
          local_1ef4 = ppppppppppfVar22 + 9;
          local_1e98 = local_1e1c;
          local_1e94 = local_1dd8[2];
          local_1ea0 = local_1e24;
          local_1e9c = local_1e20;
          func_00115C70((float *)(local_1d70 + 2),(float *)local_1eb4,(float *)local_1ef4);
          local_1f08 = (float ***********)(ppppppppppfVar22 + 0xf);
          pppppppppppfVar17 = (float ***********)(ppppppppppfVar22 + 0x12);
          pppppppppppfVar9 = (float ***********)(ppppppppppfVar22 + 0x15);
          ppppppppppfVar56 = local_1d64;
          ppppppppppfVar73 = local_1d60;
          pppppppppppfVar34 = local_1d70[2];
          pppppppppppfVar78 = local_1e94;
          local_1ee8 = (float ************)pppppppppppfVar50;
          local_1ee4 = pppppppppppfVar50;
          local_1ee0 = (float ************)pppppppppppfVar50;
          func_00115C70((float *)&local_1ee8,(float *)local_1eb4,(float *)&local_1e5c);
          func_00115C70((float *)&local_1ee8,(float *)&local_1ee8,(float *)(ppppppppppfVar22 + 9));
          pppppppppfVar23 = ppppppppppfVar22[0xc];
          pppppppppppfVar49 = local_1ebc;
          if (ALBW_AS_F32(pppppppppppfVar50) <=
              ALBW_AS_F32(local_1ee8) * ALBW_AS_F32(pppppppppfVar23) +
              ALBW_AS_F32(local_1ee4) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
              ALBW_AS_F32(local_1ee0) * ALBW_AS_F32(ppppppppppfVar22[0xe])) {
            pppppppppfVar24 = (float *********)ALBW_PTR_BITS((fVar5 + ALBW_AS_F32(local_1e94) * DAT_001fccb0));
            if (ALBW_AS_F32(pppppppppfVar24) < ALBW_AS_F32(ppppppppppfVar22[0x18])) {
              pppppppppfVar24 = ppppppppppfVar22[0x18];
            }
            fVar57 = ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(pppppppppfVar23) +
                     ALBW_AS_F32(ppppppppppfVar56) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
                     ALBW_AS_F32(ppppppppppfVar73) * ALBW_AS_F32(ppppppppppfVar22[0xe]);
            fVar58 = ALBW_AS_F32(pppppppppppfVar78) - fVar57;
            if (fVar58 != ALBW_AS_F32(pppppppppppfVar50)) {
              if ((fVar58 < ALBW_AS_F32(pppppppppppfVar50)) || (ALBW_AS_F32(pppppppppfVar24) <= fVar58))
              goto LAB_001fcf3c;
              pppppppppppfVar75 =
                   (float ***********)
                   ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(*local_1f08) +
                    ALBW_AS_F32(ppppppppppfVar56) * ALBW_AS_F32(local_1f08[1]) +
                   ALBW_AS_F32(ppppppppppfVar73) * ALBW_AS_F32(local_1f08[2])));
              fVar60 = DAT_001fccb0;
              if (ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar75)) {
                uVar36 = 1;
LAB_001fcb70:
                func_00115C70((float *)&local_1e4c,(float *)&local_1ee8,(float *)local_1ef4);
                func_00115C70((float *)&local_1eac,(float *)local_1eb4,(float *)local_1ef4);
                pppppppppfVar23 = ppppppppppfVar22[0xc];
                fVar65 = ALBW_AS_F32(local_1e4c) * ALBW_AS_F32(pppppppppfVar23) +
                         ALBW_AS_F32(local_1e48) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
                         ALBW_AS_F32(local_1e44) * ALBW_AS_F32(ppppppppppfVar22[0xe]);
                if (fVar65 <= ALBW_AS_F32(pppppppppppfVar50)) {
                  fVar65 = -fVar65;
                }
                fVar54 = ALBW_AS_F32(local_1eac) * ALBW_AS_F32(pppppppppfVar23) +
                         ALBW_AS_F32(local_1ea8) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
                         ALBW_AS_F32(local_1ea4) * ALBW_AS_F32(ppppppppppfVar22[0xe]);
                if (fVar54 <= ALBW_AS_F32(pppppppppppfVar50)) {
                  fVar54 = -fVar54;
                }
                if (ALBW_AS_F32(pppppppppppfVar50) < fVar65 + fVar54) {
                  local_1e8c = local_1e5c;
                  local_1e88 = (float ***********)local_1e58;
                  local_1e84 = local_1e54;
                  func_00115D70((float *)&local_1e8c,(float *)&local_1e8c,
                                -(fVar54 / (fVar65 + fVar54)));
                  func_00115C54((float *)&local_1e8c,(float *)&local_1e8c,(float *)local_1eb4);
                  func_00115C70((float *)&local_1f04,(float *)&local_1e8c,(float *)local_1ef4);
                  pppppppppppfVar49 = local_1ebc;
                  if (((((uVar36 & 1) != 0) &&
                       (pppppppppppfVar75 =
                             (float ***********)
                             ALBW_PTR_BITS((ALBW_AS_F32(local_1f04) * ALBW_AS_F32(*local_1f08) +
                              ALBW_AS_F32(local_1f00[0]) * ALBW_AS_F32(local_1f08[1]) +
                             ALBW_AS_F32(local_1f00[1]) * ALBW_AS_F32(local_1f08[2]))),
                       ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar75))) ||
                      (((uVar36 & 3) != 0 &&
                       (pppppppppppfVar52 =
                             (float ***********)
                             ALBW_PTR_BITS((ALBW_AS_F32(local_1f04) * ALBW_AS_F32(*pppppppppppfVar17) +
                              ALBW_AS_F32(local_1f00[0]) * ALBW_AS_F32(ppppppppppfVar22[0x13]) +
                             ALBW_AS_F32(local_1f00[1]) * ALBW_AS_F32(ppppppppppfVar22[0x14]))),
                       ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar52))))) ||
                     ((uVar36 != 0 &&
                      (pppppppppppfVar74 =
                            (float ***********)
                            ALBW_PTR_BITS(((ALBW_AS_F32(local_1f04) * ALBW_AS_F32(*pppppppppppfVar9) +
                              ALBW_AS_F32(local_1f00[0]) * ALBW_AS_F32(ppppppppppfVar22[0x16]) +
                             ALBW_AS_F32(local_1f00[1]) * ALBW_AS_F32(ppppppppppfVar22[0x17])) -
                            ALBW_AS_F32(ppppppppppfVar22[0x1a]))),
                      ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar74))))) goto LAB_001fcf3c;
                }
              }
              else {
                pppppppppppfVar52 =
                     (float ***********)
                     ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(*pppppppppppfVar17) +
                      ALBW_AS_F32(ppppppppppfVar56) * ALBW_AS_F32(ppppppppppfVar22[0x13]) +
                     ALBW_AS_F32(ppppppppppfVar73) * ALBW_AS_F32(ppppppppppfVar22[0x14])));
                if (ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar52)) {
                  uVar36 = 2;
                  goto LAB_001fcb70;
                }
                pppppppppppfVar74 =
                     (float ***********)
                     ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(*pppppppppppfVar9) +
                       ALBW_AS_F32(ppppppppppfVar56) * ALBW_AS_F32(ppppppppppfVar22[0x16]) +
                      ALBW_AS_F32(ppppppppppfVar73) * ALBW_AS_F32(ppppppppppfVar22[0x17])) -
                     ALBW_AS_F32(ppppppppppfVar22[0x1a])));
                if (ALBW_AS_F32(pppppppppppfVar78) < ALBW_AS_F32(pppppppppppfVar74)) {
                  uVar36 = 4;
                  goto LAB_001fcb70;
                }
              }
              pppppppppppfVar34 = local_1f08;
              if (ALBW_AS_F32(pppppppppppfVar75) <= ALBW_AS_F32(pppppppppppfVar52)) {
                pppppppppppfVar33 = pppppppppppfVar17;
                pppppppppppfVar29 = pppppppppppfVar52;
                if ((ALBW_AS_F32(pppppppppppfVar75) <= ALBW_AS_F32(pppppppppppfVar74)) &&
                   (pppppppppppfVar33 = pppppppppppfVar9, pppppppppppfVar34 = pppppppppppfVar17,
                   pppppppppppfVar29 = pppppppppppfVar74, pppppppppppfVar75 = pppppppppppfVar52,
                   ALBW_AS_F32(pppppppppppfVar74) < ALBW_AS_F32(pppppppppppfVar52))) {
                  pppppppppppfVar33 = pppppppppppfVar17;
                  pppppppppppfVar34 = pppppppppppfVar9;
                  pppppppppppfVar29 = pppppppppppfVar52;
                  pppppppppppfVar75 = pppppppppppfVar74;
                }
              }
              else {
                pppppppppppfVar33 = pppppppppppfVar9;
                pppppppppppfVar29 = pppppppppppfVar74;
                if ((ALBW_AS_F32(pppppppppppfVar74) < ALBW_AS_F32(pppppppppppfVar75)) &&
                   (pppppppppppfVar33 = local_1f08, pppppppppppfVar34 = pppppppppppfVar17,
                   pppppppppppfVar29 = pppppppppppfVar75, pppppppppppfVar75 = pppppppppppfVar52,
                   ALBW_AS_F32(pppppppppppfVar52) <= ALBW_AS_F32(pppppppppppfVar74))) {
                  pppppppppppfVar34 = pppppppppppfVar9;
                  pppppppppppfVar75 = pppppppppppfVar74;
                }
              }
              if (ALBW_AS_F32(pppppppppppfVar50) < ALBW_AS_F32(pppppppppppfVar29)) {
                fVar58 = ALBW_AS_F32(*pppppppppppfVar33) * ALBW_AS_F32(*pppppppppppfVar34) +
                         ALBW_AS_F32(pppppppppppfVar33[1]) * ALBW_AS_F32(pppppppppppfVar34[1]) +
                         ALBW_AS_F32(pppppppppppfVar33[2]) * ALBW_AS_F32(pppppppppppfVar34[2]);
                pppppppppppfVar49 = local_1ebc;
                if (fVar58 * ALBW_AS_F32(pppppppppppfVar29) <= ALBW_AS_F32(pppppppppppfVar75)) {
                  fVar65 = fVar58 * fVar58 - DAT_001fd184;
                  if (0xb4000000 < (uint)ALBW_AS_U32(fVar65)) {
                    fVar65 = (fVar58 * ALBW_AS_F32(pppppppppppfVar75) - ALBW_AS_F32(pppppppppppfVar29)) / fVar65
                    ;
                    fVar58 = ALBW_AS_F32(pppppppppppfVar75) - fVar65 * fVar58;
                    func_00115D70((float *)(local_1f28 + 1),(float *)pppppppppppfVar33,fVar65);
                    local_1e40 = local_1f28[1];
                    local_1e3c = local_1f28[2];
                    local_1e38 = local_1f28[3];
                    func_00115D70((float *)&local_1d84,(float *)pppppppppppfVar34,fVar58);
                    local_1dcc = (float **********)local_1d84;
                    local_1dc8 = (float ***********)local_1d80;
                    local_1dc4 = (float ***********)local_1d7c;
                    func_00115C54(&local_1d90,(float *)&local_1e40,(float *)&local_1dcc);
                    fVar58 = local_1d90 * local_1d90 + local_1d8c * local_1d8c +
                             local_1d88 * local_1d88;
                    fVar65 = ALBW_AS_F32(pppppppppppfVar78) * ALBW_AS_F32(pppppppppppfVar78) - fVar58;
                    pppppppppppfVar49 = local_1ebc;
                    if ((ALBW_AS_F32(pppppppppppfVar50) < fVar65) && (fVar58 <= fVar57 * fVar57 * fVar60))
                    goto LAB_001fce84;
                  }
                }
                else if (ALBW_AS_F32(pppppppppppfVar29) <= fVar57) {
                  fVar65 = ALBW_AS_F32(pppppppppppfVar78) * ALBW_AS_F32(pppppppppppfVar78) -
                           ALBW_AS_F32(pppppppppppfVar29) * ALBW_AS_F32(pppppppppppfVar29);
LAB_001fce84:
                  pppppppppppfVar49 = local_1ebc;
                  if (fVar57 * fVar57 <= fVar65) {
                    fVar58 = SQRT(fVar65) - fVar57;
                    goto LAB_001fce9c;
                  }
                }
                goto LAB_001fcf3c;
              }
            }
LAB_001fce9c:
            pppppppppppfVar49 = local_1ebc;
            if (ALBW_AS_F32(pppppppppppfVar50) < fVar58) {
              func_00115D70((float *)&local_1d78,(float *)(ppppppppppfVar22 + 0xc),fVar58);
              if (ALBW_AS_F32(local_1d78) < ALBW_AS_F32(local_1ed0)) {
                local_1ed0 = (float ***********)local_1d78;
              }
              if (ALBW_AS_F32(local_1d74) < ALBW_AS_F32(local_1ecc)) {
                local_1ecc = local_1d74;
              }
              if (ALBW_AS_F32(local_1d70[0]) < ALBW_AS_F32(local_1ec8)) {
                local_1ec8 = local_1d70[0];
              }
              ppppppppppppfVar85 = local_1d78;
              if (ALBW_AS_F32(local_1d78) < ALBW_AS_F32(local_1ec4)) {
                ppppppppppppfVar85 = local_1ec4;
              }
              ppppppppppppfVar13 = local_1d74;
              if (ALBW_AS_F32(local_1d74) < ALBW_AS_F32(local_1ec0)) {
                ppppppppppppfVar13 = local_1ec0;
              }
              local_1ec4 = ppppppppppppfVar85;
              local_1ec0 = ppppppppppppfVar13;
              pppppppppppfVar49 = local_1d70[0];
              if (ALBW_AS_F32(local_1d70[0]) < ALBW_AS_F32(local_1ebc)) {
                pppppppppppfVar49 = local_1ebc;
              }
            }
          }
        }
      }
LAB_001fcf3c:
      local_1ebc = pppppppppppfVar49;
      pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
    }
    local_1eb0 = (float ***********)&local_1ec4;
    func_00115C54((float *)&stack0xffffe0d0,(float *)&local_1ed0,(float *)local_1eb0);
    if ((uint)ALBW_AS_U32((ALBW_AS_F32(sp_x) * ALBW_AS_F32(pppppppppppfVar15[0x18]) +
               ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(pppppppppppfVar15[0x19]) +
              ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(pppppppppppfVar15[0x1a]))) <= (uint)ALBW_AS_U32(DAT_001fc488)) {
      pppppppppfVar48 = (float *********)0x0;
      fVar58 = DAT_001fd184;
      while( true ) {
        ppppppppppfVar22 = pppppppppppfVar15[1];
        pppppppppfVar23 = *ppppppppppfVar22;
        if ((int)pppppppppfVar23 <= (int)pppppppppfVar48) break;
        if (pppppppppfVar48 < pppppppppfVar23) {
          pppppppppfVar24 = ppppppppppfVar22[2];
        }
        else {
          ppppppppppfVar22 = (float **********)0x0;
          pppppppppfVar24 = pppppppppfVar23;
        }
        if (pppppppppfVar48 < pppppppppfVar23) {
          ppppppppppfVar22 = (float **********)pppppppppfVar24[(int)pppppppppfVar48];
        }
        pppppppppppfVar52 = local_1ebc;
        if (ALBW_AS_F32(ppppppppppfVar22[0xc]) * ALBW_AS_F32(pppppppppppfVar15[0x18]) +
            ALBW_AS_F32(ppppppppppfVar22[0xd]) * ALBW_AS_F32(pppppppppppfVar15[0x19]) +
            ALBW_AS_F32(ppppppppppfVar22[0xe]) * ALBW_AS_F32(pppppppppppfVar15[0x1a]) < ALBW_AS_F32(pppppppppppfVar50)
           ) {
          local_1ef0 = &local_1ea0;
          local_1f10 = (float ***********)(ppppppppppfVar22 + 9);
          local_1ea0 = local_1e24;
          local_1e9c = local_1e20;
          local_1e98 = local_1e1c;
          local_1e94 = local_1dd8[2];
          fVar57 = fVar5;
          func_00115C70((float *)&local_1db4,(float *)local_1ef0,(float *)local_1f10);
          local_1f14 = (float ***********)(ppppppppppfVar22 + 0x12);
          pppppppppppfVar74 = (float ***********)(ppppppppppfVar22 + 0xf);
          pppppppppppfVar17 = (float ***********)(ppppppppppfVar22 + 0x15);
          pppppppppppfVar49 = local_1e94;
          pppppppppppfVar34 = local_1db4;
          local_1edc = pppppppppppfVar50;
          local_1ed8 = pppppppppppfVar50;
          local_1ed4 = pppppppppppfVar50;
          pppppppppppfVar78 = local_1db0;
          pppppppppppfVar9 = local_1dac;
          func_00115C70((float *)&local_1edc,(float *)local_1ef0,(float *)&local_1e5c);
          func_00115C70((float *)&local_1edc,(float *)&local_1edc,(float *)(ppppppppppfVar22 + 9));
          pppppppppfVar23 = ppppppppppfVar22[0xc];
          pppppppppppfVar52 = local_1ebc;
          if (ALBW_AS_F32(pppppppppppfVar50) <=
              ALBW_AS_F32(local_1edc) * ALBW_AS_F32(pppppppppfVar23) +
              ALBW_AS_F32(local_1ed8) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
              ALBW_AS_F32(local_1ed4) * ALBW_AS_F32(ppppppppppfVar22[0xe])) {
            pppppppppfVar24 = (float *********)ALBW_PTR_BITS((fVar57 + ALBW_AS_F32(local_1e94) * DAT_001fd188));
            if (ALBW_AS_F32(pppppppppfVar24) < ALBW_AS_F32(ppppppppppfVar22[0x18])) {
              pppppppppfVar24 = ppppppppppfVar22[0x18];
            }
            fVar60 = ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(pppppppppfVar23) +
                     ALBW_AS_F32(pppppppppppfVar78) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
                     ALBW_AS_F32(pppppppppppfVar9) * ALBW_AS_F32(ppppppppppfVar22[0xe]);
            fVar57 = ALBW_AS_F32(pppppppppppfVar49) - fVar60;
            if (fVar57 != ALBW_AS_F32(pppppppppppfVar50)) {
              if ((fVar57 < ALBW_AS_F32(pppppppppppfVar50)) || (ALBW_AS_F32(pppppppppfVar24) <= fVar57))
              goto LAB_001fd534;
              pppppppppppfVar75 =
                   (float ***********)
                   ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(*pppppppppppfVar74) +
                    ALBW_AS_F32(pppppppppppfVar78) * ALBW_AS_F32(ppppppppppfVar22[0x10]) +
                   ALBW_AS_F32(pppppppppppfVar9) * ALBW_AS_F32(ppppppppppfVar22[0x11])));
              fVar65 = DAT_001fd188;
              if (ALBW_AS_F32(pppppppppppfVar49) < ALBW_AS_F32(pppppppppppfVar75)) {
                uVar36 = 1;
code_r0x001fd180:
                func_00115C70((float *)&local_1e74,(float *)&local_1edc,(float *)local_1f10);
                func_00115C70((float *)&local_1e68,(float *)local_1ef0,(float *)local_1f10);
                pppppppppfVar23 = ppppppppppfVar22[0xc];
                fVar54 = ALBW_AS_F32(local_1e74) * ALBW_AS_F32(pppppppppfVar23) +
                         ALBW_AS_F32(local_1e70) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
                         ALBW_AS_F32(local_1e6c) * ALBW_AS_F32(ppppppppppfVar22[0xe]);
                if (fVar54 <= ALBW_AS_F32(pppppppppppfVar50)) {
                  fVar54 = -fVar54;
                }
                fVar59 = ALBW_AS_F32(local_1e68) * ALBW_AS_F32(pppppppppfVar23) +
                         ALBW_AS_F32(local_1e64) * ALBW_AS_F32(ppppppppppfVar22[0xd]) +
                         ALBW_AS_F32(local_1e60) * ALBW_AS_F32(ppppppppppfVar22[0xe]);
                if (fVar59 <= ALBW_AS_F32(pppppppppppfVar50)) {
                  fVar59 = -fVar59;
                }
                if (ALBW_AS_F32(pppppppppppfVar50) < fVar54 + fVar59) {
                  local_1e80 = local_1e5c;
                  local_1e7c = (float ***********)local_1e58;
                  local_1e78 = local_1e54;
                  func_00115D70((float *)&local_1e80,(float *)&local_1e80,
                                -(fVar59 / (fVar54 + fVar59)));
                  func_00115C54((float *)&local_1e80,(float *)&local_1e80,(float *)local_1ef0);
                  func_00115C70((float *)(local_1f28 + 2),(float *)&local_1e80,(float *)local_1f10);
                  pppppppppppfVar52 = local_1ebc;
                  if (((((uVar36 & 1) != 0) &&
                       (pppppppppppfVar75 =
                             (float ***********)
                             ALBW_PTR_BITS((ALBW_AS_F32(local_1f28[2]) * ALBW_AS_F32(*pppppppppppfVar74) +
                              ALBW_AS_F32(local_1f28[3]) * ALBW_AS_F32(ppppppppppfVar22[0x10]) +
                             ALBW_AS_F32(local_1f28[4]) * ALBW_AS_F32(ppppppppppfVar22[0x11]))),
                       ALBW_AS_F32(pppppppppppfVar49) < ALBW_AS_F32(pppppppppppfVar75))) ||
                      (((uVar36 & 3) != 0 &&
                       (pppppppppppfVar81 =
                             (float ***********)
                             ALBW_PTR_BITS((ALBW_AS_F32(local_1f28[2]) * ALBW_AS_F32(*local_1f14) +
                              ALBW_AS_F32(local_1f28[3]) * ALBW_AS_F32(local_1f14[1]) +
                             ALBW_AS_F32(local_1f28[4]) * ALBW_AS_F32(local_1f14[2]))),
                       ALBW_AS_F32(pppppppppppfVar49) < ALBW_AS_F32(pppppppppppfVar81))))) ||
                     ((uVar36 != 0 &&
                      (pppppppppppfVar53 =
                            (float ***********)
                            ALBW_PTR_BITS(((ALBW_AS_F32(local_1f28[2]) * ALBW_AS_F32(*pppppppppppfVar17) +
                              ALBW_AS_F32(local_1f28[3]) * ALBW_AS_F32(ppppppppppfVar22[0x16]) +
                             ALBW_AS_F32(local_1f28[4]) * ALBW_AS_F32(ppppppppppfVar22[0x17])) -
                            ALBW_AS_F32(ppppppppppfVar22[0x1a]))),
                      ALBW_AS_F32(pppppppppppfVar49) < ALBW_AS_F32(pppppppppppfVar53))))) goto LAB_001fd534;
                }
              }
              else {
                pppppppppppfVar81 =
                     (float ***********)
                     ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(*local_1f14) +
                      ALBW_AS_F32(pppppppppppfVar78) * ALBW_AS_F32(local_1f14[1]) +
                     ALBW_AS_F32(pppppppppppfVar9) * ALBW_AS_F32(local_1f14[2])));
                if (ALBW_AS_F32(pppppppppppfVar49) < ALBW_AS_F32(pppppppppppfVar81)) {
                  uVar36 = 2;
                  goto code_r0x001fd180;
                }
                pppppppppppfVar53 =
                     (float ***********)
                     ALBW_PTR_BITS(((ALBW_AS_F32(pppppppppppfVar34) * ALBW_AS_F32(*pppppppppppfVar17) +
                       ALBW_AS_F32(pppppppppppfVar78) * ALBW_AS_F32(ppppppppppfVar22[0x16]) +
                      ALBW_AS_F32(pppppppppppfVar9) * ALBW_AS_F32(ppppppppppfVar22[0x17])) -
                     ALBW_AS_F32(ppppppppppfVar22[0x1a])));
                if (ALBW_AS_F32(pppppppppppfVar49) < ALBW_AS_F32(pppppppppppfVar53)) {
                  uVar36 = 4;
                  goto code_r0x001fd180;
                }
              }
              if (ALBW_AS_F32(pppppppppppfVar75) <= ALBW_AS_F32(pppppppppppfVar81)) {
                pppppppppppfVar34 = local_1f14;
                pppppppppppfVar78 = pppppppppppfVar81;
                if ((ALBW_AS_F32(pppppppppppfVar75) <= ALBW_AS_F32(pppppppppppfVar53)) &&
                   (pppppppppppfVar74 = pppppppppppfVar17, pppppppppppfVar75 = pppppppppppfVar53,
                   ALBW_AS_F32(pppppppppppfVar81) <= ALBW_AS_F32(pppppppppppfVar53))) {
                  pppppppppppfVar34 = pppppppppppfVar17;
                  pppppppppppfVar74 = local_1f14;
                  pppppppppppfVar78 = pppppppppppfVar53;
                  pppppppppppfVar75 = pppppppppppfVar81;
                }
              }
              else {
                pppppppppppfVar34 = pppppppppppfVar17;
                pppppppppppfVar78 = pppppppppppfVar53;
                if ((ALBW_AS_F32(pppppppppppfVar53) < ALBW_AS_F32(pppppppppppfVar75)) &&
                   (pppppppppppfVar34 = pppppppppppfVar74, pppppppppppfVar74 = local_1f14,
                   pppppppppppfVar78 = pppppppppppfVar75, pppppppppppfVar75 = pppppppppppfVar81,
                   ALBW_AS_F32(pppppppppppfVar81) <= ALBW_AS_F32(pppppppppppfVar53))) {
                  pppppppppppfVar74 = pppppppppppfVar17;
                  pppppppppppfVar75 = pppppppppppfVar53;
                }
              }
              if (ALBW_AS_F32(pppppppppppfVar50) < ALBW_AS_F32(pppppppppppfVar78)) {
                fVar57 = ALBW_AS_F32(*pppppppppppfVar34) * ALBW_AS_F32(*pppppppppppfVar74) +
                         ALBW_AS_F32(pppppppppppfVar34[1]) * ALBW_AS_F32(pppppppppppfVar74[1]) +
                         ALBW_AS_F32(pppppppppppfVar34[2]) * ALBW_AS_F32(pppppppppppfVar74[2]);
                pppppppppppfVar52 = local_1ebc;
                if (fVar57 * ALBW_AS_F32(pppppppppppfVar78) <= ALBW_AS_F32(pppppppppppfVar75)) {
                  fVar54 = fVar57 * fVar57 - fVar58;
                  if (0xb4000000 < (uint)ALBW_AS_U32(fVar54)) {
                    fVar54 = (fVar57 * ALBW_AS_F32(pppppppppppfVar75) - ALBW_AS_F32(pppppppppppfVar78)) / fVar54
                    ;
                    fVar57 = ALBW_AS_F32(pppppppppppfVar75) - fVar54 * fVar57;
                    func_00115D70((float *)(local_1f28 + 2),(float *)pppppppppppfVar34,fVar54);
                    local_1d9c = local_1f28[2];
                    local_1d98 = local_1f28[3];
                    local_1d94 = (float ************)local_1f28[4];
                    func_00115D70((float *)(local_1f28 + 1),(float *)pppppppppppfVar74,fVar57);
                    local_1da8 = local_1f28[1];
                    local_1da4 = local_1f28[2];
                    local_1da0 = local_1f28[3];
                    func_00115C54((float *)&stack0xffffe0d0,(float *)&local_1d9c,
                                  (float *)&local_1da8);
                    fVar54 = ALBW_AS_F32(sp_x) * ALBW_AS_F32(sp_x) + ALBW_AS_F32(in_stack_ffffe0d4) * ALBW_AS_F32(in_stack_ffffe0d4) +
                             ALBW_AS_F32(local_1f28[0]) * ALBW_AS_F32(local_1f28[0]);
                    fVar57 = ALBW_AS_F32(pppppppppppfVar49) * ALBW_AS_F32(pppppppppppfVar49) - fVar54;
                    pppppppppppfVar52 = local_1ebc;
                    if ((ALBW_AS_F32(pppppppppppfVar50) < fVar57) && (fVar54 <= fVar60 * fVar60 * fVar65))
                    goto LAB_001fd47c;
                  }
                }
                else if (ALBW_AS_F32(pppppppppppfVar78) <= fVar60) {
                  fVar57 = ALBW_AS_F32(pppppppppppfVar49) * ALBW_AS_F32(pppppppppppfVar49) -
                           ALBW_AS_F32(pppppppppppfVar78) * ALBW_AS_F32(pppppppppppfVar78);
LAB_001fd47c:
                  pppppppppppfVar52 = local_1ebc;
                  if (fVar60 * fVar60 <= fVar57) {
                    fVar57 = SQRT(fVar57) - fVar60;
                    goto LAB_001fd494;
                  }
                }
                goto LAB_001fd534;
              }
            }
LAB_001fd494:
            pppppppppppfVar52 = local_1ebc;
            if (ALBW_AS_F32(pppppppppppfVar50) < fVar57) {
              func_00115D70((float *)local_1f28,(float *)(ppppppppppfVar22 + 0xc),fVar57);
              if (ALBW_AS_F32(local_1f28[0]) < ALBW_AS_F32(local_1ed0)) {
                local_1ed0 = local_1f28[0];
              }
              if (ALBW_AS_F32(local_1f28[1]) < ALBW_AS_F32(local_1ecc)) {
                local_1ecc = (float ************)local_1f28[1];
              }
              if (ALBW_AS_F32(local_1f28[2]) < ALBW_AS_F32(local_1ec8)) {
                local_1ec8 = local_1f28[2];
              }
              ppppppppppppfVar85 = (float ************)local_1f28[0];
              if (ALBW_AS_F32(local_1f28[0]) < ALBW_AS_F32(local_1ec4)) {
                ppppppppppppfVar85 = local_1ec4;
              }
              ppppppppppppfVar13 = (float ************)local_1f28[1];
              if (ALBW_AS_F32(local_1f28[1]) < ALBW_AS_F32(local_1ec0)) {
                ppppppppppppfVar13 = local_1ec0;
              }
              local_1ec4 = ppppppppppppfVar85;
              local_1ec0 = ppppppppppppfVar13;
              pppppppppppfVar52 = local_1f28[2];
              if (ALBW_AS_F32(local_1f28[2]) < ALBW_AS_F32(local_1ebc)) {
                pppppppppppfVar52 = local_1ebc;
              }
            }
          }
        }
LAB_001fd534:
        local_1ebc = pppppppppppfVar52;
        pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1);
      }
      func_00115C54((float *)(local_1f28 + 1),(float *)&local_1ed0,(float *)local_1eb0);
      if ((uint)ALBW_AS_U32((ALBW_AS_F32(local_1f28[1]) * ALBW_AS_F32(pppppppppppfVar15[0x18]) +
                 ALBW_AS_F32(local_1f28[2]) * ALBW_AS_F32(pppppppppppfVar15[0x19]) +
                ALBW_AS_F32(local_1f28[3]) * ALBW_AS_F32(pppppppppppfVar15[0x1a]))) <= (uint)ALBW_AS_U32(DAT_001fdb78))
      goto LAB_001fd59c;
    }
    uVar42 = (ushort)local_1eb8 & 0x1f | 0x1e0;
  }
LAB_001fd59c:
  *(ushort *)(local_1e50 + 0x2b) = uVar42;
  fVar5 = local_2c[0x1a];
  if (fVar5 != 0.0) {
    uVar36 = 1 - *(int *)((int)ALBW_AS_U32(fVar5) + 0x1cc);
    *(uint *)((int)ALBW_AS_U32(fVar5) + 0x1cc) = uVar36;
    iVar21 = (int)ALBW_AS_U32(fVar5) + 0x17c;
    if (uVar36 < 2) {
      iVar21 = (int)ALBW_AS_U32(fVar5) + 0x17c + uVar36 * 0x28;
    }
    func_002C1720(iVar21,*(uint **)((int)ALBW_AS_U32(fVar5) + 0x178),0);
    piVar6 = DAT_001fdb7c;
    local_1e30 = *(float ************)((int)ALBW_AS_U32(fVar5) + 0x178);
    *local_1e30 = (float **********)0x0;
    puVar4 = DAT_001fdb80;
    local_1e30[3] = (float **********)0x0;
    local_1e30[6] = (float **********)0x0;
    *(undefined4 *)(*piVar6 + 0x28) = 0;
    for (pppppppppfVar48 = (float *********)0x0; pppppppppppfVar52 = local_1e30,
        (int)pppppppppfVar48 < (int)**local_30;
        pppppppppfVar48 = (float *********)((int)pppppppppfVar48 + 1)) {
      iVar21 = *piVar6;
      iVar35 = *(int *)(iVar21 + 0x28);
      if (iVar35 < *(int *)(iVar21 + 0x2c)) {
        *(int *)(iVar21 + 0x28) = iVar35 + 1;
        pfVar39 = (float *)(*(int *)(iVar21 + 0x30) + iVar35 * 0x148);
      }
      else {
        pfVar39 = (float *)0x0;
      }
      if (pppppppppfVar48 < **local_30) {
        ppppppppfVar14 = (*local_30)[2][(int)pppppppppfVar48];
      }
      else {
        ppppppppfVar14 = (float ********)0x0;
      }
      *pfVar39 = ALBW_AS_F32(ppppppppfVar14[2]);
      pfVar39[1] = ALBW_AS_F32(ppppppppfVar14[3]);
      func_0010E3C4((uint *)(pfVar39 + 7),(uint *)(ppppppppfVar14 + 9),0x48);
      pfVar39[0x1a] = pfVar39[7];
      pfVar8 = pfVar39 + 7;
      pfVar39[0x1b] = pfVar39[8];
      pfVar39[0x1c] = pfVar39[9];
      fVar57 = pfVar39[0xc] * pfVar39[0xe] - pfVar39[0xb] * pfVar39[0xf];
      fVar5 = pfVar39[0xb] * pfVar39[0xd] - pfVar39[10] * pfVar39[0xe];
      fVar55 = pfVar39[10] * pfVar39[0xf] - pfVar39[0xc] * pfVar39[0xd];
      fVar58 = fVar57 * pfVar39[0x13] + fVar55 * pfVar39[0x14] + fVar5 * pfVar39[0x15];
      if (fVar58 <= ALBW_AS_F32(pppppppppppfVar50)) {
        pfVar39[0x1d] = *pfVar8;
        pfVar39[0x1e] = pfVar39[8];
        pfVar39[0x1f] = pfVar39[9];
      }
      else {
        fVar58 = pfVar39[0x18] / fVar58;
        pfVar39[0x1d] = *pfVar8 + fVar57 * fVar58;
        pfVar39[0x1e] = pfVar39[8] + fVar55 * fVar58;
        pfVar39[0x1f] = pfVar39[9] + fVar5 * fVar58;
      }
      fVar57 = pfVar39[0x12] * pfVar39[0xb] - pfVar39[0x11] * pfVar39[0xc];
      fVar58 = pfVar39[0x10] * pfVar39[0xc] - pfVar39[0x12] * pfVar39[10];
      fVar55 = pfVar39[0x11] * pfVar39[10] - pfVar39[0x10] * pfVar39[0xb];
      fVar5 = fVar57 * pfVar39[0x13] + fVar58 * pfVar39[0x14] + fVar55 * pfVar39[0x15];
      if (fVar5 <= ALBW_AS_F32(pppppppppppfVar50)) {
        pfVar39[0x20] = *pfVar8;
        pfVar39[0x21] = pfVar39[8];
        pfVar39[0x22] = pfVar39[9];
      }
      else {
        fVar5 = pfVar39[0x18] / fVar5;
        pfVar39[0x20] = *pfVar8 + fVar57 * fVar5;
        pfVar39[0x21] = pfVar39[8] + fVar58 * fVar5;
        pfVar39[0x22] = pfVar39[9] + fVar55 * fVar5;
      }
      if (((*DAT_001fdb84 & 1) == 0) &&
         (iVar21 = func_00100174((int *)DAT_001fdb84), puVar26 = DAT_001fdb88, iVar21 != 0)) {
        uVar36 = puVar4[1];
        puVar27 = DAT_001fdb88 + 2;
        *DAT_001fdb88 = *puVar4;
        puVar26[1] = uVar36;
        uVar36 = puVar4[1];
        *puVar27 = *puVar4;
        puVar26[3] = uVar36;
        uVar36 = puVar4[1];
        puVar26[4] = *puVar4;
        puVar26[5] = uVar36;
      }
      func_0010E3C4((uint *)(pfVar39 + 0x23),DAT_001fdb88,0x18);
      pfVar39[0x29] = 0.0;
    }
    uVar70 = (ulonglong)DAT_001fdb8c;
    local_1e60 = local_30;
    local_1e28 = (uint *)(*piVar6 + 0x28);
    if (local_30[0x29] != (float **********)0x0) {
      fVar5 = ALBW_AS_F32(local_30[0x1c]) * local_2c[0x1c];
      uVar36 = DAT_001fdb8c;
      func_00100118((int)&local_1f10,DAT_001fdb90,0x18,6);
      uVar70 = CONCAT44(DAT_001fdb94,uVar36);
      pppppppppppfVar15 = local_1e60 + 0xc;
      func_00115D70((float *)local_1f28,DAT_001fdb98,DAT_001fdb94);
      local_1e3c = local_1f28[0];
      local_1e38 = local_1f28[1];
      local_1e34 = local_1f28[2];
      func_00115C70((float *)local_1f28,(float *)pppppppppppfVar15,(float *)&local_1e3c);
      pfVar39 = DAT_001fdb98;
      local_1ebc = (float ***********)ALBW_PTR_BITS(*DAT_001fdb98);
      local_1eb8 = (float ***********)ALBW_PTR_BITS(DAT_001fdb98[1]);
      local_1eb4 = (float ************)ALBW_PTR_BITS(DAT_001fdb98[2]);
      local_1ec8 = local_1f28[0];
      local_1ec4 = (float ************)local_1f28[1];
      local_1ec0 = (float ************)local_1f28[2];
      pppppppppppfVar15 = local_1e60 + 0xc;
      func_00115D70((float *)&stack0xffffe0d4,DAT_001fdb98,fVar5);
      local_1e38 = local_1f28[0];
      local_1e34 = local_1f28[1];
      local_1e3c = in_stack_ffffe0d4;
      func_00115C54((float *)&stack0xffffe0d4,(float *)pppppppppppfVar15,(float *)&local_1e3c);
      local_1ed4 = (float ***********)ALBW_PTR_BITS(-*pfVar39);
      local_1ed0 = (float ***********)ALBW_PTR_BITS(-pfVar39[1]);
      local_1ecc = (float ************)ALBW_PTR_BITS(-pfVar39[2]);
      local_1edc = local_1f28[0];
      local_1ed8 = local_1f28[1];
      local_1ee0 = (float ************)in_stack_ffffe0d4;
      if (((*DAT_001fdb9c & 1) == 0) &&
         (iVar21 = func_00100174((int *)DAT_001fdb9c), piVar6 = DAT_001fdba0, iVar21 != 0)) {
        piVar25 = DAT_001fdba0 + 3;
        DAT_001fdba0[1] = 0x20;
        piVar6[2] = (int)piVar25;
        *piVar6 = 0;
      }
      fVar55 = DAT_001fdba4;
      uVar67 = CONCAT44(pppppppppppfVar83,DAT_001fdba4);
      *DAT_001fdba0 = 0;
      fVar55 = ALBW_AS_F32(local_1edc) - fVar55;
      pppppppppppfVar15 = (float ***********)0x0;
      local_1e58 = (float ************)0x0;
      pppppppppppfVar81 = pppppppppppfVar50;
      pppppppppppfVar83 = pppppppppppfVar50;
      pppppppppppfVar53 = pppppppppppfVar50;
      if (0 < (int)local_1e60[0x29]) {
        do {
          if (local_1e58 < local_1e60[0x29]) {
            ppppppppppfVar22 = local_1e60[0x2b] + (int)local_1e58 * 0x27;
          }
          else {
            ppppppppppfVar22 = local_1e60[0x2b];
          }
          if (*(char *)((int)ppppppppppfVar22 + 0x92) == '\0') {
            local_1f10 = (float ***********)ppppppppppfVar22[0x12];
            local_1f0c = (float ***********)ppppppppppfVar22[0x13];
            local_1f08 = (float ***********)ppppppppppfVar22[0x14];
            local_1f04 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(*ppppppppppfVar22));
            local_1f00[0] = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar22[1]));
            local_1f00[1] = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar22[2]));
          }
          else {
            if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
              func_00115C70((float *)&local_1e0c,(float *)(ppppppppppfVar22 + 0x18),
                            (float *)(ppppppppppfVar22 + 0x12));
              local_1e7c = local_1e0c;
              local_1e78 = local_1e08;
              local_1e74 = local_1e04;
              func_00115E10((float *)&local_1e6c,(float *)&local_1e7c,
                            (float *)(ppppppppppfVar22 + 6));
            }
            else {
              func_00115C70((float *)&local_1e18,(float *)(ppppppppppfVar22 + 0x18),
                            (float *)(ppppppppppfVar22 + 0x12));
              local_1e24 = local_1e18;
              local_1e20 = (float ***********)local_1e14;
              local_1e1c = (float ***********)local_1e10;
              func_00115E10((float *)&local_1e6c,(float *)(ppppppppppfVar22 + 6),
                            (float *)&local_1e24);
            }
            func_00115DC8((float *)&local_1e6c);
            local_1f10 = (float ***********)ppppppppppfVar22[0x12];
            local_1f0c = (float ***********)ppppppppppfVar22[0x13];
            local_1f08 = (float ***********)ppppppppppfVar22[0x14];
            local_1f04 = local_1e6c;
            local_1f00[0] = local_1e68;
            local_1f00[1] = local_1e64;
          }
          if (*(char *)((int)ppppppppppfVar22 + 0x91) == '\0') {
            local_1ee4 = (float ***********)ppppppppppfVar22[2];
            local_1ef4 = (float **********)ppppppppppfVar22[0x10];
            local_1ef0 = (float ************)ppppppppppfVar22[0x11];
            local_1f00[2] = (float ***********)ppppppppppfVar22[0xf];
            local_1ee8 = (float ************)ppppppppppfVar22[1];
            local_1eec = (float ***********)*ppppppppppfVar22;
          }
          else {
            if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
              func_00115C70((float *)&local_1e00,(float *)(ppppppppppfVar22 + 0x15),
                            (float *)(ppppppppppfVar22 + 0xf));
              local_1f28[3] = local_1e00;
              local_1f28[4] = local_1dfc;
              local_1f14 = local_1df8;
              func_00115E10((float *)&local_1e48,(float *)(ppppppppppfVar22 + 6),
                            (float *)(local_1f28 + 3));
            }
            else {
              func_00115C70((float *)&local_1df4,(float *)(ppppppppppfVar22 + 0x15),
                            (float *)(ppppppppppfVar22 + 0xf));
              local_1e6c = local_1df4;
              local_1e68 = local_1df0;
              local_1e64 = local_1dec;
              func_00115E10((float *)&local_1e48,(float *)&local_1e6c,
                            (float *)(ppppppppppfVar22 + 6));
            }
            func_00115DC8((float *)&local_1e48);
            local_1f00[2] = (float ***********)ppppppppppfVar22[0xf];
            local_1ef4 = (float **********)ppppppppppfVar22[0x10];
            local_1ef0 = (float ************)ppppppppppfVar22[0x11];
            local_1eec = local_1e48;
            local_1ee8 = (float ************)local_1e44;
            local_1ee4 = local_1e40;
          }
          fVar58 = ALBW_AS_F32(uVar70);
          func_00115D70((float *)&local_1e7c,(float *)(ppppppppppfVar22 + 6),fVar58);
          local_1e54 = local_1e7c;
          local_1e50 = local_1e78;
          local_1e4c = local_1e74;
          func_00115C54((float *)(local_1f28 + 3),(float *)(ppppppppppfVar22 + 0x12),
                        (float *)&local_1e54);
          local_1ea4 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar22[6]));
          local_1ea0 = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar22[7]));
          local_1e9c = (float ***********)ALBW_PTR_BITS(-ALBW_AS_F32(ppppppppppfVar22[8]));
          local_1eb0 = local_1f28[3];
          local_1eac = (float ************)local_1f28[4];
          local_1ea8 = local_1f14;
          func_00115D70((float *)&local_1e7c,(float *)(ppppppppppfVar22 + 6),fVar58);
          local_1e54 = local_1e7c;
          local_1e50 = local_1e78;
          local_1e4c = local_1e74;
          func_00115C70((float *)(local_1f28 + 3),(float *)(ppppppppppfVar22 + 0xf),
                        (float *)&local_1e54);
          local_1e8c = (float ***********)ppppppppppfVar22[6];
          local_1e88 = (float ***********)ppppppppppfVar22[7];
          local_1e84 = (float ***********)ppppppppppfVar22[8];
          local_1e98 = local_1f28[3];
          local_1e94 = local_1f28[4];
          local_1e90 = local_1f14;
          local_1e6c = (float ***********)*pppppppppppfVar52;
          iVar21 = *DAT_001fdba0;
          if (iVar21 < DAT_001fdba0[1]) {
            piVar6 = DAT_001fdba0 + 2;
            *DAT_001fdba0 = iVar21 + 1;
            *(float ************)(*piVar6 + iVar21 * 4) = local_1e6c;
          }
          uVar36 = 0;
          if (0 < (int)*local_1e28) {
            fVar58 = ALBW_AS_F32(uVar67);
            do {
              uVar16 = local_1e28[2];
              if (uVar36 < *local_1e28) {
                uVar16 = uVar16 + uVar36 * 0x148;
              }
              func_00570D00(uVar16,(int *)pppppppppppfVar52,(int)&local_1f10,6,0,fVar58);
              uVar36 = uVar36 + 1;
            } while ((int)uVar36 < (int)*local_1e28);
          }
          func_004324A4((uint *)pppppppppppfVar52,(float *)(ppppppppppfVar22 + 6),(uint)local_1e6c,
                        DAT_001fe138);
          pppppppppfVar48 =
               (float *********)ALBW_PTR_BITS((ALBW_AS_F32(ppppppppppfVar22[0x1d]) - ALBW_AS_F32(ppppppppppfVar22[0x1c])));
          bVar47 = false;
          bVar44 = false;
          pppppppppppfVar15 = (float ***********)((ulonglong)uVar67 >> 0x20);
          ppppppppppfVar22[0x21] = pppppppppfVar48;
          ppppppppppfVar22[0x20] = pppppppppfVar48;
          ppppppppppfVar22[0x23] = pppppppppfVar48;
          ppppppppppfVar22[0x22] = pppppppppfVar48;
          ppppppppppfVar22[0x1e] = (float *********)pppppppppppfVar83;
          ppppppppppfVar22[0x1c] = (float *********)pppppppppppfVar81;
          pppppppppppfVar17 = (float ***********)*pppppppppppfVar52;
          bVar46 = local_1e6c <= pppppppppppfVar17;
          bVar45 = pppppppppppfVar17 == local_1e6c;
          pppppppppppfVar49 = local_1e6c;
          pppppppppppfVar34 = pppppppppppfVar79;
          pppppppppppfVar78 = pppppppppppfVar79;
          pppppppppppfVar74 = pppppppppppfVar15;
          pppppppppppfVar9 = pppppppppppfVar79;
          if ((int)local_1e6c < (int)pppppppppppfVar17) {
            do {
              if (!bVar46 || bVar45) {
                uVar36 = (uint)pppppppppppfVar52[2];
              }
              else {
                uVar36 = (int)pppppppppppfVar52[2] + (int)pppppppppppfVar49 * 0x24;
              }
              iVar21 = 0;
              do {
                pfVar39 = (float *)(uVar36 + iVar21 * 0xc);
                if (fVar55 <= pfVar39[1]) {
                  bVar47 = true;
                  if (*(char *)((int)ppppppppppfVar22 + 0x91) == '\0') {
                    ppppppppppfVar56 = ppppppppppfVar22 + 0xf;
                  }
                  else {
                    ppppppppppfVar56 = ppppppppppfVar22 + 0x15;
                  }
                  func_00115C70((float *)&local_1e48,pfVar39,(float *)ppppppppppfVar56);
                  pppppppppppfVar17 =
                       (float ***********)
                       ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1e48) +
                        ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1e44) +
                       ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1e40)));
                  if (ALBW_AS_F32(pppppppppppfVar15) < ALBW_AS_F32(pppppppppppfVar17)) {
                    ppppppppppfVar22[0x21] = (float *********)pppppppppppfVar17;
                    pppppppppppfVar15 = pppppppppppfVar17;
                  }
                  if (ALBW_AS_F32(pppppppppppfVar17) < ALBW_AS_F32(pppppppppppfVar79)) {
                    ppppppppppfVar22[0x20] = (float *********)pppppppppppfVar17;
                    pppppppppppfVar79 = pppppppppppfVar17;
                  }
                }
                iVar21 = iVar21 + 1;
                if (pfVar39[1] < ALBW_AS_F32(pppppppppppfVar34)) {
                  pppppppppppfVar34 = (float ***********)ALBW_PTR_BITS(pfVar39[1]);
                }
              } while (iVar21 < 3);
              pppppppppppfVar17 = (float ***********)*pppppppppppfVar52;
              pppppppppppfVar49 = (float ***********)((int)pppppppppppfVar49 + 1);
              bVar46 = pppppppppppfVar49 <= pppppppppppfVar17;
              bVar45 = pppppppppppfVar17 == pppppppppppfVar49;
            } while (!bVar45 && (int)pppppppppppfVar49 <= (int)pppppppppppfVar17);
          }
          fVar58 = ALBW_AS_F32(pppppppppppfVar15) - ALBW_AS_F32(pppppppppppfVar79);
          bVar45 = DAT_001fe13c == fVar58;
          bVar46 = fVar58 <= DAT_001fe13c;
          if (!bVar46 || bVar45) {
            bVar45 = fVar58 == ALBW_AS_F32(uVar67);
            bVar46 = ALBW_AS_F32(uVar67) <= fVar58;
          }
          if (!bVar46 || bVar45) {
            bVar47 = false;
LAB_001fdee4:
            ppppppppppfVar22[0x20] = (float *********)pppppppppppfVar50;
            ppppppppppfVar22[0x21] = (float *********)pppppppppppfVar50;
            ppppppppppfVar22[0x1f] = ppppppppppfVar22[0x1e];
          }
          else {
            if (!bVar47) goto LAB_001fdee4;
            pppppppppppfVar83 = (float ***********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar83) + fVar58));
            ppppppppppfVar22[0x1f] = (float *********)pppppppppppfVar83;
          }
          *(bool *)((int)ppppppppppfVar22 + 0x96) = bVar47;
          fVar58 = ALBW_AS_F32(local_1ec4) - ALBW_AS_F32(pppppppppppfVar34);
          if (fVar58 <= ALBW_AS_F32(pppppppppppfVar50)) {
            fVar58 = -fVar58;
          }
          pppppppppppfVar79 = pppppppppppfVar9;
          if ((int)ALBW_AS_U32(fVar58) < DAT_001fe140) {
            pppppppppppfVar15 = (float ***********)*pppppppppppfVar52;
            bVar45 = local_1e6c <= pppppppppppfVar15;
            bVar47 = pppppppppppfVar15 == local_1e6c;
            fVar58 = ALBW_AS_F32(pppppppppppfVar34) + ALBW_AS_F32((uVar70 >> 0x20));
            if ((int)pppppppppppfVar15 <= (int)local_1e6c) goto LAB_001fdff0;
            pppppppppppfVar15 = local_1e6c;
            do {
              if (!bVar45 || bVar47) {
                uVar36 = (uint)pppppppppppfVar52[2];
              }
              else {
                uVar36 = (int)pppppppppppfVar52[2] + (int)pppppppppppfVar15 * 0x24;
              }
              iVar21 = 0;
              do {
                pfVar39 = (float *)(uVar36 + iVar21 * 0xc);
                if (pfVar39[1] <= fVar58) {
                  bVar44 = true;
                  if (*(char *)((int)ppppppppppfVar22 + 0x91) == '\0') {
                    ppppppppppfVar56 = ppppppppppfVar22 + 0xf;
                  }
                  else {
                    ppppppppppfVar56 = ppppppppppfVar22 + 0x15;
                  }
                  func_00115C70((float *)&local_1e48,pfVar39,(float *)ppppppppppfVar56);
                  pppppppppppfVar49 =
                       (float ***********)
                       ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1e48) +
                        ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1e44) +
                       ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1e40)));
                  if (ALBW_AS_F32(pppppppppppfVar74) < ALBW_AS_F32(pppppppppppfVar49)) {
                    ppppppppppfVar22[0x23] = (float *********)pppppppppppfVar49;
                    pppppppppppfVar74 = pppppppppppfVar49;
                  }
                  if (ALBW_AS_F32(pppppppppppfVar49) < ALBW_AS_F32(pppppppppppfVar78)) {
                    ppppppppppfVar22[0x22] = (float *********)pppppppppppfVar49;
                    pppppppppppfVar78 = pppppppppppfVar49;
                  }
                }
                iVar21 = iVar21 + 1;
              } while (iVar21 < 3);
              pppppppppppfVar49 = (float ***********)*pppppppppppfVar52;
              pppppppppppfVar15 = (float ***********)((int)pppppppppppfVar15 + 1);
              bVar45 = pppppppppppfVar15 <= pppppppppppfVar49;
              bVar47 = pppppppppppfVar49 == pppppppppppfVar15;
            } while (!bVar47 && (int)pppppppppppfVar15 <= (int)pppppppppppfVar49);
            if (!bVar44) goto LAB_001fdff0;
            pppppppppppfVar81 =
                 (float ***********)
                 ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar81) + (ALBW_AS_F32(pppppppppppfVar74) - ALBW_AS_F32(pppppppppppfVar78))));
            ppppppppppfVar22[0x1d] = (float *********)pppppppppppfVar81;
          }
          else {
LAB_001fdff0:
            if (*(char *)((int)ppppppppppfVar22 + 0x97) == '\0') {
              if (*(char *)((int)ppppppppppfVar22 + 0x91) == '\0') {
                ppppppppppfVar56 = ppppppppppfVar22 + 0xf;
              }
              else {
                ppppppppppfVar56 = ppppppppppfVar22 + 0x15;
              }
              func_00115C70((float *)&local_1e7c,(float *)(ppppppppppfVar22 + 0x12),
                            (float *)ppppppppppfVar56);
              pppppppppfVar48 =
                   (float *********)
                   ALBW_PTR_BITS((ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1e7c) +
                    ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1e78) +
                   ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1e74)));
              ppppppppppfVar22[0x23] = pppppppppfVar48;
              ppppppppppfVar22[0x22] = (float *********)pppppppppppfVar50;
              ppppppppppfVar22[0x1d] =
                   (float *********)ALBW_PTR_BITS((ALBW_AS_F32(pppppppppppfVar81) + ALBW_AS_F32(pppppppppfVar48)));
            }
            else {
              ppppppppppfVar22[0x1d] = (float *********)pppppppppppfVar81;
              ppppppppppfVar22[0x22] = (float *********)pppppppppppfVar50;
              ppppppppppfVar22[0x23] = (float *********)pppppppppppfVar50;
            }
          }
          pppppppppppfVar15 = (float ***********)local_1e60[0x29];
          local_1e58 = (float ************)((int)local_1e58 + 1);
        } while ((int)local_1e58 < (int)pppppppppppfVar15);
      }
      lVar69 = ZEXT48(pppppppppppfVar53) << 0x20;
      if (ALBW_AS_F32(pppppppppppfVar81) <= ALBW_AS_F32(pppppppppppfVar53)) {
        pppppppppppfVar15 = local_1e60;
      }
      if (ALBW_AS_F32(pppppppppppfVar81) <= ALBW_AS_F32(pppppppppppfVar53)) {
        pppppppppppfVar81 = (float ***********)pppppppppppfVar15[0x1b];
      }
      if (ALBW_AS_F32(pppppppppppfVar83) <= ALBW_AS_F32(pppppppppppfVar53)) {
        pppppppppppfVar15 = local_1e60;
      }
      fVar55 = ALBW_AS_F32((uVar70 >> 0x20));
      uVar68 = (ulonglong)(uint)ALBW_AS_U32(fVar55);
      if (ALBW_AS_F32(pppppppppppfVar83) <= ALBW_AS_F32(pppppppppppfVar53)) {
        pppppppppppfVar83 = (float ***********)pppppppppppfVar15[0x1b];
      }
      fVar57 = DAT_001fe148 / ALBW_AS_F32(pppppppppppfVar83);
      fVar58 = DAT_001fe144 - local_2c[0x17] * DAT_001fe144;
      lVar82 = (ulonglong)(uint)ALBW_AS_U32((DAT_001fe148 / ALBW_AS_F32(pppppppppppfVar81))) << 0x20;
      local_1e50 = (float ***********)0x0;
      uVar67 = CONCAT44(DAT_001fe148 / (fVar5 + fVar55),
                        (DAT_001fe144 + local_2c[0x16] * DAT_001fe144) - fVar58);
      if (0 < (int)local_1e60[0x29]) {
        do {
          if (local_1e50 < local_1e60[0x29]) {
            ppppppppppfVar22 = local_1e60[0x2b] + (int)local_1e50 * 0x27;
          }
          else {
            ppppppppppfVar22 = local_1e60[0x2b];
          }
          if ((float ***********)((int)local_1e60[0x29] + -1) == local_1e50) {
            local_1e64 = (float ***********)*pppppppppppfVar52;
          }
          else {
            puVar18 = (undefined4 *)DAT_001fdba0[2];
            if ((char *)((int)local_1e50 + 1) < (char *)*DAT_001fdba0) {
              puVar18 = puVar18 + (int)local_1e50 + 1;
            }
            local_1e64 = (float ***********)*puVar18;
          }
          fVar55 = ALBW_AS_F32(((ulonglong)lVar82 >> 0x20));
          ppppppppppfVar22[0x1c] = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(ppppppppppfVar22[0x1c]) * fVar55));
          ppppppppppfVar22[0x1d] = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(ppppppppppfVar22[0x1d]) * fVar55));
          ppppppppppfVar22[0x1e] = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(ppppppppppfVar22[0x1e]) * fVar57));
          ppppppppppfVar22[0x1f] = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(ppppppppppfVar22[0x1f]) * fVar57));
          uVar80 = (undefined4)((ulonglong)lVar69 >> 0x20);
          lVar69 = CONCAT44(uVar80,local_1ec4);
          pppppppppfVar48 = (float *********)ALBW_PTR_BITS((ALBW_AS_F32(local_1ec4) + fVar5 + ALBW_AS_F32(uVar68)));
          if (*(char *)((int)ppppppppppfVar22 + 0x96) == '\0') {
            if (*(char *)((int)ppppppppppfVar22 + 0x93) == '\0') {
              pppppppppfVar48 = ppppppppppfVar22[0x10];
              pppppppppfVar23 = ppppppppppfVar22[0x13];
              if (ALBW_AS_F32(ppppppppppfVar22[0x13]) < ALBW_AS_F32(pppppppppfVar48)) goto LAB_001fe4b8;
            }
            else {
              pppppppppfVar48 = ppppppppppfVar22[0x19];
              pppppppppfVar23 = ppppppppppfVar22[0x16];
              if (ALBW_AS_F32(ppppppppppfVar22[0x16]) < ALBW_AS_F32(pppppppppfVar48)) {
LAB_001fe4b8:
                pppppppppfVar48 = pppppppppfVar23;
              }
            }
          }
          else if (*(char *)((int)ppppppppppfVar22 + 0x97) != '\0') {
            lVar69 = CONCAT44(uVar80,ppppppppppfVar22[0x16]);
          }
          puVar4 = (uint *)DAT_001fdba0[2];
          if (local_1e50 < (float ***********)*DAT_001fdba0) {
            puVar4 = puVar4 + (int)local_1e50;
          }
          uVar36 = *puVar4;
          if ((int)uVar36 < (int)local_1e64) {
            fVar55 = ALBW_AS_F32(pppppppppfVar48) - ALBW_AS_F32(lVar69);
            do {
              if (uVar36 < *pppppppppppfVar52) {
                uVar16 = (int)pppppppppppfVar52[2] + uVar36 * 0x24;
              }
              else {
                uVar16 = (uint)pppppppppppfVar52[2];
              }
              uVar43 = (uint)pppppppppppfVar52[6];
              if ((int)uVar43 < (int)pppppppppppfVar52[7]) {
                iVar21 = (int)pppppppppppfVar52[8] + uVar43 * 0x18;
                pppppppppppfVar52[6] = (float **********)(uVar43 + 1);
              }
              else {
                iVar21 = 0;
              }
              iVar35 = 0;
              do {
                pfVar39 = (float *)(uVar16 + iVar35 * 0xc);
                if (*(char *)((int)ppppppppppfVar22 + 0x91) == '\0') {
                  ppppppppppfVar56 = ppppppppppfVar22 + 0xf;
                }
                else {
                  ppppppppppfVar56 = ppppppppppfVar22 + 0x15;
                }
                func_00115C70((float *)(local_1f28 + 1),pfVar39,(float *)ppppppppppfVar56);
                pppppppppfVar48 = ppppppppppfVar22[0x1c];
                fVar65 = ALBW_AS_F32(ppppppppppfVar22[0x23]) - ALBW_AS_F32(ppppppppppfVar22[0x22]);
                fVar54 = ALBW_AS_F32(((ulonglong)lVar69 >> 0x20));
                fVar60 = ALBW_AS_F32(*ppppppppppfVar22) * ALBW_AS_F32(local_1f28[1]) +
                         ALBW_AS_F32(ppppppppppfVar22[1]) * ALBW_AS_F32(local_1f28[2]) +
                         ALBW_AS_F32(ppppppppppfVar22[2]) * ALBW_AS_F32(local_1f28[3]);
                if (fVar65 != fVar54) {
                  pppppppppfVar48 =
                       (float *********)
                       ALBW_PTR_BITS((((ALBW_AS_F32(ppppppppppfVar22[0x1d]) - ALBW_AS_F32(pppppppppfVar48)) *
                        (fVar60 - ALBW_AS_F32(ppppppppppfVar22[0x22]))) / fVar65 + ALBW_AS_F32(pppppppppfVar48)))
                  ;
                }
                pppppppppfVar23 = ppppppppppfVar22[0x1e];
                fVar65 = ALBW_AS_F32(ppppppppppfVar22[0x21]) - ALBW_AS_F32(ppppppppppfVar22[0x20]);
                if (fVar65 != fVar54) {
                  pppppppppfVar23 =
                       (float *********)
                       ALBW_PTR_BITS((((ALBW_AS_F32(ppppppppppfVar22[0x1f]) - ALBW_AS_F32(pppppppppfVar23)) *
                        (fVar60 - ALBW_AS_F32(ppppppppppfVar22[0x20]))) / fVar65 + ALBW_AS_F32(pppppppppfVar23)))
                  ;
                }
                fVar60 = pfVar39[1];
                if (fVar55 != fVar54) {
                  pppppppppfVar48 =
                       (float *********)
                       ALBW_PTR_BITS((((ALBW_AS_F32(pppppppppfVar23) - ALBW_AS_F32(pppppppppfVar48)) *
                        (fVar60 - ALBW_AS_F32(lVar69))) / fVar55 + ALBW_AS_F32(pppppppppfVar48)));
                }
                pfVar39 = (float *)(iVar21 + iVar35 * 8);
                iVar35 = iVar35 + 1;
                *pfVar39 = fVar58 + ALBW_AS_F32(uVar67) * ALBW_AS_F32(pppppppppfVar48);
                pfVar39[1] = (fVar60 - ALBW_AS_F32(local_1ec4)) * ALBW_AS_F32(((ulonglong)uVar67 >> 0x20));
              } while (iVar35 < 3);
              uVar36 = uVar36 + 1;
            } while ((int)uVar36 < (int)local_1e64);
          }
          local_1e50 = (float ***********)((int)local_1e50 + 1);
        } while ((int)local_1e50 < (int)local_1e60[0x29]);
      }
    }
    fVar5 = local_2c[0x1a];
    *(uint *)((int)ALBW_AS_U32(fVar5) + 0x164) = *(uint *)((int)ALBW_AS_U32(fVar5) + 0x164) | 0x40000000;
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x80) = local_30[0x12];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x84) = local_30[0x15];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x88) = local_30[0x18];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x8c) = local_30[0xc];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x90) = local_30[0x13];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x94) = local_30[0x16];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x98) = local_30[0x19];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0x9c) = local_30[0xd];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0xa0) = local_30[0x14];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0xa4) = local_30[0x17];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0xa8) = local_30[0x1a];
    *(float ***********)((int)ALBW_AS_U32(fVar5) + 0xac) = local_30[0xe];
    if (*(char *)((int)local_2c + 0x96) != '\0') {
      func_0043285C((uint *)local_1e30,ALBW_AS_F32(uVar70));
    }
  }
/* WALKER: end: walker stays free */
LAB_001fe470:
  *(char *)local_1e2c = '\0';
  return 0;
}

