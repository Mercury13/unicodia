import { FontCanvas } from "./fontcanvas.js";
import { norm2, get_dir, get_rad, rad_to_dir, moved_point, get_extended_dest , widfun, widfun_d, widfun_stop, widfun_stop_d, widfun_fat, widfun_fat_d, DIR_POSX, DIR_NEGX, bezier_to_y, bezier_to_line, CURVE_THIN} from "./util.js";
import { STROKETYPE, STARTTYPE, ENDTYPE} from "./stroketype.js";
import { Bezier} from "./bezier.js";
import { Polygon } from "./polygon.js";
import { isCrossBoxWithOthers, isCrossWithOthers } from "./2d.js";
export class Mincho {
  constructor(size) {
    //if (!size) size=2.3;
    this.kRate = 50;
    if (size == 1) {
      this.kMinWidthY = 1.2;
      this.kMinWidthU = 1.2;
      this.kMinWidthT = 3.6;
      this.kWidth = 3;
      this.kKakato = 1.8;
      this.kL2RDfatten = 1.1;
      this.kMage = 6;

      this.kAdjustKakatoL = ([8, 5, 3, 1]); // for KAKATO adjustment 000,100,200,300,400
      this.kAdjustKakatoR = ([4, 3, 2, 1]); // for KAKATO adjustment 000,100,200,300
      this.kAdjustKakatoRangeX = 12; // check area width
      this.kAdjustKakatoRangeY = ([1, 11, 14, 18]); // 3 steps of checking
      this.kAdjustKakatoStep = 3; // number of steps

      this.kAdjustUrokoX = ([14, 12, 9, 7]); // for UROKO adjustment 000,100,200,300
      this.kAdjustUrokoY = ([7, 6, 5, 4]); // for UROKO adjustment 000,100,200,300
      this.kAdjustUrokoLength = ([13, 21, 30]); // length for checking
      this.kAdjustUrokoLengthStep = 3; // number of steps
      this.kAdjustUrokoLine = ([13, 15, 18]); // check for crossing. corresponds to length

      this.kAdjustUroko2Step = 3;
      this.kAdjustUroko2Length = 25;

      this.kAdjustTateStep = 4;
      this.kAdjustMageStep = 5;
    } else if (size == 3) {
      this.kMinWidthY = 3;
      this.kMinWidthU = 3;
      this.kMinWidthT = 8;
      this.kWidth = 6;
      this.kKakato = 4;
      this.kL2RDfatten = 1.1;
      this.kMage = 14;

      this.kAdjustKakatoL = ([20, 13, 7, 3]); // for KAKATO adjustment 000,100,200,300,400
      this.kAdjustKakatoR = ([12, 9, 6, 3]); // for KAKATO adjustment 000,100,200,300
      this.kAdjustKakatoRangeX = 26; // check area width
      this.kAdjustKakatoRangeY = ([2, 26, 30, 40]); // 3 steps of checking
      this.kAdjustKakatoStep = 3; // number of steps

      this.kAdjustUrokoX = ([30, 25, 20, 15]); // for UROKO adjustment 000,100,200,300
      this.kAdjustUrokoY = ([15, 14, 13, 12]); // for UROKO adjustment 000,100,200,300
      this.kAdjustUrokoLength = ([29, 40, 62]); // length for checking
      this.kAdjustUrokoLengthStep = 3; // number of steps
      this.kAdjustUrokoLine = ([29, 34, 39]); // check for crossing. corresponds to length

      this.kAdjustUroko2Step = 3;
      this.kAdjustUroko2Length = 50;

      this.kAdjustTateStep = 4;
      this.kAdjustMageStep = 5;
    } else if (size > 1) {
      this.kMinWidthY = size;
      this.kMinWidthU = size;
      this.kMinWidthT = size * 2.6;
      this.kWidth = size * 2.2;
      this.kKakato = size * 1.2 + 0.6;
      this.kL2RDfatten = 1.2;
      this.kMage = size * 4 + 2;

      this.kAdjustKakatoL = ([14, 11, 8, 6, 4, 2]); // for KAKATO adjustment 000,100,200,300,400
      this.kAdjustKakatoR = ([8, 6.5, 5, 4.5, 3, 2]); // for KAKATO adjustment 000,100,200,300
      this.kAdjustKakatoRangeX = 20; // check area width
      this.kAdjustKakatoRangeY = ([3, 9, 15, 21, 27, 32]); // 5 steps of checking
      this.kAdjustKakatoStep = 5; // number of steps

      this.kAdjustUrokoX = ([size * 9.5 + 4, size * 8 + 3.5, size * 6.5 + 3, size * 5 + 2.5]); // for UROKO adjustment 000,100,200,300
      this.kAdjustUrokoY = ([size * 4.6 + 2, size * 4.4 + 1.5, size * 4.2 + 1, size * 4.0 + 0.5]); // for UROKO adjustment 000,100,200,300
      this.kAdjustUrokoLength = ([size * 7 + 7, size * 11 + 11, size * 15 + 15]); // length for checking
      this.kAdjustUrokoLengthStep = 3; // number of steps
      this.kAdjustUrokoLine = ([size * 7 + 7, size * 9 + 8, size * 11 + 9]); // check for crossing. corresponds to length

      this.kAdjustUroko2Step = 3;
      this.kAdjustUroko2Length = size * 20;

      this.kAdjustTateStep = 4;
      this.kAdjustMageStep = 5;
    } else {
      this.kMinWidthY = 2;
      this.kMinWidthYY = 2;//横線の太さのみを決める。kMinWidthY以上の値が望ましい
      this.kMinWidthYY *= 1.2;
      this.kMinWidthU = 2;
      this.kMinWidthT = 6;
      this.kMinWidthT_adjust = 6;//use consistent parameter for adjustment
      //this.kMinWidthT = 5.7;
      this.kMinWidthC = 1;//開放・左下右下カドの傾きを決める。1なら元と同じ
      this.kMinWidthC *= 0.8;
      this.kWidth = 5;
      this.kKakato = 3;
      this.kL2RDfatten = 1.1;
      this.kMage = 10;
      //size = "HAIR_LINE"
      if (size == "HAIR_LINE"){//デバッグ用
        this.kMinWidthY *= 0.3;
        this.kMinWidthYY *= 0.3;
        this.kMinWidthT *= 0.2;
      } else if (size == "EXTRA_LIGHT"){
        this.kMinWidthU *= 0.71;
        this.kMinWidthY *= 0.89;
        this.kMinWidthYY *= 0.85;
        this.kMinWidthT *= 0.76;
      } else if (size == "LIGHT"){
        this.kMinWidthU *= 0.85;
        this.kMinWidthY *= 0.94;
        this.kMinWidthYY *= 0.92;
        this.kMinWidthT *= 0.87;
      } else if (size == "MEDIUM"){
        this.kMinWidthU *= 1.2;
        this.kMinWidthYY *= 1.1;
        this.kMinWidthT *= 1.15;
      } else if (size == "DEMIBOLD"){//曲線に課題あり
        this.kMinWidthU *= 1.44;
        this.kMinWidthYY *= 1.21;
        this.kMinWidthT *= 1.34;
      }
      
      this.kAdjustKakatoL = ([12, 9.6, 7.3, 5, 3, 2]); // for KAKATO adjustment
      this.kAdjustKakatoR = ([10,  8, 6, 4, 2, 1]); // for KAKATO adjustment
      this.kAdjustKakatoRangeX = 20; // check area width
      this.kAdjustKakatoRangeY = ([1, 14, 19, 24, 29, 35]); //[0]は0以上[1]以下であればなんでもよい？
      this.kAdjustKakatoStep = 5; // number of steps

      this.kAdjustUrokoX = ([24, 21, 18, 16, 14, 12]); // for UROKO adjustment 000,100,200,300
      this.kAdjustUrokoY = ([12, 11, 10, 9, 8.5, 8]); // for UROKO adjustment 000,100,200,300
      this.kAdjustUrokoLength = ([16, 23, 30, 38, 46, 55]); // length for checking
      this.kAdjustUrokoLengthStep = 5; // number of steps
      this.kAdjustUrokoLine = ([18, 20, 23, 26, 30, 35]); // check for crossing. corresponds to length

      this.kAdjustUroko2Step = 5;//max value
      this.kAdjustUroko2Length = 40;

      this.kAdjustTateStep = 4;
      this.kAdjustMageStep = 5;
    }
  }

