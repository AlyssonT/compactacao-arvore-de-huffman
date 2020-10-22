#ifndef HUFFMAN_H
#define HUFFMAN_H
#include "MyVecNewIterator.h"
#include "MyPriorityQueue.h"
#include <iostream>
using namespace std;

class Nodo{
    public:
        Nodo(int f, unsigned char letra): freq(f), c(letra) {
            dir=NULL;
            esq=NULL;
            vazio=false;
        }
        Nodo(int f, bool a): freq(f), vazio(a){
            dir=NULL;
            esq=NULL;
        }
        int freq;
        bool vazio;
        unsigned char c;
        Nodo *dir, *esq;
};

class Tree{
    public:
        Tree() { raiz=NULL; }
        Tree(int f, unsigned char letra){ raiz=new Nodo(f,letra); }
        Tree(const Tree&);
        Tree & operator=(const Tree &);
        ~Tree();
        void deleteNodos(Nodo *);
        Nodo* copyNodos(Nodo*) const;
        void defineCodigoRecursiva(Nodo *, MyVec<bool> codigos[], MyVec<bool> &) const;
        void merge(Tree &);
        bool operator>(const Tree&) const;
        void print() const;
        void imprime(Nodo*) const;
        Nodo *raiz;
};

class HuffManTree{
    public:
        HuffManTree(int freqs[256]);

        void comprimir(MyVec<bool> &out, const MyVec<char> &in) const;
        void descomprimir(MyVec<char> &out, const MyVec<bool> &in) const;
        
    private:
        MyVec<bool> codigos[256];
        MyPriorityQueue<Tree> lista;
        Tree ans;
        void defineCodigo(MyVec<bool> codigos[], MyVec<bool> &) const;
};

#endif