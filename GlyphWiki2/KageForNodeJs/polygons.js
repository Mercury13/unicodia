export class Polygons{
  // method
  constructor(){
    this.array = new Array();
  }
 	clear(){ // void
    this.array = new Array();
  }
  push(polygon){ // void
    // only a simple check
    var minx = 200;
    var maxx = 0;
    var miny = 200;
    var maxy = 0;
    var error = 0;
    for(var i = 0; i < polygon.array.length; i++){
      if(polygon.array[i].x < minx){
        minx = polygon.array[i].x;
      }
      if(polygon.array[i].x > maxx){
        maxx = polygon.array[i].x;
      }
      if(polygon.array[i].y < miny){
        miny = polygon.array[i].y;
      }
      if(polygon.array[i].y > maxy){
        maxy = polygon.array[i].y;
      }
      if(isNaN(polygon.array[i].x) || isNaN(polygon.array[i].y)){
        error++;
      }
    }
    if(error == 0 && minx != maxx && miny != maxy && polygon.array.length >= 3){
      var newArray = new Array();
      newArray.push(polygon.array.shift());
      while(polygon.array.length != 0){
        var temp = polygon.array.shift();
        //if(newArray[newArray.length - 1].x != temp.x ||
        //   newArray[newArray.length - 1].y != temp.y){
          newArray.push(temp);
        //}
      }
      if(newArray.length >= 3){
        polygon.array = newArray;
        this.array.push(polygon);
      }
    }
  }
  concat(polygons){
    for(let polygon of polygons.array){
      this.push(polygon);
    }
  }
  generateSVG(){ // string
    var buffer = "";
    buffer += "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.1\" baseProfile=\"full\" viewBox=\"0 0 200 200\" width=\"200\" height=\"200\">\n";
    for(var i = 0; i < this.array.length; i++){
      buffer += "<path d=\"";
      buffer += this.array[i].get_sub_path_svg();
      buffer += "\" fill=\"black\" />\n";
    }
    buffer += "</svg>\n";
    return buffer;
  }
  generateSVG2(){ // string
    var buffer = "";
    buffer += "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.1\" baseProfile=\"full\" viewBox=\"0 0 200 200\" width=\"200\" height=\"200\">\n";
    buffer += "<path d=\"";
    buffer += this.get_path_svg();
    buffer += "\" fill=\"black\" />\n";
    buffer += "</svg>\n";
    return buffer;
  }
  get_path_svg(){
    var buffer = "";
    for(var i = 0; i < this.array.length; i++){
      buffer += this.array[i].get_sub_path_svg();
    }
    return buffer;
  }
  get_path_svg_font(){
    var buffer = "";
    for(var i = 0; i < this.array.length; i++){
      buffer += this.array[i].get_sub_path_svg_font();
    }
    return buffer;
  }
  generateEPS(){ // string
    var buffer = "";
    buffer += "%!PS-Adobe-3.0 EPSF-3.0\n";
    buffer += "%%BoundingBox: 0 -208 1024 816\n";
    buffer += "%%Pages: 0\n";
    buffer += "%%Title: Kanji glyph\n";
    buffer += "%%Creator: GlyphWiki powered by KAGE system\n";
    buffer += "%%CreationDate: " + new Date() + "\n";
    buffer += "%%EndComments\n";
    buffer += "%%EndProlog\n";
    
    for(var i = 0; i < this.array.length; i++){
      for(var j = 0; j < this.array[i].array.length; j++){
        buffer += (this.array[i].array[j].x * 5) + " " + (1000 - this.array[i].array[j].y * 5 - 200) + " ";
        if(j == 0){
          buffer += "newpath\nmoveto\n";
        } else {
          buffer += "lineto\n";
        }
      }
      buffer += "closepath\nfill\n";
    }
    buffer += "%%EOF\n";
    return buffer;
  }

}
