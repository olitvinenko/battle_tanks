#pragma once

struct GlTexture
{
	union
	{
		unsigned int index;
		void          *ptr;
	};

	bool operator <(const GlTexture &r) const
	{
		return ptr < r.ptr;
	}
	bool operator ==(const GlTexture &r) const
	{
		return ptr == r.ptr;
	}
};
