#include "mincho.h"

#include "defs.h"


constinit const kage::MinchoSets kage::MinchoSets::ONE;


kage::Drawing kage::Mincho::draw(std::span<const Stroke> strokes) const
{
    Drawing r;
    for (auto& v : strokes) {
        drawAdjusted(r, v, strokes);
    }
    return r;
}

void kage::Mincho::drawAdjusted(
        Drawing& cv, const Stroke& s, std::span<const Stroke>& all) const
{
    unsigned a1 = s.type % 100;
    unsigned a2 = s.v1 % 100;
    unsigned a3 = (s.v2 == end::LOWER_LEFT_ZH_OLD || s.v2 == end::LOWER_LEFT_ZH_NEW) ? s.v2 : s.v2 % 100;
    Point<int> p1 = s.p3_4();
    Point<int> p2 = s.p5_6();
    Point<int> p3 = s.p7_8();
    Point<int> p4 = s.p9_10();
    //if(a2>100){
    //  console.log("error: start type"+a2)
    //}
    //if(a3>100){
    //  console.log("error: end type"+a3)
    //}
    auto v12 = p2 - p1;
    auto dir12 = v12.dir();
    auto dir23 = (p3 - p2).dir();
    auto dir34 = (p4 - p3).dir();
    auto rad12 = dir12.rad();
    auto rad23 = dir23.rad();

    switch (a1) {
    case 0: //rotate and flip
        if (a2 == 98) {
          //cv.flipLeftRight(x1, y1, x2, y2);
        } else if (a2 == 97) {
          //cv.flip_up_down(x1, y1, x2, y2);
        } else if (a2 == 99 && a3 == 1) {
          //cv.rotate90(x1, y1, x2, y2);
        } else if (a2 == 99 && a3 == 2) {
          //cv.rotate180(x1, y1, x2, y2);
        } else if (a2 == 99 && a3 == 3) {
          //cv.rotate270(x1, y1, x2, y2);
        }
        break;
    case stroke::STRAIGHT: {
            auto dir = dir12;
            if (a3 == end::CONNECTING_H) {//usually horizontal
                cv.drawLine(p1, p2, sets.kMinWidthYY);
            } else if (a3 == end::OPEN && std::abs(v12.y) < v12.x) { //horizontal or gentle slope
                /// @todo [urgent] uroko
                /*
                const param_uroko = this.adjustUrokoParam(s, others);
                const param_uroko2 = this.adjustUroko2Param(s, others);
                */
                cv.drawLine(p1, p2, sets.kMinWidthYY);
                Float urokoScale = (sets.kMinWidthU / sets.kMinWidthY - 1) / 4 + 1;
                /// @todo [urgent] uroko
                /*
                if (v12.y == 0) {//horizontal
                    const uroko_max = Math.floor(norm2(param_uroko, param_uroko2))
                    //const uroko_max = param_uroko == 0 ? param_uroko2 : param_uroko
                    //↑元の実装だとadjustUrokoによる調整がかかったものはadjustUroko2を一切通らないのでそれ以上小さくならない。
                    //Math.max(param_uroko, param_uroko2) などのほうが合理的
                    cv.drawUroko_h(x2, y2, this.kMinWidthYY, this.kAdjustUrokoX[uroko_max] * urokoScale, this.kAdjustUrokoY[uroko_max] * urokoScale);
                } else {
                    cv.drawUroko(x2, y2, dir, this.kMinWidthYY, this.kAdjustUrokoX[param_uroko] * urokoScale, this.kAdjustUrokoY[param_uroko] * urokoScale);
                }
                */
            } else {//vertical or steep slope
            /// @todo [urgent] tate
         /*
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
          */
            }
        } break;

    case stroke::CURVE: {
    /*

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
        */
        break;
    }



    case stroke::BENDING:
    case stroke::BENDING_ROUND:  {
    /*
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
        */
        } break;


    case 12:
        throw std::logic_error("Uknown stroketype 12");

    case stroke::BEZIER: {
        /*
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
        */
        } break;

    case stroke::VCURVE: {
        /*
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
        */
      }
    case 9: // may not be exist ... no need
        break;
    default: {
            char buf[100];
            snprintf(buf, std::size(buf), "Unknown stroke %u", a1);
            throw buf;
        }
    }
}
