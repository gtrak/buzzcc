function factorial(n)
  if n == 0 then
    return 1
  else
    return n * factorial2(n - 1)
  end
end


function factorial2(n)
  if n == 0 then
    return 1
  else
    return n * factorial(n - 1)
  end
end


print (factorial(5))

