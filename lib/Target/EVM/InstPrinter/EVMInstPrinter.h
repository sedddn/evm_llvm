//===-- EVMInstPrinter.h - Convert EVM MCInst to asm syntax ---*- C++ -*--//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints a EVM MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_EVM_INSTPRINTER_EVMINSTPRINTER_H
#define LLVM_LIB_TARGET_EVM_INSTPRINTER_EVMINSTPRINTER_H

#include "MCTargetDesc/EVMMCTargetDesc.h"
#include "llvm/MC/MCInstPrinter.h"

namespace llvm {
class MCOperand;

class EVMInstPrinter : public MCInstPrinter {
public:
  EVMInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                   const MCRegisterInfo &MRI)
      : MCInstPrinter(MAI, MII, MRI) {}

  void printInst(const MCInst *MI, raw_ostream &O, StringRef Annot,
                 const MCSubtargetInfo &STI) override;
  void printRegName(raw_ostream &O, unsigned RegNo) const override;

  void printOperand(const MCInst *MI, unsigned OpNo, const MCSubtargetInfo &STI,
                    raw_ostream &O, const char *Modifier = nullptr);

  // Autogenerated by tblgen.
  static const char *getRegisterName(unsigned RegNo);
};
} // namespace llvm

#endif