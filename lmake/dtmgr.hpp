#pragma once
#include "sdk.hpp"

struct RecvProp;
struct RecvTable;

union RecvProxyResult
{
	int   i32;
	float f32;
	void  *data;
};

struct RecvProxyData
{
	const RecvProp *recvprop;

	struct
	{
		union
		{
			float f32;
			int	  i32;
			void  *data;
		};

		int type;
	}
	value;

	int	element;
	int	objectid;
};

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

	void SetHook(const char *, const char *, void (*)(const RecvProxyData &, void *, RecvProxyResult &));
	int GetOffset(const char *, const char *);
}