
#include"b_plus_tree.h"
#include"dbHead.h"
#include<stdio.h>

int searchRecord(Node &node, int key)
{
	int i;
	for(i = 0; i < node.count && key > node.pair[i].key; i ++);
	return i;
}

void searchNode( struct dbSysHead *head, long fid, Node &node, int key )
{
	int pos, offset;
	if(node.type == LEAF)
		return;
	else{
		pos = searchRecord(node, key);
		pos = pos == node.count ? pos - 1 : pos;
		offset = node.pair[pos].pos;
		rdFile( head, fid, offset, sizeof(node), &node );
		searchNode( head, fid, node, key );
	}
}
int search( struct dbSysHead *head, long fid, int key)
{
	Node node;
	int pos;
	int offset;

	/*
		���ͷ�ڵ㣬������������Ѿ���������ȷ
		����ֻ��һ��ʾ����ʵ���ϲ�������д
	*/
	rdFile( head, fid, 0, sizeof(int), &offset);
	rdFile( head, fid, offset, sizeof(Node), &node );

	searchNode( head, fid, node, key );
	pos = searchRecord(node, key);
	if(pos == node.count || node.pair[pos].key != key)
		return -1;
	else
		return node.pair[pos].pos;
}