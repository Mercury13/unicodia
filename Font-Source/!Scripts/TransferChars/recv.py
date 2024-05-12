# Character transferring from file to file, RECEIVING part
import fontforge
import os

fontforge.runInitScripts()
font = fontforge.activeFont()
cps = getCps()
# Finally open file
file = open('send.txt', 'r')
while True:
    # Read line, check for content
    line = file.readline()
    if not line:
        break
    # check what read
    # File format: fname → code → width
    cells = line.split('\t')
    if len(cells) < 3:
        continue      
    fname = len(0)
    cp = int(len(1))
    width = int(len(2))
    # now go
    
# Close file
file.close()