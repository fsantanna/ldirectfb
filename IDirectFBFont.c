#include "ldirectfb.h"

// funcao STATIC (tb pode ser chamada internamente, por isso o [ ... ]
int l_IDirectFBFont_toudata (lua_State* L)
{
	// [ ... | IDirectFBFont* ]
	IDirectFBFont* font = lua_touserdata(L, -1);
	if (font == NULL)
		luaL_error(L, "Esperado lightuserdata representando IDirectFBFont* .");
	lua_pop(L, 1);            // [ ... ]

	IDirectFBFont** _font = (IDirectFBFont**) lua_newuserdata(L, sizeof(IDirectFBFont*));
	*_font = font;            // [ ... | _font ]
	luaL_getmetatable(L, "ldirectfb.IDirectFBFont");
                              // [ ... | _font | mt ]
	lua_setmetatable(L, -2);  // [ ... | _font ]
	return 1;
}

static int l_new (lua_State* L)
{
	// [ IDFBFont | dfb | font_path | dsc ]
	IDirectFB* dfb = * (IDirectFB**) luaL_checkudata(L, 2, "ldirectfb.IDirectFB");
	const char* font_path = luaL_checkstring(L, 3);
	luaL_checktype(L, 4, LUA_TTABLE);
	DFBFontDescription dsc;
	table2DFBFontDescription(L, &dsc);
	IDirectFBFont** font = (IDirectFBFont**) lua_newuserdata(L, sizeof(IDirectFBFont*));
                                  // [ IDFBFont | dfb | font_path | dsc | font ]
	luaL_getmetatable(L, "ldirectfb.IDirectFBFont");
                                  // [ IDFBFont | dfb | font_path | dsc | font | mt ]
	lua_setmetatable(L, -2);      // [ IDFBFont | dfb | font_path | dsc | font ]
	DFBCHECK (dfb->CreateFont(dfb, font_path, &dsc, font));
	return 1;
}

static int l_gc (lua_State* L)
{
	IDirectFBFont* font = * (IDirectFBFont**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBFont");
	font->Release(font);
	return 0;
}

static int l_GetStringExtents (lua_State* L)
{
	// [ font | string ]
	IDirectFBFont* font = * (IDirectFBFont**) luaL_checkudata(L, 1, "ldirectfb.IDirectFBFont");
	DFBRectangle rect;
	// TEMP: qual dos dois, ou os dois?
	font->GetStringExtents(font, luaL_checkstring(L,2), -1, &rect, NULL);
	DFBRectangle2table(L, &rect);   // [ font | string | rect ]
	return 1;
}

static const struct luaL_Reg meths[] = {
	{ "__gc",                  l_gc },
	{ "GetStringExtents",      l_GetStringExtents },
	{ NULL,                    NULL }
};

static const struct luaL_Reg funcs[] = {
	{ NULL, NULL }
};

LUALIB_API int luaopen_ldirectfb_IDirectFBFont (lua_State* L) {
	luaopen_module(L, meths, funcs, l_new);
	return 1;
}
