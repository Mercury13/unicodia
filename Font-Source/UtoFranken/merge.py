import os
import fontTools.ttLib as ttLib
from fontTools.merge import Merger
from fontTools.ttLib.tables import otTables

# Adds an empty GSUB table to a font.
def add_gsub_to_font(fontfile, scriptTag):
    font = ttLib.TTFont(fontfile)
    gsub_table = ttLib.getTableClass("GSUB")("GSUB")
    gsub_table.table = otTables.GSUB()
    gsub_table.table.Version = 0x10000
    gsub_table.table.ScriptList = otTables.ScriptList()
    gsub_table.table.ScriptCount = 1
    gsub_table.table.LookupList = otTables.LookupList()
    gsub_table.table.LookupList.LookupCount = 0
    gsub_table.table.LookupList.Lookup = []
    gsub_table.table.FeatureList = otTables.FeatureList()
    gsub_table.table.FeatureList.FeatureCount = 0
    gsub_table.table.LookupList.FeatureRecord = []

    script_record = otTables.ScriptRecord()
    script_record.ScriptTag = scriptTag
    script_record.Script = otTables.Script()
    script_record.Script.LangSysCount = 0
    script_record.Script.LangSysRecord = []

    default_lang_sys = otTables.DefaultLangSys()
    default_lang_sys.FeatureIndex = []
    default_lang_sys.FeatureCount = 0
    default_lang_sys.LookupOrder = None
    default_lang_sys.ReqFeatureIndex = 65535
    script_record.Script.DefaultLangSys = default_lang_sys

    gsub_table.table.ScriptList.ScriptRecord = [script_record]

    font["GSUB"] = gsub_table

    target_file = "~" + os.path.basename(fontfile)
    font.save(target_file)
    return target_file

# Checks whether we gave GSUB
def has_gsub_table(fontfile):
    font = ttLib.TTFont(fontfile)
    return "GSUB" in font

files = [
    # It's recommended to put NotoSans-Regular.ttf as the first element in the
    # list to maximize the amount of meta data retained in the final merged font.
    [ "NotoSansImperialAramaic-Regular.ttf",    "armi" ],
    [ "NotoSansLinearA-Regular.ttf",            "lina" ],
    [ "NotoSansLinearB-Regular.ttf",            "linb" ],
    [ "NotoSansLisu-Regular.ttf",               "lisu" ],
    [ "NotoSansMedefaidrin-Regular.ttf",        "medf" ],
    [ "NotoSansMro-Regular.ttf",                "mroo" ],
    [ "NotoSansNabataean-Regular.ttf",          "nbat" ],
    [ "NotoSansPalmyrene-Regular.ttf",          "palm" ],
]

# Get new fonts
newFiles = []
for fileInfo in files:
    file = fileInfo[0]
    scriptTag = fileInfo[1]
    if has_gsub_table(file):
        newFiles.append(file)
    else:
        newFile = add_gsub_to_font(file, scriptTag)
        newFiles.append(newFile)

print(newFiles)

# Merge the fonts
merger = Merger()
new_font = merger.merge(newFiles)

# Save the merged font
new_font.save("UtoFranken.ttf")