  getPolygons(glyphData) {
    var cv = new FontCanvas();
    for (var i = 0; i < glyphData.length; i++) {
      var tempdata = glyphData.slice();
      tempdata.splice(i, 1);
      this.drawAdjustedStroke(cv, glyphData[i], tempdata);
    }
    return cv.getPolygons();
  }
  getPolygonsSeparated(strokesArrays) {
    return strokesArrays.map((glyphData, index) => {
      const cp = strokesArrays.slice();
      cp.splice(index,1)
      const other_groups = cp.flat();
     
      var cv = new FontCanvas();
      for (var i = 0; i < glyphData.length; i++) {
        var tempdata = glyphData.slice();
        tempdata.splice(i, 1);
        this.drawAdjustedStroke(cv, glyphData[i], other_groups.concat(tempdata));
      }
      return cv.getPolygons();
    });
  }
  drawAdjustedStroke(cv, s, others) {//draw stroke on the canvas

    const a1 = s[0] % 100;
    const a2 = s[1] % 100;
    const a3 = (s[2] == ENDTYPE.LOWER_LEFT_ZH_OLD || s[2] == ENDTYPE.LOWER_LEFT_ZH_NEW) ? s[2] : s[2] % 100;
    const x1 = s[3];
    const y1 = s[4];
    const x2 = s[5];
    const y2 = s[6];
    const x3 = s[7];
    const y3 = s[8];
    const x4 = s[9];
    const y4 = s[10];
    //if(a2>100){
    //  console.log("error: start type"+a2)
    //}
    //if(a3>100){
    //  console.log("error: end type"+a3)
    //}
    const dir12 = get_dir(x2-x1, y2-y1);
    const dir23 = get_dir(x3-x2, y3-y2);
    const dir34 = get_dir(x4-x3, y4-y3);
    const rad12 = get_rad(x2-x1, y2-y1);
    const rad23 = get_rad(x3-x2, y3-y2);
    
    switch (a1) {
      case 0: {//rotate and flip
        if (a2 == 98) {
          cv.flip_left_right(x1, y1, x2, y2);
        } else if (a2 == 97) {
          cv.flip_up_down(x1, y1, x2, y2);
        } else if (a2 == 99 && a3 == 1) {
          cv.rotate90(x1, y1, x2, y2);
        } else if (a2 == 99 && a3 == 2) {
          cv.rotate180(x1, y1, x2, y2);
        } else if (a2 == 99 && a3 == 3) {
          cv.rotate270(x1, y1, x2, y2);
        }
        break;
      }
      case STROKETYPE.STRAIGHT: {
        const dir = get_dir(x2-x1, y2-y1);
        if (a3 == ENDTYPE.CONNECTING_H) {//usually horizontal
          cv.drawLine(x1, y1, x2, y2, this.kMinWidthYY);
        } else if (a3 == ENDTYPE.OPEN && Math.abs(y2 - y1) < x2 - x1) { //horizontal or gentle slope
          const param_uroko = this.adjustUrokoParam(s, others);
          const param_uroko2 = this.adjustUroko2Param(s, others);
          cv.drawLine(x1, y1, x2, y2, this.kMinWidthYY);
          const urokoScale = (this.kMinWidthU / this.kMinWidthY - 1.0) / 4.0 + 1.0;
          if (y1 == y2) {//horizontal
            const uroko_max = Math.floor(norm2(param_uroko, param_uroko2))
            //const uroko_max = param_uroko == 0 ? param_uroko2 : param_uroko 
            //↑元の実装だとadjustUrokoによる調整がかかったものはadjustUroko2を一切通らないのでそれ以上小さくならない。
            //Math.max(param_uroko, param_uroko2) などのほうが合理的
            cv.drawUroko_h(x2, y2, this.kMinWidthYY, this.kAdjustUrokoX[uroko_max] * urokoScale, this.kAdjustUrokoY[uroko_max] * urokoScale);
          } else {
            cv.drawUroko(x2, y2, dir, this.kMinWidthYY, this.kAdjustUrokoX[param_uroko] * urokoScale, this.kAdjustUrokoY[param_uroko] * urokoScale);
          }
        } else {//vertical or steep slope
          let poly_end = new Polygon(2);
          const param_tate = this.adjustTateParam(s, others);
          const kMinWidthT_m = this.kMinWidthT - param_tate / 2;
          //head
          let poly_start = this.getStartOfVLine(x1, y1, x2, y2, a2, kMinWidthT_m, cv);
          if (a2 == STARTTYPE.CONNECTING_MANUAL){
            var r = get_rad(x1-x3, y1-y3) - get_rad(x2-x1, y2-y1) - Math.PI/2;
            poly_start = this.getStartOfOffsetLine(x1, y1, dir, kMinWidthT_m, kMinWidthT_m * Math.tan(r), kMinWidthT_m * -Math.tan(r));
          }
          //tail
          switch (a3) {
            case ENDTYPE.OPEN: {
              const right2 = this.kMinWidthC * kMinWidthT_m / 2;
              const left2 = this.kMinWidthC * -kMinWidthT_m / 2;
              poly_end = this.getEndOfOffsetLine(x1, y1, x2, y2, kMinWidthT_m, right2, left2);
              break;
            }
            case ENDTYPE.TURN_LEFT: {
              let [tx1, ty1] = moved_point(x2, y2, dir12, -this.kMage);
              const width_func = (t) => { return kMinWidthT_m; }
              const new_x2 = x2 - this.kMage * (((this.kAdjustTateStep + 4) - param_tate) / (this.kAdjustTateStep + 4));
              cv.drawQBezier(tx1, ty1, x2, y2,
                new_x2, y2, width_func, t => 0, undefined, true, false);
              const param_hane = this.adjustHaneParam(s, x2, y2, others);
              cv.drawTurnLeft(new_x2, y2, kMinWidthT_m, this.kWidth * 4 * Math.min(1 - param_hane / 10, Math.pow(kMinWidthT_m / this.kMinWidthT, 3)));
              poly_end = this.getEndOfLine(x1, y1, tx1, ty1, kMinWidthT_m);
              break;
            }
            case ENDTYPE.LOWER_LEFT_CORNER: {
              const param_kakato = this.adjustKakatoParam(s, others);
              const right2 = this.kAdjustKakatoL[param_kakato] + this.kMinWidthC * kMinWidthT_m;
              const left2 = this.kAdjustKakatoL[param_kakato];
              poly_end = this.getEndOfOffsetLine(x1, y1, x2, y2, kMinWidthT_m, right2, left2);
              break;
            }
            case ENDTYPE.LOWER_RIGHT_CORNER: {
              const param_kakato = this.adjustKakatoParam(s, others);
              const right2 = this.kAdjustKakatoR[param_kakato] + this.kMinWidthC * kMinWidthT_m;
              const left2 = this.kAdjustKakatoR[param_kakato];
              poly_end = this.getEndOfOffsetLine(x1, y1, x2, y2, kMinWidthT_m, right2, left2);
              break;
            }
            case ENDTYPE.LOWER_LEFT_ZH_NEW: {
              if (x1 == x2) {//vertical
                cv.drawNewGTHbox_v(x2, y2, kMinWidthT_m, this.kMinWidthY);
              } else {
                var m = 0;
                if (x1 > x2 && y1 != y2) {
                  m = Math.floor((x1 - x2) / (y2 - y1) * 3);
                }
                cv.drawNewGTHbox(x2 + m, y2, kMinWidthT_m, this.kMinWidthY);
              }
              //in the original implementation, opt2 is calculated to 413 % 100 = 4, and kAdjustKakatoL[4] is manually set to 0. 
              //The appearance is typically remedied by the crossing horizontal line.
              const right2 = this.kMinWidthC * kMinWidthT_m;
              const left2 = 0;
              poly_end = this.getEndOfOffsetLine(x1, y1, x2, y2, kMinWidthT_m, right2, left2);
              break;
            }
            case ENDTYPE.LOWER_LEFT_ZH_OLD: {
              //in the original implementation, opt2 is calculated to 313 % 100 = 3, corresponding to (original) kAdjustKakatoStep.
              const right2 = this.kAdjustKakatoL[this.kAdjustKakatoStep] + this.kMinWidthC * kMinWidthT_m;
              const left2 = this.kAdjustKakatoL[this.kAdjustKakatoStep];
              poly_end = this.getEndOfOffsetLine(x1, y1, x2, y2, kMinWidthT_m, right2, left2);
              break;
            }
            case ENDTYPE.CONNECTING_V: {
              if (y1 == y2) {//horizontal (error)
                console.log("error: connecting_v at the end of the horizontal line")
                cv.drawLine(x1, y1, x2, y2, kMinWidthT_m);
              } else if (x1 == x2) {//vertical
                poly_end.set(0, x2 + kMinWidthT_m, y2 + this.kMinWidthY - 0.001);
                poly_end.set(1, x2 - kMinWidthT_m, y2 + this.kMinWidthY - 0.001);
              } else {
                const rad = Math.atan((y2 - y1) / (x2 - x1));
                const v = (x1 > x2) ? -1 : 1;
                poly_end.set(0, x2 + (kMinWidthT_m * v) / Math.sin(rad), y2);
                poly_end.set(1, x2 - (kMinWidthT_m * v) / Math.sin(rad), y2);
              }
              break;
            }
            case ENDTYPE.LOWER_RIGHT_HT: {
              if (x1 == x2) {//vertical
                cv.drawLowerRightHT_v(x2, y2, kMinWidthT_m, this.kMinWidthY);
              } else {
                cv.drawLowerRightHT(x2, y2, kMinWidthT_m, this.kMinWidthY);
              }
              if (y1 == y2) {//horizontal (error)
                console.log("error: connecting_v at the end of the horizontal line")
                cv.drawLine(x1, y1, x2, y2, kMinWidthT_m);
              } else if (x1 == x2) {//vertical
                poly_end.set(0, x2 + kMinWidthT_m, y2 + this.kMinWidthY);
                poly_end.set(1, x2 - kMinWidthT_m, y2 + this.kMinWidthY);
              } else {
                const rad = Math.atan((y2 - y1) / (x2 - x1));
                const v = (x1 > x2) ? -1 : 1;
                poly_end.set(0, x2 + (kMinWidthT_m * v) / Math.sin(rad), y2);
                poly_end.set(1, x2 - (kMinWidthT_m * v) / Math.sin(rad), y2);
              }
              break;
            }
            default:
              throw ("error: unknown end type at the straight line: "+a3);
              break;
          }
          //body
          poly_start.concat(poly_end);
          cv.addPolygon(poly_start);
        }
        break;
      }



      case STROKETYPE.CURVE: {

        //for CONNECTING_MANUAL stroke (very very tricky implementation)
        if (a2 == STARTTYPE.CONNECTING_MANUAL){
          s[0] = s[0]-1//CURVE -> STRAIGHT
          this.drawAdjustedStroke(cv, s, others)//treat as STRAIGHT line data
          return
        }

        const kMinWidthT_mod = this.kMinWidthT - ~~((s[1] % 10000) / 1000) / 2
        const end_width_factor = (~~(s[2] / 1000) / 2) / this.kMinWidthT
        //head
        if (a2 == STARTTYPE.OPEN) {
          let [x1ext, y1ext] = moved_point(x1, y1, dir12, 1 * this.kMinWidthY * 0.5);
          if (y1ext <= y3) { //from up to bottom
            cv.drawOpenBegin_curve_down(x1ext, y1ext, rad12, kMinWidthT_mod, this.kMinWidthY);
          }
          else { //from bottom to up
            cv.drawOpenBegin_curve_up(x1ext, y1ext, dir12, kMinWidthT_mod, this.kMinWidthY);
          }
        } else if (a2 == STARTTYPE.UPPER_RIGHT_CORNER || a2 == STARTTYPE.ROOFED_THIN) {
          cv.drawUpperRightCorner2(x1, y1, kMinWidthT_mod, this.kMinWidthY, this.kWidth, a2 == STARTTYPE.ROOFED_THIN);
        } else if (a2 == STARTTYPE.UPPER_LEFT_CORNER) {
          let [x1ext, y1ext] = moved_point(x1, y1, dir12, -this.kMinWidthY);
          cv.drawUpperLeftCorner(x1ext, y1ext, dir12, kMinWidthT_mod);//this.kMinWidthC * ?
        }
        //body
        const a2temp = (a2 == STARTTYPE.CONNECTING_V && this.adjustKirikuchiParam(s, others)) ? 100 + a2 : a2;
        this.minchoDrawCurve(x1, y1, x2, y2, x3, y3, a2temp, a3, cv, kMinWidthT_mod, end_width_factor);
        //tail
        switch (a3) {
          case ENDTYPE.TURN_LEFT: {
            let [tx1, ty1] = moved_point(x3, y3, dir23, -this.kMage*0.439);
            const param_hane = this.adjustHaneParam(s, x3, y3, others);
            const width_func = (t) => { return kMinWidthT_mod; }
            cv.drawQBezier(tx1, ty1, x3, y3, x3 - this.kMage, y3, width_func, t => 0, undefined, true, false);
            cv.drawTurnLeft(x3 - this.kMage, y3, kMinWidthT_mod, this.kWidth * 4 * Math.min(1 - param_hane / 10, Math.pow(kMinWidthT_mod / this.kMinWidthT, 3)));
            break;
          }
          case ENDTYPE.TURN_UPWARDS: {
            cv.drawTailCircle(x3, y3, dir23, kMinWidthT_mod);
            cv.drawTurnUpwards_pos(x3, y3, kMinWidthT_mod, this.kWidth*5, (y1<y3)?DIR_POSX:DIR_NEGX);
            break;
          }
          case ENDTYPE.STOP: {
            //let [x3ex, y3ex] = moved_point(x3, y3, dir23, -1 * kMinWidthT_mod * 0.52);
            //cv.drawTailCircle_tan(x3ex, y3ex, dir23, kMinWidthT_mod*1.1, tan1, tan2);
            break;
          }
          default: {
            if (a3 == ENDTYPE.OPEN){
              if (a2 == STARTTYPE.THIN || a2 == STARTTYPE.ROOFED_THIN) {
                cv.drawL2RSweepEnd(x3, y3, dir23, kMinWidthT_mod, this.kL2RDfatten);
              }else if(a2 == STARTTYPE.CONNECT_THIN){
                cv.drawL2RSweepEnd(x3, y3, dir23, kMinWidthT_mod, 1);
              }
            }

            break;
          }
        }
        break;
      }



      case STROKETYPE.BENDING:
      case STROKETYPE.BENDING_ROUND:  {
        const param_tate = a1 == STROKETYPE.BENDING ? this.adjustTateParam(s, others) : 0;
        const param_mage = a1 == STROKETYPE.BENDING ? this.adjustMageParam(s, others) : 0;
        const kMinWidthT_m = this.kMinWidthT - param_tate / 2;
        const kMinWidthT_mage = this.kMinWidthT - param_mage / 2;

        var rate;
        if (a1 == STROKETYPE.BENDING){
          rate=1
        }else{//BENDING_ROUND
          rate = 6;
          if ((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2) < 14400) { // smaller than 120 x 120
            rate = Math.sqrt((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2)) / 120 * 6;
          }
        }

        let [tx1, ty1] = moved_point(x2, y2, dir12, -this.kMage * rate);
        let [tx2, ty2] = moved_point(x2, y2, dir23, this.kMage * rate);
        //first line
        let poly_start = this.getStartOfVLine(x1, y1, x2, y2, a2, kMinWidthT_m, cv);
        const width_func = function (t) {
          return (kMinWidthT_mage - kMinWidthT_m) * t + kMinWidthT_m;
        }        
        let [bez1, bez2] = Bezier.qBezier(tx1, ty1, x2, y2, tx2, ty2, width_func, t => 0, undefined);
        poly_start.concat(Bezier.bez_to_poly(bez1));
        let edd = this.getEndOfLine(tx2, ty2, x3, y3, kMinWidthT_mage);
        poly_start.concat(edd);
        poly_start.concat(Bezier.bez_to_poly(bez2));
        cv.addPolygon(poly_start);
      
        if (y2 == y3) {
          if (tx2 < x3) {
            cv.drawCircle_bend_pos(x3, y3, DIR_POSX, kMinWidthT_mage);
          } else {
            cv.drawCircle_bend_neg(x3, y3, DIR_NEGX, kMinWidthT_mage);
          }
          if (a3 == ENDTYPE.TURN_UPWARDS) {
            if (tx2 < x3) {
              cv.drawTurnUpwards_pos(x3, y3, kMinWidthT_mage, this.kWidth * (4 * (1 - param_mage / this.kAdjustMageStep) + 1), DIR_POSX);
            } else {
              cv.drawTurnUpwards_neg(x3, y3, kMinWidthT_mage, this.kWidth * (4 * (1 - param_mage / this.kAdjustMageStep) + 1), DIR_NEGX);
            }
          }
        } else {
          const dir = get_dir(x3-x2, y3-y2);
          if (tx2 < x3) {
            cv.drawCircle_bend_pos(x3, y3, dir, kMinWidthT_mage);
          } else {
            cv.drawCircle_bend_neg(x3, y3, dir, kMinWidthT_mage);
          }
          if (a3 == ENDTYPE.TURN_UPWARDS) {
            if (tx2 < x3) {
              cv.drawTurnUpwards_pos(x3, y3, kMinWidthT_mage, this.kWidth*5, dir);
            } else {
              cv.drawTurnUpwards_neg(x3, y3, kMinWidthT_mage, this.kWidth*5, dir);
            }
          }
        }
        break;
      }


      case 12: {
        throw "error: unknown stroketype 12";
        break;
      }




      case STROKETYPE.BEZIER: {
        const kMinWidthT_mod = this.kMinWidthT - ~~((s[1] % 10000) / 1000) / 2
        //head
        if (a2 == STARTTYPE.OPEN) {
          let [x1ext, y1ext] = moved_point(x1, y1, dir12, 1 * this.kMinWidthY * 0.5);

          if (y1ext <= y4) { //from up to bottom
            cv.drawOpenBegin_curve_down(x1ext, y1ext, rad12, kMinWidthT_mod, this.kMinWidthY);
          }
          else { //from bottom to up
            cv.drawOpenBegin_curve_up(x1ext, y1ext, dir12, kMinWidthT_mod, this.kMinWidthY);
          }
        } else if (a2 == STARTTYPE.UPPER_RIGHT_CORNER || a2 == STARTTYPE.ROOFED_THIN) {
          cv.drawUpperRightCorner2(x1, y1, kMinWidthT_mod, this.kMinWidthY, this.kWidth, a2 == STARTTYPE.ROOFED_THIN);
        } else if (a2 == STARTTYPE.UPPER_LEFT_CORNER) {
          let [x1ext, y1ext] = moved_point(x1, y1, dir12, -this.kMinWidthY);
          cv.drawUpperLeftCorner(x1ext, y1ext, dir12, kMinWidthT_mod);
        }
        //body
        let [tan1, tan2] = this.minchoDrawBezier(x1, y1, x2, y2, x3, y3, x4, y4, a2, a3, cv, kMinWidthT_mod);
        //tail
        switch (a3) {
          case ENDTYPE.TURN_LEFT:
            let [tx1, ty1] = moved_point(x4, y4, dir34, -this.kMage*0.439);
            const width_func = (t) => { return kMinWidthT_mod; }
            cv.drawQBezier(tx1, ty1, x4, y4, x4 - this.kMage, y4, width_func, t => 0, undefined, true, false);
            const param_hane = this.adjustHaneParam(s, x4, y4, others);
            cv.drawTurnLeft(x4 - this.kMage, y4, kMinWidthT_mod, this.kWidth * 4 * Math.min(1 - param_hane / 10, Math.pow(kMinWidthT_mod / this.kMinWidthT, 3)));
            break;
          case ENDTYPE.TURN_UPWARDS:
            cv.drawTailCircle(x4, y4, dir34, kMinWidthT_mod);
            cv.drawTurnUpwards_pos(x4, y4, kMinWidthT_mod, this.kWidth*5, (y1<y4)?DIR_POSX:DIR_NEGX);
            break;
          case ENDTYPE.STOP:
            let [x4ex, y4ex] = moved_point(x4, y4, dir34, -kMinWidthT_mod * 0.52);
            if(a2 == STARTTYPE.THIN || a2 == STARTTYPE.ROOFED_THIN){
              cv.drawTailCircle_tan(x4ex, y4ex, dir34, kMinWidthT_mod*1.1, tan1, tan2);
            }else{//CONNECT_THIN
              cv.drawTailCircle(x4ex, y4ex, dir34, kMinWidthT_mod);
            }
            break;
          default:
            if ((a2 == STARTTYPE.THIN || a2 == STARTTYPE.ROOFED_THIN) && a3 == ENDTYPE.OPEN) {
              cv.drawL2RSweepEnd(x4, y4, dir34, kMinWidthT_mod, this.kL2RDfatten);
            }
            break;
        }
        break;
      }



      case STROKETYPE.VCURVE: {
        const param_tate = this.adjustTateParam(s, others);
        const kMinWidthT_m = this.kMinWidthT - param_tate / 2;
        //straight
        let poly_start = this.getStartOfVLine(x1, y1, x2, y2, a2, kMinWidthT_m, cv);
        let poly_end = this.getEndOfLine(x1, y1, x2, y2, kMinWidthT_m);
        poly_start.concat(poly_end);
        cv.addPolygon(poly_start);
        //semicircle for connection point
        cv.drawTailCircle(x2, y2, rad_to_dir(rad23 + Math.PI), kMinWidthT_m);
        //curve
        const width_func = function (t) {
          //const deltad = Math.pow(1.0-t,0.7)*0.8+0.2;
          const deltad = (1 - Math.pow(t, 1.8)) * 0.85 + 0.15;
          return deltad * kMinWidthT_m;
        }
        cv.drawQBezier(x2, y2, x3, y3, x4, y4, width_func, t => -1.8 * Math.pow(t, 0.8) * 0.85 * kMinWidthT_m);
        break;
      }
      case 9: // may not be exist ... no need
        //kageCanvas[y1][x1] = 0;
        //kageCanvas[y2][x2] = 0;
        break;
      default:
        throw "error: unknown stroke: "+s;
        break;
    }
  }

