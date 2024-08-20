#!/usr/bin/env python3

import sys
from web3 import Web3

def get_blockchain_name(chain_id):
    chain_names = {
        1: "Ethereum",
        11155111: "Sepolia",
    }
    return chain_names.get(chain_id, "Unknown Chain")

def main(address):
    alchemy_url = "https://eth-sepolia.g.alchemy.com/v2/FkV8_7Fa7GD44eTEiM3366RfO0mK_hFc"
    web3 = Web3(Web3.HTTPProvider(alchemy_url))

    if not web3.is_connected():
        print("Failed to connect to the Ethereum network")
        return

    chain_id = web3.eth.chain_id
    blockchain_name = get_blockchain_name(chain_id)
    print(f"Network: {blockchain_name}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python check_balance.py <address>")
        sys.exit(1)

    address = sys.argv[1]
    main(address)
