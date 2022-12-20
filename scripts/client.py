
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
    pass


class RQT(Enum):
    '''Request Type'''

    def _generate_next_value_(name, start, count, last_values):
        return (0 + count).to_bytes(2, BYTE_ORDER)

    USER_GET = auto()
    USER_DEL = auto()
    USER_COUNT = auto()
    USER_UPDATE = auto()


class User:
    user_id: int
    nickname: str
    picture: str
    token: bytes
    phone: tuple[int, str]

    def __init__(self, **kwargs) -> None:
        self.user_id = kwargs['user_id']
        self.nickname = kwargs['nickname']
        self.picture = kwargs['picture']
        self.token = kwargs['token']
        self.phone = kwargs['phone']

    @classmethod
    def count(cls, exact=False):
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
        pass

    @classmethod
    def get(cls, user_id: int, input_token: bytes = None):
        # get a user via user_id
        # if token was provided, check the token as well

        # 2 bytes for request id
        # 8 bytes for user_id

        user_id_b = user_id.to_bytes(8, BYTE_ORDER)

        sock.send(RQT.USER_GET.value + user_id_b)
        response = sock.recv(135)

        if response[0] == b'\x04':
            raise UserNotFound

        # cc, phone, flag, ext, picture, token, nickname
        user_struct = struct.unpack('H12sBB4s64s50s', response[1:])

        return cls(
            user_id=user_id,
            nickname=bin2str(user_struct[6]),
            picture=(user_id_b + user_struct[4]).hex() + '.png',
            token=user_struct[5],
            phone=(user_struct[0], bin2str(user_struct[1]))
        )
        # typedef struct {
        #     unsigned short cc;
        #     char phone[12];
        #     unsigned char flag;  // DELETED_FLAG or anything else
        #     unsigned char ext;
        #     unsigned char picture[USER_PICTURE_SIZE];
        #     char token[USER_TOKEN_SIZE];
        #     char nickname[USER_NICNAME_SIZE];
        # } User;

    def update(self):
        pass

    def delete(self):
        pass

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
    print('user: ', User.get(12))
    print('count: ', User.count())
    sock.close()


if __name__ == '__main__':
    main()
