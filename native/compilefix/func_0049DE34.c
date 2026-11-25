/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern int func_0010C0B8(int param_1);
extern int func_0010EEE0(int param_1);
extern void func_0010F1F8(int *param_1,float fparam_0);
extern int func_003CD8E0(int param_1);
extern int func_0048D9D0(int *param_1,byte *param_2);
extern int func_0049D774(int param_1,int *param_2);
extern int func_0049DC40(int param_1,byte *param_2,int param_3);
extern int func_0049E064(int param_1);
extern int *DAT_0049e038;

// USA 0049DE34 <- EU 0049DE0C (Ghidra, non-matching)
// GLOBAL DAT_0049e038 int *

undefined4 func_0049DE34(int param_1)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  float in_s0;
  
  func_0010C0B8(param_1 + 4);
  piVar3 = *(int **)(param_1 + 0x94);
  if ((piVar3 != (int *)0x0) &&
     (iVar2 = func_003CD8E0(*(int *)(*(int *)(param_1 + 0x20) + 0x20) + 4), iVar2 != 0)) {
    func_0049D774(param_1,piVar3);
    func_0010EEE0(*(int *)(*(int *)(param_1 + 0x20) + 0x20) + 4);
  }
  piVar3 = *(int **)(param_1 + 0x30);
  while (piVar4 = piVar3, piVar4 != (int *)(param_1 + 0x2c)) {
    piVar3 = (int *)piVar4[1];
    if (piVar4[3] != 0) {
      piVar4[3] = 0;
      func_0010F1F8(piVar4,in_s0);
      *(int *)(param_1 + 0x34) = *(int *)(param_1 + 0x34) + -1;
    }
  }
  piVar3 = *(int **)(param_1 + 0x48);
  while (piVar4 = piVar3, piVar4 != (int *)(param_1 + 0x44)) {
    piVar3 = (int *)piVar4[1];
    if (piVar4[3] != 0) {
      piVar4[3] = 0;
      func_0010F1F8(piVar4,in_s0);
      *(int *)(param_1 + 0x4c) = *(int *)(param_1 + 0x4c) + -1;
    }
  }
  piVar3 = *(int **)(param_1 + 0x60);
  while (piVar4 = piVar3, piVar4 != (int *)(param_1 + 0x5c)) {
    piVar3 = (int *)piVar4[1];
    if (piVar4[3] != 0) {
      piVar4[3] = 0;
      func_0010F1F8(piVar4,in_s0);
      *(int *)(param_1 + 100) = *(int *)(param_1 + 100) + -1;
    }
  }
  piVar4 = *(int **)(param_1 + 0x6c);
  piVar3 = DAT_0049e038;
  while (piVar1 = piVar4, piVar1 != (int *)(param_1 + 0x68)) {
    piVar4 = (int *)piVar1[1];
    if (piVar1[3] != 0) {
      piVar1[3] = 0;
      func_0010F1F8(piVar1,in_s0);
      *(int *)(param_1 + 0x70) = *(int *)(param_1 + 0x70) + -1;
      piVar3 = DAT_0049e038;
    }
  }
  iVar2 = 0;
  if (0 < *piVar3) {
    do {
      piVar4 = *(int **)(param_1 + iVar2 * 4 + 0x74);
      if (piVar4 != (int *)0x0) {
        (**(code **)(*piVar4 + 0x28))(piVar4);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < *piVar3);
  }
  func_0049E064(param_1);
  *(undefined4 *)(param_1 + 0x94) = 0;
  *(undefined1 *)(param_1 + 0x110) = 1;
  iVar2 = param_1 + 4;
  *(int *)(param_1 + 0x104) = param_1 + 0x94;
  *(undefined4 *)(param_1 + 0xfc) = 0;
  func_0010C0B8(iVar2);
  if (*(int **)(param_1 + 0xfc) == (int *)0x0) {
    func_0049DC40(param_1,(byte *)(param_1 + 0x98),0);
    func_0010EEE0(iVar2);
  }
  else {
    func_0048D9D0(*(int **)(param_1 + 0xfc),(byte *)(param_1 + 0x98));
    func_0010EEE0(iVar2);
  }
  return func_0010EEE0(param_1 + 4);
}

