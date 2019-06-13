function f1(n)
	local x = n + 1
	return x
end

function f2(n)
	local x = f1(n) + 1
	return x
end

function f3(n)
	local x = f2(n) + 1
	return x
end


function factorial(n)
  if n == 0.0 then
    return 1.0
  else
    return n * factorial(n-1.0)
  end
end

i = 0
while (i < 10) do
	print("Iteration", i)
	if ((i % 2) == 0) then
		print("Even")
	else 
		print("Odd")
	end
	i = i + 1
end

function junk(a, b, c, d)
	print("float:", a)
	print("bool:", b)
	print("string:", c)
	print("integer:", d)	
end  


string = "this is a string"
int = 1
flt = 1.0 
bool = true 

print("Before function")
junk(flt,bool, string, int)
print("After function")

--[[
for i=1,10,2 do
	print("For Iteration", i)
end
--]]

-- factorial(3)
y = 1
z = 3.0
x = "string"
print(x)
print(y)
print(z)

print("")

do
	local x = 5
	print("local x", x)
end 
print("global x", x)

function inc(n)
	return n + 1
end
y = 1
print("Function Test")
print("Original value", y)
y = inc(y)
print("Final value", y)

for i=1.0,10.0 do
	print("factorial: ", i, " ", factorial(i));
end

print(f3(4 ^ 2))


