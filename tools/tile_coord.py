#!C:/Anaconda2/python.exe
import sys,math

def coord_rotation(x1,y1, x0,y0,angle):
    """坐标点x1,y1, 原点x0,y0, 选择角度angle"""
    sn = math.sin(math.radians(angle))
    cs = math.cos(math.radians(angle))
    x2 = (x1-x0)*cs - (y1-y0)*sn + x0
    y2 = (x1-x0)*sn + (y1-y0)*cs + y0    
    return x2, y2
def result(x1,y1, x0,y0,angle):
    x2, y2 = coord_rotation(x1, y1, x0, y0, angle)
    return scale(x2, y2, 3000, 1500)
    
def scale(x, y, view_width, view_height):
    """ """
    x1, offset_y = coord_rotation(1920, 0, 1920/2,1920/2, -45)
    offset_x, y1 = coord_rotation(0, 0, 1920/2,1920/2, -45)
    x += abs(offset_x)
    y += abs(offset_y)
    h = math.sqrt(1920**2+1920**2)
    w = h
    #w:x = view_width : new_x
    new_x = x*view_width / w
    new_y = y*view_height/ h
    return new_x, new_y
    
def scale2(x, y):
    h = math.sqrt(1920**2+1920**2)
    w = h
    #w:new_ = view_width : x
    new_x = w*x / 3000
    new_y = h*y / 1500
    x1, offset_y = coord_rotation(1920, 0, 1920/2,1920/2, -45)
    offset_x, y1 = coord_rotation(0, 0, 1920/2,1920/2, -45)
    new_x -= abs(offset_x)
    new_y -= abs(offset_y)
    return new_x, new_y

def view_to_tile(x,y):
    x1,y1 = scale2(x, y)
    return coord_rotation(x1, y1, 1920/2, 1920/2, 45)
        
def main(args):
    #BmpToPng(args[0])
    print coord_rotation(1920, 1920, 1920/2,1920/2, -45)
    print result(1920, 1920, 1920/2,1920/2, -45)
    print result(0, 1920, 1920/2,1920/2, -45)
    print result(960, 1920/2, 1920/2,1920/2, -45)
    x, y = result(393, 1920-576, 1920/2,1920/2, -45)
    print x, y
    print view_to_tile(x, y)
if __name__ == "__main__":
    args = sys.argv[1:]
    main(args)    