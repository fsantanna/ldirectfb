#include "directfb.h"

// funcao STATIC (tb pode ser chamada internamente, por isso o [ ... ]
int l_IDirectFBEventBuffer_toudata (lua_State* L)
{
	// [ ... | IDirectFBEventBuffer* ]
	IDirectFBEventBuffer* evtbuf = lua_touserdata(L, -1);
	if (evtbuf == NULL)
		luaL_error(L, "Esperado lightuserdata representando IDirectFBSurface* .");
	lua_pop(L, 1);            // [ ... ]

	IDirectFBEventBuffer** _evtbuf = (IDirectFBEventBuffer**) lua_newuserdata(L, sizeof(IDirectFBEventBuffer*));
	*_evtbuf = evtbuf;        // [ ... | _evtbuf ]
	luaL_getmetatable(L, "directfb.IDirectFBEventBuffer");
                              // [ ... | _evtbuf | mt ]
	lua_setmetatable(L, -2);  // [ ... | _evtbuf ]
	return 1;
}

static int l_new (lua_State* L)
{
	// [ IDFBEventBuffer | dfb | caps | global ]
	IDirectFB* dfb = * (IDirectFB**) luaL_checkudata(L, 2, "directfb.IDirectFB");
	int caps = luaL_checknumber(L, 3);
	int global = lua_toboolean(L, 4);
	IDirectFBEventBuffer* evtbuf;
	DFBCHECK (dfb->CreateInputEventBuffer (dfb, caps, global, &evtbuf));
	lua_pushlightuserdata(L, evtbuf);   // [ IDFBEventBuffer | dfb | caps | global | evtbuf ]
	l_IDirectFBEventBuffer_toudata(L);  // [ IDFBEventBuffer | dfb | caps | global | _evtbuf ]
	return 1;
}

static int l_gc (lua_State* L)
{
	IDirectFBEventBuffer* evtbuf = * (IDirectFBEventBuffer**) luaL_checkudata(L, 1, "directfb.IDirectFBEventBuffer");
	evtbuf->Release(evtbuf);
	return 0;
}

static int l_WaitForEvent (lua_State* L)
{
	// [ evtbuf ]
	IDirectFBEventBuffer* evtbuf = * (IDirectFBEventBuffer**) luaL_checkudata(L, 1, "directfb.IDirectFBEventBuffer");
	evtbuf->WaitForEvent(evtbuf);
	return 0;
}

static int l_WaitForEventWithTimeout (lua_State* L)
{
	// [ evtbuf | s | ms ]
	IDirectFBEventBuffer* evtbuf = * (IDirectFBEventBuffer**) luaL_checkudata(L, 1, "directfb.IDirectFBEventBuffer");
	evtbuf->WaitForEventWithTimeout(evtbuf, luaL_checknumber(L,2), luaL_checknumber(L,3));
	return 0;
}

static int l_GetEvent (lua_State* L)
{
	IDirectFBEventBuffer* evtbuf = * (IDirectFBEventBuffer**) luaL_checkudata(L, 1, "directfb.IDirectFBEventBuffer");
	DFBEvent evt;
	evtbuf->GetEvent(evtbuf, (DFBEvent*)&evt);
	switch (evt.clazz) {
		case DFEC_INPUT:
			DFBInputEvent2table(L, &evt.input);
			break;
		case DFEC_USER:
			DFBUserEvent2table(L, &evt.user);
			break;
		// TEMP: default?? (retorna nil?)
	}
	return 1;
}

static int l_HasEvent (lua_State* L)
{
	// [ evtbuf ]
	IDirectFBEventBuffer* evtbuf = * (IDirectFBEventBuffer**) luaL_checkudata(L, 1, "directfb.IDirectFBEventBuffer");
	lua_pushboolean(L, evtbuf->HasEvent(evtbuf) == DFB_OK );   // [ evtbuf | has? ]
	return 1;
}

static int l_Reset (lua_State* L)
{
	// [ evtbuf ]
	IDirectFBEventBuffer* evtbuf = * (IDirectFBEventBuffer**) luaL_checkudata(L, 1, "directfb.IDirectFBEventBuffer");
	evtbuf->Reset(evtbuf);
	return 0;
}

static int l_PostEvent (lua_State* L)
{
	// [ evtbuf | evt ]
	IDirectFBEventBuffer* evtbuf = * (IDirectFBEventBuffer**) luaL_checkudata(L, 1, "directfb.IDirectFBEventBuffer");
	DFBUserEvent evt;
	table2DFBUserEvent(L, &evt);
	evtbuf->PostEvent(evtbuf, (DFBEvent*)&evt);
	return 0;
}

static const struct luaL_Reg meths[] = {
	{ "__gc",                    l_gc },
	{ "WaitForEvent",            l_WaitForEvent },
	{ "WaitForEventWithTimeout", l_WaitForEventWithTimeout },
	{ "GetEvent",                l_GetEvent },
	{ "HasEvent",                l_HasEvent },
	{ "Reset",                   l_Reset },
	{ "PostEvent",               l_PostEvent },
	{ NULL,                      NULL }
};

static const struct luaL_Reg funcs[] = {
	{ NULL, NULL }
};

LUALIB_API int luaopen_directfb_IDirectFBEventBuffer (lua_State* L) {
	luaopen_module(L, meths, funcs, l_new);
	return 1;
}
