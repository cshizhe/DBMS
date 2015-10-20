
#include<stdio.h>
#include<stdlib.h>
#include"err.h"

/**
 * @brief ���ڼ��ָ���Ƿ����
 *
 * @param [in] funcNum : char *     ���ִ���ĺ�����
 * @param [in] type : int     ���������
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