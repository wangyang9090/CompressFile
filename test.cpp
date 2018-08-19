#include "FileCompress.h"


void TestFilecompress()
{
	FileCompress fc;

	size_t start = GetTickCount();
	fc.Compress("Input.txt");
	size_t end = GetTickCount();
	cout << "UseTime:" << end - start << endl;

	fc.Uncompress("Input.txt.huffman");
}

int main(){

	TestFilecompress();

	system("pause");
	return 0;
}