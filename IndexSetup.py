# -*- coding: utf-8 -*-

import subprocess
import numpy as np
import sys, os
from os import listdir
from os.path import isfile
import json
from time import sleep

import chardet
from Text2Speech import SpeechConvertor
from mfcc import MFCC

PATH = '/media/billy/New Volume/Billy/audio/'
MP3_PATH = '/media/billy/New Volume/Billy/quires/'
QUERY_PATH = '/media/billy/New Volume/Test/queries/'

class StreamAudioIndex:
    def __init__(self):
        print 'Start Building Indexing'
        self.test = SpeechConvertor()
        self.featureLoader = MFCC()

        self.program = "./cmake-build-release/StreamAudioIndex"
        # print program
        self.cpp_proc = subprocess.Popen([self.program], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        self.flag = 1

    def __del__(self):
        self.cpp_proc.kill()

    def searchFromScreen(self):
        data = raw_input("Please enter the words you want to search(use ',' to divide): ")
        self.search(data)

    def searchForTest(self):
        global queryTestPho
        allfiles = [ f for f in listdir(QUERY_PATH) if isfile(QUERY_PATH+f)]
        for file in allfiles:
            try:
                f = open(QUERY_PATH+file, 'r')
                queryTestStr = f.readline().strip('\n')
                queryStr = queryTestStr
                Phones = []
                while queryTestStr:
                    data = f.readline().strip('\n').split(',')
                    # data = data.remove('')
                    queryTestPho = np.array([float(i) for i in data[:-1]]).astype('float32')
                    Phones.append(queryTestPho)
                    queryTestStr = f.readline().strip('\n')
                    queryStr += ',' + queryTestStr

                while self.flag:
                    buff = self.cpp_proc.stdout.readline()
                    if buff == "000This is end000\n":
                        print "Index Building Finished!"
                        self.flag = 0

                title = queryStr.rstrip(',').decode(sys.stdin.encoding).encode('utf-8')
                # print chardet.detect(title)
                dataLen = len(title)
                self.cpp_proc.stdin.write(format(dataLen, '08x'))
                self.cpp_proc.stdin.write(title)

                pause = "#!#"
                begin = "!#!"

                for pho in Phones:
                    count = 0
                    for num in pho:
                        if count == 0:
                            self.cpp_proc.stdin.write(begin)
                        count += 1
                        if count >= 13:
                            count = 0
                        self.cpp_proc.stdin.write(num)

                self.cpp_proc.stdin.write(pause)

                with open('test_for_query.txt', 'a') as f:
                    results = []
                    Time = []
                    overlap = 0
                    while True:
                        buff = self.cpp_proc.stdout.readline()
                        if buff == "000This is end000\n":
                            break
                        # print buff.strip('\n')
                        # 搜索id,查看结果重合程度
                        if buff.find("id： ") != -1:
                            tmp = buff.lstrip('id： ').rstrip('\n')
                            id = int(tmp)
                            if id in results:
                                overlap += 1
                            else:
                                results.append(id)
                        #搜索Time,查看搜索时间
                        elif buff.find("Time: ") != -1:
                            time = float(buff.lstrip('Time: ').rstrip('e+09s\n'))
                            Time.append(time)
                    if len(Time) >= 2:
                        f.write("%d,%d,%f,%f\n"%(overlap, len(results)+overlap, Time[0], Time[1]))
                        print("%d,%d,%f,%f\n"%(overlap, len(results)+overlap, Time[0], Time[1]))
                    else:
                        f.write("%d,%d\n"%(overlap, len(results)+overlap))
                        print("%d,%d\n"%(overlap, len(results)+overlap))
            except Exception, e:
                print e
                continue

    def search(self, data):
        try:
            # print chardet.detect(data)
            Quires = data.split(',')
            Phones = []
            for quiry in Quires:
                filename = self.test.convertText(quiry)
                java_proc = subprocess.Popen("java -jar cmake-build-debug/MP3ToWav.jar " + filename + '.mp3',
                                             shell=True)
                java_proc.wait()
                os.remove(MP3_PATH + filename + '.mp3')
                sourcePool = self.featureLoader.extractwav(PATH + filename + '.wav')
                sourceCoeff = np.array(sourcePool["coefficents"])
                sourceEnergy = np.array(sourcePool["energy"])
                mean = self.featureLoader.find_phonome(sourceCoeff, sourceEnergy)
                Phones.append(mean.astype('float32'))
                os.remove(PATH + filename + '.wav')

            while self.flag:
                buff = self.cpp_proc.stdout.readline()
                # print buff
                if buff == "000This is end000\n":
                    print "Index Building Finished!"
                    self.flag = 0

            title = data.decode(sys.stdin.encoding).encode('utf-8')
            dataLen = len(title)
            self.cpp_proc.stdin.write(format(dataLen, '08x'))
            self.cpp_proc.stdin.write(title)

            pause = "#!#"
            begin = "!#!"

            for pho in Phones:
                count = 0
                for num in pho:
                    if count == 0:
                        self.cpp_proc.stdin.write(begin)
                    count += 1
                    if count >= 13:
                        count = 0
                    self.cpp_proc.stdin.write(num)

            self.cpp_proc.stdin.write(pause)

            resultText = ""
            resultPho = ""
            isText = 0
            while True:
                buff = self.cpp_proc.stdout.readline()
                if buff == "000This is end000\n":
                    break
                if buff == "The result of Text Indexes:\n":
                    isText = 1
                elif buff == "The result of Phoneme Indexes:\n":
                    isText = -1
                elif isText == 1:
                    resultText += buff
                elif isText == -1:
                    resultPho += buff
                # print buff.strip('\n')
            # sleep(0.1)
            data = {"content":resultText, "phone":resultPho}
            json_data = json.dumps(data)
            return json_data
        except Exception, e:
            print e
            self.cpp_proc.kill()
            self.cpp_proc = subprocess.Popen([self.program], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
            error = {"content":"Error, please wait and search again.","phone":""}
            return json.dumps(error)


def generateTest(test, featureLoader):
    with open(QUERY_PATH+'query_test.txt', 'r') as queryFile:
        query = queryFile.readline().strip('\n')
        count = 0
        while query:
            try:
                Quires = query.split(',')
                Phones = []
                for quiry in Quires:
                    filename = test.convertText(quiry)
                    java_proc = subprocess.Popen("java -jar cmake-build-debug/MP3ToWav.jar " + filename + '.mp3',
                                                 shell=True)
                    java_proc.wait()
                    os.remove(MP3_PATH + filename + '.mp3')
                    sourcePool = featureLoader.extractwav(PATH + filename + '.wav')
                    sourceCoeff = np.array(sourcePool["coefficents"])
                    sourceEnergy = np.array(sourcePool["energy"])
                    mean = featureLoader.find_phonome(sourceCoeff, sourceEnergy)
                    Phones.append(mean.astype('float32'))
                    os.remove(PATH + filename + '.wav')

                with open(QUERY_PATH+str(count)+'.txt','w') as testFile:
                    for i in range(0, Quires.__len__()):
                        testFile.write(str(Quires[i])+'\n')
                        for item in Phones[i]:
                            testFile.write(str(item)+',')
                        testFile.write('\n')
                print("%d: %s"%(count, query))
                query = queryFile.readline().strip('\n')
                count += 1
            except Exception, e:
                print e
                query = queryFile.readline().strip('\n')
                count += 1
                continue


if __name__ == '__main__':
    # test = SpeechConvertor()
    # featureLoader = MFCC()
    # generateTest(test, featureLoader)
    # print 'Start Building Indexing'
    test = StreamAudioIndex()
    while (True):
        # test.searchFromScreen()
        test.searchForTest()

