#include "TypeRegistry.h"

TypeIndex TypeRegistry::nextIndex = 1;

TypeRegistry::TypeRegistry()
{
	index = nextIndex;
	++nextIndex;
}

TypeIndex TypeRegistry::getIndex() const
{
	return index;
}
