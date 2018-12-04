/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.

** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.

** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*
 * dll_garbage.c --- memory garbage collection for the dll
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>

#ifdef HAVE_STRING_H
#	include <string.h>
#endif /* HAVE_STRING_H */

#include "dll_garbage.h"

#undef malloc
#undef free
#undef calloc
#undef realloc

#if qMFC || qCvsDebug
	/* to intercept static C++ allocations */
#	define AUTO_INITALIZE 1
#else /* !macintosh */
#	define AUTO_INITALIZE 0
#endif /* !macintosh */

#if qCvsDebug && defined(macintosh)
#	define qRangeCheck 0
#else /* !qCvsDebug */
#	define qRangeCheck 0
#endif /* !qCvsDebug */

#if qRangeCheck && defined(macintosh)
#include <MacMemory.h>
static void *	platform_calloc(size_t nmemb, size_t size)
{
	size_t tot = nmemb * size;
	void *res = NewPtr(tot);
	if(res != 0L)
		memset(res, 0, tot);
	return res;
}

static void *	platform_malloc(size_t size)
{
	return NewPtr(size);
}

static void *	platform_realloc(void * ptr, size_t size)
{
	void *newptr = NewPtr(size);
	if(newptr != 0L && ptr != 0L)
	{
		Size oldS = GetPtrSize((Ptr)ptr);
		if(oldS != 0)
		{
			size_t copySize = size < oldS ? size : oldS;
			memcpy(newptr, ptr, copySize);
		}
		DisposePtr((Ptr)ptr);
	}
	return newptr;
}

static void platform_free(void *ptr)
{
	if(ptr != 0L)
		DisposePtr((Ptr)ptr);
}
#else
#	define platform_calloc(s1, s2) calloc(s1, s2)
#	define platform_malloc(s) malloc(s)
#	define platform_realloc(p, s) realloc(p, s)
#	define platform_free(p) free(p)
#endif

#if qRangeCheck
#	include <assert.h>
#	ifdef macintosh
#		define ASSERT_RANGE(cond) do {if(!(cond)) DebugStr("\pRange check failed !");} while(0)
#	else /* !macintosh */
#		define ASSERT_RANGE(cond) assert(cond)
#	endif /* !macintosh */
	static void garbyRangeCheck(void);

