

// Defines the memory manager functions.
#include <memory.h>
#include "MemoryMgr.h"
#include <string>
#include <fstream>
using namespace std;


// Static members
MemoryManager::MemoryNode *MemoryManager::pNodes = NULL;
MemoryManager::MemoryNode *MemoryManager::pLastNode = NULL;
unsigned int MemoryManager::iuNodeCount = 0;
unsigned int MemoryManager::pItemCount[ENUMERATED_OBJECT_COUNT];
unsigned int MemoryManager::iuTotalMem;

// Dump the contents of the nodes to an ostream.
void MemoryManager::Dump(ostream &osStream)
{

	// Print basic stats about the memory manager. 
	osStream << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n MEMORY USAGE SUMMARY \n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
	osStream << "Total Bytes Allocated:\t " << iuTotalMem << "\t\tActive Allocation Nodes: " << iuNodeCount;
	int iMMOverhead = sizeof(MemoryNode) * ((iuNodeCount > MEMORY_INITIAL_NODE_POOL) ? iuNodeCount : MEMORY_INITIAL_NODE_POOL) /*+ sizeof(MemoryManager)*/;
	osStream << "\nMemory Monitor Overhead: " << iMMOverhead;
	osStream << "\nOther Unreleased Memory: " << iuTotalMem - iMMOverhead; 

	// Print item counts
	osStream << "\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n ACTIVE OBJECT COUNTS \n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
	osStream << "\n\tTypeID\t\t\tNumber Active\n\t----------------------------------------\n";
	for (int i = 0; i < ENUMERATED_OBJECT_COUNT; i++)
	{
		if (pItemCount[i] > 0) 
			osStream << '\t' << i << "\t\t\t" << pItemCount[i] << '\n';
	}

	// Traverse the list and print
	osStream << "\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n ACTIVE MEMORY NODES \n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
	MemoryNode *pCurrent = pNodes;
	while (pCurrent)
	{
		// Only print active nodes
		if (pCurrent->bActive)
		{
			osStream  << "File: " << pCurrent->strFile << '\n';
			osStream << "Line: " << pCurrent->iLine << "\t\tType ID: " << pCurrent->isType << "\t\tAddress: " << pCurrent->pAddress << '\n';
			osStream << "Size: " << pCurrent->iuSize << "\t\tCount: " << pCurrent->iuCount << "\t\tTotal allocated: " << (pCurrent->iuCount * pCurrent->iuSize) << "\n\n";
		}

		// Iterate
		pCurrent = pCurrent->pNext;
	}
}


// Dump, with a pre-determined output file stream
void MemoryManager::DumpToFile(void)
{
			ofstream ofsMemoryLog;
			ofsMemoryLog.open("Debug\\MemoryUsage.txt");
			Dump(ofsMemoryLog);
			ofsMemoryLog.close();
}

// Called when we use "new"
void MemoryManager::New(void *pAddress, size_t szSize, int iTypeSize,  unsigned char isType, const char *strFile, int iLine)
{
	
	// Update total memory used
	iuTotalMem += (unsigned int)szSize; 
	
	// Update total nodes used.
	iuNodeCount++;

	// Only do one divide, if that much
	// The - 4 is to account for the phantom four bytes added to arrays with new[].
	// This isn't an issue unless we were new-ing a 4 byte class, so this is a bit paranoid.
	int iItemCount = (iTypeSize == (int)szSize) ? 1 : ((int)szSize - 4)/ iTypeSize;

	// Count the item in its item type.
	pItemCount[isType] += iItemCount;

	// Find an available node
	MemoryNode *pNode = pNodes;

	while (pNode)
	{
		if (!pNode->bActive)
			break;
		pNode = pNode->pNext;
	}
	
	// If no active node was found, make one.
	if (!pNode)
	{
		pNode = new MemoryNode; 
		iuTotalMem += sizeof(MemoryNode);
		iuNodeCount++;
		pLastNode->pNext = pNode;
		pLastNode = pNode;
	}

	// Fill in node.
	pNode->pAddress = pAddress;
	pNode->bActive = true;
	pNode->isType = isType;

	strncpy(pNode->strFile, strFile, MEMORY_FILENAME_LENGTH);
	pNode->strFile[MEMORY_FILENAME_LENGTH - 1] = '\0';

	pNode->iLine = iLine;
	pNode->iuCount = iItemCount;
	pNode->iuSize = iTypeSize;

}

