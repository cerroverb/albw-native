/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
// MAPFIX-INTEGRATED v1
#include "ghidra_types.h"

extern int func_00100118(int param_1,undefined *param_2,int param_3,int param_4);
extern int func_0010983C(undefined4 param_1,undefined4 param_2);
extern int func_0010A794(undefined4 *param_1);
extern int func_0010B3EC(undefined4 *param_1,int param_2,int param_3);
extern int func_0010BA78(undefined4 param_1);
extern int func_0010BAB4(undefined4 param_1);
extern undefined8 func_0010BEC4(int param_1,uint param_2,int *param_3);
extern int func_0010CEE0(undefined4 param_1,int *param_2,undefined4 param_3);
extern int func_00151358(int *param_1);
extern int func_0015137C(undefined4 *param_1,undefined4 param_2,undefined4 param_3);
extern int func_002AFEC4(int param_1,int param_2,int param_3);
extern int func_002E9BFC(int param_1);
extern int func_002EBFB8(int param_1);
extern int func_002F36B4(int param_1);
extern int func_0032F024(float *param_1,uint param_2,float fparam_0);
extern int func_00361610(int param_1,undefined1 param_2);
extern int func_00361634();
extern int func_003673F4(undefined4 *param_1,int param_2);
extern int func_0036C884(undefined4 *param_1,undefined4 *param_2,undefined1 param_3);
extern int func_0036CC94(undefined4 *param_1,undefined4 param_2,undefined4 param_3);
extern int func_0036D0C8(int *param_1,int param_2);
extern int func_0039D398(undefined4 *param_1,undefined4 *param_2);
extern int func_004102B0(void);
extern int func_004356B4(int *param_1);
extern int func_004EAA5C(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4);
extern int func_004ECF6C(int param_1);
extern int func_004F5BD8(int param_1,uint *param_2,int **param_3,float *param_4);
extern int func_004F8774(int param_1,uint param_2,int param_3,uint param_4);
extern int func_004F8864(int param_1,undefined4 *param_2,uint param_3,float fparam_0);
extern int func_004F88A0(int *param_1,undefined2 *param_2);
extern float func_004F898C(void);
extern int func_004F9554(int param_1,ushort param_2,ushort param_3,int param_4);
extern int func_004FAB30(int param_1,int param_2,int param_3,int param_4);
extern int func_004FAC70(int param_1,uint param_2,int *param_3);
extern int func_004FACA4(int param_1,uint param_2,int *param_3);
extern float func_004FAD90(int param_1);
extern int func_004FAF94(void);
extern int func_004FAFA0(int param_1,undefined4 *param_2);
extern int func_004FAFAC(int param_1,int param_2,int param_3,int param_4);
extern int func_004FB024(int param_1);
extern int func_004FB30C(int param_1,int param_2);
extern int func_004FB344(int param_1,int param_2);
extern void func_004FB408(int param_1);
extern int func_004FB468(int param_1);
extern int func_004FC78C(int param_1,undefined2 param_2);
extern int func_004FC8D8(int param_1,int param_2);
extern int func_004FD464(undefined4 *param_1,uint *param_2,uint *param_3);
extern int func_005330BC(int param_1,int *param_2);
extern int func_0054D964(int param_1,uint param_2);
extern int func_0054DDF4(int param_1,uint param_2);
extern float func_0054DE3C(int param_1,uint param_2,int param_3);
extern int func_00564A78(int param_1,int *param_2);
extern int func_0058A468(int param_1);
extern undefined4 func_006640EC(undefined4 *param_1,undefined1 param_2,float fparam_0);
extern uint *DAT_002f6e28;
extern int DAT_002f6e2c;
extern undefined4 DAT_002f6e30;
extern undefined4 DAT_002f6e34;
extern undefined4 DAT_002f6e38;
extern undefined4 *DAT_002f6e3c;
extern int DAT_002f6e40;
extern undefined4 DAT_002f6e44;
extern undefined4 DAT_002f6e48;
extern undefined4 DAT_002f6e4c;
extern undefined4 DAT_002f6e50;
extern undefined4 DAT_002f6e54;
extern undefined4 DAT_002f7244;
extern undefined *DAT_002f7248;
extern undefined4 *DAT_002f724c;
extern undefined4 *DAT_002f7250;
extern int *DAT_002f7254;
extern uint DAT_002f7260;
extern undefined4 *DAT_002f76cc;
extern undefined4 *DAT_002f7e04;
extern undefined4 *DAT_002f7e08;
extern undefined4 *DAT_002f7e0c;
extern undefined4 *DAT_002f7e10;
extern undefined4 *DAT_002f7e14;
extern undefined4 *DAT_002f7e18;
extern undefined4 *DAT_002f7e1c;
extern undefined4 *DAT_002f7e20;
extern undefined4 *DAT_002f7e24;
extern undefined4 *DAT_002f7e28;
extern undefined4 *DAT_002f7e2c;
extern undefined4 *DAT_002f7e30;
extern undefined4 *DAT_002f7e34;
extern undefined4 *DAT_002f7e38;
extern undefined4 *DAT_002f7e3c;
extern undefined4 *DAT_002f7e40;
extern undefined4 *DAT_002f7e44;
extern undefined4 *DAT_002f7e48;
extern undefined4 *DAT_002f7e4c;
extern undefined4 *DAT_002f7e50;
extern int *DAT_002f7e54;
extern int DAT_002f7e58;
extern int DAT_002f7e5c;
extern undefined DAT_002f7e60;
extern uint DAT_002f7e68;
extern undefined *DAT_002f7e98;
extern int DAT_002f7e9c;
extern int DAT_002f831c;
extern undefined4 *DAT_002f8c44;
extern undefined4 *DAT_002f8c90;
extern float DAT_002f8c94;
extern char s_arm_l1_002f769c[];
extern char s_arm_l2_002f76a4[];
extern char s_arm_r1_002f76b4[];
extern char s_arm_r1_002f7e74[];
extern char s_arm_r2_002f76bc[];
extern char s_cState_Appear1st_002f7ea0[];
extern char s_cState_Appear2nd_002f7eb4[];
extern char s_cState_AttackHarai_002f82f4[];
extern char s_cState_AttackLongTsuki_002f82dc[];
extern char s_cState_AttackMagic_002f87f4[];
extern char s_cState_AttackTsuki_002f82c8[];
extern char s_cState_Die_002f8c84[];
extern char s_cState_Guard_002f82b8[];
extern char s_cState_HitBall_002f87a0[];
extern char s_cState_PreHitBall_002f878c[];
extern char s_cState_RallyMBall_002f87c4[];
extern char s_cState_RallyWait_002f836c[];
extern char s_cState_Reaction_002f8c74[];
extern char s_cState_ShootBall_002f8380[];
extern char s_cState_ShootMBall_002f87b0[];
extern char s_cState_StunLast_002f87e4[];
extern char s_cState_Stun_002f87d8[];
extern char s_cState_ThrowTrident_002f8358[];
extern char s_cState_ToWall_002f8808[];
extern char s_cState_Wait_002f82a0[];
extern char s_cState_Walk_002f82ac[];
extern char s_cState_WallArrowEvent_002f8c5c[];
extern char s_cState_WallAttack_002f8c10[];
extern char s_cState_WallIn_002f8818[];
extern char s_cState_WallJump_002f8c24[];
extern char s_cState_WallOutLast_002f8c48[];
extern char s_cState_WallOut_002f8c34[];
extern char s_cState_WallSummons_002f8868[];
extern char s_cState_WallWait2_002f8840[];
extern char s_cState_WallWaitFirst_002f8828[];
extern char s_cState_WallWaitLast_002f8854[];
extern char s_cState_WarpAway2nd_002f8330[];
extern char s_cState_WarpAwayDark_002f8344[];
extern char s_cState_WarpAway_002f8320[];
extern char s_cState_WarpToPlayer_002f8308[];
extern char s_chest_002f7258[];
extern char s_chest_002f7e6c[];
extern char s_skl_root_002f7e8c[];
extern char s_trident_002f7e7c[];
extern char s_wrist_l_002f76ac[];
extern char s_wrist_l_002f7e84[];
extern char s_wrist_r_002f76c4[];

// USA 002F6A38 <- EU 002F6A14 (Ghidra, non-matching)
// GLOBAL DAT_002f6e28 uint *
// GLOBAL DAT_002f6e2c int
// GLOBAL DAT_002f6e30 undefined4
// GLOBAL DAT_002f6e34 undefined4
// GLOBAL DAT_002f6e38 undefined4
// GLOBAL DAT_002f6e3c undefined4 *
// GLOBAL DAT_002f6e40 int
// GLOBAL DAT_002f6e44 undefined4
// GLOBAL DAT_002f6e48 undefined4
// GLOBAL DAT_002f6e4c undefined4
// GLOBAL DAT_002f6e50 undefined4
// GLOBAL DAT_002f6e54 undefined4
// GLOBAL DAT_002f7244 undefined4
// GLOBAL DAT_002f7248 undefined *
// GLOBAL DAT_002f724c undefined4 *
// GLOBAL DAT_002f7250 undefined4 *
// GLOBAL DAT_002f7254 int *
// GLOBAL DAT_002f7260 uint
// GLOBAL DAT_002f76cc undefined4 *
// GLOBAL DAT_002f7e04 undefined4 *
// GLOBAL DAT_002f7e08 undefined4 *
// GLOBAL DAT_002f7e0c undefined4 *
// GLOBAL DAT_002f7e10 undefined4 *
// GLOBAL DAT_002f7e14 undefined4 *
// GLOBAL DAT_002f7e18 undefined4 *
// GLOBAL DAT_002f7e1c undefined4 *
// GLOBAL DAT_002f7e20 undefined4 *
// GLOBAL DAT_002f7e24 undefined4 *
// GLOBAL DAT_002f7e28 undefined4 *
// GLOBAL DAT_002f7e2c undefined4 *
// GLOBAL DAT_002f7e30 undefined4 *
// GLOBAL DAT_002f7e34 undefined4 *
// GLOBAL DAT_002f7e38 undefined4 *
// GLOBAL DAT_002f7e3c undefined4 *
// GLOBAL DAT_002f7e40 undefined4 *
// GLOBAL DAT_002f7e44 undefined4 *
// GLOBAL DAT_002f7e48 undefined4 *
// GLOBAL DAT_002f7e4c undefined4 *
// GLOBAL DAT_002f7e50 undefined4 *
// GLOBAL DAT_002f7e54 int *
// GLOBAL DAT_002f7e58 int
// GLOBAL DAT_002f7e5c int
// GLOBAL DAT_002f7e60 undefined
// GLOBAL DAT_002f7e68 uint
// GLOBAL DAT_002f7e98 undefined *
// GLOBAL DAT_002f7e9c int
// GLOBAL DAT_002f831c int
// GLOBAL DAT_002f8c44 undefined4 *
// GLOBAL DAT_002f8c90 undefined4 *
// GLOBAL DAT_002f8c94 float
// GLOBAL s_arm_l1_002f769c string
// GLOBAL s_arm_l2_002f76a4 string
// GLOBAL s_arm_r1_002f76b4 string
// GLOBAL s_arm_r1_002f7e74 string
// GLOBAL s_arm_r2_002f76bc string
// GLOBAL s_cState_Appear1st_002f7ea0 string
// GLOBAL s_cState_Appear2nd_002f7eb4 string
// GLOBAL s_cState_AttackHarai_002f82f4 string
// GLOBAL s_cState_AttackLongTsuki_002f82dc string
// GLOBAL s_cState_AttackMagic_002f87f4 string
// GLOBAL s_cState_AttackTsuki_002f82c8 string
// GLOBAL s_cState_Die_002f8c84 string
// GLOBAL s_cState_Guard_002f82b8 string
// GLOBAL s_cState_HitBall_002f87a0 string
// GLOBAL s_cState_PreHitBall_002f878c string
// GLOBAL s_cState_RallyMBall_002f87c4 string
// GLOBAL s_cState_RallyWait_002f836c string
// GLOBAL s_cState_Reaction_002f8c74 string
// GLOBAL s_cState_ShootBall_002f8380 string
// GLOBAL s_cState_ShootMBall_002f87b0 string
// GLOBAL s_cState_StunLast_002f87e4 string
// GLOBAL s_cState_Stun_002f87d8 string
// GLOBAL s_cState_ThrowTrident_002f8358 string
// GLOBAL s_cState_ToWall_002f8808 string
// GLOBAL s_cState_Wait_002f82a0 string
// GLOBAL s_cState_Walk_002f82ac string
// GLOBAL s_cState_WallArrowEvent_002f8c5c string
// GLOBAL s_cState_WallAttack_002f8c10 string
// GLOBAL s_cState_WallIn_002f8818 string
// GLOBAL s_cState_WallJump_002f8c24 string
// GLOBAL s_cState_WallOutLast_002f8c48 string
// GLOBAL s_cState_WallOut_002f8c34 string
// GLOBAL s_cState_WallSummons_002f8868 string
// GLOBAL s_cState_WallWait2_002f8840 string
// GLOBAL s_cState_WallWaitFirst_002f8828 string
// GLOBAL s_cState_WallWaitLast_002f8854 string
// GLOBAL s_cState_WarpAway2nd_002f8330 string
// GLOBAL s_cState_WarpAwayDark_002f8344 string
// GLOBAL s_cState_WarpAway_002f8320 string
// GLOBAL s_cState_WarpToPlayer_002f8308 string
// GLOBAL s_chest_002f7258 string
// GLOBAL s_chest_002f7e6c string
// GLOBAL s_skl_root_002f7e8c string
// GLOBAL s_trident_002f7e7c string
// GLOBAL s_wrist_l_002f76ac string
// GLOBAL s_wrist_l_002f7e84 string
// GLOBAL s_wrist_r_002f76c4 string

