
// 数据库最多容纳量192M
#define SIZE_DATA_SPACE (192*1024*1024)
// 每页大小4K
#define SIZE_PER_PAGE (4*1024)
// BitMap为二进制串，每一位代表对应页是否为空
#define SIZE_BIT_MAP  (SIZE_DATA_SPACE/(SIZE_PER_PAGE*8))

// 整个数据库存在一个文件DBMS_DAT当中，依次顺序为 ??为暂时还没有用到的
// (table description??, bitmap, data (pages), segment??)
#define TABLE_DESC_ADDR 0
// BitMap在存储文件中的地址
#define BITMAP_ADDR 1024 
// 数据段在存储文件中的地址
#define DATA_ADDR	(BITMAP_ADDR+SIZE_BIT_MAP)
// 其他文件中似乎都没有出现，有用吗？
#define SEGMENT_ADDR (BITMAP_ADDR+SIZE_BIT_MAP+SIZE_DATA_SPACE)

// 每个段的固定页面数
#define PAGE_PER_SEGMENT 28
// 最大存储的表个数
#define MAX_FILE_NUM 64
// 第一个表的编号，从1开始
#define FIRST_FID	1

// 缓冲区所含页面个数
#define SIZE_BUFF 32
// Buffer中不存在查询页面时的返回值
#define BUFF_NOT_HIT -1
// 对页面操作的描述
#define P_AVAI		1
#define P_UNAVAI	0
#define P_EDIT		1
#define P_UNEDIT	0
#define ALLO_FAIL	-1

struct Segment 
{
	long fid;					//	该Segment属于哪个文件，即每段只属于一个表（存在较大空间浪费）
	long count;					//	该Segment中记录了多少个页
	long nextAddr;				//	下一个Segment的指针
	long preAddr;				//	前一个Segment的指针
	long pageNo[PAGE_PER_SEGMENT];	//	记录每一个页的页号
};

struct FileDesc
{
	long fileID;				//	文件的标识
	long fileAddr;				//	文件的Segment地址值（Segment用链表串连）
	long filePageNum;			//	文件占用多少页
};

struct SysDesc
{
	long sizeOfFile;			//	存储文件中，数据区的大小，默认设为196M
	long sizePerPage;           //	每一个页的大小
	long totalPage;				//	总共的页数
	long pageAvai;				//	当前有多少可用的页

	long bitMapAddr;			//	bitMap在存储文件中的地址
	long sizeBitMap;			//	bitMap的大小,以字节为单位

	long dataAddr;				//	存储文件中数据区的大小
	long segmentNum;			//	Segment的数量
	long segmentAddr;			//	Segment在存储文件的地址
	long segmentCur;			//	目前使用过的Segment数量

	long curfid;				//	目前可以分配的fid号
	long curFileNum;			//	目前有多少个文件，最多为 MAX_FILE_NUM
	struct FileDesc fileDesc[MAX_FILE_NUM];			//	对每一个文件进行描述
};

struct buffMap
{
    long pageNo;					//	该缓冲区块中存储的数据文件的页号
	long loadTime;					//	读入缓冲区的时间
	long vstTime;					//	访问该缓冲区块的时间
	int edit;						//	该缓冲区块中的数据是否被修改
};

struct buffSpace
{
	 /*
		应该动态分配缓冲区数据的空间
		因为这样静态分配能分配到的空间太小
	*/
	char data[SIZE_BUFF][SIZE_PER_PAGE];	//	缓冲区数据块，目前设置SIZE_BUFF块，每一块的大小为页的大小	
	struct buffMap map[SIZE_BUFF];			//	记录每一个缓冲区块的信息
	long curTimeStamp;						//	目前的相对时间戳
};