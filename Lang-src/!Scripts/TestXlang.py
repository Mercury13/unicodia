import xlang

print('Test Xlang!')
xlang.exportXml('..', 'en.uorig', 'English', '~out', True)
xlang.exportXml('..', 'ja.utran', 'Japanese', '~out', False)
xlang.exportXml('..', 'ru.utran', 'Russian', '~out', True)
