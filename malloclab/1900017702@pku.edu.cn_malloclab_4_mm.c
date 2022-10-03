//textbook实现的是一个带边界标记的隐式链表
//init产生一个0，8/1，8/1，0/1的4字部分
//序言块没有内容，大小为8，尾声块只有一个0/1的header作为指示
//这个代码是基于textbook改动的，最后才移植到mm.c上
//所以可能第一次提交没有评分？


//这个版本实现的是基于相对偏移量的显式不分离链表
//策略是best fit
//序言块同时作为表头和表尾
//（严格来说应该基于绝对偏移量，但it works）
//堆检查函数的注释在约300行处

//重要函数InsertNode和DeleteNode（简单的链表操作）
//init：多占用两个字作为指针域（6字），prev和next
//malloc：fit就place，否则extend再place
//free：释放，合并，插入
//realloc：不变
//calloc：基础版本

//每次新得到一个空闲块，LIFO插入链表，每次分配掉一个空闲块，删除对应结点
//每次产生一个空闲块，要么是place的，要么需要合并
//只有place和coalesce真正产生新的空闲块

//extend：申请一个足够空间并coalesce
//place：先delete，两侧肯定是已分配空间，如果足够一个最小块还要分割并insert
//coalesce：只留下一对最前指针域，余下结点删除
//findfit：使用最佳适配


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)


//要么用next fit，要么用first fit
#define NEXT_FITx

//字长4字节；双字为8字节；片大小为4K
/* Basic constants and macros */
    #define WSIZE       4       /* Word and header/footer size (bytes) */ 
    #define DSIZE       8       /* Double word size (bytes) */
    #define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */  
//取MAX运算
    #define MAX(x, y) ((x) > (y)? (x) : (y))  
//把size和alloc信息合并到一个字里
    #define PACK(size, alloc)  ((size) | (alloc)) 
//封装的在地址p处的读写操作（不能自己读和写）
    #define GET(p)       (*(int *)(p))            
    #define PUT(p, val)  (*(int *)(p) = (val))    
//根据指针p返回它包含的size（包括H和F的各4字节）和allocated信息
    #define GET_SIZE(p)  (GET(p) & ~0x7)
    #define GET_ALLOC(p) (GET(p) & 0x1)
//根据块指针bp返回头部、脚部、上一块、下一块
//使用char（1字节）防止伸缩
    #define HDRP(bp)       ((char *)(bp) - WSIZE)                      
    #define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) 
    #define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) 
    #define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

//自定义宏：取得next/prev值;沿链表找next/prev;修改next/prev用PUT
    #define PREV(bp)    ((void*)(bp))
    #define NEXT(bp)    ((void*)(bp) + WSIZE)
    #define GETPREV(bp)    (GET(PREV(bp)))
    #define GETNEXT(bp)    (GET(NEXT(bp)))
    #define LINKPREV(bp)    (((void*)(bp) - GETPREV(bp)))    //注意是减去偏移量
    #define LINKNEXT(bp)    (((void*)(bp) + GETNEXT(bp)))
    #define PUTPREV(bp,val) (PUT((void*)(bp), val))
    #define PUTNEXT(bp,val) (PUT(((void*)(bp) + WSIZE), val))

/* Global variables */
//堆指针
    static char *heap_listp = 0;  /* Pointer to first block */
//next fit需要用到当前位置“指针”
    #ifdef NEXT_FIT
    static char *rover;           /* Next fit rover */
    #endif

//静态函数声明
/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);//拓展堆
static void place(void *bp, size_t asize);//在一个空闲块中分配所需位置
static void *find_fit(int asize);//寻找适配空闲块
static void *coalesce(void *bp);//合并
static void InsertNode(void* bp);//插入节点
static void DeleteNode(void* bp);//删除节点
static void PrintBlock(char* bp);//打印块
static int CheckBlock(char* bp);//删除块


