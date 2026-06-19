import xlang

print('Test Xlang!')
xlang.exportToIni('..', 'en.uorig', 'English',   '~out', True)
xlang.exportToIni('..', 'ja.utran', 'Japanese',  '~out', False)
xlang.exportToIni('..', 'ru.utran', 'Russian',   '~out', True)
xlang.exportToIni('..', 'tr.utran', 'Turkish',   '~out', False)
xlang.exportToIni('..', 'uk.utran', 'Ukrainian', '~out', True)
xlang.exportToIni('..', 'zh.utran', 'Chinese',   '~out', False)
