
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"

/**
 * @brief ���ļ���ҳת����ϵͳ��ҳ
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long    �ļ���ʶ
 * @return  long 
 * @retval  �����ļ��е�ҳ��ϵͳ�е�ҳ��
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
 * @brief ��һҳ������λ�ö����ַ�
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] pgID : long    ҳ��
 * @param [in] pos : long     ҳ����ʼ��ַ
 * @param [in] legnth : long  ��ȡ�ĳ���
 * @param [out] des : void *  ��ָ�룬���ڽ��ܶ�ȡ���ַ�
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
 * @brief ��һҳ������λ��д���ַ�
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] pgID : long    ҳ��
 * @param [in] pos : long     ҳ����ʼ��ַ
 * @param [in] legnth : long  ��ȡ�ĳ���
 * @param [out] des : void *  ��ָ�룬���ڴ���Ҫд����ַ�
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
 * @brief ��һ���ļ�������λ��д�����ⳤ�ȵ��ַ����������ļ����ȣ�����Զ���չ�ļ�����
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     �ļ���ʶ
 * @param [in] pos : long     �ļ���ʼ��ַ
 * @param [in] legnth : long  д��ĳ���
 * @param [out] des : void *  ��ָ�룬���ڴ���Ҫд����ַ�
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
		// ע�⣺SIZE_PER_PAGE * head->desc.fileDesc[i].filePageNum �������
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
 * @brief ��һ���ļ�������λ�ö������ⳤ�ȵ��ַ������ܳ����ļ��Ĵ�С��
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     �ļ���ʶ
 * @param [in] pos : long     �ļ���ʼ��ַ
 * @param [in] legnth : long  ��ȡ�ĳ���
 * @param [out] des : void *  ��ָ�룬���ڽ��ܶ�ȡ���ַ�
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
		// ע�⣺SIZE_PER_PAGE * head->desc.fileDesc[i].filePageNum �������
		isAvail(NULL,"rdFile",BOUND);
	}
	// ����û�б�Ҫmallocһ����ʱ���ڴ����������˷�ʱ�䡣Ӧ��ֱ�Ӱ�����д��des
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
