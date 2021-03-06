// Tests of inferred bounds for expressions in assignments and declarations.
// The goal is to check that the bounds are being inferred correctly.  This
// file covers:
// - Assignments to variables with _Array_ptr and declarations of
//   variables of pointer type,
// - where the right-hand side or intializing expression is an integer,
//    variable, or address-of expression.
//
// The tests have the general form:
// 1. Some C code.
// 2. A description of the inferred bounds for that C code:
//  a. The source assignnment or declaration.
//  b. The expected bounds.
//  c. The inferred bounds.
// The description uses AST dumps.
//
// This line is for the clang test infrastructure:
// RUN: %clang_cc1 -fcheckedc-extension -verify -fdump-inferred-bounds %s | FileCheck %s

//-------------------------------------------------------------------------//
// Test assignment of integers to _Array_ptr variables.  This covers both  //
// 0 (NULL) and non-zero integers (the results of casts).                  //
//-------------------------------------------------------------------------//

// First test the null case.  Also test different ways of declaring the same
// bounds for the lhs of an assignment
void f1(_Array_ptr<int> a : bounds(a, a + 5)) {
  a = 0;
}

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <NullToPointer>
// CHECK:  `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 0
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK:   |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK:   | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK:   `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: RHS Bounds:
// CHECK:  NullaryBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'Any

void f2(_Array_ptr<int> b : count(5)) {
  b = 0;
}

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'b' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <NullToPointer>
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 0
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'b' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'b' '_Array_ptr<int>'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: RHS Bounds:
// CHECK: NullaryBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'Any

void f3(_Array_ptr<int> c : byte_count(sizeof(int) * 5)) {
  c = 0;
}

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'c' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <NullToPointer>
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 0
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-CStyleCastExpr {{0x[0-9a-f]+}} '_Array_ptr<char>' <BitCast>
// CHECK: | `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: |   `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'c' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<char>' '+'
// CHECK: |-CStyleCastExpr {{0x[0-9a-f]+}} '_Array_ptr<char>' <BitCast>
// CHECK: | `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: |   `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'c' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} 'unsigned {{.*}}' '*'
// CHECK: |-UnaryExprOrTypeTraitExpr {{0x[0-9a-f]+}} 'unsigned {{.*}}' sizeof 'int'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} 'unsigned {{.*}}' <IntegralCast>
// CHECK:       `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: RHS Bounds:
// CHECK: NullaryBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'Any

//  Test assignment of an integer constant expressed as
// an enum constant.
enum E1 {
  EnumVal1,
  EnumVal2
};

void f4(_Array_ptr<int> d : count(5)) {
  d = (_Array_ptr<int>) EnumVal1;
}

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'd' '_Array_ptr<int>'
// CHECK: `-CStyleCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <NullToPointer>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} 'int' EnumConstant {{0x[0-9a-f]+}} 'EnumVal1' 'int'
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'd' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'd' '_Array_ptr<int>'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: RHS Bounds:
// CHECK: NullaryBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'Any

void f5(void) {
  _Array_ptr<int> d : count(5) = 0;
}

// Now test the non-null case.  Also test different ways of declaring the same
// bounds for the lhs of an assignment

// CHECK: VarDecl {{0x[0-9a-f]+}} {{.*}} d '_Array_ptr<int>' cinit
// CHECK: |-CountBoundsExpr {{0x[0-9a-f]+}} {{.*}} 'NULL TYPE' Element
// CHECK: | `-IntegerLiteral {{0x[0-9a-f]+}} {{.*}} 'int' 5
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} {{.*}} '_Array_ptr<int>' <NullToPointer>
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} {{.*}} 'int' 0
// CHECK: Declared Bounds:
// CHECK: CountBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Element
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: Initializer Bounds:
// CHECK: NullaryBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'Any

void f6(_Array_ptr<int> a : bounds(a, a + 5)) {
  a = (_Array_ptr<int>) 5; // expected-error {{expression has no bounds}}
}

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-CStyleCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <IntegralToPointer>
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: RHS Bounds:
// CHECK: NullaryBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Invalid

