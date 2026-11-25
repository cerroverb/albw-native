/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
// MAPFIX-INTEGRATED v1
#include "ghidra_types.h"

extern int func_00100174(int *param_1);
extern void func_0010F1F8(int *param_1,float fparam_0);
extern int func_00115C54(float *param_1,float *param_2,float *param_3);
extern int func_00165E28(int param_1,undefined2 param_2);
extern int func_00230610(int param_1,undefined4 param_2);
extern int func_002982F4(int param_1);
extern int func_002E0268(int param_1);
extern int func_002FCAD4(int param_1);
extern int func_002FCAF8(int param_1);
extern int func_004EAB38(int param_1,int param_2);
extern int func_005045C4(int param_1);
extern int func_0050462C(int param_1);
extern int func_0052FBA0(int param_1,uint param_2);
extern int func_0052FBDC(int param_1,int param_2,int param_3);
extern int func_00556A90();
extern int func_00588268(int param_1);
extern int func_00588BE4(int param_1);
extern int func_00588BF0(int param_1,int param_2);
extern int *DAT_00298b20;
extern int *DAT_00298b24;
extern int *DAT_00298b28;
extern undefined4 DAT_00298b2c;
extern uint *DAT_00298b30;
extern undefined4 *DAT_00298b34;
extern int *DAT_00298b38;
extern int *DAT_00298b3c;
extern int *DAT_00298b40;
extern int *DAT_00298b44;
extern int *DAT_00298b48;
extern int *DAT_00298b4c;
extern int *DAT_00298b50;
extern uint *DAT_00298b54;
extern undefined4 DAT_00298b58;
extern undefined4 *DAT_00298b5c;
extern uint *DAT_00298b60;
extern undefined4 *DAT_00298b64;
extern undefined4 DAT_00298b68;

// USA 00298394 <- EU 00298370 (Ghidra, non-matching)
// MAPFIX: tools/ghidra_name_fixes.json applied (do not apply again)
// GLOBAL DAT_00298b20 int *
// GLOBAL DAT_00298b24 int *
// GLOBAL DAT_00298b28 int *
// GLOBAL DAT_00298b2c undefined4
// GLOBAL DAT_00298b30 uint *
// GLOBAL DAT_00298b34 undefined4 *
// GLOBAL DAT_00298b38 int *
// GLOBAL DAT_00298b3c int *
// GLOBAL DAT_00298b40 int *
// GLOBAL DAT_00298b44 int *
// GLOBAL DAT_00298b48 int *
// GLOBAL DAT_00298b4c int *
// GLOBAL DAT_00298b50 int *
// GLOBAL DAT_00298b54 uint *
// GLOBAL DAT_00298b58 undefined4
// GLOBAL DAT_00298b5c undefined4 *
// GLOBAL DAT_00298b60 uint *
// GLOBAL DAT_00298b64 undefined4 *
// GLOBAL DAT_00298b68 undefined4

undefined4 func_00298394(int param_1)

