#include "BaseConfiguration.h"

BaseConfiguration::BaseConfiguration(ConfVarTable *root, bool)
    : _root(root)
    , _isOwner(nullptr == root)
{
    if (_isOwner)
    {
        assert(!_root);
        _root = new ConfVarTable();
    }
}
    
BaseConfiguration::~BaseConfiguration()
{
    if (_isOwner)
    {
        assert(_root);
        delete _root;
    }
}
