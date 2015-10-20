
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include"dbHead.h"
#include"b_plus_tree.h"

char *index_filename = "b_plus_tree_index.dat";

int load_b_plus_index( struct dbSysHead *head,long fid );

int init_database( struct dbSysHead *head )
{
	initSys(head);
	head->fpdesc = fopen( DBMS_DAT, "rb+" );
	isAvail( head->fpdesc, "main", FOPEN);
	return 0;
}

int exit_database( struct dbSysHead *head )
{
	sysUpdate(head);
	fclose(head->fpdesc);
	free(head->bitMap);
	return 0;
}

int main()
{
	struct dbSysHead head;
	long fid1,fid2;
	

	/*
		��ʼ����Ȼ���ӡ����ǰϵͳ����Ϣ
	*/
	init_database( &head );
	showDesc(&head);

	/*
		����һ���ļ���Ȼ�������д���ļ�
		������Ĭ�������Ѿ����ã��������ļ� b_plus_tree_index.dat �У�����ֱ�Ӷ��뵽DBMS��
		ʵ����Ӧ��ͨ������B+���ķ�ʽ�����������浽DBMS��
	*/
	fid1 = creatFileSpace( &head );
	load_b_plus_index( &head, fid1 );
	
	printf("search result: %d\n",search( &head, fid1, -1));
	printf("search result: %d\n",search( &head, fid1, 0));
	printf("search result: %d\n",search( &head, fid1, 5000));
	printf("search result: %d\n",search( &head, fid1, 99999));
	printf("search result: %d\n",search( &head, fid1, 100000));

	showDesc(&head);
	showFileDesc(&head);
	exit_database( &head );
	system("pause");
	return 0;
}

int load_b_plus_index( struct dbSysHead *head,long fid )
{
	int pos;
	int n;
	char str[1024+1];
	FILE *fp;
	
	str[1024] = '\0';
	fp = fopen(index_filename,"rb");
	isAvail( fp, "load_b_plus_index", FOPEN);
	pos = 0;
	while( (n = fread( str, sizeof(char), 1024, fp )) != 0 ) {
		wtFile( head, fid, pos, n, str );
		pos += n;
	}
	printf("pos:%d\n",pos);
	fclose(fp);
	sysUpdate(head);
	return pos;
}