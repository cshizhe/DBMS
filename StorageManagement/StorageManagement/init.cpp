
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"

/**
 * @brief ��ʼ��ϵͳ�����ļ��ж�����ص���Ϣ��ɳ�ʼ��������ļ������ڣ������´����ļ��������г�ʼ����
 *
 * @param [in/out] head  : struct dbSysHead *
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int initSys(struct dbSysHead *head)
{
	FILE *fp;
	int i;

	fp = fopen( DBMS_DAT, "rb" );
	if( NULL == fp ) {
		printf("���ݿ��ļ������ڣ���ʼ�������ݿ�......\n\n");
		creaSysHead();
		fp = fopen( DBMS_DAT, "rb" );
	}
	isAvail( fp, "initSys", FOPEN);
	rewind(fp);
	fread( &(head->desc), sizeof(struct  SysDesc), 1, fp );

	head->bitMap = (unsigned long *)malloc(head->desc.sizeBitMap);
	isAvail(head->bitMap,"initSys",MALLOC);
	rewind(fp);
	fseek( fp, head->desc.bitMapAddr, SEEK_SET );
	fread( head->bitMap, 1, head->desc.sizeBitMap, fp );

	for( i=0; i<SIZE_BUFF; i++ ) {
		head->buff.map[i].pageNo = -1;
		head->buff.map[i].vstTime = 0;
		head->buff.map[i].edit =  P_UNEDIT;
	}
	head->buff.curTimeStamp = 0;

	fclose(fp);
	return 0;
}



/**
 * @brief ����һ�����г������ʼ��ϵͳ�����ѳ�ʼ������Ϣд�뵽�ļ��С�
 *
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int creaSysHead()
{
	FILE *fp;
	struct dbSysHead sysHead;
	char ch;
	
	fp = fopen( DBMS_DAT, "wb" );
	isAvail( fp, "creaSysHead", FOPEN );
	rewind( fp );
	fseek( fp, BITMAP_ADDR+SIZE_BIT_MAP+SIZE_DATA_SPACE, SEEK_SET );
	ch = '\0';
	fwrite( &ch, sizeof(char), 1, fp );
	
	/*
		����Ѳ���д���ˣ���ʵ�������ļ��ķ�ʽ��ʼ�����á�
	*/
	sysHead.desc.sizeOfFile = SIZE_DATA_SPACE;
	sysHead.desc.sizePerPage = SIZE_PER_PAGE;
	sysHead.desc.totalPage = ( SIZE_DATA_SPACE ) / ( SIZE_PER_PAGE );

	sysHead.desc.bitMapAddr =  BITMAP_ADDR ;
	sysHead.desc.sizeBitMap = sysHead.desc.totalPage / 8;	//byte
	sysHead.desc.pageAvai =  ( SIZE_DATA_SPACE ) / ( SIZE_PER_PAGE );
	
	sysHead.desc.dataAddr =  DATA_ADDR;
	sysHead.desc.iNodeNum = 0;
	sysHead.desc.iNodeAddr = INODE_ADDR;
	sysHead.desc.iNodeCur = 0;
	
	sysHead.desc.curfid = FIRST_FID;
	sysHead.desc.curFileNum = 0;
	memset(sysHead.desc.fileDesc, 0, sizeof(struct FileDesc)*MAX_FILE_NUM);

	rewind(fp);
	fwrite( &(sysHead.desc), sizeof(struct SysDesc), 1, fp);

	sysHead.bitMap = (unsigned long *)malloc( sysHead.desc.sizeBitMap );
	memset( sysHead.bitMap, -1, sysHead.desc.sizeBitMap );
	rewind(fp);
	fseek( fp, sysHead.desc.bitMapAddr, SEEK_SET );
	fwrite( sysHead.bitMap, 1, sysHead.desc.sizeBitMap, fp );

	free(sysHead.bitMap);
	fclose(fp);

	return 0;	
}