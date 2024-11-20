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

def read_line_metrics(font):
    metrics = {
        "ascent": font["hhea"].ascent,
        "descent": font["hhea"].descent,
        "usWinAscent": font["OS/2"].usWinAscent,
        "usWinDescent": font["OS/2"].usWinDescent,
        "sTypoAscender": font["OS/2"].sTypoAscender,
        "sTypoDescender": font["OS/2"].sTypoDescender,
        "sxHeight": font["OS/2"].sxHeight,
        "sCapHeight": font["OS/2"].sCapHeight,
        "sTypoLineGap": font["OS/2"].sTypoLineGap,
    }
    return metrics

def set_line_metrics(font, metrics):
    font["hhea"].ascent = metrics["ascent"]
    font["hhea"].descent = metrics["descent"]
    font["OS/2"].usWinAscent = metrics["usWinAscent"]
    font["OS/2"].usWinDescent = metrics["usWinDescent"]
    font["OS/2"].sTypoAscender = metrics["sTypoAscender"]
    font["OS/2"].sTypoDescender = metrics["sTypoDescender"]
    font["OS/2"].sxHeight = metrics["sxHeight"]
    font["OS/2"].sCapHeight = metrics["sCapHeight"]
    font["OS/2"].sTypoLineGap = metrics["sTypoLineGap"]


files = [
    [ "NotoSansAvestan-Regular.ttf",            "avst" ],
    [ "NotoSansBassaVah-Regular.ttf",           "bass" ],
    [ "NotoSansCaucasianAlbanian-Regular.ttf",  "aghb" ],
    [ "NotoSansCherokee-Regular.ttf",           "cher" ],
    [ "NotoSansChorasmian-Regular.ttf",         "chrs" ],
    [ "NotoSansHatran-Regular.ttf",             "hatr" ],
    [ "NotoSansImperialAramaic-Regular.ttf",    "armi" ],
    [ "NotoSansInscriptionalParthian-Regular.ttf", "prti" ],
    [ "NotoSansLinearA-Regular.ttf",            "lina" ],
    [ "NotoSansLinearB-Regular.ttf",            "linb" ],
    [ "NotoSansLisu-Regular.ttf",               "lisu" ],
    [ "NotoSansMedefaidrin-Regular.ttf",        "medf" ],
    [ "NotoSansMendeKikakui-Regular.ttf",       "mend" ],
    [ "NotoSansMro-Regular.ttf",                "mroo" ],
    [ "NotoSansNabataean-Regular.ttf",          "nbat" ],
    [ "NotoSansOgham-Regular.ttf",              "ogam" ],
    [ "NotoSansOldItalic-Regular.ttf",          "ital" ],
    [ "NotoSansOldNorthArabian-Regular.ttf",    "narb" ],
    [ "NotoSansOldPersian-Regular.ttf",         "xpeo" ],
    [ "NotoSansOldTurkic-Regular.ttf",          "orkh" ],
    [ "NotoSansPalmyrene-Regular.ttf",          "palm" ],
    [ "NotoSansPauCinHau-Regular.ttf",          "pauc" ],        
    [ "NotoSansPhoenician-Regular.ttf",         "phnx" ],
    [ "NotoSansPsalterPahlavi-Regular.ttf",     "phlp" ],
    [ "NotoSansRunic-Regular.ttf",              "runr" ],
    [ "NotoSansSamaritan-Regular.ttf",          "samr" ],
    [ "NotoSansTifinagh-Regular.ttf",           "tfng" ],
    [ "NotoSansUgaritic-Regular.ttf",           "ugar" ],
    [ "NotoSansWancho-Regular.ttf",             "wcho" ],
    [ "NotoSansWarangCiti-Regular.ttf",         "wara" ],
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

metrics = read_line_metrics(ttLib.TTFont("../../Fonts/NotoSerif-Regular.ttf"))
set_line_metrics(new_font, metrics)

# Save the merged font
new_font.save("UtoSansFranken.ttf")
