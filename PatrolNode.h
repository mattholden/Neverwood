#ifndef StormForge_PatrolNode
#define StormForge_PatrolNode

#include "Vector.h"

class PatrolRoute
{

	private:

	struct PatrolNode
		{
			PatrolNode *pNext, *pPrev;
			Vector vPatrolPt;
		};


public:
	PatrolNode *pFirst, *pLast;
	int iPointCount;

	// construct
	PatrolRoute(void) { pFirst = NULL; pLast = NULL; iPointCount = 0; }
	
	// Add to end of list
	void Add(Vector &vPointToAdd)
	{
		// Make a node for it
		PatrolNode *pNew = new PatrolNode;
		pNew->vPatrolPt = vPointToAdd;
		pNew->pNext = NULL;

		iPointCount++;

		// Special case - 1st item in the list
		if (!pFirst)
		{
			pFirst = pNew;
			pLast = pNew;
			pNew->pPrev = NULL;
		}

		else
		{
			// Add.
			pNew->pPrev = pLast;
			pLast->pNext = pNew;
			pLast = pNew;
		}

	}

	void Remove(Vector &vToRemove)
	{
		PatrolNode *pRemove = pFirst;
		while (pRemove)
		{
			if (pRemove->vPatrolPt == vToRemove)
			{
				if (pRemove->pPrev)
					pRemove->pPrev->pNext = pRemove->pNext;
				if (pRemove->pNext)
					pRemove->pNext->pPrev = pRemove->pPrev;

				if (pRemove == pFirst)
					pFirst = pRemove->pNext;
				if (pRemove == pLast)
					pLast = pRemove->pPrev;

				iPointCount--;
				delete pRemove;
				return;
			}

			pRemove = pRemove->pNext;
		}

	}

	void Clear(void)
	{
		PatrolNode *pDel = pLast;
		while (pDel)
		{
			PatrolNode *pTemp = pDel;
			pDel = pDel->pPrev;
			delete pTemp;
		}
		pFirst = NULL;
		pLast = NULL;
		iPointCount  = 0;

	}

	// quick access to the list
	PatrolNode* operator[] (int index)
	{
		PatrolNode* pCurrent = pFirst;
		for (int i=0; i<index; i++)
			pCurrent = pCurrent->pNext;
		return pCurrent;
	}

	
	~PatrolRoute(void) { Clear(); }


};

#endif
