package.cpath = package.cpath .. ';/linux/Lua/?.so'
require 'ldirectfb'

local dfb = ldirectfb.init()
dfb:SetCooperativeLevel(ldirectfb.DFSCL_FULLSCREEN)

local dsc = {
	flags = ldirectfb.DSDESC_CAPS;
	caps  = ldirectfb.DSCAPS_PRIMARY + ldirectfb.DSCAPS_FLIPPING;
}
local sfc = dfb:CreateSurface(dsc)
local w, h = sfc:GetSize()

sfc:FillRectangle(0, 0, w, h)
sfc:SetColor(0x80, 0x80, 0xff, 0xff)
sfc:DrawLine(0, h/2, w-1, h/2)
sfc:Flip()

io.read()
