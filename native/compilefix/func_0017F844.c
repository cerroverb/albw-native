/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern int func_00100174(int *param_1);
extern int func_0011558C(float *param_1,float *param_2);
extern int func_001157FC(float *param_1,float *param_2,float *param_3);
extern int func_0011593C(undefined4 *param_1);
extern int func_00115C54(float *param_1,float *param_2,float *param_3);
extern int func_00115D70(float *param_1,float *param_2,float fparam_0);
extern int func_0017EBCC(int param_1);
extern int func_00180B48(int param_1);
extern int func_001BB724(int param_1,uint param_2,int param_3);
extern int func_001BB754(int param_1,uint param_2,int param_3);
extern int func_001BB8F4(int param_1,uint param_2,int param_3);
extern int func_00352210(int param_1,uint param_2,float fparam_0);
extern int func_003AAE2C(int param_1,undefined4 *param_2);
extern int func_0048B330(float *param_1,float *param_2,float *param_3,float fparam_0);
extern int func_00491B3C(float *param_1,float *param_2);
extern int func_00491C08(float *param_1,float *param_2);
extern int func_004BC8D8(int param_1);
extern int func_004C4D6C(int param_1,uint param_2,uint param_3,int param_4);
extern int func_004CDF40(int param_1,int param_2,int param_3);
extern void func_004CE804(int param_1,int param_2,int param_3,int param_4,float fparam_0);
extern int func_004CEAF0(int param_1);
extern int func_0052107C(int param_1);
extern int func_00522E00(int param_1);
extern int func_0052A05C(int param_1,uint param_2);
extern int func_0052A07C(int param_1,uint param_2);
extern int func_0052A1B4(int param_1,uint param_2);
extern int func_00584B94(int param_1,int param_2);
extern int func_00588BE4(int param_1);
extern float *pfRam0017ff88;
extern int *piRam0017ff8c;
extern undefined4 *puRam0017fe28;
extern int *DAT_0017fddc;
extern int *DAT_0017fde0;
extern float DAT_0017fde4;
extern uint *DAT_0017fde8;
extern float *DAT_0017fdec;
extern uint *DAT_0017fdf0;
extern float *DAT_0017fdf4;
extern int *DAT_0017fdf8;
extern int *DAT_0017fdfc;
extern float DAT_0017fe00;
extern int *DAT_0017fe04;
extern int DAT_0017fe08;
extern int *DAT_0017fe0c;
extern undefined4 DAT_0017fe10;
extern float DAT_0017fe14;
extern float DAT_0017fe18;
extern undefined4 DAT_0017fe1c;
extern float DAT_0017fe20;
extern float DAT_0017fe24;
extern int *DAT_004adb14;
extern int *DAT_004adb18;

// USA 0017F844 <- EU 0017F844 (Ghidra, non-matching)
// GLOBAL DAT_0017fddc int *
// GLOBAL DAT_0017fde0 int *
// GLOBAL DAT_0017fde4 float
// GLOBAL DAT_0017fde8 uint *
// GLOBAL DAT_0017fdec float *
// GLOBAL DAT_0017fdf0 uint *
// GLOBAL DAT_0017fdf4 float *
// GLOBAL DAT_0017fdf8 int *
// GLOBAL DAT_0017fdfc int *
// GLOBAL DAT_0017fe00 float
// GLOBAL DAT_0017fe04 int *
// GLOBAL DAT_0017fe08 int
// GLOBAL DAT_0017fe0c int *
// GLOBAL DAT_0017fe10 undefined4
// GLOBAL DAT_0017fe14 float
// GLOBAL DAT_0017fe18 float
// GLOBAL DAT_0017fe1c undefined4
// GLOBAL DAT_0017fe20 float
// GLOBAL DAT_0017fe24 float
// GLOBAL DAT_004adb14 int *
// GLOBAL DAT_004adb18 int *
// GLOBAL UNK_0017fe28 undefined4 *
// GLOBAL UNK_0017ff88 float *
// GLOBAL UNK_0017ff8c int *

undefined4 func_0017F844(int param_1)

