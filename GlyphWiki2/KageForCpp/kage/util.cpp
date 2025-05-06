#include "util.h"

constexpr double SQRT_2 = 1.41421356'2373095048'801688724209;

constexpr kage::Float bez_cir = 4*(SQRT_2-1)/3;

//a constant for drawing circles with Bezier curves
constexpr kage::Float CURVE_THIN = 0.1910;

constexpr kage::Float HALF = 0.5;
constexpr kage::Float ONE = 1;


kage::Vec kage::Vec::toLen(Float wanted) const noexcept
{
    auto q = wanted / len();
    return { x * q, y * q };
}


kage::Dir kage::Vec::dir() const noexcept
{
    auto q = std::hypot(x, y);
    if (q <= 0)
        return { .cos = 1, .sin = 0 };
    q = ONE / q;
    return { .cos = x/q, .sin = y/q };
}


auto kage::Vec::unitNormal() const noexcept -> Vec
{
#define ix (this->x)
#define iy (this->y)
    Float ia, ib;
  // line SUICHOKU by vector
    if (ix != 0 && iy != 0) {
        Float rr = ONE / std::hypot(ix, iy);
        // Angle used to be atan(-y/x), cos/sin switched →
        // sin(x,y) = -sin(x, -y)
        // cos      =  cos
        ia = -iy * rr;
        ib =  ix * rr;
    }
    else if (ix == 0) {
        if (iy < 0) {
          ia = -1;
        } else {
          ia = 1;
        }
        ib = 0;
    } else {
        ia = 0;
        ib = 1;
    }
    //reverse if vector is going 2nd/3rd quadrants
    if (ix <= 0) {
        ia = -ia;
        ib = -ib;
    }
    return { ia, ib };
#undef ix
#undef iy
}


kage::Float kage::widfun(Float t, Point<Float> a, Point<Float> b, Float wid)
{
    const auto len = a.distFrom(b);
    const auto p = Float(1) + std::sqrt(Float(100)/len);
    return (  (std::sqrt(p*p+(p-1)*(p-1)-(p-t)*(p-t))-(p-1))*(1-CURVE_THIN)+CURVE_THIN  )*wid;
}

kage::Float kage::widfun_d(Float t, Point<Float> a, Point<Float> b, Float wid)
{
    const auto len = a.distFrom(b);
    const auto p = Float(1) + std::sqrt(Float(100)/len);
    return wid*(1-CURVE_THIN) // *0.5*2 here
            * (p-t) / std::sqrt(p*p+(p-1)*(p-1)-(p-t)*(p-t));
}

kage::Float kage::widfun_stop(Float t, Point<Float> a, Point<Float> b, Float wid)
{
    const auto len = a.distFrom(b);
    const auto p = Float(1) + std::sqrt(Float(100)/len);
    return (  (std::sqrt(p*p+(p-1)*(p-1)-(p-t)*(p-t))-(p-1))*(Float(1.10)-CURVE_THIN)+CURVE_THIN  )*wid;
}

kage::Float kage::widfun_stop_d(Float t, Point<Float> a, Point<Float> b, Float wid)
{
    const auto len = a.distFrom(b);
    const auto p = Float(1) + std::sqrt(Float(100)/len);
    return wid*(Float(1.10)-CURVE_THIN) // *0.5*2
            *(p-t) / std::sqrt(p*p+(p-1)*(p-1)-(p-t)*(p-t));
}

kage::Float kage::widfun_stop2(Float t, Point<Float>, Point<Float>, Float wid)
{
    return (  (1-std::pow(1-t, Float(1.21)))*(Float(1.10)-CURVE_THIN)+CURVE_THIN  )*wid;
}

kage::Float kage::widfun_stop2_d(Float t, Point<Float>, Point<Float>, Float wid)
{
    return wid*(Float(1.10)-CURVE_THIN)*Float(1.21) * std::pow(1-t, Float(0.21));
}

//fat version (used in cubic bezier)
kage::Float kage::widfun_fat(Float t, Point<Float> a, Point<Float> b, Float wid)
{
    const auto len = a.distFrom(b);
    const auto p = Float(1) + std::sqrt(40/len);
    return (  (std::sqrt(p*p + (p-1)*(p-1) - (p-t)*(p-t)) - (p-1)  )*(1-CURVE_THIN)+CURVE_THIN  )*wid;
}

kage::Float kage::widfun_fat_d(Float t, Point<Float> a, Point<Float> b, Float wid)
{
    const auto len = a.distFrom(b);
    const auto p = Float(1) + std::sqrt(40/len);
    return wid*(1-CURVE_THIN) //*0.5*2
          *(p-t) / std::sqrt(p*p + (p-1)*(p-1) - (p-t)*(p-t));
}

