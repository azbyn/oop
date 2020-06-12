import sys

res = 0
for c in sys.argv[1][::-1]:
    res = (res << 8) | ord(c)

print(res)
