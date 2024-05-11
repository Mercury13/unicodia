# Character transferring from file to file, RECEIVING part
import fontforge
import os

fontforge.runInitScripts()
font = fontforge.activeFont()
cps = getCps()
# Finally open file
file = open('send.txt', 'r')
# @todo [urgent] File format: SVG CP Width
# Close file
file.close()