/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern undefined8 func_0010E3C4(uint *param_1,uint *param_2,uint param_3);
extern int func_00112EA0(undefined4 *param_1,int param_2,uint param_3,undefined4 *param_4);
extern float *DAT_002c0cc4;
extern uint DAT_002c0cc8;
extern int DAT_002c0ccc;

// USA 002C0B34 <- EU 002C0B10 (Ghidra, non-matching)
// GLOBAL DAT_002c0cc4 float *
// GLOBAL DAT_002c0cc8 uint
// GLOBAL DAT_002c0ccc int

void func_002C0B34(int param_1)

{
  float *pfVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int *piVar6;
  float *pfVar7;
  int iVar8;
  uint uVar9;
  uint *puVar10;
  int *piVar11;
  float fVar12;
  float fVar13;
  _Alignas(8) unsigned char albw_frame[0x70];
  #define local_70 ((float *)(albw_frame + 0x0))
  piVar11 = (int *)(param_1 + 0x2a08);
  iVar3 = *piVar11;
  *piVar11 = 1 - iVar3;
  iVar3 = param_1 + (1 - iVar3) * 0x14;
  piVar4 = *(int **)(iVar3 + 0x24);
  piVar6 = piVar4 + *(int *)(iVar3 + 0x1c);
  pfVar1 = DAT_002c0cc4;
  uVar2 = DAT_002c0cc8;
  for (; piVar4 != piVar6; piVar4 = piVar4 + 1) {
    *(undefined4 *)(*piVar4 + 0x15e4) = 0;
    pfVar1 = DAT_002c0cc4;
    uVar2 = DAT_002c0cc8;
  }
  for (iVar3 = *(int *)(param_1 + 0x48); iVar3 + -0x4c != param_1 + -8; iVar3 = *(int *)(iVar3 + 4))
  {
    fVar12 = *pfVar1;
    fVar13 = pfVar1[1];
    iVar8 = 0;
    do {
      iVar5 = iVar8 + 1;
      pfVar7 = (float *)(iVar3 + -0x4c + iVar8 * 0xc);
      local_70[iVar8 * 3] = *pfVar7 - fVar12;
      local_70[iVar8 * 3 + 1] = pfVar7[1] - fVar13;
      local_70[iVar8 * 3 + 2] = pfVar7[2];
      iVar8 = iVar5;
    } while (iVar5 < 6);
    iVar8 = param_1 + *piVar11 * 0x14;
    if (*(uint *)(iVar3 + -4) < *(uint *)(iVar8 + 0x1c)) {
      iVar8 = *(int *)(*(int *)(iVar8 + 0x24) + *(uint *)(iVar3 + -4) * 4);
    }
    else {
      iVar8 = 0;
    }
    puVar10 = (uint *)(iVar8 + 0x15e4);
    uVar9 = *puVar10;
    if ((uVar9 < 0x3a) && (uVar9 * 0x12 < uVar2)) {
      func_0010E3C4((uint *)(iVar8 + uVar9 * 0x48),(uint *)local_70,0x48);
      *puVar10 = *puVar10 + 1;
    }
  }
  iVar3 = param_1 + *piVar11 * 0x14;
  piVar4 = *(int **)(iVar3 + 0x24);
  piVar6 = piVar4 + *(int *)(iVar3 + 0x1c);
  if (piVar4 != piVar6) {
    do {
      *(int *)(*piVar4 + 0x15dc) = *(int *)(DAT_002c0ccc + *piVar4) * 6;
      func_00112EA0((undefined4 *)(*piVar4 + 0xe48),param_1 + 0x2784,4,
                    (undefined4 *)(param_1 + 0x28dc));
      piVar4 = piVar4 + 1;
    } while (piVar4 != piVar6);
  }
  return;
}

