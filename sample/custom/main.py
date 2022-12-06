from binascii import a2b_hex, b2a_hex
from timeit import timeit

from shared import get_user_nickname, get_user_phone, get_user_picture
from shared import get_user_token


def stage_1():
    '''
    write 100k data randomly
    '''

    with open('test.bin', 'wb') as f:
        for i in range(1, 100_001):
            cc, phone = get_user_phone(i).split('-')

            row = i.to_bytes(8, 'big')
            row += int(cc).to_bytes(2, 'big')
            row += phone.encode()[:12].ljust(12, b'\0')
            row += get_user_token(i)
            nickname = get_user_nickname(i)
            row += nickname.encode().ljust(52, b'\0')
            row += a2b_hex(get_user_picture())

            # buffer.seek(0, SEEK_END)  # go the end
            f.write(row)
            # cur.execute('INSERT INTO user(phone, token, nickname, picture) VALUES (?, ?, ?, ?)', (
            #     get_user_phone(i),
            #     get_user_token(i),
            #     get_user_nickname(i),
            #     get_user_picture()
            # ))


def main():
    stage_1()
    # print(timeit(lambda: stage_1(), number=1))

    # con.commit()
    # con.close()


if __name__ == '__main__':
    main()
