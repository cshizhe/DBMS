
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"


/**
 * @brief 查询一个文件是否存在
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     文件标识
 * @param [out] i : int    该文件在结构中的下标
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
 * @brief 初始化一个Node
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] startPage : long     为该Node分配的连续的页面的第一个页面的页号
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
 * @brief 获得一个Node的值
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] iNodeAddr : long     该Node在存储文件中的位置
 * @param [out] newNode :struct iNode *    用于接受相应的Node
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
 * @brief 把一个Node的信息写入存储文件
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] iNodeAddr : long     该Node在存储文件中的位置
 * @param [out] newNode :struct iNode *    要写入的Node
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
 * @brief 获得一个文件的最后一个Node在存储文件中的位置
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     文件标识
 * @return  long
 * @retval  存储文件的位置
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
 * @brief 扩展一个文件的空间
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] fid : long     文件标识
 * @param [in] extendPage : long    要扩展的页数
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
 * @brief 创建一个文件，为它分配空间
 *
 * @param [in/out] head  : struct dbSysHead *
 * @return  int 
 * @retval  返回文件的标识号，即fid
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
 * @brief 为一个文件分配连续的空间（如果存储文件有足够的空间，但不是连续的，则分配不成功）
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] reqPageNum : int    请求分配的页数
 * @return  long 
 * @retval  如果分配成功，则返回该连续空间的第一个页面的页号
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
 * @brief 回收一个页
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] pageNo : int    要回收的页号
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
 * @brief 回收分配给一个文件的所有页
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] iNodeAddr : long    该文件的Node在存储文件的首地址
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
 * @brief 删除一个文件
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] fid : long    要回收文件的标识
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