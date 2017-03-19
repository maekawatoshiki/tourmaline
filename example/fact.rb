# alternative for loop stmt like 'for' or 'while'
def loop(n int)
  if n > 0
    loop(n - 1)
    puts("fact(%d) = %d", n, fact(n))
  end
end

def fact(n int) int
  if n == 1
    1
  else
    fact(n - 1) * n
  end
end

loop(9)
