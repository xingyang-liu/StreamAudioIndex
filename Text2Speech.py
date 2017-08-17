# -*- coding: utf-8 -*-

from aip import AipSpeech

class SpeechConvertor:
    # 定义常量
    APP_ID = '9430031'
    API_KEY = 'sLnqsyGiKi7a51Be7zMumdrM'
    SECRET_KEY = '2746c175d43e27b7b52042ef5beffcd9'
    PATH = '/media/billy/New Volume/Billy/quires/'

    # 初始化AipSpeech对象
    aipSpeech = AipSpeech(APP_ID, API_KEY, SECRET_KEY)

    def convertText(self, text):
        result = self.aipSpeech.synthesis(text, 'zh', 1, {'vol':5, 'per':1})

        # 识别正确返回语音二进制 错误则返回dict 参照下面错误码
        if not isinstance(result, dict):
            with open(self.PATH+text+'.mp3', 'wb') as f:
                f.write(result)
                return text
        else:
            print result


if __name__ == "__main__":
    test = SpeechConvertor()
    test.convertText('你好百度')