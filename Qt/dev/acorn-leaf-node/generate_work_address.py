
from web3 import Web3
import os
import json
import sys

def generate_eth_address():
    w3 = Web3(Web3.HTTPProvider("https://eth-sepolia.g.alchemy.com/v2/FkV8_7Fa7GD44eTEiM3366RfO0mK_hFc"))

    if not w3.is_connected():
        print("Failed to connect to Sepolia network")
        return None

    account = w3.eth.account.create()

    address = account.address
    private_key = account._private_key.hex()

    output = {
        "address": address,
        "private_key": private_key
    }

    return json.dumps(output)

if __name__ == "__main__":
    address_json = generate_eth_address()
    if address_json:
        print(address_json)
    else:
        print("Failed to generate Ethereum address")