  minchoDrawCurve(x1pre, y1pre, sx, sy, x2pre, y2pre, a1, a2, cv, kMinWidthT_mod, end_width_factor) {
    var delta;
    switch (a1) {
      case STARTTYPE.OPEN:
      case STARTTYPE.THIN:
      case STARTTYPE.ROOFED_THIN:
        delta = -1 * this.kMinWidthY * 0.5;
        break;
      case STARTTYPE.UPPER_LEFT_CORNER:
        //case 32:
        delta = this.kMinWidthY;
        break;
      default:
        delta = 0;
        break;
    }
    let [x1, y1] = get_extended_dest(x1pre, y1pre, sx, sy, delta);

    switch (a2) {
      case ENDTYPE.STOP: // get shorten for tail's circle
        delta = -1 * kMinWidthT_mod * 0.52;
        break;
      case ENDTYPE.TURN_LEFT:
        delta = -this.kMage * 0.439;
        break;
      default:
        delta = 0;
        break;
    }
    let [x2, y2] = get_extended_dest(x2pre, y2pre, sx, sy, delta);

    var cornerOffset = 0;
    if((a1 == STARTTYPE.UPPER_RIGHT_CORNER || a1 == STARTTYPE.ROOFED_THIN) && a2 == ENDTYPE.LEFT_SWEEP){
      var sx1 = sx; var sx2 = sx; var sy1 = sy; var sy2 = sy;//元の実装と名前を揃える
      function hypot() {
        return Math.sqrt(arguments[0] * arguments[0] + arguments[1] * arguments[1]);
      }
      var contourLength = hypot(sx1-x1, sy1-y1) + hypot(sx2-sx1, sy2-sy1) + hypot(x2-sx2, y2-sy2);
      if (contourLength < 100){
        cornerOffset = (kMinWidthT_mod > 6) ? (kMinWidthT_mod - 6) * ((100 - contourLength) / 100) : 0;
        x1 += cornerOffset;
      }
    }

    var width_func;
    var width_func_d;
    let bez1, bez2;
    let thin_stop_param;
    if ((a1 == STARTTYPE.THIN || a1 == STARTTYPE.ROOFED_THIN) && a2 == ENDTYPE.STOP) { //stop
      //const slant_cos = 
      const len=Math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
      thin_stop_param = (1 + (len-100)*0.0007);
      
      width_func = t => widfun_stop(t, x1, y1, x2, y2, kMinWidthT_mod)*thin_stop_param;
      width_func_d = t => widfun_stop_d(t, x1, y1, x2, y2, kMinWidthT_mod)*thin_stop_param;
      [bez1, bez2] = Bezier.qBezier2(x1, y1, sx, sy, x2, y2, width_func, width_func_d);
    }
    else {
      if ((a1 == STARTTYPE.THIN || a1 == STARTTYPE.ROOFED_THIN) && a2 == ENDTYPE.OPEN) { // L2RD: fatten
        width_func = t => widfun(t, x1, y1, x2, y2, kMinWidthT_mod) * this.kL2RDfatten;
        width_func_d = t => widfun_d(t, x1, y1, x2, y2, kMinWidthT_mod) * this.kL2RDfatten;
      }
      else if (a1 == STARTTYPE.CONNECTING_V && a2 == ENDTYPE.LOWER_LEFT_CORNER) { //未使用。さんずい用 (実験)
        width_func = t => {return ((1-t)*0.628+Math.pow((1-t),30)*0.600+0.222)*kMinWidthT_mod};
        //don't feel like 'export'ing CURVE_THIN for this experimental change...
        width_func_d = t => {return (-0.628-30*Math.pow((1-t),29)*0.600)*kMinWidthT_mod};
      }
      else if (a1 == STARTTYPE.THIN || a1 == STARTTYPE.ROOFED_THIN || a1 == STARTTYPE.CONNECT_THIN) {
        width_func = t => widfun(t, x1, y1, x2, y2, kMinWidthT_mod);
        width_func_d = t => widfun_d(t, x1, y1, x2, y2, kMinWidthT_mod);
      }
      else if (a2 == ENDTYPE.LEFT_SWEEP) {
        width_func = t => widfun(1 - t, x1, y1, x2, y2, kMinWidthT_mod);
        width_func_d = t => -widfun_d(1 - t, x1, y1, x2, y2, kMinWidthT_mod);
      }
      else {
        if (a2 == ENDTYPE.TURN_LEFT) end_width_factor = 0;
        width_func = t => kMinWidthT_mod * (1 - t*end_width_factor);
        width_func_d = t => -kMinWidthT_mod*end_width_factor;
      }
      [bez1, bez2] = Bezier.qBezier(x1, y1, sx, sy, x2, y2, width_func, width_func_d);
    }
    if (a1 == 132 && x1 != sx) {
      let b1 = bezier_to_y(bez2[bez2.length - 1], y1);
      if (b1) { bez2[bez2.length - 1] = b1; }
      var temp = bez1[0].concat();//deep copy
      let b2 = bezier_to_y(temp.reverse(), y1);
      if (b2) { bez1[0] = b2.reverse(); }
    } else if (40 <=a1 && a1 <= 80) {
      var r = get_rad((x2pre - x1pre) * Math.pow(1.4, (a1%10) - 4.5), y2pre - y1pre)
      if(a1 >= 50){
        r = -r
      }
      if(a1 == 60){
        r = Math.PI* 0.5 // vertical edge
      }
      let b1 = bezier_to_line(bez2[bez2.length - 1], x1, y1, r);
      if (b1) { bez2[bez2.length - 1] = b1; }
      var temp = bez1[0].concat();//deep copy
      let b2 = bezier_to_line(temp.reverse(), x1, y1, r);
      if (b2) { bez1[0] = b2.reverse(); }
    } else if (a1 == 22 && x1 != sx) {
      let b1 = bezier_to_y(bez2[bez2.length - 1], y1);
      if (b1) { bez2[bez2.length - 1] = b1; }
    
    /*} else if (a1 == 22 && x1 != sx && y1 > y2) {
      let b1 = bezier_to_y(bez2[bez2.length - 1], y1);
      if (b1) { bez2[bez2.length - 1] = b1; }
      var temp = bez1[0].concat();//deep copy
      let b2 = bezier_to_y(temp.reverse(), y1 + 1);//??
      if (b2) { bez1[0] = b2.reverse(); }
      */
    }
    var poly = Bezier.bez_to_poly(bez1);
    poly.concat(Bezier.bez_to_poly(bez2));
    if(a1==22){
      poly.push(x1, y1);
    }
    cv.addPolygon(poly);
    if(a2 == ENDTYPE.STOP){
      if(a1 == STARTTYPE.THIN || a1 == STARTTYPE.ROOFED_THIN){
        const bez1e = bez1[bez1.length - 1][3];
      const bez1c2 = bez1[bez1.length - 1][2];
      const bez2s = bez2[0][0];
      const bez2c1 = bez2[0][1];
      const tan1 = [bez1e[0] - bez1c2[0], bez1e[1] - bez1c2[1]];
      const tan2 = [bez2s[0] - bez2c1[0], bez2s[1] - bez2c1[1]];
      const cent_x = (x1 + 4*sx + x2) / 6;
      const cent_y = (y1 + 4*sy + y2) / 6;
      var rad_end = get_dir(x2-cent_x, y2-cent_y);
       cv.drawTailCircle_tan(x2, y2, rad_end, kMinWidthT_mod*1.1*thin_stop_param, tan1, tan2);
      }else{//CONNECT_THIN or others
        const enddir = get_dir(x2-sx, y2-sy);
        cv.drawTailCircle(x2, y2, enddir, kMinWidthT_mod* (1-end_width_factor));
      }
    }
    if (a1 == STARTTYPE.CONNECT_THIN){
      var dir_to_start = get_dir(x1-sx, y1-sy);
      cv.drawTailCircle(x1, y1, dir_to_start, kMinWidthT_mod*CURVE_THIN);
    }
  }

