/* TEMP
 * - Em vez de dar require's de dependencias nos luaopen
 *   pq nao abrir todas as libs antes?
 * - l_toudata p/ todos
 */

#include <sys/time.h>
#include "directfb.h"
#include <errno.h>

void l_dump (lua_State* L, char* point)
{
	int i;
	int top = lua_gettop(L);
	printf("Ponto %s: ", point);
	for (i=1; i<=top; i++)
	{
		int t = lua_type(L, i);
		switch (t)
		{
			case LUA_TSTRING:
				printf("'%s'", lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				printf("%g", lua_tonumber(L, i));
				break;
			default:
				printf("%s (%p)", lua_typename(L, t), lua_topointer(L, i));
				break;
		}
		printf(" ");
	}
	printf("\n");
}

int l_gettimeofday (lua_State* L)
{
	// [ ]
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0)
		luaL_error(L, "%s", strerror(errno));
	lua_pushnumber(L, tv.tv_sec);  // [ tv_sec ]
	lua_pushnumber(L, tv.tv_usec); // [ tv_sec | tv_usec ]
	return 2;
}

int DFBRectangle2table (lua_State* L, DFBRectangle* rect)
{
	// [ ]
	lua_newtable(L);               // [ table ]
	lua_pushnumber(L, rect->x);    // [ table | x ]
	lua_setfield(L, -2, "x");      // [ table ]
	lua_pushnumber(L, rect->y);    // [ table | y ]
	lua_setfield(L, -2, "y");      // [ table ]
	lua_pushnumber(L, rect->w);    // [ table | w ]
	lua_setfield(L, -2, "w");      // [ table ]
	lua_pushnumber(L, rect->h);    // [ table | h ]
	lua_setfield(L, -2, "h");      // [ table ]
	return 1;
}
void table2DFBRectangle (lua_State* L, DFBRectangle* rect)
{
	// [ ... | rect ]
	int idx = lua_gettop(L);
	lua_getfield(L, idx, "x");      // [ rect | x ]
	rect->x = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "y");      // [ rect | x | y ]
	rect->y = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "w");      // [ rect | x | y | w ]
	rect->w = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "h");      // [ rect | x | y | w | h ]
	rect->h = luaL_checknumber(L, -1);
	lua_settop(L, idx);             // [ rect ]
}

int DFBRegion2table (lua_State* L, DFBRegion* region)
{
	// [ ]
	lua_newtable(L);               // [ table ]
	lua_pushnumber(L, region->x1); // [ table | x1 ]
	lua_setfield(L, -2, "x1");     // [ table ]
	lua_pushnumber(L, region->y1); // [ table | y1 ]
	lua_setfield(L, -2, "y1");     // [ table ]
	lua_pushnumber(L, region->x2); // [ table | x2 ]
	lua_setfield(L, -2, "x2");     // [ table ]
	lua_pushnumber(L, region->y2); // [ table | y2 ]
	lua_setfield(L, -2, "y2");     // [ table ]
	return 1;
}
void table2DFBRegion (lua_State* L, DFBRegion* region)
{
	// [ region ]
	int idx = lua_gettop(L);
	lua_getfield(L, idx, "x1");     // [ region | x1 ]
	region->x1 = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "y1");     // [ region | x1 | y1 ]
	region->y1 = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "x2");     // [ region | x1 | y1 | x2 ]
	region->x2 = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "y2");     // [ region | x1 | y1 | x2 | y2 ]
	region->y2 = luaL_checknumber(L, -1);
	lua_settop(L, idx);             // [ region ]
}

int DFBColor2table (lua_State* L, DFBColor* color)
{
	// [ ]
	lua_newtable(L);               // [ table ]
	lua_pushnumber(L, color->r);   // [ table | r ]
	lua_setfield(L, -2, "r");      // [ table ]
	lua_pushnumber(L, color->g);   // [ table | g ]
	lua_setfield(L, -2, "g");      // [ table ]
	lua_pushnumber(L, color->b);   // [ table | b ]
	lua_setfield(L, -2, "b");      // [ table ]
	lua_pushnumber(L, color->a);   // [ table | a ]
	lua_setfield(L, -2, "a");      // [ table ]
	return 1;
}
void table2DFBColor (lua_State* L, DFBColor* color)
{
	// [ color ]
	int idx = lua_gettop(L);
	lua_getfield(L, idx, "r");      // [ color | r ]
	color->r = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "g");      // [ color | r | g ]
	color->g = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "b");      // [ color | r | g | b ]
	color->b = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "a");      // [ color | r | g | b | a ]
	color->a = luaL_optint(L, -1, 0xFF); // default 255 -> opaco
	lua_settop(L, idx);             // [ color ]
}

	// DFBUserEvent
