#pragma once
#include <Windows.h>
#include <string>

using namespace std;

class Quantum;

/** Node of the AVL tree, contains Type which has its key information inside*/
template < class Type >
struct Node
{
	Node < Type > *left, *right;
	/**Heights of left and right subtrees*/
	char left_height, right_height;
	/**Pointer to data member - ArgData or Quantum*/
	Type *data;
	/** Constructor, receives and copies input, do not delete it manually!
	\param	input	-	pointer to information to store
	*/
	Node(Type *input);
	~Node();
};

template < class Type >
class AVLTree
{
	/**Compares two keys received from Nodes
	\param		st, nd - links to the keys to compare
	\return		0 - equal, 1 - if st > nd, -1 - if st < nd 
	*/
	char _compare(TKeyData& st, TKeyData& nd);
	/** Root of the tree*/
	Node < Type > *_root;
	/**Inserts a new element to the tree (information in _insert_tmp)
	\param previous		-	pointer to the parent node, used for rotates
	\return					result code (0)	
	*/
	char _balanced_insert(Node<Type> **previous);
	/**Temporary buffer for information, copies the pointer (do not delete it manually!)*/
	Type *_insert_tmp;
	/** Right-to-left balancing rotation
	\param		current		-	pointer to the current root node of the subtree
	\param		parent		-	parent node of the current	
	*/
	void _RL_Rotate(Node<Type>* current, Node<Type>**parent);
	/** Right-to-left balancing rotation
	\param		current		-	pointer to the current root node of the subtree
	\param		parent		-	parent node of the current
	*/
	void _R_Rotate(Node<Type>*current, Node<Type>**parent);
	/** Right-to-left balancing rotation
	\param		current		-	pointer to the current root node of the subtree
	\param		parent		-	parent node of the current
	*/
	void _L_Rotate(Node<Type>*current, Node<Type>**parent);
	/** Right-to-left balancing rotation
	\param		current		-	pointer to the current root node of the subtree
	\param		parent		-	parent node of the current
	*/
	void _LR_Rotate(Node<Type>*current, Node<Type>**parent);
	/**Deletes Values and Variables from Node::data
	\param	tmp  -  current node processed
	*/
	void deleteValVar(Node < Type > * tmp);
public:
	AVLTree();
	/** Inserts a new node containing data
	\param	data	-	pointer to the data to insert (copies the pointer, do not delete manually!)
	\return				result code	
	*/
	char insert(Type *data);
	/** Finds a node according to key
	\param	key		-	pointer to the key of data to find
	\return				NULL - not found, otherwise - actual pointer to data
	*/
	Type* find(TKeyData *key);
	Type* find(Quantum*, short);
	Type* find(string *, short);
	Type* findName(TKeyData *key);
	~AVLTree();
};

template < class Type >
Node<Type>::Node(Type *input)
{
	left = right = NULL;
	data = input;
	left_height = right_height = 0;
}

template <class Type>
Node<Type>::~Node()
{
	if (left)
		delete left;
	if (right)
		delete right;
	if (data)
		delete data;
}

template < class Type >
AVLTree<Type>::AVLTree()
{
	_root = NULL;
	_insert_tmp = NULL;
}

template < class Type >
AVLTree<Type>::~AVLTree()
{
	if (_root)
		deleteValVar(_root);
	if (_root)
		delete _root;
}

template < class Type >
void AVLTree<Type>::deleteValVar(Node < Type > * tmp)
{
	///if Quantum - skip
	if (!isQ(tmp->data))
		return;
	///Maintains correct erase of Function
	kostyl(tmp->data);

	if (tmp->left)
		deleteValVar(tmp->left);
	if (tmp->right)
		deleteValVar(tmp->right);
}

template < class Type >
Type* AVLTree<Type>::find(TKeyData *key)
{
	Node < Type > *current = _root;

	while (TRUE)
	{
		if (current == NULL) return NULL;

		TKeyData *tmp = current->data->GetName();
		char c = _compare(*tmp, *key);
		delete tmp;

		if (c == 0)
			return current->data;
		else
			if (c > 0)
				current = current->left;
			else
				current = current->right;		
	}
}

