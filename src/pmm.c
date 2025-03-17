#include <common.h>

#define max(a, b) (a < b ? b : a)
#define min(a, b) (a < b ? a : b)

typedef struct {
	uintptr_t addr;
	size_t size;
} mem_sec_t;

static struct mem_block
{
	mem_sec_t req_mem;  // Memories of user required
	uintptr_t item_addr;  // Memories of this linked list item
	struct mem_block *prev;
	struct mem_block *next;
} *hp_head = NULL;

static struct mem_block *hp_cur = NULL;

static spinlock_t lk = 0;

static inline uintptr_t check_mem(size_t size) {
	uintptr_t heap_start = (uintptr_t) heap.start;
	uintptr_t heap_end = (uintptr_t) heap.end;

	// Iterate all possible aligned addresses that the required memories can use
	for (uintptr_t cur_addr = size, m = 2; cur_addr + size < heap_end; cur_addr = size * m, m++)
	{
		// The user required memories must be in heap
		if (cur_addr < heap_start) continue;

		bool is_overlapped = false;

		if (hp_head != NULL)
		{
			// If hp_head is not NULL, there are some memories have been assigned.

			// Iterate the linked list to check if required memories are occuiped
			for (struct mem_block *cur_itm = hp_head; cur_itm != NULL; cur_itm = cur_itm->next)
			{
				// Check the linked list memories first
				if (max(cur_addr, cur_itm->item_addr) <= min(cur_addr + size, cur_itm->item_addr + sizeof(struct mem_block)))
				{
					// Overlapped
					is_overlapped = true;
					break;
				}

				// Check the assigned memories then
				if (cur_itm->req_mem.addr != 0) 
				{
					if (max(cur_addr, cur_itm->req_mem.addr) <= min(cur_addr + size, cur_itm->req_mem.addr + cur_itm->req_mem.size))
					{
						// Overlapped
						is_overlapped = true;
						break;
					}
				}
			}
		}

		if (!is_overlapped)
		{
			return cur_addr;
		}
	}

	return 0;
}

static void *kalloc(size_t size)
{
	spin_lock(&lk);

	// Assign memories for new linked list item
	struct mem_block *new_itm = (struct mem_block *) check_mem(sizeof(struct mem_block));

	if (new_itm == NULL)
	{
		// There's even no free space for linked list item
		spin_unlock(&lk);
		return NULL;
	}	

	if (hp_cur == NULL)
	{
		// hp_cur should not be NULL unless current item is hp_head and hp_head is NULL
		hp_head = new_itm;
		hp_cur = hp_head;
		hp_cur->prev = NULL;
	}
	else
	{
		hp_cur->next = new_itm;
		new_itm->prev = hp_cur;
		hp_cur = new_itm;
	}

	hp_cur->item_addr = (uintptr_t) new_itm;
	hp_cur->req_mem.addr = 0; // We will find address for user required memories soon
	hp_cur->next = NULL;

	void *req_mem = (void *) check_mem(size);

	if (req_mem == NULL)
	{
		// Cannot allocate memories for user requirement.
		// Delete the new linked list item
		if (hp_cur == hp_head)
		{
			hp_head = NULL;
			hp_cur = NULL;
		}
		else
		{
			hp_cur = hp_cur->prev;
			hp_cur->next = NULL;
		}

		spin_unlock(&lk);
		return NULL;
	}

	hp_cur->req_mem.addr = (uintptr_t) req_mem;
	hp_cur->req_mem.size = size;

	printf("Alloc [%p, %p] by CPU #%d\n", req_mem, req_mem + size, cpu_current());
	
	spin_unlock(&lk);
	return req_mem;
}

static void kfree(void *ptr)
{
	while (1)
	{
		spin_lock(&lk);

		for (struct mem_block *cur_itm = hp_head; cur_itm != NULL; cur_itm = cur_itm->next)
		{
			if (cur_itm->req_mem.addr == (uintptr_t)ptr)
			{
				// Delete this item

				if (hp_cur == cur_itm)
				{
					if (hp_head == hp_cur)
					{
						hp_head = NULL;
						hp_cur = NULL;
					}
					else
					{
						hp_cur = hp_cur->prev;
						hp_cur->next = NULL;
					}
				}
				else
				{
					cur_itm->prev->next = cur_itm->next;
					cur_itm->next->prev = cur_itm->prev;
				}
				
				printf("Free [%p, %p] by CPU #%d\n", ptr, ptr + cur_itm->req_mem.size, cpu_current());
				spin_unlock(&lk);
				return;
			}
		}

		spin_unlock(&lk);
	}
}

static void pmm_init()
{
	uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
	printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);

	hp_head = NULL;
	hp_cur = NULL;
}

MODULE_DEF(pmm) = {
	.init = pmm_init,
	.alloc = kalloc,
	.free = kfree,
};