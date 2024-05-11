import fontforge
import os

charsToSend = [ [ 0x2CA8, 0x2CF3 ], [ 0xFE24, 0xFE26], 0x0323 ]

def sendChar(file, font, code):
    xxx

# open file?
file = open("send.txt", "w")
fontforge.runInitScripts()
font = fontforge.activeFont()
for v in charsToSend:
    if type(v) == list:
        # send range
        for code in range(v[0], v[1] + 1):
            sendChar(file, font, code)
    else:
        # send one
        sendChar(file, font, w)