  minchoDrawBezier(x1pre, y1pre, sx1, sy1, sx2, sy2, x2pre, y2pre, a1, a2, cv, kMinWidthT_mod) {
    var delta;
    switch (a1) {
      case STARTTYPE.OPEN:
      case STARTTYPE.THIN:
        delta = -1 * this.kMinWidthY * 0.5;
        break;
      case STARTTYPE.UPPER_LEFT_CORNER:
        //case 32:
        delta = this.kMinWidthY;
        break;
      default:
        delta = 0;
        break;
    }
    let [x1, y1] = get_extended_dest(x1pre, y1pre, sx1, sy1, delta);

    switch (a2) {
      case ENDTYPE.STOP: // get shorten for tail's circle
        delta = -1 * kMinWidthT_mod * 0.52;
        break;
      case ENDTYPE.TURN_LEFT:
        delta = -this.kMage*0.439;
        break;
      default:
        delta = 0;
        break;
    }
    let [x2, y2] = get_extended_dest(x2pre, y2pre, sx2, sy2, delta);

    var cornerOffset = 0;
    if((a1 == STARTTYPE.UPPER_RIGHT_CORNER || a1 == STARTTYPE.ROOFED_THIN) && a2 == ENDTYPE.LEFT_SWEEP){
      function hypot() {
        return Math.sqrt(arguments[0] * arguments[0] + arguments[1] * arguments[1]);
      }
      var contourLength = hypot(sx1-x1, sy1-y1) + hypot(sx2-sx1, sy2-sy1) + hypot(x2-sx2, y2-sy2);
      if (contourLength < 100){
        cornerOffset = (kMinWidthT_mod > 6) ? (kMinWidthT_mod - 6) * ((100 - contourLength) / 100) : 0;
        x1 += cornerOffset;
      }
    }

    var width_func;
    var width_func_d;
    let bez1, bez2;
    
    if ((a1 == STARTTYPE.THIN || a1 == STARTTYPE.ROOFED_THIN) && a2 == ENDTYPE.STOP) { //stop
      width_func = t => widfun_stop(t, x1, y1, x2, y2, kMinWidthT_mod);
      width_func_d = t => widfun_stop_d(t, x1, y1, x2, y2, kMinWidthT_mod);

      [bez1, bez2] = Bezier.cBezier(x1, y1, sx1, sy1, sx2, sy2, x2, y2, width_func, width_func_d);

      //width_func = t => widfun_fat(t, x1, y1, x2, y2, kMinWidthT_mod);
      //width_func_d = t => widfun_fat_d(t, x1, y1, x2, y2, kMinWidthT_mod);
      //[bez1, bez2] = Bezier.cBezier_slant(x1, y1, sx1, sy1, sx2, sy2, x2, y2, width_func, width_func_d);
    }
    else {
      if ((a1 == STARTTYPE.THIN || a1 == STARTTYPE.ROOFED_THIN) && a2 == ENDTYPE.OPEN) { // L2RD: fatten
        width_func = t => widfun(t, x1, y1, x2, y2, kMinWidthT_mod) * this.kL2RDfatten;
        width_func_d = t => widfun_d(t, x1, y1, x2, y2, kMinWidthT_mod) * this.kL2RDfatten;
      }
      else if (a1 == STARTTYPE.THIN || a1 == STARTTYPE.ROOFED_THIN || a1 == STARTTYPE.CONNECT_THIN) {
        width_func = t => widfun_fat(t, x1, y1, x2, y2, kMinWidthT_mod);
        width_func_d = t => widfun_fat_d(t, x1, y1, x2, y2, kMinWidthT_mod);
      }
      else if (a2 == ENDTYPE.LEFT_SWEEP) {
        width_func = t => widfun(1 - t, x1, y1, x2, y2, kMinWidthT_mod);
        width_func_d = t => -widfun_d(1 - t, x1, y1, x2, y2, kMinWidthT_mod);
      }
      else {
        width_func = t => kMinWidthT_mod;
        width_func_d = t => 0;
      }
      [bez1, bez2] = Bezier.cBezier(x1, y1, sx1, sy1, sx2, sy2, x2, y2, width_func, width_func_d);
    }
    //以下は今は実行されないコードだが実行時には2次ベジエのときと同様にdeep copyが必要か？
    if (a1 == 132 && x1 != sx1) {
      let b1 = bezier_to_y(bez2[bez2.length - 1], y1);
      if (b1) { bez2[bez2.length - 1] = b1; }
      var temp = bez1[0];
      let b2 = bezier_to_y(temp.reverse(), y1);
      if (b2) { bez1[0] = b2.reverse(); }
    } else if (a1 == 22 && x1 > sx1) {
      let b1 = bezier_to_y(bez2[bez2.length - 1], y1);
      if (b1) { bez2[bez2.length - 1] = b1; }
      var temp = bez1[0];
      let b2 = bezier_to_y(temp.reverse(), y1 + 1);//" + 1" ??
      if (b2) { bez1[0] = b2.reverse(); }
    }
    var poly = Bezier.bez_to_poly(bez1);
    poly.concat(Bezier.bez_to_poly(bez2));
    cv.addPolygon(poly);

    if (a1 == STARTTYPE.CONNECT_THIN){
      var dir_to_start = get_dir(x1-sx1, y1-sy1);
      cv.drawTailCircle(x1, y1, dir_to_start, kMinWidthT_mod*CURVE_THIN);
    }

    const bez1e = bez1[bez1.length - 1][3];
    const bez1c2 = bez1[bez1.length - 1][2];

    const bez2s = bez2[0][0];
    const bez2c1 = bez2[0][1];
    const tan1 = [bez1e[0] - bez1c2[0], bez1e[1] - bez1c2[1]];
    const tan2 = [bez2s[0] - bez2c1[0], bez2s[1] - bez2c1[1]];
    
    return [tan1, tan2];
  }

