
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"


/**
 * @brief 查询一个页号是否在缓冲区中
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] query : long     要查询的页号（在数据库所有页中的相对顺序）
 * @return  int 
 * @retval  如果该页在缓冲区中，则返回它在缓冲区中的下标
 *
 * @author cshizhe
 * @date 2015/10/20 
 **/
int queryPage( struct dbSysHead *head, long query )
{
	int i;

	for( i=0; i<SIZE_BUFF; i++ ) {
		if( (head->buff).map[i].pageNo == query ) {
			return i;
		}
	}
	return BUFF_NOT_HIT;
}


/**
 * @brief 替换缓冲区中的页
 *
 * @param [in\out] head  : struct dbSysHead *
 * @param [in] mapNo : int     要换出的页在缓冲区中的下标
 * @param [in] pageNo : long   要换入的页号（在所有页中的下标）
 * @return  int 
 *
 * @author cshizhe
 * @date 2015/10/20 
 **/
int replacePage( struct dbSysHead *head, int mapNo, long pageNo )
{
	// 如果缓冲区的数据被修改过，则需要重新写回读到内存的file中
	if( P_EDIT == (head->buff).map[mapNo].edit ) {
		rewind(head->fpdesc);
		fseek( head->fpdesc, head->desc.dataAddr + (head->buff).map[mapNo].pageNo*SIZE_PER_PAGE, SEEK_SET );
		fwrite( head->buff.data[mapNo], sizeof(char), SIZE_PER_PAGE, head->fpdesc );
	}
	// 从文件中读入在所有页中是pageNo的页，放入缓冲区
	rewind(head->fpdesc);
	fseek( head->fpdesc, head->desc.dataAddr + pageNo*SIZE_PER_PAGE, SEEK_SET  );
	fread( head->buff.data[mapNo], sizeof(char), SIZE_PER_PAGE, head->fpdesc );
	head->buff.map[mapNo].edit = P_UNEDIT;
	head->buff.map[mapNo].pageNo = pageNo;
	return 0;
}


/**
 * @brief 调度算法，可以选择LRU, FIFO
 *
 * @param [in] head  : struct dbSysHead *
 * @return  int 替换的页在缓冲区的下标
 *
 * @author cshizhe
 * @date 2015/10/20 
 **/
int scheBuff( struct dbSysHead *head )
{
	return LRU(head);
}

/**
* @brief 调度算法LRU，替换出最久没有读或写的页
*
* @param [in] head  : struct dbSysHead *
* @return  int 替换的页在缓冲区的下标
*
* @author cshizhe
* @date 2015/10/20
**/
int LRU(struct dbSysHead *head)
{
	int i;
	int min;

	min = 0;
	for (i = 0; i<SIZE_BUFF; i++){
		if (head->buff.map[i].pageNo < 0) {
			return i;
		}
		else {
			if (head->buff.map[i].vstTime < head->buff.map[min].vstTime) {
				min = i;
			}
		}
	}
	return min;
}

/**
* @brief 调度算法FIFO,淘汰最早读入缓冲区的页
*
* @param [in] head  : struct dbSysHead *
* @return  int 替换的页在缓冲区的下标
*
* @author cshizhe
* @date 2015/10/20
**/
int FIFO(struct dbSysHead *head)
{
	int i;
	int min;

	min = 0;
	for (i = 0; i<SIZE_BUFF; i++){
		// 如果当前改缓冲页面为空
		if (head->buff.map[i].pageNo < 0) {
			return i;
		}
		// 如果缓冲区已满，则替换最早读入缓冲区的页面
		else {
			if (head->buff.map[i].loadTime < head->buff.map[min].loadTime) {
				min = i;
			}
		}
	}
	return min;
}

/**
 * @brief 请求读写一个页，若该页不在缓冲区中，则调用替换算法把该页掉到缓冲区中
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] query : long
 * @return  int 
 * @retval  返回该页在缓冲区中的下标
 *
 * @author cshizhe
 * @date 2015/10/20 
 **/
int reqPage( struct dbSysHead *head, long query )
{
	int i;
	int mapNo;

	// 查询缓冲区中是否存在query的页
	mapNo = queryPage( head, query );
	// 如果不存在，则利用调度算法调入缓冲区中
	if( BUFF_NOT_HIT == mapNo ) {
		mapNo = scheBuff( head );
		replacePage( head, mapNo, query );
	}
	// 读入后，系统相对时间片增加，同时修改该页的读入和访问时间
	head->buff.curTimeStamp++;
	head->buff.map[mapNo].vstTime = head->buff.curTimeStamp;
	head->buff.map[mapNo].loadTime = head->buff.curTimeStamp;
	// 如果相对时间片达到long的最大长度，全部时间都归0
	if( (1<<30) == head->buff.curTimeStamp ) {
		for( i=0; i<SIZE_BUFF; i++ ) {
			head->buff.map[i].loadTime = 0;
			head->buff.map[i].vstTime = 0;
			head->buff.curTimeStamp = 0;
		}
	}
	return mapNo;
}