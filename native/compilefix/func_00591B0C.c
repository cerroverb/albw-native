/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern int func_00110490(undefined4 *param_1,uint param_2);
extern undefined8 func_00110D44(undefined4 *param_1,byte *param_2,uint param_3);
extern undefined8 func_001163A0(uint *param_1,uint *param_2,uint param_3);
extern undefined4 *DAT_005922bc;
extern uint DAT_005922c0;
extern int *DAT_005922c4;
extern int DAT_005922c8;
extern int *DAT_005922cc;
extern int DAT_005922d0;
extern uint DAT_005922d4;
extern undefined4 DAT_005922d8;
extern undefined4 DAT_005922dc;
extern undefined4 DAT_005922e0;
extern int DAT_005922e4;
extern int DAT_005922e8;
extern undefined4 DAT_005922ec;
extern undefined4 DAT_005922f0;
extern undefined4 DAT_005922f4;
extern int DAT_005922f8;
extern undefined4 DAT_005922fc;
extern int DAT_00592300;
extern uint DAT_00592304;
extern int DAT_00592308;
extern int DAT_0059230c;
extern int DAT_00592310;
extern int *DAT_00592314;
extern int *DAT_00592318;
extern int DAT_0059231c;
extern int *DAT_00592320;
extern int *DAT_00592324;
extern int DAT_00592328;

// USA 00591B0C <- EU 00591AE4 (Ghidra, non-matching)
// GLOBAL DAT_005922bc undefined4 *
// GLOBAL DAT_005922c0 uint
// GLOBAL DAT_005922c4 int *
// GLOBAL DAT_005922c8 int
// GLOBAL DAT_005922cc int *
// GLOBAL DAT_005922d0 int
// GLOBAL DAT_005922d4 uint
// GLOBAL DAT_005922d8 undefined4
// GLOBAL DAT_005922dc undefined4
// GLOBAL DAT_005922e0 undefined4
// GLOBAL DAT_005922e4 int
// GLOBAL DAT_005922e8 int
// GLOBAL DAT_005922ec undefined4
// GLOBAL DAT_005922f0 undefined4
// GLOBAL DAT_005922f4 undefined4
// GLOBAL DAT_005922f8 int
// GLOBAL DAT_005922fc undefined4
// GLOBAL DAT_00592300 int
// GLOBAL DAT_00592304 uint
// GLOBAL DAT_00592308 int
// GLOBAL DAT_0059230c int
// GLOBAL DAT_00592310 int
// GLOBAL DAT_00592314 int *
// GLOBAL DAT_00592318 int *
// GLOBAL DAT_0059231c int
// GLOBAL DAT_00592320 int *
// GLOBAL DAT_00592324 int *
// GLOBAL DAT_00592328 int

undefined4 func_00591B0C(undefined4 *param_1)

