
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"



/**
 * @brief 设定一个unsigned long类型的某个位
 *
 * @param [in] num  : unsigned long *    要修改的unsigned long
 * @param [in] pos : int    要修改的unsigned long的位置，该unsigned long的最高位为第1位
 * @param [in] setValue : int    要设定的值，只能为0或1
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int setBit( unsigned long *num, int pos, int setValue )
{
	unsigned long result;

	if( setValue != 1 && setValue !=0 ) {
		return -1;
	}
	if( setValue == getBit( *num , pos ) ) {
		return 0;
	}
	result = 0x00000001;
    result = result<<(32-pos);
	if( setValue == 0 ) {
        result = ~result;
		*num = *num & result;
	} else {
	    *num = *num + result;
	}
    return 0;
}



/**
 * @brief 读取一个unsigned long类型的某个位
 *
 * @param [in] num  : unsigned long *    要读取的unsigned long
 * @param [in] pos : int    要读取的unsigned long的位置，该unsigned long的最高位为第1位
 * @return  int 
 * @retval  返回值为0或1
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int getBit(unsigned long num, int pos)
{
   unsigned long result;

   result = 0x00000001;
   result = result<<(32-pos);
   result = result & num;
   if( result == 0 ) {
	   return 0;
   } else {
	   return 1;
   }
}