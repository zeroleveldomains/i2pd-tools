# i2pd-tools

This repository contains tools that supplement i2pd.

Notice: git submodules are used so make sure to clone this repository recursively

    git clone --recursive https://github.com/purplei2pd/i2pd-tools

## Tools included

### keygen

Generate an i2p private key

#### Usage

Make a DSA-SHA1 destination key

    ./keygen privkey.dat

Make an destination key with a certain key type

    ./keygen privkey.dat <number>


| key type             | number |
| -------------------- | ------ |
| DSA SHA1             | 0      |
| ECDSA_SHA256_P256    | 1      |
| ECDSA_SHA384_P384    | 2      |
| ECDSA_SHA512_P521    | 3      |
| RSA_SHA256_2048      | 4      |
| RSA_SHA384_3072      | 5      |
| RSA_SHA512_4096      | 6      |
| EDDSA_SHA512_ED25519 | 7      |



### keyinfo

Prints information about an i2p private key

#### Usage

Print just the b32 address for this key

     ./keyinfo privatekey.dat

... just the base64 address

    ./keyinfo -d privatekey.dat

Print all info about the public key

    ./keyinfo -v privatekey.dat