import {DIR_POSX} from "./util.js";
export class PointMaker{
  constructor(x, y, dir, scale){
    this.x = x;
    this.y = y;
    this.dir = dir;
    this.scale = scale;
    if(!scale){
      this.scale = 1;
    }
    if(!dir){
    this.dir = DIR_POSX;//positive x
    }
  }
  setpos(x, y){
    this.x = x;
    this.y = y;
  }
  vec(x, y){ // void
    return [this.x + this.scale*this.dir.cos*x - this.scale*this.dir.sin*y,
            this.y + this.scale*this.dir.sin*x + this.scale*this.dir.cos*y]
  }
  setdir(dir){
    this.dir = dir;
  }
  setscale(scale){
    this.scale = scale;
  }
}
