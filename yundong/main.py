from flask import Flask, request
import json
import random
import user_manager as um

app = Flask('fbk')

@app.route('/', methods=['POST', 'GET'])
def main_page():
    return "Hello, FBK!"

def check_user_exists(uid):
    return um.get_user_exists(uid)

def ret(code, msg):
    return json.dumps({'code':int(code), 'msg':msg})

def check_user(data):
    if data == None:
        return False
    uid = data.get('uid')
    psw = data.get('psw')
    if uid == None or psw == None or uid == '' or psw == '':
        return False
    if not check_user_exists(uid):
        return False
    if um.get_user_info(uid, 'password') != psw:
        return False
    return True

@app.route('/user/register', methods=['GET', 'POST'])
def check_register():
    data = request.data
    data = json.loads(data)
    if data == None:
        return ret('400', 'empty form')
    uid = data.get('uid')
    psw = data.get('psw')
    if uid == None or psw == None or uid == '' or psw == '':
        return ret('400', 'invalid form')
    if check_user_exists(uid):
        return ret('400', 'user has already exists')
    else:
        um.create_user_data(uid, {'password':psw, 'state':0, 'score':0, 'prize_list':[]})
        return ret('200', 'ok')
        
@app.route('/user/score', methods=['GET', 'POST'])
def get_score():
    data = request.data
    data = json.loads(data)
    if not check_user(data):
        return ret('400', 'login failed')
    else:
        return ret('200', um.get_user_info(data.get('uid'), 'score'))

@app.route('/user/increase', methods=['GET', 'POST'])
def increase_score():
    data = request.data
    print("data is " + str(data))
    data = json.loads(data)
    if not check_user(data):
        return ret('400', 'login failed')
    else:
        um.set_user_info(data.get('uid'), 'score', um.get_user_info(data.get('uid'), 'score') + 1)
        return ret('200', um.get_user_info(data.get('uid'), 'score'))


@app.route('/user/achieve', methods=['GET', 'POST'])
def increase_state():
    data = request.data
    data = json.loads(data)
    if not check_user(data):
        return ret('400', 'login failed')
    else:
        um.set_user_info(data.get('uid'), 'state', um.get_user_info(data.get('uid'), 'state') + 1)
        return ret('200', um.get_user_info(data.get('uid'), 'state'))

@app.route('/user/gacha', methods=['GET', 'POST'])
def get_gacha():
    data = request.data
    data = json.loads(data)
    gacha_list = um.get_fbk_info('gacha_list')
    prize = random.sample(gacha_list, 1)[0]
    if not check_user(data):
        return ret('400', 'login failed')
    else:
        if um.get_user_info(data.get('uid'), 'state') <= 0:
            return ret('400', 'invalid gacha')
        else:
            um.set_user_info(data.get('uid'), 'state', um.get_user_info(data.get('uid'), 'state') - 1)
        pl = um.get_user_info(data.get('uid'), 'prize_list')
        pl.append(prize)
        um.set_user_info(data.get('uid'), 'prize_list', pl)
        return ret('200', um.get_user_info(data.get('uid'), 'prize_list'))

if __name__ == '__main__':
    app.run(debug=True, host='127.0.0.1', port=4080)