{
  uint uVar1;
  byte bVar2;
  ushort uVar3;
  char cVar4;
  uint *puVar5;
  bool bVar6;
  bool bVar7;
  uint uVar8;
  int *piVar9;
  int iVar10;
  undefined4 *puVar11;
  int iVar12;
  undefined4 uVar13;
  uint uVar14;
  int *piVar15;
  uint uVar16;
  undefined4 *puVar17;
  undefined4 *puVar18;
  int iVar19;
  undefined4 *puVar20;
  int *piVar21;
  int *piVar22;
  uint in_fpscr;
  float in_s0;
  float fVar23;
  float fVar24;
  float fVar25;
  ulonglong uVar26;
  _Alignas(8) unsigned char albw_frame[0xc0];
  #define local_c0 (*(float *)(albw_frame + 0x0))
  #define local_bc (*(int *)(albw_frame + 0x4))
  #define uStack_b8 (*(undefined4 *)(albw_frame + 0x8))
  #define afStack_b0 ((float *)(albw_frame + 0x10))
  #define fStack_a4 (*(float *)(albw_frame + 0x1c))
  #define iStack_94 (*(int *)(albw_frame + 0x2c))
  #define uStack_84 (*(undefined4 *)(albw_frame + 0x3c))
  #define afStack_80 ((float *)(albw_frame + 0x40))
  #define uStack_74 (*(undefined4 *)(albw_frame + 0x4c))
  #define uStack_64 (*(undefined4 *)(albw_frame + 0x5c))
  #define uStack_54 (*(undefined4 *)(albw_frame + 0x6c))
  #define afStack_50 ((float *)(albw_frame + 0x70))
  #define afStack_40 ((float *)(albw_frame + 0x80))
  piVar22 = (int *)(param_1 + 0x7a0);
  if (*(uint *)(param_1 + 0x9d4) < *(uint *)(param_1 + 0x9d8)) {
    *(uint *)(param_1 + 0x9d4) = *(uint *)(param_1 + 0x9d4) + 1;
  }
  if (*(uint *)(param_1 + 0x998) < *(uint *)(param_1 + 0x99c)) {
    *(uint *)(param_1 + 0x998) = *(uint *)(param_1 + 0x998) + 1;
  }
  *(undefined1 *)(param_1 + 0xbf1) = 0;
  if ((*(char *)(param_1 + 0x964) == '\0') && (*(char *)(param_1 + 0xbed) != '\0')) {
    *(undefined1 *)(param_1 + 0xbed) = 0;
    *(undefined1 *)(param_1 + 0xbec) = 0;
    *(undefined1 *)(param_1 + 0x853) = 0;
    *(undefined4 *)(param_1 + 0x368) = *(undefined4 *)(param_1 + 8);
    *(undefined4 *)(param_1 + 0x36c) = *(undefined4 *)(param_1 + 0xc);
    *(undefined4 *)(param_1 + 0x370) = *(undefined4 *)(param_1 + 0x10);
    *(undefined4 *)(param_1 + 0x3d0) = *(undefined4 *)(param_1 + 8);
    *(undefined4 *)(param_1 + 0x3d4) = *(undefined4 *)(param_1 + 0xc);
    *(undefined4 *)(param_1 + 0x3d8) = *(undefined4 *)(param_1 + 0x10);
    uVar8 = func_00522E00(*DAT_0017fddc);
    func_004CE804(*(int *)(param_1 + 0x7a0),uVar8 & 0xffff,0,0,in_s0);
  }
  *(undefined1 *)(param_1 + 0xbf5) = 0;
  uVar8 = *(uint *)(*(int *)(param_1 + 0x97c) + *(char *)(param_1 + 0x964) * 8 + 4);
  piVar15 = (int *)(*(int *)(param_1 + 0x974) + ((int)uVar8 >> 1));
  piVar9 = (int *)(*(int *)(param_1 + 0x97c) + *(char *)(param_1 + 0x964) * 8);
  if ((uVar8 & 1) != 0) {
    piVar9 = (int *)(*piVar9 + *piVar15);
  }
  (*(code *)*piVar9)(piVar15);
  iVar10 = *(int *)(param_1 + 0x960) + 1;
  *(int *)(param_1 + 0x960) = iVar10;
  if (*(int *)(param_1 + 0x970) != 0) {
    fVar23 = (float)VectorUnsignedToFloat(iVar10,(byte)(in_fpscr >> 0x15) & 3);
    func_00352210(*(int *)(param_1 + 0x970),1,fVar23);
  }
  if (*(char *)(param_1 + 0xbf2) != '\0') {
    *(undefined1 *)(param_1 + 0xbf2) = 0;
    puVar5 = DAT_0017fde8;
    iVar10 = *DAT_0017fde0;
    local_c0 = DAT_0017fde4;
    if (iVar10 != 0) {
      puVar20 = *(undefined4 **)(iVar10 + 0x3dbc8);
      for (puVar17 = *(undefined4 **)(iVar10 + 0x3dbc0); puVar17 != puVar20; puVar17 = puVar17 + 1)
      {
        puVar18 = (undefined4 *)*puVar17;
        if (puVar18 != (undefined4 *)0x0) {
          if (((*puVar5 & 1) == 0) && (iVar10 = func_00100174((int *)DAT_0017fde8), iVar10 != 0)) {
            *DAT_0017fdec = local_c0;
          }
          iVar10 = (**(code **)*puVar18)(puVar18,DAT_0017fdec);
          if (iVar10 == 0) {
            puVar18 = (undefined4 *)0x0;
          }
          if (puVar18 != (undefined4 *)0x0) {
            local_bc = *(int *)(param_1 + 0x2cc);
            func_003AAE2C((int)puVar18,&local_bc);
          }
        }
      }
    }
    puVar17 = *(undefined4 **)(*DAT_0017fde0 + 0x3dbc0);
    puVar20 = *(undefined4 **)(*DAT_0017fde0 + 0x3dbc8);
    if (puVar17 != puVar20) {
      do {
        puVar18 = (undefined4 *)*puVar17;
        if (puVar18 != (undefined4 *)0x0) {
          local_bc = func_00588BE4((int)puVar18);
          if (((*DAT_0017fdf0 & 1) == 0) &&
             (iVar10 = func_00100174((int *)DAT_0017fdf0), iVar10 != 0)) {
            *DAT_0017fdf4 = local_c0;
          }
          iVar10 = (**(code **)*puVar18)(puVar18,DAT_0017fdf4);
          puVar11 = (undefined4 *)0x0;
          if (iVar10 != 0) {
            puVar11 = puVar18;
          }
          if (puVar11 == (undefined4 *)0x0) {
            if ((local_bc == *DAT_0017fdf8) || (local_bc == *DAT_0017fdfc)) {
              puVar18[0x103] = param_1 + 0xbd4;
            }
          }
          else {
            puVar11[0x107] = param_1 + 0xbd4;
          }
        }
        puVar17 = puVar17 + 1;
      } while (puVar17 != puVar20);
    }
  }
  iVar10 = DAT_0017fe08;
  piVar9 = DAT_0017fe04;
  fVar23 = DAT_0017fe00;
  uVar26 = (ulonglong)(uint)ALBW_AS_U32(DAT_0017fe00);
  if (*(char *)(param_1 + 0xbec) != '\0') {
    if (*(uint *)(param_1 + 0x9bc) < *(uint *)(param_1 + 0x9c0)) {
      *(uint *)(param_1 + 0x9bc) = *(uint *)(param_1 + 0x9bc) + 1;
    }
    else {
      piVar21 = *(int **)(*DAT_0017fde0 + 0x3dbc8);
      uVar13 = DAT_0017fe10;
      for (piVar15 = *(int **)(*DAT_0017fde0 + 0x3dbc0); piVar15 != piVar21;
          piVar15 = piVar15 + 1) {
        iVar19 = *piVar15;
        if ((iVar19 != 0) &&
           ((iVar12 = func_00588BE4(iVar19), iVar12 == *piVar9 ||
            (iVar12 = func_00588BE4(iVar19), iVar12 == *DAT_0017fe0c)))) {
          fVar23 = *(float *)(param_1 + 0x3d0) - *(float *)(iVar19 + 0x3d0);
          fVar24 = *(float *)(param_1 + 0x3d4) - *(float *)(iVar19 + 0x3d4);
          fVar25 = *(float *)(param_1 + 0x3d8) - *(float *)(iVar19 + 0x3d8);
          fVar23 = fVar23 * fVar23 + fVar24 * fVar24 + fVar25 * fVar25;
          if ((int)ALBW_AS_U32(fVar23) < iVar10) {
            uVar13 = *(undefined4 *)(param_1 + 0x2cc);
          }
          else {
            uVar13 = 0;
          }
          *(undefined4 *)(iVar19 + 0x988) = uVar13;
          if (iVar10 <= (int)ALBW_AS_U32(fVar23)) {
            *(undefined4 *)(iVar19 + 0x908) = uVar13;
          }
        }
        fVar23 = ALBW_AS_F32(uVar26);
        uVar13 = DAT_0017fe10;
      }
      *(undefined4 *)(param_1 + 0x9c0) = 0x3c;
      *(undefined4 *)(param_1 + 0x9c4) = uVar13;
      *(undefined4 *)(param_1 + 0x9bc) = 0;
    }
  }
  cVar4 = *(char *)(param_1 + 0x964);
  fVar24 = DAT_0017fe14;
  if ((cVar4 == '\0' || cVar4 == '\a') || ((cVar4 == '\v' || cVar4 == '\b') || cVar4 == '\x05')) {
    bVar6 = func_0052107C(param_1);
    uVar13 = DAT_0017fe1c;
    fVar25 = DAT_0017fe18;
    if (bVar6) {
      if (*(char *)(param_1 + 0x964) == '\0' || *(char *)(param_1 + 0x964) == '\v') {
        *(float *)(param_1 + 0x3e4) = fVar24;
        *(float *)(param_1 + 0x3dc) = fVar24;
      }
      *(undefined4 *)(param_1 + 0x3e0) = uVar13;
      *(float *)(param_1 + 0x3d4) = *(float *)(param_1 + 0x3d4) - fVar25;
    }
    else {
      fVar25 = *(float *)(param_1 + 0x3e0) - DAT_0017fe20;
      in_fpscr = in_fpscr & 0xfffffff | (uint)(DAT_0017fe24 <= fVar25) << 0x1d;
      if (!SUB41(in_fpscr >> 0x1d,0)) {
        fVar25 = DAT_0017fe24;
      }
      *(float *)(param_1 + 0x3e0) = fVar25;
    }
    func_00115C54((float *)(param_1 + 0x3d0),(float *)(param_1 + 0x3d0),(float *)(param_1 + 0x3dc));
    func_00115C54((float *)(param_1 + 0x3d0),(float *)(param_1 + 0x3d0),(float *)(param_1 + 0xa34));
  }
  func_0017EBCC(param_1);
  func_00180B48(param_1);
  func_004BC8D8(param_1);
  puVar17 = puRam0017fe28;
  if (*(char *)(param_1 + 0x964) == '\t') {
    iVar10 = *(int *)(param_1 + 0x7a0);
    *(uint *)(iVar10 + 0x164) = *(uint *)(iVar10 + 0x164) | 0x40000000;
    *(undefined4 *)(iVar10 + 0x74) = *(undefined4 *)(param_1 + 0xa64);
    *(undefined4 *)(iVar10 + 0x78) = *(undefined4 *)(param_1 + 0xa68);
    *(undefined4 *)(iVar10 + 0x7c) = *(undefined4 *)(param_1 + 0xa6c);
    func_004CEAF0(*piVar22);
  }
  else if (*(char *)(param_1 + 0x964) == '\x06') {
    iVar10 = *(int *)(param_1 + 0x7a0);
    *(uint *)(iVar10 + 0x164) = *(uint *)(iVar10 + 0x164) | 0x40000000;
    *(undefined4 *)(iVar10 + 0x74) = *puVar17;
    *(undefined4 *)(iVar10 + 0x78) = puVar17[1];
    *(undefined4 *)(iVar10 + 0x7c) = puVar17[2];
    iVar10 = *piVar22;
    *(uint *)(iVar10 + 0x164) = *(uint *)(iVar10 + 0x164) | 0x40000000;
    func_0011593C((undefined4 *)(iVar10 + 0x80));
    func_004CEAF0(*piVar22);
    if (((*(uint *)(param_1 + 0x9a8) != 0) && (uVar8 = *(uint *)(param_1 + 0x9a4), uVar8 != 0)) &&
       (fVar24 = fVar23, uVar8 < *(uint *)(param_1 + 0x9a8))) {
      fVar23 = (float)VectorUnsignedToFloat(uVar8,(byte)(in_fpscr >> 0x15) & 3);
      fVar24 = fVar23 * *(float *)(param_1 + 0x9ac);
    }
    func_0011558C(afStack_b0,*(float **)(*(int *)(*(int *)(param_1 + 0x7a0) + 0x24) + 0x14));
    func_00491C08(afStack_50,afStack_b0);
    local_c0 = fStack_a4;
    local_bc = iStack_94;
    uStack_b8 = uStack_84;
    func_00115D70(&local_c0,&local_c0,fVar24);
    func_0048B330(afStack_40,pfRam0017ff88,afStack_50,fVar24);
    func_00491B3C(afStack_b0,afStack_40);
    fStack_a4 = local_c0;
    iStack_94 = local_bc;
    uStack_84 = uStack_b8;
    func_00491B3C(afStack_80,(float *)(param_1 + 0xbc4));
    uStack_74 = *(undefined4 *)(param_1 + 0x3d0);
    uStack_64 = *(undefined4 *)(param_1 + 0x3d4);
    uStack_54 = *(undefined4 *)(param_1 + 0x3d8);
    iVar10 = *piVar22;
    *(uint *)(iVar10 + 0x164) = *(uint *)(iVar10 + 0x164) | 0x40000000;
    *(undefined4 *)(iVar10 + 0x74) = *puVar17;
    *(undefined4 *)(iVar10 + 0x78) = puVar17[1];
    *(undefined4 *)(iVar10 + 0x7c) = puVar17[2];
    iVar10 = *piVar22;
    *(uint *)(iVar10 + 0x164) = *(uint *)(iVar10 + 0x164) | 0x40000000;
    func_001157FC((float *)(iVar10 + 0x80),afStack_80,afStack_b0);
    func_004CEAF0(*piVar22);
  }
  if (*(char *)(param_1 + 0x85d) == '\0') {
    uVar3 = *(ushort *)(param_1 + 0x38);
    uVar8 = (uint)uVar3;
    if (uVar8 != 0) {
      bVar2 = *(byte *)(param_1 + 0x35);
      bVar6 = *(char *)(param_1 + 0x964) == '\x05' || *(char *)(param_1 + 0x964) == '\n';
      uVar16 = (uint)bVar6;
      iVar10 = *piRam0017ff8c;
      if (uVar8 == 0) {
        return 0;
      }
      bVar7 = false;
      switch((uint)bVar2) {
      case 0:
        uVar14 = *(uint *)(iVar10 + 0x76a4 + (uint)(uVar3 >> 5) * 4);
        uVar1 = 1 << (uVar8 & 0x1f);
        bVar7 = ((uVar14 & uVar1) != 0) != bVar6;
        if (uVar16 == 0) {
          uVar14 = uVar14 & ~uVar1;
        }
        else {
          uVar14 = uVar14 | uVar1;
        }
        *(uint *)(iVar10 + 0x76a4 + (uint)(uVar3 >> 5) * 4) = uVar14;
        break;
      case 1:
        bVar7 = func_0052A07C(*(int *)(iVar10 + 0x40),uVar8);
        bVar7 = bVar7 != bVar6;
        func_001BB754(*(int *)(iVar10 + 0x40),uVar8,uVar16);
        break;
      case 2:
        bVar7 = func_0052A1B4(*(int *)(iVar10 + 0x40),uVar8);
        bVar7 = bVar7 != bVar6;
        func_001BB8F4(*(int *)(iVar10 + 0x40),uVar8,uVar16);
        break;
      case 3:
        bVar7 = func_0052A05C(*(int *)(iVar10 + 0x40),uVar8);
        bVar7 = bVar7 != bVar6;
        func_001BB724(*(int *)(iVar10 + 0x40),uVar8,uVar16);
        break;
      case 4:
        iVar10 = *DAT_004adb14;
        bVar7 = func_00584B94(iVar10,uVar8);
        bVar7 = bVar7 != bVar6;
        func_004CDF40(iVar10,uVar8,uVar16);
      }
      if (!bVar7) {
        return 0;
      }
      return func_004C4D6C(*DAT_004adb18,uVar8,(uint)bVar2,uVar16);
    }
  }
  return 0;
}

