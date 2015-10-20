
// ���ݿ����������192M
#define SIZE_DATA_SPACE (192*1024*1024)
// ÿҳ��С4K
#define SIZE_PER_PAGE (4*1024)
// BitMapΪ�����ƴ���ÿһλ�����Ӧҳ�Ƿ�Ϊ��
#define SIZE_BIT_MAP  (SIZE_DATA_SPACE/(SIZE_PER_PAGE*8))

// �������ݿ����һ���ļ�DBMS_DAT���У�����˳��Ϊ ??Ϊ��ʱ��û���õ���
// (table description??, bitmap, data (pages), segment??)
#define TABLE_DESC_ADDR 0
// BitMap�ڴ洢�ļ��еĵ�ַ
#define BITMAP_ADDR 1024 
// ���ݶ��ڴ洢�ļ��еĵ�ַ
#define DATA_ADDR	(BITMAP_ADDR+SIZE_BIT_MAP)
// �����ļ����ƺ���û�г��֣�������
#define SEGMENT_ADDR (BITMAP_ADDR+SIZE_BIT_MAP+SIZE_DATA_SPACE)

// ÿ���εĹ̶�ҳ����
#define PAGE_PER_SEGMENT 28
// ���洢�ı����
#define MAX_FILE_NUM 64
// ��һ����ı�ţ���1��ʼ
#define FIRST_FID	1

// ����������ҳ�����
#define SIZE_BUFF 32
// Buffer�в����ڲ�ѯҳ��ʱ�ķ���ֵ
#define BUFF_NOT_HIT -1
// ��ҳ�����������
#define P_AVAI		1
#define P_UNAVAI	0
#define P_EDIT		1
#define P_UNEDIT	0
#define ALLO_FAIL	-1

struct Segment 
{
	long fid;					//	��Segment�����ĸ��ļ�����ÿ��ֻ����һ�������ڽϴ�ռ��˷ѣ�
	long count;					//	��Segment�м�¼�˶��ٸ�ҳ
	long nextAddr;				//	��һ��Segment��ָ��
	long preAddr;				//	ǰһ��Segment��ָ��
	long pageNo[PAGE_PER_SEGMENT];	//	��¼ÿһ��ҳ��ҳ��
};

struct FileDesc
{
	long fileID;				//	�ļ��ı�ʶ
	long fileAddr;				//	�ļ���Segment��ֵַ��Segment����������
	long filePageNum;			//	�ļ�ռ�ö���ҳ
};

struct SysDesc
{
	long sizeOfFile;			//	�洢�ļ��У��������Ĵ�С��Ĭ����Ϊ196M
	long sizePerPage;           //	ÿһ��ҳ�Ĵ�С
	long totalPage;				//	�ܹ���ҳ��
	long pageAvai;				//	��ǰ�ж��ٿ��õ�ҳ

	long bitMapAddr;			//	bitMap�ڴ洢�ļ��еĵ�ַ
	long sizeBitMap;			//	bitMap�Ĵ�С,���ֽ�Ϊ��λ

	long dataAddr;				//	�洢�ļ����������Ĵ�С
	long segmentNum;			//	Segment������
	long segmentAddr;			//	Segment�ڴ洢�ļ��ĵ�ַ
	long segmentCur;			//	Ŀǰʹ�ù���Segment����

	long curfid;				//	Ŀǰ���Է����fid��
	long curFileNum;			//	Ŀǰ�ж��ٸ��ļ������Ϊ MAX_FILE_NUM
	struct FileDesc fileDesc[MAX_FILE_NUM];			//	��ÿһ���ļ���������
};

struct buffMap
{
    long pageNo;					//	�û��������д洢�������ļ���ҳ��
	long loadTime;					//	���뻺������ʱ��
	long vstTime;					//	���ʸû��������ʱ��
	int edit;						//	�û��������е������Ƿ��޸�
};

struct buffSpace
{
	 /*
		Ӧ�ö�̬���仺�������ݵĿռ�
		��Ϊ������̬�����ܷ��䵽�Ŀռ�̫С
	*/
	char data[SIZE_BUFF][SIZE_PER_PAGE];	//	���������ݿ飬Ŀǰ����SIZE_BUFF�飬ÿһ��Ĵ�СΪҳ�Ĵ�С	
	struct buffMap map[SIZE_BUFF];			//	��¼ÿһ�������������Ϣ
	long curTimeStamp;						//	Ŀǰ�����ʱ���
};