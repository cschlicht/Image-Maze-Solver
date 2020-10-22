#include <iostream>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <typeinfo>
#include <tuple>
#include <vector>
#include <unordered_map> 
#include "Graph.cpp"
using namespace std;
using namespace cv;



struct Key
{
	int first;
	int second;


	bool operator==(const Key& other) const
	{
		return (first == other.first
			&& second == other.second);
	}

	
};

namespace std {

	template <>
	struct hash<Key>
	{
		std::size_t operator()(const Key& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<int>()(k.first)
				^ (hash<int>()(k.second) << 1)) >> 1);
		}
	};

	void printKey(const Key& k)
	{
		cout << "(" << k.first << ", " << k.second << ") ";
	}

}


vector<linked_list> addEdge(vector<linked_list> adj, int r1, int c1, int r2, int c2) //connects the two on the linked list
{
	for (int i = 0; i < adj.size(); i++)
	{
		if (adj[i].getRow() == r1 && adj[i].getCol() == c1)
		{
			adj[i].insert(r2, c2);
		}
		if (adj[i].getRow() == r2 && adj[i].getCol() == c2)
		{
			adj[i].insert(r1, c1);
		}
	}
	return adj;
}

void printAdj(vector<linked_list> adj)
{
	for (int i = 0; i < adj.size(); i++)
	{
		adj[i].printList();

	}



}

bool check(vector<linked_list> adj, int r, int c)
{
	for (int i = 0; i < adj.size(); i++)
	{
		if (adj[i].getRow() == r && adj[i].getCol() == c)
		{
			return true;
		}
	}

	return false;


}


bool clearPath(int** pixVals, int r, int c, int r1, int c1)
{
	//trying to get from r,c -> r1, c1 without touching black
	int rowIter = r - 1;
	int colIter = c - 1;

	if (r > r1) //we are below the node, go up
	{
		while (rowIter > r1)
		{
			if (pixVals[rowIter][c] == 0) //we've hit black
			{
				return false;
			}
			else {
				rowIter--;
			}
		}
		return true;

	}
	else { //we must be on the right of the node, go left
	
		while (colIter > c1)
		{
			if (pixVals[r][colIter] == 0) //we've hit black
			{
				return false;
			}
			else {
				colIter--;
			}
		}
		return true;

	}






}


//will return the indice in the adj list where the node is found
int getPos(vector<linked_list> adj, int row, int col)
{
	for (int i = 0; i < adj.size(); i++)
	{
		if (adj[i].getRow() == row && adj[i].getCol() == col)
		{
			return i;
		}
	}

	return -1; //not found
}


//pred = predeecesssor of each vertex
//dest = distance from source
bool BFS(vector<linked_list> adj, unordered_map<Key, bool> visited, node* src, node* dest, unordered_map < Key, node*> &pred, unordered_map < Key, int> &dist)
{

	list<node*> queue;

	int srcPos = getPos(adj, src->row, src->col);
	int destPos = getPos(adj, dest->row, dest->col);
	Key k;
	node* dummy = new node;
	dummy->row = -1;
	dummy->col = -1;
	//initially all verticies are unvistied
	for (int i = 0; i < adj.size(); i++)
	{
		k = { adj[i].getRow(), adj[i].getCol() };
		dist.insert(make_pair(k, INT_MAX));
		visited.insert(make_pair(k, false));
		pred.insert(make_pair(k, dummy));
	}
	
	 k = { src->row, src->col };
	
	
	visited.find(k)->second = true; //we start by visiting the source node
	dist.find(k)->second = 0;
	queue.push_back(src);
	int i = 0;
	node* curr;
	node* u;
	while (!queue.empty())
	{
		u = queue.front();
		int uPos = getPos(adj, u->row, u->col);
		queue.pop_front();
		Key uKey = { u->row, u->col };
		curr = adj[uPos].getHead(); //starting pos of the linked list
		while (curr != NULL)
		{
			
			k = { curr->row, curr->col };
			//printKey(k);
			//cout << endl;
			if (visited.find(k)->second == false) //it has not already been visited
			{
				visited.find(k)->second = true;
				dist.find(k)->second = dist.find(uKey)->second + 1;
				pred.find(k)->second = u; 
				queue.push_back(curr);
				

				
				if (curr->row == dest->row && curr->col == dest->col) //we found the destination
				{
					return true;
				}
				curr = curr->next;
			}
			else {
				curr = curr->next;
			}
			
		}


	}

	


	return false;

}


