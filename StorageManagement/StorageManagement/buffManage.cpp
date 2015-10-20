
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"


/**
 * @brief ��ѯһ��ҳ���Ƿ��ڻ�������
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] query : long     Ҫ��ѯ��ҳ�ţ������ݿ�����ҳ�е����˳��
 * @return  int 
 * @retval  �����ҳ�ڻ������У��򷵻����ڻ������е��±�
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
 * @brief �滻�������е�ҳ
 *
 * @param [in\out] head  : struct dbSysHead *
 * @param [in] mapNo : int     Ҫ������ҳ�ڻ������е��±�
 * @param [in] pageNo : long   Ҫ�����ҳ�ţ�������ҳ�е��±꣩
 * @return  int 
 *
 * @author cshizhe
 * @date 2015/10/20 
 **/
int replacePage( struct dbSysHead *head, int mapNo, long pageNo )
{
	// ��������������ݱ��޸Ĺ�������Ҫ����д�ض����ڴ��file��
	if( P_EDIT == (head->buff).map[mapNo].edit ) {
		rewind(head->fpdesc);
		fseek( head->fpdesc, head->desc.dataAddr + (head->buff).map[mapNo].pageNo*SIZE_PER_PAGE, SEEK_SET );
		fwrite( head->buff.data[mapNo], sizeof(char), SIZE_PER_PAGE, head->fpdesc );
	}
	// ���ļ��ж���������ҳ����pageNo��ҳ�����뻺����
	rewind(head->fpdesc);
	fseek( head->fpdesc, head->desc.dataAddr + pageNo*SIZE_PER_PAGE, SEEK_SET  );
	fread( head->buff.data[mapNo], sizeof(char), SIZE_PER_PAGE, head->fpdesc );
	head->buff.map[mapNo].edit = P_UNEDIT;
	head->buff.map[mapNo].pageNo = pageNo;
	return 0;
}


/**
 * @brief �����㷨������ѡ��LRU, FIFO
 *
 * @param [in] head  : struct dbSysHead *
 * @return  int �滻��ҳ�ڻ��������±�
 *
 * @author cshizhe
 * @date 2015/10/20 
 **/
int scheBuff( struct dbSysHead *head )
{
	return LRU(head);
}

/**
* @brief �����㷨LRU���滻�����û�ж���д��ҳ
*
* @param [in] head  : struct dbSysHead *
* @return  int �滻��ҳ�ڻ��������±�
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
* @brief �����㷨FIFO,��̭������뻺������ҳ
*
* @param [in] head  : struct dbSysHead *
* @return  int �滻��ҳ�ڻ��������±�
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
		// �����ǰ�Ļ���ҳ��Ϊ��
		if (head->buff.map[i].pageNo < 0) {
			return i;
		}
		// ������������������滻������뻺������ҳ��
		else {
			if (head->buff.map[i].loadTime < head->buff.map[min].loadTime) {
				min = i;
			}
		}
	}
	return min;
}

/**
 * @brief �����дһ��ҳ������ҳ���ڻ������У�������滻�㷨�Ѹ�ҳ������������
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] query : long
 * @return  int 
 * @retval  ���ظ�ҳ�ڻ������е��±�
 *
 * @author cshizhe
 * @date 2015/10/20 
 **/
int reqPage( struct dbSysHead *head, long query )
{
	int i;
	int mapNo;

	// ��ѯ���������Ƿ����query��ҳ
	mapNo = queryPage( head, query );
	// ��������ڣ������õ����㷨���뻺������
	if( BUFF_NOT_HIT == mapNo ) {
		mapNo = scheBuff( head );
		replacePage( head, mapNo, query );
	}
	// �����ϵͳ���ʱ��Ƭ���ӣ�ͬʱ�޸ĸ�ҳ�Ķ���ͷ���ʱ��
	head->buff.curTimeStamp++;
	head->buff.map[mapNo].vstTime = head->buff.curTimeStamp;
	head->buff.map[mapNo].loadTime = head->buff.curTimeStamp;
	// ������ʱ��Ƭ�ﵽlong����󳤶ȣ�ȫ��ʱ�䶼��0
	if( (1<<30) == head->buff.curTimeStamp ) {
		for( i=0; i<SIZE_BUFF; i++ ) {
			head->buff.map[i].loadTime = 0;
			head->buff.map[i].vstTime = 0;
			head->buff.curTimeStamp = 0;
		}
	}
	return mapNo;
}