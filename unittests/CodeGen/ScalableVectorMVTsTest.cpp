//===-------- llvm/unittest/CodeGen/ScalableVectorMVTsTest.cpp ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/MachineValueType.h"
#include "llvm/Support/ScalableSize.h"
#include "gtest/gtest.h"

using namespace llvm;

namespace {

TEST(ScalableVectorMVTsTest, IntegerMVTs) {
  for (auto VecTy : MVT::integer_scalable_vector_valuetypes()) {
    ASSERT_TRUE(VecTy.isValid());
    ASSERT_TRUE(VecTy.isInteger());
    ASSERT_TRUE(VecTy.isVector());
    ASSERT_TRUE(VecTy.isScalableVector());
    ASSERT_TRUE(VecTy.getScalarType().isValid());

    ASSERT_FALSE(VecTy.isFloatingPoint());
  }
}

TEST(ScalableVectorMVTsTest, FloatMVTs) {
  for (auto VecTy : MVT::fp_scalable_vector_valuetypes()) {
    ASSERT_TRUE(VecTy.isValid());
    ASSERT_TRUE(VecTy.isFloatingPoint());
    ASSERT_TRUE(VecTy.isVector());
    ASSERT_TRUE(VecTy.isScalableVector());
    ASSERT_TRUE(VecTy.getScalarType().isValid());

    ASSERT_FALSE(VecTy.isInteger());
  }
}

TEST(ScalableVectorMVTsTest, HelperFuncs) {
  LLVMContext Ctx;

  // Create with scalable flag
  EVT Vnx4i32 = EVT::getVectorVT(Ctx, MVT::i32, 4, /*Scalable=*/true);
  ASSERT_TRUE(Vnx4i32.isScalableVector());

  // Create with separate llvm::ElementCount
  auto EltCnt = ElementCount(2, true);
  EVT Vnx2i32 = EVT::getVectorVT(Ctx, MVT::i32, EltCnt);
  ASSERT_TRUE(Vnx2i32.isScalableVector());

  // Create with inline llvm::ElementCount
  EVT Vnx2i64 = EVT::getVectorVT(Ctx, MVT::i64, {2, true});
  ASSERT_TRUE(Vnx2i64.isScalableVector());

  // Check that changing scalar types/element count works
  EXPECT_EQ(Vnx2i32.widenIntegerVectorElementType(Ctx), Vnx2i64);
  EXPECT_EQ(Vnx4i32.getHalfNumVectorElementsVT(Ctx), Vnx2i32);

  // Check that overloaded '*' and '/' operators work
  EXPECT_EQ(EVT::getVectorVT(Ctx, MVT::i64, EltCnt * 2), MVT::nxv4i64);
  EXPECT_EQ(EVT::getVectorVT(Ctx, MVT::i64, EltCnt / 2), MVT::nxv1i64);

  // Check that float->int conversion works
  EVT Vnx2f64 = EVT::getVectorVT(Ctx, MVT::f64, {2, true});
  EXPECT_EQ(Vnx2f64.changeTypeToInteger(), Vnx2i64);

  // Check fields inside llvm::ElementCount
  EltCnt = Vnx4i32.getVectorElementCount();
  EXPECT_EQ(EltCnt.Min, 4U);
  ASSERT_TRUE(EltCnt.Scalable);

  // Check that fixed-length vector types aren't scalable.
  EVT V8i32 = EVT::getVectorVT(Ctx, MVT::i32, 8);
  ASSERT_FALSE(V8i32.isScalableVector());
  EVT V4f64 = EVT::getVectorVT(Ctx, MVT::f64, {4, false});
  ASSERT_FALSE(V4f64.isScalableVector());

  // Check that llvm::ElementCount works for fixed-length types.
  EltCnt = V8i32.getVectorElementCount();
  EXPECT_EQ(EltCnt.Min, 8U);
  ASSERT_FALSE(EltCnt.Scalable);
}

TEST(ScalableVectorMVTsTest, IRToVTTranslation) {
  LLVMContext Ctx;

  Type *Int64Ty = Type::getInt64Ty(Ctx);
  VectorType *ScV8Int64Ty = VectorType::get(Int64Ty, {8, true});

  // Check that we can map a scalable IR type to an MVT 
  MVT Mnxv8i64 = MVT::getVT(ScV8Int64Ty);
  ASSERT_TRUE(Mnxv8i64.isScalableVector());
  ASSERT_EQ(ScV8Int64Ty->getElementCount(), Mnxv8i64.getVectorElementCount());
  ASSERT_EQ(MVT::getVT(ScV8Int64Ty->getElementType()),
            Mnxv8i64.getScalarType());

  // Check that we can map a scalable IR type to an EVT
  EVT Enxv8i64 = EVT::getEVT(ScV8Int64Ty);
  ASSERT_TRUE(Enxv8i64.isScalableVector());
  ASSERT_EQ(ScV8Int64Ty->getElementCount(), Enxv8i64.getVectorElementCount());
  ASSERT_EQ(EVT::getEVT(ScV8Int64Ty->getElementType()),
            Enxv8i64.getScalarType());
}

TEST(ScalableVectorMVTsTest, VTToIRTranslation) {
  LLVMContext Ctx;

  EVT Enxv4f64 = EVT::getVectorVT(Ctx, MVT::f64, {4, true});

  Type *Ty = Enxv4f64.getTypeForEVT(Ctx);
  VectorType *ScV4Float64Ty = cast<VectorType>(Ty);
  ASSERT_TRUE(ScV4Float64Ty->isScalable());
  ASSERT_EQ(Enxv4f64.getVectorElementCount(), ScV4Float64Ty->getElementCount());
  ASSERT_EQ(Enxv4f64.getScalarType().getTypeForEVT(Ctx),
            ScV4Float64Ty->getElementType());
}

} // end anonymous namespace