void f7(void) {
  _Array_ptr<int> d : count(5) = (_Array_ptr<int>) 5; // expected-error {{expression has no bounds}}
}

// CHECK: VarDecl {{0x[0-9a-f]+}} {{.*}} d '_Array_ptr<int>' cinit
// CHECK: |-CountBoundsExpr {{0x[0-9a-f]+}} {{.*}} 'NULL TYPE' Element
// CHECK: | `-IntegerLiteral {{0x[0-9a-f]+}} {{.*}} 'int' 5
// CHECK:`-CStyleCastExpr {{0x[0-9a-f]+}} {{.*}} '_Array_ptr<int>' <IntegralToPointer>
// CHECK:`-IntegerLiteral {{0x[0-9a-f]+}} {{.*}} 'int' 5
// CHECK: Declared Bounds:
// CHECK: CountBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Element
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: Initializer Bounds:
// CHECK: NullaryBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Invalid

//-------------------------------------------------------------------------//
// Test assignment of variables to _Array_ptr variables.  This covers both //
// variables with bounds and variables without bounds.                     //
//-------------------------------------------------------------------------//

void f20(_Array_ptr<int> a : count(len),
         _Array_ptr<int> b : count(len),
         int len) {
  a = b;
}

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'b' '_Array_ptr<int>'
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} 'int' <LValueToRValue>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} 'int' lvalue ParmVar {{0x[0-9a-f]+}} 'len' 'int'
// CHECK: RHS Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'b' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'b' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} 'int' <LValueToRValue>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} 'int' lvalue ParmVar {{0x[0-9a-f]+}} 'len' 'int'

void f21(_Array_ptr<int> a : count(5),
         _Array_ptr<int> b) {
  a = b;  // expected-error {{expression has no bounds}}
}

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'b' '_Array_ptr<int>'
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: RHS Bounds:
// CHECK: NullaryBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Invalid

// Only test declarations for the negative case (where an error is expected}
void f22(_Array_ptr<int> b) {
  _Array_ptr<int> a : count(5) = b;  // expected-error {{expression has no bounds}}
}

// CHECK: VarDecl {{0x[0-9a-f]+}} {{.*}} a '_Array_ptr<int>' cinit
// CHECK: |-CountBoundsExpr {{0x[0-9a-f]+}} {{.*}} 'NULL TYPE' Element
// CHECK: | `-IntegerLiteral {{0x[0-9a-f]+}} {{.*}} 'int' 5
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} {{.*}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} {{.*}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'b' '_Array_ptr<int>'
// CHECK: Declared Bounds:
// CHECK: CountBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Element
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: Initializer Bounds:
// CHECK: NullaryBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Invalid

//-------------------------------------------------------------------------//
// Test assignment of arrays to _Array_ptr variables with bounds           //
//-------------------------------------------------------------------------//

void f30(_Array_ptr<int> a : count(3)) {
  int arr[5];
  a = arr;
}

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <BitCast>
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} 'int *' <ArrayToPointerDecay>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5]'
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue ParmVar {{0x[0-9a-f]+}} 'a' '_Array_ptr<int>'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 3
// CHECK: RHS Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int *':'int *' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5]'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} 'int *':'int *' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int *':'int *' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5]'
// CHECK:   `-IntegerLiteral {{0x[0-9a-f]+}} 'unsigned long long' 5

void f31(void) {
  int arr[5];
  _Array_ptr<int> b : count(3) = arr;
}

// CHECK: VarDecl {{0x[0-9a-f]+}} {{.*}} b '_Array_ptr<int>' cinit
// CHECK: |-CountBoundsExpr {{0x[0-9a-f]+}} {{.*}} 'NULL TYPE' Element
// CHECK: | `-IntegerLiteral {{0x[0-9a-f]+}} {{.*}} 'int' 3
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} {{.*}} '_Array_ptr<int>' <BitCast>
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} {{.*}} 'int *' <ArrayToPointerDecay>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} {{.*}} 'int [5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5]'
// CHECK: Declared Bounds:
// CHECK: CountBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Element
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 3
// CHECK: Initializer Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int *':'int *' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5]'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} 'int *':'int *' '+'
// CHECK:   |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int *':'int *' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5]'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'unsigned long long' 5

