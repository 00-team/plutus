
import socket
import struct
from binascii import b2a_hex
from enum import Enum, auto
from hashlib import sha3_512
from pathlib import Path

sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
BYTE_ORDER = 'little'


def bin2str(data: bytes) -> str:
    return data.strip(b'\x00').decode()


class UserNotFound(Exception):
    user_id: int

    def __init__(self, *args, user_id) -> None:
        super().__init__(*args)
        self.user_id = user_id


class RQT(Enum):
    '''Request Type'''

    def _generate_next_value_(name, start, count, last_values):
        return (0 + count).to_bytes(2, BYTE_ORDER)

    USER_GET = auto()
    USER_DEL = auto()
    USER_COUNT = auto()
    USER_LOGIN = auto()
    USER_UPDATE = auto()


class User:
    user_id: int
    nickname: str
    picture: str
    token: bytes
    phone: tuple[int, str]

    def __init__(self, user_id, phone, token, nickname, picture) -> None:
        self.user_id = user_id
        self.user_id_b = user_id.to_bytes(8, BYTE_ORDER)
        self.phone = phone
        self.token = token
        self.nickname = nickname
        self.picture = picture

    @classmethod
    def count(cls, exact=False) -> int:
        # get user count
        # by default user count is based on files size
        request = RQT.USER_COUNT.value + exact.to_bytes(1, BYTE_ORDER)

        sock.send(request)
        response = sock.recv(8)

        return int.from_bytes(response, BYTE_ORDER)

    @classmethod
    def login(cls, cc: int, phone: str, token: bytes):
        # provide a phone number and a token
        # if user the exists update the token and return it
        # if not return a new created user
        request = RQT.USER_LOGIN.value + cc.to_bytes(2, BYTE_ORDER)
        request += phone.encode() + token
        print(len(request))

        print(request.hex('-'))

        sock.send(request)
        sock.recv(135)

    @classmethod
    def get(cls, user_id: int, input_token: bytes = None):
        # get a user via user_id
        # if token was provided, check the token as well

        # 2 bytes for request id
        # 8 bytes for user_id

        user_id_b = user_id.to_bytes(8, BYTE_ORDER)

        sock.send(RQT.USER_GET.value + user_id_b)
        response = sock.recv(135)

        if response[0] == 4:
            raise UserNotFound(user_id=user_id)

        # cc, phone, flag, ext, picture, token, nickname
        user_struct = struct.unpack('H12sBB4s64s50s', response[1:])

        return cls(
            user_id,
            phone=(user_struct[0], bin2str(user_struct[1])),
            token=user_struct[5],
            nickname=bin2str(user_struct[6]),
            picture=(user_id_b + user_struct[4]).hex() + '.png',
        )

    def update(self):
        pass

    def delete(self):

        if self.user_id == -1:
            raise UserNotFound(user_id=-1)

        sock.send(RQT.USER_DEL.value + self.user_id_b)
        response = sock.recv(1)

        if response == b'\x04':
            raise UserNotFound(user_id=self.user_id)

        self.user_id = -1

    def __repr__(self) -> str:
        return f'<{self.__class__.__name__}: {self.user_id}>'


def connect():
    server_addr = Path('/tmp/plutus.server.sock')

    if not server_addr.exists():
        print('server is not running')
        exit(1)

    client_addr = Path('/tmp/plutus.client.sock')
    client_addr.unlink(True)

    sock.bind(str(client_addr))
    sock.connect(str(server_addr))


def main():
    connect()
    # try:
    #     user = User.get(8)
    #     user.delete()
    #     user.delete()
    #     # User.get(13)
    # except UserNotFound as e:
    #     print('User %d Not Found' % e.user_id)

    randomtoken = sha3_512(b'gg token ez')
    print(randomtoken.hexdigest())

    User.login(98, '091244444444', randomtoken.digest())
    print('count: ', User.count())
    print('count exact: ', User.count(True))
    sock.close()


if __name__ == '__main__':
    main()