auto kage::movedPoint(Point<Float> p, Dir dir, Float delta) -> Point<Float>
{
  return { p.x + delta*dir.cos, p.y + delta*dir.sin };
}

auto kage::getExtendedDest(Point<Float> dest, Point<Float> src, Float delta) -> Point<Float>
{
    const auto dir = (dest - src).dir();
    return movedPoint(dest, dir, delta);
}

auto kage::getExtendedDestWrong(Point<Float> dest, Point<Float> src, Float delta) -> Point<Float>
{
  //The process for lines directed exactly in the negative x-direction or y-direction is not correct, so it's named as "wrong".
    auto destNew = dest;
    if (src.x == dest.x) {
        destNew.y += delta;
    } else if (src.y == dest.y) {
        destNew.x += delta;
    } else {
        const auto dx = dest.x - src.x;
        const auto dy = dest.y - src.y;
        Float v = (src.x < dest.x) ? 1 : -1;
        const auto vrlen = v / std::hypot(dx, dy);
        destNew.x += dx * vrlen;
        destNew.y += dy * vrlen;
    }
    return destNew;
}

kage::Float kage::calcHosomi(Point<Float> x, Point<Float> y)
{
    Float hosomi = 0.5;
    if (auto d = x.distFrom(y); d < 50) {
        hosomi += 0.4 * (ONE - d / 50);
    }
    return hosomi;
}


void kage::Box::intersectWith(Point<int> p)
{
    minX = std::min(minX, p.x);
    maxX = std::max(maxX, p.x);
    minY = std::min(minY, p.y);
    maxY = std::max(maxY, p.y);
}


kage::Box kage::getBoundingBox(std::span<const Stroke> strokes)
{
    Box a { .minX = 200, .minY = 200, .maxX = 0, .maxY = 0 };
    for (auto& v : strokes) {
        if (v.type == 0)
            continue;
        a.intersectWith(v.p3_4());
        a.intersectWith(v.p5_6());
        if (v.type == 1 || v.type == 99)
            continue;
        a.intersectWith(v.p7_8());
        if (v.type == 2 || v.type == 3 || v.type == 4)
            continue;
        a.intersectWith(v.p9_10());
    }
    return a;
}

