#include "queue.h"

int main()
{
	queue* q = queue_Construct();
	
	queue_Dump(q, "stdout", "TEST");
	queue_Destroy(q);
	return 0;
}
