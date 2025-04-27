import {unit_normal_vector, rad_to_vector, get_rad} from "./util.js";
import {Polygon} from "./polygon.js";
import {fitCubic_tang, fitCurve} from "./fit-curve.js";
const bezier_steps = 200;
export class Bezier{
  static generalBezier(x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d){
    var a1 = [];
    var a2 = [];
    var tang1 = [];
    var tang2 = [];
    for (var tt = 0; tt <= bezier_steps; tt++) {
      const t = tt / bezier_steps;
      const x = x_fun(t);
      const y = y_fun(t);
      const vx = dx_fun(t);
      const vy = dy_fun(t);

      let [ia, ib] = unit_normal_vector(vx, vy);
      const deltad = width_func(t);
      ia = ia * deltad;
      ib = ib * deltad;
      
      const rad = get_rad(vx, vy);
      const velocity = Math.sqrt(vx*vx+vy*vy);
      const width_rad = Math.atan(width_func_d(t)/velocity);
      a1.push([x - ia, y - ib]);
      a2.push([x + ia, y + ib]);
      tang1.push(rad_to_vector(rad-width_rad));
      tang2.push(rad_to_vector(rad+width_rad-Math.PI));
    }
    //const bez1 = fitCubic_tang(a1, tang1, 0.03);
    //const bez2 = fitCubic_tang(a2.reverse(), tang2.reverse(), 0.03);
    const bez1 = fitCurve(a1, 0.03);
    const bez2 = fitCurve(a2.reverse(), 0.03);
    return [bez1, bez2];
  }
  static generalBezier2(x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d, dir_func){
    //offset vector (ia, ib) is calculated with dir_func
    var a1 = [];
    var a2 = [];
    var tang1 = [];
    var tang2 = [];
    for (var tt = 0; tt <= bezier_steps; tt++) {
      const t = tt / bezier_steps;
      const x = x_fun(t);
      const y = y_fun(t);
      const vx = dx_fun(t);
      const vy = dy_fun(t);

      let [ia, ib] = dir_func(t);
      const deltad = width_func(t);
      ia = ia * deltad;
      ib = ib * deltad;
      
      const rad = get_rad(vx, vy);
      const velocity = Math.sqrt(vx*vx+vy*vy);
      const width_rad = Math.atan(width_func_d(t)/velocity);
      a1.push([x - ia, y - ib]);
      a2.push([x + ia, y + ib]);
      tang1.push(rad_to_vector(rad-width_rad));
      tang2.push(rad_to_vector(rad+width_rad-Math.PI));
    }
    //const bez1 = fitCubic_tang(a1, tang1, 0.03);
    //const bez2 = fitCubic_tang(a2.reverse(), tang2.reverse(), 0.03);
    const bez1 = fitCurve(a1, 0.03);
    const bez2 = fitCurve(a2.reverse(), 0.03);
    return [bez1, bez2];
  }

  static qBezier(x1, y1, sx, sy, x2, y2, width_func, width_func_d){
    const x_fun = t => ((1.0 - t) * (1.0 - t) * x1 + 2.0 * t * (1.0 - t) * sx + t * t * x2);
    const y_fun = t => ((1.0 - t) * (1.0 - t) * y1 + 2.0 * t * (1.0 - t) * sy + t * t * y2);
    const dx_fun = t => (x1 - 2.0 * sx + x2) * 2.0 * t + (-2.0 * x1 + 2.0 * sx);
    const dy_fun = t => (y1 - 2.0 * sy + y2) * 2.0 * t + (-2.0 * y1 + 2.0 * sy);
    return this.generalBezier(x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d);
  }
  static qBezier2(x1, y1, sx, sy, x2, y2, width_func, width_func_d){
    //similar to qBezier(), but the direction changes at a constant speed (not decided by normal vector)
    const x_fun = t => ((1.0 - t) * (1.0 - t) * x1 + 2.0 * t * (1.0 - t) * sx + t * t * x2);
    const y_fun = t => ((1.0 - t) * (1.0 - t) * y1 + 2.0 * t * (1.0 - t) * sy + t * t * y2);
    const dx_fun = t => (x1 - 2.0 * sx + x2) * 2.0 * t + (-2.0 * x1 + 2.0 * sx);
    const dy_fun = t => (y1 - 2.0 * sy + y2) * 2.0 * t + (-2.0 * y1 + 2.0 * sy);
    const cent_x = (x1 + 4*sx + x2) / 6;
    const cent_y = (y1 + 4*sy + y2) / 6;
    
    const rad_begin = Math.atan2(cent_y-y1, cent_x-x1);
    var rad_end   =   Math.atan2(y2-cent_y, x2-cent_x);
    if(rad_end - rad_begin > Math.PI) {
      rad_end -= Math.PI*2;
    }else if(rad_begin - rad_end > Math.PI){
      rad_end += Math.PI*2;
    }
    const dir_func = t => rad_to_vector((1-t)*rad_begin+t*rad_end+Math.PI/2);
    return this.generalBezier2(x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d, dir_func);
  }
  
