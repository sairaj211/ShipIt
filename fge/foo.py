import fge, sys
print("imported script")

def run():
    print(sys.path)
    print(fge.__file__)
    print(dir(fge))
    print("First test 2 - 1 = ", fge.sub(2,1))
    print(fge.hello())