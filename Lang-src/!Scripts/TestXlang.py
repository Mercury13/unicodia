import xlang

print('Test Xlang!')
QT_DIR = 'C:\\Qt\\6.2.4\\mingw_64'
IS_REL = True
xlang.exportFull('..', 'en.uorig', 'English',   '~out', True,  None,   IS_REL)
xlang.exportFull('..', 'ja.utran', 'Japanese',  '~out', False, QT_DIR, IS_REL)
xlang.exportFull('..', 'ru.utran', 'Russian',   '~out', True,  QT_DIR, IS_REL)
xlang.exportFull('..', 'tr.utran', 'Turkish',   '~out', False, QT_DIR, IS_REL)
xlang.exportFull('..', 'uk.utran', 'Ukrainian', '~out', True,  QT_DIR, IS_REL)
xlang.exportFull('..', 'zh.utran', 'Chinese',   '~out', False, QT_DIR, IS_REL, 'zh_CN')
