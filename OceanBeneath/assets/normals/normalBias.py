from PIL import Image
import numpy as np


def getAverageNormal(img):
    arr=np.array(img)
    size=img.size[0]*img.size[1]
    sum=np.array([0.0,0.0,0.0])
    for l in arr:
        for p in l:
            sum=sum+p[0:3:]
    sum=sum/(size*255)
    return sum*2-np.array([1,1,1])


if __name__ == "__main__":
    an=np.array([0.0,0.0,0.0])
    c=0
    for i in range(1,40):
        img=Image.open("%04d.png"%i)
        n=getAverageNormal(img)
        print(n)
        an=an+n
        c+=1
    print("bias:")
    print(an/c)