#include "vmt.h"

VMT::VMT(void *baseaddress)
{
	classbase = (void ***)baseaddress;

	org_vmt = *classbase;
	vmtsize	= getvmtsize();

	new_vmt = new void * [vmtsize];
	memcpy(new_vmt, org_vmt, vmtsize * sizeof(void *));

	*classbase = new_vmt;
}

VMT::~VMT()
{
	*classbase = org_vmt;
	delete new_vmt;
}





int VMT::getvmtsize()
{
	register int i = 0;

	for (; org_vmt[i] && org_vmt[i] < (void *)org_vmt; i++);
	return i;
}

void VMT::hook(int i, void *imp, void **org)
{
	new_vmt[i] = imp;

	if (org)
	{
		*org = org_vmt[i];
	}
}