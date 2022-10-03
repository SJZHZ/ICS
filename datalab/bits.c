/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 朱越 Zhu Yue 1900017702@pku.edu.cn 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/* 
 * minusOne - return a value of -1 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int minusOne(void) {
  return ~0;
}
/* 
 * implication - return x -> y in propositional logic - 0 for false, 1
 * for true
 *   Example: implication(1,1) = 1
 *            implication(1,0) = 0
 *   Legal ops: ! ~ ^ |
 *   Max ops: 5
 *   Rating: 2
 */
int implication(int x, int y) {
    return !x | y;
}
/* 
 * leastBitPos - return a mask that marks the position of the
 *               least significant 1 bit. If x == 0, return 0
 *   Example: leastBitPos(96) = 0x20
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2 
 */
int leastBitPos(int x) {
	//x's leastBit means all 0 ahead		xxxx10000
	//meanwhile, ~x's bit is opposite to x		yyyy01111
	//~x + 1 = -x					yyyy10000
	//x & -x					000010000
  return x & (~x + 1);
}
/* 
 * rotateLeft - Rotate x to the left by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateLeft(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateLeft(int x, int n) {
	int k = 33 + ~n;	//32 - n
	int q = ~0;
	
	int c = q + (1 << n);
	int y = (x >> k) & c;//left n numbers to right
	
	int d = (1 << k) + q;
	int z = (x & d) << n;//right 32-n numbers to left
	//unbelievable!
	//1 << 32 = 0
	//1 << k = 1(while k = 32)

	int condi = q + !!n;
	int other = condi & x;
  return z + y + other;

}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
	//~0 & a represents a * 1
	//0 & b represents b * 0
	int f1 = ~0;
	int fx = !x;
	int left = f1 + fx;
	int right = ~left;
  return (left & y) + (right & z);//weighted average
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
	int k = x ^ (~x + 1);//high all 1
	//TMin gets 0
  return ((k >> 31) + 1) & (~(x >> 31));
}
/* 
 * oneMoreThan - return 1 if y is one more than x, and 0 otherwise
 *   Examples oneMoreThan(0, 1) = 1, oneMoreThan(-1, 1) = 0
 *   Legal ops: ~ & ! ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int oneMoreThan(int x, int y) {
//  return !((x + 1) ^ y) & !!((1 << 31) ^ y);//y != TMin
  return !( ((x + 1) ^ y) | !((1 << 31) ^ y) );//!a & !b = !(a | b)
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
	int k = n + ~0;
	//x < 1 << n or ~x < 1 << n
  return !(x >> k) | !(~x >> k);

//  return !( (x >> k) & (~x >> k) );
//  Wrong, this is not bool! It's bit
//  notice that 1 & 2 = 0
}
/*
 * multFiveEighths - multiplies by 5/8 rounding toward 0.
 *   Should exactly duplicate effect of C expression (x*5/8),
 *   including overflow behavior.
 *   Examples: multFiveEighths(77) = 48
 *             multFiveEighths(-22) = -13
 *             multFiveEighths(1073741824) = 13421728 (overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */
int multFiveEighths(int x) {
		//int n = 1 << 31;
		//int y = n & x;	//get highest bit
	int z;
	x = x + (x << 2);//5x

	z = x >> 31;//x's sign
		//x = (x ^ (n & x)) + y;//make highest bit
	x = x + (z & 7);//up or down?
	x = x >> 3;//
  return x;
}
/*
 * satMul2 - multiplies by 2, saturating to Tmin or Tmax if overflow
 *   Examples: satMul2(0x30000000) = 0x60000000
 *             satMul2(0x40000000) = 0x7FFFFFFF (saturate to TMax)
 *             satMul2(0x90000000) = 0x80000000 (saturate to TMin)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int satMul2(int x) {
	int k = 1 << 31;//TMin
	int a = x >> 31;//x's sign
	int y = x << 1;//2x
//*2 overflow: + to - or - to +
	int b = ((x & k) ^ y) >> 31;//overflow? (compare 31st bit)
  return (y & ~b) + (k & b & a) + (~k & b & ~a);
}
/*
 * modThree - calculate x mod 3 without using %.
 *   Example: modThree(12) = 0,
 *            modThree(2147483647) = 1,
 *            modThree(-8) = -2,
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 60
 *   Rating: 4
 */
