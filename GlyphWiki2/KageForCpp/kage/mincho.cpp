#include "mincho.h"

#include "defs.h"
#include "2d.h"

constinit const kage::MinchoSets kage::MinchoSets::ONE;
using namespace kage_literals;


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
    auto v23 = p3 - p2;
    const auto dir12 = v12.dir();
    const auto dir23 = v23.dir();
    const auto dir34 = (p4 - p3).dir();
    const auto rad12 = dir12.rad();
    const auto rad23 = dir23.rad();

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
        Float param_tate = (a1 == stroke::BENDING) ? adjustTateParam(s, all) : 0;
        Float param_mage = (a1 == stroke::BENDING) ? adjustMageParam(s, all) : 0;
        Float kMinWidthT_m = sets.kMinWidthT - param_tate / 2;
        Float kMinWidthT_mage = sets.kMinWidthT - param_mage / 2;

        Float rate;
        if (a1 == stroke::BENDING) {
            rate = 1;
        } else {//BENDING_ROUND
            rate = 6;
            auto area = v23.len2();
            if (area < 14400) { // smaller than 120 x 120
                rate = std::sqrt(Float(area)) / 120 * 6;
            }
        }
        Float dist = sets.kMage * rate;
        auto t1 = movedPoint(p2, dir12, -dist);
        auto t2 = movedPoint(p2, dir23,  dist);
        //first line
        auto polyStart = getStartOfVLine(p1, p2, a2, kMinWidthT_m, cv);
        auto widthFunc = [&](Float t) {
            return (kMinWidthT_mage - kMinWidthT_m) * t + kMinWidthT_m;
        };
        /*
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


kage::Float kage::Mincho::adjustTateParam(const Stroke& stroke, std::span<const Stroke>& all) const
{
    //for illegal strokes
    if (stroke.v1 >= 1000)
      return std::floor((stroke.v1 % 10000) / 1000);
    if (stroke.type >= 100) return 0;

    //(STROKETYPE.STRAIGHT || STROKETYPE.BENDING || STROKETYPE.VCURVE)
    if (stroke.v3 != stroke.v5)
        return 0;
    Float sumSq = 0;
    std::vector<Float> res_arr;
    for (auto& other : all) {
        if (&other == &stroke)
            continue;
        switch (other.type) {
        case stroke::STRAIGHT:  // 1
        case stroke::BENDING:   // 3
        case stroke::VCURVE:
            if ((stroke.v4 + 1 > other.v6 || stroke.v6 - 1 < other.v4) &&
                    std::abs(stroke.v3 - other.v3) < sets.kMinWidthT_adjust * sets.kAdjustTateStep) {
                auto val = (sets.kAdjustTateStep - std::floor(std::abs(stroke.v3 - other.v3) / sets.kMinWidthT_adjust));
                sumSq += val;
            }
        }
    }

    constexpr Float kAdjustTateStep_org = 4;//original implementation
    auto res = std::sqrt(sumSq) * Float(1.1);  //1.1を取ってnorm2ではなく+にすると以前と同じ

    return std::min(res, kAdjustTateStep_org);
    return res;//a2 += res * 1000
}

kage::Float kage::Mincho::adjustMageParam(const Stroke& stroke, std::span<const Stroke>& all) const
{
    //for illegal strokes
    if (stroke.v2 >= 1000) return std::floor((stroke.v2 % 10000) / 1000);
    if (stroke.type >= 100) return 0;

  //STROKETYPE.BENDING
  //applied only if y2=y3
    if (stroke.v6 != stroke.v8) return 0;
    Float resAbove = 0, resBelow = 0;
    for (auto& other : all) {
        if (&other == &stroke)
            continue;
        if (
            (other.type == stroke::STRAIGHT && other.v4 == other.v6 &&
                !(stroke.v5 + 1 > other.v5 || stroke.v7 - 1 < other.v3) &&
                std::abs(stroke.v6 - other.v4) < sets.kMinWidthT_adjust * sets.kAdjustMageStep) ||
            (other.type == stroke::BENDING && other.v6 == other.v8 &&
                !(stroke.v5 + 1 > other.v7 || stroke.v7 - 1 < other.v3) &&
                std::abs(stroke.v6 - other.v6) < sets.kMinWidthT_adjust * sets.kAdjustMageStep)) {
            Float p = sets.kAdjustMageStep - std::floor(std::abs(stroke.v6 - other.v6) / sets.kMinWidthT_adjust);
            if ((other.type == stroke::STRAIGHT && stroke.v6 < other.v4)
                    || (other.type == stroke::BENDING && stroke.v6 < other.v6)) { //lines "above" the hane
                resAbove = std::max(resAbove, p);
            } else {
                resBelow = std::max(resBelow, p);
            }
        }
    }
    resBelow *= Float(1.3);
    auto res = std::max(resAbove, resBelow); //1.3とかを外して上2行を含めたmaxとかnorm2を+にすると以前と同じ

    Float maxlen = (stroke.v6 - stroke.v4) * 0.6_f; //y2-y1から算出
    auto res2 = (maxlen <= 0) ? 0 : (1 - (maxlen/sets.kWidth - 1)/4 ) * sets.kAdjustMageStep; //"this.kWidth * (4 * (1 - param_mage / this.kAdjustMageStep) + 1)" を参考に逆算
    res = std::max(res,res2); //小数値が返るため、問題が出る可能性もある？今のところ問題なし
    res = std::min(res, sets.kAdjustMageStep);
    return res;//a3 += res * 1000;
}


kage::Polygon kage::Mincho::getStartOfVLine(
        kage::Point<int> p1, kage::Point<int> p2, int a1, Float kMinWidthT, Drawing& cv) const
{
    auto dir = (p2 - p1).dir();
    Polygon poly_start;
    if (dir.cos == 0) {//vertical
        Float left1, right1;
        switch (a1) {
        case 0:
            right1 = -sets.kMinWidthT * Float(0.5);
            left1 =  -sets.kMinWidthT * Float(0.7);
            break;
        case 12:
            right1 = sets.kMinWidthY + kMinWidthT;
            left1 = sets.kMinWidthY;
            break;
        case 32:
            right1 = sets.kMinWidthY - 0.001;
            left1 = sets.kMinWidthY - 0.001;
            break;
        case 1:
        case 6: //... no need
        case 22:
        default:
            right1 = 0;
            left1 = 0;
            break;
        }
        poly_start = getStartOfOffsetLine(p1, dir, kMinWidthT, right1, left1);
        if (a1 == 22) { //box's right top corner
            /// @todo [urgent] drawing here
            /*
            cv.drawUpperRightCorner_straight_v(x1, y1, kMinWidthT, this.kMinWidthYY, this.kWidth);
            */
        }
        if (a1 == 0) { //beginning of the stroke
            /// @todo [urgent] drawing here
            /*
            cv.drawOpenBegin_straight(x1, y1, kMinWidthT, this.kMinWidthYY, rad);
            */
        }
    } else {
        const Float v = 1; //previously (x1 > x2) ? -1 : 1;
        if (a1 == 22) {
            if (dir.isSinSmall()) { //error
                //console.log("error: connecting_v at the end of the horizontal line")
                poly_start = getStartOfLine(p1, dir, kMinWidthT);
            } else {
                //poly_start.set(1, x1 + (kMinWidthT * v + 1) / Math.sin(rad), y1 + 1);//" + 1" ??
                poly_start.resize(2);
                poly_start.set(1, p1.x + (kMinWidthT * v) / dir.sin, p1.y);
                poly_start.set(0, p1.x - (kMinWidthT * v) / dir.sin, p1.y);
            }
        } else if (a1 == 32) {
            if (dir.isSinSmall()) { //error
                //console.log("error: connecting_v at the end of the horizontal line")
                poly_start = getStartOfLine(p1, dir, kMinWidthT);
            } else {
                poly_start.resize(2);
                poly_start.set(1, p1.x + (kMinWidthT * v) / dir.sin, p1.y);
                poly_start.set(0, p1.x - (kMinWidthT * v) / dir.sin, p1.y);
            }
        } else {
            Float left1, right1;
            switch (a1) {
            case 0:
                right1 = -sets.kMinWidthT * 0.5_f;
                left1  = -sets.kMinWidthT * 0.7_f;
                break;
            case 12:
                right1 = sets.kMinWidthY + kMinWidthT;
                left1  = sets.kMinWidthY;
                break;
            case 1:
            case 6:
            default:
                right1 = 0;
                left1 = 0;
                break;
            }
            poly_start = getStartOfOffsetLine(p1, dir, kMinWidthT, right1, left1);
        }
        if (a1 == 22) { //SHIKAKU MIGIUE UROKO NANAME DEMO MASSUGU MUKI
            /// @todo [urgent] drawing here
            /*
          cv.drawUpperRightCorner(x1, y1, kMinWidthT, this.kMinWidthYY, this.kWidth);
          */
        }
        if (a1 == 0) { //beginning of the stroke
            /// @todo [urgent] drawing here
            /*
          cv.drawOpenBegin_straight(x1, y1, kMinWidthT, this.kMinWidthYY, rad);
          */
        }
    }
    return poly_start;
}


kage::Polygon kage::Mincho::getStartOfLine(
        Point<Float> pt, Dir dir, Float halfWidth) const
{
    //get polygon data for the start of line
    Polygon poly(2);
    poly.set(1, pt.x + dir.sin * halfWidth,
                pt.y - dir.cos * halfWidth);
    poly.set(0, pt.x - dir.sin * halfWidth,
                pt.y + dir.cos * halfWidth);
    return poly;
}

kage::Polygon kage::Mincho::getStartOfOffsetLine(
        Point<Float> pt, Dir dir, Float halfWidth, Float off_right1, Float off_left1) const
{
  //get polygon data for the start of line (with offset)
    Polygon poly(2);
    poly.set(1, pt.x + dir.sin * halfWidth - dir.cos * off_left1,
                pt.y - dir.cos * halfWidth - dir.sin * off_left1);
    poly.set(0, pt.x - dir.sin * halfWidth - dir.cos * off_right1,
                pt.y + dir.cos * halfWidth - dir.sin * off_right1);
    return poly;
}
