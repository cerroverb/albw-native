/* compilefix: USA 0046019C (message / queue update; Ghidra, non-matching): the decompiled text wrote `DAT_00460514 = iVar6` in the
 * loop condition - DAT_00460514 is a literal-pool word in the code (Ghidra merged a register with the pool slot it was loaded
 * from). The ARM code never writes there and reloads the constant each time, so the write is dropped; it faulted (the pool is
 * read-only): crash at frame 4165, fault address 0x08db1a64 = DAT_00460514. Same source as the linked object
 * (ghidra_analyzed5_v4) otherwise. Confidence: high. */
// MAPFIX-INTEGRATED v1
#include "ghidra_types.h"

extern int func_001B6C2C(int *param_1,undefined4 param_2,undefined4 param_3,int param_4);
extern int func_00231340(short *param_1);
extern int func_00231448(int param_1);
extern float func_002317C0(undefined2 *param_1,float fparam_0);
extern int func_003616D4();
extern int func_00427688(undefined4 param_1);
extern int func_0045F76C(int param_1);
extern int func_005190AC(int param_1);
extern int func_0051C22C(int param_1);
extern int func_005229AC(int *param_1,int *param_2);
extern int func_00556AA8();
extern int func_0065CD54(undefined4 *param_1,undefined1 param_2);
extern int DAT_00460514;
extern undefined4 DAT_00460518;
extern undefined4 DAT_0046051c;
extern int *DAT_00460520;
extern float DAT_00460524;
extern int *DAT_00460528;

// USA 0046019C <- EU 00460174 (Ghidra, non-matching)
// GLOBAL DAT_00460514 int
// GLOBAL DAT_00460518 undefined4
// GLOBAL DAT_0046051c undefined4
// GLOBAL DAT_00460520 int *
// GLOBAL DAT_00460524 float
// GLOBAL DAT_00460528 int *

undefined4 func_0046019C(int *param_1,int param_2,undefined4 param_3,uint param_4)

