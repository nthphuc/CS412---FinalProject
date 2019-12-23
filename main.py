from random import seed
from random import gauss
from random import random
from cv2 import cv2 as cv2
import math 
import jsonpickle
import json
import numpy 
import shutil
import os

count = 0; 
def sift(img):
    gray= cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    gray = numpy.float32(gray)
    sift = cv2.xfeatures2d.SIFT_create()
    kp1, des1 = sift.detectAndCompute(img,None) 
    for i in range(0,len(des1)): 
        des1[i] /= 2.0; 
    return kp1,des1 

def keypointToFloatArray(kp):
    pts = kp.pt 
    return pts
class LSH: 
    listA = None; 
    B = None;  
    w = 4.0; 
    dumplist = None
    def init(self,dimension): 
        self.listA = numpy.zeros(dimension)
        self.dumplist = numpy.zeros(dimension+1) 
        self.B = random()*self.w
        self.dumplist[dimension] = self.B 
        for j in range(0,dimension): 
            self.listA[j] = gauss(-1,1)
            self.dumplist[j] = self.listA[j]
    
    def getHashValue(self,array):
        ret = 0
        dotProduct = numpy.dot(array,self.listA); 
        ret= math.floor((dotProduct + self.B)/self.w); 
        return ret; 
    def getDumpListArr(self): 
        return self.dumplist; 

    def fromDumplistArr(self,dimension,dumplist): 
        self.dumplist = dumplist; 
        self.listA = dumplist[:dimension]
        
        self.B = dumplist[dimension]
        global count 
        count +=1; 
#this is the basic g function
class HashingFunc: 
    listLSH = [];  
    hash_string = ""
    def init(self): 
        for j in range(0,3):
            lsh = LSH()
            lsh.init(128)
            self.listLSH.append(lsh); 
    
    def hash(self,array):
        result = numpy.empty(3) 
        self.hash_string  = ""; 
        for j in range(0,3): 
            hashVal = self.listLSH[j].getHashValue(array) 
            result[j] = hashVal
            self.hash_string += str(hashVal)
        return self.hash_string,result
    def toFile(self,filename):
        dumplist = numpy.zeros([3,129]) 
        for i in range(0,3): 
            dumplist[i] = self.listLSH[i].getDumpListArr()
        numpy.save(filename,dumplist)
        
    def fromFile(self,filename): 
        self.listLSH = []
        dumplist = numpy.load(filename)
        for i in range(0,3): 
            lsh = LSH()
            lsh.fromDumplistArr(128,dumplist[i])
            self.listLSH.append(lsh)
        # lines = text.split('\n'); 
        # for i in range(0,3): 
        #     funcLines = []
        #     for j in range(0,2): 
        #         funcLines.append(lines[i*j + j])
        #     lsh = LSH() 
        #     lsh.fromString(funcLines)
        #     self.listLSH.append(lsh)

