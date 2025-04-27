function divide_curve(kage, x1, y1, sx1, sy1, x2, y2, curve, div_curve, off_curve){
  var rate = 0.5;
  var cut = Math.floor(curve.length * rate);
  var cut_rate = cut / curve.length;
  var tx1 = x1 + (sx1 - x1) * cut_rate;
  var ty1 = y1 + (sy1 - y1) * cut_rate;
  var tx2 = sx1 + (x2 - sx1) * cut_rate;
  var ty2 = sy1 + (y2 - sy1) * cut_rate;
  var tx3 = tx1 + (tx2 - tx1) * cut_rate;
  var ty3 = ty1 + (ty2 - ty1) * cut_rate;
  
  div_curve[0] = new Array();
  div_curve[1] = new Array();
  off_curve[0] = new Array(6);
  off_curve[1] = new Array(6);
  
  // must think about 0 : <0
  var i;
  for(i = 0; i <= cut; i++){
    div_curve[0].push(curve[i]);
  }
  off_curve[0][0] = x1;
  off_curve[0][1] = y1;
  off_curve[0][2] = tx1;
  off_curve[0][3] = ty1;
  off_curve[0][4] = tx3;
  off_curve[0][5] = ty3;
  
  for(i = cut; i < curve.length; i++){
    div_curve[1].push(curve[i]);
  }
  off_curve[1][0] = tx3;
  off_curve[1][1] = ty3;
  off_curve[1][2] = tx2;
  off_curve[1][3] = ty2;
  off_curve[1][4] = x2;
  off_curve[1][5] = y2;
}

// ------------------------------------------------------------------
function find_offcurve(kage, curve, sx, sy, result){
  var nx1, ny1, nx2, ny2, tx, ty;
  var minx, miny, count, diff;
  var tt, t, x, y, ix, iy;
  var mindiff = 100000;
  var area = 8;
  var mesh = 2;
  // area = 10   mesh = 5 -> 281 calcs
  // area = 10   mesh = 4 -> 180 calcs
  // area =  8   mesh = 4 -> 169 calcs
  // area =  7.5 mesh = 3 -> 100 calcs
  // area =  8   mesh = 2 ->  97 calcs
  // area =  7   mesh = 2 ->  80 calcs
  
  nx1 = curve[0][0];
  ny1 = curve[0][1];
  nx2 = curve[curve.length - 1][0];
  ny2 = curve[curve.length - 1][1];
  
  for(tx = sx - area; tx < sx + area; tx += mesh){
    for(ty = sy - area; ty < sy + area; ty += mesh){
      count = 0;
      diff = 0;
      for(tt = 0; tt < curve.length; tt++){
        t = tt / curve.length;
        
        //calculate a dot
        x = ((1.0 - t) * (1.0 - t) * nx1 + 2.0 * t * (1.0 - t) * tx + t * t * nx2);
        y = ((1.0 - t) * (1.0 - t) * ny1 + 2.0 * t * (1.0 - t) * ty + t * t * ny2);
        
        //KATAMUKI of vector by BIBUN
        ix = (nx1 - 2.0 * tx + nx2) * 2.0 * t + (-2.0 * nx1 + 2.0 * tx);
        iy = (ny1 - 2.0 * ty + ny2) * 2.0 * t + (-2.0 * ny1 + 2.0 * ty);
        
        diff += (curve[count][0] - x) * (curve[count][0] - x) + (curve[count][1] - y) * (curve[count][1] - y);
        if(diff > mindiff){
          tt = curve.length;
        }
        count++;
      }
      if(diff < mindiff){
        minx = tx;
        miny = ty;
        mindiff = diff;
      }
    }
  }
  
  for(tx = minx - mesh + 1; tx <= minx + mesh - 1; tx += 0.5){
    for(ty = miny - mesh + 1; ty <= miny + mesh - 1; ty += 0.5){
      count = 0;
      diff = 0;
      for(tt = 0; tt < curve.length; tt++){
        t = tt / curve.length;
        
        //calculate a dot
        x = ((1.0 - t) * (1.0 - t) * nx1 + 2.0 * t * (1.0 - t) * tx + t * t * nx2);
        y = ((1.0 - t) * (1.0 - t) * ny1 + 2.0 * t * (1.0 - t) * ty + t * t * ny2);
        
        //KATAMUKI of vector by BIBUN
        ix = (nx1 - 2.0 * tx + nx2) * 2.0 * t + (-2.0 * nx1 + 2.0 * tx);
        iy = (ny1 - 2.0 * ty + ny2) * 2.0 * t + (-2.0 * ny1 + 2.0 * ty);
        
        diff += (curve[count][0] - x) * (curve[count][0] - x) + (curve[count][1] - y) * (curve[count][1] - y);
        if(diff > mindiff){
          tt = curve.length;
        }
        count++;
      }
      if(diff < mindiff){
        minx = tx;
        miny = ty;
        mindiff = diff;
      }
    }
  }
  
  result[0] = nx1;
  result[1] = ny1;
  result[2] = minx;
  result[3] = miny;
  result[4] = nx2;
  result[5] = ny2;
  result[6] = mindiff;
}