#	define RANGE_MAGIC_SIZE (3 * sizeof(size_t))
#	define RANGE_TO_PTR(ptr) ((void *)(((size_t *)(ptr))+2))
#	define PTR_TO_RANGE(ptr) ((void *)(((size_t *)(ptr))-2))
#	define SIZE_FROM_PTR(ptr) (((size_t *)(ptr))[-2])
#	define MAGIC1_FROM_PTR(ptr) ((void *)&(((size_t *)(ptr))[-1]))
#	define MAGIC2_FROM_PTR(ptr) ((void *)&(((char *)(ptr))[SIZE_FROM_PTR(ptr)]))
#	define RANGE_MAGIC ((char)0xFC)
	static void FILL_MAGIC(void *ptr)
	{
		char *tmp = (char *)ptr;
		*tmp++ = RANGE_MAGIC;
		*tmp++ = RANGE_MAGIC;
		*tmp++ = RANGE_MAGIC;
		*tmp = RANGE_MAGIC;
	}
	
	static void CHECK_MAGIC(void *ptr)
	{
		char *tmp = (char *)ptr;
		ASSERT_RANGE(*tmp++ == RANGE_MAGIC);
		ASSERT_RANGE(*tmp++ == RANGE_MAGIC);
		ASSERT_RANGE(*tmp++ == RANGE_MAGIC);
		ASSERT_RANGE(*tmp == RANGE_MAGIC);
	}
	
	static void *malloc_check(size_t size)
	{
		void *result, *range;

		garbyRangeCheck();
		
		if(size == 0)
			return 0L;
		
		range = platform_malloc(size + RANGE_MAGIC_SIZE);
		if(range == 0L)
			return 0L;
		
		result = RANGE_TO_PTR(range);
		SIZE_FROM_PTR(result) = size;
		FILL_MAGIC(MAGIC1_FROM_PTR(result));
		FILL_MAGIC(MAGIC2_FROM_PTR(result));
		
		return result;
	}
	
	static void *calloc_check(size_t nmemb, size_t size)
	{
		void *result;
		size_t totsize = nmemb * size;

		garbyRangeCheck();
		
		if(totsize == 0)
			return 0L;
		
		result = malloc_check(totsize);
		if(result == 0L)
			return 0L;
		
		memset(result, 0, totsize);
		return result;
	}
	
	static void free_check(void *ptr)
	{
		void *range;

		garbyRangeCheck();
		
		if(ptr == 0L)
			return;

		ASSERT_RANGE(SIZE_FROM_PTR(ptr) > 0);
		CHECK_MAGIC(MAGIC1_FROM_PTR(ptr));
		CHECK_MAGIC(MAGIC2_FROM_PTR(ptr));

		range = PTR_TO_RANGE(ptr);
		memset(range, 0xFC, SIZE_FROM_PTR(ptr) + RANGE_MAGIC_SIZE);
		platform_free(range);
	}
	
	static void *realloc_check(void *ptr, size_t size)
	{
		void *result, *range;

		garbyRangeCheck();
		
		if(ptr == 0L)
			return malloc_check(size);
		if(size == 0)
		{
			free_check(ptr);
			return 0L;
		}
		
		ASSERT_RANGE(SIZE_FROM_PTR(ptr) > 0);
		CHECK_MAGIC(MAGIC1_FROM_PTR(ptr));
		CHECK_MAGIC(MAGIC2_FROM_PTR(ptr));
		
		range = PTR_TO_RANGE(ptr);
		range = platform_realloc(range, size + RANGE_MAGIC_SIZE);
		if(range == 0L)
			return 0L;
		
		result = RANGE_TO_PTR(range);
		SIZE_FROM_PTR(result) = size;
		FILL_MAGIC(MAGIC1_FROM_PTR(result));
		FILL_MAGIC(MAGIC2_FROM_PTR(result));
		
		return result;		
	}
#	define platform_malloc malloc_check
#	define platform_free free_check
#	define platform_calloc calloc_check
#	define platform_realloc realloc_check
#endif /* qRangeCheck */

#if qRangeCheck && defined(macintosh)
#ifdef __cplusplus
extern "C"
#endif
void *	calloc(size_t nmemb, size_t   s)
{
	return shl_calloc(nmemb, s, 0, 0L);
}

#ifdef __cplusplus
extern "C"
#endif
void	free(void *  p)
{
	shl_free(p);
}

#ifdef __cplusplus
extern "C"
#endif
void *	malloc(size_t   s)
{
	return shl_malloc(s, 0, 0L);
}

#ifdef __cplusplus
extern "C"
#endif
void *	realloc(void *   p, size_t   s)
{
	return shl_realloc(p, s, 0, 0L);
}

#endif

#define GARBY_DEF_ALLOC 20
#define GARBY_DEF_POOL 20
#define MODULE_SIZE 8

typedef struct _frameRec
{
	void *entry;
#if qCvsDebug
	char module[MODULE_SIZE];
	short line;
#endif
	struct _frameRec *next;
} frameRec;

typedef struct DYN_LIST
{
	int numentries;
	int defaultnumcells;
	int sizeofcell;
	int totpage;
	
	void *first;
	struct DYN_LIST *next;
	struct DYN_LIST *current;
#ifdef hpux /* align 8 bytes for hpux */
	void *nothing;
#endif /* hpux */
} DYN_LIST;

typedef struct 
{
	frameRec *firstentry;
	frameRec *firstfree;
	DYN_LIST *pool;
} poolRec, *poolPtr;

typedef struct 
{
	poolRec pools[GARBY_DEF_POOL];
} garbyRec, *garbyPtr;

