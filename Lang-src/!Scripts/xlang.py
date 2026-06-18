import os
import xml.etree.ElementTree as ET
from enum import Enum

class Xstring:
    def __init__(self, knownOrig : str, value : str, tag : int):
        self.knownOrig = knownOrig
        self.value = str
        self.tag = tag

class Xgroup:
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

    def nStrings(self):
        r = len(self.strings)
        for _,v in self.children.items():
            r += v.nStrings()
        return r

    def _loadXml(self, node, isTransl : bool, knowLang : bool):
        pass

class Xlang:
    def __init__(self):
        self.clear()

    def clear(self):
        self.files = dict()

    def nFiles(self):
        return len(self.files)

    def nStrings(self):
        r = 0
        for _,v in self.files.items():
            r += v.nStrings()
        return r

    def loadXml(self, fname : str, knowLang : bool):
        '''
           @param  fname   FULL filename
        '''
        self.clear()
        xml = ET.parse(fname)
        root = xml.getroot()
        sType = root.get('type', '')
        isTransl = False
        if sType == 'original':
            print('Object {fname} is an original.')
            self.original = ''
        elif sType == 'full-transl':
            isTransl = True
            knowLang = false
            hInfo = root.find('info')
            if hInfo is None:
                raise Exception('Translation has no <info> tag')
            hOrig = hInfo.find('orig')
            if hOrig is None:
                raise Exception('Translation has no <info/orig> tag')
            self.original = hOrig.get('fname', '')
            if self.original == '':
                raise Exception('Translation does not link to original')
        else:
            raise Exception(f'Unknown XML type {sType}')

        for v in root.findall('file'):
            fname = v.get('name', '')
            if fname == '':
                print(v)
                raise Exception('Found a file without name')
            file = Xgroup()
            self.files[fname] = file
            file._loadXml(v, isTransl, knowLang)

def exportXml(indir : str, fname : str, tech: str, outdir : str, knowLang : bool):
    iso,_ = os.path.splitext(fname)
    print(f'Export language START: lang={iso} ({tech}), know={knowLang}')
    lang = Xlang()
    lang.loadXml(indir + '/' + fname, knowLang)
    nf = lang.nFiles()
    ns = lang.nStrings()
    print(f'Loaded {nf} files, {ns} strings.')
    print(f'Export language END')