int modThree(int x) {
//观察发现，4和1模3同余，所以4k和k模3同余
//进一步，16和1模3同余
//把数字按上述规律分段，对齐、求和，模3结果不变
//最后可以控制在0-3之间
//注意最后控制在0-2比较麻烦——不妨控制在1-3，然后遇3减3
//注意负数模出来是非正的
	int ans;
	int n1 = ~0;
	int signx = x >> 31;
	int temp8 = (1 << 8) + n1;
	int bias = signx;//n1 & signx;  //negative to -1,positive or 0 to 0
	int dealthree;
	int dealnegative;
	
	int firstB = temp8 & (x >> 24);//consider as unsigned(equal to plus 1 << 32)
	int secondB = temp8 & (x >> 16);
	int thirdB = temp8 & (x >> 8);
	int lastB = temp8 & x;
	
	int firstMod = firstB + secondB + lastB + thirdB;//may be 10 bits
	int B1 = (firstMod >> 8) & 15;
	int B2 = (firstMod >> 4) & 15;
	int B3 = firstMod & 15;
	
	int secondMod = B1 + B2 + B3 + 3 + bias;//less than 6 bits
	int B4 = (secondMod >> 4) & 3;
	int B5 = (secondMod >> 2) & 3;
	int B6 = secondMod & 3;

	int thirdMod = B4 + B5 + B6;//MAX 3*3
	int B7 = (thirdMod >> 2) & 3;
	int B8 = thirdMod & 3;

	int forthMod = B7 + B8;//MAX 6
	int B9 = (forthMod >> 2) & 3;
	int B10 = forthMod & 3;

	int fifthMod = B9 + B10;//MAX 4
	//int B11 = (fifthMod >> 2) & 3;
	//int B12 = fifthMod & 3;
	
	//int sixthMod = B11 + B12;//MAX 3
	//int B13 = (sixthMod >> 2) & 3;
	//int B14 = sixthMod & 3;
	//int seventhMod = B13 + B14;//MAX 3
	
	//confirm sixthMod is enough
	ans = fifthMod;//test fifthMod
	dealnegative = signx & ~2;
	ans = ans + dealnegative;
	
	dealthree = (!(ans ^ 3) << 31) >> 31;
	ans = ans + (dealthree & ~2);
    return ans;
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
//找最高位的1在第几位
//二分法

//每次判断左半边是否有1
//ans+=半边长度
//如果有1，右移半边长度
//否则，不右移
//继续二分判断
	int y = !!(x >> 16);
	int ans = (y << 4);
	int temp = ans;
	x = x >> temp;//32to16

	y = !!(x >> 8);
	temp = y << 3;
	ans = temp + ans;
	x = x >> temp;//16to8

	y = !!(x >> 4);
	temp = y << 2;
	ans = temp + ans;
	x = x >> temp;//8to4

	y = !!(x >> 2);
	temp = y << 1;
	ans = temp + ans;
	x = x >> temp;//4to2

	y = (x >> 1);//y只剩一位
	temp = y;
	ans = temp + ans;

  return ans;
}
/* 
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_abs(unsigned uf) {
	unsigned x = uf;
		if(x << 9)//not INF
			if (((x >> 23) + 1) >> 9)//is NaN
				return uf;
  return (uf << 1) >> 1;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
//向偶数舍入

	int pointer = 31;
	int sign = x >> 31;
	unsigned ux;
	int exp = 126;
	unsigned ans;
	int bias;
	int tail;
	int biaspow;
	int halfbp;

	if (!x)
		return 0;
	if (sign)
		x = -x;//consider as unsigned, so TMin is OK

	while(pointer)
	{
		if ((1 << pointer) & x)
			break;
		pointer = pointer - 1;
	}//get highest bit

	ux = x;
	if (pointer > 23)//need to round
	{
		bias = pointer - 23;
		biaspow = 1 << bias;
		tail = biaspow - 1;
		tail = tail & ux;//get tail
		ux = ux >> (pointer - 23);
		halfbp = biaspow >> 1;
		if ((tail + halfbp) >> bias)//over half
		{
			if (tail - halfbp)//not half
				ux = ux + 1;
			else if (ux & 1)//half & last 1
				ux = ux + 1;
		//I think +1 may cause pointer +1?
			if (ux >> 23)//point change(first 1 goes left)
			{
				ux >> 1;
				pointer = pointer + 1;
			}

		}




	}
	else
		ux = ux << (23 - pointer);
	exp = exp + pointer;//should be +127
	ans = (exp << 23) + ux;//includes ux's first 1

	if (sign)
		return ans + (1 << 31);
	else
		return ans;
	//ans = sign + exp + ux
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
	int sign = uf >> 31;//negative should ~ans + 1
	int exp = (uf << 1) >> 24;
	int frac = (uf << 9) >> 9;
	int OFR = 1 << 31;
	int M = exp - 127;
	if ((exp + 1) >> 8)
		return OFR;//NaN or infinity
	if (!exp)
		return 0;//unnormalized
	if (M > 30)
		return OFR;
	if (M < 0)
		return 0;
	//text book says
	//float trans to int
	//round to 0

	frac = frac + (1 << 23);
	if (M > 23)
		frac = frac << (M - 23);
	if (M < 23)
		frac = frac >> (23 - M);
	if (sign)
	return -frac;//TMin is also OFR, don't worry
  return frac;
}
/* 
 * float_negpwr2 - Return bit-level equivalent of the expression 2.0^-x
 *   (2.0 raised to the power -x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^-x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned float_negpwr2(int x) {
	unsigned infinity = 255 << 23;
	x = -x;//-x
	if (x > 127)
		return infinity;
	if (x == (1 << 31))//-TMin = TMin
		return infinity;

	x = x + 127;//adjust
	if (x < -22)//too small
		return 0;
	if (x < 1)
		return 1 << (22 + x);//unnormalized
    return x << 23;//normalized
}