static garbyPtr gGarbage = NULL;

static DYN_LIST *
listNew(int sizeOfCell, int numOfDefaultCell)
{
	DYN_LIST *result;

	result = (DYN_LIST *)platform_malloc(sizeof(DYN_LIST) + sizeOfCell * numOfDefaultCell);
	if(result == NULL)
	{
		fprintf(stderr, "failed to allocate %d bytes\n", sizeof(DYN_LIST) + sizeOfCell * numOfDefaultCell);
		exit(-1);
	}

	result->numentries = result->totpage = 0;
	result->defaultnumcells = numOfDefaultCell;
	result->sizeofcell = sizeOfCell;
	result->next = NULL;
	result->current = result;
	
	return result;
}

static void
listFree(DYN_LIST *list)
{
	DYN_LIST *ptc1, *ptc2;
	
	ptc1 = list;
	while(ptc1 != NULL)
	{
		ptc2 = ptc1;
		ptc1 = ptc1->next;
		platform_free(ptc2);
	}
}

static void *
listAddCell(DYN_LIST *list)
{
	DYN_LIST *current;
	
	current = list->current;
	
	if(current->numentries == list->defaultnumcells)
	{
		current->next = (DYN_LIST *)platform_malloc(sizeof(DYN_LIST) + list->sizeofcell * list->defaultnumcells);
		if(current->next == NULL)
		{
			fprintf(stderr, "failed to allocate %d bytes\n", sizeof(DYN_LIST) + list->sizeofcell * list->defaultnumcells);
			exit(-1);
		}
		
		current = current->next;
		current->numentries = 0;
		current->next = NULL;
		list->current = current;
		list->totpage++;
	}
	
	return (char *)(current + 1) + current->numentries++ * list->sizeofcell;
}

static int
GARBY_KEY(void *address)
{
	register unsigned long add = (unsigned long)address;

	return (add >> 8) % GARBY_DEF_POOL;
}

void
garbyNew(void)
{
	int i;

#if AUTO_INITALIZE
	if(gGarbage != 0L)
		return;
#endif /* AUTO_INITALIZE */

	gGarbage = (garbyPtr) platform_malloc(sizeof(garbyRec));	
	if(gGarbage == NULL)
	{
		fprintf(stderr, "failed to allocate %d bytes\n", sizeof(garbyRec));
		exit(-1);
	}
	
#if qRangeCheck
	for(i = 0; i < GARBY_DEF_POOL; i++)
	{
		gGarbage->pools[i].firstentry = NULL;
		gGarbage->pools[i].firstfree = NULL;
		gGarbage->pools[i].pool = 0L;
	}
#endif /* qRangeCheck */
	for(i = 0; i < GARBY_DEF_POOL; i++)
	{
		gGarbage->pools[i].firstentry = NULL;
		gGarbage->pools[i].firstfree = NULL;
		gGarbage->pools[i].pool = listNew(sizeof(frameRec), GARBY_DEF_ALLOC);
	}
}

static void
poolRemove(poolPtr collector, int reallyRemove, int verboseMemoryLeak, int *numLeaks)
{
	frameRec *frame = collector->firstentry;
	
#if qRangeCheck
	DYN_LIST *pool;
	if(reallyRemove)
	{
		collector->firstentry = 0L;
		collector->firstfree = 0L;
	}
#endif /* qRangeCheck */

	while(frame != NULL)
	{
#if qCvsDebug
		if(verboseMemoryLeak)
			fprintf(stderr, "MEMORY LEAK!: %s line:%d\n", frame->module, frame->line);
#endif
		if(reallyRemove)
			platform_free(frame->entry);
		frame = frame->next;
		(*numLeaks)++;
	}
	if(reallyRemove)
	{
#if qRangeCheck
		pool = collector->pool;
		collector->pool = 0L;
		listFree(pool);
#else /* !qRangeCheck */
		listFree(collector->pool);
#endif /* !qRangeCheck */
	}
}

