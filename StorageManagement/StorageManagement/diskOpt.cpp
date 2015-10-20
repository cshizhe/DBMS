
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"


/**
 * @brief ��ѯһ���ļ��Ƿ����
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     �ļ���ʶ
 * @param [out] i : int    ���ļ��ڽṹ�е��±�
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int queryFileID( struct dbSysHead *head, long fid )
{
	int i;

	for( i=0; i<MAX_FILE_NUM; i++ ) {
		if( (head->desc).fileDesc[i].fileID == fid ){
				break;
		}
	}
	if( MAX_FILE_NUM == i )
		return -1;
	else
		return i;
}


/**
 * @brief ��ʼ��һ��Node
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] startPage : long     Ϊ��Node�����������ҳ��ĵ�һ��ҳ���ҳ��
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int initiNode( struct iNode *newNode, long startPage, long pageNum, long fd, long preAddr,long nextAddr )
{
	long i;
	
	for( i=0; i<pageNum; i++ ){
			(*newNode).pageNo[i] = startPage+i;
	}
	(*newNode).count = pageNum;
	(*newNode).fid = fd;
	(*newNode).preAddr = preAddr;
	(*newNode).nextAddr = nextAddr;
	return 0;
}



/**
 * @brief ���һ��Node��ֵ
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] iNodeAddr : long     ��Node�ڴ洢�ļ��е�λ��
 * @param [out] newNode :struct iNode *    ���ڽ�����Ӧ��Node
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int getiNodeValue( struct dbSysHead *head, long iNodeAddr,struct iNode *newNode)
{
	rewind(head->fpdesc);
	fseek( head->fpdesc, iNodeAddr, SEEK_SET );
	fread( newNode, sizeof(struct iNode),1, head->fpdesc );
	return 0;
}



/**
 * @brief ��һ��Node����Ϣд��洢�ļ�
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] iNodeAddr : long     ��Node�ڴ洢�ļ��е�λ��
 * @param [out] newNode :struct iNode *    Ҫд���Node
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int wrtiNodeValue( struct dbSysHead *head, long iNodeAddr,struct iNode *newNode)
{
	rewind(head->fpdesc);
	fseek( head->fpdesc, iNodeAddr, SEEK_SET );
	fwrite( newNode, sizeof(struct iNode),1, head->fpdesc );
	return 0;
}



/**
 * @brief ���һ���ļ������һ��Node�ڴ洢�ļ��е�λ��
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     �ļ���ʶ
 * @return  long
 * @retval  �洢�ļ���λ��
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
long getLastiNodeAddr( struct dbSysHead *head, long fid )
{
	struct iNode newNode;
	int descNo;
	long iNodeAddr;
	long curAddr;

	descNo = queryFileID( head, fid );
	if(descNo<0) {
		return descNo;
	}
	iNodeAddr = (head->desc).fileDesc[descNo].fileAddr;
	while( iNodeAddr>0 ) {
		getiNodeValue(head,iNodeAddr,&newNode);
		curAddr = iNodeAddr;
		iNodeAddr = newNode.nextAddr;
	}
	return curAddr;
}



/**
 * @brief ��չһ���ļ��Ŀռ�
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] fid : long     �ļ���ʶ
 * @param [in] extendPage : long    Ҫ��չ��ҳ��
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int extendFileSpace( struct dbSysHead *head, long fid, int extendPage )
{
	long iNodeAddr;
	struct iNode newNode;
	struct iNode lastNode;
	int allocStaPage;
	int i;
	int count;
	int nPage;
	int numPage;
	int idx;

	numPage = extendPage;
	allocStaPage = allocatePage( head, numPage );
	if( allocStaPage < 0 ) {
		printf("extendFileSpace error:out of disk space.");
		exit(0);
	}
	iNodeAddr = getLastiNodeAddr( head, fid );
	getiNodeValue( head, iNodeAddr, &lastNode );
	if( lastNode.count < PAGE_PER_INODE ) {
		nPage = (numPage < (PAGE_PER_INODE - lastNode.count)) ?  numPage : (PAGE_PER_INODE - lastNode.count);
		count = lastNode.count;
		for( i=0; i<nPage; i++ ) {
			lastNode.pageNo[count+i] = allocStaPage ;
			allocStaPage++;
		 }
		lastNode.count += nPage;
		wrtiNodeValue(head,iNodeAddr,&lastNode);
		numPage -= nPage;
	}
	if( numPage > 0 ) {
		while( numPage > 0 ) {
			nPage = (numPage < PAGE_PER_INODE) ? numPage:PAGE_PER_INODE; 
			initiNode( &newNode, allocStaPage,nPage, fid, iNodeAddr, -1 );
			allocStaPage += nPage;

			lastNode.nextAddr = (head->desc).iNodeAddr + (head->desc).iNodeCur * sizeof(struct iNode);
			newNode.preAddr = iNodeAddr;
			wrtiNodeValue( head, iNodeAddr, &lastNode );
			iNodeAddr = lastNode.nextAddr;
			(head->desc).iNodeCur++;
			(head->desc).iNodeNum++;

			lastNode.count = newNode.count;
			lastNode.nextAddr = newNode.nextAddr;
			lastNode.preAddr = newNode.preAddr;
			for( i=0; i<newNode.count; i++ ) {
				lastNode.pageNo[i] = newNode.pageNo[i];
			}
			numPage -= nPage;
		}
		wrtiNodeValue( head, iNodeAddr, &newNode );
	}
	idx = queryFileID(head,fid);
	head->desc.fileDesc[idx].filePageNum += extendPage;
	return 0;
}



/**
 * @brief ����һ���ļ���Ϊ������ռ�
 *
 * @param [in/out] head  : struct dbSysHead *
 * @return  int 
 * @retval  �����ļ��ı�ʶ�ţ���fid
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
long creatFileSpace( struct dbSysHead *head )
{
	struct iNode newNode;
	long inodeAddr;
	long alloStaPage;
	int i;

	if( (head->desc).curFileNum >= MAX_FILE_NUM ) {
			printf("creatFile Space error:total files cannot be more than %d.\n",(head->desc).curFileNum);
			return -1;
	}
	inodeAddr = (head->desc).iNodeAddr + sizeof(struct iNode) * (head->desc).iNodeCur;
	alloStaPage = allocatePage( head, PAGE_PER_INODE );
	if( alloStaPage >= 0 ) {
			for( i=0; i<MAX_FILE_NUM; i++ ) {
				if( (head->desc).fileDesc[i].fileID <= 0 ) {
					break;
				}
			}
			(head->desc).fileDesc[i].fileID = (head->desc).curfid;
			(head->desc).fileDesc[i].fileAddr = inodeAddr;
			(head->desc).fileDesc[i].filePageNum = PAGE_PER_INODE;
			(head->desc).curFileNum++;
			initiNode( &newNode, alloStaPage, PAGE_PER_INODE, (head->desc).fileDesc[i].fileID, -1, -1 );
			rewind(head->fpdesc);
			fseek( head->fpdesc,  (head->desc).fileDesc[i].fileAddr, SEEK_SET );
			fwrite( &newNode, sizeof(struct iNode), 1, head->fpdesc );
			(head->desc).iNodeNum++;
			(head->desc).iNodeCur++;
			(head->desc).curfid++;
	}
	else {
			printf("creatFileSpace error:creat error");
			exit(0);
	}
	return (head->desc).fileDesc[i].fileID;
}



/**
 * @brief Ϊһ���ļ����������Ŀռ䣨����洢�ļ����㹻�Ŀռ䣬�����������ģ�����䲻�ɹ���
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] reqPageNum : int    ��������ҳ��
 * @return  long 
 * @retval  �������ɹ����򷵻ظ������ռ�ĵ�һ��ҳ���ҳ��
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
long allocatePage( struct dbSysHead *head, int reqPageNum )
{
	long totalPage;
	int i,j;
	int page,pos;
	int count;
	int alloStaPage;

	totalPage = head->desc.totalPage;
	for( i=0; i<totalPage; i++ ) {
			page = i/(8*sizeof(long));
			pos = i - 8*sizeof(long)*page + 1;
			count = 0;
			if( getBit( *(head->bitMap+page),pos) == P_AVAI ) {
				for( j=i; count<reqPageNum && j<totalPage; j++ ) {
						page = j/(8*sizeof(long));
						pos = j - 8*sizeof(long)*page + 1;
						if( getBit( *(head->bitMap+page),pos) == P_AVAI ) {
							count++;
						}
						else {
							break;
						}
				}
			}
			if( count == reqPageNum ) {
				break;
			}
			else {
				i = i + count;
			}
	}
	if( count != reqPageNum ) {
		return ALLO_FAIL;
	}
	else {
		alloStaPage = i;
		for( j=0; j<reqPageNum; j++ ) {
			page = (i+j) / ( 8*sizeof(long) );
			pos = (i+j) - 8*sizeof(long)*page + 1;
			setBit( head->bitMap+page, pos, P_UNAVAI );
		}
		(head->desc).pageAvai -= reqPageNum;
	}
	return alloStaPage;
}



/**
 * @brief ����һ��ҳ
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] pageNo : int    Ҫ���յ�ҳ��
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int recyOnePage( struct dbSysHead *head ,long pageNo )
{
	int page,pos;

	page = pageNo / ( sizeof(long) * 8 );
	pos = pageNo - page*8*sizeof(long) + 1;
	setBit( head->bitMap+page, pos, P_AVAI );
	return 0;
}



/**
 * @brief ���շ����һ���ļ�������ҳ
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] iNodeAddr : long    ���ļ���Node�ڴ洢�ļ����׵�ַ
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int recyPage( struct dbSysHead *head, long iNodeAddr )
{
	struct iNode newNode;
	int i;

	getiNodeValue( head, iNodeAddr, &newNode );
	for( i=0; i<newNode.count; i++ ) {
			recyOnePage( head, *(newNode.pageNo+i) );
	}
	(head->desc).iNodeNum--;
	if( newNode.nextAddr > 0 ) {
			recyPage( head, newNode.nextAddr );
	}
	return 0;
}



/**
 * @brief ɾ��һ���ļ�
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] fid : long    Ҫ�����ļ��ı�ʶ
 * @return  int 
 *
 * @author linzhixia
 * @date 2013/11/6 
 **/
int recyFileSpace( struct dbSysHead *head, long fid )
{
	int i;

	i =  queryFileID(head,fid);
	if( i < 0 ) {
		return i;
	}
	recyPage( head, (head->desc).fileDesc[i].fileAddr );
	(head->desc).pageAvai += (head->desc).fileDesc[i].filePageNum;
	(head->desc).fileDesc[i].fileAddr = 0;
	(head->desc).fileDesc[i].fileID = 0;
	(head->desc).fileDesc[i].filePageNum = 0;
	(head->desc).curFileNum--;
	return 0;
}