int * func_002F6A38(undefined4 *param_1,uint *param_2)

{
  byte bVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  undefined4 *puVar7;
  int *piVar8;
  uint uVar9;
  uint *puVar10;
  undefined4 *puVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  int *piVar17;
  undefined4 uVar18;
  undefined4 uVar19;
  int iVar20;
  float *pfVar21;
  undefined4 uVar22;
  int *piVar23;
  int *piVar24;
  undefined4 uVar25;
  bool bVar26;
  float fVar27;
  float fVar28;
  undefined8 uVar29;
  _Alignas(8) unsigned char albw_frame[0x60];
  #define local_58 (*(uint *)(albw_frame + 0x8))
  #define local_54 (*(char * *)(albw_frame + 0xc))
  #define local_50 (*(uint *)(albw_frame + 0x10))
  #define local_4c (*(char * *)(albw_frame + 0x14))
  #define local_48 (*(int * *)(albw_frame + 0x18))
  #define local_44 (*(int *)(albw_frame + 0x1c))
  #define puStack_2c (*(undefined4 * *)(albw_frame + 0x34))
  #define local_28 (*(uint * *)(albw_frame + 0x38))
  puStack_2c = param_1;
  local_28 = param_2;
  piVar2 = func_004FD464(param_1,param_2,DAT_002f6e28);
  iVar12 = DAT_002f6e2c;
  piVar2[200] = DAT_002f6e2c + 0x84;
  *piVar2 = iVar12;
  piVar2[0x22f] = 0;
  piVar2[0x230] = 0;
  piVar2[0x231] = 0;
  puVar3 = func_0010A794(piVar2 + 0x232);
  puVar3[5] = 0;
  uVar16 = DAT_002f6e30;
  puVar3[6] = 0;
  puVar3[7] = 0;
  puVar3[8] = 0;
  puVar3[9] = uVar16;
  puVar3[10] = puVar3 + -0x232;
  puVar3[0xb] = 0;
  puVar3[0xc] = 0xffffffff;
  puVar3[0xf] = 0;
  puVar3[0xd] = 0xffffffff;
  puVar3[0x10] = 0;
  uVar16 = DAT_002f6e34;
  puVar3[0x11] = 0;
  puVar3[0x12] = 0;
  puVar3[0x13] = uVar16;
  puVar3[0x14] = 0;
  puVar7 = DAT_002f6e3c;
  uVar18 = DAT_002f6e38;
  puVar3[0x15] = 0;
  puVar3[0x17] = 0;
  puVar3[0x18] = uVar18;
  puVar3[0x19] = uVar18;
  iVar12 = DAT_002f6e40;
  uVar18 = puVar7[1];
  uVar25 = puVar7[2];
  puVar3[0x1a] = *puVar7;
  puVar3[0x1b] = uVar18;
  puVar3[0x1c] = uVar25;
  uVar18 = puVar7[1];
  uVar25 = puVar7[2];
  puVar3[0x1d] = *puVar7;
  puVar3[0x1e] = uVar18;
  puVar3[0x1f] = uVar25;
  uVar18 = puVar7[1];
  uVar25 = puVar7[2];
  puVar3[0x20] = *puVar7;
  puVar3[0x21] = uVar18;
  puVar3[0x22] = uVar25;
  uVar18 = 1;
  puVar3[0x23] = 0;
  puVar3[0x24] = 0;
  puVar3[0x25] = 0;
  puVar3[0x26] = uVar16;
  uVar25 = *(undefined4 *)(iVar12 + 4);
  uVar22 = *(undefined4 *)(iVar12 + 8);
  puVar3 = func_0036C884(puVar3 + 0x27,puVar3 + -0x232,1);
  *puVar3 = DAT_002f6e44;
  puVar3[0x40] = uVar25;
  puVar3[0x41] = uVar22;
  puVar3 = func_0036CC94(puVar3 + 0x42,puVar3 + -0x259,uVar18);
  puVar3[0x25] = 0;
  puVar3[0x26] = 0;
  puVar3[0x27] = uVar16;
  puVar3[0x28] = 0;
  puVar3[0x29] = 0;
  puVar3[0x2a] = uVar16;
  uVar18 = DAT_002f6e48;
  puVar3[0x2c] = 0xffff;
  puVar3[0x2d] = uVar18;
  puVar3[0x2b] = 0;
  puVar3[0x2e] = 0;
  puVar3[0x2f] = 0;
  puVar11 = puVar3 + -0x29b;
  puVar3[0x30] = uVar16;
  puVar3[0x31] = 0;
  puVar3[0x32] = 0;
  puVar3[0x33] = uVar16;
  uVar25 = DAT_002f6e4c;
  puVar3[0x34] = 0;
  puVar3[0x35] = 0;
  puVar3[0x36] = uVar16;
  puVar3[0x37] = 0;
  puVar3[0x38] = 0;
  puVar3[0x39] = uVar16;
  puVar3[0x3a] = 0;
  puVar3[0x3b] = 0;
  puVar3[0x3c] = uVar16;
  puVar3[0x3d] = 0;
  puVar3[0x3e] = 6;
  puVar3[0x3f] = uVar16;
  puVar3[0x40] = 0;
  puVar3[0x41] = 0x1e;
  puVar3[0x42] = uVar16;
  puVar3[0x43] = 0xffffffff;
  puVar3[0x44] = 0;
  puVar3[0x45] = 0xffffffff;
  puVar3[0x46] = 0xffffffff;
  puVar3[0x48] = uVar25;
  puVar3[0x49] = uVar16;
  puVar3[0x4a] = uVar25;
  fVar27 = func_004F898C();
  puVar3[0x4b] = ALBW_AS_U32(fVar27);
  puVar3[0x4c] = 0;
  puVar3[0x4d] = 0;
  puVar3[0x4e] = uVar16;
  puVar3[0x4f] = puVar3[-0x299];
  puVar3[0x50] = puVar3[-0x298];
  puVar3[0x51] = puVar3[-0x297];
  puVar3[0x52] = puVar3[-0x299];
  puVar3[0x53] = puVar3[-0x298];
  puVar3[0x54] = puVar3[-0x297];
  puVar3[0x55] = puVar3[-0x299];
  puVar3[0x56] = puVar3[-0x298];
  puVar3[0x57] = puVar3[-0x297];
  uVar22 = puVar7[1];
  uVar15 = puVar7[2];
  puVar3[0x5b] = *puVar7;
  puVar3[0x5c] = uVar22;
  puVar3[0x5d] = uVar15;
  uVar22 = puVar7[1];
  uVar15 = puVar7[2];
  puVar3[0x5e] = *puVar7;
  puVar3[0x5f] = uVar22;
  puVar3[0x60] = uVar15;
  uVar22 = puVar7[1];
  uVar15 = puVar7[2];
  puVar3[0x61] = *puVar7;
  puVar3[0x62] = uVar22;
  puVar3[99] = uVar15;
  puVar3[100] = puVar3[-0x1a7];
  puVar3[0x65] = puVar3[-0x1a6];
  puVar3[0x66] = puVar3[-0x1a5];
  puVar3[0x67] = uVar25;
  uVar22 = puVar7[1];
  uVar15 = puVar7[2];
  puVar3[0x68] = *puVar7;
  puVar3[0x69] = uVar22;
  puVar3[0x6a] = uVar15;
  puVar3[0x6b] = uVar25;
  puVar3[0x6c] = uVar25;
  iVar4 = func_0058A468((int)puVar11);
  puVar3[0x6d] = iVar4;
  puVar3[0x6e] = 0;
  puVar3[0x6f] = 0;
  puVar3[0x70] = 0;
  puVar3[0x71] = 0;
  puVar3[0x72] = 0;
  puVar3[0x73] = uVar25;
  puVar3[0x74] = uVar25;
  puVar3[0x75] = 0;
  puVar3[0x76] = 0;
  puVar3[0x77] = uVar16;
  uVar22 = puVar7[1];
  uVar15 = puVar7[2];
  puVar3[0x78] = *puVar7;
  puVar3[0x79] = uVar22;
  puVar3[0x7a] = uVar15;
  uVar22 = puVar7[1];
  uVar15 = puVar7[2];
  puVar3[0x7b] = *puVar7;
  puVar3[0x7c] = uVar22;
  puVar3[0x7d] = uVar15;
  puVar3[0x7e] = 0;
  puVar3[0x7f] = 0;
  puVar3[0x80] = uVar25;
  puVar3[0x81] = uVar25;
  puVar3[0x82] = 0;
  puVar3[0x83] = 0;
  puVar3[0x84] = uVar16;
  puVar3[0x85] = 0;
  puVar3[0x86] = 0;
  puVar3[0x87] = 0;
  puVar3[0x88] = 0;
  puVar3[0x89] = 0;
  puVar3[0x8a] = 0;
  puVar3[0x8b] = 0;
  puVar3[0x8d] = 0;
  puVar3[0x8f] = 0;
  puVar3[0x90] = 0;
  puVar3[0x91] = 0;
  uVar22 = *(undefined4 *)(iVar12 + 0x10);
  puVar3[0x95] = *(undefined4 *)(iVar12 + 0xc);
  puVar3[0x96] = uVar22;
  uVar22 = DAT_002f6e50;
  puVar3[0x93] = DAT_002f6e50;
  puVar3[0x94] = puVar11;
  uVar15 = *(undefined4 *)(iVar12 + 0x14);
  uVar19 = *(undefined4 *)(iVar12 + 0x18);
  puVar3[0x97] = uVar22;
  puVar3[0x98] = puVar11;
  puVar3[0x99] = uVar15;
  puVar3[0x9a] = uVar19;
  uVar15 = *(undefined4 *)(iVar12 + 0x1c);
  uVar19 = *(undefined4 *)(iVar12 + 0x20);
  puVar3[0x9b] = uVar22;
  puVar3[0x9c] = puVar11;
  puVar3[0x9d] = uVar15;
  puVar3[0x9e] = uVar19;
  puVar3[0xa0] = 0;
  uVar15 = *(undefined4 *)(iVar12 + 0x3a8);
  puVar3[0xa1] = *(undefined4 *)(iVar12 + 0x3a4);
  uVar22 = DAT_002f6e54;
  puVar3[0xa2] = uVar15;
  puVar3[0x9f] = uVar22;
  uVar15 = *(undefined4 *)(iVar12 + 0x24);
  uVar19 = *(undefined4 *)(iVar12 + 0x28);
  puVar3[0xa3] = uVar22;
  puVar3[0xa4] = puVar11;
  puVar3[0xa5] = uVar15;
  puVar3[0xa6] = uVar19;
  uVar15 = *(undefined4 *)(iVar12 + 0x2c);
  uVar19 = *(undefined4 *)(iVar12 + 0x30);
  puVar3[0xa7] = uVar22;
  puVar3[0xa8] = puVar11;
  puVar3[0xa9] = uVar15;
  puVar3[0xaa] = uVar19;
  uVar15 = *(undefined4 *)(iVar12 + 0x34);
  uVar19 = *(undefined4 *)(iVar12 + 0x38);
  puVar3[0xab] = uVar22;
  puVar3[0xac] = puVar11;
  puVar3[0xad] = uVar15;
  puVar3[0xae] = uVar19;
  puVar3[0xaf] = 0;
  puVar3[0xb0] = 0;
  puVar3[0xb1] = 0;
  puVar3[0xb2] = 0;
  puVar3[0xb3] = 0;
  *(undefined1 *)(puVar3 + 0xb4) = 0;
  *(undefined1 *)((int)puVar3 + 0x2d1) = 0;
  *(undefined1 *)((int)puVar3 + 0x2d3) = 0;
  *(undefined1 *)(puVar3 + 0xb5) = 0;
  *(undefined1 *)((int)puVar3 + 0x2d5) = 0;
  *(undefined1 *)((int)puVar3 + 0x2d6) = 0;
  *(undefined1 *)((int)puVar3 + 0x2d7) = 0;
  *(undefined1 *)(puVar3 + 0xb6) = 0;
  *(undefined1 *)((int)puVar3 + 0x2d9) = 0;
  *(undefined1 *)((int)puVar3 + 0x2da) = 0;
  uVar22 = DAT_002f7244;
  *(undefined1 *)((int)puVar3 + 0x2db) = 0;
  *(undefined1 *)(puVar3 + 0xb7) = 0;
  puVar3[0xb8] = uVar22;
  puVar3[0xbb] = 0xffff;
  puVar3[0xbc] = uVar18;
  puVar3[0xba] = 0;
  iVar4 = func_003673F4(puVar3 + 0xc2,3);
  *(undefined4 *)(iVar4 + 0x38) = 0;
  *(undefined4 *)(iVar4 + 0x3c) = 0;
  *(undefined4 *)(iVar4 + 0x40) = 0;
  *(undefined4 *)(iVar4 + 0x44) = 0;
  *(undefined4 *)(iVar4 + 0x48) = 0;
  *(undefined4 *)(iVar4 + 0x4c) = 0;
  *(undefined4 *)(iVar4 + 0x50) = 0;
  *(undefined4 *)(iVar4 + 0x54) = uVar25;
  *(undefined4 *)(iVar4 + 0x58) = 0;
  *(undefined4 *)(iVar4 + 0x5c) = *(undefined4 *)(iVar4 + -0x9a4);
  *(undefined4 *)(iVar4 + 0x60) = *(undefined4 *)(iVar4 + -0x9a0);
  *(undefined4 *)(iVar4 + 100) = *(undefined4 *)(iVar4 + -0x99c);
  uVar18 = *(undefined4 *)(iVar4 + -0x99c);
  *(undefined4 *)(iVar4 + 0x68) = *(undefined4 *)(iVar4 + -0x9a4);
  *(undefined4 *)(iVar4 + 0x6c) = *(undefined4 *)(iVar4 + -0x9a0);
  *(undefined4 *)(iVar4 + 0x70) = uVar18;
  *(undefined4 *)(iVar4 + 0x74) = *(undefined4 *)(iVar4 + -0x9a4);
  *(undefined4 *)(iVar4 + 0x78) = *(undefined4 *)(iVar4 + -0x9a0);
  *(undefined4 *)(iVar4 + 0x7c) = uVar18;
  *(undefined4 *)(iVar4 + 0x88) = uVar18;
  *(undefined4 *)(iVar4 + 0x80) = *(undefined4 *)(iVar4 + 0x74);
  *(undefined4 *)(iVar4 + 0x84) = *(undefined4 *)(iVar4 + 0x78);
  *(undefined4 *)(iVar4 + 0x8c) = *(undefined4 *)(iVar4 + -0x9a4);
  *(undefined4 *)(iVar4 + 0x90) = *(undefined4 *)(iVar4 + -0x9a0);
  *(undefined4 *)(iVar4 + 0x94) = *(undefined4 *)(iVar4 + -0x99c);
  *(undefined1 *)(iVar4 + 0x98) = 0;
  *(undefined1 *)(iVar4 + 0x9a) = 0;
  *(undefined4 *)(iVar4 + 0x9c) = uVar25;
  *(undefined4 *)(iVar4 + 0xa0) = uVar25;
  *(undefined4 *)(iVar4 + 0xa4) = uVar25;
  *(undefined4 *)(iVar4 + 0xa8) = uVar25;
  *(undefined4 *)(iVar4 + 0xac) = 0;
  *(undefined4 *)(iVar4 + 0xb0) = 0;
  *(undefined4 *)(iVar4 + 0xb4) = 0;
  *(undefined1 *)(iVar4 + 0xb8) = 0;
  *(undefined1 *)(iVar4 + 0xb9) = 0;
  *(undefined1 *)(iVar4 + 0xba) = 0;
  uVar18 = puVar7[1];
  uVar25 = puVar7[2];
  *(undefined4 *)(iVar4 + 0xbc) = *puVar7;
  *(undefined4 *)(iVar4 + 0xc0) = uVar18;
  *(undefined4 *)(iVar4 + 0xc4) = uVar25;
  *(undefined4 *)(iVar4 + 200) = 0;
  iVar4 = func_00100118(iVar4 + 0xcc,DAT_002f7248,0x34,0x14);
  *(undefined1 *)(iVar4 + 0x410) = 0;
  piVar2 = (int *)(iVar4 + -0xe40);
  *(undefined4 *)(iVar4 + 0x414) = 0;
  *(undefined4 *)(iVar4 + 0x418) = 0;
  *(undefined4 *)(iVar4 + 0x41c) = 0;
  *(undefined4 *)(iVar4 + 0x420) = uVar16;
  *(undefined4 *)(iVar4 + 0x424) = 0;
  *(undefined4 *)(iVar4 + 0x428) = 0;
  *(undefined4 *)(iVar4 + 0x42c) = uVar16;
  func_0036D0C8((int *)(iVar4 + -0x584),0);
  iVar13 = *(int *)(iVar4 + -0x584);
  uVar5 = (uint)(iVar13 == 0);
  if (iVar13 != 0) {
    uVar5 = func_0010983C(*DAT_002f724c,iVar13);
  }
  puVar10 = DAT_002f6e28;
  uVar6 = DAT_002f6e28[3];
  if ((int)uVar6 < 0) {
    uVar6 = func_005330BC(*(int *)(*(int *)(iVar4 + -0xb70) + 0x1c8 + DAT_002f6e28[2] * 4),
                          (int *)(DAT_002f6e28 + 6));
  }
  local_58 = puVar10[0x11];
  func_004F8774((int)piVar2,puVar10[1],puVar10[2],uVar6);
  local_48 = (int *)(iVar4 + -0x898);
  if (*(char *)(iVar4 + -0x7f0) == '\0') {
    iVar13 = 0;
  }
  else {
    iVar13 = *local_48;
  }
  *(int *)(iVar4 + -0x54c) = iVar13;
  if (uVar5 != 1) {
    func_0010983C(*DAT_002f724c,uVar5);
  }
  iVar13 = func_0054DDF4((int)piVar2,1);
  piVar24 = DAT_002f7254;
  puVar7 = DAT_002f7250;
  *(int *)(iVar4 + -0x354) = iVar13;
  if (*(short *)(iVar4 + -0xe12) == 1) {
    func_004F9554(*(int *)(iVar4 + -0xa10),7,0,1);
    piVar8 = (int *)0x1;
    func_004F9554(*(int *)(iVar4 + -0xa0c),7,0,1);
    func_004F88A0(piVar2,*(undefined2 **)(iVar4 + -0x354));
    func_004FAD90((int)piVar2);
    *(undefined1 *)(iVar4 + -0xfb) = 1;
    func_004FC78C((int)piVar2,0x120);
    uVar18 = *(undefined4 *)(iVar12 + 0x3c);
    uVar25 = *(undefined4 *)(iVar12 + 0x40);
    *(int **)(iVar4 + -0x154) = piVar2;
    *(undefined4 *)(iVar4 + -0x150) = uVar18;
    *(undefined4 *)(iVar4 + -0x14c) = uVar25;
    func_004EAA5C(*puVar7,uVar18,uVar25,piVar8);
  }
  else {
    *(undefined1 *)(iVar4 + -0xfb) = 0;
    func_00361634(*piVar24,1);
    local_58 = *(uint *)(iVar4 + -0xb74);
    func_0039D398((undefined4 *)(iVar4 + -0xa70),&local_58);
    uVar18 = *(undefined4 *)(iVar12 + 0x44);
    uVar25 = *(undefined4 *)(iVar12 + 0x48);
    *(int **)(iVar4 + -0x154) = piVar2;
    *(undefined4 *)(iVar4 + -0x150) = uVar18;
    *(undefined4 *)(iVar4 + -0x14c) = uVar25;
    func_004102B0();
  }
  func_00361610(*piVar24,1);
  *(undefined4 *)(iVar4 + -0x518) = uVar16;
  func_0036D0C8((int *)(iVar4 + -0x580),0);
  if (*(int *)(iVar4 + -0x580) == 0) {
    local_44 = 1;
  }
  else {
    local_44 = func_0010983C(*DAT_002f724c,*(int *)(iVar4 + -0x580));
  }
  iVar13 = func_0054D964((int)piVar2,1);
  *(int *)(iVar4 + -0x34c) = iVar13;
  fVar28 = func_0054DE3C(piVar2,1,1);
  *(float *)(iVar4 + -0x2ac) = fVar28;
  fVar27 = *(float *)(iVar4 + -0x60);
  iVar13 = *(int *)(iVar4 + -0x34c);
  uVar16 = *(undefined4 *)(iVar4 + -0x5c);
  *(undefined4 *)(iVar13 + 4) = *(undefined4 *)(iVar4 + -100);
  *(float *)(iVar13 + 8) = fVar27 + fVar28;
  *(undefined4 *)(iVar13 + 0xc) = uVar16;
  func_004356B4(*(int **)(iVar4 + -0x34c));
  iVar13 = func_0010CEE0(0x180,(int *)0x0,4);
  if (iVar13 != 0) {
    iVar14 = 0;
    iVar20 = 7;
    *(int *)(iVar4 + -0x10c) = iVar13;
    do {
      iVar20 = iVar20 + -1;
      *(int *)(iVar13 + iVar14 * 4) = iVar13 + (iVar14 + 0xb) * 4;
      iVar14 = iVar14 + 0xb;
    } while (iVar20 != 0);
    *(undefined4 *)(iVar13 + 0x134) = 0;
    *(int *)(iVar4 + -0x108) = iVar13;
    func_0010B3EC((undefined4 *)(iVar4 + -0x118),8,iVar13 + 0x160);
  }
  puVar7 = (undefined4 *)func_0010BAB4(0x20);
  if (puVar7 == (undefined4 *)0x0) {
    puVar7 = (undefined4 *)0x0;
  }
  else {
    puVar7 = func_0015137C(puVar7,piVar2,0);
  }
  uVar5 = DAT_002f7260;
  piVar24 = (int *)(iVar4 + -0xa34);
  local_54 = s_chest_002f7258;
  local_58 = DAT_002f7260;
  uVar6 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_58);
  if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
    puVar3 = *(undefined4 **)(iVar4 + -0x10c);
    if (puVar3 != (undefined4 *)0x0) {
      *(undefined4 *)(iVar4 + -0x10c) = *puVar3;
      puVar3[1] = uVar6;
      *puVar3 = puVar7;
    }
    if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
      *(undefined4 **)(*(int *)(iVar4 + -0x110) + *(int *)(iVar4 + -0x118) * 4) = puVar3;
      *(int *)(iVar4 + -0x118) = *(int *)(iVar4 + -0x118) + 1;
    }
  }
  puVar3 = (undefined4 *)func_0010BAB4(0x20);
  puVar7 = (undefined4 *)0x0;
  if (puVar3 != (undefined4 *)0x0) {
    puVar7 = func_0015137C(puVar3,piVar2,0);
  }
  local_54 = s_arm_l1_002f769c;
  local_58 = uVar5;
  uVar6 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_58);
  if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
    puVar3 = *(undefined4 **)(iVar4 + -0x10c);
    if (puVar3 != (undefined4 *)0x0) {
      *(undefined4 *)(iVar4 + -0x10c) = *puVar3;
      puVar3[1] = uVar6;
      *puVar3 = puVar7;
    }
    if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
      *(undefined4 **)(*(int *)(iVar4 + -0x110) + *(int *)(iVar4 + -0x118) * 4) = puVar3;
      *(int *)(iVar4 + -0x118) = *(int *)(iVar4 + -0x118) + 1;
    }
  }
  puVar7 = (undefined4 *)func_0010BAB4(0x20);
  if (puVar7 == (undefined4 *)0x0) {
    puVar7 = (undefined4 *)0x0;
  }
  else {
    puVar7 = func_0015137C(puVar7,piVar2,0);
  }
  local_54 = s_arm_l2_002f76a4;
  local_58 = uVar5;
  uVar6 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_58);
  if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
    puVar3 = *(undefined4 **)(iVar4 + -0x10c);
    if (puVar3 != (undefined4 *)0x0) {
      *(undefined4 *)(iVar4 + -0x10c) = *puVar3;
      puVar3[1] = uVar6;
      *puVar3 = puVar7;
    }
    if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
      *(undefined4 **)(*(int *)(iVar4 + -0x110) + *(int *)(iVar4 + -0x118) * 4) = puVar3;
      *(int *)(iVar4 + -0x118) = *(int *)(iVar4 + -0x118) + 1;
    }
  }
  puVar7 = (undefined4 *)func_0010BAB4(0x20);
  if (puVar7 == (undefined4 *)0x0) {
    puVar7 = (undefined4 *)0x0;
  }
  else {
    puVar7 = func_0015137C(puVar7,piVar2,0);
  }
  local_54 = s_wrist_l_002f76ac;
  local_58 = uVar5;
  uVar6 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_58);
  if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
    puVar3 = *(undefined4 **)(iVar4 + -0x10c);
    if (puVar3 != (undefined4 *)0x0) {
      *(undefined4 *)(iVar4 + -0x10c) = *puVar3;
      puVar3[1] = uVar6;
      *puVar3 = puVar7;
    }
    if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
      *(undefined4 **)(*(int *)(iVar4 + -0x110) + *(int *)(iVar4 + -0x118) * 4) = puVar3;
      *(int *)(iVar4 + -0x118) = *(int *)(iVar4 + -0x118) + 1;
    }
  }
  puVar3 = (undefined4 *)func_0010BAB4(0x20);
  puVar7 = (undefined4 *)0x0;
  if (puVar3 != (undefined4 *)0x0) {
    puVar7 = func_0015137C(puVar3,piVar2,0);
  }
  local_54 = s_arm_r1_002f76b4;
  local_58 = uVar5;
  uVar6 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_58);
  if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
    puVar3 = *(undefined4 **)(iVar4 + -0x10c);
    if (puVar3 != (undefined4 *)0x0) {
      *(undefined4 *)(iVar4 + -0x10c) = *puVar3;
      puVar3[1] = uVar6;
      *puVar3 = puVar7;
    }
    if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
      *(undefined4 **)(*(int *)(iVar4 + -0x110) + *(int *)(iVar4 + -0x118) * 4) = puVar3;
      *(int *)(iVar4 + -0x118) = *(int *)(iVar4 + -0x118) + 1;
    }
  }
  puVar7 = (undefined4 *)func_0010BAB4(0x20);
  if (puVar7 == (undefined4 *)0x0) {
    puVar7 = (undefined4 *)0x0;
  }
  else {
    puVar7 = func_0015137C(puVar7,piVar2,0);
  }
  local_54 = s_arm_r2_002f76bc;
  local_58 = uVar5;
  uVar6 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_58);
  if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
    puVar3 = *(undefined4 **)(iVar4 + -0x10c);
    if (puVar3 != (undefined4 *)0x0) {
      *(undefined4 *)(iVar4 + -0x10c) = *puVar3;
      puVar3[1] = uVar6;
      *puVar3 = puVar7;
    }
    if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
      *(undefined4 **)(*(int *)(iVar4 + -0x110) + *(int *)(iVar4 + -0x118) * 4) = puVar3;
      *(int *)(iVar4 + -0x118) = *(int *)(iVar4 + -0x118) + 1;
    }
  }
  puVar7 = (undefined4 *)func_0010BAB4(0x20);
  if (puVar7 == (undefined4 *)0x0) {
    puVar7 = (undefined4 *)0x0;
  }
  else {
    puVar7 = func_0015137C(puVar7,piVar2,0);
  }
  local_54 = s_wrist_r_002f76c4;
  local_58 = uVar5;
  uVar6 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_58);
  if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
    puVar3 = *(undefined4 **)(iVar4 + -0x10c);
    if (puVar3 != (undefined4 *)0x0) {
      *(undefined4 *)(iVar4 + -0x10c) = *puVar3;
      puVar3[1] = uVar6;
      *puVar3 = puVar7;
    }
    if (*(int *)(iVar4 + -0x118) < *(int *)(iVar4 + -0x114)) {
      *(undefined4 **)(*(int *)(iVar4 + -0x110) + *(int *)(iVar4 + -0x118) * 4) = puVar3;
      *(int *)(iVar4 + -0x118) = *(int *)(iVar4 + -0x118) + 1;
    }
  }
  bVar26 = *(int *)(iVar4 + -0x118) != 0;
  piVar8 = (int *)0x0;
  if (bVar26) {
    piVar8 = *(int **)(iVar4 + -0x110);
  }
  uVar16 = *DAT_002f76cc;
  uVar18 = DAT_002f76cc[1];
  if (bVar26) {
    iVar13 = *piVar8;
  }
  else {
    iVar13 = 0;
  }
  *(undefined4 *)(iVar13 + 0x10) = DAT_002f76cc[2];
  *(undefined4 *)(iVar13 + 8) = uVar16;
  *(undefined4 *)(iVar13 + 0xc) = uVar18;
  uVar16 = *DAT_002f7e04;
  uVar18 = DAT_002f7e04[1];
  *(undefined4 *)(iVar13 + 0x1c) = DAT_002f7e04[2];
  *(undefined4 *)(iVar13 + 0x14) = uVar16;
  *(undefined4 *)(iVar13 + 0x18) = uVar18;
  bVar26 = *(int *)(iVar4 + -0x118) != 0;
  iVar13 = 0;
  if (bVar26) {
    iVar13 = **(int **)(iVar4 + -0x110);
  }
  uVar16 = *DAT_002f7e08;
  uVar18 = DAT_002f7e08[1];
  if (!bVar26) {
    iVar13 = 0;
  }
  *(undefined4 *)(iVar13 + 0x28) = DAT_002f7e08[2];
  *(undefined4 *)(iVar13 + 0x20) = uVar16;
  *(undefined4 *)(iVar13 + 0x24) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (1 < uVar9) {
    uVar6 = *(uint *)(iVar4 + -0x110);
  }
  uVar16 = *DAT_002f7e0c;
  uVar18 = DAT_002f7e0c[1];
  if (uVar9 < 2) {
    iVar13 = 0;
  }
  else {
    iVar13 = *(int *)(uVar6 + 4);
  }
  *(undefined4 *)(iVar13 + 0x10) = DAT_002f7e0c[2];
  *(undefined4 *)(iVar13 + 8) = uVar16;
  *(undefined4 *)(iVar13 + 0xc) = uVar18;
  uVar16 = *DAT_002f7e10;
  uVar18 = DAT_002f7e10[1];
  *(undefined4 *)(iVar13 + 0x1c) = DAT_002f7e10[2];
  *(undefined4 *)(iVar13 + 0x14) = uVar16;
  *(undefined4 *)(iVar13 + 0x18) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (1 < uVar9) {
    uVar6 = *(uint *)(*(int *)(iVar4 + -0x110) + 4);
  }
  uVar16 = *DAT_002f7e14;
  uVar18 = DAT_002f7e14[1];
  if (uVar9 < 2) {
    uVar6 = 0;
  }
  *(undefined4 *)(uVar6 + 0x28) = DAT_002f7e14[2];
  *(undefined4 *)(uVar6 + 0x20) = uVar16;
  *(undefined4 *)(uVar6 + 0x24) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (2 < uVar9) {
    uVar6 = *(uint *)(iVar4 + -0x110);
  }
  uVar16 = *DAT_002f7e18;
  uVar18 = DAT_002f7e18[1];
  if (uVar9 < 3) {
    iVar13 = 0;
  }
  else {
    iVar13 = *(int *)(uVar6 + 8);
  }
  *(undefined4 *)(iVar13 + 0x10) = DAT_002f7e18[2];
  *(undefined4 *)(iVar13 + 8) = uVar16;
  *(undefined4 *)(iVar13 + 0xc) = uVar18;
  uVar16 = *DAT_002f7e1c;
  uVar18 = DAT_002f7e1c[1];
  *(undefined4 *)(iVar13 + 0x1c) = DAT_002f7e1c[2];
  *(undefined4 *)(iVar13 + 0x14) = uVar16;
  *(undefined4 *)(iVar13 + 0x18) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (2 < uVar9) {
    uVar6 = *(uint *)(iVar4 + -0x110);
  }
  uVar16 = *DAT_002f7e20;
  uVar18 = DAT_002f7e20[1];
  if (uVar9 < 3) {
    iVar13 = 0;
  }
  else {
    iVar13 = *(int *)(uVar6 + 8);
  }
  *(undefined4 *)(iVar13 + 0x28) = DAT_002f7e20[2];
  *(undefined4 *)(iVar13 + 0x20) = uVar16;
  *(undefined4 *)(iVar13 + 0x24) = uVar18;
  if (*(uint *)(iVar4 + -0x118) < 4) {
    iVar13 = 0;
  }
  else {
    iVar13 = *(int *)(*(int *)(iVar4 + -0x110) + 0xc);
  }
  uVar16 = *DAT_002f7e24;
  uVar18 = DAT_002f7e24[1];
  *(undefined4 *)(iVar13 + 0x10) = DAT_002f7e24[2];
  *(undefined4 *)(iVar13 + 8) = uVar16;
  *(undefined4 *)(iVar13 + 0xc) = uVar18;
  uVar16 = *DAT_002f7e28;
  uVar18 = DAT_002f7e28[1];
  *(undefined4 *)(iVar13 + 0x1c) = DAT_002f7e28[2];
  *(undefined4 *)(iVar13 + 0x14) = uVar16;
  *(undefined4 *)(iVar13 + 0x18) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (3 < uVar9) {
    uVar6 = *(uint *)(*(int *)(iVar4 + -0x110) + 0xc);
  }
  uVar16 = *DAT_002f7e2c;
  uVar18 = DAT_002f7e2c[1];
  if (uVar9 < 4) {
    uVar6 = 0;
  }
  *(undefined4 *)(uVar6 + 0x28) = DAT_002f7e2c[2];
  *(undefined4 *)(uVar6 + 0x20) = uVar16;
  *(undefined4 *)(uVar6 + 0x24) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (4 < uVar9) {
    uVar6 = *(uint *)(iVar4 + -0x110);
  }
  uVar16 = *DAT_002f7e30;
  uVar18 = DAT_002f7e30[1];
  if (uVar9 < 5) {
    iVar13 = 0;
  }
  else {
    iVar13 = *(int *)(uVar6 + 0x10);
  }
  *(undefined4 *)(iVar13 + 0x10) = DAT_002f7e30[2];
  *(undefined4 *)(iVar13 + 8) = uVar16;
  *(undefined4 *)(iVar13 + 0xc) = uVar18;
  uVar16 = *DAT_002f7e34;
  uVar18 = DAT_002f7e34[1];
  *(undefined4 *)(iVar13 + 0x1c) = DAT_002f7e34[2];
  *(undefined4 *)(iVar13 + 0x14) = uVar16;
  *(undefined4 *)(iVar13 + 0x18) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (4 < uVar9) {
    uVar6 = *(uint *)(*(int *)(iVar4 + -0x110) + 0x10);
  }
  uVar16 = *DAT_002f7e38;
  uVar18 = DAT_002f7e38[1];
  iVar13 = DAT_002f7e38[2];
  if (uVar9 < 5) {
    uVar6 = 0;
  }
  *(int *)(uVar6 + 0x28) = iVar13;
  *(undefined4 *)(uVar6 + 0x20) = uVar16;
  *(undefined4 *)(uVar6 + 0x24) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (5 < uVar9) {
    uVar6 = *(uint *)(iVar4 + -0x110);
  }
  uVar16 = *DAT_002f7e3c;
  uVar18 = DAT_002f7e3c[1];
  if (uVar9 < 6) {
    iVar13 = 0;
  }
  if (5 < uVar9) {
    iVar13 = *(int *)(uVar6 + 0x14);
  }
  *(undefined4 *)(iVar13 + 0x10) = DAT_002f7e3c[2];
  *(undefined4 *)(iVar13 + 8) = uVar16;
  *(undefined4 *)(iVar13 + 0xc) = uVar18;
  uVar16 = *DAT_002f7e40;
  uVar18 = DAT_002f7e40[1];
  *(undefined4 *)(iVar13 + 0x1c) = DAT_002f7e40[2];
  *(undefined4 *)(iVar13 + 0x14) = uVar16;
  *(undefined4 *)(iVar13 + 0x18) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (5 < uVar9) {
    uVar6 = *(uint *)(*(int *)(iVar4 + -0x110) + 0x14);
  }
  uVar16 = *DAT_002f7e44;
  uVar18 = DAT_002f7e44[1];
  if (uVar9 < 6) {
    uVar6 = 0;
  }
  *(undefined4 *)(uVar6 + 0x28) = DAT_002f7e44[2];
  *(undefined4 *)(uVar6 + 0x20) = uVar16;
  *(undefined4 *)(uVar6 + 0x24) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (6 < uVar9) {
    uVar6 = *(uint *)(iVar4 + -0x110);
  }
  uVar16 = *DAT_002f7e48;
  uVar18 = DAT_002f7e48[1];
  if (uVar9 < 7) {
    iVar13 = 0;
  }
  else {
    iVar13 = *(int *)(uVar6 + 0x18);
  }
  *(undefined4 *)(iVar13 + 0x10) = DAT_002f7e48[2];
  *(undefined4 *)(iVar13 + 8) = uVar16;
  *(undefined4 *)(iVar13 + 0xc) = uVar18;
  uVar16 = *DAT_002f7e4c;
  uVar18 = DAT_002f7e4c[1];
  *(undefined4 *)(iVar13 + 0x1c) = DAT_002f7e4c[2];
  *(undefined4 *)(iVar13 + 0x14) = uVar16;
  *(undefined4 *)(iVar13 + 0x18) = uVar18;
  uVar9 = *(uint *)(iVar4 + -0x118);
  uVar6 = uVar9;
  if (6 < uVar9) {
    uVar6 = *(uint *)(*(int *)(iVar4 + -0x110) + 0x18);
  }
  uVar16 = *DAT_002f7e50;
  pfVar21 = (float *)DAT_002f7e50[1];
  if (uVar9 < 7) {
    uVar6 = 0;
  }
  *(undefined4 *)(uVar6 + 0x28) = DAT_002f7e50[2];
  *(undefined4 *)(uVar6 + 0x20) = uVar16;
  *(float **)(uVar6 + 0x24) = pfVar21;
  puVar7 = *(undefined4 **)(iVar4 + -0x110);
  puVar3 = puVar7 + *(int *)(iVar4 + -0x118);
  piVar8 = DAT_002f7e54;
  for (; puVar7 != puVar3; puVar7 = puVar7 + 1) {
    piVar23 = (int *)*puVar7;
    piVar17 = (int *)*piVar23;
    piVar17[4] = *piVar8;
    pfVar21 = (float *)piVar8[1];
    piVar17[5] = (int)pfVar21;
    piVar17[6] = piVar8[2];
    func_00151358(piVar17);
    func_004FC8D8((int)local_48,*piVar23);
    piVar8 = DAT_002f7e54;
  }
  func_004FB344((int)piVar2,0x2b);
  iVar13 = DAT_002f7e58;
  uVar6 = 0;
  do {
    func_004FACA4((int)piVar2,uVar6,(int *)(iVar13 + uVar6 * 8));
    uVar6 = uVar6 + 1;
  } while ((int)uVar6 < 0x2b);
  func_004FB468((int)piVar2);
  if (local_44 != 1) {
    func_0010983C(*DAT_002f724c,local_44);
  }
  func_0036D0C8((int *)(iVar4 + -0x57c),0);
  iVar13 = *(int *)(iVar4 + -0x57c);
  if (iVar13 == 0) {
    iVar13 = 1;
  }
  else {
    iVar13 = func_0010983C(*DAT_002f724c,iVar13);
  }
  func_004FB30C((int)piVar2,4);
  iVar14 = DAT_002f7e5c;
  uVar6 = 0;
  do {
    func_004FAC70((int)piVar2,uVar6,(int *)(iVar14 + uVar6 * 8));
    uVar6 = uVar6 + 1;
  } while ((int)uVar6 < 4);
  func_004FB408((int)piVar2);
  if (iVar13 != 1) {
    func_0010983C(*DAT_002f724c,iVar13);
  }
  local_4c = &DAT_002f7e60;
  local_50 = uVar5;
  uVar9 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_50);
  uVar6 = DAT_002f7e68;
  if (uVar9 != DAT_002f7e68) {
    *(uint *)(iVar4 + -0x548) = uVar9;
    puVar10 = (uint *)(*(int *)(*(int *)(*(int *)(iVar4 + -0xa34) + 0x24) + 0xc) + uVar9 * 0x40);
    *puVar10 = *puVar10 | 0x10;
    *(int *)(*piVar24 + 0x16c) = iVar4 + -0x554;
  }
  local_4c = s_chest_002f7e6c;
  local_50 = uVar5;
  uVar9 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_50);
  if (uVar9 != uVar6) {
    *(uint *)(iVar4 + -0x544) = uVar9;
    puVar10 = (uint *)(*(int *)(*(int *)(*(int *)(iVar4 + -0xa34) + 0x24) + 0xc) + uVar9 * 0x40);
    *puVar10 = *puVar10 | 0x10;
    *(int *)(*piVar24 + 0x16c) = iVar4 + -0x554;
  }
  local_4c = s_arm_r1_002f7e74;
  local_50 = uVar5;
  uVar9 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_50);
  if (uVar9 != uVar6) {
    *(uint *)(iVar4 + -0x540) = uVar9;
    puVar10 = (uint *)(*(int *)(*(int *)(*(int *)(iVar4 + -0xa34) + 0x24) + 0xc) + uVar9 * 0x40);
    *puVar10 = *puVar10 | 0x10;
    *(int *)(*piVar24 + 0x16c) = iVar4 + -0x554;
  }
  local_4c = s_trident_002f7e7c;
  local_50 = uVar5;
  uVar9 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_50);
  if (uVar9 != uVar6) {
    *(uint *)(iVar4 + -0x2c0) = uVar9;
  }
  local_4c = s_wrist_l_002f7e84;
  local_50 = uVar5;
  uVar9 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_50);
  if (uVar9 != uVar6) {
    *(uint *)(iVar4 + -700) = uVar9;
  }
  local_4c = s_skl_root_002f7e8c;
  local_50 = uVar5;
  uVar9 = func_00564A78(*(int *)(iVar4 + -0xa34),(int *)&local_50);
  if (uVar9 != uVar6) {
    *(uint *)(iVar4 + -0x2b8) = uVar9;
  }
  func_004F5BD8((int)piVar2,(uint *)0x0,(int **)0x0,pfVar21);
  func_002EBFB8((int)piVar2);
  func_002E9BFC((int)piVar2);
  func_0032F024((float *)(iVar4 + -0x10),local_28[0xa7],*(float *)(iVar4 + -0xa6c));
  puVar3 = (undefined4 *)(iVar4 + -0x578);
  *(undefined4 *)(iVar4 + -0x280) = *(undefined4 *)(iVar4 + -0x10);
  *(undefined4 *)(iVar4 + -0x27c) = *(undefined4 *)(iVar4 + -0xc);
  *(undefined4 *)(iVar4 + -0x278) = *(undefined4 *)(iVar4 + -8);
  *(undefined1 *)(iVar4 + -0x572) = 0x23;
  *(int **)(iVar4 + -0x564) = piVar2;
  iVar13 = func_0010BA78(0x230);
  *(int *)(iVar4 + -0x560) = iVar13;
  *(uint *)(iVar4 + -0x55c) = iVar13 + (uint)*(byte *)(iVar4 + -0x572) * 8;
  iVar14 = 0x23;
  puVar7 = (undefined4 *)(iVar13 + -8);
  do {
    uVar16 = *(undefined4 *)(DAT_002f6e40 + 0x3a0);
    iVar14 = iVar14 + -1;
    puVar7[2] = *(undefined4 *)(DAT_002f6e40 + 0x39c);
    puVar7[3] = uVar16;
    uVar16 = *(undefined4 *)(iVar12 + 0x3a0);
    puVar7[4] = *(undefined4 *)(iVar12 + 0x39c);
    puVar7[5] = uVar16;
    puVar7 = puVar7 + 4;
  } while (iVar14 != 0);
  bVar1 = *(byte *)(iVar4 + -0x572);
  uVar6 = (uint)bVar1;
  puVar7 = (undefined4 *)func_0010BA78(uVar6 << 3);
  *(undefined4 **)(iVar4 + -0x558) = puVar7;
  if (uVar6 != 0) {
    puVar11 = puVar7 + -2;
    if ((bVar1 & 1) != 0) {
      uVar16 = *(undefined4 *)(iVar12 + 0x60);
      *puVar7 = *(undefined4 *)(iVar12 + 0x5c);
      puVar7[1] = uVar16;
      puVar11 = puVar7;
    }
    for (iVar13 = (int)uVar6 >> 1; iVar13 != 0; iVar13 = iVar13 + -1) {
      uVar16 = *(undefined4 *)(iVar12 + 0x60);
      puVar11[2] = *(undefined4 *)(iVar12 + 0x5c);
      puVar11[3] = uVar16;
      uVar16 = *(undefined4 *)(iVar12 + 0x60);
      puVar11[4] = *(undefined4 *)(iVar12 + 0x5c);
      puVar11[5] = uVar16;
      puVar11 = puVar11 + 4;
    }
  }
  puVar7 = (undefined4 *)func_0010CEE0(0x4f4,(int *)0x0,4);
  iVar13 = 0;
  if (puVar7 != (undefined4 *)0x0) {
    *puVar7 = 0x24;
    puVar7[1] = 0x23;
    iVar13 = func_00100118((int)(puVar7 + 2),DAT_002f7e98,0x24,0x23);
  }
  if (iVar13 != 0) {
    *(int *)(iVar4 + -0x56c) = iVar13;
    *(undefined4 *)(iVar4 + -0x570) = 0x23;
  }
  iVar13 = DAT_002f7e9c;
  *(undefined4 **)(iVar4 + -0xaec) = puVar3;
  *(undefined2 *)(iVar13 + (int)piVar2) = 0x23;
  local_4c = s_cState_Appear1st_002f7ea0;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0,(int *)&local_50);
  puVar7 = *(undefined4 **)(iVar4 + -0x560);
  uVar18 = *(undefined4 *)(iVar12 + 0x50);
  uVar16 = *(undefined4 *)(iVar12 + 0x54);
  uVar25 = *(undefined4 *)(iVar12 + 0x58);
  uVar29 = *(undefined8 *)(iVar12 + 0x5c);
  *puVar7 = *(undefined4 *)(iVar12 + 0x4c);
  puVar7[1] = uVar18;
  puVar7 = *(undefined4 **)(iVar4 + -0x55c);
  *puVar7 = uVar16;
  puVar7[1] = uVar25;
  puVar7 = *(undefined4 **)(iVar4 + -0x558);
  *puVar7 = (int)uVar29;
  puVar7[1] = (int)((ulonglong)uVar29 >> 0x20);
  local_4c = s_cState_Appear2nd_002f7eb4;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,1,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar12 + 0x68);
  uVar18 = *(undefined4 *)(iVar12 + 0x6c);
  uVar25 = *(undefined4 *)(iVar12 + 0x70);
  *(undefined4 *)(iVar13 + 8) = *(undefined4 *)(iVar12 + 100);
  *(undefined4 *)(iVar13 + 0xc) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 8) = uVar18;
  *(undefined4 *)(iVar13 + 0xc) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(int *)(iVar13 + 8) = (int)uVar29;
  *(int *)(iVar13 + 0xc) = (int)((ulonglong)uVar29 >> 0x20);
  local_4c = s_cState_Wait_002f82a0;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,2,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar12 + 0x78);
  uVar18 = *(undefined4 *)(iVar12 + 0x7c);
  uVar25 = *(undefined4 *)(iVar12 + 0x80);
  *(undefined4 *)(iVar13 + 0x10) = *(undefined4 *)(iVar12 + 0x74);
  *(undefined4 *)(iVar13 + 0x14) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x10) = uVar18;
  *(undefined4 *)(iVar13 + 0x14) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(int *)(iVar13 + 0x10) = (int)uVar29;
  *(int *)(iVar13 + 0x14) = (int)((ulonglong)uVar29 >> 0x20);
  local_4c = s_cState_Walk_002f82ac;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,3,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar12 + 0x88);
  uVar18 = *(undefined4 *)(iVar12 + 0x8c);
  uVar25 = *(undefined4 *)(iVar12 + 0x90);
  *(undefined4 *)(iVar13 + 0x18) = *(undefined4 *)(iVar12 + 0x84);
  *(undefined4 *)(iVar13 + 0x1c) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x18) = uVar18;
  *(undefined4 *)(iVar13 + 0x1c) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(int *)(iVar13 + 0x18) = (int)uVar29;
  *(int *)(iVar13 + 0x1c) = (int)((ulonglong)uVar29 >> 0x20);
  local_4c = s_cState_Guard_002f82b8;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,4,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar25 = *(undefined4 *)(iVar12 + 0x98);
  uVar16 = *(undefined4 *)(iVar12 + 0x9c);
  uVar18 = *(undefined4 *)(iVar12 + 0xa0);
  *(undefined4 *)(iVar13 + 0x20) = *(undefined4 *)(iVar12 + 0x94);
  *(undefined4 *)(iVar13 + 0x24) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x20) = uVar16;
  *(undefined4 *)(iVar13 + 0x24) = uVar18;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(int *)(iVar13 + 0x20) = (int)uVar29;
  *(int *)(iVar13 + 0x24) = (int)((ulonglong)uVar29 >> 0x20);
  local_4c = s_cState_AttackTsuki_002f82c8;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,5,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar15 = *(undefined4 *)(iVar12 + 0xa8);
  uVar18 = *(undefined4 *)(iVar12 + 0xb4);
  uVar25 = *(undefined4 *)(iVar12 + 0xb8);
  uVar16 = *(undefined4 *)(iVar12 + 0xac);
  uVar22 = *(undefined4 *)(iVar12 + 0xb0);
  *(undefined4 *)(iVar13 + 0x28) = *(undefined4 *)(iVar12 + 0xa4);
  *(undefined4 *)(iVar13 + 0x2c) = uVar15;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x28) = uVar16;
  *(undefined4 *)(iVar13 + 0x2c) = uVar22;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0x28) = uVar18;
  *(undefined4 *)(iVar13 + 0x2c) = uVar25;
  local_4c = s_cState_AttackLongTsuki_002f82dc;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,6,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar12 + 0xc0);
  uVar18 = *(undefined4 *)(iVar12 + 0xc4);
  uVar25 = *(undefined4 *)(iVar12 + 200);
  uVar22 = *(undefined4 *)(iVar12 + 0xcc);
  uVar15 = *(undefined4 *)(iVar12 + 0xd0);
  *(undefined4 *)(iVar13 + 0x30) = *(undefined4 *)(iVar12 + 0xbc);
  *(undefined4 *)(iVar13 + 0x34) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x30) = uVar18;
  *(undefined4 *)(iVar13 + 0x34) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0x30) = uVar22;
  *(undefined4 *)(iVar13 + 0x34) = uVar15;
  local_4c = s_cState_AttackHarai_002f82f4;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,7,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar18 = *(undefined4 *)(iVar12 + 0xd8);
  uVar25 = *(undefined4 *)(iVar12 + 0xdc);
  uVar22 = *(undefined4 *)(iVar12 + 0xe0);
  uVar15 = *(undefined4 *)(iVar12 + 0xe4);
  uVar16 = *(undefined4 *)(iVar12 + 0xe8);
  *(undefined4 *)(iVar13 + 0x38) = *(undefined4 *)(iVar12 + 0xd4);
  *(undefined4 *)(iVar13 + 0x3c) = uVar18;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x38) = uVar25;
  *(undefined4 *)(iVar13 + 0x3c) = uVar22;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0x38) = uVar15;
  *(undefined4 *)(iVar13 + 0x3c) = uVar16;
  local_4c = s_cState_WarpToPlayer_002f8308;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,8,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar12 + 0xf0);
  uVar22 = *(undefined4 *)(DAT_002f831c + 0xfc);
  uVar18 = *(undefined4 *)(DAT_002f831c + 0x100);
  uVar15 = *(undefined4 *)(iVar12 + 0xf4);
  uVar25 = *(undefined4 *)(iVar12 + 0xf8);
  *(undefined4 *)(iVar13 + 0x40) = *(undefined4 *)(iVar12 + 0xec);
  *(undefined4 *)(iVar13 + 0x44) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x40) = uVar15;
  *(undefined4 *)(iVar13 + 0x44) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0x40) = uVar22;
  *(undefined4 *)(iVar13 + 0x44) = uVar18;
  local_4c = s_cState_WarpAway_002f8320;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,9,(int *)&local_50);
  iVar13 = DAT_002f831c;
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(DAT_002f831c + 0x108);
  uVar18 = *(undefined4 *)(DAT_002f831c + 0x10c);
  uVar25 = *(undefined4 *)(DAT_002f831c + 0x110);
  uVar22 = *(undefined4 *)(DAT_002f831c + 0x114);
  uVar15 = *(undefined4 *)(DAT_002f831c + 0x118);
  *(undefined4 *)(iVar14 + 0x48) = *(undefined4 *)(DAT_002f831c + 0x104);
  *(undefined4 *)(iVar14 + 0x4c) = uVar16;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x48) = uVar18;
  *(undefined4 *)(iVar14 + 0x4c) = uVar25;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x48) = uVar22;
  *(undefined4 *)(iVar14 + 0x4c) = uVar15;
  local_4c = s_cState_WarpAway2nd_002f8330;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,10,(int *)&local_50);
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar13 + 0x120);
  uVar18 = *(undefined4 *)(iVar13 + 0x124);
  uVar25 = *(undefined4 *)(iVar13 + 0x128);
  uVar22 = *(undefined4 *)(iVar13 + 300);
  uVar15 = *(undefined4 *)(iVar13 + 0x130);
  *(undefined4 *)(iVar14 + 0x50) = *(undefined4 *)(iVar13 + 0x11c);
  *(undefined4 *)(iVar14 + 0x54) = uVar16;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x50) = uVar18;
  *(undefined4 *)(iVar14 + 0x54) = uVar25;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x50) = uVar22;
  *(undefined4 *)(iVar14 + 0x54) = uVar15;
  local_4c = s_cState_WarpAwayDark_002f8344;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0xb,(int *)&local_50);
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar13 + 0x138);
  uVar18 = *(undefined4 *)(iVar13 + 0x13c);
  uVar25 = *(undefined4 *)(iVar13 + 0x140);
  uVar22 = *(undefined4 *)(iVar13 + 0x144);
  uVar15 = *(undefined4 *)(iVar13 + 0x148);
  *(undefined4 *)(iVar14 + 0x58) = *(undefined4 *)(iVar13 + 0x134);
  *(undefined4 *)(iVar14 + 0x5c) = uVar16;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x58) = uVar18;
  *(undefined4 *)(iVar14 + 0x5c) = uVar25;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x58) = uVar22;
  *(undefined4 *)(iVar14 + 0x5c) = uVar15;
  local_4c = s_cState_ThrowTrident_002f8358;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0xc,(int *)&local_50);
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar13 + 0x150);
  uVar18 = *(undefined4 *)(iVar13 + 0x154);
  uVar25 = *(undefined4 *)(iVar13 + 0x158);
  uVar22 = *(undefined4 *)(iVar13 + 0x15c);
  uVar15 = *(undefined4 *)(iVar13 + 0x160);
  *(undefined4 *)(iVar14 + 0x60) = *(undefined4 *)(iVar13 + 0x14c);
  *(undefined4 *)(iVar14 + 100) = uVar16;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x60) = uVar18;
  *(undefined4 *)(iVar14 + 100) = uVar25;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x60) = uVar22;
  *(undefined4 *)(iVar14 + 100) = uVar15;
  local_4c = s_cState_RallyWait_002f836c;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0xd,(int *)&local_50);
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar13 + 0x168);
  uVar18 = *(undefined4 *)(iVar13 + 0x16c);
  uVar25 = *(undefined4 *)(iVar13 + 0x170);
  uVar22 = *(undefined4 *)(iVar13 + 0x174);
  uVar15 = *(undefined4 *)(iVar13 + 0x178);
  *(undefined4 *)(iVar14 + 0x68) = *(undefined4 *)(iVar13 + 0x164);
  *(undefined4 *)(iVar14 + 0x6c) = uVar16;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x68) = uVar18;
  *(undefined4 *)(iVar14 + 0x6c) = uVar25;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x68) = uVar22;
  *(undefined4 *)(iVar14 + 0x6c) = uVar15;
  local_4c = s_cState_ShootBall_002f8380;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0xe,(int *)&local_50);
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar25 = *(undefined4 *)(iVar13 + 0x184);
  uVar22 = *(undefined4 *)(iVar13 + 0x188);
  uVar18 = *(undefined4 *)(DAT_002f831c + 0x180);
  uVar16 = *(undefined4 *)(iVar13 + 0x18c);
  uVar15 = *(undefined4 *)(iVar13 + 400);
  *(undefined4 *)(iVar14 + 0x70) = *(undefined4 *)(DAT_002f831c + 0x17c);
  *(undefined4 *)(iVar14 + 0x74) = uVar18;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x70) = uVar25;
  *(undefined4 *)(iVar14 + 0x74) = uVar22;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x70) = uVar16;
  *(undefined4 *)(iVar14 + 0x74) = uVar15;
  local_4c = s_cState_PreHitBall_002f878c;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0xf,(int *)&local_50);
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar13 + 0x198);
  uVar18 = *(undefined4 *)(iVar13 + 0x19c);
  uVar25 = *(undefined4 *)(iVar13 + 0x1a0);
  uVar22 = *(undefined4 *)(iVar13 + 0x1a4);
  uVar15 = *(undefined4 *)(iVar13 + 0x1a8);
  *(undefined4 *)(iVar14 + 0x78) = *(undefined4 *)(iVar13 + 0x194);
  *(undefined4 *)(iVar14 + 0x7c) = uVar16;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x78) = uVar18;
  *(undefined4 *)(iVar14 + 0x7c) = uVar25;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x78) = uVar22;
  *(undefined4 *)(iVar14 + 0x7c) = uVar15;
  local_4c = s_cState_HitBall_002f87a0;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x10,(int *)&local_50);
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar13 + 0x1b0);
  uVar18 = *(undefined4 *)(iVar13 + 0x1b4);
  uVar25 = *(undefined4 *)(iVar13 + 0x1b8);
  uVar22 = *(undefined4 *)(iVar13 + 0x1bc);
  uVar15 = *(undefined4 *)(iVar13 + 0x1c0);
  *(undefined4 *)(iVar14 + 0x80) = *(undefined4 *)(iVar13 + 0x1ac);
  *(undefined4 *)(iVar14 + 0x84) = uVar16;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x80) = uVar18;
  *(undefined4 *)(iVar14 + 0x84) = uVar25;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x80) = uVar22;
  *(undefined4 *)(iVar14 + 0x84) = uVar15;
  local_4c = s_cState_ShootMBall_002f87b0;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x11,(int *)&local_50);
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar13 + 0x1c8);
  uVar18 = *(undefined4 *)(iVar13 + 0x1cc);
  uVar25 = *(undefined4 *)(iVar13 + 0x1d0);
  uVar22 = *(undefined4 *)(iVar13 + 0x1d4);
  uVar15 = *(undefined4 *)(iVar13 + 0x1d8);
  *(undefined4 *)(iVar14 + 0x88) = *(undefined4 *)(iVar13 + 0x1c4);
  *(undefined4 *)(iVar14 + 0x8c) = uVar16;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x88) = uVar18;
  *(undefined4 *)(iVar14 + 0x8c) = uVar25;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x88) = uVar22;
  *(undefined4 *)(iVar14 + 0x8c) = uVar15;
  local_4c = s_cState_RallyMBall_002f87c4;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x12,(int *)&local_50);
  iVar14 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar13 + 0x1e0);
  uVar18 = *(undefined4 *)(iVar13 + 0x1e4);
  uVar25 = *(undefined4 *)(iVar13 + 0x1e8);
  uVar22 = *(undefined4 *)(iVar13 + 0x1ec);
  uVar15 = *(undefined4 *)(iVar13 + 0x1f0);
  *(undefined4 *)(iVar14 + 0x90) = *(undefined4 *)(iVar13 + 0x1dc);
  *(undefined4 *)(iVar14 + 0x94) = uVar16;
  iVar14 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar14 + 0x90) = uVar18;
  *(undefined4 *)(iVar14 + 0x94) = uVar25;
  iVar14 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar14 + 0x90) = uVar22;
  *(undefined4 *)(iVar14 + 0x94) = uVar15;
  local_4c = s_cState_Stun_002f87d8;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x1e,(int *)&local_50);
  iVar14 = DAT_002f831c;
  iVar20 = *(int *)(iVar4 + -0x560);
  uVar18 = *(undefined4 *)(DAT_002f831c + 0x204);
  uVar25 = *(undefined4 *)(DAT_002f831c + 0x208);
  uVar15 = *(undefined4 *)(DAT_002f831c + 0x1fc);
  uVar16 = *(undefined4 *)(DAT_002f831c + 0x200);
  uVar22 = *(undefined4 *)(iVar13 + 0x1f8);
  *(undefined4 *)(iVar20 + 0xf0) = *(undefined4 *)(iVar13 + 500);
  *(undefined4 *)(iVar20 + 0xf4) = uVar22;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xf0) = uVar15;
  *(undefined4 *)(iVar13 + 0xf4) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xf0) = uVar18;
  *(undefined4 *)(iVar13 + 0xf4) = uVar25;
  local_4c = s_cState_StunLast_002f87e4;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x1f,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar14 + 0x210);
  uVar18 = *(undefined4 *)(iVar14 + 0x214);
  uVar25 = *(undefined4 *)(iVar14 + 0x218);
  uVar22 = *(undefined4 *)(iVar14 + 0x21c);
  uVar15 = *(undefined4 *)(iVar14 + 0x220);
  *(undefined4 *)(iVar13 + 0xf8) = *(undefined4 *)(iVar14 + 0x20c);
  *(undefined4 *)(iVar13 + 0xfc) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xf8) = uVar18;
  *(undefined4 *)(iVar13 + 0xfc) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xf8) = uVar22;
  *(undefined4 *)(iVar13 + 0xfc) = uVar15;
  local_4c = s_cState_AttackMagic_002f87f4;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x20,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar14 + 0x22c);
  uVar18 = *(undefined4 *)(iVar14 + 0x230);
  uVar25 = *(undefined4 *)(DAT_002f831c + 0x228);
  uVar22 = *(undefined4 *)(iVar14 + 0x234);
  uVar15 = *(undefined4 *)(iVar14 + 0x238);
  *(undefined4 *)(iVar13 + 0x100) = *(undefined4 *)(DAT_002f831c + 0x224);
  *(undefined4 *)(iVar13 + 0x104) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x100) = uVar16;
  *(undefined4 *)(iVar13 + 0x104) = uVar18;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0x100) = uVar22;
  *(undefined4 *)(iVar13 + 0x104) = uVar15;
  local_4c = s_cState_ToWall_002f8808;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x13,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar14 + 0x240);
  uVar18 = *(undefined4 *)(iVar14 + 0x244);
  uVar25 = *(undefined4 *)(iVar14 + 0x248);
  uVar22 = *(undefined4 *)(iVar14 + 0x24c);
  uVar15 = *(undefined4 *)(iVar14 + 0x250);
  *(undefined4 *)(iVar13 + 0x98) = *(undefined4 *)(iVar14 + 0x23c);
  *(undefined4 *)(iVar13 + 0x9c) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x98) = uVar18;
  *(undefined4 *)(iVar13 + 0x9c) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0x98) = uVar22;
  *(undefined4 *)(iVar13 + 0x9c) = uVar15;
  local_4c = s_cState_WallIn_002f8818;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x14,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar14 + 600);
  uVar18 = *(undefined4 *)(iVar14 + 0x25c);
  uVar25 = *(undefined4 *)(iVar14 + 0x260);
  uVar22 = *(undefined4 *)(iVar14 + 0x264);
  uVar15 = *(undefined4 *)(iVar14 + 0x268);
  *(undefined4 *)(iVar13 + 0xa0) = *(undefined4 *)(iVar14 + 0x254);
  *(undefined4 *)(iVar13 + 0xa4) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xa0) = uVar18;
  *(undefined4 *)(iVar13 + 0xa4) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xa0) = uVar22;
  *(undefined4 *)(iVar13 + 0xa4) = uVar15;
  local_4c = s_cState_WallWaitFirst_002f8828;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x15,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar14 + 0x270);
  uVar18 = *(undefined4 *)(iVar14 + 0x274);
  uVar25 = *(undefined4 *)(iVar14 + 0x278);
  uVar22 = *(undefined4 *)(iVar14 + 0x27c);
  uVar15 = *(undefined4 *)(iVar14 + 0x280);
  *(undefined4 *)(iVar13 + 0xa8) = *(undefined4 *)(iVar14 + 0x26c);
  *(undefined4 *)(iVar13 + 0xac) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xa8) = uVar18;
  *(undefined4 *)(iVar13 + 0xac) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xa8) = uVar22;
  *(undefined4 *)(iVar13 + 0xac) = uVar15;
  local_4c = s_cState_WallWait2_002f8840;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x16,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar18 = *(undefined4 *)(iVar14 + 0x288);
  uVar25 = *(undefined4 *)(iVar14 + 0x28c);
  uVar16 = *(undefined4 *)(iVar14 + 0x290);
  uVar22 = *(undefined4 *)(iVar14 + 0x294);
  uVar15 = *(undefined4 *)(iVar14 + 0x298);
  *(undefined4 *)(iVar13 + 0xb0) = *(undefined4 *)(iVar14 + 0x284);
  *(undefined4 *)(iVar13 + 0xb4) = uVar18;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xb0) = uVar25;
  *(undefined4 *)(iVar13 + 0xb4) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xb0) = uVar22;
  *(undefined4 *)(iVar13 + 0xb4) = uVar15;
  local_4c = s_cState_WallWaitLast_002f8854;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x17,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar14 + 0x2a0);
  uVar18 = *(undefined4 *)(iVar14 + 0x2a4);
  uVar25 = *(undefined4 *)(iVar14 + 0x2a8);
  uVar22 = *(undefined4 *)(iVar14 + 0x2ac);
  uVar15 = *(undefined4 *)(iVar14 + 0x2b0);
  *(undefined4 *)(iVar13 + 0xb8) = *(undefined4 *)(iVar14 + 0x29c);
  *(undefined4 *)(iVar13 + 0xbc) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xb8) = uVar18;
  *(undefined4 *)(iVar13 + 0xbc) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xb8) = uVar22;
  *(undefined4 *)(iVar13 + 0xbc) = uVar15;
  local_4c = s_cState_WallSummons_002f8868;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x18,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar14 + 0x2b8);
  uVar18 = *(undefined4 *)(iVar14 + 700);
  uVar25 = *(undefined4 *)(iVar14 + 0x2c0);
  uVar22 = *(undefined4 *)(iVar14 + 0x2c4);
  uVar15 = *(undefined4 *)(iVar14 + 0x2c8);
  *(undefined4 *)(iVar13 + 0xc0) = *(undefined4 *)(iVar14 + 0x2b4);
  *(undefined4 *)(iVar13 + 0xc4) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xc0) = uVar18;
  *(undefined4 *)(iVar13 + 0xc4) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xc0) = uVar22;
  *(undefined4 *)(iVar13 + 0xc4) = uVar15;
  local_4c = s_cState_WallAttack_002f8c10;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x19,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar25 = *(undefined4 *)(iVar14 + 0x2d4);
  uVar18 = *(undefined4 *)(iVar14 + 0x2d8);
  uVar15 = *(undefined4 *)(DAT_002f831c + 0x2d0);
  uVar16 = *(undefined4 *)(iVar14 + 0x2dc);
  uVar22 = *(undefined4 *)(iVar14 + 0x2e0);
  *(undefined4 *)(iVar13 + 200) = *(undefined4 *)(DAT_002f831c + 0x2cc);
  *(undefined4 *)(iVar13 + 0xcc) = uVar15;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 200) = uVar25;
  *(undefined4 *)(iVar13 + 0xcc) = uVar18;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 200) = uVar16;
  *(undefined4 *)(iVar13 + 0xcc) = uVar22;
  local_4c = s_cState_WallJump_002f8c24;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x1a,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar14 + 0x2e8);
  uVar18 = *(undefined4 *)(iVar14 + 0x2ec);
  uVar25 = *(undefined4 *)(iVar14 + 0x2f0);
  uVar22 = *(undefined4 *)(iVar14 + 0x2f4);
  uVar15 = *(undefined4 *)(iVar14 + 0x2f8);
  *(undefined4 *)(iVar13 + 0xd0) = *(undefined4 *)(iVar14 + 0x2e4);
  *(undefined4 *)(iVar13 + 0xd4) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xd0) = uVar18;
  *(undefined4 *)(iVar13 + 0xd4) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xd0) = uVar22;
  *(undefined4 *)(iVar13 + 0xd4) = uVar15;
  local_4c = s_cState_WallOut_002f8c34;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x1b,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar15 = DAT_002f8c44[1];
  uVar25 = DAT_002f8c44[2];
  uVar22 = DAT_002f8c44[3];
  uVar16 = *(undefined4 *)(iVar12 + 0x30c);
  uVar18 = *(undefined4 *)(iVar12 + 0x310);
  *(undefined4 *)(iVar13 + 0xd8) = *DAT_002f8c44;
  *(undefined4 *)(iVar13 + 0xdc) = uVar15;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xd8) = uVar25;
  *(undefined4 *)(iVar13 + 0xdc) = uVar22;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xd8) = uVar16;
  *(undefined4 *)(iVar13 + 0xdc) = uVar18;
  local_4c = s_cState_WallOutLast_002f8c48;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x1c,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar12 + 0x318);
  uVar18 = *(undefined4 *)(iVar12 + 0x31c);
  uVar25 = *(undefined4 *)(iVar12 + 800);
  uVar22 = *(undefined4 *)(iVar12 + 0x324);
  uVar15 = *(undefined4 *)(iVar12 + 0x328);
  *(undefined4 *)(iVar13 + 0xe0) = *(undefined4 *)(iVar12 + 0x314);
  *(undefined4 *)(iVar13 + 0xe4) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xe0) = uVar18;
  *(undefined4 *)(iVar13 + 0xe4) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xe0) = uVar22;
  *(undefined4 *)(iVar13 + 0xe4) = uVar15;
  local_4c = s_cState_WallArrowEvent_002f8c5c;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x1d,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar16 = *(undefined4 *)(iVar12 + 0x330);
  uVar18 = *(undefined4 *)(iVar12 + 0x334);
  uVar25 = *(undefined4 *)(iVar12 + 0x338);
  uVar22 = *(undefined4 *)(iVar12 + 0x33c);
  uVar15 = *(undefined4 *)(iVar12 + 0x340);
  *(undefined4 *)(iVar13 + 0xe8) = *(undefined4 *)(iVar12 + 0x32c);
  *(undefined4 *)(iVar13 + 0xec) = uVar16;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0xe8) = uVar18;
  *(undefined4 *)(iVar13 + 0xec) = uVar25;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0xe8) = uVar22;
  *(undefined4 *)(iVar13 + 0xec) = uVar15;
  local_4c = s_cState_Reaction_002f8c74;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x21,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  uVar18 = *(undefined4 *)(iVar12 + 0x348);
  uVar25 = *(undefined4 *)(iVar12 + 0x34c);
  uVar22 = *(undefined4 *)(iVar12 + 0x350);
  uVar15 = *(undefined4 *)(iVar12 + 0x354);
  uVar16 = *(undefined4 *)(iVar12 + 0x358);
  *(undefined4 *)(iVar13 + 0x108) = *(undefined4 *)(iVar12 + 0x344);
  *(undefined4 *)(iVar13 + 0x10c) = uVar18;
  iVar13 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar13 + 0x108) = uVar25;
  *(undefined4 *)(iVar13 + 0x10c) = uVar22;
  iVar13 = *(int *)(iVar4 + -0x558);
  *(undefined4 *)(iVar13 + 0x108) = uVar15;
  *(undefined4 *)(iVar13 + 0x10c) = uVar16;
  local_4c = s_cState_Die_002f8c84;
  local_50 = uVar5;
  func_0010BEC4((int)puVar3,0x22,(int *)&local_50);
  iVar13 = *(int *)(iVar4 + -0x560);
  fVar27 = *(float *)(iVar12 + 0x5c);
  uVar18 = *(undefined4 *)(iVar12 + 0x364);
  uVar22 = *(undefined4 *)(iVar12 + 0x368);
  uVar16 = *(undefined4 *)(iVar12 + 0x60);
  uVar25 = *(undefined4 *)(iVar12 + 0x360);
  *(undefined4 *)(iVar13 + 0x110) = *(undefined4 *)(iVar12 + 0x35c);
  *(undefined4 *)(iVar13 + 0x114) = uVar25;
  iVar12 = *(int *)(iVar4 + -0x55c);
  *(undefined4 *)(iVar12 + 0x110) = uVar18;
  *(undefined4 *)(iVar12 + 0x114) = uVar22;
  iVar12 = *(int *)(iVar4 + -0x558);
  *(float *)(iVar12 + 0x110) = fVar27;
  *(undefined4 *)(iVar12 + 0x114) = uVar16;
  if (*(char *)(iVar4 + -0xfb) == '\x01') {
    func_006640EC(puVar3,1,fVar27);
  }
  else {
    func_006640EC(puVar3,0,fVar27);
  }
  func_002F36B4((int)piVar2);
  *(uint *)(iVar4 + -0x8a4) = *(uint *)(iVar4 + -0x8a4) | 0x68000;
  func_004FB024((int)piVar2);
  *(int *)(iVar4 + -0xa00) = iVar4 + -0x188;
  *(int *)(iVar4 + -0x9fc) = iVar4 + -0x178;
  func_004F8864((int)piVar2,DAT_002f8c90,0,DAT_002f8c94);
  *(undefined1 *)(iVar4 + -0x5f8) = 1;
  *(uint *)(iVar4 + -0x8a4) = *(uint *)(iVar4 + -0x8a4) & 0xfff7ffff;
  func_004FAD90((int)piVar2);
  *(uint *)(*(int *)(iVar4 + -0xa1c) + 0x2c) = *(uint *)(*(int *)(iVar4 + -0xa1c) + 0x2c) | 0x10000;
  *(uint *)(iVar4 + -0x8a4) = *(uint *)(iVar4 + -0x8a4) | 0x100000;
  if ((char)local_48[0x2a] == '\0') {
    iVar12 = 0;
  }
  else {
    iVar12 = *local_48;
  }
  *(uint *)(iVar12 + 0x164) = *(uint *)(iVar12 + 0x164) | 0x90000;
  *(undefined1 *)(iVar4 + -0x5ab) = 5;
  func_002AFEC4((int)piVar2,-1,-1);
  puVar7 = (undefined4 *)func_004FAF94();
  func_004FAFA0((int)piVar2,puVar7);
  func_004FAB30((int)piVar2,0,0,0);
  func_004FAFAC((int)piVar2,0,0,0);
  func_004ECF6C((int)piVar2);
  return piVar2;
}

