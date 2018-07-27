-- 使用方法
-- 在需要运行的Lua文件头部加上
-- require("SublimeLuaCmd")
-- SublimeLuaCmd.run(debug.getinfo(function()end))

SublimeLuaCmd={}

function GetFileName(info)
	local ret=nil
	local prefix=nil
	for k,v in pairs(info) do
		if k=="source" then 
			--正则匹配文件名
			ret=string.match(v,".+\\(.*)%.lua")
			prefix=string.match(v,"@(.+)\\.+")
		end
	end
	return ret,prefix
end

function processLuaFile(file,time)
	user_str=file:read("*all")
	pattern="require%(\"SublimeLuaCmd.+debug.getinfo%(function%(%)end%)%)\n+"
	ret=string.gsub(user_str,pattern,"")
	str0="os.execute(\"pause\")\n"
	str1=string.format("os.remove(\"%s.lua\")\n",time)
	str2=string.format("os.remove(\"%s.bat\")\n",time)
	ret=ret..str0
	ret=ret..str1
	ret=ret..str2
	return ret 
end


function GenerateRunLuaFile()
	user_file=io.open(filename..".lua")
	t=processLuaFile(user_file,time)
	user_file:close()
	path=time..".lua"
	run_file=io.open(time..".lua","w")
	run_file:write(t)
	run_file:close()
end

function GenerateRunBatFile()
	run_file=io.open(time..".bat","w")
	cmd=string.format("start cmd /k lua %s",prefix.."\\"..time..".lua")
	run_file:write(cmd)
	run_file:close()
end

function SublimeLuaCmd.run(info)
	filename,prefix=GetFileName(info)
	print(prefix)
	time=string.gsub(os.time()," ","")
	GenerateRunLuaFile()
	GenerateRunBatFile()
	os.execute(time..".bat")
end

return SublimeLuaCmd

	