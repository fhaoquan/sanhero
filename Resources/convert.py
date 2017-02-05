#coding:utf8
from PIL import Image
import sys, os
def BmpToPng(f):

    img = Image.open(f)
    file_out = os.path.basename(f).split('.')[0]+'.png' 
    img.save(file_out)
    #print file_out
def PngToCur(f):
    img = Image.open(f)
    file_out = os.path.basename(f).split('.')[0]+'.cur' 
    img.save(file_out)
    
def main(args):
    #BmpToPng(args[0])
    PngToCur(args[0])
if __name__ == "__main__":
    args = sys.argv[1:]
    main(args)