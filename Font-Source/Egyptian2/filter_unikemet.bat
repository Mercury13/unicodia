rem Run every time when something changes in Unikemet data
echo #	This is an excerpt from Unikemet base to simplify tracking changes >Unisesh.txt
findstr "kEH_JSesh kEH_Cat" ..\..\MiscFiles\RawData\Unikemet.txt >>Unisesh.txt