typedef int queue_t;
typedef struct queue
{
	int head;
	int tail;
	int amount_of_allocated_blocks;
	queue_t* data;
} queue;
typedef enum queue_error_codes
{
	QUEUE_OK,
	QUEUE_IS_NULL,
	QUEUE_DATA_IS_NULL,
	QUEUE_POISON_BETWEEN_HEAD_AND_TAIL,
	QUEUE_OVERFLOW,
	QUEUE_POP_POISON,
	
	QUEUE_FULL,
	QUEUE_HAS_EMPTY_BLOCK,
	QUEUE_SIZE_OK,
	
} queue_error_code;

queue*			queue_Construct();
void			queue_Destroy(queue* q);

int				queue_Push(queue* q, queue_t value);
queue_t			queue_Pop(queue* q);

int 			queue_check(queue* q);
const char* 	queue_meaning_of_error_message(int error_code);
void 			queue_print_error_code(int error_code);
void 			queue_Dump(queue* q, const char* out, const char* queue_name);

int				queue_expand(queue* q);
int				queue_reduce(queue* q);
int				queue_size_status(queue* q);
