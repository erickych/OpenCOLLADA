#include "GeneratedSaxParserStackMemoryManager.h"

namespace GeneratedSaxParser
{
	//--------------------------------------------------------------------
	StackMemoryManager::StackMemoryManager(size_t stackSize)
		: mCurrentPosition(0),
		mMaxMemoryBlob(stackSize)
		
	{
		mMemoryBlob = new char[stackSize];
	}
	
	//--------------------------------------------------------------------
	StackMemoryManager::~StackMemoryManager()
	{
			delete[] mMemoryBlob;
	}

	//--------------------------------------------------------------------
	void* StackMemoryManager::newObject( size_t objectSize )
	{
		size_t newDataPos = mCurrentPosition;

		size_t newDataSizePos = mCurrentPosition + objectSize;
		mCurrentPosition = newDataSizePos + sizeof(mCurrentPosition);

		if (mCurrentPosition >= mMaxMemoryBlob)
		{
//			assert(false);
			//realloc();
		}

		*((size_t*)(mMemoryBlob + newDataSizePos)) = objectSize;
		//(mMemoryBlob + newDataSizePos) = objectSize;
		return mMemoryBlob + newDataPos;
	}

	void StackMemoryManager::deleteObject()
	{
		//mCurrentPosition -=  ( (*((size_t*)mMemoryBlob[mCurrentPosition - 1])) + sizeof(mCurrentPosition));
		mCurrentPosition -=  ( (*((size_t*)(mMemoryBlob + mCurrentPosition - sizeof(mCurrentPosition)) )) + sizeof(mCurrentPosition));
	}


} // namespace GeneratedSaxParser