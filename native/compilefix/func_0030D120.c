/* compilefix: USA 0030D120 (EU 0030D0FC), Ghidra-derived (decomp snapshot, ghidra_analyzed5_v4).
 * Lost argument: the virtual call through slot 0xC of the newly attached object had no arguments in
 * Ghidra's text. The ARM code passes the object itself (EU 0030D204: add r0, r6, #0x75C / str r0,
 * [r4, #0x1c] / ldr r1, [r6, #0x75c] / ldr r1, [r1, #0xc] / blx r1). Without it the callee
 * (func_0036D6BC, f(O*)) got whatever the native call left in its first argument: in the armhf
 * build its own address, so it read its code as the object's handle and func_0052FBA0 crashed
 * (scripted new game, frame ~9960, the yard). Fixed: the call passes puVar1 + 0x1d7.
 * Confidence: high (checked in the EU disassembly). The rest is the converted Ghidra text. */
// MAPFIX-INTEGRATED v1
#include "ghidra_types.h"

extern int func_0030DAE0(int *param_1,int param_2);
extern int func_0030DFA0(int *param_1,int param_2);
extern int func_00361640();
extern int func_004D80A4(float *param_1,uint param_2,float fparam_0);
extern int func_00556068(int param_1);
extern int *DAT_0030d354;
extern float DAT_0030d358;
extern float DAT_0030d35c;

// USA 0030D120 <- EU 0030D0FC (Ghidra, non-matching)
// GLOBAL DAT_0030d354 int *
// GLOBAL DAT_0030d358 float
// GLOBAL DAT_0030d35c float

undefined4 func_0030D120(int *param_1,int param_2)

{
  undefined4 *puVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  int *piVar5;
  bool bVar6;
  float fVar7;
  float fparam_0;
  _Alignas(8) unsigned char albw_frame[0x40];
  #define local_3c (*(float *)(albw_frame + 0x4))
  #define local_38 (*(float *)(albw_frame + 0x8))
  #define local_34 (*(float *)(albw_frame + 0xc))
  #define local_2c (*(float *)(albw_frame + 0x14))
  #define local_28 (*(float *)(albw_frame + 0x18))
  #define local_24 (*(float *)(albw_frame + 0x1c))
  piVar5 = DAT_0030d354;
  if (param_2 == 0) {
    if ((int *)param_1[7] != (int *)0x0) {
      (**(code **)(*(int *)param_1[7] + 0x10))((int *)param_1[7]);
    }
    param_1[7] = param_1[8];
    (**(code **)(*(int *)param_1[8] + 0xc))((int *)param_1[8]);
    param_1[9] = param_1[7];
  }
  else if (param_2 == 100) {
    if ((int *)param_1[7] != (int *)0x0) {
      (**(code **)(*(int *)param_1[7] + 0x10))((int *)param_1[7]);
    }
    piVar2 = (int *)func_00361640(*piVar5);
    param_1[7] = (int)piVar2;
    (**(code **)(*piVar2 + 0xc))(piVar2);
    param_1[9] = param_1[7];
  }
  else {
    puVar1 = func_0030DAE0(param_1,param_2);
    if (puVar1 == (undefined4 *)0x0) {
      func_0030DFA0(param_1,param_2);
      puVar1 = func_0030DAE0(param_1,param_2);
      if (puVar1 == (undefined4 *)0x0) goto LAB_0030d224;
    }
    if (*(char *)((int)puVar1 + 0x2d7) == '\0') {
      if (param_1[7] != 0) {
        puVar3 = func_00556068(param_1[7]);
        if (puVar3 == puVar1) {
          param_1[9] = param_1[7];
          return 0;
        }
        (**(code **)(*(int *)param_1[7] + 0x10))((int *)param_1[7]);
      }
      param_1[7] = (int)(puVar1 + 0x1d7);
      (**(code **)(puVar1[0x1d7] + 0xc))(puVar1 + 0x1d7);   /* this (EU 0030D204) */
      param_1[9] = param_1[7];
    }
  }
LAB_0030d224:
  if (param_1[7] != 0) {
    if ((((param_2 != 100 && param_2 != 200) && (param_2 != 0xc9 && param_2 != 0xca)) &&
        (puVar1 = func_00556068(param_1[7]), puVar1 != (undefined4 *)0x0)) &&
       ((char)param_1[0x56] == '\0')) {
      fparam_0 = DAT_0030d358;
      func_004D80A4(&local_2c,**(uint **)(*piVar5 + 0x24),DAT_0030d358);
      local_28 = DAT_0030d35c;
      fVar7 = DAT_0030d35c;
      puVar1 = func_00556068(param_1[7]);
      func_004D80A4(&local_3c,puVar1[0xfa],fparam_0);
      local_38 = fVar7;
      if (local_2c * local_3c + local_28 * fVar7 + local_24 * local_34 < fVar7) {
        iVar4 = func_00361640(*piVar5);
        *(undefined1 *)(iVar4 + 0x10) = 0;
      }
      else {
        iVar4 = func_00361640(*piVar5);
        *(undefined1 *)(iVar4 + 0x10) = 1;
      }
    }
    if (param_1[0x54] < 1) {
      piVar5 = (int *)0x0;
    }
    else {
      if (param_1[0x53] == 0) {
        return 1;
      }
      piVar5 = (int *)(param_1[0x53] - param_1[0x55]);
    }
    if (piVar5 != (int *)0x0) {
      do {
        piVar5[6] = param_1[7];
        piVar5 = *(int **)((int)piVar5 + param_1[0x55] + 4);
        bVar6 = piVar5 == param_1 + 0x52;
        if (!bVar6) {
          piVar5 = (int *)((int)piVar5 - param_1[0x55]);
          bVar6 = piVar5 == (int *)0x0;
        }
      } while (!bVar6);
    }
  }
  return 1;
}

