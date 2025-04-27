import { FontCanvas } from "./fontcanvas.js";
import {get_extended_dest, get_extended_dest_wrong} from "./util.js";

export class Gothic{
  constructor(size) {
    this.kRate = 50;
    if (size == 1) {
      this.kWidth = 3;
      this.kKakato = 1.8;
      this.kMage = 6;
    } else {
      this.kWidth = 5;
      this.kKakato = 3;
      this.kMage = 10;
    }
  }
  getPolygons(glyphData) {
    var cv = new FontCanvas();
    for (let glyph of glyphData) {
      this.drawStroke(cv, glyph);
    }
    return cv.getPolygons();
  }
  drawStroke(cv, s){ // gothic
    const a1 = s[0];
    const a2 = s[1];
    const a3 = s[2];
    const x1 = s[3];
    const y1 = s[4];
    const x2 = s[5];
    const y2 = s[6];
    const x3 = s[7];
    const y3 = s[8];
    const x4 = s[9];
    const y4 = s[10];
    const curve_step = 1000 / this.kRate;
    switch(a1 % 100){
    case 0:
      break;
    case 1:
      if(a3 == 4){
        let [tx1, ty1] = get_extended_dest(x2, y2, x1, y1, -this.kMage);
        this.gothicDrawLine(x1, y1, tx1, ty1, a2, 1, cv);
        this.gothicDrawCurve(tx1, ty1, x2, y2, x2 - this.kMage * 2, y2 - this.kMage * 0.5, a1, a2, cv);
      }
      else{
        this.gothicDrawLine(x1, y1, x2, y2, a2, a3, cv);
      }
      break;
    case 2:
    case 12:{
      if(a3 == 4){
        let [tx1, ty1] = get_extended_dest_wrong(x3, y3, x2, y2, -this.kMage);
        this.gothicDrawCurve(x1, y1, x2, y2, tx1, ty1, a1, a2, cv);
        this.gothicDrawCurve(tx1, ty1, x3, y3, x3 - this.kMage * 2, y3 - this.kMage * 0.5, a1, a2, cv);
      }
      else if(a3 == 5){
        const tx1 = x3 + this.kMage;
        const ty1 = y3;
        const tx2 = tx1 + this.kMage * 0.5;
        const ty2 = y3 - this.kMage * 2;
        this.gothicDrawCurve(x1, y1, x2, y2, x3, y3, a1, a2, cv);
        this.gothicDrawCurve(x3, y3, tx1, ty1, tx2, ty2, a1, a2, cv);
      }
      else{
        this.gothicDrawCurve(x1, y1, x2, y2, x3, y3, a1, a2, cv);
      }
      break;
    }
    case 3:{
      let [tx1, ty1] = get_extended_dest(x2, y2, x1, y1, -this.kMage);
        let [tx2, ty2] = get_extended_dest(x2, y2, x3, y3, -this.kMage);
        
      if(a3 == 5){
        const tx3 = x3 - this.kMage;
        const ty3 = y3;
        const tx4 = x3 + this.kMage * 0.5;
        const ty4 = y3 - this.kMage * 2;
        this.gothicDrawLine(x1, y1, tx1, ty1, a2, 1, cv);
        this.gothicDrawCurve(tx1, ty1, x2, y2, tx2, ty2, a1, a2, cv);
        this.gothicDrawLine(tx2, ty2, tx3, ty3, 1, 1, cv);
        this.gothicDrawCurve(tx3, ty3, x3, y3, tx4, ty4, a1, a2, cv);
      }
      else{
        this.gothicDrawLine(x1, y1, tx1, ty1, a2, 1, cv);
        this.gothicDrawCurve(tx1, ty1, x2, y2, tx2, ty2, a1, a2, cv);
        this.gothicDrawLine(tx2, ty2, x3, y3, 1, a3, cv);
      }
      break;
    }
    case 6:
      if(a3 == 5){
        const tx1 = x4 - this.kMage;
        const ty1 = y4;
        const tx2 = x4 + this.kMage * 0.5;
        const ty2 = y4 - this.kMage * 2;
        cv.drawCBezier(x1, y1, x2, y2, x3, y3, tx1, ty1, (t) => { return this.kWidth; }, t => 0, 1000 / this.kRate);
        this.gothicDrawCurve(tx1, ty1, x4, y4, tx2, ty2, a1, a2, cv);
      }
      else{
        cv.drawCBezier(x1, y1, x2, y2, x3, y3, x4, y4,  (t) => { return this.kWidth; }, t => 0, 1000 / this.kRate);
      }
      break;
    case 7:
      this.gothicDrawLine(x1, y1, x2, y2, a2, 1, cv);
      this.gothicDrawCurve(x2, y2, x3, y3, x4, y4, a1, a2, cv);
      break;
    case 9: // may not be exist
      //kageCanvas[y1][x1] = 0;
      //kageCanvas[y2][x2] = 0;
      break;
    default:
      break;
    }
  }

  gothicDrawCurve(x1, y1, x2, y2, x3, y3, ta1, ta2, cv) {
    var a1, a2;
    if (a1 % 10 == 2) {
      let [x1ext, y1ext] = get_extended_dest_wrong(x1, y1, x2, y2, this.kWidth);
      x1 = x1ext; y1 = y1ext;
    } else if (a1 % 10 == 3) {
      let [x1ext, y1ext] = get_extended_dest_wrong(x1, y1, x2, y2, this.kWidth * this.kKakato);
      x1 = x1ext; y1 = y1ext;
    }
    if (a2 % 10 == 2) {
      let [x2ext, y2ext] = get_extended_dest_wrong(x3, y3, x2, y2, this.kWidth);
      x3 = x2ext; y3 = y2ext;
    } else if (a2 % 10 == 3) {
      let [x2ext, y2ext] = get_extended_dest_wrong(x3, y3, x2, y2, this.kWidth * this.kKakato);
      x3 = x2ext; y3 = y2ext;
    }
    cv.drawQBezier(x1, y1, x2, y2, x3, y3, (t) => { return this.kWidth; }, t => 0, 1000 / this.kRate);
  }

  gothicDrawLine(tx1, ty1, tx2, ty2, ta1, ta2, cv) {
    var x1 = tx1;
    var y1 = ty1;
    var x2 = tx2;
    var y2 = ty2;
    if (ta1 % 10 == 2) {
      let [x1ext, y1ext] = get_extended_dest(tx1, ty1, tx2, ty2, this.kWidth);
      x1 = x1ext; y1 = y1ext;
    } else if (ta1 % 10 == 3) {
      let [x1ext, y1ext] = get_extended_dest(tx1, ty1, tx2, ty2, this.kWidth * this.kKakato);
      x1 = x1ext; y1 = y1ext;
    }
    if (ta2 % 10 == 2) {
      let [x2ext, y2ext] = get_extended_dest(tx2, ty2, tx1, ty1, this.kWidth);
      x2 = x2ext; y2 = y2ext;
    } else if (ta2 % 10 == 3) {
      let [x2ext, y2ext] = get_extended_dest(tx2, ty2, tx1, ty1, this.kWidth * this.kKakato);
      x2 = x2ext; y2 = y2ext;
    }
    cv.drawLine(x1, y1, x2, y2, this.kWidth);
  }
}