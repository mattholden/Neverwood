
// Define a memory monitor.

#ifndef StormForge_Matt_MemoryMonitor
#define StormForge_Matt_MemoryMonitor


/* USAGE NOTES: 
	
	1. For items that are singletons or that are rarely allocated (such as manager classes), you may find it easier to manually 
		register the allocation with the memory manager. To do this, simply follow your call to 'new' for the instance pointer with
		this macro call:

		#if _DEBUG
			QuickNew(pInstance, TYPEID);
		#endif

		The TYPEID must be a valid, unique member of an enumeration in Identifiers.h and can be hardcoded into the 
		MemoryManager::New() call without storing an iTypeID in each manager class. A CoreTypes enum has been provided 
		in Identifiers.h for adding any new types of core technologies that the memory manager should know about.

		The call that follows 'delete' is even simpler:

		#if _DEBUG
			QuickDelete(pInstance);
		#endif

		Note that MemoryMgr.h must still be included to use this.
		
	2. To make a class memory managed, paste the following code into the class declaration's PUBLIC section.
		Note that this code MUST be in all derivations of the class; inheriting from the base will not be sufficient.
		MemoryMgr.h will need to be included in the header file of any class which uses the memory manager.

		Paste this code:
		#if _DEBUG
			ManageMemory(TYPE, TYPEID)
		#endif

		Replace TYPE with the name of the class (i.e. 'Base') and TYPEID with the enumerated type of this object.
		All object types tracked by the memory manager must have a unique ID. See Identifiers.h for more information
		regarding this requirement and how to fulfill it.

		ex: ManageMemory(Ranger, BASETYPE_RANGER);


	*/

#include <malloc.h>
#include "Identifiers.h"
#include <iostream>
using namespace std;

#define MEMORY_INITIAL_NODE_POOL 5000
#define MEMORY_FILENAME_LENGTH 120

class MemoryManager
{
private:

	struct MemoryNode
	{
		char strFile[MEMORY_FILENAME_LENGTH];				// File name that this object was declared on.
																						// This hurts a lot memorywise but is faster than strtok() through the
																						// filename looking for where the last \ is.
																						// Bear in mind that the MemoryMonitor only does anything in Debug.

		MemoryNode *pNext;		// Linked-list support
		void *pAddress;				// The address of where the memory is stored. For Delete.
		int iLine;					// Line number that this object was declared on.
		bool bActive;				// Is this node currently active?
		unsigned short isType;	// The type of object that this is.
		unsigned int iuSize;			// Size of one of this type.
		unsigned int iuCount;		// How many we allocated. (for Delete)
		unsigned char  __PADDING__;
	};


	// Count of all memory allocated in the system at the moment
	static unsigned int iuTotalMem;

	// Count how many of each type of item we have.
	static unsigned int pItemCount[ENUMERATED_OBJECT_COUNT];

	// Our nodes that we're using to keep track of the memory.
	static unsigned int iuNodeCount;
	static MemoryNode *pNodes;
	static MemoryNode *pLastNode;

	// Delete nodes recursively
	static void NodeDelete(MemoryNode *pMem);

public:


	// Dump the contents of the nodes to an ostream.
	static void MemoryManager::Dump(std::ostream &osStream);
	static void MemoryManager::DumpToFile(void); 

	// Singleton instance.
	//static MemoryManager * Instance;

	// How much dynamic RAM are we using right now?
	static unsigned int GetTotalMemoryUsage(void) { return iuTotalMem; }

	// Called when we use "new"
	static void New(void *pAddress, size_t szSize, int iTypeSize, unsigned char isType, const char *strFile = "Filename unspecified in call.", int iLine = 0);

	// Called when we use "delete"
	static bool Delete(void *pAddress);

	// One stop init code.
	static void Initialize(void);

	// Clean up after the memory manager itself.
	static void Shutdown(void);

	// Free up any mem we would have leaked
	static void MemoryManager::FreeLeakedMemory(void);

};



// Quick manual New and Delete for non-overloaded data types.
#define QuickNew(_pInstance, _typeID) MemoryManager::New((_pInstance), sizeof(*_pInstance), sizeof(*_pInstance), (_typeID), __FILE__, __LINE__);
#define QuickDelete(_pInstance) MemoryManager::Delete((_pInstance));

// Conglomerated class overload for new and delete
#define ManageMemory(_type, _typeID) OpNew(_type, _typeID); OpNewArray(_type, _typeID); OpDeleteCore; OpDelete; OpDeleteArray; 

////////////////////////////////////////////////////////////////////////
// Call all of these in the header of every class you wish to have the Mem Manager track
// You still have to do it for inherited classes, too.. not just in base classes
// Do this in the PUBLIC section of the class declaration
////////////////////////////////////////////////////////////////////////
#define OpNew(_type, _typeID)  static __forceinline   void *operator new(size_t szSz, int iL = __LINE__, const char* strF = __FILE__) { \
	void *pNew = malloc(szSz); \
	MemoryManager::New(pNew, szSz, sizeof(_type), (_typeID), strF,  iL); \
	return pNew; }

#define OpNewArray(_type, _typeID) static __forceinline   void *operator new[](size_t szSz,  int iL = __LINE__, const char* strF = __FILE__) { \
	void *pNew = malloc(szSz); \
    MemoryManager::New(pNew, szSz, sizeof(_type), (_typeID), strF,  iL); \
	return pNew; }

// This extra delete needed to be added to defeat some compiler issues and warnings
#define OpDeleteCore		static __forceinline  void operator delete	(void *pAddr) { \
												MemoryManager::Delete(pAddr); \
												if(pAddr) free(pAddr);  pAddr = NULL;} \
												 void operator delete[]	(void *pAddr) { \
												MemoryManager::Delete(pAddr); \
												if(pAddr) free(pAddr); pAddr = NULL;  } 

#define OpDelete			static __forceinline  void operator delete	(void *pAddr, int i, const char* c) { \
	MemoryManager::Delete(pAddr); \
	delete pAddr;   pAddr = NULL;}

#define OpDeleteArray	static __forceinline   void operator delete[]	(void *pAddr, int i, const char* c) { \
	MemoryManager::Delete(pAddr); \
	delete [] pAddr; 	 pAddr = NULL;	}

//////////////////////////////////////////////////////////////////////////


#endif
 