//空闲链表部分的设计：
//以4字节的偏移量为信息，每个空闲块都有prev和next两个
//链的开头是序言块，序言块的prev是0，next是真正意义的链头
//链的终点是以0为next的空闲块


static void InsertNode(void* bp)//新产生了一个空闲块，插入到序言和栈顶之间
{
    //hp-(insert)-firstbp
    //hp-ptr-firstbp
    void* hp = heap_listp;
    int val1 = bp - hp;
    if (GETNEXT(hp) != 0)//非空表
    {
        void* firstbp = LINKNEXT(hp);//找到链式栈的栈顶元素位置
        unsigned int val2 = firstbp - bp;
        PUTPREV(firstbp, val2);
        PUTNEXT(bp, val2);
        PUTPREV(bp, val1);
        PUTNEXT(hp, val1);
    }
    else//空表
    {
        PUTNEXT(bp, 0);
        PUTPREV(bp, val1);
        PUTNEXT(hp, val1);
    }
    
}
//注意不能对序言作用（序言是allocated的，不会发生）
static void DeleteNode(void* bp)//一个空闲块被分配时，才会delete
{
    void* pbp = LINKPREV(bp);
    if (GETNEXT(bp) == 0)//表尾
    {
        PUTNEXT(pbp, 0);
        return;
    }
    void* nbp = LINKNEXT(bp);
    unsigned int val = nbp - pbp;
    PUTPREV(nbp, val);
    PUTNEXT(pbp, val);
}




int mm_init(void)//初始化
{
    //创建初始化空堆，即一个序言块
    if ((heap_listp = mem_sbrk(6*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);//必要的对齐浪费
    PUT(heap_listp + (1*WSIZE), PACK(16, 1)); //序言头部 
    PUT(heap_listp + (2*WSIZE), 0); //序言prev 
    PUT(heap_listp + (3*WSIZE), 0); //序言next
    PUT(heap_listp + (4*WSIZE), PACK(16, 1)); //序言脚部
    PUT(heap_listp + (5*WSIZE), PACK(0, 1));//尾声块只需要一个头
    //注意：先检查这一块的大小，再访问prev和next，以识别尾声
    heap_listp += (2*WSIZE);                     

#ifdef NEXT_FIT
    rover = heap_listp;
#endif

    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)//预载一块4KB的
        return -1;
    return 0;
}


void *malloc(size_t size)//malloc（具有最小值）
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;      

    //处理边界情况
    if (heap_listp == 0)//若未初始化，则初始化
        mm_init();
    if (size == 0)//无需分配空间
        return NULL;

    //把size调节成块大小asize（有最小值且向8字节对齐）
    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)          //最少2个双字（4个字）
        asize = 2*DSIZE;
    else                        //向双字对齐；向上取整；多用1个双字的空间
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); 

    //在链表中搜索空闲块
    if ((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }

    //没有可用块，申请新的空间（堆拓展）并做成块
    //注意如果多申请了要把余下部分放入链表
    extendsize = MAX(asize,CHUNKSIZE);//一次最少4KB
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;                                  
    place(bp, asize);                                 
    return bp;
} 


//free目标地点，合并部分会完成链表的维护
void free(void *bp)
{
    if (bp == 0)//空指针，free nothing
        return;

    size_t size = GET_SIZE(HDRP(bp));
//为什么可以free一个没有初始化过的堆？
//为什么可以free一个没有初始化过的堆？
/*
    if (heap_listp == 0)//块指针为空（未初始化），则重新初始化
    {
        mm_init();
    }
*/
    //头部尾部重设
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);//合并
}