int DFBUserEvent2table (lua_State* L, DFBUserEvent* evt)
{
	// [ ]
	lua_newtable(L);                     // [ table ]
	lua_pushnumber(L, evt->clazz);       // [ table | clazz ]
	lua_setfield(L, -2, "clazz");        // [ table ]
	lua_pushnumber(L, evt->type);        // [ table | type ]
	lua_setfield(L, -2, "type");         // [ table ]
	// pega o valor "data" que esta no registry na posicao guardada por evt->data
	lua_rawgeti(L, LUA_REGISTRYINDEX, (int)evt->data); // [ table | data ]
	luaL_unref(L, LUA_REGISTRYINDEX, (int)evt->data);
	lua_setfield(L, -2, "data");         // [ table ]
	return 1;
}
void table2DFBUserEvent (lua_State* L, DFBUserEvent* evt)
{
	// assume que a tabela evt esta no topo da pilha
	// [ ... | evt ]
	int idx = lua_gettop(L);

	lua_getfield(L, idx, "clazz");        // [ evt | clazz ]
	evt->clazz = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "type");         // [ evt | clazz | type ]
	evt->type = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "data");         // [ evt | flags | type | data ]
	// guarda o valor "data" no registry e coloca o inteiro de referencia em evt->data
	evt->data = (void*) luaL_ref(L, LUA_REGISTRYINDEX); // [ evt | flags | type ]
	lua_settop(L, idx);                   // [ evt ]
}

int DFBWindowDescription2table (lua_State* L, DFBWindowDescription* dsc)
{
	// [ ]
	lua_newtable(L);                     // [ table ]
	lua_pushnumber(L, dsc->flags);       // [ table | flags ]
	lua_setfield(L, -2, "flags");        // [ table ]
	lua_pushnumber(L, dsc->caps);        // [ table | caps ]
	lua_setfield(L, -2, "caps");         // [ table ]
	lua_pushnumber(L, dsc->width);       // [ table | width ]
	lua_setfield(L, -2, "width");        // [ table ]
	lua_pushnumber(L, dsc->height);      // [ table | height ]
	lua_setfield(L, -2, "height");       // [ table ]
	lua_pushnumber(L, dsc->pixelformat); // [ table | pixelformat ]
	lua_setfield(L, -2, "pixelformat");  // [ table ]
	lua_pushnumber(L, dsc->posx);        // [ table | posx ]
	lua_setfield(L, -2, "posx");         // [ table ]
	lua_pushnumber(L, dsc->posy);        // [ table | posy ]
	lua_setfield(L, -2, "posy");         // [ table ]

	// TEMP
 	//DFBSurfaceCapabilities 	surface_caps; 	pixel format
	return 1;
}
void table2DFBWindowDescription (lua_State* L, DFBWindowDescription* dsc)
{
	// assume que a tabela dsc esta no topo da pilha
	// [ ... | dsc ]
	int idx = lua_gettop(L);

	lua_getfield(L, idx, "flags");        // [ dsc | flags ]
	dsc->flags = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "caps");         // [ dsc | flags | caps ]
	dsc->caps = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "width");        // [ dsc | flags | caps | width ]
	dsc->width = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "height");       // [ dsc | flags | caps | width | height ]
	dsc->height = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "pixelformat");  // [ dsc | flags | caps | width | height | pixelformat ]
	dsc->pixelformat = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "posx");         // [ dsc | flags | caps | width | height | pixelformat | posx ]
	dsc->posx = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "posy");         // [ dsc | flags | caps | width | height | pixelformat | posx | posy ]
	dsc->posy = luaL_optint(L, -1, 0);

	// TEMP
 	//DFBSurfaceCapabilities 	surface_caps; 	pixel format
	lua_settop(L, idx);                   // [ dsc ]
}