{
  undefined4 *puVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  uint *puVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  undefined1 *puVar13;
  
  uVar10 = DAT_005922c0;
  puVar1 = DAT_005922bc;
  DAT_005922bc[2] = param_1;
  func_00110490(param_1,uVar10);
  piVar2 = DAT_005922c4;
  puVar1[1] = 1;
  puVar1[3] = 0xffffffff;
  *puVar1 = 1;
  iVar7 = DAT_005922c8;
  iVar3 = 0;
  iVar4 = *piVar2;
  piVar5 = DAT_005922cc;
  iVar6 = DAT_005922d0;
  while (iVar4 != -1) {
    piVar5 = piVar2 + iVar3 * 4;
    iVar3 = iVar3 + 1;
    iVar6 = *piVar5;
    *(int *)(iVar7 + iVar6 * 0xc) = piVar5[1];
    iVar6 = iVar7 + iVar6 * 0xc;
    *(int *)(iVar6 + 4) = piVar5[2];
    *(int *)(iVar6 + 8) = piVar5[3];
    piVar5 = DAT_005922cc;
    iVar6 = DAT_005922d0;
    iVar4 = piVar2[iVar3 * 4];
  }
  iVar3 = 0;
  iVar7 = *piVar5;
  uVar10 = DAT_005922d4;
  while (iVar7 != -1) {
    iVar7 = iVar3 * 2;
    iVar3 = iVar3 + 1;
    *(int *)(iVar6 + piVar5[iVar7] * 4) = (piVar5 + iVar7)[1];
    uVar10 = DAT_005922d4;
    iVar7 = piVar5[iVar3 * 2];
  }
  iVar3 = puVar1[2];
  *(undefined4 *)(iVar3 + 0x13a4) = 0x1000;
  *(undefined1 *)(uVar10 + iVar3) = 2;
  uVar11 = (uVar10 | 3) ^ 7;
  *(undefined4 *)(iVar3 + 0x13a8) = 0;
  *(undefined1 *)((uVar10 | 3) + iVar3) = 0xf;
  uVar12 = uVar10 ^ 7;
  *(undefined4 *)(iVar3 + 0x13ac) = 0;
  *(undefined1 *)(uVar11 + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x13b0) = 9;
  *(undefined1 *)(uVar12 + iVar3) = 0xf;
  uVar11 = uVar11 | 0xf;
  *(undefined4 *)(iVar3 + 0x13b4) = 0;
  *(undefined1 *)((uVar10 ^ 4) + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x13b8) = 0;
  *(undefined1 *)(uVar11 + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x13bc) = 0;
  uVar8 = DAT_005922d8;
  *(undefined1 *)(uVar12 + 3 + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x13c0) = uVar8;
  uVar8 = DAT_005922dc;
  *(undefined1 *)((uVar10 ^ 4) + 3 + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x13c4) = uVar8;
  *(undefined1 *)(uVar11 + 3 + iVar3) = 0xf;
  *(undefined4 *)((uVar12 - 0x271) + iVar3) = 0x3c00;
  *(undefined1 *)(uVar11 + 0x2d + iVar3) = 0xf;
  uVar8 = DAT_005922e0;
  *(undefined4 *)(iVar3 + 0x1364) = 0;
  uVar10 = uVar11 - 0x15 | 3;
  *(undefined1 *)((uVar11 - 0x15) + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x1368) = uVar8;
  *(undefined1 *)(uVar10 + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x136c) = uVar8;
  *(undefined1 *)((uVar10 ^ 7) + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x1370) = uVar8;
  *(undefined1 *)(iVar3 + 0x16cd) = 0xf;
  *(undefined4 *)(iVar3 + 0x1374) = uVar8;
  *(undefined1 *)(iVar3 + 0x16ce) = 0xf;
  uVar10 = uVar10 + 5;
  *(undefined4 *)(iVar3 + 0x1378) = uVar8;
  *(undefined1 *)(iVar3 + 0x16cf) = 0xf;
  uVar12 = 0x5c;
  uVar11 = uVar10 | (int)uVar10 >> 0xb;
  *(undefined4 *)(iVar3 + 0x137c) = uVar8;
  *(undefined1 *)(uVar10 + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x1380) = uVar8;
  *(undefined1 *)(iVar3 + 0x16d1) = 0xf;
  *(undefined4 *)(iVar3 + 0x139c) = 1;
  *(undefined1 *)(iVar3 + 0x16d8) = 0xf;
  *(undefined4 *)(iVar3 + 0x135c) = 0xbf0000;
  *(undefined1 *)(iVar3 + 0x16c8) = 0xf;
  *(undefined4 *)(iVar3 + 0x1360) = 0;
  *(undefined1 *)(iVar3 + 0x16c9) = 0xf;
  *(undefined4 *)(iVar3 + 0x1384) = 0;
  *(undefined1 *)(uVar11 + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 5000) = 0;
  *(undefined1 *)((uVar11 | 3) + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x138c) = 0;
  *(undefined1 *)(((uVar11 | 3) ^ 7) + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x1390) = 0;
  *(undefined1 *)((uVar11 ^ 7) + iVar3) = 0xf;
  *(undefined4 *)(iVar3 + 0x1394) = 0;
  *(undefined1 *)((uVar11 ^ 4) + iVar3) = 0xf;
  do {
    if (uVar12 == 0x8d) {
LAB_00592084:
      *(undefined4 *)(iVar3 + uVar12 * 4 + 0x1300) = 0x3c00;
      *(undefined1 *)(iVar3 + uVar12 + 0x16b1) = 0xf;
    }
    else {
      if (0x8d < (int)uVar12) {
        if (uVar12 != 0xa3) {
          if ((int)uVar12 < 0xa4) {
            if (uVar12 != 0x98) {
              if ((int)uVar12 < 0x99) {
                if (uVar12 == 0x8f) goto LAB_005920a8;
                if (uVar12 == 0x90) goto LAB_005920b0;
                if (uVar12 != 0x92) goto LAB_00591e58;
              }
              else {
                if (uVar12 == 0x9a) goto LAB_005920a8;
                if (uVar12 == 0x9b) goto LAB_005920b0;
                if (uVar12 != 0x9d) goto LAB_00591e58;
              }
LAB_00592060:
              uVar8 = 0x3f000;
LAB_00592064:
              *(undefined4 *)(iVar3 + uVar12 * 4 + 0x1300) = uVar8;
              *(undefined1 *)(iVar3 + uVar12 + 0x16b1) = 0xf;
              goto LAB_00591e78;
            }
          }
          else if (uVar12 != 0xae) {
            if ((int)uVar12 < 0xaf) {
              if (uVar12 == 0xa5) goto LAB_005920a8;
              if (uVar12 == 0xa6) goto LAB_005920b0;
              if (uVar12 != 0xa8) goto LAB_00591e58;
            }
            else {
              if (uVar12 == 0xb0) goto LAB_005920a8;
              if (uVar12 == 0xb1) goto LAB_005920b0;
              if (uVar12 != 0xb3) goto LAB_00591e58;
            }
            goto LAB_00592060;
          }
        }
        goto LAB_00592084;
      }
      if (uVar12 == 0x77) goto LAB_00592084;
      if ((int)uVar12 < 0x78) {
        if (uVar12 != 0x6c) {
          if ((int)uVar12 < 0x6d) {
            if (uVar12 == 0x61) goto LAB_00592084;
            if (uVar12 == 99) goto LAB_005920a8;
            if (uVar12 == 100) goto LAB_005920b0;
            if (uVar12 != 0x66) goto LAB_00591e58;
          }
          else {
            if (uVar12 == 0x6e) goto LAB_005920a8;
            if (uVar12 == 0x6f) goto LAB_005920b0;
            if (uVar12 != 0x71) goto LAB_00591e58;
          }
          goto LAB_00592060;
        }
        goto LAB_00592084;
      }
      if (uVar12 == 0x82) goto LAB_00592084;
      if ((int)uVar12 < 0x83) {
        if (uVar12 == 0x79) {
LAB_005920a8:
          uVar8 = 0x800;
          goto LAB_00592064;
        }
        if (uVar12 == 0x7a) goto LAB_005920b0;
        if (uVar12 == 0x7c) goto LAB_00592060;
LAB_00591e58:
        *(undefined4 *)(iVar3 + uVar12 * 4 + 0x1300) = 0;
        *(undefined1 *)(iVar3 + uVar12 + 0x16b1) = 0xf;
      }
      else {
        if (uVar12 == 0x84) goto LAB_005920a8;
        if (uVar12 != 0x85) {
          if (uVar12 != 0x87) goto LAB_00591e58;
          goto LAB_00592060;
        }
LAB_005920b0:
        *(undefined4 *)(iVar3 + uVar12 * 4 + 0x1300) = 1;
        *(undefined1 *)(iVar3 + uVar12 + 0x16b1) = 0xf;
      }
    }
LAB_00591e78:
    iVar7 = DAT_005922e8;
    iVar6 = DAT_005922e4;
    uVar12 = uVar12 + 1;
    if (0xb3 < uVar12) {
      iVar4 = DAT_005922e4 + 0x158;
      *(undefined4 *)(DAT_005922e4 + iVar3) = DAT_005922ec;
      uVar8 = DAT_005922f0;
      *(undefined1 *)(iVar7 + iVar3) = 0xf;
      *(undefined4 *)(iVar4 + iVar3) = uVar8;
      *(undefined1 *)(iVar7 + 0x56 + iVar3) = 0xf;
      *(undefined4 *)(iVar6 + 0x15c + iVar3) = 0;
      *(undefined1 *)(iVar3 + 0x1766) = 0xf;
      *(undefined4 *)(iVar3 + 0x15e0) = 1;
      uVar8 = DAT_005922f4;
      *(undefined1 *)(iVar3 + 0x1769) = 0xf;
      *(undefined4 *)(iVar3 + 0x15d8) = uVar8;
      *(undefined1 *)(iVar3 + 0x1767) = 0xf;
      *(undefined4 *)(iVar3 + 0x15dc) = 0xffffffff;
      *(undefined1 *)(iVar3 + 0x1768) = 0xf;
      *(undefined4 *)(iVar3 + 0x15e8) = 0;
      *(undefined1 *)(iVar3 + 0x176b) = 0xf;
      *(undefined4 *)(iVar3 + 0x15ec) = 0;
      *(undefined1 *)(iVar3 + 0x176c) = 0xf;
      iVar6 = DAT_005922f8;
      *(undefined4 *)(iVar3 + 0x15e4) = DAT_005922fc;
      *(undefined1 *)(iVar3 + 0x176a) = 0xf;
      *(undefined4 *)(iVar3 + 0x15f0) = 0;
      *(undefined1 *)(iVar6 + iVar3) = 0xf;
      *(undefined4 *)(iVar3 + 0x1440) = 0;
      *(undefined1 *)(iVar6 + -0x6c + iVar3) = 0xf;
      *(undefined4 *)(iVar3 + 0x1444) = 0;
      puVar1 = DAT_005922bc;
      *(undefined1 *)(iVar6 + -0x6b + iVar3) = 0xf;
      *(undefined4 *)(iVar3 + 0x1448) = 0x3c00;
      *(undefined1 *)((iVar6 - 0x6aU) + iVar3) = 0xf;
      uVar10 = iVar6 - 0x6aU ^ 5;
      *(undefined4 *)(iVar3 + 0x1454) = 0;
      *(undefined1 *)(uVar10 + iVar3) = 0xf;
      *(undefined4 *)(iVar3 + 0x1458) = 0;
      *(undefined1 *)((uVar10 | 5) + iVar3) = 0xf;
      *(undefined4 *)(iVar3 + 0x145c) = 0x100;
      *(undefined1 *)(iVar3 + 0x1708) = 0xf;
      iVar6 = DAT_00592300;
      *(undefined4 *)(iVar3 + 0x1460) = 0xa00;
      *(undefined1 *)(iVar6 + iVar3) = 7;
      *(undefined4 *)(iVar3 + 0x144c) = 0x3c00;
      *(undefined1 *)(iVar3 + 0x1704) = 0xf;
      *(undefined4 *)(iVar3 + 0x1464) = 0xe40100;
      *(undefined1 *)(iVar3 + 0x170a) = 0xd;
      *(undefined4 *)(iVar3 + 0x1468) = 0x10;
      uVar10 = DAT_00592304;
      *(undefined1 *)(iVar3 + 0x170b) = 0xf;
      iVar6 = 0;
      iVar7 = 0x1e;
      puVar9 = (uint *)(iVar3 + 0x13c8);
      puVar13 = (undefined1 *)(iVar3 + 0x16e3);
LAB_005922a8:
      *puVar9 = uVar10;
      do {
        *puVar13 = 0xf;
        while( true ) {
          iVar4 = DAT_0059230c;
          iVar7 = iVar7 + -1;
          puVar9 = puVar9 + 1;
          puVar13 = puVar13 + 1;
          iVar6 = iVar6 + 1;
          if (iVar7 == 0) {
            *(undefined4 *)(DAT_00592308 + iVar3) = 0;
            *(undefined1 *)(iVar4 + iVar3) = 0xf;
            func_00110D44((undefined4 *)(iVar3 + 0x100c),(byte *)(iVar3 + 0x1300),0x2f4);
            func_001163A0((uint *)(puVar1[2] + 0x15f4),(uint *)(puVar1[2] + 0x16b1),0xbd);
            piVar5 = DAT_00592314;
            iVar3 = DAT_00592310;
            iVar6 = 0;
            *(undefined4 *)(DAT_00592310 + 0x104) = 0;
            *(undefined4 *)(iVar3 + 0x108) = 0;
            *(undefined4 *)(iVar3 + 0x10c) = 0;
            *(undefined4 *)(iVar3 + 0x110) = 0;
            *(undefined4 *)(iVar3 + 0x114) = 0;
            *(undefined4 *)(iVar3 + 0x118) = 0;
            if (*piVar5 != 0xbd) {
              do {
                puVar9 = (uint *)(piVar5 + iVar6);
                iVar6 = iVar6 + 1;
                iVar7 = (int)*puVar9 >> 5;
                *(uint *)(iVar3 + 0x104 + iVar7 * 4) =
                     *(uint *)(iVar3 + 0x104 + iVar7 * 4) | 1 << (*puVar9 & 0x1f);
              } while (piVar5[iVar6] != 0xbd);
            }
            *(undefined4 *)(iVar3 + 0x11c) = 0;
            *(undefined4 *)(iVar3 + 0x120) = 0;
            piVar2 = DAT_00592318;
            *(undefined4 *)(iVar3 + 0x124) = 0;
            *(undefined4 *)(iVar3 + 0x128) = 0;
            *(undefined4 *)(iVar3 + 300) = 0;
            *(undefined4 *)(iVar3 + 0x130) = 0;
            iVar7 = DAT_0059231c;
            iVar6 = 0;
            iVar4 = *piVar2;
            piVar5 = DAT_00592320;
            while (iVar4 != 0xbd) {
              puVar9 = (uint *)(piVar2 + iVar6);
              iVar6 = iVar6 + 1;
              iVar4 = (int)*puVar9 >> 5;
              *(uint *)(iVar7 + iVar4 * 4) = *(uint *)(iVar7 + iVar4 * 4) | 1 << (*puVar9 & 0x1f);
              piVar5 = DAT_00592320;
              iVar4 = piVar2[iVar6];
            }
            iVar6 = 0;
            iVar4 = *piVar5;
            while (iVar4 != 0xbd) {
              puVar9 = (uint *)(piVar5 + iVar6);
              iVar6 = iVar6 + 1;
              iVar4 = (int)*puVar9 >> 5;
              *(uint *)(iVar7 + iVar4 * 4) = *(uint *)(iVar7 + iVar4 * 4) | 1 << (*puVar9 & 0x1f);
              iVar4 = piVar5[iVar6];
            }
            *(undefined4 *)(iVar3 + 0x134) = 0;
            *(undefined4 *)(iVar3 + 0x138) = 0;
            piVar5 = DAT_00592324;
            *(undefined4 *)(iVar3 + 0x13c) = 0;
            *(undefined4 *)(iVar3 + 0x140) = 0;
            *(undefined4 *)(iVar3 + 0x144) = 0;
            *(undefined4 *)(iVar3 + 0x148) = 0;
            iVar7 = DAT_00592328;
            iVar3 = 0;
            iVar6 = *piVar5;
            while (iVar6 != 0xbd) {
              puVar9 = (uint *)(piVar5 + iVar3);
              iVar3 = iVar3 + 1;
              iVar6 = (int)*puVar9 >> 5;
              *(uint *)(iVar7 + iVar6 * 4) = *(uint *)(iVar7 + iVar6 * 4) | 1 << (*puVar9 & 0x1f);
              iVar6 = piVar5[iVar3];
            }
            return 0;
          }
          if (iVar6 == 0) goto LAB_005922a8;
          if (((iVar6 == 5 || iVar6 == 10) || (iVar6 == 0xf || iVar6 == 0x14)) || (iVar6 == 0x19))
          break;
          *puVar9 = 0;
          *puVar13 = 0xf;
        }
        *puVar9 = uVar10 | (int)uVar10 >> 1;
      } while( true );
    }
  } while( true );
}