  getStartOfVLine(x1, y1, x2, y2, a1, kMinWidthT, cv) {
    const rad = get_rad(x2 - x1, y2 - y1);
    const dir = get_dir(x2 - x1, y2 - y1);
    var poly_start = new Polygon(2);
    if (dir.cos==0) {//vertical
      var left1, right1;
      switch (a1) {
        case 0:
          right1 = -this.kMinWidthT * 0.5;
          left1 =  -this.kMinWidthT * 0.7;
          break;
        case 12:
          right1 = this.kMinWidthY + kMinWidthT;
          left1 = this.kMinWidthY;
          break;
        case 32:
          right1 = this.kMinWidthY - 0.001;
          left1 = this.kMinWidthY - 0.001;
          break;
        case 1:
        case 6: //... no need
        case 22:
        default:
          right1 = 0;
          left1 = 0;
          break;
      }
      poly_start = this.getStartOfOffsetLine(x1, y1, dir, kMinWidthT, right1, left1);
      if (a1 == 22) { //box's right top corner
        cv.drawUpperRightCorner_straight_v(x1, y1, kMinWidthT, this.kMinWidthYY, this.kWidth);
      }
      if (a1 == 0) { //beginning of the stroke
        cv.drawOpenBegin_straight(x1, y1, kMinWidthT, this.kMinWidthYY, rad);
      }
    } else {
      const v = 1 //previously (x1 > x2) ? -1 : 1;
      if (a1 == 22) {
        if (dir.sin==0) {//error
          console.log("error: connecting_v at the end of the horizontal line")
          poly_start = this.getStartOfLine(x1, y1, dir, kMinWidthT);
        } else {
          //poly_start.set(1, x1 + (kMinWidthT * v + 1) / Math.sin(rad), y1 + 1);//" + 1" ??
          poly_start.set(1, x1 + (kMinWidthT * v) / Math.sin(rad), y1);
          poly_start.set(0, x1 - (kMinWidthT * v) / Math.sin(rad), y1);
        }
      } else if (a1 == 32) {
        if (dir.sin==0) {//error
          console.log("error: connecting_v at the end of the horizontal line")
          poly_start = this.getStartOfLine(x1, y1, dir, kMinWidthT);
        } else {
          poly_start.set(1, x1 + (kMinWidthT * v) / Math.sin(rad), y1);
          poly_start.set(0, x1 - (kMinWidthT * v) / Math.sin(rad), y1);
        }
      } else {
        var left1, right1;
        switch (a1) {
          case 0:
            right1 = -this.kMinWidthT * 0.5;
            left1 = -this.kMinWidthT * 0.7;
            break;
          case 12:
            right1 = this.kMinWidthY + kMinWidthT;
            left1 = this.kMinWidthY;
            break;
          case 1:
          case 6:
          default:
            right1 = 0;
            left1 = 0;
            break;
        }
        poly_start = this.getStartOfOffsetLine(x1, y1, dir, kMinWidthT, right1, left1);
      }
      if (a1 == 22) { //SHIKAKU MIGIUE UROKO NANAME DEMO MASSUGU MUKI
        cv.drawUpperRightCorner(x1, y1, kMinWidthT, this.kMinWidthYY, this.kWidth);
      }
      if (a1 == 0) { //beginning of the stroke
        cv.drawOpenBegin_straight(x1, y1, kMinWidthT, this.kMinWidthYY, rad);
      }
    }
    return poly_start;
  }

