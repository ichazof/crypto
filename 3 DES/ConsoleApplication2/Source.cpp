#define _CRT_SECURE_NO_WARNINGS
#include "iostream"
#include <string>
#include <conio.h>
#include <string>
#include "..\cryptopp565\des.h"
#include "..\cryptopp565\cryptlib.h"
#include "..\cryptopp565\modes.h"
#include "..\cryptopp565\osrng.h"
#include "..\cryptopp565\filters.h"
#include "..\cryptopp565\cbcmac.h"
#include <vector>

#pragma comment(lib,"cryptlib.lib")


class MyFile
{
private:
	std::vector<byte> _data;
	FILE* _file;

public:
	MyFile()
		: _file(NULL), _data(NULL)
	{

	}

	bool Open(std::string name)
	{
		const char *_name = name.c_str();
		_file = fopen(_name, "rb");

		if (_file != NULL)
		{
			fseek(_file, 0, SEEK_END);
			int size = ftell(_file);
			rewind(_file);
			_data.resize(size);
			fread(_data.data(), 1, size, _file);
			fclose(_file);
		}
		return !_data.empty();
	}
	bool Write(std::string name)
	{
		const char *_name = name.c_str();
		_file = fopen(_name, "w+b");
		int flag = fwrite(_data.data(), 1, _data.size(), _file);
		fclose(_file);
		return !(flag == 0);
	}

	std::vector<byte> &GetData() { return _data; }

};

class DES
{
private:
	MyFile plaintext;
	MyFile _key;
	MyFile _iv;
	MyFile _ciphertext;
	std::string input_file;
public:
	DES()
	{

	}

