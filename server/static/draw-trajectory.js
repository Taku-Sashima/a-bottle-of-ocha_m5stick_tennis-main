
//データの間隔
const FREQ = 0.01;
let k = 0;

let acc_list=[]
let vel_list=[[0],[0],[0]]
let coor_list=[[0],[0],[0]]


/**
 * 加速度配列を下に速度を計算する関数、速度から変位も
 * 時間領域で台形公式で数値積分
 * @param {any[]} origin_list
 * @param {any[]} calculated_list
 * @returns {any[]}
 */
function trapezoidFormula(origin_list, calculated_list){
  // print("trapezoidFormula")
  for(let i=0; i<calculated_list.length; i++){
    for(let j=1; j < origin_list[i].length; j++){
      calculated_list[i][j] = calculated_list[i][j-1] + 0.5*(origin_list[i][j]+origin_list[i][j])*FREQ;
    }
    // print("calculated_list",i,calculated_list)
  }
  // print("calculated_list",calculated_list)
  return calculated_list;
}

/**
 * fetchDataから加速度を取ってきてリスト化、速度、変異を計算
 * @param {any[]} lsit
 * @returns {any[]}
 */
function getFetchData(lsit){
  // print("getFetcData")
  // print("fetchedData",fetchedData);
  for(let i=0; i<3; i++){
    sub_list=[]
    for(let j=0; j<5; j++){
      for(let k=0; k<20; k++){
        sub_list.push(fetchedData.data_list[j].moments[k].acc[i])
      }
    }
    // print("sub_list", subb_list)
    lsit.push(sub_list)
  }
  // print("list",list)

  return lsit;
}

function setup(){
  console.log("setup!!")
	createCanvas(windowWidth, windowHeight,WEBGL);
  //frameRateは30以上でない
	frameRate(30);
  // debugMode();
  normalMaterial();

  //速度と変異を入れベクトル
  currendVelocity = createVector(0,0,0);
  currentCoordinate = createVector(-windowWidth/2,-windowHeight/2,0);
}


function draw(){
  if(isData){
    //加速度データの取得と変位の計算
    if(isNewData){
      acc_list=[];
      vel_list=[[0],[0],[0]];
      coor_list=[[0],[0],[0]];
      //加速度リスト
      acc_list = getFetchData(acc_list)
      // print("acc_list",acc_list);
      //速度リスト
      vel_list = trapezoidFormula(acc_list, vel_list)
      // print("vel_list",vel_list);
      //変位リスト
      coor_list = trapezoidFormula(vel_list, coor_list)
      // print("coor_list",coor_list);

      k=0;
      isNewData =false;
    }

    background(33);
    // fill("#002fff")
    orbitControl();

    if (k<100){
      currentCoordinate.x = coor_list[0][k]*1000;
      currentCoordinate.y = coor_list[1][k]*1000;
      currentCoordinate.z = coor_list[2][k]*1000;
      translate(-currentCoordinate.x, -currentCoordinate.y, -currentCoordinate.z);
      sphere(10);
    }else{
      for(let i=0; i<100; i++){
        currentCoordinate.x = coor_list[0][i]*1000;
        currentCoordinate.y = coor_list[1][i]*1000;
        currentCoordinate.z = coor_list[2][i]*1000;
        push();
        translate(-currentCoordinate.x, -currentCoordinate.y, -currentCoordinate.z);
        sphere(10);
        pop();
      }
    }
    k+=1;
  }else{
    background(33);
  }

}
