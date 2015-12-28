require "lib.fs"

local ffi  = require "ffi"

local kr32 = ffi.load "kernel32"
local u32  = ffi.load "user32"

ffi.cdef "void *FindWindowA(const char *, const char *)"
ffi.cdef "bool  GetWindowThreadProcessId(void *, uint32_t *)"

ffi.cdef "void *OpenProcess(uint32_t, bool, uint32_t)"
ffi.cdef "void  CloseHandle(void *)"
ffi.cdef "void *GetModuleHandleA(const char *)"
ffi.cdef "void *GetProcAddress(void *, const char *)"
ffi.cdef "void *VirtualAllocEx(void *, void *, uint32_t, uint32_t, uint32_t)"
ffi.cdef "void  VirtualFreeEx(void *, void *, uint32_t, uint32_t)"
ffi.cdef "bool  WriteProcessMemory(void *, void *, const char *, uint32_t, uint32_t *)"
ffi.cdef "void *CreateRemoteThread(void *, void *, uint32_t, void *, void *, uint32_t, uint32_t *)"

do
	local nh = fs.getcwd() .. "/nh3.0.dll"

	local processid = ffi.new( "uint32_t[?]", 1 )
	u32.GetWindowThreadProcessId( u32.FindWindowA( "Valve001", nil ), processid )

	local game = ffi.gc( kr32.OpenProcess( 0x043a, false, processid[0] ), kr32.CloseHandle )

	if tonumber( ffi.cast( "intptr_t", game ) ) ~= 0 then
		local kr32_ptr = kr32.GetModuleHandleA( "kernel32.dll" )
		local lla_ptr  = kr32.GetProcAddress( kr32_ptr, "LoadLibraryA" )

		local pool = kr32.VirtualAllocEx( game, nil, 0x0100, 0x3000, 4 )

		kr32.WriteProcessMemory( game, pool, nh, nh:len(), nil )
		kr32.CreateRemoteThread( game, nil, 0, lla_ptr, pool, 0, nil )

		kr32.VirtualFreeEx( game, pool, 0, 0xc000 )
	end

	kr32.CloseHandle( ffi.gc( game, nil ) )
end