void
garbyRemove(int verboseSummary, int verboseMemoryLeak)
{
	int i;
	int numLeaks = 0;
	int reallyRemove = !verboseSummary;
	
	if(gGarbage == NULL)
		return;
	
	for(i = 0; i < GARBY_DEF_POOL; i++)
		poolRemove(&gGarbage->pools[i], reallyRemove, verboseMemoryLeak, &numLeaks);
	
	if(reallyRemove)
	{
		platform_free(gGarbage);
		gGarbage = NULL;
	}
	
#if qCvsDebug
	if(verboseSummary)
		fprintf(stderr, "\nGarbage collection : purged %d allocations\n", numLeaks);
#endif
}

#if qRangeCheck
static void
garbyRangeCheck(void)
{
	poolPtr collector;
	frameRec *frame;
	DYN_LIST *pool;
	int i;
	
	if(gGarbage == 0L)
		return;
	
	for(i = 0; i < GARBY_DEF_POOL; i++)
	{
		collector = &gGarbage->pools[i];
		pool = collector->pool;
		while(pool != 0L)
		{
			ASSERT_RANGE(SIZE_FROM_PTR(pool) > 0);
			CHECK_MAGIC(MAGIC1_FROM_PTR(pool));
			CHECK_MAGIC(MAGIC2_FROM_PTR(pool));
			pool = pool->next;
		}
		
		frame = collector->firstentry;
		
		while(frame != NULL)
		{
			ASSERT_RANGE(SIZE_FROM_PTR(frame->entry) > 0);
			CHECK_MAGIC(MAGIC1_FROM_PTR(frame->entry));
			CHECK_MAGIC(MAGIC2_FROM_PTR(frame->entry));
			frame = frame->next;
		}
	}
}
#endif /* qRangeCheck */

static void
setModule(char *where, const char *what)
{
	int len, cp;

	if(what == 0L)
	{
		strcpy(where, "Unknown");
		return;
	}
	
	cp = len = strlen(what);
	if(len > (MODULE_SIZE - 1))
		cp = MODULE_SIZE - 1;
	
	memcpy(where, what + len - cp, cp * sizeof(char));
	where[cp] = '\0';
}

#if qCvsDebug
static void * poolAlloc(poolPtr pool, void *ptr, int line, const char *file)
#else
static void * poolAlloc(poolPtr pool, void *ptr)
#endif
{
	frameRec *newframe;

	if(pool->firstfree != NULL)
	{
		/*
		 * there is place already free inside collector
		 */
		newframe = pool->firstfree;
		pool->firstfree = newframe->next;
	}
	else
	{
		/*
		 * create a new entry into the pool
		 */
		newframe = (frameRec *)listAddCell(pool->pool);
	}
	
	newframe->entry = ptr;
	newframe->next = pool->firstentry;
	pool->firstentry = newframe;

#if qCvsDebug
	setModule(newframe->module, file);
	newframe->line = line;
#endif
	
	return ptr;
}

static void * poolFree(register poolPtr garb, void *ptr)
{
	frameRec *frame, *prev;
	
	/*
	 * if failed, collector is corrupted
	 */
	frame = garb->firstentry;
	if(frame == NULL)
	{
#ifdef macintosh
		/* only  on mac cause we are sure to intercept every allocations */
		fprintf(stderr, "panic ! memory collector is corrupted !\n");
		return NULL;
#else /* !macintosh */
		return ptr;
#endif /* !macintosh */
	}
	
	/*
	 * - we mark the entry to be free
	 */
	if(frame->entry == ptr)
	{
		frame = garb->firstentry;
		garb->firstentry = frame->next;
	}
	else
	{
		prev = NULL;
		while(frame->entry != ptr)
		{	
			prev = frame;
			frame = frame->next;
			/*
			 * if failed, collector is corrupted
			 */
			if(frame == NULL)
			{
#ifdef macintosh
				/* only  on mac cause we are sure to intercept every allocations */
				fprintf(stderr, "panic ! memory collector is corrupted !\n");
				return NULL;
#else /* !macintosh */
				return ptr;
#endif /* !macintosh */
			}
		}
		prev->next = frame->next;
	}
	
	/*
	 * insert frame into free list
	 */
	frame->next = garb->firstfree;
	garb->firstfree = frame;
	
	return ptr;
}

