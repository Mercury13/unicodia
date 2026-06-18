import os
import xml.etree.ElementTree as ET
from enum import Enum

def _readTextMaybe(node, subname : str) -> str | None:
    q = node.find(subname)
    if q is None:
        return None
    paras = q.findall('p')
    if len(paras) == 0:
        return node.text
    else:
        isNext = False
        r = ''
        for v in paras:
            if isNext:
                r += '\n'
            q = v.text
            if q is str:
                r += q
            isNext = True
        return r

def _readText(node, chain : str, subname : str) -> str:
    q = _readTextMaybe(node, subname)
    if q is None:
        raise Exception(f'Chain {chain}: cannot find text <{subname}>')
    return q

class Xstring:
    def __init__(self, knownOrig : str, value : str):
        self.knownOrig = knownOrig
        self.value = str
        self.tag = 0

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

    def putStr(self, iid : str, knownOrig : str, value : str):
        self.strings[iid] = Xstring(knownOrig, value)

    def nStrings(self):
        r = len(self.strings)
        for _,v in self.children.items():
            r += v.nStrings()
        return r
        
    def _loadXml(self, node, chain : str, isTransl : bool, knowLang : bool):
        for v in node.findall('group'):
            iid = v.get('id', '')
            child = Xgroup()
            self.children[iid] = child
            child._loadXml(v, chain + '/' + iid, isTransl, knowLang)
        for v in node.findall('text'):
            iid = v.get('id', '')
            chain2 = chain + ':' + iid
            orig = _readText(v, chain2, 'orig');
            knownOrig = None
            translation = None
            if isTransl:
                knownOrig = _readTextMaybe(v, 'known-orig')
                if knowLang and not(knownOrig is None):
                    raise Exception(f'Know language, string {chain2} is untranslated')
                translation = _readTextMaybe(v, 'transl')
            if knownOrig is None:
                knownOrig = orig
            if translation is None:
                translation = orig
            self.putStr(iid, knownOrig, translation)

class Xlang:
    def __init__(self):
        self.clear()

    def clear(self):
        self.files = dict()

    def nFiles(self):
        return len(self.files)

    def nStrings(self):
        '''  @return  total # of strings in all files  '''
        r = 0
        for _,v in self.files.items():
            r += v.nStrings()
        return r

    def loadXml(self, fname : str, knowLang : bool):
        self.clear()
        xml = ET.parse(fname)
        root = xml.getroot()
        sType = root.get('type', '')
        isTransl = False
        if sType == 'original':
            print(f'Object {fname} is an original.')
            self.original = ''
        elif sType == 'full-transl':
            isTransl = True
            knowLang = False
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
        # Load files
        for v in root.findall('file'):
            fname = v.get('name', '')
            if fname == '':
                print(v)
                raise Exception('Found a file without name')
            file = Xgroup()
            self.files[fname] = file
            file._loadXml(v, '', isTransl, knowLang)

def exportXml(indir : str, fname : str, tech: str, outdir : str, knowLang : bool):
    iso,_ = os.path.splitext(fname)
    print(f'Export language START: lang={iso} ({tech}), know={knowLang}')
    lang = Xlang()
    lang.loadXml(indir + '/' + fname, knowLang)
    nf = lang.nFiles()
    ns = lang.nStrings()
    print(f'Loaded {nf} files, {ns} strings.')
    print(f'Export language END')
