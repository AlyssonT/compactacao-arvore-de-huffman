#include "Huffman.h"
#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;

int main(int argc, char *argv[]){
    if(argv[1][0]=='c'){            //compactaçao
        MyVec<char> arq;
        unsigned char *arquivo;
        streampos tam;

        ifstream in(argv[2], ios::binary | ios::ate);   //abre o arquivo ja com o seekg no end
        tam = (unsigned)in.tellg();     //pega o tamanho do arquivo
        if(tam==0){                 //arquivo vazio
            ofstream out(argv[3]);
            return 0;
        }
        arquivo=new unsigned char[(unsigned int)tam+1u];    //vetor que guardará os dados
        in.seekg(0,ios::beg);
        in.read((char*)arquivo,(unsigned int)tam);      //le o arquivo
        in.close();

        int freq[256]={0};
        for(long long i=0;i<tam;i++){   //verifica a frequencia de cada caracter
            arq.push_back(arquivo[i]);
            freq[arquivo[i]]++;
        }
        delete []arquivo;

        HuffManTree tree(freq);         //gera a arvore de Huffman
        MyVec<bool> comprimido;
        tree.comprimir(comprimido, arq);        //comprime o arquivo para um MyVec<bool>
        unsigned int v[comprimido.size()/(8*sizeof(int))+1]={0};
        int ignore[1];
        ignore[0]=(comprimido.size()/(8*sizeof(int))+1)*(8*sizeof(int)) - comprimido.size();    //quantos bits devemos ignorar no final do arquivo

        int cont=sizeof(int)*8-1;
        for(long long i=0, j=0;i<comprimido.size();i++){    //passa os dados do MyVec<bool> para um vetor de int com operaçoes de bits
            if(comprimido[i]){
                v[j] |= (1 << cont);
            }
            cont--;
            if(cont==-1){
                cont=sizeof(int)*8-1;
                j++;
            }
        }

        ofstream out(argv[3], ios::binary);
        out.write((char*)ignore,sizeof(int));   //escreve quanto devemos ignorar no final
        out.write((char*)freq,sizeof(int)*256);     //a freq de cada caracter
        out.write((char*)v,sizeof(int)*(comprimido.size()/32+1));       //o arquivo compactador em si
        out.close();
    }
    else if(argv[1][0]=='d'){                   //descompactaçao
        int freq[256]={0};
        int ignore[1]={0};
        ifstream in(argv[2], ios::binary);
        in.seekg(0,ios::end);
        streampos tam=in.tellg();
        if(tam==0){                     //arquivo vazio
            ofstream out(argv[3]);
            return 0;
        }
        in.seekg(0,ios::beg);
        in.read((char*)ignore,sizeof(int));     //le o quanto devemos ignorar
        in.read((char*)freq,256*sizeof(int));   //le as frequencias
        
        HuffManTree tree(freq);         //cria a arvore de Huffman
        MyVec<bool> comprimido;
        unsigned int *arquivo;

        unsigned int sizeV = ((unsigned int)tam-257*sizeof(int))/sizeof(int);
        arquivo=new unsigned int[sizeV];
        in.read((char*)arquivo,(unsigned int)tam-sizeof(int)*257);  //le o arquivo compactado

        int cont=sizeof(int)*8-1;
        unsigned int k=0;
        while(k<sizeV){                         //transfere os dados do vetor de int para o MyVec<bool>
            if((1<<cont) & arquivo[k]){
                comprimido.push_back(true);
            }
            else{
                comprimido.push_back(false);
            }
            cont--;
            if(cont==-1){
                cont=sizeof(int)*8-1;
                k++;
            }
        }
        delete []arquivo;
        for(int i=0;i<ignore[0];i++) comprimido.pop_back();     //elimina bits extras escritos na compressão

        MyVec<char> saida;
        tree.descomprimir(saida,comprimido);        //descomprime
        ofstream arq_descomprimido(argv[3]);
        for(long long i=0;i<saida.size();i++){      //salva no arquivo de saida o arquivo original
            arq_descomprimido << saida[i];
        }
    }
    return 0;
}