int DFBSurfaceDescription2table (lua_State* L, DFBSurfaceDescription* dsc)
{
	// [ ]
	lua_newtable(L);               // [ table ]
	lua_pushnumber(L, dsc->flags); // [ table | flags ]
	lua_setfield(L, -2, "flags");  // [ table ]
	lua_pushnumber(L, dsc->caps);  // [ table | caps ]
	lua_setfield(L, -2, "caps");   // [ table ]
	lua_pushnumber(L, dsc->width); // [ table | width ]
	lua_setfield(L, -2, "width");  // [ table ]
	lua_pushnumber(L, dsc->height);// [ table | height ]
	lua_setfield(L, -2, "height"); // [ table ]
	lua_pushnumber(L, dsc->pixelformat);// [ table | pixelformat ]
	lua_setfield(L, -2, "pixelformat"); // [ table ]

	/*
	void* pt;
	int pt_size;
	// inclui o campo preallocated as is
	pt_size = sizeof(dsc->preallocated);
	pt_size = 16; // TEMP: calculo acima nao esta correto
	pt = lua_newuserdata(L, pt_size);     // [ table | pt ]
	memcpy(pt, dsc->preallocated, pt_size);
	lua_setfield(L, -2, "preallocated");  // [ table ]

	// inclui o campo palette as is
	pt_size = sizeof(dsc->palette);
	pt_size = 8; // TEMP: calculo acima nao esta correto
	pt = lua_newuserdata(L, pt_size);     // [ table | pt ]
	memcpy(pt, &dsc->palette, pt_size);
	lua_setfield(L, -2, "palette");       // [ table ]
	*/

	return 1;
}
void table2DFBSurfaceDescription (lua_State* L, DFBSurfaceDescription* dsc)
{
	// assume que a tabela dsc esta no topo da pilha
	// [ ... | dsc ]
	int idx = lua_gettop(L);

	lua_getfield(L, idx, "flags");        // [ dsc | flags ]
	dsc->flags = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "caps");         // [ dsc | flags | caps ]
	dsc->caps = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "width");        // [ dsc | flags | caps | width ]
	dsc->width = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "height");       // [ dsc | flags | caps | width | height ]
	dsc->height = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "pixelformat");  // [ dsc | flags | caps | width | height | pixelformat ]
	dsc->pixelformat = luaL_optint(L, -1, 0);

	/*
	void* pt;
	// restaura o campo preallocated as is
	lua_getfield(L, idx, "preallocated"); // [ dsc | flags | caps | width | height | pixelformat | preallocated ]
	if (pt = lua_touserdata(L, -1))
		memcpy(dsc->preallocated, pt, 16); // TEMP sizeof(dsc->preallocated));: calculo acima nao esta correto

	// restaura o campo palette as is
	lua_getfield(L, idx, "palette");     // [ dsc | flags | caps | width | height | pixelformat | preallocated | palette ]
	if (pt = lua_touserdata(L, -1))
		memcpy(&dsc->palette, pt, 8); // TEMP sizeof(dsc->palette));: calculo acima nao esta correto
		*/

	lua_settop(L, idx);                   // [ dsc ]
}

int DFBFontDescription2table (lua_State* L, DFBFontDescription* desc)
{
	// [ ]
	lua_newtable(L);                        // [ table ]
	lua_pushnumber(L, desc->flags);         // [ table | flags ]
	lua_setfield(L, -2, "flags");           // [ table ]
	lua_pushnumber(L, desc->attributes);    // [ table | attributes ]
	lua_setfield(L, -2, "attributes");      // [ table ]
	lua_pushnumber(L, desc->height);        // [ table | height ]
	lua_setfield(L, -2, "height");          // [ table ]
	lua_pushnumber(L, desc->width);         // [ table | width ]
	lua_setfield(L, -2, "width");           // [ table ]
	lua_pushnumber(L, desc->index);         // [ table | index ]
	lua_setfield(L, -2, "index");           // [ table ]
	lua_pushnumber(L, desc->fixed_advance); // [ table | fixed_advance ]
	lua_setfield(L, -2, "fixed_advance");   // [ table ]
	lua_pushnumber(L, desc->fract_height);  // [ table | fract_height ]
	lua_setfield(L, -2, "fract_height");    // [ table ]
	lua_pushnumber(L, desc->fract_width);   // [ table | fract_width ]
	lua_setfield(L, -2, "fract_width");     // [ table ]
	return 1;
}
void table2DFBFontDescription (lua_State* L, DFBFontDescription* desc)
{
	// [ ... | desc ]
	int idx = lua_gettop(L);
	lua_getfield(L, idx, "flags");         // [ desc | flags ]
	desc->flags = luaL_checknumber(L, -1);
	lua_getfield(L, idx, "attributes");    // [ desc | flags | attributes ]
	desc->attributes = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "height");        // [ desc | flags | attributes | height ]
	desc->height = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "width");         // [ desc | flags | attributes | height | width ]
	desc->width = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "index");         // [ desc | flags | attributes | height | width | index ]
	desc->index = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "fixed_advance"); // [ desc | flags | attributes | height | width | index | fx_adv ]
	desc->fixed_advance = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "fract_height");  // [ desc | flags | attributes | height | width | index | fx_adv | fr_h ]
	desc->fract_height = luaL_optint(L, -1, 0);
	lua_getfield(L, idx, "fract_width");   // [ desc | flags | attributes | height | width | index | fx_adv | fr_h | fr_w ]
	desc->fract_width = luaL_optint(L, -1, 0);
	lua_settop(L, idx);                    // [ desc ]
}

/*
 DFBInputEvent
 	DFBEventClass 	clazz; 	clazz of event
 	DFBInputEventType 	type; 	type of event
 	DFBInputDeviceID 	device_id; 	source of event
 	DFBInputEventFlags 	flags; 	which optional fields are valid?
 	struct timeval 	timestamp; 	time of event creation
 	int 	key_code; 	hardware keycode, no mapping, -1 if device doesn't differentiate between several keys
 	DFBInputDeviceKeyIdentifier 	key_id; 	basic mapping, modifier independent
 	DFBInputDeviceKeySymbol 	key_symbol; 	advanced mapping, unicode compatible, modifier dependent
 	DFBInputDeviceModifierMask 	modifiers; 	pressed modifiers (optional)
 	DFBInputDeviceLockState 	locks; 	active locks (optional)
 	DFBInputDeviceButtonIdentifier 	button; 	in case of a button event
 	DFBInputDeviceButtonMask 	buttons; 	mask of currently pressed buttons
 	DFBInputDeviceAxisIdentifier 	axis; 	in case of an axis event
 	int 	axisabs; 	absolute mouse/ joystick coordinate
 	int 	axisrel; 	relative mouse/ joystick movement
	*/
