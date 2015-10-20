
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"

/**
 * @brief ��ʾϵͳ�Ļ�����Ϣ
 *
 * @param [in] head  : struct dbSysHead *
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int showDesc(struct dbSysHead *head)
{
	printf("**************************************************\n\n");
	printf( "sizeOfFile:\t%ld\n", (head->desc.sizeOfFile) );
	printf("sizePerPage:\t%ld\n",(head->desc.sizePerPage) );
	printf("totalPage:\t%ld\n",(head->desc. totalPage) );
	printf("pageAvai:\t%ld\n",(head->desc.pageAvai) );
	printf("bitMapAddr:\t%ld\n",(head->desc.bitMapAddr) );
	printf("sizeBitMap:\t%ld\n",(head->desc.sizeBitMap) );
	
	printf("dataAddr:\t%ld\n",(head->desc.dataAddr) );
	printf("iNodeNum:\t%ld\n",(head->desc.iNodeNum) );
	printf("iNodeAddr:\t%ld\n",(head->desc.iNodeAddr) );
	printf("iNodeCur:\t%ld\n",(head->desc.iNodeCur) );

	printf("curfid: \t%ld\n",(head->desc.curfid) );
	printf("curFileNum:\t%ld\n",(head->desc.curFileNum) );
	
	printf("\n**************************************************\n");
	return 0;
}



/**
 * @brief ��ʾÿ���ļ��Ļ�����Ϣ
 *
 * @param [in] head  : struct dbSysHead *
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int showFileDesc( struct dbSysHead *head )
{
	int i;

	printf("*********************************************************************\n\n");
	for( i=0; i<MAX_FILE_NUM; i++ ) {
		if( head->desc.fileDesc[i].fileID > 0 ) {
			printf("fileID:\t%ld\tfileAddr:\t%ld\tpageNum:\t%ld\n",
				head->desc.fileDesc[i].fileID, head->desc.fileDesc[i].fileAddr, head->desc.fileDesc[i].filePageNum);
		}
	}
	printf("\n*********************************************************************\n");
	return 0;
}



/**
 * @brief ��ʾһ���ļ��Ĵ洢ռ������Щҳ
 *
 * @param [in] head  : struct dbSysHead *
 * @param [int] fid  : long    Ҫ��ѯ�ļ���IDֵ
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int showiNode( struct dbSysHead *head, long fid )
{
	int idx;
	struct iNode node;
	long iNodeAddr;
	int i;

	idx = queryFileID( head, fid );
	if( idx<0 ) {
		printf("showiNode error:no such fid.\n");
		return -1;
	}
	else {
		iNodeAddr = head->desc.fileDesc[idx].fileAddr;
		while( iNodeAddr > 0 ) {
			getiNodeValue( head, iNodeAddr, &node );
			for( i=0; i<node.count; i++ ) {
				printf("%ld\t",node.pageNo[i]);
			}
			iNodeAddr = node.nextAddr;
		}
	}
	printf("\n");
	return 0;
}



/**
 * @brief ��ʾϵͳ��bitmap
 *
 * @param [in] head  : struct dbSysHead *
 * @param [int] start : int    Ҫ��ѯbitMap����ʼҳ���
 * @param [int] end  : int     Ҫ��ѯbitMap����ֹҳ��� 
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int showBitMap( struct dbSysHead *head, long start, long end )
{
	long i;
	int count;
	int page;
	int pos;

	if( end > (*head).desc.totalPage ) {
		isAvail( NULL, "showBitMap", DEFAULT );
	}
	for( count=0,i=start; i<end; i++ ) {
		page = i/(8*sizeof(long));
		pos = i - 8*sizeof(long)*page + 1;	
		printf("%d",getBit( *((head->bitMap)+page),pos));
		count++;
		if( count%16 == 0 )
			printf("\t");
	}
	printf("\n");
	return 0;
}