  getStartOfLine(x1, y1, dir, halfWidth) {
    //get polygon data for the start of line
    var poly = new Polygon(2);
    poly.set(1, x1 + dir.sin * halfWidth,
      y1 - dir.cos * halfWidth);
    poly.set(0, x1 - dir.sin * halfWidth,
      y1 + dir.cos * halfWidth);
    return poly;
  }

  getStartOfOffsetLine(x1, y1, dir, halfWidth, off_right1, off_left1) {
    //get polygon data for the start of line (with offset)
    var poly = new Polygon(2);
    poly.set(1, x1 + dir.sin * halfWidth - dir.cos * off_left1,
      y1 - dir.cos * halfWidth - dir.sin * off_left1);
    poly.set(0, x1 - dir.sin * halfWidth - dir.cos * off_right1,
      y1 + dir.cos * halfWidth - dir.sin * off_right1);
    return poly;
  }

  getEndOfLine(x1, y1, x2, y2, halfWidth) {
    //get polygon data for the end of line
    const dir = get_dir(x2 - x1, y2 - y1);
    var poly = new Polygon(2);
    poly.set(0, x2 + dir.sin * halfWidth,
      y2 - dir.cos * halfWidth);
    poly.set(1, x2 - dir.sin * halfWidth,
      y2 + dir.cos * halfWidth);
    return poly;
  }

