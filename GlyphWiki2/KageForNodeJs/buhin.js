export class Buhin{
  constructor(number){
    this.hash = {};
  }
  set(name, data){ // void
    this.hash[name] = data;
  }
  push(name, data){ // void
    this.set(name, data)
  }search(name){ // string
    if(this.hash[name]){
      return this.hash[name];
    }
    return ""; // no data
  }
}
