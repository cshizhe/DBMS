
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"



/**
 * @brief �趨һ��unsigned long���͵�ĳ��λ
 *
 * @param [in] num  : unsigned long *    Ҫ�޸ĵ�unsigned long
 * @param [in] pos : int    Ҫ�޸ĵ�unsigned long��λ�ã���unsigned long�����λΪ��1λ
 * @param [in] setValue : int    Ҫ�趨��ֵ��ֻ��Ϊ0��1
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
 * @brief ��ȡһ��unsigned long���͵�ĳ��λ
 *
 * @param [in] num  : unsigned long *    Ҫ��ȡ��unsigned long
 * @param [in] pos : int    Ҫ��ȡ��unsigned long��λ�ã���unsigned long�����λΪ��1λ
 * @return  int 
 * @retval  ����ֵΪ0��1
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