#include "ldirectfb.h"

static int l_new (lua_State* L)
{
	IDirectFB** dfb = (IDirectFB**) lua_newuserdata(L, sizeof(IDirectFB*));
	luaL_getmetatable(L, "ldirectfb.IDirectFB");
	lua_setmetatable(L, -2);
	DFBCHECK (DirectFBCreate (dfb));
	return 1;
}

static int l_gc (lua_State* L)
{
	IDirectFB* dfb = * (IDirectFB**) luaL_checkudata(L, 1, "ldirectfb.IDirectFB");
	dfb->Release(dfb);
	return 0;
}

static int l_SetCooperativeLevel (lua_State* L)
{
	IDirectFB* dfb = * (IDirectFB**) luaL_checkudata(L, 1, "ldirectfb.IDirectFB");
	DFBCHECK (dfb->SetCooperativeLevel(dfb, (DFBCooperativeLevel)luaL_checknumber(L, 2)) );
	return 0;
}

LUALIB_API int luaopen_ldirectfb_IDirectFBDisplayLayer (lua_State* L);
static int l_GetDisplayLayer (lua_State* L) {
	CreateInstance(L, "ldirectfb.IDirectFBDisplayLayer");
	return 1;
}

LUALIB_API int luaopen_ldirectfb_IDirectFBSurface (lua_State* L);
static int l_CreateSurface (lua_State* L) {
	CreateInstance(L, "ldirectfb.IDirectFBSurface");
	return 1;
}

LUALIB_API int luaopen_ldirectfb_IDirectFBImageProvider (lua_State* L);
static int l_CreateImageProvider (lua_State* L) {
	CreateInstance(L, "ldirectfb.IDirectFBImageProvider");
	return 1;
}

LUALIB_API int luaopen_ldirectfb_IDirectFBVideoProvider (lua_State* L);
static int l_CreateVideoProvider (lua_State* L) {
	CreateInstance(L, "ldirectfb.IDirectFBVideoProvider");
	return 1;
}

LUALIB_API int luaopen_ldirectfb_IDirectFBFont (lua_State* L);
static int l_CreateFont (lua_State* L) {
	CreateInstance(L, "ldirectfb.IDirectFBFont");
	return 1;
}

LUALIB_API int luaopen_ldirectfb_IDirectFBEventBuffer (lua_State* L);
static int l_CreateInputEventBuffer (lua_State* L) {
	CreateInstance(L, "ldirectfb.IDirectFBEventBuffer");
	return 1;
}

static const struct luaL_Reg meths[] = {
	//{ "__call", l_new },
	{ "__gc",   l_gc },
	{ "SetCooperativeLevel",    l_SetCooperativeLevel },
	{ "GetDisplayLayer",        l_GetDisplayLayer },
	{ "CreateSurface",          l_CreateSurface },
	{ "CreateImageProvider",    l_CreateImageProvider },
	{ "CreateVideoProvider",    l_CreateVideoProvider },
	{ "CreateFont",             l_CreateFont },
	{ "CreateInputEventBuffer", l_CreateInputEventBuffer },
	{ NULL, NULL }
};

static const struct luaL_Reg funcs[] = {
	{ NULL, NULL }
};

LUALIB_API int luaopen_ldirectfb_IDirectFB (lua_State* L) {
	luaopen_module(L, meths, funcs, l_new);
	return 1;
}
