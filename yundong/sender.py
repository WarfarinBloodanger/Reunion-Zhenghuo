import requests
import json

while True:
    s = input('Input data: ')
    ss = s.split(' ')
    j = json.loads(ss[1])
    print('Send: ' + str(j))
    print(requests.post(url='http://127.0.0.1:4080/user/' + ss[0],
                       data=ss[1]).content)