  getEndOfOffsetLine(x1, y1, x2, y2, halfWidth, off_right2, off_left2) {
    //get polygon data for the end of line (with offset)
    const dir = get_dir(x2 - x1, y2 - y1);
    var poly = new Polygon(2);
    poly.set(0, x2 + dir.sin * halfWidth + off_left2 * dir.cos,
      y2 - dir.cos * halfWidth + off_left2 * dir.sin);
    poly.set(1, x2 - dir.sin * halfWidth + off_right2 * dir.cos,
      y2 + dir.cos * halfWidth + off_right2 * dir.sin);
    return poly;
  }

  //functions for adjustment

  adjustTateParam(stroke, others) { // strokes
    //for illegal strokes
    if (stroke[1] >= 1000) return ~~((stroke[1] % 10000) / 1000);
    if (stroke[0] >= 100) return 0;
    
    //(STROKETYPE.STRAIGHT || STROKETYPE.BENDING || STROKETYPE.VCURVE)
    if (stroke[3] != stroke[5]) return 0;
    var res_arr = [];
    for (let other of others) {
      if ((other[0] == 1 || other[0] == 3 || other[0] == 7) && other[3] == other[5] &&
        !(stroke[4] + 1 > other[6] || stroke[6] - 1 < other[4]) &&
        Math.abs(stroke[3] - other[3]) < this.kMinWidthT_adjust * this.kAdjustTateStep) {
        res_arr.push(this.kAdjustTateStep - Math.floor(Math.abs(stroke[3] - other[3]) / this.kMinWidthT_adjust));
      }
    }
    const kAdjustTateStep_org = 4;//original implementation
    var res = res_arr.reduce((acc, val) => norm2(acc, val), 0)*1.1//1.1を取ってnorm2ではなく+にすると以前と同じ
    
    res = Math.min(res, kAdjustTateStep_org);
    return res;//a2 += res * 1000
  }