//---------------------------------------------------------------------------//
// Test inference of bounds for address-of (&) expressions. The & operator  //
// can be applied to lvalue expressions, so this includes tests of & applied //
// dereference and array-subscriptiong expressions.                          //
//--------------------------------------------------------==-----------------//

// Address-of scalar variable.
void f40(void) {
  int x;
  _Array_ptr<int> p : count(1) = &x;
}

// CHECK:  VarDecl {{0x[0-9a-f]+}} {{.*}} p '_Array_ptr<int>' cinit
// CHECK: |-CountBoundsExpr {{0x[0-9a-f]+}} <col:23, col:30> 'NULL TYPE' Element
// CHECK: | `-IntegerLiteral {{0x[0-9a-f]+}} <col:29> 'int' 1
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} <col:34, col:35> '_Array_ptr<int>' <BitCast>
// CHECK: `-UnaryOperator {{0x[0-9a-f]+}} <col:34, col:35> 'int *' prefix '&'
// CHECK:     `-DeclRefExpr {{0x[0-9a-f]+}} <col:35> 'int' lvalue Var {{0x[0-9a-f]+}} 'x' 'int'
// CHECK: Declared Bounds:
// CHECK: CountBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Element
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 1
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-UnaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' prefix '&'
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int' lvalue Var {{0x[0-9a-f]+}} 'x' 'int'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-UnaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' prefix '&'
// CHECK:   | `-DeclRefExpr {{0x[0-9a-f]+}} 'int' lvalue Var {{0x[0-9a-f]+}} 'x' 'int'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'unsigned long long' 1

// Address-of array variable.
void f41(void) {
  int x[5];
  _Array_ptr<int> p : count(5) = (_Array_ptr<int>) &x;
}

// CHECK: VarDecl {{0x[0-9a-f]+}} <{{.*}} p '_Array_ptr<int>' cinit
// CHECK: |-CountBoundsExpr {{0x[0-9a-f]+}} <col:23, col:30> 'NULL TYPE' Element
// CHECK: | `-IntegerLiteral {{0x[0-9a-f]+}} <col:29> 'int' 5
// CHECK: `-CStyleCastExpr {{0x[0-9a-f]+}} <col:34, col:53> '_Array_ptr<int>' <BitCast>
// CHECK: `-UnaryOperator {{0x[0-9a-f]+}} <col:52, col:53> 'int (*)[5]' prefix '&'
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} <col:53> 'int [5]' lvalue Var {{0x[0-9a-f]+}} 'x' 'int [5]'
// CHECK: Declared Bounds:
// CHECK: CountBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Element
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: Initializer Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int *':'int *' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5]' lvalue Var {{0x[0-9a-f]+}} 'x' 'int [5]'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} 'int *':'int *' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int *':'int *' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5]' lvalue Var {{0x[0-9a-f]+}} 'x' 'int [5]'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'unsigned long long' 5

// Address-of dereference of a pointer.
void f42(void) {
  int x;
  _Array_ptr<int> p : count(1) = &x;
  _Array_ptr<int> q : count(1) = &*p;

// CHECK: VarDecl {{0x[0-9a-f]+}} {{.*}} q '_Array_ptr<int>' cinit
// CHECK: |-CountBoundsExpr {{0x[0-9a-f]+}} {{.*}} 'NULL TYPE' Element
// CHECK: | `-IntegerLiteral {{0x[0-9a-f]+}} <col:29> 'int' 1
// CHECK: `-UnaryOperator {{0x[0-9a-f]+}} <col:34, col:36> '_Array_ptr<int>' prefix '&'
// CHECK: `-UnaryOperator {{0x[0-9a-f]+}} <col:35, col:36> 'int' lvalue prefix '*'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} <col:36> '_Array_ptr<int>' <LValueToRValue>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} <col:36> '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: Declared Bounds:
// CHECK: CountBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Element
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 1
// CHECK: Initializer Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 1

  _Array_ptr<int> r : count(1) = &p[0];

// CHECK: VarDecl {{0x[0-9a-f]+}} {{.*}} r '_Array_ptr<int>' cinit
// CHECK: |-CountBoundsExpr {{0x[0-9a-f]+}} {{.*}} 'NULL TYPE' Element
// CHECK: | `-IntegerLiteral {{0x[0-9a-f]+}} {{.*}} 'int' 1
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} {{.*}} '_Array_ptr<int>' <BitCast>
// CHECK: `-UnaryOperator {{0x[0-9a-f]+}} {{.*}} 'int *' prefix '&'
// CHECK:     `-ArraySubscriptExpr {{0x[0-9a-f]+}} {{.*}} 'int' lvalue
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} {{.*}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} {{.*}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} {{.*}} 'int' 0
// CHECK: Declared Bounds:
// CHECK: CountBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE' Element
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 1
// CHECK: Initializer Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK:   `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 1
}