{
  char cVar1;
  ushort uVar2;
  uint uVar3;
  char *pcVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  undefined4 extraout_r1;
  uint uVar8;
  int *piVar9;
  int iVar10;
  bool bVar11;
  float fparam_0;
  _Alignas(8) unsigned char albw_frame[0x30];
  #define local_30 (*(int *)(albw_frame + 0x0))
  #define local_2c (*(undefined4 *)(albw_frame + 0x4))
  cVar1 = *(char *)((int)param_1 + 0xd);
  bVar11 = cVar1 != '\0';
  if (!bVar11) {
    cVar1 = *(char *)((int)param_1 + 0xe);
  }
  if (bVar11 || cVar1 != '\0') {
    return 0;
  }
  uVar3 = param_1[0x34];
  iVar6 = DAT_00460514;
  while (uVar3 = uVar3 - 1, -1 < (int)uVar3) {
    if (uVar3 < (uint)param_1[0x34]) {
      param_4 = param_1[0x33];
      iVar5 = param_4 + uVar3;
      if (param_1[0x32] <= iVar5) {
        iVar5 = iVar5 - param_1[0x32];
      }
      pcVar4 = (char *)(param_1[0x31] + iVar5 * 2);
    }
    else {
      pcVar4 = (char *)param_1[0x31];
    }
    if (*pcVar4 != -1) {
      if (uVar3 < (uint)param_1[0x34]) {
        iVar6 = param_1[0x33] + uVar3;
        if (param_1[0x32] <= iVar6) {
          iVar6 = iVar6 - param_1[0x32];
        }
        pcVar4 = (char *)(param_1[0x31] + iVar6 * 2);
      }
      else {
        pcVar4 = (char *)param_1[0x31];
      }
      iVar5 = func_00427688((int)*pcVar4);
      iVar6 = DAT_00460514;
      if (iVar5 == 0) {
        uVar8 = param_1[0x34];
        if (0 < (int)uVar8) {
          bVar11 = uVar3 == 0;
          if (-1 < (int)uVar3) {
            bVar11 = uVar8 == uVar3;
          }
          if (!bVar11 && (-1 < (int)uVar3 && (int)uVar3 <= (int)uVar8)) {
            uVar7 = uVar3;
            if ((int)uVar3 < (int)(uVar8 - 1)) {
              do {
                iVar5 = param_1[0x32];
                iVar6 = param_1[0x33] + uVar7;
                if (iVar5 <= iVar6) {
                  iVar6 = iVar6 - iVar5;
                }
                iVar10 = param_1[0x33] + uVar7 + 1;
                if (iVar5 <= iVar10) {
                  iVar10 = iVar10 - iVar5;
                }
                uVar7 = uVar7 + 1;
                uVar2 = *(ushort *)(param_1[0x31] + iVar10 * 2);
                param_4 = (uint)uVar2;
                *(ushort *)(param_1[0x31] + iVar6 * 2) = uVar2;
              } while ((int)uVar7 < param_1[0x34] + -1);
            }
            param_1[0x34] = param_1[0x34] + -1;
            iVar6 = DAT_00460514;
          }
        }
      }
    }
  }
  iVar5 = (int)*(char *)((int)param_1 + 0x11a);
  if (iVar5 == param_2) goto LAB_004603a0;
  if (iVar5 == 2) {
    local_30 = iVar6;
    local_2c = DAT_00460518;
    iVar5 = func_005229AC((int *)param_1[7],&local_30);
LAB_0046030c:
    if ((iVar5 != 0) && ((*(byte *)(iVar5 + 0x44) & 1) != 0)) {
      *(byte *)(iVar5 + 0x44) = *(byte *)(iVar5 + 0x44) & 0xfe;
    }
  }
  else if (iVar5 == 1) {
    local_30 = iVar6;
    local_2c = DAT_0046051c;
    iVar5 = func_005229AC((int *)param_1[7],&local_30);
    goto LAB_0046030c;
  }
  if (param_2 == 2) {
    local_2c = DAT_00460518;
    local_30 = iVar6;
    iVar6 = func_005229AC((int *)param_1[7],&local_30);
LAB_00460374:
    if (iVar6 != 0) {
      iVar5 = param_1[7];
      if ((*(byte *)(iVar6 + 0x44) & 1) == 0) {
        *(byte *)(iVar6 + 0x44) = *(byte *)(iVar6 + 0x44) & 0xfe | 1;
        *(byte *)(iVar5 + 0x70) = *(byte *)(iVar5 + 0x70) | 2;
      }
    }
  }
  else if (param_2 == 1) {
    local_2c = DAT_0046051c;
    local_30 = iVar6;
    iVar6 = func_005229AC((int *)param_1[7],&local_30);
    goto LAB_00460374;
  }
  *(char *)((int)param_1 + 0x11a) = (char)param_2;
  if (param_2 == 2) {
    *(undefined1 *)((int)param_1 + 0x11b) = 1;
  }
  else {
    *(undefined1 *)((int)param_1 + 0x11b) = 2;
  }
LAB_004603a0:
  iVar5 = *DAT_00460520;
  iVar6 = func_00556AA8(iVar5,(int)*(char *)((int)param_1 + 0x11a));
  *(char *)(param_1 + 0x46) = (char)iVar6;
  iVar6 = func_00556AA8(iVar5,(int)*(char *)((int)param_1 + 0x11b));
  *(char *)((int)param_1 + 0x119) = (char)iVar6;
  func_003616D4(iVar5,(int)*(char *)((int)param_1 + 0x11a),0xffffffff);
  func_003616D4(iVar5,(int)*(char *)((int)param_1 + 0x11b),0xffffffff);
  piVar9 = param_1 + 0x31;
  iVar6 = 0;
  *(char *)(param_1 + 0x49) = (char)param_1[0x34] + -1;
  if (param_1[0x34] != 0) {
    do {
      param_4 = param_1[0x33] + iVar6;
      if (param_1[0x32] <= (int)param_4) {
        param_4 = param_4 - param_1[0x32];
      }
      if (*(char *)(*piVar9 + param_4 * 2) == (char)param_1[0x46]) {
        *(char *)(param_1 + 0x49) = (char)iVar6;
        break;
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 != param_1[0x34]);
  }
  func_0045F76C((int)param_1);
  iVar6 = param_1[0x2b];
  fparam_0 = DAT_00460524;
  func_00231448(iVar6 + 0x70);
  *(undefined4 *)(iVar6 + 0x78) = *(undefined4 *)(iVar6 + 0x1c);
  *(undefined2 *)(iVar6 + 0x72) = 0;
  *(undefined2 *)(iVar6 + 0x70) = 0;
  func_002317C0((undefined2 *)(iVar6 + 0x70),fparam_0);
  func_00231340((short *)(iVar6 + 0x70));
  *(undefined2 *)((int)param_1 + 0x122) = 300;
  *(undefined1 *)((int)param_1 + 0x125) = 0;
  *(undefined1 *)((int)param_1 + 0x127) = 0;
  func_001B6C2C(param_1,extraout_r1,piVar9,param_4);
  piVar9 = DAT_00460528;
  func_0065CD54((undefined4 *)(*(int *)(*DAT_00460528 + 0x130) + 0xc),1);
  func_005190AC(*piVar9);
  return func_0051C22C(*piVar9);
}

