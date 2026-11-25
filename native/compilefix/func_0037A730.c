/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern int func_0010F1DC(int *param_1,int *param_2);
extern void func_0010F1F8(int *param_1,float fparam_0);
extern int func_0052FBDC(int param_1,int param_2,int param_3);
extern int *DAT_0037a838;

// USA 0037A730 <- EU 0037A70C (Ghidra, non-matching)
// GLOBAL DAT_0037a838 int *

void func_0037A730(int param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  int *piVar5;
  float in_s0;
  undefined4 uVar6;
  
  if (param_2 == 0) {
    return;
  }
  piVar1 = *(int **)(param_1 + 0x324);
  piVar5 = DAT_0037a838;
  while (piVar1 != (int *)(param_1 + 800)) {
    piVar5 = (int *)piVar1[1];
    func_0010F1F8(piVar1,in_s0);
    *(int *)(param_1 + 0x328) = *(int *)(param_1 + 0x328) + -1;
    piVar1[-1] = *(int *)(param_1 + 0x32c);
    *(int **)(param_1 + 0x32c) = piVar1 + -1;
    piVar1 = piVar5;
    piVar5 = DAT_0037a838;
  }
  iVar3 = 0;
  if (*(char *)(param_2 + 0x58) != '\0') {
    do {
      iVar2 = func_0052FBDC(*piVar5 + 0x3dbbc,(int)*(short *)(param_2 + iVar3 * 6 + 0x5a),0);
      if (iVar2 != 0) {
        uVar6 = *(undefined4 *)(iVar2 + 0x2cc);
        if (*(int *)(param_1 + 0x328) < *(int *)(param_1 + 0x334)) {
          puVar4 = *(undefined4 **)(param_1 + 0x32c);
          if (puVar4 != (undefined4 *)0x0) {
            *(undefined4 *)(param_1 + 0x32c) = *puVar4;
            *puVar4 = 0;
            puVar4[1] = 0;
            puVar4[2] = 0;
          }
          func_0010F1DC((int *)(param_1 + 800),puVar4 + 1);
          *(int *)(param_1 + 0x328) = *(int *)(param_1 + 0x328) + 1;
          *puVar4 = uVar6;
        }
        if ((int)*(short *)(param_1 + 0x7b8) < *(int *)(param_1 + 0x328)) {
          *(undefined2 *)(param_1 + 0x7ba) = *(undefined2 *)(param_1 + 0x32);
        }
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)(uint)*(byte *)(param_2 + 0x58));
  }
  *(undefined1 *)(param_1 + 0x7c2) = 0;
  return;
}

