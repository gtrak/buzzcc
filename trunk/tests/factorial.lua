function factorial(n)
  if n == 0.0 then
    local x = 0.0 + 1.0
    return x
  else
    return n * factorial(n - 1.0)
  end
end

print(factorial(3.0))

