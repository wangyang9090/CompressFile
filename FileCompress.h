#pragma once

#include <string>
#include <stdio.h>
#include <windows.h>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include "HuffmanTree.h"
#pragma warning (disable: 4996)
using namespace std;


typedef long long LongType;

struct CharInfo{

	char _ASCII; //字符
	LongType _count;//次数
	string _code;//权值   

	CharInfo operator+(const CharInfo& info){
		CharInfo ret;
		ret._count = _count + info._count;
		return ret;
	}

	bool operator>(const CharInfo& info){
		return _count > info._count;
	}

	bool operator<(const CharInfo& info){
		return _count < info._count;
	}

	bool operator!=(const CharInfo& info){
		return _count != info._count;
	}

	bool operator==(const CharInfo& info){
		return _count == info._count;
	}

};



class FileCompress{

	typedef HuffmanTreeNode<CharInfo> Node;

	struct TmpInfo{
		char _ch; //字符
		LongType _num;//数量
	};

public:
	FileCompress(){
		for (size_t i = 0; i < 256; ++i){
			_infos[i]._ASCII = i;
			_infos[i]._count = 0;
		}
	}

	//1.统计字符出现的次数
	void Compress(const char* file){

		FILE* f_out = fopen(file, "r");
		assert(f_out);

		char character = fgetc(f_out);

		while (character != EOF){
			_infos[(unsigned char)character]._count++; 
			character = fgetc(f_out);
		}

		//2.构建huffman tree

		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);

		//3.生成code
		GenerateHuffmanCode(tree.GetRoot());

		//4.压缩
		string compressFile = file;  
		compressFile += ".huffman";

		FILE* f_in = fopen(compressFile.c_str(), "wb");
		
		for (size_t i = 0; i < 256; ++i){
			if (_infos[i]._count > 0){
				TmpInfo tmpinfo;
				tmpinfo._ch = _infos[i]._ASCII; //字符
				tmpinfo._num = _infos[i]._count;//次数

				//解压时需要用其生成Huffman树 
				fwrite(&tmpinfo, sizeof(TmpInfo), 1, f_in);
			}
		}

		TmpInfo tmpinfo;
		tmpinfo._num = -1;
		fwrite(&tmpinfo, sizeof(TmpInfo), 1, f_in);
		rewind(f_out);

		character = fgetc(f_out); 
		size_t CeffectiveBit = 0;
		char value = 0;
		int pos = 0;

		while (character != EOF){

			string& code = _infos[(unsigned char)character]._code;
			for (size_t i = 0; i < code.size(); ++i){

				if (code[i] == '0'){
					value &= ~(1 << pos);
				}
				else if (code[i] == '1'){
					value |= (1 << pos);
				}
				else{
					assert(false);
				}

				++pos;

				if (pos == 8){
					fputc(value, f_in);
					value = 0;
					pos = 0;
				}
				++CeffectiveBit;

			}
			character = fgetc(f_out); 
		}
		cout << "压缩Huffman编码长度:" << CeffectiveBit<< endl;

		if (pos > 0){
			fputc(value, f_in);
		}

		fclose(f_out);
		fclose(f_in);
	}


	void GenerateHuffmanCode(Node* root){
		if (root == NULL){
			return;
		}
		if (root->_left == NULL && root->_right == NULL)  {

			string code; 
			Node* cur = root;
			Node* parent = cur->_parent;

			while (parent) {
				if (cur == parent->_left){
					code.push_back('0');
				}
				else{
					code.push_back('1');
				}
				cur = parent;
				parent = cur->_parent;
			}

			reverse(code.begin(), code.end());

			_infos[(unsigned char)root->_w._ASCII]._code = code;
			return;
		}
		GenerateHuffmanCode(root->_left);
		GenerateHuffmanCode(root->_right);
	}

	/**************************   解压缩   *******************************/

	void Uncompress(const char* file){
		assert(file);

		string uncompressFile = file;
		size_t pos = uncompressFile.rfind('.');    
		assert(pos != string::npos);

		uncompressFile.erase(pos, uncompressFile.size() - pos);
		uncompressFile += ".unhuffman";//测试所用  分辨与源文件的区别

		FILE* fout = fopen(file, "rb");
		assert(fout);

		FILE* fin = fopen(uncompressFile.c_str(), "w");
		assert(fin);

		TmpInfo info;
		fread(&info, sizeof(TmpInfo), 1, fout);

		while (info._num != -1){
			_infos[(unsigned char)info._ch]._ASCII = info._ch;
			_infos[(unsigned char)info._ch]._count = info._num;
			fread(&info, sizeof(TmpInfo), 1, fout);
		}

		//1.重建huffman tree 
		size_t n = 0;
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);
		Node* root = tree.GetRoot();
		Node* cur = root;
		LongType fileSize = root->_w._count;
		cout <<"文件中ASCII个数:" <<fileSize << endl;

		//2.解压缩
		char value = fgetc(fout);
		while (1){
			for (size_t pos = 0; pos < 8; ++pos){

				if (value & (1 << pos)){
					cur = cur->_right;
				}
				else{
					cur = cur->_left;
				}

				++n;

				if (NULL == cur->_left && NULL == cur->_right){

					fputc(cur->_w._ASCII, fin); 
					cur = root;

					if (--fileSize == 0){
						break;
					}
				}

			}
			if (fileSize == 0){
				break;
			}
			value = fgetc(fout);
		}
		cout <<"解压Huffman编码长度:"<< n << endl;

		fclose(fout);
		fclose(fin);
	}




protected:

	CharInfo _infos[256];

};//类结束

