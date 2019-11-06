#include "SaveFile.h"

SaveFile::SaveFile(FileSystem::Stream &s, bool loading)
  : _stream(s)
  , _load(loading)
{
}

void SaveFile::RegPointer(GC_Object *ptr)
{
	assert(!_ptrToIndex.count(ptr));
	_ptrToIndex[ptr] = _indexToPtr.size();
	_indexToPtr.push_back(ptr);
}

size_t SaveFile::GetPointerId(GC_Object *ptr) const
{
	if( ptr )
	{
		assert(_ptrToIndex.count(ptr));
		return _ptrToIndex.find(ptr)->second;
	}
	return 0;
}

GC_Object* SaveFile::RestorePointer(size_t id) const
{
	if( _indexToPtr.size() <= id )
		throw std::runtime_error("(Unserialize) invalid pointer id");
	return _indexToPtr[id];
}

void SaveFile::Serialize(std::string &str)
{
	std::string::size_type len = str.length();
	Serialize(len);
	if( len )
	{
		if( loading() )
		{
			std::vector<std::string::value_type> buffer(len);
			SerializeArray(&*buffer.begin(), len);
			str.resize(0);
			str.reserve(len);
			str.insert(str.begin(), buffer.begin(), buffer.end());
		}
		else
		{
			SerializeArray(const_cast<std::string::value_type*>(str.data()), len);
		}
	}
}

