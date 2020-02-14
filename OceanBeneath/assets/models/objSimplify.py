#coding=utf-8
import sys
import os
from os import path


class Vertex:
    def __init__(self):
        self.pos=[]
        self.normal=[]
        self.texCoord=[]
    
    def __str__(self):
        return "pos:%r normal:%r texCoord:%r"%(self.pos,self.normal,self.texCoord)

def parseObj(lines):
    vertices=[]
    positions=[]
    normals=[]
    texCoords=[]
    for l in lines:
        tokens=l.split()
        if tokens[0]=='v':
            positions.append([float(x) for x in tokens[1::]])
        elif tokens[0]=='vn':
            normals.append([float(x) for x in tokens[1::]])
        elif tokens[0]=='vt':
            texCoords.append([float(x) for x in tokens[1:3:]])
        elif tokens[0]=='f':
            for ti in tokens[1::]:
                vi,tci,ni=ti.split("/")
                v=Vertex()
                v.pos=positions[int(vi)-1] if vi!='' else [0,0,0]
                v.normal=normals[int(ni)-1] if ni!='' else [0,0,0]
                v.texCoord=texCoords[int(tci)-1] if tci!='' else [0,0]
                vertices.append(v)
    return vertices

def foreach(l,op):
    for i in l:
        op(i)

def exportToCpp(fn,vs):
    with open(fn+".model","w") as f:
        for v in vs:
            foreach(v.pos,lambda x:f.write("%r "%x))
            foreach(v.normal,lambda x:f.write("%r "%x))
            foreach(v.texCoord,lambda x:f.write("%r "%x))
            f.write("\n")

if __name__=="__main__":
    '''
    f=sys.argv[1] if len(sys.argv)>1 else input("input obj file path:")
    lines=open(f).readlines()
    vs=parseObj(lines)
    ofn=path.basename(f).split(".")[0]
    exportToCpp(ofn,vs)
    '''
    curPath=path.abspath(".")
    for r,ds,fs in os.walk("."):
        if path.abspath(r)==curPath:
            for f in fs:
                fpath=os.path.join(r,f)
                pf=os.path.basename(fpath).split(".")[-1].lower()
                name=os.path.basename(fpath).split(".")[0].lower()
                if pf=="obj":
                    print("process file:"+fpath)
                    lines=open(fpath).readlines()
                    vs=parseObj(lines)
                    exportToCpp(name,vs)