// TEMP: falta a volta e mapear todos os campos
int DFBInputEvent2table (lua_State* L, DFBInputEvent* evt)
{
	// [ ]
	lua_newtable(L);                     // [ table ]
	lua_pushnumber(L, evt->clazz);       // [ table | class ]
	lua_setfield(L, -2, "clazz");        // [ table ]
	lua_pushnumber(L, evt->type);        // [ table | type ]
	lua_setfield(L, -2, "type");         // [ table ]
	lua_pushnumber(L, evt->key_id);      // [ table | key_id ]
	lua_setfield(L, -2, "key_id");       // [ table ]
	lua_pushnumber(L, evt->key_symbol);  // [ table | key_symbol ]
	lua_setfield(L, -2, "key_symbol");   // [ table ]
	lua_pushnumber(L, evt->axis);        // [ table | axis ]
	lua_setfield(L, -2, "axis");         // [ table ]
	lua_pushnumber(L, evt->axisabs);     // [ table | axis ]
	lua_setfield(L, -2, "axisabs");      // [ table ]
	lua_pushnumber(L, evt->button);      // [ table | axis ]
	lua_setfield(L, -2, "button");       // [ table ]
	return 1;
}

int l_init (lua_State* L)
{
	lua_getfield(L, LUA_ENVIRONINDEX, "dfb");     // [ dfb ]
/*
	char* argv[] = { "./df_andi", "--dfb:system=sdl" };
	char** oi = argv;
	int argc = 2;
*/

	if lua_isnil(L, -1) {
		lua_pop(L, 1);                            // [ ]
        //DFBCHECK( DirectFBInit(&argc, &oi) );
        DFBCHECK( DirectFBInit(NULL, NULL) );
		CreateInstance(L, "directfb.IDirectFB");  // [ dfb ]
		IDirectFB** dfb = (IDirectFB**) lua_touserdata(L, -1);
        //DFBCHECK( (*dfb)->SetCooperativeLevel (*dfb, DFSCL_FULLSCREEN) );
		lua_pushvalue(L, -1);                     // [ dfb | dfb ]
		lua_setfield(L, LUA_ENVIRONINDEX, "dfb"); // [ dfb ]
	}

	return 1;
}

// TEMP unificar todas essas funcoes assim como os construtores das classes
int CreateInstance (lua_State* L, char* module)
{
	// [ ... ]
	int n = lua_gettop(L);
	lua_getglobal(L, "require");
	lua_pushstring(L, module);
	                      // [ ... | require | "module" ]
	lua_call(L, 1, 1);    // [ ... | _M ]
	lua_insert(L, 1);     // [ _M | ... ]
 	// chama o construtor do modulo (__call --> l_new)
	lua_call(L, n, 1);  // [ instance ]
	return 1;
}

LUALIB_API int luaopen_module (lua_State* L, const struct luaL_Reg* meths, const struct luaL_Reg* funcs, lua_CFunction l_new)
{
	// [ "module" ]
	const char* module = luaL_checkstring(L, -1);
	// cria a metatable padrao para o udata do modulo
	// mt = { __index = mt }
	luaL_newmetatable(L, module);         // [ "module" | mt ]
	lua_pushvalue(L, -1);                 // [ "module" | mt | mt ]
	lua_setfield(L, -2, "__index");       // [ "module" | mt ]
	luaL_register(L, NULL, meths);        // [ "module" | mt ]
	lua_pop(L, 1);                        // [ "module" ]

	// setmetatable(_M, {__call = l_new})
	luaL_register(L, module, funcs);      // [ "module" | _M ]
	lua_newtable(L);                      // [ "module" | _M | mt2 ]
	lua_pushcfunction(L, l_new);          // [ "module" | _M | mt2 | l_new ]
	lua_setfield(L, -2, "__call");        // [ "module" | _M | mt2 ]
	lua_setmetatable(L, -2);              // [ "module" | _M ]
	lua_remove(L, -2);					  // [ _M ]
	
	// [ _M ]
	return 1;
}

static const struct luaL_Reg directfb_funcs[] = {
	{ "init",         l_init },
	{ "gettimeofday", l_gettimeofday },
	{ NULL, NULL }
};

struct field_value {
	char* field;
	int   value;
};

	// DFBVideoProviderStatus
static const struct field_value sDFBVideoProviderStatus[] = {
 	{ "DVSTATE_UNKNOWN",   DVSTATE_UNKNOWN   },
 	{ "DVSTATE_PLAY",      DVSTATE_PLAY      },
 	{ "DVSTATE_STOP",      DVSTATE_STOP      },
 	{ "DVSTATE_FINISHED",  DVSTATE_FINISHED  },
 	{ "DVSTATE_BUFFERING", DVSTATE_BUFFERING },
	{ NULL, 0 }
};


	// DFBSurfaceDescriptionFlags
