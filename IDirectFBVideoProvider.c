#include <unistd.h>
#include "ldirectfb.h"

#include <lauxlib.h>

// funcao STATIC (tb pode ser chamada internamente, por isso o [ ... ]
int l_IDirectFBVideoProvider_toudata (lua_State* L)
{
	// [ ... | IDirectFBVideoProvider* ]
	IDirectFBVideoProvider* vid = lua_touserdata(L, -1);
	if (vid == NULL)
		luaL_error(L, "Esperado lightuserdata representando IDirectFBVideoProvider* .");
	lua_pop(L, 1);            // [ ... ]

	IDirectFBVideoProvider** _vid = (IDirectFBVideoProvider**) lua_newuserdata(L, sizeof(IDirectFBVideoProvider*));
	*_vid = vid;              // [ ... | _vid ]
	luaL_getmetatable(L, "ldirectfb.IDirectFBVideoProvider");
                              // [ ... | _vid | mt ]
	lua_setmetatable(L, -2);  // [ ... | _vid ]
	return 1;
}

static int l_new (lua_State* L)
{
	// [ IDFBVideoProvider | dfb | filename ]
	IDirectFB* dfb = * (IDirectFB**) luaL_checkudata(L, 2, "ldirectfb.IDirectFB");
	IDirectFBVideoProvider* vid;
	DFBCHECK( dfb->CreateVideoProvider(dfb, luaL_checkstring(L, 3), &vid) );
	lua_pushlightuserdata(L, vid);        // [ IDFBVideoProvider | dfb | vid* ]
	l_IDirectFBVideoProvider_toudata(L);  // [ IDFBVideoProvider | dfb | vid ]
	return 1;
}

static int l_gc (lua_State* L)
{
	// [ vid ]
	IDirectFBVideoProvider* vid = * (IDirectFBVideoProvider**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBVideoProvider");
	lua_pushnil(L);                                // [ vid | nil ]
	lua_rawseti(L, LUA_REGISTRYINDEX, (int)vid);   // [ vid ]
	vid->Release(vid);
	return 0;
}

/*
struct s_cb {
	IDirectFBVideoProvider* vid;
	lua_State* L;
	int idx;
};

void callback (void* data)
{
	// [ ]
	struct s_cb* cb = (struct s_cb*) data;
	lua_rawgeti(cb->L, LUA_REGISTRYINDEX, cb->idx);   // [ callback ]
	if (lua_type(cb->L, -1) == LUA_TFUNCTION)
		lua_call(cb->L, 0, 0);                         // [ ]
	else
		lua_pop(cb->L, 1);

	DFBVideoProviderStatus status;
	cb->vid->GetStatus(cb->vid, &status);
	printf("Status: %d vs %d\n", status, DVSTATE_UNKNOWN);
	double pos, length;
	//cb->vid->GetPos(cb->vid, &pos);
	//cb->vid->GetLength(cb->vid, &length);
	//printf("Total: %lf vs %lf\n", pos, length);
}
*/

static int l_PlayTo (lua_State* L)
{
	// [ vid | sfc | trect | callback ]
	IDirectFBVideoProvider* vid = * (IDirectFBVideoProvider**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBVideoProvider");
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 2, "ldirectfb.IDirectFBSurface");

	/*
	struct s_cb* cb = (struct s_cb*) malloc(sizeof(struct s_cb));
	cb->vid = vid;
	cb->L   = L;
	cb->idx = (int) vid;

	lua_rawseti(L, LUA_REGISTRYINDEX, (int)vid);   // [ vid | sfc | trect ]
	DFBCHECK( vid->PlayTo(vid, sfc, NULL, callback, cb) )
	*/

	DFBCHECK( vid->PlayTo(vid, sfc, NULL, NULL, NULL) )
	return 0;
}

static int l_Stop (lua_State* L)
{
	// [ vid ]
	IDirectFBVideoProvider* vid = * (IDirectFBVideoProvider**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBVideoProvider");
	DFBCHECK( vid->Stop(vid) )
	return 0;
}

static int l_GetSurfaceDescription (lua_State* L)
{
	// [ vid ]
	IDirectFBVideoProvider* vid = * (IDirectFBVideoProvider**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBVideoProvider");
	DFBSurfaceDescription dsc;
	DFBCHECK (vid->GetSurfaceDescription(vid, &dsc));
	DFBSurfaceDescription2table(L, &dsc);  // [ vid | t_dsc ]
	return 1;
}

static int l_GetPos (lua_State* L)
{
	// [ vid ]
	IDirectFBVideoProvider* vid = * (IDirectFBVideoProvider**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBVideoProvider");
	double ret;
	DFBCHECK( vid->GetPos(vid, &ret) );
	lua_pushnumber(L, ret);   // [ vid | ret ]
	return 1;
}

static int l_GetLength (lua_State* L)
{
	// [ vid ]
	IDirectFBVideoProvider* vid = * (IDirectFBVideoProvider**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBVideoProvider");
	double ret;
	DFBCHECK( vid->GetLength(vid, &ret) );
	lua_pushnumber(L, ret);   // [ vid | ret ]
	return 1;
}

static int l_GetStatus (lua_State* L)
{
	// [ vid ]
	IDirectFBVideoProvider* vid = * (IDirectFBVideoProvider**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBVideoProvider");
 	DFBVideoProviderStatus ret;
	DFBCHECK( vid->GetStatus(vid, &ret) );
	lua_pushnumber(L, ret);   // [ vid | ret ]
	return 1;
}

static const struct luaL_Reg meths[] = {
	{ "__gc",                   l_gc        },
	{ "PlayTo",                 l_PlayTo    },
	{ "Stop",                   l_Stop      },
	{ "GetSurfaceDescription",  l_GetSurfaceDescription },
	{ "GetPos",                 l_GetPos    },
	{ "GetLength",              l_GetLength },
	{ "GetStatus",              l_GetStatus },
	{ NULL,                     NULL }
};

static const struct luaL_Reg funcs[] = {
	{ NULL, NULL }
};

LUALIB_API int luaopen_ldirectfb_IDirectFBVideoProvider (lua_State* L) {
	luaopen_module(L, meths, funcs, l_new);
	return 1;
}
