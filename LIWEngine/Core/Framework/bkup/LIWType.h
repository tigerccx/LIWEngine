#pragma once
#include "LIWBaseType.h"

namespace LIW {
	template<class T>
	class LIWType :
		public LIWBaseType
	{
	public:
		static LIWBaseType const* GetType() { return (LIWBaseType*)&instance; }
		static const long GetTypeID() { return (long)&instance; }
		static bool IsType(LIWBaseType const* type) { return type == (LIWBaseType*)&instance; }
	protected:
		LIWType<T>() { }
		const static LIWType<T> instance;
	};

	class LIWType_Char :		public LIWType<char> {};
	class LIWType_PChar :		public LIWType<char*> {};
	class LIWType_UChar :		public LIWType<unsigned char> {};
	class LIWType_PUChar :		public LIWType<unsigned char*> {};
	class LIWType_Short :		public LIWType<short> {};
	class LIWType_PShort :		public LIWType<short*> {};
	class LIWType_UShort :		public LIWType<unsigned short> {};
	class LIWType_PUShort :		public LIWType<unsigned short*> {};
	class LIWType_Int:			public LIWType<int>{};
	class LIWType_PInt:			public LIWType<int*>{};
	class LIWType_UInt:			public LIWType<unsigned int>{};
	class LIWType_PUInt:		public LIWType<unsigned int*>{};
	class LIWType_Long :		public LIWType<long> {};
	class LIWType_PLong :		public LIWType<long*> {};
	class LIWType_ULong :		public LIWType<unsigned long> {};
	class LIWType_PULong :		public LIWType<unsigned long*> {};
	class LIWType_LongLong :	public LIWType<long long> {};
	class LIWType_PLongLong :	public LIWType<long long*> {};
	class LIWType_Float :		public LIWType<float> {};
	class LIWType_PFloat :		public LIWType<float*> {};
	class LIWType_Double :		public LIWType<double> {};
	class LIWType_PDouble :		public LIWType<double*> {};
	class LIWType_PVoid :		public LIWType<void*> {};	
}