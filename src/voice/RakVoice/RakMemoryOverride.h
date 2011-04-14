/// \file
/// \brief If _USE_RAK_MEMORY_OVERRIDE is defined, memory allocations go through rakMalloc, rakRealloc, and rakFree
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __RAK_MEMORY_H
#define __RAK_MEMORY_H

#include <new>

#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
                                                                             
#endif

#include <malloc.h>

#if _USE_RAK_MEMORY_OVERRIDE==1
	#if defined(new)
		#pragma push_macro("new")
		#undef new
		#define RMO_NEW_UNDEF
	#endif
#endif


// These pointers are statically and globally defined in RakMemoryOverride.cpp
// Change them to point to your own allocators if you want.
// Use the functions for a DLL, or just reassign the variable if using source
extern void * (*rakMalloc) (size_t size);
extern void * (*rakRealloc) (void *p, size_t size);
extern void (*rakFree) (void *p);
extern void * (*rakMalloc_Ex) (size_t size, const char *file, unsigned int line);
extern void * (*rakRealloc_Ex) (void *p, size_t size, const char *file, unsigned int line);
extern void (*rakFree_Ex) (void *p, const char *file, unsigned int line);
extern void (*notifyOutOfMemory) (const char *file, const long line);
extern void * (*dlMallocMMap) (size_t size);
extern void * (*dlMallocDirectMMap) (size_t size);
extern int (*dlMallocMUnmap) (void* ptr, size_t size);

// Change to a user defined allocation function
void SetMalloc( void* (*userFunction)(size_t size) );
void SetRealloc( void* (*userFunction)(void *p, size_t size) );
void SetFree( void (*userFunction)(void *p) );
void SetMalloc_Ex( void* (*userFunction)(size_t size, const char *file, unsigned int line) );
void SetRealloc_Ex( void* (*userFunction)(void *p, size_t size, const char *file, unsigned int line) );
void SetFree_Ex( void (*userFunction)(void *p, const char *file, unsigned int line) );
// Change to a user defined out of memory function
void SetNotifyOutOfMemory( void (*userFunction)(const char *file, const long line) );
void SetDLMallocMMap( void* (*userFunction)(size_t size) );
void SetDLMallocDirectMMap( void* (*userFunction)(size_t size) );
void SetDLMallocMUnmap( int (*userFunction)(void* ptr, size_t size) );

extern void * (*GetMalloc()) (size_t size);
extern void * (*GetRealloc()) (void *p, size_t size);
extern void (*GetFree()) (void *p);
extern void * (*GetMalloc_Ex()) (size_t size, const char *file, unsigned int line);
extern void * (*GetRealloc_Ex()) (void *p, size_t size, const char *file, unsigned int line);
extern void (*GetFree_Ex()) (void *p, const char *file, unsigned int line);
extern void *(*GetDLMallocMMap())(size_t size);
extern void *(*GetDLMallocDirectMMap())(size_t size);
extern int (*GetDLMallocMUnmap())(void* ptr, size_t size);

namespace RakNet
{

	template <class Type>
	Type* OP_NEW(const char *file, unsigned int line)
	{
#if _USE_RAK_MEMORY_OVERRIDE==1
		char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
		Type *t = new (buffer) Type;
		return t;
#else
		(void) file;
		(void) line;
		return new Type;
#endif
	}

	template <class Type, class P1>
	Type* OP_NEW_1(const char *file, unsigned int line, const P1 &p1)
	{
#if _USE_RAK_MEMORY_OVERRIDE==1
		char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
		Type *t = new (buffer) Type(p1);
		return t;
#else
		(void) file;
		(void) line;
		return new Type(p1);
#endif
	}

	template <class Type, class P1, class P2>
	Type* OP_NEW_2(const char *file, unsigned int line, const P1 &p1, const P2 &p2)
	{
#if _USE_RAK_MEMORY_OVERRIDE==1
		char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
		Type *t = new (buffer) Type(p1, p2);
		return t;
#else
		(void) file;
		(void) line;
		return new Type(p1, p2);
#endif
	}

