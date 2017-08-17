# -*- coding: utf-8 -*-

import subprocess
import numpy as np
from Text2Speech import SpeechConvertor
from mfcc import MFCC

PATH = '/media/billy/New Volume/Billy/audio/'
data = raw_input("Please enter the words you want to search(use ',' to divide): ")

test = SpeechConvertor()
featureLoader = MFCC()
Quires = data.split(',')
Phones = []
for quiry in Quires:
    filename = test.convertText(quiry)
    java_proc = subprocess.Popen("java -jar cmake-build-debug/MP3ToWav.jar " + filename + '.mp3', shell=True)
    java_proc.wait()
    sourcePool = featureLoader.extractwav(PATH + filename + '.wav')
    sourceCoeff = np.array(sourcePool["coefficents"])
    sourceEnergy = np.array(sourcePool["energy"])
    mean = featureLoader.find_phonome(sourceCoeff, sourceEnergy)
    mean.astype('float32').tofile(PATH+filename+'.mfcc')
    print mean
    Phones.append(mean.astype('float32'))