//释放并重新分配，同时数据不变，无需修改
void *realloc(void *ptr, size_t size)
{
    size_t oldsize;
    void *newptr;

    //无需分配，只做free
    if(size == 0) {
        mm_free(ptr);
        return 0;
    }
    //无需释放，只作malloc
    if(ptr == NULL)
        return mm_malloc(size);

    newptr = mm_malloc(size);//直接分配一个新的空间
//失败不会free
    if(!newptr) {
        return 0;
    }

    //拷贝旧数据
    oldsize = GET_SIZE(HDRP(ptr));
    if(size < oldsize) oldsize = size;
    memcpy(newptr, ptr, oldsize);
    mm_free(ptr);//最后再free，保证不会用到同一个块
    return newptr;
}


void *calloc (size_t nmemb, size_t size)
{
    size_t bytes = nmemb * size;
    void *newptr;
    newptr = malloc(bytes);
    memset(newptr, 0, bytes);

    return newptr;
}




//堆检查函数（可用调用这个函数以识别调用的行号）
    /*
        检查堆：
        – 检查序言块和尾声块(OK)
        – 检查堆边界(提供的函数报错？)
        – 检查每个块的地址对齐(OK)
        – 检查每个块的头部、脚部（对齐和最小值）, 前后指针的分配/释放一致性、头部脚部一致性(OK)
        – 检查合并：没有两个连续的空闲块(OK)
        检查空闲链表:
        – 前后指针对应(OK)
        – 所有空闲指针在堆顶和堆底之间(提供的函数报错？)
        – 顺序遍历和按链表遍历验证空闲块数目(OK)
        – 分离链表的每个桶包含的块都属于它的给定范围(NO NEED)
    */
void mm_checkheap(int lineno)
{
    printf("/////////CHECK HEAP///////////\n");
    int cntheap = 0, cntlist = 0;
    if (lineno)
        printf("Heap:%p\n", heap_listp);
    else
        return;
    if (GET(HDRP(heap_listp)) != GET(FTRP(heap_listp))
	|| GET_ALLOC(HDRP(heap_listp)) == 0)
        printf("\nBad prologue header\n");

    //顺序检查
    for (void* bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
	{
		CheckBlock(bp);
		PrintBlock(bp);

        if (GET_ALLOC(HDRP(bp)) == 0)
            if (GET_ALLOC(HDRP(NEXT_BLKP(bp))) == 0)
                printf("2 consecutive free block!\n");
	//不知道怎么用这两个函数
        //if (bp < mem_heap_lo() || bp > mem_heap_high())
        //    printf("Block beyond boundaries!\n"); 

        if (GET_ALLOC(HDRP(bp)) == 0)
            cntheap++;
	}

    //沿链表检查
	printf("//////////CHECK LIST//////////\n");
    for (void* bp = heap_listp; ; bp = LINKNEXT(bp))
	{
		CheckBlock(bp);
		PrintBlock(bp);
	//不知道怎么用这两个函数
        //if (bp < mem_heap_lo() || bp > mem_heap_high())
        //    printf("Block beyond boundaries!\n"); 
        cntlist++;
		if (GETNEXT(bp) == 0)
			break;
        void* np = LINKNEXT(bp);
        if (GETPREV(np) != GETNEXT(bp))
            printf
            ("Block:[%p]'s NEXT is [%d] & Block:[%p]'s PREV is [%d]",
            bp, GETNEXT(bp), np, GETPREV(np));
	}

    if (cntheap != cntlist)
        printf("Free block numbers isn't correct\n");

    printf("/////////FINISH CHECK///////////\n\n\n");
    return;
}


static void PrintBlock(char* bp)//打印块的头尾
{
    long int hsize = GET_SIZE(HDRP(bp)), fsize = GET_SIZE(FTRP(bp));
    int halloc = GET_ALLOC(HDRP(bp)), falloc = GET_ALLOC(FTRP(bp));
	int prev = GETPREV(bp), next = GETNEXT(bp);
    printf
    ("%p:header:[%ld:%d] prev:[%d]&next:[%d] footer[%ld:%d]\n",
    bp, hsize, halloc, prev, next, fsize, falloc);
    return;
}
static int CheckBlock(char* bp)//检查单个块
{
    if (((long long)bp % 8))//起始地址对齐
        printf("\nBlock isn't aligned:%p\n\n", bp);
    if (GET_SIZE(HDRP(bp)) % 8)//大小对齐
        printf("\nSize isn't aligned:%p\n\n", bp);
    if (GET_SIZE(HDRP(bp)) < 16)//大小对齐
        printf("\nSize isn't enough:%p\n\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))//头部和脚部匹配
        printf("\nHeader&Footer don't match:%p\n\n", bp);
    return (GET_ALLOC(HDRP(bp)));
}


//拓展堆：没有符合的空闲块时，请求一个新的空间，返回块指针
static void *extend_heap(size_t words) 
{
    char *bp;
    size_t size;

//size应该是一个偶数（因为数据区要按8字节对齐），向上取整
//size包含Header和Footer
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; 
    if ((long)(bp = mem_sbrk(size)) == -1)  
        return NULL;                                        

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */   
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */   
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); //尾声的头部
    return coalesce(bp);//向前合并
}

//尝试合并，并返回合并后块的指针，同时维护链表
//总是假定bp位置是一个新的结点，已经设置好了头部和脚部，但未设置next和prev
static void *coalesce(void *bp) 
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc)//前后都已经分配，无需合并
    {
        InsertNode(bp);
        return bp;
    }
