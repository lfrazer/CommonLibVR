#include "RE/TESObjectSTAT.h"


namespace RE
{
	bool TESObjectSTAT::HasTreeLOD() const
	{
		return (formFlags & RecordFlags::kHasTreeLOD) != 0;
	}
}
