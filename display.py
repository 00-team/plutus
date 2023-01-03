
import os
import sys
from hashlib import sha3_224
from struct import Struct

ADMIN = Struct('<Q64s')
USER = Struct('<H12sBB4s64s50s')

R = '\033[31m'
G = '\033[32m'
B = '\033[34m'
C = '\033[36m'
S = '\033[0m'


EXT = {
    0: None,
    1: 'png',
    2: 'jpg',
    3: 'gif',
}


def b2s(data: bytes):
    return data.strip(b'\x00').decode()


def print_admin():
    c = 0
    with open('./data/admin.bin', 'rb') as f:
        while (a := f.read(ADMIN.size)):
            c += 1
            user_id, b_perms = ADMIN.unpack(a)
            perms = int.from_bytes(b_perms, 'little')
            print(f'{R}{c:<2}{S} | {G}{user_id:<8}{S} | {B}{perms:<8}{S}')


def print_user():
    c = 0
    with open('./data/user.bin', 'rb') as f:
        while (u := f.read(USER.size)):
            c += 1
            cc, b_phone, iflag, iext, pic, token, nick = USER.unpack(u)

            picn = sha3_224(c.to_bytes(8, 'little') + pic).hexdigest()
            ext = EXT.get(iext)

            user_id = f'{R}{c:<3}{S}'
            phone = f'{G}+{cc}{S} {b2s(b_phone):<12}'
            flag = f'{R}D{S}' if iflag == 1 else f'{G}N{S}'
            nickname = b2s(nick) or f'{C}NO NIC{S}'
            picture = f'{picn}.{G}{ext}{S}' if ext else f'{R}NO PIC{S}'

            print(' | '.join((user_id, phone, flag)))
            print(f'{nickname}\n{picture}\n{token[:20].hex(" ")}\n' + ('='*50))


def main(args):

    if len(args) < 2 or args[1] not in ['admin', 'user']:
        return print(f'usage: {args[0]} <admin | user>')

    if args[1] == 'admin':
        print_admin()

    elif args[1] == 'user':
        print_user()


if __name__ == '__main__':
    os.system('clear')
    main(sys.argv)