  adjustUrokoParam(stroke, others) { // strokes
    //for illegal strokes
    if (stroke[2] >= 100) return ~~((stroke[2] % 1000) / 100);
    if (stroke[0] >= 100) return 0;

    //STROKETYPE.STRAIGHT && ENDTYPE.OPEN
    for (var k = 0; k < this.kAdjustUrokoLengthStep; k++) {
      var tx, ty, tlen;
      if (stroke[4] == stroke[6]) { // YOKO
        tx = stroke[5] - this.kAdjustUrokoLine[k];
        ty = stroke[6] - 0.5;
        tlen = stroke[5] - stroke[3];
      } else {
        var rad = Math.atan((stroke[6] - stroke[4]) / (stroke[5] - stroke[3]));
        tx = stroke[5] - this.kAdjustUrokoLine[k] * Math.cos(rad) - 0.5 * Math.sin(rad);
        ty = stroke[6] - this.kAdjustUrokoLine[k] * Math.sin(rad) - 0.5 * Math.cos(rad);
        tlen = Math.sqrt((stroke[6] - stroke[4]) * (stroke[6] - stroke[4]) +
          (stroke[5] - stroke[3]) * (stroke[5] - stroke[3]));
      }
      if (tlen < this.kAdjustUrokoLength[k] ||
        isCrossWithOthers(others, -1, tx, ty, stroke[5], stroke[6])
      ) {
        return (this.kAdjustUrokoLengthStep - k);
      }
    }
    return 0;//a3 += res * 100;
  }

  adjustUroko2Param(stroke, others) { // strokes
    //for illegal strokes
    if (stroke[2] >= 100) return ~~((stroke[2] % 1000) / 100);
    if (stroke[0] >= 100) return 0;

    //STROKETYPE.STRAIGHT && ENDTYPE.OPEN && y1==y2
    var pressures = [];
    for (let other of others) {
      if (
        (other[0] == 1 && other[4] == other[6] &&
          !(stroke[3] + 1 > other[5] || stroke[5] - 1 < other[3]) &&
          Math.abs(stroke[4] - other[4]) < this.kAdjustUroko2Length) ||
        (other[0] == 3 && other[6] == other[8] &&
          !(stroke[3] + 1 > other[7] || stroke[5] - 1 < other[5]) &&
          Math.abs(stroke[4] - other[6]) < this.kAdjustUroko2Length)
      ) {
        pressures.push(Math.pow(this.kAdjustUroko2Length - Math.abs(stroke[4] - other[6]), 1.1));
      }
    }
    var pressure = pressures.reduce((acc, val) => norm2(acc, val), 0)*1.9//1.7を取ってnorm2ではなく+にすると以前と同じ
    var result = Math.min(Math.floor(pressure / this.kAdjustUroko2Length), this.kAdjustUroko2Step);
    return result;//a3 += res * 100;
  }

  adjustHaneParam(stroke, epx, epy, others) { // adjust "Hane" (short line turning to the left)
    //for illegal strokes
    if (stroke[2] >= 100) return ~~((stroke[2] % 1000) / 100);
    if (stroke[0] >= 100) return 0;

    //endPointX, endPointY
    //(STROKETYPE.STRAIGHT || STROKETYPE.CURVE || STROKETYPE.BEZIER) && ENDTYPE.TURN_LEFT
    var res = 0;
    var nearest = Infinity; // the nearest point to the short line
    if (epx + 18 < 100) {
      nearest = epx + 18;
    }
    for (let other of others) {
      if (other[0] == STROKETYPE.STRAIGHT && other[3] == other[5] && other[3] < epx && other[4] <= epy && other[6] >= epy) {
        if (epx - other[3] < 100) {
          nearest = Math.min(nearest, epx - other[3]);
        }
      }
    }
    if (nearest != Infinity) {
      res = 7 - Math.floor(nearest / 15);
    }
    return res;//a3 += res * 100;
  }

  adjustMageParam(stroke, others) {
    //for illegal strokes
    if (stroke[2] >= 1000) return ~~((stroke[2] % 10000) / 1000);
    if (stroke[0] >= 100) return 0;

    //STROKETYPE.BENDING
    //applied only if y2=y3
    if (stroke[6] != stroke[8]) return 0;
    var res0_above = [];
    var res0_below = [];
    for (let other of others) {
      if (
        (other[0] == 1 && other[4] == other[6] &&
          !(stroke[5] + 1 > other[5] || stroke[7] - 1 < other[3]) &&
          Math.abs(stroke[6] - other[4]) < this.kMinWidthT_adjust * this.kAdjustMageStep) ||
        (other[0] == 3 && other[6] == other[8] &&
          !(stroke[5] + 1 > other[7] || stroke[7] - 1 < other[5]) &&
          Math.abs(stroke[6] - other[6]) < this.kMinWidthT_adjust * this.kAdjustMageStep)
      ) {
        var p = this.kAdjustMageStep - Math.floor(Math.abs(stroke[6] - other[6]) / this.kMinWidthT_adjust);
        if ((other[0] == 1 && stroke[6] < other[4]) || (other[0] == 3 && stroke[6] < other[6])) //lines "above" the hane
        {
          res0_above.push(p);
        }else{
          res0_below.push(p);
        }
      }
    }
    var res_above = res0_above.reduce((acc, val) => Math.max(acc, val), 0)
    var res_below = res0_below.reduce((acc, val) => Math.max(acc, val), 0)*1.3
    var res = Math.max(res_above, res_below)//1.3とかを外して上2行を含めたmaxとかnorm2を+にすると以前と同じ
    
    const maxlen = (stroke[6] - stroke[4]) * 0.6//y2-y1から算出
    const res2 = maxlen <= 0 ? 0 : (1 - (maxlen/this.kWidth - 1)/4 ) * this.kAdjustMageStep//"this.kWidth * (4 * (1 - param_mage / this.kAdjustMageStep) + 1)" を参考に逆算
    res = Math.max(res,res2)//小数値が返るため、問題が出る可能性もある？今のところ問題なし
    res = Math.min(res, this.kAdjustMageStep);
    return res;//a3 += res * 1000;
  }

  adjustKirikuchiParam(stroke, others) { // connecting to other strokes.
    //for illegal strokes
    if (~~((stroke[1]%1000) / 100) == 1) return true;
    if (stroke[0] >= 100) return false;

    //STROKETYPE.CURVE, STARTTYPE.CONNECTING_V
    if (stroke[3] > stroke[5] &&
      stroke[4] < stroke[6]) {
      for (let other of others) {
        if (other[0] == 1 &&
          other[3] < stroke[3] && other[5] > stroke[3] &&
          other[4] == stroke[4] && other[4] == other[6]) {
          return true;
        }
      }
    }
    return false;
    //if (res) a2 += 100;
  }
  
  adjustKakatoParam(stroke, others) {
    //for illegal strokes
    if (stroke[2] >= 100) return ~~((stroke[2] % 1000) / 100);
    if (stroke[0] >= 100) return 0;

    
    //if (STROKETYPE.STRAIGHT && (LOWER_LEFT_CORNER || LOWER_RIGHT_CORNER))
    for (var k = 0; k < this.kAdjustKakatoStep; k++) {
      if (isCrossBoxWithOthers(others, -1,
        stroke[5] - this.kAdjustKakatoRangeX / 2,
        stroke[6] + this.kAdjustKakatoRangeY[k],
        stroke[5] + this.kAdjustKakatoRangeX / 2,
        stroke[6] + this.kAdjustKakatoRangeY[k + 1])
        | stroke[6] + this.kAdjustKakatoRangeY[k + 1] > 200 // adjust for baseline
        | stroke[6] - stroke[4] < this.kAdjustKakatoRangeY[k + 1] // for thin box
      ) {
        return (this.kAdjustKakatoStep - k);
      }
    }
    return 0;
    //a3 += res * 100;
  }
}