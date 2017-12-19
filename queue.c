#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "queue.h"

#define QUEUE_BLOCK_SIZE 8
#define QUEUE_POISON -111
#define DEBUG

queue*			queue_Construct()
{
	queue* new_queue = (queue* )calloc(1, sizeof(queue));
	new_queue->data = (queue_t* )calloc(QUEUE_BLOCK_SIZE, sizeof(queue_t));
	new_queue->amount_of_allocated_blocks = 1;
	for (int i = 0; i < new_queue->amount_of_allocated_blocks*QUEUE_BLOCK_SIZE; i++)
		new_queue->data[i] = QUEUE_POISON;
	
	return new_queue;
}

void			queue_Destroy(queue* q)
{
	#ifdef DEBUG
	assert(q->data);
	assert(q);
	#endif
	for (int i = 0; i < q->amount_of_allocated_blocks*QUEUE_BLOCK_SIZE; i++)
		q->data[i] = QUEUE_POISON;
	if (q->data)
		free(q->data);
	free(q);
}

int				queue_Push(queue* q, queue_t value)
{
	#ifdef DEBUG
	assert(q->data);
	assert(q);
	#endif
	
	
	
	q->data[q->tail] = value;
	q->tail++;
	
	return queue_check(q);
}
queue_t			queue_Pop(queue* q)
{
	#ifdef DEBUG
	assert(q->data);
	assert(q);
	#endif
	
	queue_t tmp = q->data[q->head];
	if (tmp == QUEUE_POISON)
	{
		#ifdef DEBUG
		queue_print_error_code(QUEUE_POP_POISON);
		assert(0);
		#endif
	}
	q->data[q->head] = QUEUE_POISON;
	q->head++;
	
	queue_check(q);
	return tmp;
}

int 			queue_check(queue* q)
{
	if (!q)
	{
		#ifdef DEBUG
		queue_print_error_code(QUEUE_IS_NULL);
		queue_Dump(q, "crashed_queue.dmp", "Queue");
		assert(0);
		#endif
		return QUEUE_IS_NULL;
	}
	if (!q->data)
	{
		#ifdef DEBUG
		queue_print_error_code(QUEUE_DATA_IS_NULL);
		queue_Dump(q, "crashed_queue.dmp", "Queue");
		assert(0);
		#endif
		return QUEUE_DATA_IS_NULL;
	}
	for (int i = q->head; i < q->tail; i++)
		if (q->data[i] == QUEUE_POISON)
		{
			#ifdef DEBUG
			queue_print_error_code(QUEUE_POISON_BETWEEN_HEAD_AND_TAIL);
			queue_Dump(q, "crashed_queue.dmp", "Queue");
			assert(0);
			#endif
			return QUEUE_POISON_BETWEEN_HEAD_AND_TAIL;
		}
	
	switch (queue_size_status(q))
	{
		case QUEUE_FULL:
		{
			queue_expand(q);
			break;
		}
		case QUEUE_HAS_EMPTY_BLOCK:
		{
			queue_reduce(q);
			break;
		}
		case QUEUE_OVERFLOW:
		{
			#ifdef DEBUG
			queue_print_error_code(QUEUE_OVERFLOW);
			queue_Dump(q, "crashed_queue.dmp", "Queue");
			assert(0);
			#endif
			return QUEUE_OVERFLOW;
		}
		case QUEUE_SIZE_OK: break;
		default:
		{
			#ifdef DEBUG
			printf("\033[1;31mUnexpected queue_size_status code: %d\033[0m\n", queue_size_status(q));
			assert(0);
			#endif
		}
	}
	
	return QUEUE_OK;
}
const char* 	queue_meaning_of_error_message(int error_code)
{
	switch (error_code)
	{
		#define _RET_CODE(code) case code: return #code
		
		_RET_CODE(QUEUE_DATA_IS_NULL);
		_RET_CODE(QUEUE_IS_NULL);
		_RET_CODE(QUEUE_OK);
		_RET_CODE(QUEUE_OVERFLOW);
		_RET_CODE(QUEUE_POISON_BETWEEN_HEAD_AND_TAIL);
		_RET_CODE(QUEUE_POP_POISON);
		
		default: return "QUEUE_UNDEFINED_ERROR";
		#undef _RET_CODE
	}
}
void 			queue_print_error_code(int error_code)
{
	printf("\033[1;31m%s\033[0m\n", queue_meaning_of_error_message(error_code));
}
void 			queue_Dump(queue* q, const char* out, const char* queue_name)
{
	#ifdef DEBUG
	assert(q->data);
	assert(q);
	#endif
	
	FILE* fout = NULL;
	
	if (!strcmp(out, "stdout"))
	{
		fout = stdout;
	}
	else
	{
		fout = fopen(out, "w");
	}
	
	fprintf(fout, "DUMP: QUEUE \"%s\" [%p]\n{", queue_name, q);
	fprintf(fout, "	head:	%d\n	tail:	%d\n	blocks:	%d\n\n", q->head, q->tail, q->amount_of_allocated_blocks);
	for (int i = 0; i < q->amount_of_allocated_blocks*QUEUE_BLOCK_SIZE; i++)
	{	
		if (i % QUEUE_BLOCK_SIZE == 0)
			fprintf(fout, "	-------------------------------\n");
			
		if (i == q->tail && i == q->head)
			fprintf(fout, "	data[%d]:	%d	<- head, tail\n", i, q->data[i]);
		else if (i == q->head)
			fprintf(fout, "	data[%d]:	%d	<- head\n", i, q->data[i]);
		else if (i == q->tail)
			fprintf(fout, "	data[%d]:	%d	<- tail\n", i, q->data[i]);
		else
			fprintf(fout, "	data[%d]:	%d\n", i, q->data[i]);
	}
	fprintf(fout, "}\n\n");
	
	fclose(fout);
}

