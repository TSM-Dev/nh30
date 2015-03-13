#include "dtmgr.h"

NetworkClass *dtmgr::nwclass;
RecvTable   **dtmgr::nwtable;

extern IClient *client;


void dtmgr::Lineout(RecvTable *table)
{
	static int dtindex = 0;
	nwtable[dtindex++] = table;

	for (int i = 0; i < table->iprops; i++)
	{
		RecvTable *dt = table->props[i].datatable;

		if (dt)
		{
			bool g = true;

			for (int j = 0; j < dtindex && g; j++)
			{
				if (nwtable[j] == dt)
					g = false;
			}

			if (g)
				Lineout(dt);
		}
	}
}

int dtmgr::GetOffset(const char *dt, const char *name)
{
	for (int i = 0;; i++)
	{
		RecvTable *table = nwtable[i];

		if (!table)
			return 0;

		if (*dt == '*' || strcmp(table->name, dt) == 0)
		{
			for (int j = 0; j < table->iprops; j++)
			{
				RecvProp *prop = &table->props[j];

				if (strcmp(prop->name, name) == 0)
				{
					return prop->offset;
				}
			}
		}
	}

	return 0;
}

void dtmgr::Start()
{
	nwtable = new RecvTable *[1024];


	for (int i = 0; !nwclass; i++)
	{
		if (*client->GetMethod<char *>(i) == 0xa1)
			nwclass = **(NetworkClass ***)(client->GetMethod<char *>(i) + 1);
	}

	for (NetworkClass *next = nwclass; next; next = next->next)
		Lineout(next->table);

	Map();
}