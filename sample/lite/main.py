import sqlite3
from timeit import timeit

from shared import get_user_nickname, get_user_phone, get_user_picture
from shared import get_user_token

con = sqlite3.connect('./test.db')
cur = con.cursor()

cur.execute('''CREATE TABLE user(
    id INTEGER PRIMARY KEY,
    phone VARCHAR(15) NOT NULL,
    token VARCHAR(64) NOT NULL,
    nickname VARCHAR(52),
    picture VARCHAR(24)
)''')


def stage_1():
    '''
    write 100k data randomly
    '''

    for i in range(1, 100_001):
        cur.execute('INSERT INTO user(phone, token, nickname, picture) VALUES (?, ?, ?, ?)', (
            get_user_phone(i),
            get_user_token(i),
            get_user_nickname(i),
            get_user_picture()
        ))


def main():
    stage_1()
    # print(timeit(lambda: stage_1(), number=1))

    con.commit()
    con.close()


if __name__ == '__main__':
    main()