int				queue_expand(queue* q)
{
	#ifdef DEBUG
	assert(q->data);
	assert(q);
	#endif
	
	q->amount_of_allocated_blocks++;
	queue_t* new_data = (queue_t* )malloc(q->amount_of_allocated_blocks*QUEUE_BLOCK_SIZE*sizeof(queue_t));
	for (int i = 0; i <= q->tail; i++)
		new_data[i] = q->data[i];
	for (int i = q->tail + 1; i < q->amount_of_allocated_blocks*QUEUE_BLOCK_SIZE; i++)
		new_data[i] = QUEUE_POISON;

	free(q->data);
	q->data = new_data;
	
	return queue_check(q);
}
int				queue_reduce(queue* q)
{
	#ifdef DEBUG
	assert(q->data);
	assert(q);
	#endif
	
	q->amount_of_allocated_blocks = trunc(q->tail - q->head + 1) / QUEUE_BLOCK_SIZE + 1;
	queue_t* new_data = (queue_t* )malloc(q->amount_of_allocated_blocks*QUEUE_BLOCK_SIZE*sizeof(queue_t));
	for (int i = q->head; i <= q->tail; i++)
		new_data[i - q->head] = q->data[i];
		free(q->data);
	q->data = new_data;
	q->tail -= q->head;
	q->head = 0;
	
	for (int i = q->tail + 1; i < q->amount_of_allocated_blocks*QUEUE_BLOCK_SIZE; i++)
		new_data[i - q->head] = QUEUE_POISON;
	
	return queue_check(q);
}
int				queue_size_status(queue* q)
{
	#ifdef DEBUG
	assert(q->data);
	assert(q);
	#endif
	
	if (q->tail == q->amount_of_allocated_blocks*QUEUE_BLOCK_SIZE - 1)
		return QUEUE_FULL;
	if (q->head >= QUEUE_BLOCK_SIZE)
		return QUEUE_HAS_EMPTY_BLOCK;
	if (q->tail > q->amount_of_allocated_blocks*QUEUE_BLOCK_SIZE)
		return QUEUE_OVERFLOW;
	
	return QUEUE_SIZE_OK;
}
