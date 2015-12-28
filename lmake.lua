-- This script served preprocessing purposes for NanoHack 3.0

require "lib.sleep"
require "lib.bar"
require "lib.conioext"
require "lib.fs"
require "lib.str"
require "lib.tables"

local template =
{
	buildoptions =
	{
		"-std=c++11",

		"-msse2",
		"-mfpmath=sse",

		"-masm=intel",

		"-fno-asynchronous-unwind-tables",
		"-fno-exceptions",

		"-fno-ident"
	},
	linkoptions =
	{
		"-nostdlib",
		"-Wl,--exclude-libs,ALL",
		"-Wl,--version-script=../novis",
		"-e__Z5startv"
	},
	links =
	{
		"msvcrt",
		"kernel32",
	}
}

local args_meta =
{
	names = function( self )
		local t = { }

		for k, v in pairs( self ) do
			table.insert( t, v.name )
		end

		return t
	end,
	types = function( self )
		local t = { }

		for k, v in pairs( self ) do
			table.insert( t, v.type )
		end

		return t
	end
}

local function tmatch( text, pattern )
	local t = { }

	for m in text:gmatch( pattern ) do
		table.insert( t, m )
	end

	return t
end

pp =
{
	fileopen = function( _, contents ) return contents end,

	patterns =
	{
	},
	forceupdate =
	{
	},


	files =
	{
		onopen = function( onopen )
			pp.fileopen = onopen
		end,
		aliasupdate = function( name, alias )
			if not fs.isfile( "./lmake/" .. name ) or fs.stat( "./lmake/" .. name ).mtime < fs.stat( "./src/" .. alias ).mtime then
				table.insert( pp.forceupdate, name )
			end
		end,
		forceupdate = function( name )
			table.insert( pp.forceupdate, name )
		end
	},
	register =
	{
		declaration = function( word, cb )
			pp.patterns[ word .. "%(%s*(.-)%s*%)%s+(.-)%s*(%a[%a%d_]+)%(%s*(.-)%s*%)%s*;" ] = cb
		end,
		prefix = function( prefix, cb )
			pp.patterns[ prefix .. "\"(.-)\"" ] = cb
		end
	},
	parse =
	{
		args = function( args )
			local t = { }

			for a in args:gmatch "%s*([^,]+)%s*" do
				local type, name = a:match "(.-)%s*([%a%d_%:]+)$"
				table.insert( t, { type = type, name = name } )
			end


			local meta = getmetatable( t )

			setmetatable( t,
			{
				__index = function( a, b )
					if args_meta[ b ] then
						return args_meta[ b ]
					end

					return meta.__index( a, b )
				end
			} )

			return t
		end
	}
}


