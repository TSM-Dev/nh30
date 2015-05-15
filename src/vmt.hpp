#pragma once
#include <cstdlib>
#include <cstring>

class VMT
{
	friend class Interface;

	void	***classbase;
	void	**org_vmt;
	void	**new_vmt;
	int		vmtsize;

	VMT(const VMT &);

public:
	explicit VMT(void *);
	~VMT();

	void hook(int i, void *imp, void **org);
	int getvmtsize();
};