from PIL import Image
import numpy as np

img=Image.open("bubbles.png")

data = np.asarray( img, dtype="uint8" )

alpha=data[:,:,3]

alpha.setflags(write=True)

print(data.shape)

class Island:
    def __init__(self):
        self.minX=10000
        self.minY=10000
        self.maxX=-10000
        self.maxY=-10000

    def add(self,x,y):
        self.minX=min(self.minX,x)
        self.minY=min(self.minY,y)
        self.maxX=max(self.maxX,x+1)
        self.maxY=max(self.maxY,y+1)

    def __str__(self):
        return "%d:%d-%d:%d"%(self.minX,self.minY,self.maxX,self.maxY)

    def size(self):
        return [self.maxX-self.minX,self.maxY-self.minY]

def checkAdjacent(island,alpha,q):
    while len(q):
        i,j=q.pop(0)
        if (not j in range(alpha.shape[0])) or (not i in range(alpha.shape[1])) or alpha[j,i]<128:
            continue
        
        island.add(i,j)
        alpha[j,i]=0
        q.append((i,j+1))
        q.append((i,j-1))
        q.append((i+1,j))
        q.append((i-1,j))

islands=[]

for i in range(alpha.shape[1]):
    for j in range(alpha.shape[0]):
        if alpha[j,i]>127:
            print("process island")
            island=Island()
            q=[(i,j),]
            checkAdjacent(island,alpha,q)
            islands.append(island)

maxSize=islands[0].size()

print("island count:",len(islands))

for i in islands:
    print("island:",i)
    s=i.size()
    maxSize[0]=max(maxSize[0],s[0])
    maxSize[1]=max(maxSize[1],s[1])

redChannel=data[:,:,0]


index=[0,1,7,13,25,19,2,8,14,26,20,3,9,27,15,21,4,10,28,16,22,5,29,11,17,23,30,6,12,18,24]

bigData=np.zeros((maxSize[1]*5,maxSize[0]*6))

for i in range(len(islands)):
    il=islands[i]
    iss=il.size()

    imgData=np.zeros((maxSize[1],maxSize[0]))

    ox=int((maxSize[0]-iss[0])/2)
    oy=int((maxSize[1]-iss[1])/2)

    imgData[oy:iss[1]+oy,ox:iss[0]+ox]=redChannel[il.minY:il.maxY,il.minX:il.maxX]

    if i>0:
        ri=int(index[i])
        row=int((ri-1)/6)
        col=(ri-1)%6
        bigData[row*maxSize[1]:(row+1)*maxSize[1],col*maxSize[0]:(col+1)*maxSize[0]]=imgData

    '''
    im = Image.new('L', maxSize)
    im.putdata(imgData.flatten().tolist())
    im.save("%04d.png"%index[i])
    '''

im = Image.new('L', (maxSize[0]*6,maxSize[1]*5))
im.putdata(bigData.flatten().tolist())
im.save("sheet.png")



    