
DB_NAME = 'test.bin'

S = '-'.encode()

def main():
    with open(DB_NAME, 'wb') as f:
        for i in range(200):
            print(i)
            f.write((i).to_bytes(4, 'little'))
            f.write(S)
        
    # with open(DB_NAME, 'rb') as f:
    #     f.seek(4 * 30)
    #     print(int.from_bytes(f.read(4), 'big', signed=True))

if __name__ == '__main__':
    main()