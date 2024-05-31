from flask import Flask, request, make_response, jsonify, render_template
import json, os

app = Flask(__name__)

class const:
    data_path = [
        "data/m5stick_tennis_prev0_data.json",
        "data/m5stick_tennis_prev1_data.json",
        "data/m5stick_tennis_prev2_data.json",
        "data/m5stick_tennis_prev3_data.json",
        "data/m5stick_tennis_prev4_data.json"
    ]



    
@app.route("/m5stick_tennis/post", methods=["POST"])
def m5stick_tennis_post():
    #データの取得
    data = request.get_data().decode("ascii")
    # data = json.dumps(data)
    # print("data",data,flush=True)

    os.makedirs("./data", exist_ok=True)

    for i in range(len(const.data_path)-1):
        prev_data = ""
        is_file = os.path.isfile(const.data_path[3-i])
        # print("is_file:", is_file)
        if is_file:
            with open(const.data_path[3-i]) as f:
                prev_data = f.read()
                # print("prev_data",prev_data,flush=True)
        with open(const.data_path[4-i], mode="w") as f:
            f.write(prev_data)

    with open(const.data_path[0], mode="w") as f:
            f.write(data)
    
    return {"prevdata":prev_data}


#JSON形式のデータリストを返す
@app.route("/m5stick_tennis/data")
def m5stick_tennis_data():
    data_list = ['{}','{}','{}','{}','{}']
    os.makedirs("./data", exist_ok=True)      

    for i in range(len(const.data_path)):
        is_file = os.path.isfile(const.data_path[i])
        if is_file:
            with open(const.data_path[i], mode="r") as f:
                data_list[i] = json.load(f)

    return {"data_list":data_list}
    


#ブラウザからのアクセスを処理する、htmlを表示
@app.route('/')
def view():
    return render_template("index.html")



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True, reloader_interval=5)

