#include "2d.js"
#include "buhin.js"
#include "curve.js"
#include "kage.js"
#include "kagecd.js"
#include "kagedf.js"
#include "polygon.js"
#include "polygons.js"

var kage = new Kage();
kage.kUseCurve = false;
var polygons = new Polygons();

kage.kBuhin.push("u6f22", "99:0:0:9:12:73:200:u6c35-07$99:0:0:54:10:190:199:u26c29-07");
kage.kBuhin.push("u6c35-07", "2:7:8:42:12:99:23:124:35$2:7:8:20:62:75:71:97:85$2:7:8:12:123:90:151:81:188$2:2:7:63:144:109:118:188:51");
kage.kBuhin.push("u26c29-07", "1:0:0:18:29:187:29$1:0:0:73:10:73:48$1:0:0:132:10:132:48$1:12:13:44:59:44:87$1:2:2:44:59:163:59$1:22:23:163:59:163:87$1:2:2:44:87:163:87$1:0:0:32:116:176:116$1:0:0:21:137:190:137$7:32:7:102:59:102:123:102:176:10:190$2:7:0:105:137:126:169:181:182");

kage.makeGlyph(polygons, "u6f22");

_root.lineStyle(0, 0, 100);
for (var i = 0; i < polygons.array.length; i++) {
	_root.beginFill(0, 100);
	_root.moveTo(polygons.array[i].array[0].x, polygons.array[i].array[0].y);
	for (var j = 1; j < polygons.array[i].array.length; j++) {
		_root.lineTo(polygons.array[i].array[j].x, polygons.array[i].array[j].y);
	}
	_root.endFill();
}

