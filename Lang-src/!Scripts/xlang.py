import os
import xml.etree.ElementTree as ET
from enum import Enum

class Xstring:
    def __init__(self, knownOrig : str, value : str, tag : int):
        self.knownOrig = knownOrig
        self.value = str
        self.tag = tag

class Xlang:
    def __init__(self):
        self.clear()

    def clear(self):
        self.children = dict()
        self.strings = dict()
        self.original = ''

    def childAt(self, key : str) -> Xlang:
        '''
           @return  never None
        '''
        if key in self.children:
            return self.children[key]
        else:
            r = Xlang()
            self.children[key] = r
            return r

    def putStr(self, knownOrig : str, key : str, value : str, tag : int):
        self.strings[key] = Xstring(knownOrig, value, tag)

    def loadXml(self, fname : str, knowLang : bool):
        '''
           @param  fname   FULL filename
        '''
        self.clear()
        xml = ET.parse(fname)

def exportXml(indir : str, fname : str, tech: str, outdir : str, knowLang : bool):
    iso,_ = os.path.splitext(fname)
    print(f'Export language START: lang={iso} ({tech}), know={knowLang}')
    lang = Xlang()
    lang.loadXml(indir + '/' + fname, knowLang)
    print(f'Export language END')