static void *garby_free(void *ptr);
#define garbyFree(ptr) garby_free(ptr)

#if qCvsDebug
	static void *garby_alloc(void *ptr, int line, const char *file);
	static void *garby_realloc(void *ptr, void *newptr, int line, const char *file);
	
#	define garbyAlloc(ptr, line, file) garby_alloc(ptr, line, file)
#	define garbyRealloc(ptr, NewPtr, line, file) garby_realloc(ptr, NewPtr, line, file)
#else
	static void *garby_alloc(void *ptr);
	static void *garby_realloc(void *ptr, void *newptr);
	
#	define garbyAlloc(ptr, line, file) garby_alloc(ptr)
#	define garbyRealloc(ptr, NewPtr, line, file) garby_realloc(ptr, NewPtr)
#endif

#if qCvsDebug
static void *garby_alloc(void *ptr, int line, const char *file)
#else
static void *garby_alloc(void *ptr)
#endif
{
#if AUTO_INITALIZE
	if(gGarbage == 0L)
		garbyNew();
#endif /* AUTO_INITALIZE */

	if(gGarbage == NULL || ptr == NULL)
		return ptr;

#if qCvsDebug
	return poolAlloc(&gGarbage->pools[GARBY_KEY(ptr)], ptr, line, file);
#else
	return poolAlloc(&gGarbage->pools[GARBY_KEY(ptr)], ptr);
#endif
}

static void *garby_free(void *ptr)
{
	if(gGarbage == NULL || ptr == NULL)
		return ptr;
	
	return poolFree(&gGarbage->pools[GARBY_KEY(ptr)], ptr);
}

#if qCvsDebug
static void *garby_realloc(void *ptr, void *newptr, int line, const char *file)
#else
static void *garby_realloc(void *ptr, void *newptr)
#endif
{
#if AUTO_INITALIZE
	if(gGarbage == 0L)
		garbyNew();
#endif /* AUTO_INITALIZE */

	garby_free(ptr);
#if qCvsDebug
	return garby_alloc(newptr, line, file);
#else
	return garby_alloc(newptr);
#endif
}

#if qCvsDebug
void *	shl_malloc(size_t size, int line, const char *file)
#else
void *	shl_malloc(size_t size)
#endif
{
	char *ptr;

	if (size == 0)
		return NULL;

	ptr = (char *)platform_malloc(size);
	return garbyAlloc(ptr, line, file);
}

#if qCvsDebug
void *	shl_calloc(size_t nmemb, size_t size, int line, const char *file)
#else
void *	shl_calloc(size_t nmemb, size_t size)
#endif
{
	char *ptr;

	if (nmemb == 0 || size == 0)
		return NULL;

	ptr = (char *)platform_calloc(nmemb, size);
	return garbyAlloc(ptr, line, file);
}

#if qCvsDebug
void *	shl_realloc(void * ptr, size_t size, int line, const char *file)
#else
void *	shl_realloc(void * ptr, size_t size)
#endif
{
	char *Newptr;
	
	if (ptr == NULL)
	{
		Newptr = (char *)platform_malloc(size);
		return garbyAlloc(Newptr, line, file);
	}

	if (size == 0)
	{
		platform_free(garbyFree(ptr));
		return NULL;
	}
	
	Newptr = (char *)platform_realloc(ptr, size);
	if(Newptr == ptr)
		return ptr;

	return garbyRealloc(ptr, Newptr, line, file);
}

void	shl_free(void *ptr)
{
	if (ptr != NULL)
		platform_free (garbyFree(ptr));
}

