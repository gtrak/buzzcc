#!/usr/bin/lua

function test_func()
	x = 1 + 2 + 3;
	y = 5 + 4 * 3;
	z = 5 * 4 * 3;

	-- THis shouldn't effect anything 
	for i = 1,10 do
		print("Testing the for loop " .. i );
	end
	--[=[ Neither should this ]=]

	table = {Batman="Hero", Joker="Villain", Superman="Asshat"}
	for k,v in pairs(table) do
		print(k,v)
	end
	for i=1,10,0.5 do
		print("Floating Point Looping:" .. i);
	end

	if true then 
		print("If");
	elseif true then
		print("Elseif");
	elseif true then
		print("Another Elseif");
	else
		print("Else");
	end
	
	string = [[ Here is a mult-line 
	string ]];

	return x, y, z
end


test_func() 
