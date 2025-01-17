#include "Identity.h"
#include "I2PEndian.h"
#include "LeaseSet.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "common/key.hpp"

static int printHelp(const char * exe, int exitcode)
{
	std::cout << "usage: " << exe << " [-v] [-d] [-b] privatekey.dat" << std::endl;
	return exitcode;
}

std::string ConvertTime (time_t t)
{
	struct tm *tm = localtime(&t);
	char date[128];
	snprintf(date, sizeof(date), "%02d/%02d/%d %02d:%02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
	return date;
}

int main(int argc, char * argv[])
{
	if(argc == 1) {
		return printHelp(argv[0], -1);
	}

	int opt;
	bool print_full = false;
	bool print_blinded = false;
	bool verbose = false;
	while((opt = getopt(argc, argv, "hvdb")) != -1) {
		switch(opt){
		case 'h':
			return printHelp(argv[0], 0);
		case 'v':
			verbose = true;
			break;
		case 'd':
			print_full = true;
			break;
		case 'b':
			print_blinded = true;
			break;
		default:
			return printHelp(argv[0], -1);
		}
	}

	std::string fname(argv[optind]);
	i2p::data::PrivateKeys keys;
	{
		std::vector<uint8_t> buff;
		std::ifstream inf;
		inf.open(fname);
		if (!inf.is_open()) {
			std::cout << "cannot open private key file " << fname << std::endl;
			return 2;
		}
		inf.seekg(0, std::ios::end);
		const std::size_t len = inf.tellg();
		inf.seekg(0, std::ios::beg);
		buff.resize(len);
		inf.read((char*)buff.data(), buff.size());
		if (!keys.FromBuffer(buff.data(), buff.size())) {
			std::cout << "bad key file format" << std::endl;
			return 3;
		}
	}

	auto dest = keys.GetPublic();
	if(!dest) {
		std::cout << "failed to extract public key" << std::endl;
		return 3;
	}

	const auto & ident = dest->GetIdentHash();
	if (verbose) {
		std::cout << "Destination: " << dest->ToBase64() << std::endl;
		std::cout << "Destination Hash: " << ident.ToBase64() << std::endl;
		std::cout << "B32 Address: " << ident.ToBase32() << ".b32.i2p" << std::endl;
		std::cout << "Signature Type: " << SigTypeToName(dest->GetSigningKeyType()) << std::endl;
		std::cout << "Encryption Type: " << (int) dest->GetCryptoKeyType() << std::endl;
		if (keys.IsOfflineSignature ())
		{
			std::cout << "Offline signature" << std::endl;
			const auto& offlineSignature = keys.GetOfflineSignature ();
			std::cout << "Expires: " << ConvertTime (bufbe32toh(offlineSignature.data ())) << std::endl;
			std::cout << "Transient Signature Type: " << SigTypeToName(bufbe16toh(offlineSignature.data () + 4)) << std::endl;
		}
	} else {
		if(print_full) {
			std::cout << dest->ToBase64() << std::endl;
		} else {
			std::cout << ident.ToBase32() << ".b32.i2p" << std::endl;
		}
	}

	if (print_blinded) {
		if (dest->GetSigningKeyType () == i2p::data::SIGNING_KEY_TYPE_REDDSA_SHA512_ED25519 ||
		    dest->GetSigningKeyType () == i2p::data::SIGNING_KEY_TYPE_EDDSA_SHA512_ED25519 ) // 11 or 7
		{
			i2p::data::BlindedPublicKey blindedKey (dest);
			std::cout << "b33 address: " << blindedKey.ToB33 () << ".b32.i2p" << std::endl;
			std::cout << "Today's store hash: " << blindedKey.GetStoreHash ().ToBase64 () << std::endl;
		}
		else
			std::cout << "Invalid signature type " << SigTypeToName (dest->GetSigningKeyType ()) << std::endl;
	}
}