void printShortestDistance(vector<linked_list> adj, list<node*> &path, node* src, node* dest)
{
	unordered_map < Key, node*> pred;
	unordered_map < Key, int> dist;
	unordered_map<Key, bool> visited;
		if(BFS(adj, visited, src, dest, pred, dist) == false)
		{
			cout << "Impossible Path" << endl;
			return;
		}
		list<node*> pathTMP;
		node* crawl = dest;
		path.push_back(crawl);
		Key k = { dest->row, dest->col };
		while (pred.find(k)->second->row != -1)
		{
			k = { crawl->row, crawl->col };
			path.push_back(pred.find(k)->second);
			crawl = pred.find(k)->second;
		}
		k = { dest->row, dest->col };
		cout << "Shortest path length is : " << dist.find(k)->second + 1 << endl;

		cout << "Path is\n";
		node* n;
		pathTMP = path; 
		while(!pathTMP.empty())
		{
			n = pathTMP.front();
			pathTMP.pop_front();
			if (n->row == -1)
			{
				break;
			}
			cout << "(" << n->row << ", " << n->col << ") ";
		}
		cout << endl;
}

void fillMaze(Mat img_bgr, list<node*> path)
{
	node* n = path.front();
	int prevR = n->row;
	int prevC = n->col;
	n = path.front();
	path.pop_front();
	img_bgr.at<Vec3b>(prevR, prevC)[0] = 0;
	img_bgr.at<Vec3b>(prevR, prevC)[1] = 0;
	img_bgr.at<Vec3b>(prevR, prevC)[2] = 255;
	int r;
	int c;
	while (!path.empty())
	{
		n = path.front();
		path.pop_front();
		r = n->row;
		c = n->col;
		if (n->row == -1)
		{
			break;
		}

		if (prevR > r)
		{
			while (r != prevR)
			{
				img_bgr.at<Vec3b>(r, c)[0] = 0;
				img_bgr.at<Vec3b>(r, c)[1] = 0;
				img_bgr.at<Vec3b>(r, c)[2] = 255;
				r++;
			}
		}
		
		if (prevR < r)
		{
			while (r != prevR)
			{
				img_bgr.at<Vec3b>(r, c)[0] = 0;
				img_bgr.at<Vec3b>(r, c)[1] = 0;
				img_bgr.at<Vec3b>(r, c)[2] = 255;
				r--;
			}
		}

		if (prevC < c)
		{
			while (c != prevC)
			{
				img_bgr.at<Vec3b>(r, c)[0] = 0;
				img_bgr.at<Vec3b>(r, c)[1] = 0;
				img_bgr.at<Vec3b>(r, c)[2] = 255;
				c--;
			}

		}

		if (prevC > c)
		{
			while (c != prevC)
			{
				img_bgr.at<Vec3b>(r, c)[0] = 0;
				img_bgr.at<Vec3b>(r, c)[1] = 0;
				img_bgr.at<Vec3b>(r, c)[2] = 255;
				c++;
			}
		}
		
		prevC = n->col;
		prevR = n->row;

		img_bgr.at<Vec3b>(n->row, n->col)[0] = 0;
		img_bgr.at<Vec3b>(n->row, n->col)[1] = 0;
		img_bgr.at<Vec3b>(n->row, n->col)[2] = 255;
	}

	Mat tmp;
	imwrite("solved1.png", img_bgr);

}


