#include <algorithm>
#include <iostream>
#include "BplusTree.h"
using namespace std;
const int capacity = 20;

Node::Node(){
    key = new int[capacity];
    ptr = new Node*[capacity+1];
}

Node* BplusTree::getRoot(){
    return this->root;
}

void BplusTree::insertar(int data){
    // Si root es null, crear nodo y terminar.
    if(root == nullptr){
        root = new Node();
        root->key[0] = data;
        root->isLeaf = true;
        root->size = 1;
    }

    // Si no, localizar la insercion
    else{
        Node* temp = root;
        Node* parent;

        // Hasta llegar a un nodo hoja.
        // Las inserciones siempre se realizan en nodos hoja.
        while(temp->isLeaf == false){
            parent = temp;

            for(int i = 0 ; i < temp->size ; i++){

                // Si encontramos la posicion donde insertar
                if(data < temp->key[i]){
                    temp = temp->ptr[i];
                    break;
                }

                if(i == temp->size-1){
                    temp = temp->ptr[i+1];
                    break;
                }
            }
        }

        if(temp->size < capacity) {  // Hay espacio para insertar
            int i = 0;
            // localizar el indice donde insertar
            while(data > temp->key[i] && i < temp->size) i++;

            // si se inserta al medio, correr los mayores a la derecha.
            for(int x = temp->size; x > i ; x--){
                temp->key[x] = temp->key[x-1];
            }

            // actualizar posicion con el nuevo dato.
            temp->key[i] = data;
            temp->size++;

            temp->ptr[temp->size] = temp->ptr[temp->size-1];
            temp->ptr[temp->size-1] = nullptr;
        }

        else{
            Node* nuevo = new Node;

            int auxNode[capacity+1];

            // lleno el nodo auxiliar con los valores del nodo a separar
            for(int i = 0 ; i < capacity; i++){
                auxNode[i] = temp->key[i];
            }
            int i = 0;
            int x = 0;

            while(data > auxNode[i] && i < capacity) i++;

            for(int x = capacity + 1 ; x > i ; x--){
                auxNode[x] = auxNode[x-1];
            }

            auxNode[i] = data;
            nuevo->isLeaf = true;

            temp->size = (capacity+1)/2;
            nuevo->size = capacity + 1 - (capacity+1)/2;
            // (capacity+1)/2 -> 21/2            = 10
            // capacity+1 - (capacity+1)/2 -> 21 - 10 = 11
            // temp -> size > nuevo -> size

            temp->ptr[temp->size] = nuevo;
            nuevo->ptr[nuevo->size] = temp->ptr[capacity];
            temp->ptr[capacity] = nullptr;

            // actualizar nodo original con primera parte de los valores
            for(i = 0 ; i < temp->size ; i++){
                temp->key[i] = auxNode[i];
            }

            // actualizar nuevo nodo con segunda parte de los valores
            for(i = 0, x = temp->size ; i < nuevo->size; i++, x++){
                nuevo->key[i] = auxNode[x];
            }

            // Si es que el nodo separado es el root,
            // Debemos actualizar root.
            if(temp == root){
                Node* updatedRoot = new Node;

                updatedRoot->key[0] = nuevo->key[0];
                updatedRoot->ptr[0] = temp;
                updatedRoot->ptr[1] = nuevo;
                updatedRoot->isLeaf = false;
                updatedRoot->size = 1;
                root = updatedRoot;
            }

            // Si no es, llamar a funcion para insertar nodo interno.
            else{
                insertInternalNode(nuevo->key[0], parent, nuevo);
            }
        }
    }
}

void BplusTree::insertInternalNode(int data, Node* temp, Node* child){

    // No overflow
    if(temp->size < capacity){
        int i = 0;

        while(data > temp->key[i] && i < temp->size) i++;

        for(int j = temp->size; j > i ; j--){
            temp->key[j] = temp->key[j-1];
        }

        for(int j = temp->size + 1; j > i + 1 ; j--){
            temp->ptr[j] = temp->ptr[j-1];
        }

        temp->key[i] = data;
        temp->size++;
        temp->ptr[i+1] = child;
    }

    // Overflow -> split
    else{

        // Crear nuevo nodo interno.
        Node* nuevoInterno = new Node;
        int auxKeys[capacity+1];
        Node* auxPtrs[capacity+2];

        // Guardar lista de datos en auxKeys.
        for(int i = 0; i < capacity; i++){
            auxKeys[i] = temp->key[i];
        }

        // Guardar punteros del nodo en auxPtrs.
        for(int i = 0 ; i < capacity + 1; i++){
            auxPtrs[i] = temp->ptr[i];
        }

        int i = 0;

        // Localizar donde insertar nuevo nodo.
        while( data > auxKeys[i] && i < capacity) i++;

        for(int x = capacity + 1; x > i ; x--){
            auxKeys[x] = auxKeys[x-1];
        }

        auxKeys[i] = data;

        for(int x = capacity + 2; x > i+1 ; x--){
            auxPtrs[x] = auxPtrs[x-1];
        }

        auxPtrs[i+1] = child;
        nuevoInterno->isLeaf = false;

        temp->size = (capacity + 1)/2;
        nuevoInterno->size = capacity - (capacity + 1)/2;

        int x;
        for(i = 0, x = temp->size+1 ; i < nuevoInterno->size; i++,x++){
            nuevoInterno->key[i] = auxKeys[x];
        }

        for(i = 0, x = temp->size+1 ; i < nuevoInterno->size + 1; i++, x++){
            nuevoInterno->ptr[i] = auxPtrs[x];
        }

        if(temp==root){
            Node* updatedRoot = new Node;

            updatedRoot->key[0] = temp->key[temp->size];

            updatedRoot->ptr[0] = temp;
            updatedRoot->ptr[1] = nuevoInterno;
            updatedRoot->isLeaf = false;
            updatedRoot->size = 1;
            root = updatedRoot;
        }
        else{
            insertInternalNode(temp->key[temp->size], getParent(root,temp), nuevoInterno);
        }

    }
}

Node* BplusTree::getParent(Node* pointer, Node* child){
    Node* parent;

    if( pointer->isLeaf || (pointer->ptr[0])->isLeaf) return nullptr;

    for(int i = 0 ; i < pointer->size + 1 ; i++){
        
        if(pointer->ptr[i] == child){
            parent = pointer;
            return parent;
        }
        else{
            parent = getParent(pointer->ptr[i], child);

            if(parent != nullptr) return parent;
        }
    }

    return parent;
}

BplusTree::BplusTree(){
    //int m = 21;
    root = nullptr;
    //this->order = m;
    //capacityKeys = order-1;
}

BplusTree::~BplusTree(){

}

void BplusTree::search(int x)
{

	// If tree is empty
	if (root == NULL) {
		cout << "Tree is empty\n";
	}

	// Traverse to find the value
	else {

		Node* cursor = root;

		// Till we reach leaf node
		while (cursor->isLeaf == false) {

			for (int i = 0;
				i < cursor->size; i++) {

				// If the element to be
				// found is not present
				if (x < cursor->key[i]) {
					cursor = cursor->ptr[i];
					break;
				}

				// If reaches end of the
				// cursor node
				if (i == cursor->size - 1) {
					cursor = cursor->ptr[i + 1];
					break;
				}
			}
		}

		// Traverse the cursor and find
		// the node with value x
		for (int i = 0;
			i < cursor->size; i++) {

			// If found then return
			if (cursor->key[i] == x) {
				cout << "Found\n";
				return;
			}
		}

		// Else element is not present
		cout << "Not found\n";
	}
}