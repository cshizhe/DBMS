


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"

/**
 * @brief 更新数据库，把内存中的数据写入到磁盘中
 *
 * @param [in] head  : struct dbSysHead *
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int sysUpdate( struct dbSysHead *head )
{	
	int i;

	rewind( head->fpdesc );
	fwrite( &(head->desc), sizeof(struct SysDesc), 1, head->fpdesc );

	rewind( head->fpdesc );
	fseek( head->fpdesc, head->desc.bitMapAddr, SEEK_SET );
	fwrite( head->bitMap, sizeof(char), head->desc.sizeBitMap, head->fpdesc );

	for( i=0; i<SIZE_BUFF; i++ ) {
		if( P_EDIT == head->buff.map[i].edit ) {
			rewind( head->fpdesc );
			fseek( head->fpdesc, head->desc.dataAddr + SIZE_PER_PAGE * head->buff.map[i].pageNo, SEEK_SET );
			fwrite( head->buff.data[i], sizeof(char), SIZE_PER_PAGE, head->fpdesc);
			head->buff.map[i].edit = P_UNEDIT;
		}
	}
	return 0;
}