// Called when we delete something
bool MemoryManager::Delete(void *pAddress)
{
 	MemoryNode *pNode = pNodes;
	while (pNode)
	{
		if (pAddress == pNode->pAddress)
		{		
			// Un-count this item in the item counts
			pItemCount[pNode->isType] -= pNode->iuCount;

			// Uncount the "active node" counter
			iuNodeCount--;

			// HACK:
			// Since new[] creates four extra bytes (these appear to be freed when we free the array)
			// the manager tracks the 4 bytes. However, we only store in the node a count and a size.
			// Therefore, the total memory count never loses those 4 bytes.
			// If we're freeing an array, just subtract four more bytes from the count:
			if (pNode->iuCount != 1) iuTotalMem -= 4;

			// Uncount the memory's size
			iuTotalMem -= (pNode->iuSize * pNode->iuCount);

			// Clear the node (except for ->pNext)
			pNode->bActive = false;
			pNode->pAddress = NULL;
			pNode->isType = INVALID;
			pNode->iLine = -1;
			pNode->strFile[0] = '\0';
			pNode->iuSize = 0;
			pNode->iuCount = 0;

			return true;
		}

		// try the next one
		pNode = pNode->pNext;
	}
	return false;
}


// Set up the initial values for the manager.
void MemoryManager::Initialize(void)
	{
		iuTotalMem = /*sizeof(MemoryManager)*/ + (sizeof(MemoryNode) * MEMORY_INITIAL_NODE_POOL);
		pNodes = NULL; 
		pLastNode = NULL;
		iuNodeCount = 0;
		memset(pItemCount, 0, sizeof(int) * ENUMERATED_OBJECT_COUNT);

		// Try making a linked list with seperate mallocs.

			// CHANGED
			//	using new instead of calloc
			//	Rolf, 5-10-04
			
			// Make first one
			//pNodes = (MemoryNode*)calloc(sizeof(MemoryNode), 1);
			pNodes = new MemoryNode;
			memset (pNodes, 0, sizeof (MemoryNode));

			pLastNode = pNodes;

			// Pointer to "current"
			MemoryNode *pCurrent = pNodes;

			// Make child nodes
			for (int i = 1; i < MEMORY_INITIAL_NODE_POOL; i++)
			{
				pCurrent->pNext = new MemoryNode;
				memset (pCurrent->pNext, 0, sizeof (MemoryNode));

				//pCurrent->pNext = (MemoryNode*)calloc(sizeof(MemoryNode), 1); 
				pCurrent = pCurrent->pNext;
			}
	}

	// Try deleting recursively
	void MemoryManager::NodeDelete(MemoryNode *pMem)
	{
		// CHANGED (Rolf)
		// this part kept causing a stack overflow exception, so I changed it
		// Rolf, cinqo de Mayo '04

		while (pMem)
		{
			// Go to the end of the list first
			//NodeDelete(pMem->pNext);	
			MemoryNode* pTemp = pMem;
			pMem = pMem->pNext;
			delete pTemp;			
		}
	}


	// Clean up after the memory manager itself.
	void MemoryManager::Shutdown(void)
	{
		// Kill the nodes themselves
		NodeDelete(pNodes);

		// Clear out the other values.
		iuTotalMem = 0;
		iuNodeCount = 0;
		pLastNode = NULL;
		pNodes = NULL;

	}
