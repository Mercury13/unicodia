# Character transferring from file to file, RECEIVING part

fontforge.runInitScripts()
font = fontforge.activeFont()
cps = getCps()
# Finally open file
file = open('send.txt', 'r')
# @todo [urgent] File format: SVG CP Width
# close file
file.close()