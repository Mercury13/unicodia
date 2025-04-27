// Reference : http://www.cam.hi-ho.ne.jp/strong_warriors/teacher/chapter0{4,5}.html

function point(x, y){
  this.x = x;
  this.y = y;
}

function getCrossPoint(x11, y11, x12, y12, x21, y21, x22, y22){ // point
  var a1 = y12 - y11;
  var b1 = x11 - x12;
  var c1 = -1 * a1 * x11 - b1 * y11;
  var a2 = y22 - y21;
  var b2 = x21 - x22;
  var c2 = -1 * a2 * x21 - b2 * y21;
  
  var temp = b1 * a2 - b2 * a1;
  if(temp == 0){ // parallel
    return false;
  }
  return new point((c1 * b2 - c2 * b1) / temp, (a1 * c2 - a2 * c1) / temp);
}

function isCross(x11, y11, x12, y12, x21, y21, x22, y22){ // boolean
  var temp = getCrossPoint(x11, y11, x12, y12, x21, y21, x22, y22);
  if(!temp){ return false; }
  if(x11 < x12 && (temp.x < x11 || x12 < temp.x) ||
     x11 > x12 && (temp.x < x12 || x11 < temp.x) ||
     y11 < y12 && (temp.y < y11 || y12 < temp.y) ||
     y11 > y12 && (temp.y < y12 || y11 < temp.y)
     ){
    return false;
  }
  if(x21 < x22 && (temp.x < x21 || x22 < temp.x) ||
     x21 > x22 && (temp.x < x22 || x21 < temp.x) ||
     y21 < y22 && (temp.y < y21 || y22 < temp.y) ||
     y21 > y22 && (temp.y < y22 || y21 < temp.y)
     ){
    return false;
  }
  return true;
}

function isCrossBox(x1, y1, x2, y2, bx1, by1, bx2, by2){ // boolean
  if(isCross(x1, y1, x2, y2, bx1, by1, bx2, by1)){ return true; }
  if(isCross(x1, y1, x2, y2, bx2, by1, bx2, by2)){ return true; }
  if(isCross(x1, y1, x2, y2, bx1, by2, bx2, by2)){ return true; }
  if(isCross(x1, y1, x2, y2, bx1, by1, bx1, by2)){ return true; }
  return false;
}

export function isCrossBoxWithOthers(strokesArray, i, bx1, by1, bx2, by2){ // boolean
  for(var j = 0; j < strokesArray.length; j++){
    if(i == j){ continue; }
    switch(strokesArray[j][0]){
    case 0:
    case 8:
    case 9:
      break;
    case 6:
    case 7:
      if(isCrossBox(strokesArray[j][7],
                    strokesArray[j][8],
                    strokesArray[j][9],
                    strokesArray[j][10],
                    bx1, by1, bx2, by2)){
        return true;
      }
    case 2:
    case 12:
    case 3:
    case 4:
      if(isCrossBox(strokesArray[j][5],
                    strokesArray[j][6],
                    strokesArray[j][7],
                    strokesArray[j][8],
                    bx1, by1, bx2, by2)){
        return true;
      }
    default:
      if(isCrossBox(strokesArray[j][3],
                    strokesArray[j][4],
                    strokesArray[j][5],
                    strokesArray[j][6],
                    bx1, by1, bx2, by2)){
        return true;
      }
    }
  }
  return false;
}

export function isCrossWithOthers(strokesArray, i, bx1, by1, bx2, by2){ // boolean
  for(var j = 0; j < strokesArray.length; j++){
    if(i == j){ continue; }
    switch(strokesArray[j][0]){
    case 0:
    case 8:
    case 9:
      break;
    case 6:
    case 7:
      if(isCross(strokesArray[j][7],
                 strokesArray[j][8],
                 strokesArray[j][9],
                 strokesArray[j][10],
                 bx1, by1, bx2, by2)){
        return true;
      }
    case 2:
    case 12:
    case 3:
    case 4:
      if(isCross(strokesArray[j][5],
                 strokesArray[j][6],
                 strokesArray[j][7],
                 strokesArray[j][8],
                 bx1, by1, bx2, by2)){
        return true;
      }
    default:
      if(isCross(strokesArray[j][3],
                 strokesArray[j][4],
                 strokesArray[j][5],
                 strokesArray[j][6],
                 bx1, by1, bx2, by2)){
        return true;
      }
    }
  }
  return false;
}