{
  char cVar1;
  short sVar2;
  byte bVar3;
  byte bVar4;
  int *piVar5;
  int *piVar6;
  undefined4 uVar7;
  uint *puVar8;
  bool bVar9;
  int iVar10;
  int iVar11;
  undefined4 *puVar12;
  uint uVar13;
  int iVar14;
  int *piVar15;
  bool bVar16;
  float fVar17;
  _Alignas(8) unsigned char albw_frame[0x30];
  #define fStack_30 (*(float *)(albw_frame + 0x0))
  #define fStack_2c (*(float *)(albw_frame + 0x4))
  #define uStack_28 (*(undefined4 *)(albw_frame + 0x8))
  func_002982F4(param_1);
  if (*(char *)(param_1 + 0x411) != '\0') {
    if ((*(int *)(*DAT_00298b20 + 0x44) != 0) &&
       (*(int *)(*DAT_00298b20 + 0x44) == *(int *)(param_1 + 0x414))) {
      if (((int)*(float *)(param_1 + 0x48) & 4U) != 0) {
        func_002FCAD4(*(int *)(param_1 + 0x414));
      }
      if (((int)*(float *)(param_1 + 0x48) & 8U) != 0) {
        func_0050462C(*DAT_00298b24);
      }
      *(undefined1 *)(param_1 + 0x411) = 0;
    }
  }
  piVar6 = DAT_00298b28;
  if (*(char *)(param_1 + 0x450) == '\0') {
    iVar10 = func_0052FBDC(*DAT_00298b28 + 0x3dbbc,*(int *)(param_1 + 0x454),0);
    if (iVar10 == 0) {
      return 0;
    }
    *(undefined4 *)(param_1 + 0x44c) = *(undefined4 *)(iVar10 + 0x2cc);
    *(undefined1 *)(param_1 + 0x450) = 1;
  }
  bVar3 = *(byte *)(param_1 + 0x451);
  bVar9 = func_00588BF0(param_1,0);
  *(bool *)(param_1 + 0x451) = bVar9;
  sVar2 = *(short *)(param_1 + 0x38);
  bVar4 = *(byte *)(param_1 + 0x452);
  bVar9 = func_00588BF0(param_1,1);
  *(bool *)(param_1 + 0x452) = bVar9;
  uVar13 = *(uint *)(param_1 + 0x14);
  bVar16 = uVar13 == 0x3f800000;
  if (!bVar16) {
    uVar13 = (uint)*(byte *)(param_1 + 0x410);
  }
  cVar1 = *(char *)(param_1 + 0x40e);
  if (bVar16 || uVar13 == 0) {
    bVar16 = cVar1 == '\0';
    if (bVar16) {
      cVar1 = *(char *)(param_1 + 0x40f);
    }
    if (bVar16 && cVar1 == '\0') goto LAB_002984a4;
  }
  else {
    if (cVar1 != '\0') {
      *(undefined1 *)(param_1 + 0x40e) = 0;
      *(undefined1 *)(param_1 + 0x410) = 0;
    }
    if (*(char *)(param_1 + 0x40f) != '\0') {
      *(undefined1 *)(param_1 + 0x40f) = 0;
      *(undefined1 *)(param_1 + 0x410) = 0;
    }
LAB_002984a4:
    if ((bVar3 == 0) && (*(char *)(param_1 + 0x451) != '\0')) {
      *(undefined1 *)(param_1 + 0x40e) = 1;
    }
    if ((sVar2 != 0 && bVar4 != 0) && (!bVar9)) {
      *(undefined1 *)(param_1 + 0x40f) = 1;
    }
    cVar1 = *(char *)(param_1 + 0x40e);
    bVar9 = cVar1 == '\0';
    if (bVar9) {
      cVar1 = *(char *)(param_1 + 0x40f);
    }
    if (bVar9 && cVar1 == '\0') {
      return 0;
    }
  }
  *(undefined1 *)(param_1 + 0x410) = 0;
  uVar7 = DAT_00298b2c;
  iVar10 = *(int *)(param_1 + 0x418);
  if (iVar10 == 0) {
    if ((bVar3 & ~bVar4) != 0) {
      return 0;
    }
    fVar17 = ALBW_AS_F32((int)*(float *)(param_1 + 0x48));
    if (((uint)ALBW_AS_U32(fVar17) & 2) != 0) {
      puVar12 = (undefined4 *)func_0052FBA0(*piVar6 + 0x3dbbc,*(uint *)(param_1 + 0x44c));
      if (((*DAT_00298b30 & 1) == 0) && (iVar10 = func_00100174((int *)DAT_00298b30), iVar10 != 0))
      {
        *DAT_00298b34 = uVar7;
      }
      if ((puVar12 == (undefined4 *)0x0) ||
         (iVar10 = (**(code **)*puVar12)(puVar12,DAT_00298b34), iVar10 == 0)) {
        puVar12 = (undefined4 *)0x0;
      }
      if (((puVar12 != (undefined4 *)0x0) &&
          ((((iVar10 = func_00588BE4((int)puVar12), iVar10 == *DAT_00298b38 ||
             (iVar10 == *DAT_00298b3c)) || (iVar10 == *DAT_00298b40)) || (iVar10 == *DAT_00298b44)))
          ) && (iVar10 = func_00556A90(*DAT_00298b48), iVar10 == *(int *)(param_1 + 0x44c))) {
        *(undefined1 *)(param_1 + 0x2d7) = 1;
        return 0;
      }
    }
    *(undefined1 *)(param_1 + 0x40d) = 1;
    *(bool *)(*(int *)(param_1 + 0x414) + 0x118) = *(short *)(param_1 + 0x30) != 0;
    iVar10 = *(int *)(param_1 + 0x414);
    piVar5 = *(int **)(iVar10 + 0x38);
    piVar15 = DAT_00298b4c;
    while (piVar5 != (int *)(iVar10 + 0x34)) {
      piVar15 = (int *)piVar5[1];
      func_0010F1F8(piVar5,fVar17);
      *(int *)(iVar10 + 0x3c) = *(int *)(iVar10 + 0x3c) + -1;
      piVar5[-1] = *(int *)(iVar10 + 0x40);
      *(int **)(iVar10 + 0x40) = piVar5 + -1;
      piVar5 = piVar15;
      piVar15 = DAT_00298b4c;
    }
    bVar3 = *(byte *)(param_1 + 0x5c);
    iVar10 = 1;
    if (bVar3 < 2) {
      return 0;
    }
    do {
      iVar14 = (int)*(short *)(param_1 + iVar10 * 6 + 0x5e);
      if ((0 < iVar14) && (iVar14 = func_0052FBDC(*piVar6 + 0x3dbbc,iVar14,0), iVar14 != 0)) {
        func_00230610(*(int *)(param_1 + 0x414),*(undefined4 *)(iVar14 + 0x2cc));
        iVar11 = func_00588BE4(iVar14);
        if (iVar11 == *piVar15) {
          *(undefined4 *)(param_1 + 0x464) = *(undefined4 *)(iVar14 + 0x2cc);
        }
      }
      iVar10 = iVar10 + 1;
    } while (iVar10 < (int)(uint)bVar3);
    return 0;
  }
  if (iVar10 < 0) {
    return 0;
  }
  if ((iVar10 == 2) && (sVar2 = *(short *)(param_1 + 0x32), sVar2 != 0)) {
    if (sVar2 == 1) {
      func_004EAB38(*DAT_00298b50,0);
    }
    else if (sVar2 == 2) {
      func_004EAB38(*DAT_00298b50,1);
    }
  }
  if (*(short *)(param_1 + 0x30) != 0) {
    puVar12 = (undefined4 *)func_0052FBA0(*piVar6 + 0x3dbbc,*(uint *)(param_1 + 0x44c));
    if (((*DAT_00298b30 & 1) == 0) && (iVar10 = func_00100174((int *)DAT_00298b30), iVar10 != 0)) {
      *DAT_00298b34 = uVar7;
    }
    if ((puVar12 == (undefined4 *)0x0) ||
       (iVar10 = (**(code **)*puVar12)(puVar12,DAT_00298b34), iVar10 == 0)) {
      puVar12 = (undefined4 *)0x0;
    }
    if (puVar12 != (undefined4 *)0x0) {
      func_00115C54(&fStack_30,(float *)(puVar12 + 0xf4),(float *)(param_1 + 0x458));
      fVar17 = *(float *)(param_1 + 0x440);
      iVar10 = *(int *)(param_1 + 0x414);
      *(float *)(iVar10 + 0x10c) = fStack_30;
      *(float *)(iVar10 + 0x110) = fStack_2c + fVar17;
      *(undefined4 *)(iVar10 + 0x114) = uStack_28;
    }
  }
  iVar10 = *(int *)(param_1 + 0x418) + -1;
  *(int *)(param_1 + 0x418) = iVar10;
  uVar7 = DAT_00298b58;
  puVar8 = DAT_00298b54;
  if (iVar10 != 0) goto LAB_00298a7c;
  if (*(short *)(param_1 + 0x32) == 0) {
    iVar10 = func_00165E28(*(int *)(param_1 + 0x414),*(undefined2 *)(param_1 + 0x44a));
    if (iVar10 != 0) {
      if (((int)*(float *)(param_1 + 0x48) & 4U) != 0) {
        func_002FCAF8(*(int *)(param_1 + 0x414));
      }
      if (((int)*(float *)(param_1 + 0x48) & 8U) != 0) {
        func_005045C4(*DAT_00298b24);
      }
    }
    if (*(int *)(param_1 + 0x14) == 0x3f800000) {
      *(undefined1 *)(param_1 + 0x2d7) = 1;
    }
    else {
      *(undefined1 *)(param_1 + 0x410) = 1;
      *(undefined4 *)(param_1 + 0x418) = 0;
      *(undefined4 *)(param_1 + 0x41c) = 0;
      *(undefined1 *)(param_1 + 0x450) = 0;
      *(undefined1 *)(param_1 + 0x451) = 1;
    }
    if (*(uint *)(param_1 + 0x464) == 0) goto LAB_00298a7c;
    puVar12 = (undefined4 *)func_0052FBA0(*piVar6 + 0x3dbbc,*(uint *)(param_1 + 0x464));
    if (((*puVar8 & 1) == 0) && (iVar10 = func_00100174((int *)DAT_00298b54), iVar10 != 0)) {
      *DAT_00298b5c = uVar7;
    }
    if ((puVar12 == (undefined4 *)0x0) ||
       (iVar10 = (**(code **)*puVar12)(puVar12,DAT_00298b5c), iVar10 == 0)) {
      puVar12 = (undefined4 *)0x0;
    }
  }
  else {
    iVar10 = func_00588268(*DAT_00298b50);
    if (iVar10 != 0) {
      *(int *)(param_1 + 0x418) = *(int *)(param_1 + 0x418) + 1;
      goto LAB_00298a7c;
    }
    iVar10 = func_00165E28(*(int *)(param_1 + 0x414),*(undefined2 *)(param_1 + 0x44a));
    if (iVar10 != 0) {
      if (((int)*(float *)(param_1 + 0x48) & 4U) != 0) {
        func_002FCAF8(*(int *)(param_1 + 0x414));
      }
      if (((int)*(float *)(param_1 + 0x48) & 8U) != 0) {
        func_005045C4(*DAT_00298b24);
      }
    }
    if (*(int *)(param_1 + 0x14) == 0x3f800000) {
      *(undefined1 *)(param_1 + 0x2d7) = 1;
    }
    else {
      *(undefined1 *)(param_1 + 0x410) = 1;
      *(undefined4 *)(param_1 + 0x418) = 0;
      *(undefined4 *)(param_1 + 0x41c) = 0;
      *(undefined1 *)(param_1 + 0x450) = 0;
      *(undefined1 *)(param_1 + 0x451) = 1;
    }
    if (*(uint *)(param_1 + 0x464) == 0) goto LAB_00298a7c;
    puVar12 = (undefined4 *)func_0052FBA0(*piVar6 + 0x3dbbc,*(uint *)(param_1 + 0x464));
    if (((*puVar8 & 1) == 0) && (iVar10 = func_00100174((int *)DAT_00298b54), iVar10 != 0)) {
      *DAT_00298b5c = uVar7;
    }
    if ((puVar12 == (undefined4 *)0x0) ||
       (iVar10 = (**(code **)*puVar12)(puVar12,DAT_00298b5c), iVar10 == 0)) {
      puVar12 = (undefined4 *)0x0;
    }
  }
  if (puVar12 != (undefined4 *)0x0) {
    *(undefined1 *)(puVar12 + 0x106) = 0;
  }
LAB_00298a7c:
  if ((0 < *(int *)(param_1 + 0x41c)) &&
     (iVar10 = *(int *)(param_1 + 0x41c) + -1, *(int *)(param_1 + 0x41c) = iVar10, iVar10 == 0)) {
    puVar12 = (undefined4 *)func_0052FBA0(*piVar6 + 0x3dbbc,*(uint *)(param_1 + 0x44c));
    if (((*DAT_00298b60 & 1) == 0) && (iVar10 = func_00100174((int *)DAT_00298b60), iVar10 != 0)) {
      *DAT_00298b64 = DAT_00298b68;
    }
    if ((puVar12 == (undefined4 *)0x0) ||
       (iVar10 = (**(code **)*puVar12)(puVar12,DAT_00298b64), iVar10 == 0)) {
      puVar12 = (undefined4 *)0x0;
    }
    if (puVar12 != (undefined4 *)0x0) {
      return func_002E0268((int)puVar12);
    }
  }
  return 0;
}

