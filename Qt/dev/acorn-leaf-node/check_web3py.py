
from web3 import Web3, EthereumTesterProvider

def main():
    w3 = Web3(EthereumTesterProvider())
    is_connected = w3.is_connected()
    print(is_connected)

if __name__ == "__main__":
    main()
