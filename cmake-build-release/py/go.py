import os

for j in range(4):
    for i in range(200,4200,200):
        cpptest="./"+str(i) #in linux without suffix .exe
        if os.path.exists(cpptest):
            # data=f.readlines() #read the C++ printf or cout content
            # f.close()
            # print data
            print "python execute cpp program:"
            os.system(cpptest)
        else:
            print "Can't find"