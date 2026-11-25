/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern void func_0010F1F8(int *param_1,float fparam_0);
extern int func_00230610(int param_1,undefined4 param_2);
extern int func_0052FBDC(int param_1,int param_2,int param_3);
extern int *DAT_0029aacc;
extern float DAT_0029aad0;
extern float DAT_0029aad4;

// USA 0029AA04 <- EU 0029A9E0 (Ghidra, non-matching)
// GLOBAL DAT_0029aacc int *
// GLOBAL DAT_0029aad0 float
// GLOBAL DAT_0029aad4 float

void func_0029AA04(int param_1)

{
  byte bVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  uint in_fpscr;
  float in_s0;
  float fVar6;
  
  iVar3 = *(int *)(param_1 + 0x43c);
  piVar2 = *(int **)(iVar3 + 0x38);
  piVar5 = DAT_0029aacc;
  while (piVar2 != (int *)(iVar3 + 0x34)) {
    piVar5 = (int *)piVar2[1];
    func_0010F1F8(piVar2,in_s0);
    *(int *)(iVar3 + 0x3c) = *(int *)(iVar3 + 0x3c) + -1;
    piVar2[-1] = *(int *)(iVar3 + 0x40);
    *(int **)(iVar3 + 0x40) = piVar2 + -1;
    piVar2 = piVar5;
    piVar5 = DAT_0029aacc;
  }
  bVar1 = *(byte *)(param_1 + 0x5c);
  iVar3 = 0;
  if (bVar1 != 0) {
    do {
      iVar4 = (int)*(short *)(param_1 + iVar3 * 6 + 0x5e);
      if ((0 < iVar4) && (iVar4 = func_0052FBDC(*piVar5 + 0x3dbbc,iVar4,0), iVar4 != 0)) {
        func_00230610(*(int *)(param_1 + 0x43c),*(undefined4 *)(iVar4 + 0x2cc));
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)(uint)bVar1);
  }
  iVar3 = (int)*(short *)(param_1 + 0x2e);
  if (iVar3 == 0) {
    in_s0 = DAT_0029aad0;
  }
  *(int *)(param_1 + 0x434) = iVar3;
  if (iVar3 == 0) {
    *(float *)(param_1 + 0x438) = in_s0;
  }
  else {
    fVar6 = (float)VectorUnsignedToFloat(iVar3,(byte)(in_fpscr >> 0x15) & 3);
    *(float *)(param_1 + 0x438) = DAT_0029aad4 / fVar6;
  }
  *(undefined4 *)(param_1 + 0x430) = 0;
  return;
}