#indeed, we use L HashFunc
class HashTable: 
    listHashFunc = []
    mapHashStringToBucketID = []
    bucketIDCount = [] 
    mapBucketIDToImageID = []
    def init(self):
        for i in range(0,15): 
            func = HashingFunc() 
            func.init(); 
            self.listHashFunc.append(func); 
            self.bucketIDCount.append(-1); 
            self.mapHashStringToBucketID.append({}); 
            self.mapBucketIDToImageID.append({}); 
    
    def putValue(self,array,imageID):
        for i in range(0,15): 
            hashString,hashVal = self.listHashFunc[i].hash(array); 
            if hashString not in self.mapHashStringToBucketID[i]: 
                self.bucketIDCount[i] +=1
                self.mapHashStringToBucketID[i][hashString] = self.bucketIDCount[i]; 
            bucketID = self.mapHashStringToBucketID[i][hashString]; 
            bucketID = str(bucketID)
            
            if bucketID not in self.mapBucketIDToImageID[i]:
                self.mapBucketIDToImageID[i][bucketID] = []
            # else:
            #     print("HASH INTO THE SAME BUCKET")
          
            self.mapBucketIDToImageID[i][bucketID].append(imageID); 

    
    def getValue(self,array): 
        ret = []
        mark = {}
        for i in range(0,15): 
            hashString,hashret = self.listHashFunc[i].hash(array); 
            # print(hashString) 
            if (hashString in self.mapHashStringToBucketID[i]): 
                bucketID = self.mapHashStringToBucketID[i][hashString]; 
                # print(bucketID) 
                bucketID = str(bucketID)
                # print(self.mapBucketIDToImageID[i])
                listImg = self.mapBucketIDToImageID[i][bucketID]
                for j in range(0,len(listImg)):
                    if listImg[j] not in mark: 
                        ret.append(listImg[j])
                        mark[listImg[j]] = True
        return ret

    def toFiles(self,folderName):
        #dump the hash functions to files
        for i in range(0,15):  

            path = folderName+'/' +str(i); 
            if not os.path.exists(path):
                os.makedirs(path)
            #write file
            filename = path+ '/' + 'hash_func'; 
            # f_hash_func = open(filename,'w'); 
            # # print (self.listHashFunc)
            # f_hash_func.write(self.listHashFunc[i].toString()); 
            self.listHashFunc[i].toFile(filename)

            fileName_mapHashStringToBucketID = path + '/mapHashStringToBucketID'+'.map'
            f_mapHashStringToBucketID = open(fileName_mapHashStringToBucketID,'w'); 
            f_mapHashStringToBucketID.write(json.dumps(self.mapHashStringToBucketID[i]))
            f_mapHashStringToBucketID.close()

            fileName_bucketIDCount = path+ '/bucketIDCount.map'
            f_bucketIDCount = open(fileName_bucketIDCount,'w'); 
            f_bucketIDCount.write(json.dumps(self.bucketIDCount[i]))
            f_bucketIDCount.close()

            fileName_mapBucketIDToImageID = path +'/imageID.list'; 
            f_imgid = open(fileName_mapBucketIDToImageID,'w');     
            json.dump(self.mapBucketIDToImageID[i],f_imgid)
            f_imgid.close()

          
    def fromFiles(self,folderName): 
        self.listHashFunc = []
        self.mapHashStringToBucketID = []
        self.bucketIDCount = [] 
        self.mapBucketIDToImageID = []
        for i in range(0,15):  

            path = folderName+'/' +str(i); 
            if not os.path.exists(path):
                os.makedirs(path)
            #write file
            filename = path+ '/' + 'hash_func.npy'; 
            h = HashingFunc()
            h.fromFile(filename)
            self.listHashFunc.append(h); 

            fileName_mapHashStringToBucketID = path + '/mapHashStringToBucketID'+'.map'
            with  open(fileName_mapHashStringToBucketID,'r') as f_mapHashStringToBucketID: 
                self.mapHashStringToBucketID.append(json.load(f_mapHashStringToBucketID)); 
            # f_mapHashStringToBucketID.write(json.dumps(self.mapHashStringToBucketID[i]))

            fileName_bucketIDCount = path+ '/bucketIDCount.map'
            with open(fileName_bucketIDCount,'r') as f_mapHashStringToBucketID:
                self.bucketIDCount.append(json.load(f_mapHashStringToBucketID)) 

            fileName_mapBucketIDToImageID = path +'/imageID.list'; 
            with open(fileName_mapBucketIDToImageID,'r') as f_imgid:     
                self.mapBucketIDToImageID.append(json.load(f_imgid))
          
def dumper(obj):
    try:
        return obj.toJSON()
    except:
        return obj.__dict__
def train(): 
    path = './data/train/' 
    global hashtable
    for folder in os.listdir(path): 
        for f in os.listdir(path + folder):  
            print("training " + path+folder+"/"+f)
            img = cv2.imread(path+folder+"/"+f); 
            kps,desss  = sift(img); 
            des_count = 0
            for i in desss: 
                des_count +=1 
                hashtable.putValue(i,folder); 
                if (des_count > 144):
                    break  

def query(filename): 
    img = cv2.imread(filename); 
    kps,desss = sift(img); 
    ret = {}
    des_count=0
    for des in desss: 
        # hashString,hashret = hashtable.listHashFunc[i].hash(desss[i]); 
        # print(hashString); 
        tag = hashtable.getValue(des)

        for t in tag: 
            ret[t]= True 
        if (des_count > 144):
            break 
    return ret.keys()

hashtable  = HashTable()          
def main():
    global hashtable
    
    if os.path.exists("trained_model"): 
        hashtable.fromFiles("trained_model")
    else:
        hashtable.init()
        train()
    hashtable.toFiles("trained_model")

    file = open("result1.txt", "w")
    for x in range(0, 270):
        testFile = "./data/test/" + str(x+1) + ".jpg"
        ret = query(testFile)
        for i in ret:
            file.write(i + " ")
        file.write("\n")
    file.close()    
    # hashtable.toFiles("trained_model") 
    # hashTable.fromFiles("trained_model")

    file = open("result1.txt", "r")
    result1 = []
    for x in file:
        row = x.split()
        result1.append(row)
    file.close()

    # Read result from CEDD
    file = open("result2.txt", "r")
    result2 = []
    for x in file:
        row = x.split()
        result2.append(row)
    file.close()

    # Join 2 result files to 1 result
    file = open("result.txt", "w")
    for i in range(0, 270):
        for j in range(0, len(result1[i])):
            if result1[i][j] in result2[i]:
                file.write(result1[i][j] + " ")
        file.write("\n")
    file.close()
main()