#pragma once

#define SAFE_KILL(w, p)      { if(p) (p)->Kill(w); (p)=nullptr; }

#define FOREACH(list, type, var)																			\
	if( type *var = (type *) (0xffffffff) )																	\
	for( PtrList<GC_Object>::id_type var##iterator = (list).begin();										\
	     var##iterator != (list).end() && ((var = static_cast<type *>((list).at(var##iterator))), true);	\
		 var##iterator = (list).next(var##iterator))
