
from hashlib import sha3_512


def get_user_token(user_id: int):
    return sha3_512(f'GG_TOKEN_{user_id}_GG'.encode()).digest()


def get_user_phone(user_id: int):
    user_id = user_id[:12]
    return f'98-{user_id}'


def get_user_nickname(user_id: int):
    return f'nickname for {user_id}'[:52]


def get_user_picture():
    return '474747474747474747474747'
