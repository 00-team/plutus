
import socket
import struct
from binascii import b2a_hex
from enum import Enum, auto
from hashlib import sha3_224, sha3_512
from pathlib import Path
from struct import Struct

sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
BYTE_ORDER = 'little'
DELETED_FLAG = ord('D')

# cc - phone - flag - ext - pic - token - nickname
USER_STRUCT = Struct('<H12sBB4s64s50s')
# user_id - user - created: u char - 1 pad
LOGIN_RESPONSE = Struct(f'<Q{USER_STRUCT.size}sBx')
# cc: u short - char[12] - char[64]
LOGIN_ARGS = Struct(f'<H12s64s')


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
    USER_COUNT = auto()
    USER_LOGIN = auto()
    USER_UPDATE = auto()

    def __add__(self, other):
        return self.value + other


EXT = {
    0: None,

    1: 'png',
    2: 'jpg',
    3: 'gif',

    'png': 1,
    'jpg': 2,
    'gif': 3,

    None: 0
}


class User:
    user_id: int
    phone: tuple[int, str]
    nickname: str | None
    picture: str | None

    __user_id: bytes
    __cc: int
    __phone: bytes
    __flag: int
    __ext: int
    __picture: bytes
    __token: bytes
    __nickname: bytes

    def __init__(self, user_id: int, data: bytes) -> None:
        self._updateattr_(user_id, data)

    @classmethod
    def count(cls, exact=False) -> int:
        # get user count
        # by default user count is based on files size
        request = RQT.USER_COUNT + exact.to_bytes(1, BYTE_ORDER)

        sock.send(request)
        response = sock.recv(8)

        return int.from_bytes(response, BYTE_ORDER)

    @classmethod
    def login(cls, cc: int, phone: str, token: bytes) -> tuple['User', bool]:
        # provide a phone number and a token
        # if user the exists update the token and return it
        # if not return a new created user
        request = RQT.USER_LOGIN + LOGIN_ARGS.pack(cc, phone.encode(), token)
        sock.send(request)

        response = sock.recv(LOGIN_RESPONSE.size)
        user_id, user_data, created = LOGIN_RESPONSE.unpack(response)

        return cls(user_id, user_data), bool(created)

    @classmethod
    def get(cls, user_id: int, input_token: bytes = None) -> 'User':
        # get a user via user_id
        # if token was provided, check the token as well

        # 2 bytes for request id
        # 8 bytes for user_id

        user_id_b = user_id.to_bytes(8, BYTE_ORDER)

        sock.send(RQT.USER_GET + user_id_b)
        response = sock.recv(1 + USER_STRUCT.size)

        if response[0] == 4:
            return None

        user = cls(user_id, response[1:])

        if (not input_token is None) and input_token != user.token:
            return None

        return user

    def _updateattr_(self, user_id: int, data: bytes):
        cc, phone, flag, ext, picture, token, nick = USER_STRUCT.unpack(data)

        if user_id:
            self.__user_id = user_id.to_bytes(8, BYTE_ORDER)
            self.user_id = user_id

        self.__cc = cc
        self.__phone = phone
        self.__flag = flag
        self.__ext = ext
        self.__picture = picture
        self.__token = token
        self.__nickname = nick

        self.phone = (cc, bin2str(phone))
        self.nickname = bin2str(nick) or None
        self.picture = None

        pic_ext = EXT.get(ext)
        if not pic_ext is None:
            pic_name = sha3_224(self.__user_id + picture).hexdigest()
            self.picture = f'{pic_name}.{pic_ext}'

    def _pars_update_args_(self, kwargs: dict) -> bytes:
        if not kwargs:
            return

        change = False

        cc = self.__cc
        phone = self.__phone
        flag = self.__flag
        ext = self.__ext
        picture = self.__picture
        token = self.__token
        nickname = self.__nickname

        if self.__flag == DELETED_FLAG:
            return

        if kwargs.get('delete', False):
            flag = DELETED_FLAG
            return USER_STRUCT.pack(
                cc, phone, flag, ext,
                picture, token, nickname,
            )

        if 'nickname' in kwargs:
            nick = kwargs.get('nickname')
            if nick != self.nickname:
                change = True
                if nick is None:
                    nickname = b'\x00'
                else:
                    nickname = str(nick).encode()

        if 'picture' in kwargs:
            pic_ext = kwargs.get('picture')
            if pic_ext in ['png', 'jpg', 'gif', None]:
                ext = EXT[pic_ext]
                if ext != self.__ext:
                    change = True

        if change:
            return USER_STRUCT.pack(
                cc, phone, flag, ext,
                picture, token, nickname,
            )

    def update(self, **kwargs) -> bool:
        user = self._pars_update_args_(kwargs)
        if user is None:
            return False

        request = RQT.USER_UPDATE + self.__user_id + user

        sock.send(request)
        status = sock.recv(1)

        if status == b'\x00':
            self._updateattr_(0, user)
            return True

        return False

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
    # sock.send(b'')
    # return

    user, c = User.login(98, '09111111111', sha3_512(b'gg token ez').digest())
    print(user.picture)

    # user, c = User.login(98, '09111111111', sha3_512(b'gg token ez').digest())

    # user.update(nickname='12GG')
    # user.update(picture='png')
    # print(user.user_id)
    # print(user.nickname)
    # sock.send(b'')

    sock.close()


if __name__ == '__main__':
    main()
