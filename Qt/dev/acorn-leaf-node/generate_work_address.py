#!/usr/bin/env python3

from web3 import Web3
import os
import json
import sys

def generate_eth_address():
    # Connect to Sepolia network
    w3 = Web3(Web3.HTTPProvider("https://eth-sepolia.g.alchemy.com/v2/FkV8_7Fa7GD44eTEiM3366RfO0mK_hFc"))

    if not w3.is_connected():
        print("Failed to connect to Sepolia network")
        return None

    # Generate a new account
    account = w3.eth.account.create()

    # Prepare the keys to save
    address = account.address
    private_key = account._private_key.hex()

    # Prepare JSON output
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
