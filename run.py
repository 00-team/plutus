
from binascii import a2b_hex, b2a_hex
from hashlib import sha3_512
from io import SEEK_END, BufferedRandom
from random import choice, choices
from timeit import timeit

'''
user_id: 8 bytes - unsigned int
phone: 2 bytes cc, 7 bytes phone - unsigned int
token: hashed 64 bytes
nickname: 52 bytes encoded - char
picture: 12 bytes - hex
'''


DB_NAME = 'test.bin'

A_TOKNE = sha3_512(b'A_TOKEN').digest()
# 98 is cc
A_PHONE = (98).to_bytes(2, 'big') + b'09001122333'[:12].ljust(12, b'\0')
ROW_SIZE = 150


User = tuple[int, str, str, str, str]
InsertUser = tuple[str, str, str, str]


def get_user_id(buffer: BufferedRandom) -> int:
    buffer.seek(0)
    data = buffer.read(1)

    if not data:
        return 1

    # buffer.seek(0)

    # while (row := buffer.read(ROW_SIZE)):
    #     if not row[20]:
    #         return buffer.tell() // ROW_SIZE

    buffer.seek(-ROW_SIZE, SEEK_END)

    return int.from_bytes(buffer.read(8), 'big') + 1


def insert_user(buffer: BufferedRandom, user: InsertUser):
    cc_phone, token, nickname, picture = user
    cc, phone = cc_phone.split('-')

    row = get_user_id(buffer).to_bytes(8, 'big')
    row += int(cc).to_bytes(2, 'big')
    row += phone.encode()[:12].ljust(12, b'\0')
    row += a2b_hex(token)
    row += nickname.encode()[:52].ljust(52, b'\0')
    row += a2b_hex(picture)

    buffer.seek(0, SEEK_END)  # go the end
    buffer.write(row)


def get_user(row: bytes):
    user_id = int.from_bytes(row[:8], 'big')
    cc = int.from_bytes(row[8:8+2], 'big')
    phone = row[10:10 + 12].strip(b'\0').decode()
    token = b2a_hex(row[22:22+64]).decode()
    nickname = row[86:86+52].strip(b'\0').decode()
    picture = b2a_hex(row[138:]).decode()

    return (user_id, f'{cc}-{phone}', token, nickname, picture)


def print_user(user: User):
    user_id, cc_phone, token, nickname, picture = user
    print('-'*40)
    print(f'user id: {user_id}')
    cc_phone = cc_phone.split('-')
    print(f'phone: +{cc_phone[0]} {cc_phone[1]}')
    print(f'nickname: {nickname}')
    print(f'picture: {picture}')
    print(f'token: {token[:20]}...')
    print('-'*40)


def write():
    with open(DB_NAME, 'wb') as f:
        for i in range(1, 1_000_000):
            row = i.to_bytes(8, 'big')
            row += A_PHONE
            row += A_TOKNE
            row += (f'nickname for {i}'.encode())[:52].ljust(52, b'\0')

            picture = ''.join(choices('0123456789abcdef', k=24))
            row += a2b_hex(picture)

            f.write(row)


def main():
    # ids = user_id, token, phone

    # -- start --
    # write 100K users with random data

    # read randomly by ids
    # insert randomly by ids
    # get count
    # update randomly by ids
    # delete randomly by ids
    # get count
    # insert randomly by ids
    # get count

    pass
    # write()
    # return

    # user_id = choice(range(1, 1_001))

    # with open(DB_NAME, 'r+b') as f:
    #     print('read: ', f.readable())
    #     print('write: ', f.writable())
    #     # f.seek(9_000 * ROW_SIZE)
    #     # f.write(b'\0' * ROW_SIZE)

    #     print(timeit(lambda: get_user_id(f), number=10_000))

    #     # row = f.read(ROW_SIZE)
    #     # print_user(get_user(row))
    #     # insert_user(f, (
    #     #     7, '1-555888496',
    #     #     sha3_512(b'ggez').hexdigest(),
    #     #     'gg nickname cool heh',
    #     #     ''.join(choices('0123456789abcdef', k=24))
    #     # ))

    # with open(DB_NAME, 'rb') as f:
    #     print(f'{user_id}')
    #     f.seek((user_id - 1) * ROW_SIZE)
    #     row = f.read(ROW_SIZE)
    #     print(get_user(row))
    #     # while (row := f.read(ROW_SIZE)):
    #     #     user_id = int.from_bytes(row[:8], 'big')
    #     #     cc = int.from_bytes(row[8:8+2], 'big')
    #     #     phone = row[10:10 + 12].decode()
    #     #     token = b2a_hex(row[22:22+64]).decode()
    #     #     nickname = row[86:86+52].decode()
    #     #     picture = b2a_hex(row[138:]).decode()
    #     #     print(user_id)
    #     #     # print(cc, phone)
    #     #     # print(token)
    #     #     print(nickname)
    #     #     print(picture)
    #     #     break


if __name__ == '__main__':
    main()
