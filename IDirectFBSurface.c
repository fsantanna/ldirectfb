#include <unistd.h>
#include "ldirectfb.h"

#include <lauxlib.h>

// funcao STATIC (tb pode ser chamada internamente, por isso o [ ... ]
int l_IDirectFBSurface_toudata (lua_State* L)
{
	// [ ... | IDirectFBSurface* ]
	IDirectFBSurface* sfc = lua_touserdata(L, -1);
	if (sfc == NULL)
		luaL_error(L, "Esperado lightuserdata representando IDirectFBSurface* .");
	lua_pop(L, 1);            // [ ... ]

	IDirectFBSurface** _sfc = (IDirectFBSurface**) lua_newuserdata(L, sizeof(IDirectFBSurface*));
	*_sfc = sfc;              // [ ... | _sfc ]
	luaL_getmetatable(L, "ldirectfb.IDirectFBSurface");
                              // [ ... | _sfc | mt ]
	lua_setmetatable(L, -2);  // [ ... | _sfc ]
	return 1;
}

static int l_new (lua_State* L)
{
	// [ IDFBSurface | dfb | dsc ]
	IDirectFB* dfb = * (IDirectFB**) luaL_checkudata(L, 2, "ldirectfb.IDirectFB");
	DFBSurfaceDescription dsc;
	table2DFBSurfaceDescription (L, &dsc);
	IDirectFBSurface* sfc;
	//dsc.flags = dsc.flags & DSDESC_PIXELFORMAT;
	//dsc.pixelformat = DSPF_LUT8;
	//dsc.caps = DSCAPS_ALL;
	//printf("SFC: %d, %d\n", dsc.width, dsc.height);
	DFBCHECK( dfb->CreateSurface(dfb, &dsc, &sfc) );
	lua_pushlightuserdata(L, sfc);    // [ IDFBSurface | dfb | dsc | win ]
	l_IDirectFBSurface_toudata(L);    // [ IDFBSurface | dfb | dsc | _win ]
	return 1;
}

static int l_gc (lua_State* L)
{
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	sfc->Release(sfc);
	return 0;
}


static int l_GetSize (lua_State* L)
{
	// [ sfc ]
	int width, height;
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	DFBCHECK (sfc->GetSize (sfc, &width, &height));
	lua_pushnumber(L, width);   // [ sfc | width ]
	lua_pushnumber(L, height);  // [ sfc | width | height ]
	return 2;
}

static int l_SetColor (lua_State* L)
{
	// [ sfc | r | g | b | a ]
	DFBColor color;
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	DFBCHECK( sfc->SetColor(sfc,
	                        luaL_checkint(L, 2),       // r
							luaL_checkint(L, 3),       // g
							luaL_checkint(L, 4),       // b
							luaL_optint(L, 5, 255)) ); // a
	return 0;
}

static int l_SetFont (lua_State* L)
{
	// [ sfc | font ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	IDirectFBFont* font = * (IDirectFBFont**) luaL_checkudata(L, 2, "ldirectfb.IDirectFBFont");
	DFBCHECK (sfc->SetFont(sfc, font));
	return 0;
}

static int l_FillRectangle (lua_State* L)
{
	// [ sfc | x | y | w | h ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	DFBCHECK (sfc->FillRectangle(sfc, luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5)));
	return 0;
}

static int l_DrawRectangle (lua_State* L)
{
	// [ sfc | x | y | w | h ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	DFBCHECK (sfc->DrawRectangle(sfc, luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5)));
	return 0;
}

static int l_DrawLine (lua_State* L)
{
	// [ sfc | x1 | y1 | x2 | y2 ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	DFBCHECK( sfc->DrawLine(sfc, luaL_checknumber(L, 2),
	                             luaL_checknumber(L, 3),
	                             luaL_checknumber(L, 4),
	                             luaL_checknumber(L, 5)) )
	return 0;
}

static int l_DrawString (lua_State* L)
{
	// [ sfc | string | x | y | flags ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	size_t len;
	const char* str = luaL_checklstring(L, 2, &len);
	DFBCHECK (sfc->DrawString(sfc, str, len, luaL_checknumber(L,3), luaL_checknumber(L,4), (DFBSurfaceTextFlags)luaL_checknumber(L,5)));
	return 0;
}

/*
static int l_SetClip (lua_State* L)
{
	// [ sfc | t_region ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	DFBRegion region;
	table2DFBRegion(L, &region);
	DFBCHECK (sfc->SetClip(sfc, &region));
	return 0;
}
*/

static int l_SetBlittingFlags (lua_State* L)
{
	// [ sfc | flags ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	DFBCHECK( sfc->SetBlittingFlags(sfc, luaL_checknumber(L, 2)) );
	return 0;
}

static int l_Blit (lua_State* L)
{
	// [ sfc | src | rect | x | y ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	IDirectFBSurface* src = * (IDirectFBSurface**) luaL_checkudata(L, 2, "ldirectfb.IDirectFBSurface");
	int x = luaL_checknumber(L, 4);
	int y = luaL_checknumber(L, 5);
	DFBRectangle rect, *r = NULL;
	if (lua_istable(L, 3)) {
		lua_pushvalue(L, 3);           // [ sfc | rect | x | y | rect ]
		table2DFBRectangle(L, r);
		r = &rect;
	}

	DFBCHECK (sfc->Blit(sfc, src, r, x, y));
	return 0;
}

static int l_Flip (lua_State* L)
{
	// TEMP: NULL,0?
	// [ sfc ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	DFBCHECK (sfc->Flip (sfc, NULL, DSFLIP_WAITFORSYNC));
	//DFBCHECK (sfc->Flip (sfc, NULL, 0));
	//DFBCHECK (sfc->Flip (sfc, NULL, DSFLIP_BLIT));
	return 0;
}

static int l_GetSubSurface (lua_State* L)
{
	// [ sfc | t_rect ]
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBSurface");
	IDirectFBSurface* sub;
	DFBRectangle rect;
	table2DFBRectangle(L, &rect);
	DFBCHECK( sfc->GetSubSurface(sfc, &rect, &sub) );
	lua_pushlightuserdata(L, sub);    // [ sfc | t_rect | sub ]
	l_IDirectFBSurface_toudata(L);    // [ sfc | t_rect | _sub ]
	return 1;
}

static const struct luaL_Reg meths[] = {
	{ "__gc",             l_gc },
	{ "GetSize",          l_GetSize },
	{ "SetColor",         l_SetColor },
	{ "SetFont",          l_SetFont },
	{ "FillRectangle",    l_FillRectangle },
	{ "DrawRectangle",    l_DrawRectangle },
	{ "DrawLine",         l_DrawLine },
	{ "DrawString",       l_DrawString },
	//{ "SetClip",        l_SetClip },
	{ "Blit",             l_Blit },
	{ "SetBlittingFlags", l_SetBlittingFlags },
	{ "Flip",             l_Flip },
	{ "GetSubSurface",    l_GetSubSurface },
	{ NULL,               NULL }
};

static const struct luaL_Reg funcs[] = {
	//{ "__toudata",     l_IDirectFBSurface_toudata },
	{ NULL, NULL }
};

LUALIB_API int luaopen_ldirectfb_IDirectFBSurface (lua_State* L) {
	luaopen_module(L, meths, funcs, l_new);
	return 1;
}
