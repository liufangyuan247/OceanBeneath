from PIL import Image
from PIL import ImageFilter
import numpy
import struct

def convertToPng(name):
    d=open(name+".bin","rb").read()
    p=struct.unpack("%dB"%(1024*1024),d)
    p=numpy.resize(p,(1024,1024))


    im = Image.new('L', (1024,1024))
    im.putdata(p.flatten().tolist())
    im.save(name+".png")

    #return

    bigMat=numpy.resize(numpy.array(range(1028*1028)),(1028,1028))

    bigMat[2:1026,2:1026]=p
    bigMat[2:1026,0:2]=p[:,1022:1024]
    bigMat[2:1026,1026:1028]=p[:,0:2]
    bigMat[0:2,2:1026]=p[1022:1024,:]
    bigMat[1026:1028,2:1026]=p[0:2,:]


    im = Image.new('L', (1028,1028))
    im.putdata(bigMat.flatten().tolist())
    im=im.filter(ImageFilter.GaussianBlur(3))
    im=im.crop((2,2,1026,1026))
    im=im.resize((512,512))
    im.save(name+".png")

for i in range(1,40):
    convertToPng("%04d"%(i))