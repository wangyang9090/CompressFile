#pragma once

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

template <class W>
struct HuffmanTreeNode {

	W _w;//权值 次数
	HuffmanTreeNode<W>* _left;//左孩子
	HuffmanTreeNode<W>* _right;//右孩子
	HuffmanTreeNode<W>* _parent;//父亲

	HuffmanTreeNode(const W& w)
		:_left(NULL)
		, _right(NULL)
		, _w(w)
		, _parent(NULL)
	{}

};

//构造哈夫曼树
template <class W>
class HuffmanTree {

	typedef HuffmanTreeNode<W> Node;

	struct PtrNodeCompare{

		bool operator()(Node* left, Node* right){
			return left->_w > right->_w;
		}
	};

public:
	HuffmanTree()
		:_root(NULL)
	{}

	HuffmanTree(W* w, size_t n, const W& invalid){

		priority_queue <Node*, vector<Node*>, PtrNodeCompare> minheap;
		for (size_t i = 0; i < n; ++i){
			if (w[i] != invalid){
				minheap.push(new Node(w[i]));
			}
		}

		if (minheap.size() == 0){

			cout << "无效文件" << endl;
			exit(0);
		}
		else if (minheap.size() == 1){
			Node* right = minheap.top();
			_root = new Node(right->_w);
			right->_parent = _root;
			_root->_right = right;
		}
		else{
			while (minheap.size() > 1){

				Node* left = minheap.top();
				minheap.pop();

				Node* right = minheap.top();
				minheap.pop();

				Node* parent = new Node(left->_w + right->_w);

				parent->_left = left;
				parent->_right = right;

				left->_parent = parent;
				right->_parent = parent;

				minheap.push(parent);
			}

			_root = minheap.top();
		}
	}

	Node* GetRoot(){
		return _root;
	}

	~HuffmanTree(){
		Destory(_root);
		_root = NULL;
	}

	void Destory(Node* root){
		if (root == NULL){
			return;
		}
		Destory(root->_left);
		Destory(root->_right);
		delete root;
	}

private:
	HuffmanTree(const HuffmanTree<W>& t); 
	HuffmanTree<W>& operator= (const HuffmanTree<W>& t); 

protected:
	Node* _root;
};

