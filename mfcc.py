import glob
import os
import essentia.standard
import essentia.algorithms
import essentia
import time
import math
import subprocess
import numpy as np
from fastdtw import fastdtw
from scipy.spatial.distance import euclidean
from TopHeap import TopkHeap

class MFCC:

    pool = essentia.Pool()
    spectrum = essentia.standard.Spectrum()
    mfcc = essentia.standard.MFCC()
    w = essentia.standard.Windowing(type='hann')
    energy = essentia.standard.Energy()

    overlap = 10

    def findWavFiles(self, filepath):
        print "Number of files: ", len(glob.glob(filepath + "/*.wav"))
        return glob.glob(filepath + "/*.wav")

    def loadAudio(self, audiofilepath):
        loader = essentia.standard.MonoLoader(filename=audiofilepath)
        audio = loader()
        return audio

    def exportCalculatedData(self, pool, exportedfilename):
        aggrPool = essentia.standard.PoolAggregator(defaultStats=["mean"])(pool)
        essentia.standard.YamlOutput(filename=exportedfilename, format="json")(aggrPool)

    def currentTime(self):
        return int(round(time.time() * 1000))

    def extractwav(self, file):
        # exportedfilename = os.path.basename(file).replace(".wav", "")
        # print exportedfilename
        self.pool.clear()
        audio = self.loadAudio(file)

        for frame in essentia.standard.FrameGenerator(audio, frameSize=1024, hopSize=512, startFromZero=True):
            mfcc_bands, mfcc_coeffs = self.mfcc(self.spectrum(self.w(frame)))
            audio_energy = self.energy(frame)
            self.pool.add("coefficents", mfcc_coeffs)
            self.pool.add("mel_bands", mfcc_bands)
            self.pool.add("energy", audio_energy)
        # print self.pool["coefficents"]
        # print 'Pool contains %d frames of MFCCs' % len(self.pool['coefficents'])
        return self.pool
        # self.exportCalculatedData(self.pool, exportedfilename + ".sig")
        # self.pool.clear()

    def extractdir(self, dir):
        glob = self.findWavFiles(dir)
        startTime = self.currentTime()

        for file in glob:
            exportedfilename = os.path.basename(file).replace(".wav", "")
            self.extractwav(file)
            self.exportCalculatedData(self.pool, exportedfilename + ".sig")
            self.pool.clear()

        endTime = self.currentTime()
        totalTime = endTime - startTime
        print "total time is", totalTime

    def calculate_covariane_matrix(self, X):
        sum = np.zeros((X.shape[1], X.shape[1]))
        sum = np.asmatrix(sum)
        for x in X:
            mat = np.asmatrix(x)
            maT = mat.T
            sum+= maT * mat
        sum = sum/X.shape[1]
        return sum


    def calculate_AHS_distance(self, src, cue):
        X = self.calculate_covariane_matrix(src)
        Y = self.calculate_covariane_matrix(cue)
        return np.log(np.trace(Y*X.I)*np.trace(X*Y.I)) - np.log(X.shape[1])

    def calculate_Euc_distance(self, src, cue):
        diff = 0
        for i in range(src.shape[0]):
            diff += (src[i] - cue[i]) * (src[i] - cue[i])
        return math.sqrt(diff)

    def calculate_Cosine_distance(self, x, y):
        Lx = np.sqrt(x.dot(x))
        Ly = np.sqrt(y.dot(y))
        cos_angle = x.dot(y)/(Lx*Ly)
        return cos_angle


    # def find_position(self, src, cue):
    #     len = cue.shape[0]
    #     startp = 0
    #     min = 1000000
    #     while 1:
    #         diff = self.calculate_AHS_distance(src[startp:startp+len, :], cue)
    #         if diff < min:
    #             min= diff
    #             print ("position: %f s, score: %f"%(float(startp)/90, diff))
    #         if startp + len >= src.shape[0]:
    #             break
    #         startp += self.overlap

    def find_position(self, src, cue):
        iteration = 0
        max = -1
        th = TopkHeap(10)
        for sourceItem in src[:-(cue.size()+1)]:
            i = 0
            cosine = np.array([])
            while i < cue.size():
                try:
                    cosine = np.append(cosine, self.calculate_Cosine_distance(src[iteration+i], cue[i]))
                except IndexError, e:
                    break
            iteration += 1
            tmp = {"value": np.mean(cosine), "note": iteration}
            th.Push(tmp)
        for item in th.TopK():
            print ("Latice %s: %f" % (item["note"], item["value"]))


    def choose_min(self, a, b, c = 9999999):
        if a <= b and a <= c:
            return a, 1
        elif b <= c:
            return b, 0
        else:
            return c, -1

    # def find_position(self, src, cue): #
    #     iteration = 0
    #     min = 9999999
    #     th = TopkHeap(10)
    #     for sourceItem in src[:-(cue.__len__()+1)]:
    #         i = iteration
    #         iteration += 1
    #         d0, p0 = fastdtw(sourceItem, cue[0], dist=euclidean)
    #         d1, p1 = fastdtw(sourceItem, cue[1], dist=euclidean)
    #         diff, j = self.choose_min(d1, d0)
    #         while j < cue.__len__()-2:
    #             try:
    #                 j += 1
    #                 i += 1
    #                 d0, p0 = fastdtw(src[i], cue[j], dist=euclidean)
    #                 d1, p1 = fastdtw(src[i], cue[j + 1], dist=euclidean)
    #                 d_, p_ = fastdtw(src[i], cue[j - 1], dist=euclidean)
    #                 tmpd, tmpj = self.choose_min(d1, d0, d_)
    #                 diff += tmpd
    #                 j += tmpj
    #             except IndexError, e:
    #                 diff = 9999999
    #                 break
    #             # print ("source %d and cue %d distance: %f, path: %s"%(i, j, distance, path))
    #
    #         diff = diff / cue.__len__()
    #         tmp = {"value":diff, "note": iteration}
    #         th.Push(tmp)
    #     for item in th.TopK():
    #         print ("Latice %s: %f"%(item["note"], item["value"]))


    def find_phonome(self, src, energy):
        phonome = []
        prev_i = 0
        prev_diff = 0
        transi_flag = 0
        count = 0
        mean = np.array([])
        for i in range(2, src.shape[0]):
            # print ("from %f ms to %f ms: %f")%((i-2)*6, (i+1)*6, self.calculate_Euc_distance(src[i-2,:], src[i,:]))
            diff = self.calculate_Euc_distance(src[i-2,:], src[i,:])
            if diff > 100 and transi_flag == 0:
                if prev_diff < diff:
                    prev_diff = diff
                else:
                    if np.mean(energy[prev_i:i-2]) > 2:
                        count += 1
                        tmp = np.mean(src[prev_i:i-2, :], axis=0)
                        mean = np.append(mean, tmp, axis=0)
                        # mean.astype('float32').tofile("result.bin")
                        # phonome.append(src[prev_i:i-2, :].tolist())
                        # print ("%d: from %f ms to %f ms") % (count, prev_i * 12, (i - 2) * 12)
                    prev_i = i
                    prev_diff = 0
                    transi_flag = 1
            elif diff <= 100 and transi_flag == 1:
                transi_flag = 0;
        return mean
        # mean.astype('float32').tofile("result.bin")
        # return phonome




