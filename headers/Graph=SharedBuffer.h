#ifndef GraphSharedBuffer_h
#define GraphSharedBuffer_h

namespace Graph {

	template<typename T>
	class SharedBuffer {
	private:
		SharedBuffer* mpOther;
		T* mpData;
		unsigned int mnSize;
		T* mpCurrent;
		
		//these constructors are hidden, but may be used by other instances
		SharedBuffer()
			: mpData(NULL), mnSize(0), mpCurrent(NULL) {}
		SharedBuffer(const SharedBuffer* pOther,
					 const T* pData,
					 const unsigned int nSize);
		
		//the copy constructor and assignment operator are hidden and not used (or defined)
		SharedBuffer(const SharedBuffer& cSource);
		SharedBuffer& operator=(const SharedBuffer& cRhs);
		
		void stepForward();
		void stepBack();
		
	public:
		SharedBuffer(const unsigned int nSize);
		SharedBuffer(const unsigned int nSize,
					 const SharedBuffer* pOther);
		~SharedBuffer();
		
		SharedBuffer& operator++()
			{ stepForward();
			  return *this; }
		SharedBuffer& operator--()
			{ stepBack();
			  return *this; }
		inline T pull() const 
			{ this->step();
			  return *mpCurrent; }
		inline void push(T tData)
			{ this->step();
			  *mpCurrent = tData; }
		
		friend SharedBuffer& operator<<(SharedBuffer& out, T& tData); //delegates to push
		friend SharedBuffer& operator>>(SharedBuffer& in, T& tData); //delegates to pull
	}; //class Buffer

	#include "Graph=SharedBuffer.cpp"

} //namespace Graph

#endif