static const struct field_value sDFBSurfaceDescriptionFlags[] = {
	{ "DSDESC_NONE",         DSDESC_NONE },
	{ "DSDESC_CAPS",         DSDESC_CAPS },
	{ "DSDESC_WIDTH",        DSDESC_WIDTH },
	{ "DSDESC_HEIGHT",       DSDESC_HEIGHT },
	{ "DSDESC_PIXELFORMAT",  DSDESC_PIXELFORMAT },
	{ "DSDESC_PREALLOCATED", DSDESC_PREALLOCATED },
	{ "DSDESC_PALETTE",      DSDESC_PALETTE },
	{ "DSDESC_ALL",          DSDESC_ALL },
	{ NULL, 0 }
};

	// DFBSurfaceCapabilities
static const struct field_value sDFBSurfaceCapabilities[] = {
	{ "DSCAPS_NONE",        DSCAPS_NONE },
	{ "DSCAPS_PRIMARY",     DSCAPS_PRIMARY },
	{ "DSCAPS_SYSTEMONLY",  DSCAPS_SYSTEMONLY },
	{ "DSCAPS_VIDEOONLY",   DSCAPS_VIDEOONLY },
	{ "DSCAPS_DOUBLE",      DSCAPS_DOUBLE },
	{ "DSCAPS_SUBSURFACE",  DSCAPS_SUBSURFACE },
	{ "DSCAPS_INTERLACED",  DSCAPS_INTERLACED },
	{ "DSCAPS_SEPARATED",   DSCAPS_SEPARATED },
	{ "DSCAPS_STATIC_ALLOC",DSCAPS_STATIC_ALLOC },
	{ "DSCAPS_TRIPLE",      DSCAPS_TRIPLE },
	{ "DSCAPS_PREMULTIPLIED",DSCAPS_PREMULTIPLIED },
	{ "DSCAPS_DEPTH",       DSCAPS_DEPTH },
	{ "DSCAPS_FLIPPING",    DSCAPS_FLIPPING },  // TEMP: isso ta em um exemplo mas nao na API doc
	{ "DSCAPS_ALL",         DSCAPS_ALL },
	{ NULL, 0 }
};

	// DFBSurfaceBlittingFlags
static const struct field_value sDFBSurfaceBlittingFlags[] = {
 	{ "DSBLIT_NOFX",               DSBLIT_NOFX },
 	{ "DSBLIT_BLEND_ALPHACHANNEL", DSBLIT_BLEND_ALPHACHANNEL },
 	{ "DSBLIT_BLEND_COLORALPHA",   DSBLIT_BLEND_COLORALPHA },
 	{ "DSBLIT_COLORIZE",           DSBLIT_COLORIZE },
 	{ "DSBLIT_SRC_COLORKEY",       DSBLIT_SRC_COLORKEY },
 	{ "DSBLIT_DST_COLORKEY",       DSBLIT_DST_COLORKEY },
 	{ "DSBLIT_SRC_PREMULTIPLY",    DSBLIT_SRC_PREMULTIPLY },
 	{ "DSBLIT_DST_PREMULTIPLY",    DSBLIT_DST_PREMULTIPLY },
 	{ "DSBLIT_DEMULTIPLY",         DSBLIT_DEMULTIPLY },
 	{ "DSBLIT_DEINTERLACE",        DSBLIT_DEINTERLACE },
 	{ "DSBLIT_SRC_PREMULTCOLOR",   DSBLIT_SRC_PREMULTCOLOR },
 	{ "DSBLIT_XOR",                DSBLIT_XOR },
 	{ "DSBLIT_INDEX_TRANSLATION",  DSBLIT_INDEX_TRANSLATION }
};

	// DFBInputDeviceKeySymbol
