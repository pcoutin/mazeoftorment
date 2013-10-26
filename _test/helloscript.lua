print("THIS IS A TEST")
print(mt.lolo())

mt.magic()

-- factorialala
function fact (n)
   if n == 0 then
      return 1
   else
      return n * fact(n - 1)
   end
end

print("pls enter number: ")
a = io.read("*number")
print(fact(a))
