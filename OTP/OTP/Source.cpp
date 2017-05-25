// ConsoleApplication1.cpp: определяет точку входа для консольного приложения.
//


#include "iostream"
#include <vector>
#include <io.h>

using namespace std;

class MyFile
{
public:
	MyFile()
		: _file(NULL)
	{

	}

	bool Open(std::string &name)
	{
		string x = name;

		char * S = new char[x.length() + 1];
		std::strcpy(S, x.c_str());
		_file = fopen(S, "rb");

		if (_file != NULL)
		{
			fseek(_file, 0, SEEK_END);
			long size = ftell(_file);
			//check size

			_data.resize(size);
			fseek(_file, 0, SEEK_SET);
			fread(_data.data(), 1, size, _file);
		}

		return !_data.empty();
	}

	bool write(std::string &name)
	{

		/*fprintf(_file,r );*/
	}

	void Close() {};

	std::vector<char> &GetData() { return _data; }

private:
	std::vector<char> _data;
	FILE* _file;
};


int main()
{

	string x = "1.docx";

	MyFile plaintext;
	MyFile key;
	MyFile ciphertext;
	plaintext.Open(x);
	x = "2.docx";
	key.Open(x);
	x = "3.txt";
	ciphertext.Open(x);
	//plaintext.GetData().data;
	/*for (char n : plaintext.GetData()) {
	std::cout << n << '\n';
	}*/
	//if (!plaintext.Open("c:\\blabla"))
	//	return;

	////the same for key ...
	int i = 0;
	for (; i < plaintext.GetData().size(); i++)
	{
		ciphertext.GetData().push_back(plaintext.GetData().at(i) ^ key.GetData().at(i));
	}
	FILE * f;
	f = fopen("w.docx", "w");
	for (char n : ciphertext.GetData()) {
		/*std::cout << n << '\n';*/
		fprintf(f, "%c", n);

	}

	/*cout << "Hello word";
	int a;
	cin >> a;*/
	system("pause");
	return 0;
}