static const struct field_value sDFBInputDeviceKeySymbol[] = {
	{ "DIKS_0", DIKS_0 }, { "DIKS_1", DIKS_1 }, { "DIKS_2", DIKS_2 }, { "DIKS_3", DIKS_3 },
	{ "DIKS_4", DIKS_4 }, { "DIKS_5", DIKS_5 }, { "DIKS_6", DIKS_6 }, { "DIKS_7", DIKS_7 },
   	{ "DIKS_8", DIKS_8 }, { "DIKS_9", DIKS_9 },

	{ "DIKS_SMALL_A", DIKS_SMALL_A }, { "DIKS_SMALL_B", DIKS_SMALL_B }, { "DIKS_SMALL_C", DIKS_SMALL_C },
	{ "DIKS_SMALL_D", DIKS_SMALL_D }, { "DIKS_SMALL_E", DIKS_SMALL_E }, { "DIKS_SMALL_F", DIKS_SMALL_F },
	{ "DIKS_SMALL_G", DIKS_SMALL_G }, { "DIKS_SMALL_H", DIKS_SMALL_H }, { "DIKS_SMALL_I", DIKS_SMALL_I },
	{ "DIKS_SMALL_J", DIKS_SMALL_J }, { "DIKS_SMALL_K", DIKS_SMALL_K }, { "DIKS_SMALL_L", DIKS_SMALL_L },
	{ "DIKS_SMALL_M", DIKS_SMALL_M }, { "DIKS_SMALL_N", DIKS_SMALL_N }, { "DIKS_SMALL_O", DIKS_SMALL_O },
	{ "DIKS_SMALL_P", DIKS_SMALL_P }, { "DIKS_SMALL_Q", DIKS_SMALL_Q }, { "DIKS_SMALL_R", DIKS_SMALL_R },
	{ "DIKS_SMALL_S", DIKS_SMALL_S }, { "DIKS_SMALL_T", DIKS_SMALL_T }, { "DIKS_SMALL_U", DIKS_SMALL_U },
	{ "DIKS_SMALL_V", DIKS_SMALL_V }, { "DIKS_SMALL_W", DIKS_SMALL_W }, { "DIKS_SMALL_X", DIKS_SMALL_X },
	{ "DIKS_SMALL_Y", DIKS_SMALL_Y }, { "DIKS_SMALL_Z", DIKS_SMALL_Z },

	{ "DIKS_CAPITAL_A", DIKS_CAPITAL_A }, { "DIKS_CAPITAL_B", DIKS_CAPITAL_B }, { "DIKS_CAPITAL_C", DIKS_CAPITAL_C },
	{ "DIKS_CAPITAL_D", DIKS_CAPITAL_D }, { "DIKS_CAPITAL_E", DIKS_CAPITAL_E }, { "DIKS_CAPITAL_F", DIKS_CAPITAL_F },
	{ "DIKS_CAPITAL_G", DIKS_CAPITAL_G }, { "DIKS_CAPITAL_H", DIKS_CAPITAL_H }, { "DIKS_CAPITAL_I", DIKS_CAPITAL_I },
	{ "DIKS_CAPITAL_J", DIKS_CAPITAL_J }, { "DIKS_CAPITAL_K", DIKS_CAPITAL_K }, { "DIKS_CAPITAL_L", DIKS_CAPITAL_L },
	{ "DIKS_CAPITAL_M", DIKS_CAPITAL_M }, { "DIKS_CAPITAL_N", DIKS_CAPITAL_N }, { "DIKS_CAPITAL_O", DIKS_CAPITAL_O },
	{ "DIKS_CAPITAL_P", DIKS_CAPITAL_P }, { "DIKS_CAPITAL_Q", DIKS_CAPITAL_Q }, { "DIKS_CAPITAL_R", DIKS_CAPITAL_R },
	{ "DIKS_CAPITAL_S", DIKS_CAPITAL_S }, { "DIKS_CAPITAL_T", DIKS_CAPITAL_T }, { "DIKS_CAPITAL_U", DIKS_CAPITAL_U },
	{ "DIKS_CAPITAL_V", DIKS_CAPITAL_V }, { "DIKS_CAPITAL_W", DIKS_CAPITAL_W }, { "DIKS_CAPITAL_X", DIKS_CAPITAL_X },
	{ "DIKS_CAPITAL_Y", DIKS_CAPITAL_Y }, { "DIKS_CAPITAL_Z", DIKS_CAPITAL_Z },

	{ "DIKS_BACKSPACE", DIKS_BACKSPACE }, { "DIKS_RETURN", DIKS_RETURN },
	{ "DIKS_ESCAPE", DIKS_ESCAPE }, { "DIKS_SPACE", DIKS_SPACE },
 	{ "DIKS_TAB", DIKS_TAB },

 	{ "DIKS_EXCLAMATION_MARK", DIKS_EXCLAMATION_MARK }, { "DIKS_QUOTATION", DIKS_QUOTATION },
	{ "DIKS_ASTERISK",  DIKS_ASTERISK  }, { "DIKS_NUMBER_SIGN", DIKS_NUMBER_SIGN },
	{ "DIKS_PARENTHESIS_LEFT", DIKS_PARENTHESIS_LEFT }, { "DIKS_PARENTHESIS_RIGHT", DIKS_PARENTHESIS_RIGHT },
	{ "DIKS_PLUS_SIGN", DIKS_PLUS_SIGN }, { "DIKS_MINUS_SIGN",  DIKS_MINUS_SIGN  },
 	{ "DIKS_DOLLAR_SIGN", DIKS_DOLLAR_SIGN }, { "DIKS_PERCENT_SIGN", DIKS_PERCENT_SIGN },
 	{ "DIKS_AMPERSAND", DIKS_AMPERSAND }, { "DIKS_APOSTROPHE", DIKS_APOSTROPHE },
 	{ "DIKS_ASTERISK", DIKS_ASTERISK }, { "DIKS_COMMA", DIKS_COMMA },
 	{ "DIKS_PERIOD", DIKS_PERIOD }, { "DIKS_SLASH", DIKS_SLASH },

	{ "DIKS_COLON",     DIKS_COLON     }, { "DIKS_LESS_THAN_SIGN", DIKS_LESS_THAN_SIGN },
	{ "DIKS_SEMICOLON", DIKS_SEMICOLON }, { "DIKS_EQUALS_SIGN",    DIKS_EQUALS_SIGN    },
	{ "DIKS_GREATER_THAN_SIGN", DIKS_GREATER_THAN_SIGN }, { "DIKS_QUESTION_MARK", DIKS_QUESTION_MARK },
 	{ "DIKS_AT", DIKS_AT },

	{ "DIKS_SQUARE_BRACKET_LEFT", DIKS_SQUARE_BRACKET_LEFT }, { "DIKS_SQUARE_BRACKET_RIGHT", DIKS_SQUARE_BRACKET_RIGHT },
	{ "DIKS_CURLY_BRACKET_LEFT", DIKS_CURLY_BRACKET_LEFT }, { "DIKS_CURLY_BRACKET_RIGHT", DIKS_CURLY_BRACKET_RIGHT },
 	{ "DIKS_BACKSLASH", DIKS_BACKSLASH }, { "DIKS_CIRCUMFLEX_ACCENT", DIKS_CIRCUMFLEX_ACCENT },
 	{ "DIKS_GRAVE_ACCENT", DIKS_GRAVE_ACCENT }, { "DIKS_UNDERSCORE", DIKS_UNDERSCORE },
 	{ "DIKS_VERTICAL_BAR", DIKS_VERTICAL_BAR }, { "DIKS_TILDE", DIKS_TILDE },
	{ "DIKS_DELETE", DIKS_DELETE }, { "DIKS_ENTER", DIKS_ENTER },
	{ "DIKS_CURSOR_LEFT", DIKS_CURSOR_LEFT }, { "DIKS_CURSOR_RIGHT", DIKS_CURSOR_RIGHT },
	{ "DIKS_CURSOR_UP", DIKS_CURSOR_UP }, { "DIKS_CURSOR_DOWN", DIKS_CURSOR_DOWN },

 	{ "DIKS_INSERT", DIKS_INSERT }, { "DIKS_HOME", DIKS_HOME }, { "DIKS_END", DIKS_END },
 	{ "DIKS_PAGE_UP", DIKS_PAGE_UP }, { "DIKS_PAGE_DOWN", DIKS_PAGE_DOWN },

	{ "DIKS_F1", DIKS_F1 }, { "DIKS_F2", DIKS_F2 }, { "DIKS_F3", DIKS_F3 }, { "DIKS_F4", DIKS_F4 },
 	{ "DIKS_F5", DIKS_F5 }, { "DIKS_F6", DIKS_F6 }, { "DIKS_F7", DIKS_F7 }, { "DIKS_F8", DIKS_F8 },
 	{ "DIKS_F9", DIKS_F9 }, { "DIKS_F10", DIKS_F10 }, { "DIKS_F11", DIKS_F11 }, { "DIKS_F12", DIKS_F12 },

	/*
 	DIKS_PRINT 	DIKS_SHIFT 	DIKS_CONTROL 	DIKS_ALT 	
 	DIKS_ALTGR 	DIKS_CAPS_LOCK 	DIKS_NUM_LOCK 	DIKS_SCROLL_LOCK 	
	*/

	{ NULL, 0 }
};

	// DFBEventClass
