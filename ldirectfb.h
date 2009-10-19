#include <directfb.h>
#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>

void dump (lua_State* L, char* point);
int DFBRectangle2table (lua_State* L, DFBRectangle* rect);
void table2DFBRectangle (lua_State* L, DFBRectangle* rect);
int DFBRegion2table (lua_State* L, DFBRegion* rect);
void table2DFBRegion (lua_State* L, DFBRegion* rect);
int DFBColor2table (lua_State* L, DFBColor* color);
void table2DFBColor (lua_State* L, DFBColor* color);

#define DFBCHECK(x...)                                         \
  {                                                            \
    DFBResult err = x;                                         \
                                                               \
    if (err != DFB_OK)                                         \
      {                                                        \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
        DirectFBErrorFatal( #x, err );                         \
      }                                                        \
  }
