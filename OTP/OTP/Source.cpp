// ConsoleApplication1.cpp: определяет точку входа для консольного приложения.
//
#include "iostream"

#include <conio.h>
#include <string>
#include <vector>
#include <io.h>



class MyFile
{
public:
	MyFile()
		: _file(NULL)
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

	bool write(std::string name)
	{
		const char *_name = name.c_str();
		_file = fopen(_name, "w+b");
		int flag = fwrite(_data.data(), 1, _data.size(), _file);
		fclose(_file);
		return !(flag == 0);
	}

	

	std::vector<char> &GetData() { return _data; }

private:
	std::vector<char> _data;
	FILE* _file;
};

class OTP
{
private:
	std::string input_file;
	std::string output_file;
	MyFile plaintext;
	MyFile key;
	MyFile ciphertext;
	bool flag = true;
	bool key_open()
	{
		if (!key.Open("key.txt"))
		{
			std::cout << "error\n";
			system("pause");
			return false;
		}
		return true;
		
	}
	

public:
	OTP()
		: input_file(""), output_file("")
	{
	}
	void Encrypt()
	{
		
		std::cout << "Filename:\n";
		std::cin >> input_file;
		if (!plaintext.Open(input_file))
		{
			std::cout << "error\n";
			system("pause");
			return;
		}
		system("cls");
		std::cout << "create key? \n1. yes \n2. no\n";
		int need_key;
		std::cin >> need_key;
		system("cls");
		
		switch (need_key)
		{
		case 1:
			std::cout << "creating key..\n";
			for (int i = 0; i < plaintext.GetData().size(); i++)
			{
				key.GetData().push_back((char)rand() % 256);
			}
			system("cls");
			if (!key.write("key.txt"))
			{
				std::cout << "error\n";
			}
			else
			{
				std::cout << "key created\n";
			}
			break;
		case 2:
			
			flag=key_open();
			if (!key.GetData().size() == plaintext.GetData().size())
			{
				std::cout << "error\n";
				system("pause");
				return;
			}
			break;
		default:
			std::cout << "unknown data\n";
			system("pause");
			return;
		}
			
		system("pause");
		system("cls");
		if (flag)
		{
			std::cout << "encrypting..\n";
			for (int i = 0; i < plaintext.GetData().size(); i++)
			{
				ciphertext.GetData().push_back(plaintext.GetData().at(i) ^ key.GetData().at(i));
			}
			system("cls");
			std::cout << "File encrypt\n";
			std::cout << "Name to save:\n";
			std::cin >> output_file;
			system("cls");
			if (!ciphertext.write(output_file))
			{
				std::cout << "error\n";
			}
			else
			{
				std::cout << "File saved \n";
			}
			system("pause");
		}
		
	}
	void Decrypt()
	{
		std::cout << "Filename:\n";
		std::cin >> input_file;
		if (!ciphertext.Open(input_file))
		{
			std::cout << "error\n";
			system("pause");
			return;
		}
		system("cls");
		/*std::cout << "Move a key for decrypting at Crypto_1/Crypto_1\n как key.txt";
		std::cout << "\nPress any key...";
		_getch();*/
		system("cls");
		/*if (!key.Open("key.txt"))
		{
			std::cout << "error";
			system("pause");
			return;
		}
		if (!key.GetData().size() == ciphertext.GetData().size())
		{
			std::cout << "error";
			system("pause");
			return;
		}*/
		flag=key_open();
		if (flag)
		{
			if (!key.GetData().size() == ciphertext.GetData().size())
			{
				std::cout << "error\n";
				system("pause");
				return;
			}
			for (int i = 0; i < ciphertext.GetData().size(); i++)
			{
				plaintext.GetData().push_back(ciphertext.GetData().at(i) ^ key.GetData().at(i));
			}
			std::cout << "File decrypted\n";
			std::cout << "Name to save:\n";
			std::cin >> output_file;
			system("cls");
			if (!plaintext.write(output_file))
			{
				std::cout << "error\n";
			}
			else
			{
				std::cout << "File saved ok\n";
			}
			system("pause");
		}

	}

};


void main()
{
	OTP MyOTP;
	std::cout << "1. Encrypt \n2. Decrypt \n";
	int mode;
	std::cin >> mode;
	system("cls");
	switch (mode)
	{
	case 1:
		MyOTP.Encrypt();
		break;
	case 2:
		MyOTP.Decrypt();
		break;
	default:
		std::cout << "unknown data\n";
		system("pause");
	}
	return;
}