static const struct field_value sDFBEventClass[] = {
	{ "DFEC_NONE", DFEC_NONE },
	{ "DFEC_INPUT", DFEC_INPUT },
	{ "DFEC_WINDOW", DFEC_WINDOW },
	{ "DFEC_USER", DFEC_USER },
	{ "DFEC_UNIVERSAL", DFEC_UNIVERSAL },
	{ NULL, 0 }
};

	// DFBInputEventType
static const struct field_value sDFBInputEventType[] = {
	{ "DIET_UNKNOWN", DIET_UNKNOWN },
	{ "DIET_KEYPRESS", DIET_KEYPRESS },
	{ "DIET_KEYRELEASE", DIET_KEYRELEASE },
	{ "DIET_BUTTONPRESS", DIET_BUTTONPRESS },
	{ "DIET_BUTTONRELEASE", DIET_BUTTONRELEASE },
	{ "DIET_AXISMOTION", DIET_AXISMOTION },
	{ NULL, 0 }
};

	// DFBWindowDescriptionFlags
static const struct field_value sDFBWindowDescriptionFlags[] = {
	{ "DWDESC_CAPS",         DWDESC_CAPS },
	{ "DWDESC_WIDTH",        DWDESC_WIDTH },
	{ "DWDESC_HEIGHT",       DWDESC_HEIGHT },
	{ "DWDESC_PIXELFORMAT",  DWDESC_PIXELFORMAT },
	{ "DWDESC_POSX",         DWDESC_POSX },
	{ "DWDESC_POSY",         DWDESC_POSY },
	{ "DWDESC_SURFACE_CAPS", DWDESC_SURFACE_CAPS },
	{ NULL, 0 }
};

	// DFBCooperativeLevel