	template <class Type, class P1, class P2, class P3>
	Type* OP_NEW_3(const char *file, unsigned int line, const P1 &p1, const P2 &p2, const P3 &p3)
	{
#if _USE_RAK_MEMORY_OVERRIDE==1
		char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
		Type *t = new (buffer) Type(p1, p2, p3);
		return t;
#else
		(void) file;
		(void) line;
		return new Type(p1, p2, p3);
#endif
	}

	template <class Type, class P1, class P2, class P3, class P4>
	Type* OP_NEW_4(const char *file, unsigned int line, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
	{
#if _USE_RAK_MEMORY_OVERRIDE==1
		char *buffer = (char *) (GetMalloc_Ex())(sizeof(Type), file, line);
		Type *t = new (buffer) Type(p1, p2, p3, p4);
		return t;
#else
		(void) file;
		(void) line;
		return new Type(p1, p2, p3, p4);
#endif
	}


	template <class Type>
	Type* OP_NEW_ARRAY(const int count, const char *file, unsigned int line)
	{
		if (count==0)
			return 0;

#if _USE_RAK_MEMORY_OVERRIDE==1
//		Type *t;
		char *buffer = (char *) (GetMalloc_Ex())(sizeof(int)+sizeof(Type)*count, file, line);
		((int*)buffer)[0]=count;
		for (int i=0; i<count; i++)
		{
			//t = 
				new(buffer+sizeof(int)+i*sizeof(Type)) Type;
		}
		return (Type *) (buffer+sizeof(int));
#else
		(void) file;
		(void) line;
		return new Type[count];
#endif

	}

	template <class Type>
	void OP_DELETE(Type *buff, const char *file, unsigned int line)
	{
#if _USE_RAK_MEMORY_OVERRIDE==1
		if (buff==0) return;
		buff->~Type();
		(GetFree_Ex())((char*)buff, file, line );
#else
		(void) file;
		(void) line;
		delete buff;
#endif

	}

	template <class Type>
	void OP_DELETE_ARRAY(Type *buff, const char *file, unsigned int line)
	{
#if _USE_RAK_MEMORY_OVERRIDE==1
		if (buff==0)
			return;

		int count = ((int*)((char*)buff-sizeof(int)))[0];
		Type *t;
		for (int i=0; i<count; i++)
		{
			t = buff+i;
			t->~Type();
		}
		(GetFree_Ex())((char*)buff-sizeof(int), file, line );
#else
		(void) file;
		(void) line;
		delete [] buff;
#endif

	}

	void * _RakMalloc (size_t size);
	void * _RakRealloc (void *p, size_t size);
	void _RakFree (void *p);
	void * _RakMalloc_Ex (size_t size, const char *file, unsigned int line);
	void * _RakRealloc_Ex (void *p, size_t size, const char *file, unsigned int line);
	void _RakFree_Ex (void *p, const char *file, unsigned int line);
	void * _DLMallocMMap (size_t size);
	void * _DLMallocDirectMMap (size_t size);
	int _DLMallocMUnmap (void *p, size_t size);

}

// Call to make RakNet allocate a large block of memory, and do all subsequent allocations in that memory block
// Initial and reallocations will be done through whatever function is pointed to by yourMMapFunction, and yourDirectMMapFunction (default is malloc)
// Allocations will be freed through whatever function is pointed to by yourMUnmapFunction (default free)
void UseRaknetFixedHeap(size_t initialCapacity,
						void * (*yourMMapFunction) (size_t size) = RakNet::_DLMallocMMap,
						void * (*yourDirectMMapFunction) (size_t size) = RakNet::_DLMallocDirectMMap,
						int (*yourMUnmapFunction) (void *p, size_t size) = RakNet::_DLMallocMUnmap);

// Free memory allocated from UseRaknetFixedHeap
void FreeRakNetFixedHeap(void);

#if _USE_RAK_MEMORY_OVERRIDE==1
	#if defined(RMO_NEW_UNDEF)
	#pragma pop_macro("new")
	#undef RMO_NEW_UNDEF
	#endif
#endif

#endif