	void Encrypt(int cipher_mode)
	{
		std::cout << "File name to encrypt:";
		std::cin >> input_file;
		if (!plaintext.Open(input_file))
		{
			std::cout << "error\n" << std::endl;
			return;
		}
		system("cls");

		byte key[CryptoPP::DES::DEFAULT_KEYLENGTH];
		byte iv[CryptoPP::DES::BLOCKSIZE];

		int need_key;
		std::cout << "create key and initial vector? \n1. yes \n2. no\n";
		std::cin >> need_key;
		system("cls");
		if (need_key == 1)
		{
			std::cout << "creating key and initial vector...\n";
			CryptoPP::AutoSeededRandomPool rand(true);
			rand.GenerateBlock(key, CryptoPP::DES::DEFAULT_KEYLENGTH);
			rand.GenerateBlock(iv, CryptoPP::DES::BLOCKSIZE);

			for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
			{
				_key.GetData().push_back(key[i]);
			}

			for (int i = 0; i < CryptoPP::DES::BLOCKSIZE; i++)
			{
				_iv.GetData().push_back(iv[i]);
			}

			_key.Write("key.txt");
			_iv.Write("iv.txt");
			std::cout << "create ok\n";
		}
		else
		{
			std::cout << "Enter name of key: ";
			std::cin >> input_file;
			if (!_key.Open(input_file))
			{
				std::cout << "error\n";
				return;
			}
			std::cout << "Enter name of initial vector: ";
			std::cin >> input_file;
			if (!_iv.Open(input_file))
			{
				std::cout << "error\n";
				return;
			}
			for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
			{
				key[i] = _key.GetData().at(i);
			}
			for (int i = 0; i < CryptoPP::DES::BLOCKSIZE; i++)
			{
				iv[i] = _iv.GetData().at(i);
			}
		}
		std::cout << "encrypting..";

		_ciphertext.GetData().resize(plaintext.GetData().size() + CryptoPP::DES::BLOCKSIZE);
		CryptoPP::ArraySink buf(&_ciphertext.GetData()[0], _ciphertext.GetData().size());
		switch (cipher_mode)
		{
		case 1:
		{
			CryptoPP::ECB_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key));
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 2:
		{
			CryptoPP::CBC_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 3:
		{
			CryptoPP::CFB_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 4:
		{
			CryptoPP::OFB_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 5:
		{
			CryptoPP::CTR_Mode<CryptoPP::DES>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		}
		_ciphertext.GetData().resize(buf.TotalPutLength());
		std::cout << "encryption ok" << std::endl << "Name to save: ";
		std::cin >> input_file;
		_ciphertext.Write(input_file);
		system("cls");
		}

	void Decrypt(int cipher_mode)
	{
		byte key[CryptoPP::DES::DEFAULT_KEYLENGTH];
		byte iv[CryptoPP::DES::BLOCKSIZE];
		std::cout << "File name to dencrypt: ";
		std::cin >> input_file;
		if (!_ciphertext.Open(input_file))
		{
			std::cout << "error\n";
			return;
		}
		system("cls");
		if (!_key.Open("key.txt"))
		{
			std::cout << "error\n";
			return;
		}
		if (!_iv.Open("iv.txt"))
		{
			std::cout << "error\n";
			return;
		}
		for (int i = 0; i < CryptoPP::DES::DEFAULT_KEYLENGTH; i++)
		{
			key[i] = _key.GetData().at(i);
		}
		for (int i = 0; i < CryptoPP::DES::BLOCKSIZE; i++)
		{
			iv[i] = _iv.GetData().at(i);
		}
		plaintext.GetData().resize(_ciphertext.GetData().size() + CryptoPP::DES::BLOCKSIZE);
		CryptoPP::ArraySink buf(&plaintext.GetData()[0], plaintext.GetData().size());

		std::cout << "decryption..";

		switch (cipher_mode)
		{
		case 1:
		{
			CryptoPP::ECB_Mode<CryptoPP::DES>::Decryption dec(key, sizeof(key));
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(dec, new CryptoPP::Redirector(buf)));
			break;
		}
		case 2:
		{
			CryptoPP::CBC_Mode<CryptoPP::DES>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 3:
		{
			CryptoPP::CFB_Mode<CryptoPP::DES>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 4:
		{
			CryptoPP::OFB_Mode<CryptoPP::DES>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 5:
		{
			CryptoPP::CTR_Mode<CryptoPP::DES>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		}
		plaintext.GetData().resize(buf.TotalPutLength());
		std::cout << "decryption ok" << std::endl << "Name to save:";
		std::cin >> input_file;
		system("cls");
		plaintext.Write(input_file);
		std::cout << "ok\n";
	}

};

class DES_EDE3
{
private:
	MyFile plaintext;
	MyFile _key;
	MyFile _iv;
	MyFile _ciphertext;
	std::string input_file;
public:
	DES_EDE3()
	{

	}

	void Encrypt(int cipher_mode)
	{
		std::cout << "File name to encrypt:";
		std::cin >> input_file;
		if (!plaintext.Open(input_file))
		{
			std::cout << "error\n" << std::endl;
			return;
		}
		system("cls");

		byte key[CryptoPP::DES_EDE3::DEFAULT_KEYLENGTH];
		byte iv[CryptoPP::DES_EDE3::BLOCKSIZE];

		int need_key;
		std::cout << "create key and initial vector? \n1. yes \n2. no\n";
		std::cin >> need_key;
		system("cls");
		if (need_key == 1)
		{
			std::cout << "creating key and initial vector...\n";
			CryptoPP::AutoSeededRandomPool rand(true);
			rand.GenerateBlock(key, CryptoPP::DES_EDE3::DEFAULT_KEYLENGTH);
			rand.GenerateBlock(iv, CryptoPP::DES_EDE3::BLOCKSIZE);

			for (int i = 0; i < CryptoPP::DES_EDE3::DEFAULT_KEYLENGTH; i++)
			{
				_key.GetData().push_back(key[i]);
			}

			for (int i = 0; i < CryptoPP::DES_EDE3::BLOCKSIZE; i++)
			{
				_iv.GetData().push_back(iv[i]);
			}

			_key.Write("key.txt");
			_iv.Write("iv.txt");
			std::cout << "create ok\n";
		}
		else
		{
			std::cout << "Enter name of key: ";
			std::cin >> input_file;
			if (!_key.Open(input_file))
			{
				std::cout << "error\n";
				return;
			}
			std::cout << "Enter name of initial vector: ";
			std::cin >> input_file;
			if (!_iv.Open(input_file))
			{
				std::cout << "error\n";
				return;
			}
			for (int i = 0; i < CryptoPP::DES_EDE3::DEFAULT_KEYLENGTH; i++)
			{
				key[i] = _key.GetData().at(i);
			}
			for (int i = 0; i < CryptoPP::DES_EDE3::BLOCKSIZE; i++)
			{
				iv[i] = _iv.GetData().at(i);
			}
		}
		std::cout << "encrypting..";

		_ciphertext.GetData().resize(plaintext.GetData().size() + CryptoPP::DES_EDE3::BLOCKSIZE);
		CryptoPP::ArraySink buf(&_ciphertext.GetData()[0], _ciphertext.GetData().size());
		switch (cipher_mode)
		{
		case 1:
		{
			CryptoPP::ECB_Mode<CryptoPP::DES_EDE3>::Encryption enc(key, sizeof(key));
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 2:
		{
			CryptoPP::CBC_Mode<CryptoPP::DES_EDE3>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 3:
		{
			CryptoPP::CFB_Mode<CryptoPP::DES_EDE3>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 4:
		{
			CryptoPP::OFB_Mode<CryptoPP::DES_EDE3>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 5:
		{
			CryptoPP::CTR_Mode<CryptoPP::DES_EDE3>::Encryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(plaintext.GetData().data(), plaintext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		}
		_ciphertext.GetData().resize(buf.TotalPutLength());
		std::cout << "encryption ok" << std::endl << "Name to save: ";
		std::cin >> input_file;
		_ciphertext.Write(input_file);
		system("cls");
	}

	void Decrypt(int cipher_mode)
	{
		byte key[CryptoPP::DES_EDE3::DEFAULT_KEYLENGTH];
		byte iv[CryptoPP::DES_EDE3::BLOCKSIZE];
		std::cout << "File name to dencrypt: ";
		std::cin >> input_file;
		if (!_ciphertext.Open(input_file))
		{
			std::cout << "error\n";
			return;
		}
		system("cls");
		if (!_key.Open("key.txt"))
		{
			std::cout << "error\n";
			return;
		}
		if (!_iv.Open("iv.txt"))
		{
			std::cout << "error\n";
			return;
		}
		for (int i = 0; i < CryptoPP::DES_EDE3::DEFAULT_KEYLENGTH; i++)
		{
			key[i] = _key.GetData().at(i);
		}
		for (int i = 0; i < CryptoPP::DES_EDE3::BLOCKSIZE; i++)
		{
			iv[i] = _iv.GetData().at(i);
		}
		plaintext.GetData().resize(_ciphertext.GetData().size() + CryptoPP::DES_EDE3::BLOCKSIZE);
		CryptoPP::ArraySink buf(&plaintext.GetData()[0], plaintext.GetData().size());

		std::cout << "decryption..";

		switch (cipher_mode)
		{
		case 1:
		{
			CryptoPP::ECB_Mode<CryptoPP::DES_EDE3>::Decryption dec(key, sizeof(key));
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(dec, new CryptoPP::Redirector(buf)));
			break;
		}
		case 2:
		{
			CryptoPP::CBC_Mode<CryptoPP::DES_EDE3>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 3:
		{
			CryptoPP::CFB_Mode<CryptoPP::DES_EDE3>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 4:
		{
			CryptoPP::OFB_Mode<CryptoPP::DES_EDE3>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		case 5:
		{
			CryptoPP::CTR_Mode<CryptoPP::DES_EDE3>::Decryption enc(key, sizeof(key), iv);
			CryptoPP::ArraySource(_ciphertext.GetData().data(), _ciphertext.GetData().size(), true,
				new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::Redirector(buf)));
			break;
		}
		}
		plaintext.GetData().resize(buf.TotalPutLength());
		std::cout << "decryption ok" << std::endl << "Name to save:";
		std::cin >> input_file;
		system("cls");
		plaintext.Write(input_file);
		std::cout << "ok\n";
	}

};

void main()
{
	DES des;
	DES_EDE3 des3;
	int crypt;
	int action;
	int mode;
	std::cout << "1. DES\n";
	std::cout << "2. DES 3\n";
	std::cin >> crypt;
	
	std::cout << "1. Encrypt\n";
	std::cout << "2. Decrypt\n";
	std::cin >> action;
	system("cls");
	std::cout << "Choose block cipher mode:\n";
	std::cout << "1. Electronic code book (ECB)\n";
	std::cout << "2. Cipher block chaining (CBC)\n";
	std::cout << "3. Cipher feed back (CFB)\n";
	std::cout << "4. Output feed back (OFB)\n";
	std::cout << "5. Counter mode (CTR)\n";
	std::cin >> mode;
	system("cls");
	if (crypt == 1)
	{
		
		if (action == 1)
		{
			if (mode >= 1 && mode <= 5)
			{
				des.Encrypt(mode);
			}
			else
			{
				std::cout << "error\n";
			}
		}
		else if (action == 2)
		{
			if (mode >= 1 && mode <= 5)
			{
				des.Decrypt(mode);
			}
			else
			{
				std::cout << "error\n";
			}
		}
	}
	else if (crypt == 2)
	{
		
		if (action == 1)
		{
			if (mode >= 1 && mode <= 5)
			{
				des3.Encrypt(mode);
			}
			else
			{
				std::cout << "error\n";
			}
		}
		else if (action == 2)
		{
			if (mode >= 1 && mode <= 5)
			{
				des3.Decrypt(mode);
			}
			else
			{
				std::cout << "error\n";
			}
		}
	}
	
	system("pause");
}