  static cBezier(x1, y1, sx1, sy1, sx2, sy2, x2, y2, width_func, width_func_d){
    const x_fun = t => (1.0 - t) * (1.0 - t) * (1.0 - t) * x1 + 3.0 * t * (1.0 - t) * (1.0 - t) * sx1 + 3 * t * t * (1.0 - t) * sx2 + t * t * t * x2;
    const y_fun = t => (1.0 - t) * (1.0 - t) * (1.0 - t) * y1 + 3.0 * t * (1.0 - t) * (1.0 - t) * sy1 + 3 * t * t * (1.0 - t) * sy2 + t * t * t * y2;
    const dx_fun = t => t * t * (-3 * x1 + 9 * sx1 + -9 * sx2 + 3 * x2) + t * (6 * x1 + -12 * sx1 + 6 * sx2) + -3 * x1 + 3 * sx1;
    const dy_fun = t => t * t * (-3 * y1 + 9 * sy1 + -9 * sy2 + 3 * y2) + t * (6 * y1 + -12 * sy1 + 6 * sy2) + -3 * y1 + 3 * sy1;
    return this.generalBezier(x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d);
  }
  
  static slantBezier(x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d, dir_x, dir_y){
    var a1 = [];
    var a2 = [];
    var tang1 = [];
    var tang2 = [];
    let [ia, ib] = unit_normal_vector(dir_x, dir_y);
    let len = Math.sqrt(dir_x*dir_x+dir_y*dir_y);
    let ex = dir_x/len;
    let ey = dir_y/len;
    
    for (var tt = 0; tt <= bezier_steps; tt++) {
      const t = tt / bezier_steps;
      const x = x_fun(t);
      const y = y_fun(t);
      const vx = dx_fun(t);
      const vy = dy_fun(t);

      const deltad = width_func(t);
      
      const velocity = (dir_x*vx+dir_y*vy)/Math.sqrt(dir_x*dir_x+dir_y*dir_y);
      const width_tan = width_func_d(t)/velocity;
      const bez_tan = (dir_x*vy-dir_y*vx)/(dir_x*vx+dir_y*vy);
      const rad1 = Math.atan(bez_tan-width_tan);
      const rad2 = Math.atan(bez_tan+width_tan);
      a1.push([x - ia * deltad, y - ib * deltad]);
      a2.push([x + ia * deltad, y + ib * deltad]);
      tang1.push([ Math.cos(rad1)*ex-Math.sin(rad1)*ey,  Math.sin(rad1)*ex+Math.cos(rad1)*ey]);
      tang2.push([-Math.cos(rad2)*ex+Math.sin(rad2)*ey, -Math.sin(rad2)*ex-Math.cos(rad2)*ey]);
    }
    //const bez1 = fitCubic_tang(a1, tang1, 0.03);
    //const bez2 = fitCubic_tang(a2.reverse(), tang2.reverse(), 0.03);
    const bez1 = fitCurve(a1, 0.01);
    const bez2 = fitCurve(a2.reverse(), 0.01);
    return [bez1, bez2];
  }

  static qBezier_slant(x1, y1, sx, sy, x2, y2, width_func, width_func_d){
    const x_fun = t => ((1.0 - t) * (1.0 - t) * x1 + 2.0 * t * (1.0 - t) * sx + t * t * x2);
    const y_fun = t => ((1.0 - t) * (1.0 - t) * y1 + 2.0 * t * (1.0 - t) * sy + t * t * y2);
    const dir_x = x2 - x1;
    const dir_y = y2 - y1;
    const dx_fun = t => (x1 - 2.0 * sx + x2) * 2.0 * t + (-2.0 * x1 + 2.0 * sx);
    const dy_fun = t => (y1 - 2.0 * sy + y2) * 2.0 * t + (-2.0 * y1 + 2.0 * sy);
    return this.slantBezier(x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d, dir_x, dir_y);
  }
  static cBezier_slant(x1, y1, sx1, sy1, sx2, sy2, x2, y2, width_func, width_func_d){
    const x_fun = t => (1.0 - t) * (1.0 - t) * (1.0 - t) * x1 + 3.0 * t * (1.0 - t) * (1.0 - t) * sx1 + 3 * t * t * (1.0 - t) * sx2 + t * t * t * x2;
    const y_fun = t => (1.0 - t) * (1.0 - t) * (1.0 - t) * y1 + 3.0 * t * (1.0 - t) * (1.0 - t) * sy1 + 3 * t * t * (1.0 - t) * sy2 + t * t * t * y2;
    const dx_fun = t => t * t * (-3 * x1 + 9 * sx1 + -9 * sx2 + 3 * x2) + t * (6 * x1 + -12 * sx1 + 6 * sx2) + -3 * x1 + 3 * sx1;
    const dy_fun = t => t * t * (-3 * y1 + 9 * sy1 + -9 * sy2 + 3 * y2) + t * (6 * y1 + -12 * sy1 + 6 * sy2) + -3 * y1 + 3 * sy1;
    const dir_x = x2 - x1;
    const dir_y = y2 - y1;
    return this.slantBezier(x_fun, y_fun, dx_fun, dy_fun, width_func, width_func_d, dir_x, dir_y);
  }
  
 static bez_to_poly(bez){
    var poly = new Polygon();
    poly.push(bez[0][0][0], bez[0][0][1]);
    for(let bez1 of bez){
      poly.push(bez1[1][0], bez1[1][1], 2);
      poly.push(bez1[2][0], bez1[2][1], 2);
      poly.push(bez1[3][0], bez1[3][1]);
    }
    return poly;
  }
}