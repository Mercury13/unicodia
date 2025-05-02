// KAGE engine sample script for JavaScript engine
//
// Node.js:
//   Download: https://nodejs.org/en/download
//   Run: d:\Soft\NodeJs\node.exe debug_1.js >result.svg
// SpiderMonkey
//   Download: https://ftp.mozilla.org/pub/firefox/releases/137.0.2/jsshell/
//     (find the most recent version of FireFox)
//   Run: d:\Soft\SpiderMonkey\js.exe --module=debug_1.js >result.svg

import { Kage } from "./kage.js"
import { Polygons } from "./polygons.js"

var kage = new Kage();
var polygons = new Polygons();

kage.kBuhin.push("u31447", "99:0:0:8:0:121:200:u5321-j$99:0:0:30:0:198:200:u5149-02");
kage.kBuhin.push("u5149-02", "1:0:32:133:18:133:98$2:7:8:90:36:107:51:112:80$2:0:7:171:35:162:57:147:84$1:0:0:81:98:188:98$2:32:7:121:98:118:169:73:188$3:32:5:145:98:145:179:182:179");
kage.kBuhin.push("u531a-05", "1:2:0:30:28:176:28$1:12:13:30:28:30:172$1:2:0:30:172:186:172");
kage.kBuhin.push("u5321-j", "99:0:0:0:0:200:200:u531a-05$99:0:0:40:30:173:168:u738b-j");
kage.kBuhin.push("u738b-j", "1:0:0:24:32:176:32$1:32:32:100:32:100:171$1:0:0:33:101:167:101$1:0:0:14:171:186:171");

kage.makeGlyph(polygons, "u31447");

console.log(polygons.generateSVG(false));