function compiler()
	if not buildfile then
		local fd = { }

		for _, v in next, fs.dir() do
			if fs.isfile( "./" .. v .. "/build.txt" ) then
				table.insert( fd, v )
			end
		end

		io.write "\navailable projects :\n"

		for k, v in pairs( fd ) do
			io.printf( "\t[%d] %s\n", k, v )
		end

		repeat
			local l = tonumber( io.getch() )
			if l and fd[l] then
				buildfile = fd[l]
			end
		until buildfile
	end

	fs.chdir( buildfile )

	local start = os.clock( )

	local ok, err = pcall( dofile, "build.txt" )
	if not ok then
		io.write( "[!] build.txt contains errors\n\n" )
		io.write( tostring( err ), "\n" )

		io.getch()

		return
	end

	if not fs.isdir "./lmake" then
		fs.mkdir "./lmake"
	end


	local uquene = { }

	for _, f in pairs( fs.matchfiles( "./src", ".+%.c[cxp]?[xp]?$", true ) ) do
		local t = f:gsub( "src/(.+)", "lmake/%1" )
		local u = false
		local ft, q, w = { }, { }, { }

		local base = f:match "(.+/).-%..-$"

		for s in io.lines( f ) do
			local i = s:match "#%s*include%s+\"(.-)\""
			if i then
				table.insert( ft, base .. i )
			end
		end

		while #ft > 0 do
			if fs.isfile( ft[1] ) then
				for s in io.lines( ft[1] ) do
					local i = s:match "#%s*include%s+\"(.-)\""

					if i and not table.foreach( w, function( _, v ) if v == i then return true end end ) then
						table.insert( ft, base .. i )
						table.insert( w, i )
					end
				end
			else
				return io.write( ft[1] .. ": included file could not be found" )
			end

			if not fs.isfile( ft[1]:gsub( "src/(.+)", "lmake/%1" ) ) or fs.stat( ft[1] ).mtime > fs.stat( ft[1]:gsub( "src/(.+)", "lmake/%1" ) ).mtime or
				table.foreach( pp.forceupdate, function( k, v ) if v == ft[1]:match "%./src/(.+)$" then return true end end ) then

				if not u then
					u = true
				end
			end

			if u and not table.foreach( uquene, function( _, v ) if v.f == ft[1] then return true end end ) then
				table.insert( uquene, { f = ft[1], t = ft[1]:gsub( "src/(.+)", "lmake/%1" ) } )
			end

			table.remove( ft, 1 )
		end

		if u or not fs.isfile( t ) or fs.stat( f ).mtime > fs.stat( t ).mtime or
			table.foreach( configurations, function( k ) if not fs.isfile( "./lmake/" .. k .. "/" .. f:match( "%./src/(.+)%..-$" ) .. ".o" ) then return true end end ) or
			table.foreach( pp.forceupdate, function( k, v ) if v == f:match "%./src/(.+)$" then return true end end ) then

			table.insert( uquene, { f = f, t = t } )
		end
	end

	if #uquene > 0 then
		local ut, ul = ( ' ' ):rep(4), 4


		io.write "source code update\n\n"
		bar.new( #uquene )

		for k, v in pairs( uquene ) do
			local r = io.open( v.f, "r" )
			if r then
				local w = io.open( v.t, "w" )
				local s = pp.fileopen( v.f:match ".+/(.+%.%a+)$", r:read "*all" )

				local raw = s:gsub( "//(.-)\n",
					function( str )
						return "//" .. str:gsub( "[^%s]", " " ) .. "\n"
					end ):gsub( "/%*(.-)%*/",
					function( str )
						return "/*" .. str:gsub( "[^%s]", " " ) .. "*/"
					end )


				local ll = { }

				for k, v in pairs( pp.patterns ) do
					if not ll[k] then
						ll[k] = { }
					end

					local li, le = 1
					repeat
						li, le = raw:find( k, li + 1 )

						if li then
							local ln = 1
							for _ in raw:sub( 1, le ):gmatch "\n" do
								ln = ln + 1
							end

							table.insert( ll[k], { li = li, ln = ln, le = le } )
						end
					until not li
				end


				local o = 0

				for k, v in pairs( pp.patterns ) do
					for _, l in pairs( ll[k] ) do
						local gc = ( v( s:sub( l.li - o - 1, l.le - o ):match( k ) ) or "" ) .. ( '\n' ):rep( #tmatch( ( s:sub( l.li - o - 1, l.le - o ) ) , "\n" ) )

						s = s:sub( 1, l.li - o - 1 ) .. gc .. s:sub( l.le - o + 1 )
						o = o + ( l.le - l.li + 1 ) - gc:len()
					end
				end

				w:write( s )

				w:close()
				r:close()
			end

			bar.inc()



			local s = v.f:match "src/(.-)$"

			if next( uquene, k ) then
				s = s .. ", "
			end

			if ( ul + s:len() ) % 8 ~= 0 then
				s = s .. ( ' ' ):rep( 8 - ( ul + s:len() ) % 8 )
			end

			if ( ul + s:len() ) > 79 then
				ul = 4
				ut = ut .. "\n" .. ( ' ' ):rep(4)
			end

			ut = ut .. s
			ul = ul +  s:len()
		end

		io.write( "\n\nfiles updated :\n", ut, "\n" )
	end


	io.write "\n\n:: build\nconfigurations\n\t"

	for s in pairs( configurations ) do
		io.write( s )
		io.write( next( configurations, s ) and ", " or "\n\n" )
	end

	local build = { }
	local ntime = os.time( )

	for k, v in pairs( configurations ) do
		if not fs.isdir( "./lmake/" .. k ) then
			fs.mkdir( "./lmake/" .. k )
		end

		for _, i in pairs( uquene ) do
			local c = i.t:match ".+/([%a%d_]+)%.c%a*"
			if c then
				local o = ( "./lmake/%s/%s" ):format( k, c ) .. ".o"
				local p = io.popen( ( "gcc -MMD -MP %s %s -o \"%s\" -c \"%s\" 2>&1" ):format( 
					( "-D%s " ):rep( #v.defines ):format( unpack( v.defines ) ), table.concat( buildoptions, " " ), o, i.t ) )

				table.insert( build, { p = p, o = o, t = "" } )
			end
		end
	end


	local cr =
	{
		files = { },
		errlg = { },
		error = false,

		linker = { }
	}

	if #build > 0 then
		io.write "  compiling\n"
		bar.new( #build )

		repeat
			for k, v in pairs( build ) do
				if not cr.files[ v.o ] then
					cr.files[ v.o ] = { }
				end


				if fs.isfile( v.o ) and fs.stat( v.o ).mtime >= ntime then
					table.insert( cr.files[ v.o ], ( v.p:read( "*all" ):gsub( "%./lmake/", "%./src/" ) ) )
					table.remove( build, k )

					io.close( v.p )
					bar.inc()


					local g, name = v.o:match "%./lmake/(.+)/(.-%.o)$"
					if not cr.linker[g] then
						cr.linker[g] = { }
					end

					table.insert( cr.linker[g], name )
				else
					local l = v.p:read "*line"

					if l then
						if l:match "%./lmake/.-:%d+:%d+: error: " then
							cr.error = true
							cr.errlg[ v.o ] = true
						end

						table.insert( cr.files[ v.o ], ( l:gsub( "%./lmake/", "%./src/" ) ) )
					else
						table.remove( build, k )

						io.close( v.p )
						bar.inc()
					end
				end
			end
		until #build == 0

		io.write "\n\n"
	end

	if cr.error then
		for v in pairs( cr.errlg ) do
			io.bell()

			io.write( "\n\n", string.rep( ' ', 22 ), v, "\n\n" )

			for k, v in pairs( cr.files[ v ] ) do
				io.write( v, '\n' )
			end

			io.write " Press * key to continue\n\n"
			io.getch()
		end

		io.getch()
	else
		io.write "  linking\n"

		--local a = 0
		--table.foreach( configurations, function() i = i + 1 end )

		bar.new( table.size( configurations ) )


		local files = fs.matchfiles( "./src", ".+%.c[cxp]?[xp]?$", true )

		if not fs.isdir "./build" then
			fs.mkdir "./build"
		end

		for k, v in pairs( configurations ) do
			if not fs.isdir( "./build/" .. k ) then
				fs.mkdir( "./build/" .. k )
			end

			local t = { }

			for _, v in pairs( files ) do
				table.insert( t, v:match ".+/(.+)%.c[cxp]?[xp]?$" )
			end

			local o = ( "./lmake/" .. k .. "/%s.o " ):rep( #t ):format( unpack( t ) )
			local l = ( ( "-l%s " ):rep( #links ) ):format( unpack( links ) ) .. ( "-l%s " ):rep( #v.links ):format( unpack( v.links ) )

			local ld = io.popen( ( "gcc -o ./build/%s/%s%s %s -s %s %s %s 2>&1" ):format( k, buildfile, extension, o, shared and "-shared" or "", l, table.concat( linkoptions, " " ) ) ):read "*all"

			if ld:len() > 0 then
				io.write "something's wrong?\n"
				io.write( ld, '\n' )
			else
				if pp.on_done then
					if not pp.on_done( fs.getcwd() .. "/build/%s" % k, fs.getcwd() .. "/build/%s/%s%s" % { k, buildfile, extension } ) then
						io.write "failure returned by on_done()\n"
					end
				end
			end

			print( ld )

			bar.inc()
		end

		io.write( "\n\n\tcompilation done in : " .. ( os.clock() - start ) .. " seconds\n" )
		sleep( 3 )
	end
end

do
	io.write "create new project ? "

	if buildfile or not io.q() then -- todo: autobuild file
		return compiler()
	end

	local name, shared
	local conf = { }

	io.write "\n:: creation of new project\n"

	io.write "project name : "
	name = io.read( )

	io.write "make shared ? "
	shared = io.q( )

	io.write "build configurations : "

	for s in io.read( ):gmatch "(%w+)" do
		conf[s] =
		{
			defines		= { },
			links		= { }
		}

		for k, v in pairs( conf[s] ) do
			local l = k .. " @ " .. s
			io.write( l .. ( ' ' ):rep( 15 - l:len() ) .. " : " )

			for i in io.read( ):gmatch( "(%w+)" ) do
				table.insert( v, i )
			end
		end
	end

	local showname = ( '\"' .. name .. "\"" .. ( shared and " (shared object)" or "" ) )

	io.write( "\n:: summary" .. ( ' ' ):rep( 89 - 11 - showname:len() ) .. showname .. "\n" )

	for k, v in pairs( conf ) do
		io.write( ( ' ' ):rep( 10 - k:len() ) .. k .. " : \n" )

		for k, v in pairs( v ) do
			io.write( ( ' ' ):rep( 20 - k:len() ) .. k .. " : " )

			for _, s in pairs( v ) do
				io.write( s .. " " )
			end

			io.write "\n"
		end

		io.write "\n"
	end

	io.write "\n\nconfirm : "

	if io.q() then
		fs.mkdir( name )
		fs.mkdir( name .. "/src" )

		local f = io.open( name .. "/build.txt", "w" )

		if f then
			f:write "extension = \"\"\n\n"

			if shared then
				f:write "shared = true\n\n\n"
			end

			for k, t in pairs( template ) do
				f:write( k, " =\n{\n" )

				for _, v in pairs( t ) do
					f:write( "\t\"" .. v .. "\",\n" )
				end

				f:write "}\n\n"
			end

			f:write "\n\n"
			f:flush( )


			f:write "configurations =\n{\n"

			for k, v in pairs( conf ) do
				f:write( '\t', k, " =\n\t{\n" )

				for s, t in pairs( v ) do
					f:write( ( "\t\t%s =\n\t\t{\n" ):format( s ) )

					for _, v in pairs( t ) do
						f:write( ( "\t\t\t\"%s\",\n" ):format( v ) )
					end

					f:write( ( "\t\t},\n" ):format( s ) )
				end

				f:write "\t},\n"
				f:flush( )
			end

			f:write "}\n\n\n\n"
			f:close( )
		end
	end
end