template < class Type >
Type* AVLTree<Type>::findName(TKeyData *key)
{
	Node < Type > *current = _root;

	while (TRUE)
	{
		if (current == NULL) return NULL;

		TKeyData *tmp = current->data->GetName();
		tmp->pop_back();
		tmp->pop_back();
		char c = _compare(*tmp, *key);
		delete tmp;

		if (c == 0)
			return current->data;
		else
			if (c > 0)
				current = current->left;
			else
				current = current->right;
	}
}

template < class Type >
Type* AVLTree < Type >::find(Quantum* t, short num)
{
	TKeyData s = t->GetName();
	s.push_back((char) (num / 255 + 1));
	s.push_back((char) (num % 255 + 1));
	Type* tmp = find(&s);
	/*if (!tmp && num == 1)
	{
		s.pop_back();
		s.push_back((char) 1);
		tmp = find(s);
	}*/
	return tmp;
}

template < class Type >
Type* AVLTree < Type >::find(string* name, short num)
{
	name->push_back((char) (num / 255 + 1));
	name->push_back((char) (num % 255 + 1));
	Type* tmp = find(name);
	name->pop_back();
	name->pop_back();
	return tmp;
}

template < class Type >
char AVLTree<Type>::insert(Type *data)
{
	///First insert
	if (_root == NULL)
	{
		_root = new Node<Type>(data);
		return 0;
	}
	///Others insert
	else
	{
		_insert_tmp = data;
		return _balanced_insert(&(_root));
	}
}

template < class Type >
char AVLTree<Type>::_balanced_insert(Node<Type> **previous)
{
	Node<Type> *current = *previous;
	
	/**Receive the current and target names, compare them*/
	TKeyData *tmp1 = current->data->GetName();
	TKeyData *tmp2 = _insert_tmp->GetName();
	char c = _compare(*tmp1, *tmp2);
	delete tmp1;
	delete tmp2;

	///Exchanges the elements
	if (c == 0)
	{
		change(current->data, _insert_tmp);
		delete _insert_tmp;
	}

	///Insert as a leaf
	if (c > 0 && current->left == NULL)
	{
		current->left = new Node<Type>(_insert_tmp);
		current->left_height++;
	}
	else 
		///Insert as a leaf
		if (c < 0 && current->right == NULL)
		{
			current->right = new Node<Type>(_insert_tmp);
			current->right_height++;
		}
		else
			///Recursive insert
			if (c > 0 && current->left != NULL)
				current->left_height = _balanced_insert(&(current->left)) + 1;
			///Recursive insert
			else
				if (c < 0 && current->right != NULL)
					current->right_height = _balanced_insert(&(current->right)) + 1;
		
	///difference in heights - used for rotation
	c = current->left_height - current->right_height;
	char tmp;
	if (c == -2)
	{
		tmp = (current->right)->left_height - (_root->right)->right_height;

		if (tmp == 1)
			_RL_Rotate(current, previous);
		else
			_L_Rotate(current, previous);		
	}
	else
		if (c == 2)
		{
			char tmp = (current->left)->left_height - (current->left)->right_height;
		
			if (tmp == -1)
				_LR_Rotate(current, previous);
			else
				_R_Rotate(current, previous);
		}
	return max(current->left_height, current->right_height);

}

template <class Type>
void AVLTree<Type>::_L_Rotate(Node<Type> *current, Node<Type> **previous)
{
	*previous = current->right;

	current->right = (*previous)->left;
	(*previous)->left = current;

	current->right_height = (*previous)->left_height;
	(*previous)->left_height = max(current->left_height, current->right_height) + 1;

	return;
}

template <class Type>
void AVLTree<Type>::_R_Rotate(Node<Type> *current, Node<Type> **previous)
{
	*previous = current->left;
	current->left = (*previous)->right;
	(*previous)->right = current;

	current->left_height = (*previous)->right_height;
	(*previous)->right_height = max(current->left_height, current->right_height) + 1;

	return;

}

template <class Type>
void AVLTree<Type>::_RL_Rotate(Node<Type> *current, Node<Type> **previous)
{
	_R_Rotate(current->right, &(current->right));
	_L_Rotate(current, previous);
	return;
}

template <class Type>
void AVLTree<Type>::_LR_Rotate(Node<Type> *current, Node<Type> **previous)
{
	_L_Rotate(current->left, &(current->left));
	_R_Rotate(current, previous);
	return;
}

template <class Type>
char AVLTree<Type>::_compare(TKeyData& first, TKeyData& second)
{
	if (first < second)
		return -1;
	else
		if (first > second)
			return 1;
		else
			return 0;
}