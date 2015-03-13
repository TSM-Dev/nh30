#pragma once
#include "sdk.h"

struct RecvProp;
struct RecvTable;

struct RecvProp
{
	const char	*name;
	int			type;
	int			flags;
	int			size;
	bool		insidearray;
	void		*extradata;
	RecvProp	*arrayprop;
	void		*lenproxy;
	void		*proxy;
	void		*dtproxy;
	RecvTable	*datatable;
	int			offset;
	int			elementstride;
	int			elements;
	const char	*parentarrayname;
};

struct RecvTable
{
	RecvProp	*props;
	int			iprops;
	void		*decoder;
	const char	*name;
	bool		init;
	bool		main;
};

struct NetworkClass
{
private:
	void			*header[2];

public:
	const char		*name;
	RecvTable		*table;
	NetworkClass	*next;
	int				classid;
};

namespace dtmgr
{
	extern NetworkClass *nwclass;
	extern RecvTable   **nwtable;

	void Start();
	void Map();
	void Lineout(RecvTable *);

	int GetOffset(const char *, const char *);
}