static const struct field_value sDFBCooperativeLevel[] = {
	{ "DFSCL_NORMAL",       DFSCL_NORMAL },
	{ "DFSCL_FULLSCREEN",   DFSCL_FULLSCREEN },
	{ "DFSCL_EXCLUSIVE",    DFSCL_EXCLUSIVE },
	{ NULL, 0 }
};

	// DFBFontDescriptionFlags
static const struct field_value sDFBFontDescriptionFlags[] = {
	{ "DFDESC_ATTRIBUTES",   DFDESC_ATTRIBUTES },
	{ "DFDESC_HEIGHT",       DFDESC_HEIGHT },
	{ "DFDESC_WIDTH",        DFDESC_WIDTH },
	{ "DFDESC_INDEX",        DFDESC_INDEX },
	{ "DFDESC_FIXEDADVANCE", DFDESC_FIXEDADVANCE },
	{ "DFDESC_FRACT_HEIGHT", DFDESC_FRACT_HEIGHT },
	{ "DFDESC_FRACT_WIDTH",  DFDESC_FRACT_WIDTH },
	{ NULL, 0 }
};

	// DFBFontAttributes
static const struct field_value sDFBFontAttributes[] = {
	{ "DFFA_NONE",       DFFA_NONE },
	{ "DFFA_NOKERNING",  DFFA_NOKERNING },
	{ "DFFA_NOHINTING",  DFFA_NOHINTING },
	{ "DFFA_MONOCHROME", DFFA_MONOCHROME },
	{ "DFFA_NOCHARMAP",  DFFA_NOCHARMAP },
	{ NULL, 0 }
};

	// DFBSurfaceTextFlags
static const struct field_value sDFBSurfaceTextFlags[] = {
	{ "DSTF_LEFT",          DSTF_LEFT },
	{ "DSTF_CENTER",        DSTF_CENTER },
	{ "DSTF_RIGHT",         DSTF_RIGHT },
	{ "DSTF_TOP",           DSTF_TOP },
	{ "DSTF_BOTTOM",        DSTF_BOTTOM },
	{ NULL, 0 }
};

static const struct field_value sDFBInputDeviceCapabilities[] = {
	{ "DICAPS_KEYS",        DICAPS_KEYS },
	{ "DICAPS_AXES",        DICAPS_AXES },
	{ "DICAPS_BUTTONS",     DICAPS_BUTTONS },
	{ "DICAPS_ALL",         DICAPS_ALL },
	{ NULL, 0 }
};

static void register_enum (lua_State* L, char* name, const struct field_value* s)
{
	// assume que a tabela module esta no topo da pilha
	// [ _M ]

	// tabela de enums
	//lua_newtable(L);              // [ _M | table ]

	for (; s->field; s++) {
		lua_pushnumber(L, s->value);   // [ _M | num ]
		lua_setfield(L, -2, s->field); // [ _M ]
	}

	//lua_setfield(L, -2, name);    // [ _M ]
}

static void register_enums (lua_State* L)
{
	// assume que a tabela module esta no topo da pilha
	// [ _M ]
	register_enum(L, "DFBVideoProviderStatus",     sDFBVideoProviderStatus);
	register_enum(L, "DFBSurfaceDescriptionFlags", sDFBSurfaceDescriptionFlags);
	register_enum(L, "DFBSurfaceCapabilities",     sDFBSurfaceCapabilities);
	register_enum(L, "sDFBSurfaceBlittingFlags",   sDFBSurfaceBlittingFlags);
	register_enum(L, "DFBCooperativeLevel",        sDFBCooperativeLevel);
	register_enum(L, "DFBSurfaceTextFlags",        sDFBSurfaceTextFlags);
	register_enum(L, "DFBWindowDescriptionFlags",  sDFBWindowDescriptionFlags);
	register_enum(L, "DFBInputDeviceCapabilities", sDFBInputDeviceCapabilities);
	register_enum(L, "DFBFontDescriptionFlags",    sDFBFontDescriptionFlags);
	register_enum(L, "DFBFontAttributes",          sDFBFontAttributes);
	register_enum(L, "DFBInputDeviceKeySymbol",    sDFBInputDeviceKeySymbol);
	register_enum(L, "DFBInputEventType",          sDFBInputEventType);
	register_enum(L, "DFBEventClass",              sDFBEventClass);
}

LUALIB_API int luaopen_directfb (lua_State* L)
{
	// env = {}
	lua_newtable(L);                        // [ env ]
	lua_replace(L, LUA_ENVIRONINDEX);       // [ ]

	luaL_register(L, "directfb", directfb_funcs);
	register_enums(L);
	return 1;
}
