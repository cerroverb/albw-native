/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern int func_00100174(int *param_1);
extern int func_0010B3EC(undefined4 *param_1,int param_2,int param_3);
extern int func_004A8334(int param_1,int param_2,int param_3,int param_4,uint param_5);
extern int func_004A8930(int param_1,float *param_2,int *param_3,uint param_4);
extern int func_004ED9F4(int param_1,int param_2,int param_3,int param_4);
extern int func_0052FBA0(int param_1,uint param_2);
extern int func_00589A64(int param_1);
extern int *DAT_004aac60;
extern int *DAT_004aac64;
extern int DAT_004aac68;
extern int DAT_004aac6c;
extern int *DAT_004aac70;
extern uint *DAT_004aac74;

// USA 004AA9FC <- EU 004AA9D4 (Ghidra, non-matching)
// PARAM param_5 Stack[0x0]:4 uint
// GLOBAL DAT_004aac60 int *
// GLOBAL DAT_004aac64 int *
// GLOBAL DAT_004aac68 int
// GLOBAL DAT_004aac6c int
// GLOBAL DAT_004aac70 int *
// GLOBAL DAT_004aac74 uint *

undefined4 func_004AA9FC(int param_1,int param_2,int param_3,int param_4,uint param_5)

{
  uint *puVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint extraout_r1;
  uint extraout_r1_00;
  uint *puVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  bool bVar13;
  
  piVar2 = DAT_004aac60;
  iVar4 = (int)*(short *)(param_2 + 0x10);
  if (-1 < iVar4) {
    if (iVar4 == 0xff) {
      iVar4 = func_0052FBA0(*DAT_004aac64 + 0x3dbbc,*(uint *)(param_2 + 0xc));
      if ((iVar4 == 0) || (iVar4 = (int)*(short *)(iVar4 + 0x54), iVar4 < 0)) {
        iVar4 = (int)*(short *)(*(int *)(*piVar2 + 0x38) + 0x15b40);
      }
      if (iVar4 < 0) goto LAB_004aaa94;
    }
    iVar3 = DAT_004aac6c;
    iVar4 = *(int *)(*piVar2 + 0x38) + iVar4 * DAT_004aac68 * 4;
    if (iVar4 != -0x1c3c) {
      if (((*(uint *)(DAT_004aac6c + 0x10) & 1) == 0) &&
         (iVar10 = func_00100174(DAT_004aac70), puVar1 = DAT_004aac74, iVar10 != 0)) {
        puVar8 = DAT_004aac74 + 3;
        *DAT_004aac74 = 0;
        puVar1[1] = 0;
        puVar1[2] = 0;
        func_0010B3EC(puVar1,9,(int)puVar8);
      }
      uVar12 = 5;
      do {
        if ((param_5 & 1 << (uVar12 & 0xff)) != 0) {
          iVar10 = iVar4 + 0x1cf0;
          iVar5 = iVar4 + 0x1cf0;
          uVar7 = param_5;
          if (uVar12 < 6) {
            uVar7 = uVar12 * 3;
            iVar10 = iVar4 + 0x1cf0 + uVar12 * 0xc;
            iVar5 = iVar4 + 0x1cf0 + uVar12 * 0xc;
          }
          puVar1 = DAT_004aac74;
          for (iVar5 = *(int *)(iVar5 + 4); iVar5 != iVar10;
              iVar5 = *(int *)(iVar5 + 4)) {
            iVar6 = *(int *)(iVar5 + 8);
            *(int *)(iVar3 + 8) = iVar6;
            bVar13 = *(uint *)(param_2 + 0xc) != *(uint *)(iVar6 + 0x20);
            if (bVar13) {
              uVar7 = (uint)*(byte *)(iVar6 + 0x44);
            }
            if ((bVar13 && uVar7 != 0) &&
               (bVar13 = func_00589A64(iVar6), uVar7 = extraout_r1, bVar13)) {
              func_004ED9F4(*(int *)(iVar5 + 8),param_2,param_3,param_4);
              uVar7 = extraout_r1_00;
            }
            *(undefined4 *)(iVar3 + 8) = 0;
            puVar1 = DAT_004aac74;
          }
          *puVar1 = 0;
          func_004A8930(param_1,(float *)(param_4 + 4),(int *)puVar1,uVar12);
          uVar11 = *puVar1;
          uVar7 = 0;
          if (0 < (int)uVar11) {
            do {
              if (uVar7 < *DAT_004aac74) {
                iVar10 = *(int *)(DAT_004aac74[2] + uVar7 * 4);
                iVar5 = *(int *)(DAT_004aac74[2] + uVar7 * 4);
              }
              else {
                iVar10 = 0;
                iVar5 = 0;
              }
              for (iVar5 = *(int *)(iVar5 + 4); iVar5 != iVar10; iVar5 = *(int *)(iVar5 + 4)) {
                iVar6 = *(int *)(iVar5 + 8);
                *(int *)(iVar3 + 8) = iVar6;
                uVar9 = *(uint *)(param_2 + 0xc);
                bVar13 = uVar9 != *(uint *)(iVar6 + 0x20);
                if (bVar13) {
                  uVar9 = (uint)*(byte *)(iVar6 + 0x44);
                }
                if ((bVar13 && uVar9 != 0) && (bVar13 = func_00589A64(iVar6), bVar13)) {
                  func_004ED9F4(*(int *)(iVar5 + 8),param_2,param_3,param_4);
                }
                *(undefined4 *)(iVar3 + 8) = 0;
              }
              uVar7 = uVar7 + 1;
            } while ((int)uVar7 < (int)uVar11);
          }
        }
        uVar12 = uVar12 - 1;
      } while (-1 < (int)uVar12);
      return 0;
    }
  }
LAB_004aaa94:
  return func_004A8334(param_1,param_2,param_3,param_4,param_5);
}

