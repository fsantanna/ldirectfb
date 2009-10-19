#include <unistd.h>
#include "ldirectfb.h"

#include <lauxlib.h>

// funcao STATIC (tb pode ser chamada internamente, por isso o [ ... ]
int l_IDirectFBWindow_toudata (lua_State* L)
{
	// [ ... | IDirectFBWindow* ]
	IDirectFBWindow* win = lua_touserdata(L, -1);
	if (win == NULL)
		luaL_error(L, "Esperado lightuserdata representando IDirectFBWindow* .");
	lua_pop(L, 1);            // [ ... ]

	IDirectFBWindow** _win = (IDirectFBWindow**) lua_newuserdata(L, sizeof(IDirectFBWindow*));
	*_win = win;              // [ ... | _win ]
	luaL_getmetatable(L, "ldirectfb.IDirectFBWindow");
                              // [ ... | _win | mt ]
	lua_setmetatable(L, -2);  // [ ... | _win ]
	return 1;
}

static int l_new (lua_State* L)
{
	// [ IDFBWindow | layer | dsc ]
	IDirectFBDisplayLayer* layer = * (IDirectFBDisplayLayer**) luaL_checkudata(L, 2, "ldirectfb.IDirectFBDisplayLayer");
	DFBWindowDescription dsc;
	table2DFBWindowDescription (L, &dsc);
	IDirectFBWindow* win;
	DFBCHECK (layer->CreateWindow(layer, &dsc, &win));
	win->SetOptions(win, (DFBWindowOptions)(DWOP_ALPHACHANNEL));
	lua_pushlightuserdata(L, win);   // [ IDFBWindow | layer | dsc | win ]
	l_IDirectFBWindow_toudata(L);                    // [ IDFBWindow | layer | dsc | _win ]
	return 1;
}

static int l_gc (lua_State* L)
{
	// TEMP existe esse Release?
	IDirectFBWindow* win = * (IDirectFBWindow**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBWindow");
	win->Release(win);
	return 0;
}

static int l_GetSurface (lua_State* L)
{
	// [ win ]
	IDirectFBWindow* win = * (IDirectFBWindow**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBWindow");
	IDirectFBSurface** sfc = (IDirectFBSurface**) lua_newuserdata(L, sizeof(IDirectFBSurface*));
	luaL_getmetatable(L, "ldirectfb.IDirectFBSurface");
                                  // [ win | sfc | mt ]
	lua_setmetatable(L, -2);      // [ win | sfc ]
	DFBCHECK (win->GetSurface(win, sfc));
	return 1;
}

static int l_SetOpacity (lua_State* L)
{
	// [ win | opacity ]
	IDirectFBWindow* win = * (IDirectFBWindow**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBWindow");
	win->SetOpacity(win, luaL_checknumber(L, 2));
	return 0;
}

static const struct luaL_Reg meths[] = {
	{ "__gc",          l_gc },
	{ "GetSurface",    l_GetSurface },
	{ "SetOpacity",    l_SetOpacity },
	{ NULL,            NULL }
};

static const struct luaL_Reg funcs[] = {
	{ "__toudata",     l_IDirectFBWindow_toudata },
	{ NULL, NULL }
};

LUALIB_API int luaopen_ldirectfb_IDirectFBWindow (lua_State* L) {
	// depende de "IDirectFBDisplayLayer"
	// [ ... ]
	lua_getglobal(L, "require");                    // [ ... | require ]
	lua_pushstring(L, "ldirectfb.IDirectFBDisplayLayer"); // [ ... | require | "IDFBDisplayLayer" ]
	lua_call(L, 1, 1);                              // [ ... | _M ]
	lua_pop(L,1);                                   // [ ... ]

	luaopen_module(L, meths, funcs, l_new);
	return 1;
}