int main()
{
	vector<linked_list> adj;
	Mat img_bgr = imread("10x10Maze.png");

	//Transforming graph into 2D array --------------------------------------------------------------------------------
	//cout << img_bgr.size().width << endl;
	int rowSize = img_bgr.size().height;
	int colSize = img_bgr.size().width;
	int nodeCount = 0;
	int** pixVals = new int* [rowSize];
	for (int i = 0; i < rowSize; ++i)
		pixVals[i] = new int[colSize];

	for (int x = 0; x < rowSize; x++)
	{
		for (int y = 0; y < colSize; y++)
		{
			pixVals[x][y] = 0;
		}


	}

	for (int r = 0; r < img_bgr.rows; ++r) {
		for (int c = 0; c < img_bgr.cols; ++c) {

			auto pixel = img_bgr.at<Vec3b>(r, c);
			if (pixel[0] < 100) //black
			{
				pixVals[r][c] = 0;
			}
			else if (pixel[0] > 200) //white
			{
				pixVals[r][c] = 1;
			}





		}
	}

	//BEGINNING SCAN TO POPULATE ADJACENCY LIST -----------------------------------------------------------------------
	linked_list tmp;
	
	//Getting starting position
	for (int x = 0; x < rowSize; x++)
	{
		if (pixVals[0][x] == 1) //first white square, starting pos
		{
			adj.push_back(tmp);
			adj[adj.size() - 1].insert(0, x);
			nodeCount++;
			

		}
	}
	int r1;
	int c1;
	
	//Scanning through to populate maze with nodes
	for (int r = 1; r < rowSize-1; r++)//start at second row because we've already found starting pos
	{
		for (int c = 0; c < colSize-1; c++)
		{
		
			r1 = r - 1; //temp variables to search for connections left and right
			c1 = c - 1;

			if (pixVals[r][c] == 0) //if we are on a black space, do nothing
			{
				continue;

			}
			else if (pixVals[r][c] == 1 && pixVals[r - 1][c] == 0 && pixVals[r][c - 1] == 0) //if on white and up and left are black
			{
				if (pixVals[r + 1][c] == 0) //if below is also black, we are at dead end going to left, so no need to place anything
				{

					continue;
				}

				if (pixVals[r][c + 1] == 0) //if right is also black, we are at dead end going up, so no need to place anything
				{

					continue;
				}
				adj.push_back(tmp);
				adj[adj.size() - 1].insert(r, c);
				nodeCount++;

			}

			else if (pixVals[r][c] == 1 && pixVals[r - 1][c] == 0 && pixVals[r][c + 1] == 0) //if on white and up and right are black
			{
				if (pixVals[r + 1][c] == 0) //if below is also black, we are at dead end, so no need to place anything
				{

					continue;
				}
				adj.push_back(tmp);
				adj[adj.size() - 1].insert(r, c);
				nodeCount++;



				while (c1 >= 0) //searching for nodes left to connect
				{
					if (check(adj, r, c1))
					{
						if (clearPath(pixVals, r, c, r, c1))
						{
							adj = addEdge(adj, r, c1, r, c);
						}
						break;
					}
					else {
						c1--;
					}

				}



			}

			else if (pixVals[r][c] == 1 && pixVals[r + 1][c] == 0 && pixVals[r][c - 1] == 0) //if on white and bottom and left are black
			{
				if (pixVals[r][c + 1] == 0) //if right is also black, we are at dead end going down, so no need to place anything
				{

					continue;
				}
				adj.push_back(tmp);
				adj[adj.size() - 1].insert(r, c);
				nodeCount++;



				while (r1 >= 0) //searching for nodes above to connect
				{
					if (check(adj, r1, c))
					{
						if (clearPath(pixVals, r, c, r1, c))
						{
							adj = addEdge(adj, r1, c, r, c);

						}
						break;
					}
					else {
						r1--;
					}

				}


				while (c1 >= 0) //searching for nodes left to connect
				{
					if (check(adj, r, c1))
					{
						if (clearPath(pixVals, r, c, r, c1))
						{
							adj = addEdge(adj, r, c1, r, c);
						}

						break;
					}
					else {
						c1--;
					}

				}

			}

			else if (pixVals[r][c] == 1 && pixVals[r + 1][c] == 0 && pixVals[r][c + 1] == 0) //if on white and bottom and right are black
			{
				if (pixVals[r][c - 1] == 0) //if left is also black, we are at dead end going down, so no need to place anything
				{

					continue;
				}
				adj.push_back(tmp);
				adj[adj.size() - 1].insert(r, c);
				nodeCount++;



				while (r1 >= 0) //searching for nodes above to connect
				{
					if (check(adj, r1, c))
					{
						if (clearPath(pixVals, r, c, r1, c))
						{
							adj = addEdge(adj, r1, c, r, c);

						}
						break;
					}
					else {
						r1--;
					}

				}


				while (c1 >= 0) //searching for nodes left to connect
				{
					if (check(adj, r, c1))
					{
						if (clearPath(pixVals, r, c, r, c1))
						{
							adj = addEdge(adj, r, c1, r, c);
						}

						break;
					}
					else {
						c1--;
					}

				}


			}

			else if (pixVals[r][c] == 1 && pixVals[r][c - 1] == 1 && pixVals[r][c + 1] == 1 && pixVals[r - 1][c] == 1 && pixVals[r + 1][c] == 1) //left, right, up, down are white, we are at an intersection
			{

				adj.push_back(tmp);
				adj[adj.size() - 1].insert(r, c);
				nodeCount++;


				while (r1 >= 0) //searching for nodes above to connect
				{
					if (check(adj, r1, c))
					{
						if (clearPath(pixVals, r, c, r1, c))
						{
							adj = addEdge(adj, r1, c, r, c);

						}
						break;
					}
					else {
						r1--;
					}

				}


				while (c1 >= 0) //searching for nodes left to connect
				{
					if (check(adj, r, c1))
					{
						if (clearPath(pixVals, r, c, r, c1))
						{
							adj = addEdge(adj, r, c1, r, c);
						}

						break;
					}
					else {
						c1--;
					}

				}
			}

			else if (pixVals[r][c] == 1 && pixVals[r][c - 1] == 1 && pixVals[r][c + 1] == 1 && pixVals[r + 1][c] == 0 && pixVals[r - 1][c] == 1) //left, right, up, are white, and down is black 
			{
				adj.push_back(tmp);
				adj[adj.size() - 1].insert(r, c);
				nodeCount++;


				while (r1 >= 0) //searching for nodes above to connect
				{
					if (check(adj, r1, c))
					{
						if (clearPath(pixVals, r, c, r1, c))
						{
							adj = addEdge(adj, r1, c, r, c);

						}
						break;
					}
					else {
						r1--;
					}

				}


				while (c1 >= 0) //searching for nodes left to connect
				{
					if (check(adj, r, c1))
					{
						if (clearPath(pixVals, r, c, r, c1))
						{
							adj = addEdge(adj, r, c1, r, c);
						}

						break;
					}
					else {
						c1--;
					}

				}



			}

			else if (pixVals[r][c] == 1 && pixVals[r][c - 1] == 1 && pixVals[r][c + 1] == 0 && pixVals[r + 1][c] == 1 && pixVals[r - 1][c] == 1) //left, up, down, are white, and right is black 
			{
				adj.push_back(tmp);
				adj[adj.size() - 1].insert(r, c);
				nodeCount++;


				while (r1 >= 0) //searching for nodes above to connect
				{
					if (check(adj, r1, c))
					{
						if (clearPath(pixVals, r, c, r1, c))
						{
							adj = addEdge(adj, r1, c, r, c);

						}
						break;
					}
					else {
						r1--;
					}

				}


				while (c1 >= 0) //searching for nodes left to connect
				{
					if (check(adj, r, c1))
					{
						if (clearPath(pixVals, r, c, r, c1))
						{
							adj = addEdge(adj, r, c1, r, c);
						}

						break;
					}
					else {
						c1--;
					}

				}



			}

			else if (pixVals[r][c] == 1 && pixVals[r][c - 1] == 0 && pixVals[r][c + 1] == 1 && pixVals[r + 1][c] == 1 && pixVals[r - 1][c] == 1) //right, up, down, are white, and left is black 
			{
				adj.push_back(tmp);
				adj[adj.size() - 1].insert(r, c);
				nodeCount++;


				while (r1 >= 0) //searching for nodes above to connect
				{
					if (check(adj, r1, c))
					{
						if (clearPath(pixVals, r, c, r1, c))
						{
							adj = addEdge(adj, r1, c, r, c);

						}
						break;
					}
					else {
						r1--;
					}

				}


				while (c1 >= 0) //searching for nodes left to connect
				{
					if (check(adj, r, c1))
					{
						if (clearPath(pixVals, r, c, r, c1))
						{
							adj = addEdge(adj, r, c1, r, c);
						}

						break;
					}
					else {
						c1--;
					}

				}



			}


			else if (pixVals[r][c] == 1 && pixVals[r][c - 1] == 1 && pixVals[r][c + 1] == 1 && pixVals[r + 1][c] == 1 && pixVals[r - 1][c] == 0) //left, right, down, are white, and up is black 
			{
				adj.push_back(tmp);
				adj[adj.size() - 1].insert(r, c);
				nodeCount++;


				while (r1 >= 0) //searching for nodes above to connect
				{
					if (check(adj, r1, c))
					{
						if (clearPath(pixVals, r, c, r1, c))
						{
							adj = addEdge(adj, r1, c, r, c);

						}
						break;
					}
					else {
						r1--;
					}

				}


				while (c1 >= 0) //searching for nodes left to connect
				{
					if (check(adj, r, c1))
					{
						if (clearPath(pixVals, r, c, r, c1))
						{
							adj = addEdge(adj, r, c1, r, c);
						}

						break;
					}
					else {
						c1--;
					}

				}


 
			 }

			
		}




	}
	

	r1 = rowSize-2;

	
	
	//GETTING THE FINISHING POSITION OF THE MAZE ----------------------------------------------------------------------
	
	for (int c = 0; c < colSize; c++) //find last position
	{
		if (pixVals[rowSize-1][c] == 1) //first white square, starting pos
		{
			adj.push_back(tmp);
			adj[adj.size() - 1].insert(rowSize-1, c);
			nodeCount++;
				while (r1 >= 0) //searching for nodes above to connect
				{
					
					if (check(adj, r1, c))
					{
						if (clearPath(pixVals, rowSize-1, c, r1, c))
						{
							adj = addEdge(adj, r1, c, rowSize-1, c);

						}
						break;
					}
					else {
						r1--;
					}

				}



		}
	}
	
	
	
	cout << "Nodes: " << nodeCount << endl;
	cout << "Size: " << colSize << " x " << rowSize << endl;
	

	unordered_map < Key, bool > visited;  
	int size; 
	unordered_map < Key, node*> pred;
	unordered_map < Key, int> dist;
		

	

	list<node*> path;
	printShortestDistance(adj, path, adj[0].getHead(), adj[adj.size() - 1].getHead());
	fillMaze(img_bgr, path);

	unordered_map<Key, node*>::iterator it = pred.begin();
	node* curr;




	


	
	return 0;
}