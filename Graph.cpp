#include <iostream>

using namespace std;

struct node
{
	int row;
	int col;
	node* next;

};

class linked_list
{

private:
	node *head, *tail;
public:
	linked_list()
	{
		head = NULL;
		tail = NULL;
	}

	
	void insert(int r, int c)
	{
		node* n = new node;
		n->row = r;
		n->col = c;
		n->next = NULL;
		if (head == NULL)
		{
			head = n;
			tail = n;
		}
		else {

			tail->next = n;
			tail = tail->next;
		}

	}


	void printList()
	{
		if (head == NULL)
		{
			cout << "" << endl;
		}
		else {

			node* n = head;
			while (n != NULL)
			{
				cout << "(" << n->row << ", " << n->col << ") ";
				n = n->next;

			}
			cout << "\n";
		}
	}

	int getRow()
	{
		if (head != NULL)
		{
			return head->row; 
		}
	}

	int getCol()
	{
		if (head != NULL)
		{
			return head->col;
		}
	}

	int getLength()
	{
		int n = 0;
		node* curr = head;
		while (curr != NULL)
		{
			n++;
			curr = curr->next;

		}

		return n;
	}

	node* getHead()
	{
		return head;
	}


};


