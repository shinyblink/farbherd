for i = 1, 5668 do
 local s = string.format("frames/%05i.png", i)
 os.execute("png2ff < " .. s .. " > " .. string.format("oframes/%05i.ff", i))
end