if __name__ == "__main__":
    featureLoader = MFCC()
    path = "/media/billy/Braavos/Billy/"
    srcdir = "source/"
    desdir = "audio/"
    textdir = "phonome/"
    count = 0
    p = subprocess.Popen("java -jar MP3ToWav.jar " + path + " " + srcdir, shell=True, stdout=subprocess.PIPE)
    while True:
        buff = p.stdout.readline()
        if buff == '000\n':
            break
        else:
            buff = buff.strip('\n')
            try:
                sourcePool = featureLoader.extractwav(path + desdir + buff)
                sourceCoeff = np.array(sourcePool["coefficents"])
                sourceEnergy = np.array(sourcePool["energy"])
                mean = featureLoader.find_phonome(sourceCoeff, sourceEnergy)
                mbuff = buff.replace('.wav', '.mfcc')
                mean.astype('float32').tofile(path+textdir+mbuff)
                count += 1
                print("%d: %s"%(count, buff))
                os.remove(path+desdir+buff)
            except RuntimeError:
                print "error found!"
                continue
    #         print sourcePhonome
    # featureLoader = MFCC()
    # sourcePool = featureLoader.extractwav("fingerprinter/test/data/2.wav")
    # sourceCoeff = np.array(sourcePool["coefficents"])
    # sourceEnergy = np.array(sourcePool["energy"])
    # sourcePhonome = featureLoader.find_phonome(sourceCoeff, sourceEnergy)
    # cuePool = featureLoader.extractwav("fingerprinter/test/data/tangguo.wav")
    # cueCoeff = np.array(cuePool["coefficents"])
    # cueEnergy = np.array(cuePool["energy"])
    # cuePhonome = featureLoader.find_phonome(cueCoeff, cueEnergy)
    # featureLoader.find_position(sourcePhonome, cuePhonome)