// ------------------------------------------------------------------
function get_candidate(kage, curve, a1, a2, x1, y1, sx1, sy1, x2, y2, opt3, opt4){
  var x, y, ix, iy, ir, ia, ib, tt, t, deltad;
  var hosomi = 0.5;
  
  curve[0] = new Array();
  curve[1] = new Array();
  
  for(tt = 0; tt <= 1000; tt = tt + kage.kRate){
    t = tt / 1000;
    
    //calculate a dot
    x = ((1.0 - t) * (1.0 - t) * x1 + 2.0 * t * (1.0 - t) * sx1 + t * t * x2);
    y = ((1.0 - t) * (1.0 - t) * y1 + 2.0 * t * (1.0 - t) * sy1 + t * t * y2);
    
    //KATAMUKI of vector by BIBUN
    ix = (x1 - 2.0 * sx1 + x2) * 2.0 * t + (-2.0 * x1 + 2.0 * sx1);
    iy = (y1 - 2.0 * sy1 + y2) * 2.0 * t + (-2.0 * y1 + 2.0 * sy1);
    //line SUICHOKU by vector
    if(ix != 0 && iy != 0){
      ir = Math.atan(iy / ix * -1);
      ia = Math.sin(ir) * (kage.kMinWidthT);
      ib = Math.cos(ir) * (kage.kMinWidthT);
    }
    else if(ix == 0){
      ia = kage.kMinWidthT;
      ib = 0;
    }
    else{
      ia = 0;
      ib = kage.kMinWidthT;
    }
    
    if(a1 == 7 && a2 == 0){ // L2RD: fatten
      deltad = Math.pow(t, hosomi) * kage.kL2RDfatten;
    }
    else if(a1 == 7){
      deltad = Math.pow(t, hosomi);
    }
    else if(a2 == 7){
      deltad = Math.pow(1.0 - t, hosomi);
    }
    else if(opt3 > 0){
      deltad = (((kage.kMinWidthT - opt4 / 2) - opt3 / 2) / (kage.kMinWidthT - opt4 / 2)) + opt3 / 2 / (kage.kMinWidthT - opt4) * t;
    }
    else{ deltad = 1; }
    
    if(deltad < 0.15){
      deltad = 0.15;
    }
    ia = ia * deltad;
    ib = ib * deltad;
    
    //reverse if vector is going 2nd/3rd quadrants
    if(ix <= 0){
      ia = ia * -1;
      ib = ib * -1;
    }
    
    temp = new Array(2);
    temp[0] = x - ia;
    temp[1] = y - ib;
    curve[0].push(temp);
    temp = new Array(2);
    temp[0] = x + ia;
    temp[1] = y + ib;
    curve[1].push(temp);
  }
}
