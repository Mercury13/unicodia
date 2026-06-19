import os
import shutil
import xml.etree.ElementTree as ET
from enum import Enum

def _readTextMaybe(node, subname : str) -> str | None:
    q = node.find(subname)
    if q is None:
        return None
    paras = q.findall('p')
    if len(paras) == 0:
        return q.text
    else:
        isNext = False
        r = ''
        for v in paras:
            if isNext:
                r += '\n'
            q = v.text
            if isinstance(q, str):
                r += q
            isNext = True
        return r

def _readText(node, chain : str, subname : str) -> str:
    q = _readTextMaybe(node, subname)
    if q is None:
        raise Exception(f'Chain {chain}: cannot find text <{subname}>')
    return q

BACKSLASH = '\\'
BACKSLASH_ESC = '\\\\'
NEWLINE = '\n'
NEWLINE_ESC = '\\n'

def iniEscape(x : str) -> str:
    x = x.replace(BACKSLASH, BACKSLASH_ESC)
    x = x.replace(NEWLINE, NEWLINE_ESC)
    return x

class Xstring:
    def __init__(self, knownOrig : str, value : str):
        self.knownOrig = knownOrig
        self.value = value

class Xgroup:
    def __init__(self):
        self.clear()

    def clear(self):
        self.children = dict()
        self.strings = dict()

    def childAt(self, key : str) -> Xgroup:
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
        for v in self.children.values():
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
                if knowLang and (knownOrig is not None):
                    raise Exception(f'Know language, string "{chain2}" is untranslated')
                translation = _readTextMaybe(v, 'transl')
            if knownOrig is None:
                knownOrig = orig
            if translation is None:
                translation = orig
            self.putStr(iid, knownOrig, translation)

    def _recurseExportIni(self, file, prefix : str, separator : str, nWritten : int) -> int:
        ''' @return  new # of lines written'''
        # Strings
        if len(self.strings) > 0:
            if prefix != '':
                if nWritten > 0:
                    file.write('\n')
                file.write('[' + prefix + ']\n')
            for k,v in self.strings.items():
                esc = iniEscape(v.value)
                file.write(k)
                file.write('=')
                file.write(esc)
                file.write('\n')
            nWritten = nWritten + len(self.strings)
        # Children
        newSeparator = separator
        if prefix == '':
            newSeparator = ''
        for k,v in self.children.items():
            newPrefix = prefix + newSeparator + k
            nWritten = v._recurseExportIni(file, newPrefix, separator, nWritten)
        return nWritten

    def retranslate(self, other : Xlang, prefix : str, knowLang : bool):
        '''  @param [in]  prefix  just for error-checking
             @param [in]  knowLang  [+] throw error on something untranslated
        '''
        for k,v in self.children.items():
            if k in other.children:
                v.retranslate(other.children[k], prefix + '/' + k, knowLang)
            else:
                if knowLang:
                    raise Exception(f'Know language, group "{prefix}/{k}" is untranslated')
        for k,v in self.strings.items():
            if k in other.strings:
                w = other.strings[k]
                if knowLang and (v.value != w.knownOrig):
                    raise Exception(f'Know language, string "{prefix}:{k}" is obsolete')
                v.value = w.value
            else:
                if knowLang:
                    raise Exception(f'Know language, string "{prefix}:{k}" is untranslated')

    def exportIni(self, fname: str, separator : str):
        file = open(fname, 'w', encoding='UTF-8')
        self._recurseExportIni(file, '', separator, 0)

class Xlang:
    def __init__(self):
        self.clear()

    def clear(self):
        self.files = dict()
        self.original = ''

    def isOriginal(self) -> bool:
        return (self.original == '')

    def nFiles(self):
        return len(self.files)

    def nStrings(self):
        '''  @return  total # of strings in all files  '''
        r = 0
        for v in self.files.values():
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
            knowLang = False
        elif sType == 'full-transl':
            isTransl = True
            hInfo = root.find('info')
            if hInfo is None:
                raise Exception('Translation has no <info> tag')
            hOrig = hInfo.find('orig')
            if hOrig is None:
                raise Exception('Translation has no <info/orig> tag')
            self.original = hOrig.get('fname', '')
            if self.original == '':
                raise Exception('Translation does not link to original')
            print(f'Object {fname} is a full translation.')
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
            file._loadXml(v, fname + ':', isTransl, knowLang)

    def retranslate(self, other : Xlang, knowLang : bool):
        for k,v in self.files.items():
            if k in other.files:
                v.retranslate(other.files[k], k + ':', knowLang)
            else:
                if knowLang:
                    raise Exception(f'Know language, file "{k}" is untranslated')

    def exportInis(self, outdir : str, separator : str):
        os.makedirs(outdir, exist_ok=True)
        for k,v in self.files.items():
            v.exportIni(outdir + '/' + k, separator)

def exportToIni(indir : str, fname : str, tech: str, outdir : str, knowLang : bool):
    '''  @return   1) output directory, to dump locale.xml and Qt translations there
                   2) ISO code  '''
    iso,_ = os.path.splitext(fname)
    print(f'Exporting translation: lang={iso} ({tech}), know={knowLang}')
    lang = Xlang()
    lang.loadXml(indir + '/' + fname, knowLang)
    nf = lang.nFiles()
    ns = lang.nStrings()
    print(f'Loaded {nf} files, {ns} strings.')
    newOutDir = outdir + '/' + tech
    if lang.isOriginal():
        lang.exportInis(newOutDir, '.')
    else:
        orig = Xlang()
        orig.loadXml(indir + '/' + lang.original, False)
        if not orig.isOriginal():
            raise Exception('The original is actually not!')
        orig.retranslate(lang, knowLang)
        orig.exportInis(newOutDir, '.')
    return newOutDir, iso

def _copy(fnIn : str, dirOut : str, wantCopy : bool) -> bool:
    if os.path.isfile(fnIn):
        if wantCopy:
            shutil.copy(fnIn, dirOut)
            print(f'Copied {fnIn} --> {dirOut}.')
        else:
            print(f'Skipped {fnIn} --> {dirOut}, not a release.')
        return True
    else:
        print(f'File {fnIn} is missing, skipping.')
        return False


def exportFull(indir : str, fname : str, tech : str, outdir : str,
            knowLang : bool, qtdir : str | None, isRelease : bool,
            altIso : str = ''):
    isOk = True
    newout, iso = exportToIni(indir, fname, tech, outdir, knowLang)
    print('Exporting supplemental files...')
    r = _copy(indir + '/' + iso + '/locale.xml', newout, True)
    isOk = isOk and r
    if qtdir is not None:
        iso1 = iso
        if altIso != '':
            iso1 = altIso
        trfname = f'{qtdir}/translations/qtbase_{iso1}.qm'
        r = _copy(trfname, newout, isRelease)
        isOk = isOk and r
    if not isOk:
        raise Exception('Have bad files!')

def exportAll(indir : str, outdir : str, qtdir : str, isRelease : bool):
    exportFull(indir, 'en.uorig', 'English',   outdir, True,  None,  isRelease)
    exportFull(indir, 'ja.utran', 'Japanese',  outdir, False, qtdir, isRelease)
    exportFull(indir, 'ru.utran', 'Russian',   outdir, True,  qtdir, isRelease)
    exportFull(indir, 'tr.utran', 'Turkish',   outdir, False, qtdir, isRelease)
    exportFull(indir, 'uk.utran', 'Ukrainian', outdir, True,  qtdir, isRelease)
    exportFull(indir, 'zh.utran', 'Chinese',   outdir, False, qtdir, isRelease, 'zh_CN')