// Address-of multi-dimensional array access.
// - Use assignments instead of declarations to
//   cover inference of assignment with address-of
//   expressions.
void f43(void) {
  int arr[5][5];
  _Array_ptr<int> p : count(5) = 0;

// CHECK: VarDecl {{0x[0-9a-f]+}} {{.*}} used p '_Array_ptr<int>' cinit
// Skip remaining details of this declaration.
  _Array_ptr<int> r : bounds(arr, arr + 5) = 0;

// CHECK: VarDecl {{0x[0-9a-f]+}} {{.*}} used r '_Array_ptr<int>' cinit
// Skip remaining details of this declaration.
  p = *arr;

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <BitCast>
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} 'int *' <ArrayToPointerDecay>
// CHECK: `-UnaryOperator {{0x[0-9a-f]+}} 'int [5]' lvalue prefix '*'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} 'int (*)[5]' <ArrayToPointerDecay>
// CHECK: `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5][5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5][5]'
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: RHS Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int (*)[5]':'int (*)[5]' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5][5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5][5]'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} 'int (*)[5]':'int (*)[5]' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int (*)[5]':'int (*)[5]' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5][5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5][5]'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'unsigned long long' 5

  p = arr[0];

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <BitCast>
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} 'int *' <ArrayToPointerDecay>
// CHECK: `-ArraySubscriptExpr {{0x[0-9a-f]+}} 'int [5]' lvalue
// CHECK:       |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int (*)[5]' <ArrayToPointerDecay>
// CHECK:       | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5][5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5][5]'
// CHECK:       `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 0
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK:   `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: RHS Bounds:
// CHECK:  RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int (*)[5]':'int (*)[5]' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5][5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5][5]'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} 'int (*)[5]':'int (*)[5]' '+'
// CHECK:   |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int (*)[5]':'int (*)[5]' <ArrayToPointerDecay>
// CHECK:   | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5][5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5][5]'
// CHECK:   `-IntegerLiteral {{0x[0-9a-f]+}} 'unsigned long long' 5

  r = &p[0];

// CHECK: BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '='
// CHECK: |-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'r' '_Array_ptr<int>'
// CHECK: `-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <BitCast>
// CHECK: `-UnaryOperator {{0x[0-9a-f]+}} 'int *' prefix '&'
// CHECK: `-ArraySubscriptExpr {{0x[0-9a-f]+}} 'int' lvalue
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK:       | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK:       `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 0
// CHECK: Target Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int (*)[5]' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5][5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5][5]'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} 'int (*)[5]' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} 'int (*)[5]' <ArrayToPointerDecay>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} 'int [5][5]' lvalue Var {{0x[0-9a-f]+}} 'arr' 'int [5][5]'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5
// CHECK: RHS Bounds:
// CHECK: RangeBoundsExpr {{0x[0-9a-f]+}} 'NULL TYPE'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-BinaryOperator {{0x[0-9a-f]+}} '_Array_ptr<int>' '+'
// CHECK: |-ImplicitCastExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' <LValueToRValue>
// CHECK: | `-DeclRefExpr {{0x[0-9a-f]+}} '_Array_ptr<int>' lvalue Var {{0x[0-9a-f]+}} 'p' '_Array_ptr<int>'
// CHECK: `-IntegerLiteral {{0x[0-9a-f]+}} 'int' 5

}
