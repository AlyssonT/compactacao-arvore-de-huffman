#include "Huffman.h"

bool Tree::operator>(const Tree& other) const{  //sobrecarga do operador >
    if(raiz->freq < other.raiz->freq){          //aqui, ele retorna ao contrario, se for menor -> true, para que fiquem ordenadas 
        return true;                            //de modo que a menor frequencia fica na frente da fila
    }
    return false;
}

void Tree::deleteNodos(Nodo *r){    //função auxiliar do destrutor recursiva
    if(!r) return;                  //se nao existe raiz retorna
    deleteNodos(r->dir);            //deleta arvore da direita
    deleteNodos(r->esq);            //deleta arvore da esquerda
    delete r;                       //deleta a raiz principal
}

Tree::~Tree(){              //destrutor
    deleteNodos(raiz);
}

Nodo * Tree::copyNodos(Nodo *raiz) const{       //funcao auxiliar para o operador de atribuição
    if(!raiz) return NULL;                      //se nao tem raiz, retorna NULL
    Nodo *ans=new Nodo(raiz->freq, raiz->c);    
    if(raiz->vazio) ans->vazio=true;
    ans->dir = copyNodos(raiz->dir);            //copia a arvore da direita
    ans->esq = copyNodos(raiz->esq);            //copia a arvore da esquerda

    return ans;
}

Tree& Tree::operator=(const Tree &other){
    if(this==&other) return *this;              //testar autoatribuição
    deleteNodos(raiz);                          //apaga a atual
    raiz = copyNodos(other.raiz);               //copia other
    return *this;
}

Tree::Tree(const Tree &other){      //construtor de cópia
    raiz=NULL;
    *this=other;
}

void Tree::merge(Tree& other){                                      //função que junta 2 arvores em uma com a soma das frequencias
    Nodo *topo = new Nodo(raiz->freq + other.raiz->freq, true);     //cria o nodo do novo topo da arvore
    if(raiz->freq > other.raiz->freq){                          //posiciona as arvores de acordo com sua frequencia
        topo->dir = raiz;
        topo->esq = other.raiz;
    }
    else if(raiz->freq < other.raiz->freq){
        topo->dir=other.raiz;
        topo->esq=raiz;
    }
    else{
        if((raiz->vazio && !other.raiz->vazio) || (raiz->vazio && other.raiz->vazio) || (!raiz->vazio && !other.raiz->vazio)){
            topo->dir = raiz;
            topo->esq = other.raiz;
        }
        else {
            topo->dir=other.raiz;
            topo->esq=raiz;
        }
    }
    other.raiz=NULL;                //para nao dar 2 frees na arvore "other"
    raiz=topo;                      //faz com que a nova arvore comece do novo topo
}

void Tree::imprime(Nodo *raiz) const{
    if(!raiz) return;
    if(raiz->vazio);
    else cout << raiz->c << endl;
    cout << "VAZIO? " << raiz->vazio << endl;
    imprime(raiz->esq);
    imprime(raiz->dir);
}
void Tree::print() const{
    imprime(raiz);
}

HuffManTree::HuffManTree(int freq[256]){        //construtor da arvore de Huffman
    for(int i=0;i<256;i++){
        if(freq[i]!=0){
            Tree arv(freq[i], (unsigned char)i);        //cria uma arvore de um nodo para cada caracter e insere numa lista de prioridade
            lista.push(arv);
        }
    }

    Tree junta1,junta2;
    while(lista.size() > 1){        //junta todas as arvores criando apenas uma, com todos os caracteres
        junta1=Tree(lista.top());
        lista.pop();
        junta2=Tree(lista.top());
        lista.pop();
        junta1.merge(junta2);
        lista.push(junta1);
    }

    ans=lista.top();

    Tree aux(1,'\n');          //caso há apenas um caracter no arquivo, inserir um nodo com '\n' na arvore
    if(!ans.raiz->vazio){
        ans.merge(aux);
    }
}

void Tree::defineCodigoRecursiva(Nodo *raiz, MyVec<bool> codigos[], MyVec<bool> &aux) const{    //define o codigo para cada caracter na hora de compactar
    if(!raiz->vazio){                                                                           //guardando num vetor de MyVec<bool> (formando uma matriz)
        codigos[(unsigned char)raiz->c]=aux;        //percorre na arvore recursivamente e atribui os codigo na matriz
        aux.pop_back();
        return;
    }
    aux.push_back(true);
    defineCodigoRecursiva(raiz->esq,codigos,aux);
    aux.push_back(false);
    defineCodigoRecursiva(raiz->dir,codigos,aux);
    aux.pop_back();
    return;
}

void HuffManTree::defineCodigo(MyVec<bool> codigos[], MyVec<bool> &aux) const{
    ans.defineCodigoRecursiva(ans.raiz, codigos, aux);
}

void HuffManTree::comprimir(MyVec<bool> &out, const MyVec<char> &in) const{
    MyVec<bool> aux;
    MyVec<bool> codigos[256];
    aux.push_back(true);
    defineCodigo(codigos,aux);  //define os codigos de cada caracter
    for(int i=0;i<in.size();i++){
        for(int j=1;j<codigos[(unsigned char)in[i]].size();j++){    //para cada caracter do arquivo, insere no vetor de saida sua respectiva representação na arvore
           out.push_back(codigos[(unsigned char)in[i]][j]);
        }
    }
}

void HuffManTree::descomprimir(MyVec<char> &out, const MyVec<bool> &in) const{
    Nodo *aux=ans.raiz;
    if(!aux->vazio){            //teste para apenas um caracter
        out.push_back(aux->c);
        return;
    }
    for(int i=0;i<in.size();i++){       //anda na arvore e escreve o caracter no vetor de saida a partir de sua representação na arvore
        if(in[i]){
            aux=aux->esq;
            if(!aux->vazio){
                out.push_back(aux->c);
                aux=ans.raiz;
            }
        }
        else{
            aux=aux->dir;
            if(!aux->vazio){
                out.push_back(aux->c);
                aux=ans.raiz;
            }
        }
    }
}