/*
kage::Vec kage::Bez::toLine(Point<Float> p0, Float rad) const
{
    const auto crad = std::cos(rad);
    const auto srad = std::sin(rad);
    const auto msrad = -srad;

// Rotation matrix, inverted
#define minv0  crad   // Math.cos(-rad)
#define minv1  srad   // -Math.sin(-rad)
#define minv2 msrad   // Math.sin(-rad)
#define minv3  crad   // Math.sin(-rad)

    auto gentenRorate = [&](Point<Float>& p) {
        auto q = p - p0;
        p = { minv0*q.x + minv1*q.y, minv2*q.x + minv3 * q.y };
    };

// Rotation matrix, inverted
#define m0  crad    // Math.cos(rad)
#define m1 msrad    // -Math.sin(rad)
#define m2  srad    // Math.sin(rad)
#define m3  crad    // Math.cos(rad

    auto gentenRotateInv = [&](Point<Float>& p) {
        p = { minv0 * p.x + minv1 * p.y, minv2 * p.x + minv3 * p.y };
    };

    auto bezierRotated = *this;
    bezierRotated.doTransform(gentenRorate);
    var bezier_genten_rotated = bez.map(genten_rotate);
    var bez_edited = bezier_to_y(bezier_genten_rotated, 0)
    var bez_edited_return = bez_edited.map(genten_rotate_inv)
    bez_edited_return[0] = bez[0] //始点は変わらないはずなので誤差を防ぐため元の値を代入
    return bez_edited_return;
}

kage::Vec bezierToLine(bez, x0, y0, rad){
  var rotate_mat_inv = [Math.cos(-rad), -Math.sin(-rad), Math.sin(-rad), Math.cos(-rad)]
  function genten_rotate(p){
    let [x,y] = p
    var x1 = x-x0
    var y1 = y-y0
    return [rotate_mat_inv[0]*x1+rotate_mat_inv[1]*y1, rotate_mat_inv[2]*x1+rotate_mat_inv[3]*y1]
  }

  var rotate_mat = [Math.cos(rad), -Math.sin(rad), Math.sin(rad), Math.cos(rad)]
  function genten_rotate_inv(p){
    let [x,y] = p
    return [rotate_mat[0]*x+rotate_mat[1]*y + x0, rotate_mat[2]*x+rotate_mat[3]*y + y0]
  }

  var bezier_genten_rotated = bez.map(genten_rotate);
  var bez_edited = bezier_to_y(bezier_genten_rotated, 0)
  var bez_edited_return = bez_edited.map(genten_rotate_inv)
  bez_edited_return[0] = bez[0] //始点は変わらないはずなので誤差を防ぐため元の値を代入
  return bez_edited_return;
}
function stretch_bezier_end(bez, t){
  const start = [bez[0][0],
                 bez[0][1]];
  const c1 = [(1-t) * bez[0][0]+t * bez[1][0],
              (1-t) * bez[0][1]+t * bez[1][1]];
  const c2 = [(1-t) * (1-t) * bez[0][0] + 2.0 * t * (1-t) * bez[1][0] + t * t * bez[2][0],
              (1-t) * (1-t) * bez[0][1] + 2.0 * t * (1-t) * bez[1][1] + t * t * bez[2][1]]
  const end = [(1-t) * (1-t) * (1-t) * bez[0][0] + 3 * t * (1-t) * (1-t) * bez[1][0] + 3 * t * t * (1-t) * bez[2][0] + t * t * t * bez[3][0],
               (1-t) * (1-t) * (1-t) * bez[0][1] + 3 * t * (1-t) * (1-t) * bez[1][1] + 3 * t * t * (1-t) * bez[2][1] + t * t * t * bez[3][1],]
  return [start, c1, c2, end];
}
export function bezier_to_y(bez, y){
  const a =     bez[3][1] - 3 * bez[2][1] + 3 * bez[1][1] - bez[0][1];
  const b = 3 * bez[2][1] - 6 * bez[1][1] + 3 * bez[0][1];
  const c = 3 * bez[1][1] - 3 * bez[0][1];
  const d =     bez[0][1];
  const yy = solveCubic(a, b, c, d - y);
  const ext = get_extreme_points(a, b, c);
  function hit_extreme(x1, x2){
    for (let e of ext){
      if (x1 < e && e < x2) return true;
    }
    return false;
  }
  yy.sort(function (a, b) {//descending order
    return b - a;
  });
  for (let i of yy) {
    if (0 < i && i < 1) {
      return stretch_bezier_end(bez, i);
    }
  }

  yy.reverse()//ascending order
  for (let i of yy) {
    if (i > 1) {
      if(!hit_extreme(1, i)) return stretch_bezier_end(bez, i);
    }
  }
  return bez;

  //var res = shorten_bezier_to_y(bez, y);
  //if(res){return res;}else{
  //  res = extend_bezier_to_y(bez, y);
  //  if(res){return res;}else{
  //    return bez
  //  }
  //}
}
function extend_bezier_to_y(bez, y) {
  const a =     bez[3][1] - 3 * bez[2][1] + 3 * bez[1][1] - bez[0][1];
  const b = 3 * bez[2][1] - 6 * bez[1][1] + 3 * bez[0][1];
  const c = 3 * bez[1][1] - 3 * bez[0][1];
  const d =     bez[0][1];
  const yy = solveCubic(a, b, c, d - y);
  yy.sort(function (a, b) {//ascending order
    return a - b;
  });
  for (let i of yy) {
    if (i > 1) {
      return stretch_bezier_end(bez, i);
    }
  }
  return false;
}

function shorten_bezier_to_y(bez, y) {
  const a =     bez[3][1] - 3 * bez[2][1] + 3 * bez[1][1] - bez[0][1];
  const b = 3 * bez[2][1] - 6 * bez[1][1] + 3 * bez[0][1];
  const c = 3 * bez[1][1] - 3 * bez[0][1];
  const d =     bez[0][1];
  const yy = solveCubic(a, b, c, d - y);
  yy.sort(function (a, b) {//descending order
    return b - a;
  });
  for (let i of yy) {
    if (0 < i && i < 1) {
      return stretch_bezier_end(bez, i);
    }
  }
  return false;
}
function get_extreme_points(a0, b0, c0){
 let a = a0*3;
 let b = b0*2;
 let c = c0;
 let d = b * b - (4 * a * c);
 if(d > 0){
    let x1 = ((-1) * b + Math.sqrt(d)) / (2 * a);
    let x2 = ((-1) * b - Math.sqrt(d)) / (2 * a);
    return [x1, x2]
 } else if(d == 0) {
    return [(-1) * b / (2 * a)]
 } else {
    return []
 }
}
function solveCubic(a, b, c, d) {
  if (Math.abs(a) < 1e-8) { // Quadratic case, ax^2+bx+c=0
      a = b; b = c; c = d;
      if (Math.abs(a) < 1e-8) { // Linear case, ax+b=0
          a = b; b = c;
          if (Math.abs(a) < 1e-8) // Degenerate case
              return [];
          return [-b/a];
      }

      var D = b*b - 4*a*c;
      if (Math.abs(D) < 1e-8)
          return [-b/(2*a)];
      else if (D > 0)
          return [(-b+Math.sqrt(D))/(2*a), (-b-Math.sqrt(D))/(2*a)];
      return [];
  }

  // Convert to depressed cubic t^3+pt+q = 0 (subst x = t - b/3a)
  var p = (3*a*c - b*b)/(3*a*a);
  var q = (2*b*b*b - 9*a*b*c + 27*a*a*d)/(27*a*a*a);
  var roots;

  if (Math.abs(p) < 1e-8) { // p = 0 -> t^3 = -q -> t = -q^1/3
      roots = [cuberoot(-q)];
  } else if (Math.abs(q) < 1e-8) { // q = 0 -> t^3 + pt = 0 -> t(t^2+p)=0
      roots = [0].concat(p < 0 ? [Math.sqrt(-p), -Math.sqrt(-p)] : []);
  } else {
      var D = q*q/4 + p*p*p/27;
      if (Math.abs(D) < 1e-8) {       // D = 0 -> two roots
          roots = [-1.5*q/p, 3*q/p];
      } else if (D > 0) {             // Only one real root
          var u = cuberoot(-q/2 - Math.sqrt(D));
          roots = [u - p/(3*u)];
      } else {                        // D < 0, three roots, but needs to use complex numbers/trigonometric solution
          var u = 2*Math.sqrt(-p/3);
          var t = Math.acos(3*q/p/u)/3;  // D < 0 implies p < 0 and acos argument in [-1..1]
          var k = 2*Math.PI/3;
          roots = [u*Math.cos(t), u*Math.cos(t-k), u*Math.cos(t-2*k)];
      }
  }

  // Convert back from depressed cubic
  for (var i = 0; i < roots.length; i++)
      roots[i] -= b/(3*a);

  return roots;
}
function cuberoot(x) {
  var y = Math.pow(Math.abs(x), 1/3);
  return x < 0 ? -y : y;
}

export function stretch(dp, sp, p, min, max) { // integer
  var p1, p2, p3, p4;
  if (p < sp + 100) {
    p1 = min;
    p3 = min;
    p2 = sp + 100;
    p4 = dp + 100;
  } else {
    p1 = sp + 100;
    p3 = dp + 100;
    p2 = max;
    p4 = max;
  }
  return Math.floor(((p - p1) / (p2 - p1)) * (p4 - p3) + p3);
}

export function getBoundingBox(strokes) { // minX, minY, maxX, maxY
  var a = new Object();
  a.minX = 200;
  a.minY = 200;
  a.maxX = 0;
  a.maxY = 0;
  for (var i = 0; i < strokes.length; i++) {
    if (strokes[i][0] == 0) { continue; }
    a.minX = Math.min(a.minX, strokes[i][3]);
    a.maxX = Math.max(a.maxX, strokes[i][3]);
    a.minY = Math.min(a.minY, strokes[i][4]);
    a.maxY = Math.max(a.maxY, strokes[i][4]);
    a.minX = Math.min(a.minX, strokes[i][5]);
    a.maxX = Math.max(a.maxX, strokes[i][5]);
    a.minY = Math.min(a.minY, strokes[i][6]);
    a.maxY = Math.max(a.maxY, strokes[i][6]);
    if (strokes[i][0] == 1) { continue; }
    if (strokes[i][0] == 99) { continue; }
    a.minX = Math.min(a.minX, strokes[i][7]);
    a.maxX = Math.max(a.maxX, strokes[i][7]);
    a.minY = Math.min(a.minY, strokes[i][8]);
    a.maxY = Math.max(a.maxY, strokes[i][8]);
    if (strokes[i][0] == 2) { continue; }
    if (strokes[i][0] == 3) { continue; }
    if (strokes[i][0] == 4) { continue; }
    a.minX = Math.min(a.minX, strokes[i][9]);
    a.maxX = Math.max(a.maxX, strokes[i][9]);
    a.minY = Math.min(a.minY, strokes[i][10]);
    a.maxY = Math.max(a.maxY, strokes[i][10]);
  }
  return a;
}
*/
