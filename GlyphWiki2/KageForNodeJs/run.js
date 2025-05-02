// KAGE combat script executed from Python. Need node.js only!
//
// Node.js:
//   Download: https://nodejs.org/en/download
//   Run: d:\Soft\NodeJs\node.exe run.js !u4e00-j u4e00-j=1:0:0:14:101:186:101 >result.svg

import { Kage } from "./kage.js"
import { Polygons } from "./polygons.js"
import { argv } from 'node:process'

var kage = new Kage();
var polygons = new Polygons();

var entryPoint = '';
argv.forEach((val, index) => {
	if (val.startsWith('!')) {
		entryPoint = val.substring(1);
	} else if (entryPoint != '') {
		var whereEq = val.indexOf('=');
		if (whereEq > 0) {
			var k = val.substring(0, whereEq);
			var v = val.substring(whereEq + 1);
			kage.kBuhin.push(k, v);
		}
	}
});

if (entryPoint != '') {
	kage.makeGlyph(polygons, entryPoint);
	console.log(polygons.generateSVG(false));
}
