let isFetching = false;
let isNewData = false;
let isData = false;

let intervalTime = 2000;

let fetchedData;


/**
* データを要求し、そのデータを返す。
* @param {URL} endpoint fetchするエンドポイントです
// * @return {any[]}
*/
async function operateNode(endpoint) {
  console.log("operateNode");
  // let elm = document.querySelector("#show-data");
  /*
  以下のfetchでflaskからデータを撮ってきてる
  */
  const response = await fetch(endpoint);
  fetchedData = await response.json()
  console.log("respons",response)
  console.log("fetchedData", fetchedData)

}


async function startFetch() {
  console.log("startFetch");
  if (!isFetching) {
      isFetching = true;
      document.querySelector("#clickButton").innerText = "stop";
      while (isFetching){
          await operateNode("/m5stick_tennis/data");
          await new Promise(resolve => setTimeout(resolve, intervalTime));
      }
  }
}

function stopFetch() {
  console.log("stopfetch");
  if (isFetching) {
      document.querySelector("#clickButton").innerText = "start";
      isFetching = false;
      //p5において加速度の取得と速度、変位の計算
      isNewData = true;
      //軌跡の描画
      isData = true;
  }
}

async function clickHandler() {
  if (!isFetching) {
      await startFetch();
  } else {
      stopFetch();
  }
}