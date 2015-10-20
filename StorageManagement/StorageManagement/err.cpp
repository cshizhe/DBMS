
#include<stdio.h>
#include<stdlib.h>
#include"err.h"

/**
 * @brief 用于检测指针是否可用
 *
 * @param [in] funcNum : char *     出现错误的函数名
 * @param [in] type : int     错误的类型
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int isAvail(void *p,char *funcName,int type)
{
	if( NULL==p ){
		switch(type){
			case MALLOC:
				printf("%s error:malloc error.\n",funcName);
				exit(0);
				break;
			case FOPEN:
				printf("%s error:fopen error.\n",funcName);
				exit(0);
				break;
			case ARRAY:
				printf("%s error:array error.\n",funcName);
				exit(0);
				break;
			case BOUND:
				printf("%s error:bound error.\n",funcName);
				exit(0);
				break;
			case PAGE_BOUND:
				printf("%s error:page bound error.\n",funcName);
				exit(0);
				break;
			default:
				printf("%s error:undefined error.\n",funcName);
				break;
		}
	}
	return 1;
}