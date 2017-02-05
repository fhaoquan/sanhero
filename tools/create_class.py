#!C:/Anaconda2/python.exe
import sys

def create_class(name_class):
    f = open(name_class+'.h','w')
    m = '__'+str(name_class).upper() + '_H__'
    s = '#ifndef '+m+'\n'
    s += '#define '+m+'\n'
    s += '\n'
    s += '#endif'
    f.write(s)
    f.close()
    f = open(name_class+'.cpp', 'w')
    s = '#include "'+name_class+'.h"\n'
    f.write(s)
    f.close()
    
def main(args):
    #BmpToPng(args[0])
    create_class(args[0])
if __name__ == "__main__":
    args = sys.argv[1:]
    main(args)    