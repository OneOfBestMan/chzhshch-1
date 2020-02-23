#include "stdafx.h"

#include "traits.h"

#include <assert.h>


Direction operator-(const Direction& d)
{
	switch (d)
	{
	case ASCENDING:
		return DESCENDING;
	case DESCENDING:
		return ASCENDING;
	default:
		assert(0);
	}
	return d;
}