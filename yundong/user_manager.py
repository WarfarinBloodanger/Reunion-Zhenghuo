import json
import os

def get_user_data_name(uid):
        return 'userdata/' + str(uid) + '.json'

def create_user_data(uid, data):
        json.dump(data, open(get_user_data_name(uid), 'w'), indent=4)

def get_user_exists(uid):
        user_file = get_user_data_name(uid)
        return os.path.exists(user_file)

def get_user_data(uid):
        user_file = get_user_data_name(uid)
        if not os.path.exists(user_file):
                return {}
        with open(user_file) as f:
                data = json.load(f)
                return data

def set_user_info(uid, key, val):
        data = get_user_data(uid)
        data[key] = val
        json.dump(data, open(get_user_data_name(uid), 'w'), indent=4)


def get_user_info(uid, key):
        data = get_user_data(uid)
        if key not in data:
                set_user_info(uid, key, '')
        return get_user_data(uid)[key]

def get_fbk_data():
        return get_user_data('fubuki')

def get_fbk_info(key):
        return get_user_info('fubuki', key)

def set_fbk_info(key, val):
        set_user_info('fubuki', key, val)
        