//否则，链表中只保留最前的next和prev
//余下（后段）的在链表中删除
    else if (prev_alloc && !next_alloc)//前分后未分，向后合并
    {
        //删除-插入可以简化成修改
        DeleteNode(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size,0));
        InsertNode(bp);
    }

    else if (!prev_alloc && next_alloc)//后分前未分，向前合并，无需更新链表
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    else//前后都空闲，同时合并
    {
        DeleteNode(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
            GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
#ifdef NEXT_FIT
    /* Make sure the rover isn't pointing into the free block */
    /* that we just coalesced */
    if ((rover > (char *)bp) && (rover < NEXT_BLKP(bp))) 
        rover = bp;
#endif
    return bp;
}

//在空闲块中分配：要么分割(产生空闲块加入链表)，要么整块分配
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));   
    DeleteNode(bp);
    if ((csize - asize) >= (2*DSIZE))//可用放下一个最小块，作分割
    { 
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
        InsertNode(bp);
    }
    else//剩余块不足一个最小块，全分配
    {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
	//mm_checkheap(1);
}

//找到一个足够放下asize的块，使用的是best fit
static void *find_fit(int asize)
{
#ifdef NEXT_FIT 
    /* Next fit search */
    char *oldrover = rover;

    /* Search from the rover to the end of list */
    for ( ; GET_SIZE(HDRP(rover)) > 0; rover = LINKNEXT(rover))
	{
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;
	if (GETNEXT(rover) == 0)
		break;
	}
    /* search from start of list to old rover */
    for (rover = heap_listp; rover != oldrover; rover = LINKNEXT(rover))
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;

    return NULL;  /* no fit found */
#else 
    /* Best-fit search */
    void *bp = heap_listp;
	int mini = 9999999;
	void* minbp = bp;
	while(GETNEXT(bp) != 0)
	{
        if (!GET_ALLOC(HDRP(bp)) &&
	(asize <= GET_SIZE(HDRP(bp))) &&
	(GET_SIZE(HDRP(bp)) - asize < 16))
			return bp;
        if (!GET_ALLOC(HDRP(bp)) &&
	(asize <= GET_SIZE(HDRP(bp))) &&
	GET_SIZE(HDRP(bp)) - asize < mini)
        {
            mini = GET_SIZE(HDRP(bp)) - asize;
            minbp = bp;
        }
		bp = LINKNEXT(bp);
	}
    if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp))))
		return bp;
	if (minbp != heap_listp)
		return minbp;


    return NULL; /* No fit */
#endif
}
