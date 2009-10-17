#include "directfb.h"

// funcao STATIC (tb pode ser chamada internamente, por isso o [ ... ]
int l_IDirectFBImageProvider_toudata (lua_State* L)
{
	// [ ... | img* ]
	IDirectFBImageProvider* img = lua_touserdata(L, -1);
	if (img == NULL)
		luaL_error(L, "Esperado lightuserdata representando IDirectFBImageProvider* .");
	lua_pop(L, 1);            // [ ... ]

	IDirectFBImageProvider** _img = (IDirectFBImageProvider**) lua_newuserdata(L, sizeof(IDirectFBSurface*));
	*_img = img;              // [ ... | _img ]
	luaL_getmetatable(L, "directfb.IDirectFBImageProvider");
                              // [ ... | _img | mt ]
	lua_setmetatable(L, -2);  // [ ... | _img ]
	return 1;
}

static int l_new (lua_State* L)
{
	// [ IDFBImageProvider | dfb | path ]
	IDirectFB* dfb = * (IDirectFB**) luaL_checkudata(L, 2, "directfb.IDirectFB");
	const char* path = luaL_checkstring(L, 3);
	IDirectFBImageProvider** img = (IDirectFBImageProvider**) lua_newuserdata(L, sizeof(IDirectFBImageProvider*));
                                  // [ IDFBImageProvider | dfb | path | img ]
	luaL_getmetatable(L, "directfb.IDirectFBImageProvider");
                                  // [ IDFBImageProvider | dfb | path | img | mt ]
	lua_setmetatable(L, -2);      // [ IDFBImageProvider | dfb | path | img ]
	DFBCHECK (dfb->CreateImageProvider (dfb, path, img));
	return 1;
}

static int l_gc (lua_State* L)
{
	IDirectFBImageProvider* img = * (IDirectFBImageProvider**) luaL_checkudata(L, 1, "directfb.IDirectFBImageProvider");
	img->Release(img);
	return 0;
}

static int l_GetSurfaceDescription (lua_State* L)
{
	// [ img ]
	IDirectFBImageProvider* img = * (IDirectFBImageProvider**) luaL_checkudata(L, 1, "directfb.IDirectFBImageProvider");
	DFBSurfaceDescription dsc;
	DFBCHECK (img->GetSurfaceDescription(img, &dsc));
	DFBSurfaceDescription2table(L, &dsc);  // [ img | t_dsc ]
	return 1;
}

static int l_RenderTo (lua_State* L)
{
	// [ img | sfc | x | y | dx | dy ]
	IDirectFBImageProvider* img = * (IDirectFBImageProvider**) luaL_checkudata(L, 1, "directfb.IDirectFBImageProvider");
	IDirectFBSurface* sfc = * (IDirectFBSurface**) luaL_checkudata(L, 2, "directfb.IDirectFBSurface");
	DFBRectangle* rect = NULL;
	DFBRectangle r;
	if (!lua_isnil(L, 3)) {
		rect = &r;
		r.x = luaL_checknumber(L, 3);
		r.y = luaL_checknumber(L, 4);
		r.w = luaL_checknumber(L, 5);
		r.h = luaL_checknumber(L, 6);
	}
	DFBCHECK( img->RenderTo(img, sfc, rect) );
	return 0;
}

static const struct luaL_Reg meths[] = {
	{ "__gc",                  l_gc },
	{ "GetSurfaceDescription", l_GetSurfaceDescription },
	{ "RenderTo",              l_RenderTo },
	{ NULL,                    NULL }
};

static const struct luaL_Reg funcs[] = {
	{ NULL, NULL }
};

LUALIB_API int luaopen_directfb_IDirectFBImageProvider (lua_State* L) {
	luaopen_module(L, meths, funcs, l_new);
	return 1;
}
