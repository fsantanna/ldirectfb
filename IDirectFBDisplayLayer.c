#include <unistd.h>
#include "ldirectfb.h"

#include <lauxlib.h>

static int l_new (lua_State* L)
{
	// [ IDFBDisplayLayer | dfb | layer_id ]
	IDirectFB* dfb = * (IDirectFB**) luaL_checkudata(L, 2, "ldirectfb.IDirectFB");
	IDirectFBDisplayLayer** layer = (IDirectFBDisplayLayer**) lua_newuserdata(L, sizeof(IDirectFBDisplayLayer*));
	luaL_getmetatable(L, "ldirectfb.IDirectFBDisplayLayer");
                                  // [ IDFBSurface | dfb | layer_id | layer | mt ]
	lua_setmetatable(L, -2);      // [ IDFBSurface | dfb | layer_id | layer ]
	DFBCHECK (dfb->GetDisplayLayer(dfb, (DFBDisplayLayerID)luaL_checknumber(L,3), layer));
	return 1;
}

static int l_gc (lua_State* L)
{
	// TEMP existe esse Release?
	IDirectFBDisplayLayer* layer = * (IDirectFBDisplayLayer**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBDisplayLayer");
	layer->Release(layer);
	return 0;
}

static int l_GetSurface (lua_State* L)
{
	// [ layer ]
	IDirectFBDisplayLayer* layer = * (IDirectFBDisplayLayer**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBDisplayLayer");
	IDirectFBSurface* sfc;
	DFBCHECK (layer->GetSurface(layer, &sfc));
	lua_pushlightuserdata(L, sfc);    // [ layer | sfc ]
	l_IDirectFBSurface_toudata(L);    // [ layer | _sfc ]
 	return 1;
}

static int l_CreateWindow (lua_State* L)
{
	// [ ... ]
	CreateInstance(L, "ldirectfb.IDirectFBWindow");
	// [ instance ]
}

static const struct luaL_Reg meths[] = {
	{ "__gc",          l_gc },
	{ "GetSurface",    l_GetSurface },
	{ "CreateWindow",  l_CreateWindow },
	{ NULL,            NULL }
};

static const struct luaL_Reg funcs[] = {
	{ NULL, NULL }
};

LUALIB_API int luaopen_ldirectfb_IDirectFBDisplayLayer (lua_State* L)
{
	// depende de "IDirectFBSurface"
	// [ ... ]
	lua_getglobal(L, "require");                    // [ ... | require ]
	lua_pushstring(L, "ldirectfb.IDirectFBSurface"); // [ ... | require | "IDFBSurface" ]
	lua_call(L, 1, 1);                              // [ ... | _M ]
	lua_pop(L,1);                                   // [ ... ]

	luaopen_module(L, meths, funcs, l_new);
	return 1;
}
