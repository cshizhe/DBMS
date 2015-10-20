
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"

/**
 * @brief 把文件的页转化成系统的页
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long    文件标识
 * @return  long 
 * @retval  返回文件中的页在系统中的页号
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
long mapPage( struct dbSysHead *head, long fid, long num )
{
	int idx;
	long iNodeAddr;
	struct iNode node;
	long pageMap;
	int ithNode;
	int tmp;

	
	idx = queryFileID( head, fid );
	if( idx <0 ) {
		isAvail(NULL,"mapPage",ARRAY);
	}
	if( num > head->desc.fileDesc[idx].filePageNum ) {
		isAvail(NULL,"mapPage",PAGE_BOUND);
	}
	iNodeAddr = head->desc.fileDesc[idx].fileAddr;
	ithNode = num / PAGE_PER_INODE + 1;
	while( ithNode > 0 ) {
		getiNodeValue( head, iNodeAddr, &node );
		iNodeAddr = node.nextAddr;
		ithNode --;
	}
	tmp = num / PAGE_PER_INODE;
	num = num - tmp * PAGE_PER_INODE;
	pageMap = node.pageNo[num];
	if( num>PAGE_PER_INODE ) {
		isAvail(NULL,"mapPage",DEFAULT);
	}
	return pageMap;
}



/**
 * @brief 从一页中任意位置读入字符
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] pgID : long    页号
 * @param [in] pos : long     页内起始地址
 * @param [in] legnth : long  读取的长度
 * @param [out] des : void *  空指针，用于接受读取的字符
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int readInPage( struct dbSysHead *head, long pgID,long pos,long length, void *des )
{
	int nPage;

	if( pos + length > SIZE_PER_PAGE ) {
		isAvail(NULL,"readInPage",PAGE_BOUND);
	}
	nPage = reqPage( head, pgID );
	memcpy( des, (head->buff).data[nPage]+pos, length );
	return 0;
}



/**
 * @brief 往一页中任意位置写入字符
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] pgID : long    页号
 * @param [in] pos : long     页内起始地址
 * @param [in] legnth : long  读取的长度
 * @param [out] des : void *  空指针，用于传递要写入的字符
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int writeInPage( struct dbSysHead *head, long pgID, long pos, long length, void *des )
{
	int nPage;

	if( pos + length > SIZE_PER_PAGE ) {
		isAvail(NULL,"writeInPage",PAGE_BOUND);
	}
	nPage = reqPage(head,pgID);
	memcpy( (head->buff).data[nPage]+pos, des, length );
	head->buff.map[nPage].edit = P_EDIT;
	return 0;
}



/**
 * @brief 往一个文件中任意位置写入任意长度的字符，若超过文件长度，则会自动扩展文件长度
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     文件标识
 * @param [in] pos : long     文件起始地址
 * @param [in] legnth : long  写入的长度
 * @param [out] des : void *  空指针，用于传递要写入的字符
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int wtFile( struct dbSysHead *head, long fid, long pos, long length, void *des)
{
	int i;
	int idx;
	int extpage;
	long staPage,endPage;
	long pnt;
	long rSta,rEnd;
	long pageMap;
	char *tmp;

	idx = queryFileID(head,fid);
	if( idx<0 ) {
		isAvail(NULL,"wtFile",ARRAY);
	}
	if( (pos+length) > SIZE_PER_PAGE * head->desc.fileDesc[idx].filePageNum ){
		// 注意：SIZE_PER_PAGE * head->desc.fileDesc[i].filePageNum 可能溢出
		extpage = (pos+length) /  SIZE_PER_PAGE - head->desc.fileDesc[idx].filePageNum + 1;
		extendFileSpace( head, fid, extpage );
	}
	tmp = (char *)malloc( length+1 );
	isAvail( tmp, "wtFile", MALLOC );
	memcpy( tmp, des, length );
	staPage = pos/SIZE_PER_PAGE;
	endPage = (pos+length-1)/SIZE_PER_PAGE;
	pnt = 0;
	for( i=staPage; i<(endPage+1); i++ ) {
		rSta = (pos+pnt) - ( (pos+pnt)/SIZE_PER_PAGE ) * SIZE_PER_PAGE;
		rEnd = ( (i+1)*SIZE_PER_PAGE>(pos+length)) ? ((pos+length) - ((pos+length)/SIZE_PER_PAGE)*SIZE_PER_PAGE) : SIZE_PER_PAGE ;
		pageMap = mapPage( head, fid, i);
		writeInPage( head, pageMap, rSta, rEnd - rSta, tmp+pnt );
		pnt += (rEnd-rSta);
	}
	free(tmp);
	return 0;
}



/**
 * @brief 从一个文件中任意位置读入任意长度的字符（不能超过文件的大小）
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     文件标识
 * @param [in] pos : long     文件起始地址
 * @param [in] legnth : long  读取的长度
 * @param [out] des : void *  空指针，用于接受读取的字符
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int rdFile( struct dbSysHead *head, long fid, long pos, long length, void *des)
{
	int i;
	int idx;
	long staPage,endPage;
	long pnt;
	long rSta,rEnd;
	long pageMap;
	char *tmp;

	idx = queryFileID(head,fid);
	if( idx<0 ) {
		isAvail(NULL,"rdFile",ARRAY);
	}
	if( (pos+length) > SIZE_PER_PAGE * head->desc.fileDesc[idx].filePageNum ){
		// 注意：SIZE_PER_PAGE * head->desc.fileDesc[i].filePageNum 可能溢出
		isAvail(NULL,"rdFile",BOUND);
	}
	// 这里没有必要malloc一块临时的内存区，这样浪费时间。应该直接把数据写到des
	tmp = (char *)malloc( length+1 );
	isAvail( tmp, "rdFile", MALLOC );
	staPage = pos/SIZE_PER_PAGE;
	endPage = (pos+length-1)/SIZE_PER_PAGE;
	pnt = 0;
	for( i=staPage; i<(endPage+1); i++ ) {
		rSta = (pos+pnt) - ( (pos+pnt)/SIZE_PER_PAGE ) * SIZE_PER_PAGE;
		rEnd = ( (i+1)*SIZE_PER_PAGE>(pos+length)) ? ((pos+length) - ((pos+length)/SIZE_PER_PAGE)*SIZE_PER_PAGE) : SIZE_PER_PAGE ;
		pageMap = mapPage( head, fid, i);
		readInPage( head, pageMap, rSta, rEnd - rSta, tmp+pnt );
		pnt += (rEnd-rSta);
	}
	memcpy( des, tmp, length );
	free(tmp);
	return 0;
}
