
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

            if len(a) < ADMIN.size:
                print(f'{R}{c:<2}{S} | invalid admin {R}detected{S}')
                continue

            user_id, b_perms = ADMIN.unpack(a)
            perms = int.from_bytes(b_perms, 'little')
            print(f'{R}{c:<2}{S} | {G}{user_id:<8}{S} | {B}{perms:<8}{S}')


def print_user():
    c = 0
    with open('./data/user.bin', 'rb') as f:
        while (u := f.read(USER.size)):
            c += 1
            user_id = f'{R}{c:<3}{S}'

            if len(u) < USER.size:
                print(f'{user_id} | invalid user {R}detected{S}')
                continue

            cc, b_phone, iflag, iext, pic, token, nick = USER.unpack(u)

            picn = sha3_224(c.to_bytes(8, 'little') + pic).hexdigest()
            ext = EXT.get(iext)

            phone = f'{G}+{cc}{S} {b2s(b_phone):<12}'
            flag = f'{R}D{S}' if iflag == 1 else f'{G}N{S}'
            nickname = b2s(nick) or f'{C}NO NIC{S}'
            picture = f'{picn}.{G}{ext}{S}' if ext else f'{R}NO PIC{S}'

            print(' | '.join((user_id, phone, flag)))
            print(f'{nickname}\n{picture}\n{token[:20].hex(" ")}\n' + ('='*50))


def add_master(user_id: int):
    with open('./data/admin.bin', 'r+b') as f:
        perms = (1).to_bytes(64, 'little')
        f.seek(0, 2)
        f.write(user_id.to_bytes(8, 'little') + perms)


def usage(app):
    print(f'usage: {app} <admin | user | master [user_id]>')


def main(args: list[str]):

    if len(args) < 2 or args[1] not in ['admin', 'user', 'master']:
        return usage(args[0])

    if args[1] == 'admin':
        print_admin()

    elif args[1] == 'user':
        print_user()

    elif args[1] == 'master':
        try:
            user_id = int(args[2])
            add_master(user_id)
        except (ValueError, IndexError):
            return usage(args[0])


if __name__ == '__main__':
    os.system('clear')
    main(sys.argv)
