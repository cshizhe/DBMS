
#include"file.h"
#include"err.h"

#define DBMS_DAT "dbms.dat"

struct dbSysHead
{
	struct SysDesc desc;
	struct buffSpace buff;

	unsigned long *bitMap;	
	FILE *fpdesc;	//������DBMS�洢�����ļ���
};


//	bit.cpp
int setBit( unsigned long *num, int pos, int setValue );
int getBit(unsigned long num, int pos);

/*******************	buffManage.cpp   *******************/
// ��ѯҳquery��ȫ��ҳ�ţ��Ƿ��ڻ�������
int queryPage( struct dbSysHead *head, long query );
// ��pageNo��ȫ��ҳ�ţ�ҳ�滻���������±�ΪmapNo��ҳ
int replacePage( struct dbSysHead *head, int mapNo, long pageNo );
// ������ҳ������㷨����ѡ��LRU����FIFO
int scheBuff( struct dbSysHead *head );
// ������ҳ����ȣ�LRU
int LRU(struct dbSysHead *head);
// ������ҳ����ȣ�FIFO
int FIFO(struct dbSysHead *head);
// �򻺳���������queryҳ�棬�������ڻ������е��±�
int reqPage( struct dbSysHead *head, long query );

//	diskOpt.cpp
long allocatePage( struct dbSysHead *head,int reqPageNum );
int initiNode( struct iNode *newNode, long startPage, long pageNum, long fd, long preAddr,long nextAddr );
long creatFileSpace( struct dbSysHead *head );
int recyOnePage(struct dbSysHead *head,long pageNo );
int recyPage( struct dbSysHead *head, long iNodeAddr );
int recyFileSpace( struct dbSysHead *head, long fid );
int queryFileID( struct dbSysHead *head, long fid );
int getiNodeValue(struct dbSysHead *head, long iNodeAddr,struct iNode *newNode);
int wrtiNodeValue(struct dbSysHead *head, long iNodeAddr,struct iNode *newNode);
long getLastiNodeAddr( struct dbSysHead *head, long fid );
int extendFileSpace( struct dbSysHead *head, long fid, int numPage );

//	err.cpp
int isAvail(void *p,char *funcName,int type);

//	fileOpt.cpp
long mapPage( struct dbSysHead *head, long fid, long num );
int readInPage( struct dbSysHead *head, long pgID,long pos,long length, void *des );
int writeInPage( struct dbSysHead *head, long pgID, long pos, long length, void *des );
int rdFile( struct dbSysHead *head, long fid, long pos, long length, void *des);
int wtFile( struct dbSysHead *head, long fid, long pos, long length, void *des);

//	init.cpp
int initSys(struct dbSysHead *head);
int creaSysHead();

//	show.cpp
int showDesc(struct dbSysHead *head);
int showFileDesc( struct dbSysHead *head );
int showiNode( struct dbSysHead *head, long fid );
int showBitMap( struct dbSysHead *head, long start, long end );

//	update.cpp
int sysUpdate( struct dbSysHead *head );