/*  
suffix_tree contains all the necessary functions to build a suffix tree following 
Ukkonen's Suffix Tree algorithm. 

the implementation is based on https://www.geeksforgeeks.org/ukkonens-suffix-tree-construction-part-1/ to part-6
although some modifications have been performed. 
-----------------------------------------------------------------------------------------
*/


#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>

#include "suffix_tree.h"

#define MAX_CHAR 7
short lookup[256] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 3, 0, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 6, 0,
0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// This are the ASCII codes for $,A,C,G,N,T : { 36,65,67,71,78,84 }

Node *root = NULL; //Pointer to root node 

/*lastNewNode will point to newly created internal node,
waiting for it's suffix link to be set, which might get
a new suffix link (other than root) in next extension of
same phase. lastNewNode will be set to NULL when last
newly created internal node (if there is any) got it's
suffix link reset to new internal node created in next
extension of same phase. */
Node *lastNewNode = NULL;
Node *activeNode = NULL;

/*activeEdge is represeted as input string character
index (not the character itself)*/
int activeEdge = -1;
int activeLength = 0;

// remainingSuffixCount tells how many suffixes yet to 
// be added in tree 
int remainingSuffixCount = 0;
int leafEnd = -1;
int *rootEnd = NULL;
int *splitEnd = NULL;
int size = -1; //Length of input string 

Node *newNode(int start, int *end)
{
	Node *node = (Node*)malloc(sizeof(Node));
	int i;
	for (i = 0; i < MAX_CHAR; i++)
		node->children[i] = NULL;

	/*For root node, suffixLink will be set to NULL
	For internal nodes, suffixLink will be set to root
	by default in current extension and may change in
	next extension*/
	node->suffixLink = root;
	node->start = start;
	node->end = end;

	/*suffixIndex will be set to -1 by default and
	actual suffix index will be set later for leaves
	at the end of all phases*/
	node->suffixIndex = -2;
	return node;
}

int edgeLength(Node *n) {
	return *(n->end) - (n->start) + 1;
}

int walkDown(Node *currNode, int pos, char * text)
{
	/*activePoint change for walk down (APCFWD) using
	Skip/Count Trick (Trick 1). If activeLength is greater
	than current edge length, set next internal node as
	activeNode and adjust activeEdge and activeLength
	accordingly to represent same activePoint*/
	if (activeLength >= edgeLength(currNode))
	{
		activeEdge += edgeLength(currNode); 
		activeLength -= edgeLength(currNode);
		activeNode = currNode;
		if (activeEdge == 0) {
			printf("WARNING. activeEdge letter is 0 (NULL) \n"); 
		}
		return 1;
	}
	return 0;
}

void extendSuffixTree(int pos, char* text)
{
	/*Extension Rule 1, this takes care of extending all
	leaves created so far in tree*/
	leafEnd = pos;

	/*Increment remainingSuffixCount indicating that a
	new suffix added to the list of suffixes yet to be
	added in tree*/
	remainingSuffixCount++;

	/*set lastNewNode to NULL while starting a new phase,
	indicating there is no internal node waiting for
	it's suffix link reset in current phase*/
	lastNewNode = NULL;

	//Add all suffixes (yet to be added) one by one in tree 
	while (remainingSuffixCount > 0) {

		if (activeLength == 0)
			activeEdge = pos;

		// There is no outgoing edge starting with 
		// activeEdge from activeNode 
		if (activeNode->children[lookup[text[activeEdge]] - 1] == NULL) 
		{
			//Extension Rule 2 (A new leaf edge gets created) 
			activeNode->children[lookup[text[activeEdge]] - 1] = newNode(pos, &leafEnd);

			/*A new leaf edge is created in above line starting
			from an existng node (the current activeNode), and
			if there is any internal node waiting for it's suffix
			link get reset, point the suffix link from that last
			internal node to current activeNode. Then set lastNewNode
			to NULL indicating no more node waiting for suffix link
			reset.*/
			if (lastNewNode != NULL)
			{
				lastNewNode->suffixLink = activeNode;
				lastNewNode = NULL;
			}
		}
		// There is an outgoing edge starting with activeEdge 
		// from activeNode 
		else
		{
			// Get the next node at the end of edge starting 
			// with activeEdge 
			Node *next = activeNode->children[lookup[text[activeEdge]] - 1];
			if (walkDown(next, pos, text))//Do walkdown 
			{
				//Start from next node (the new activeNode) 
				continue;
			}
			/*Extension Rule 3 (current character being processed
			is already on the edge)*/
			if (text[next->start + activeLength] == text[pos])
			{
				//If a newly created node waiting for it's 
				//suffix link to be set, then set suffix link 
				//of that waiting node to curent active node 
				if (lastNewNode != NULL && activeNode != root)
				{
					lastNewNode->suffixLink = activeNode;
					lastNewNode = NULL;
				}

				//APCFER3 
				activeLength++;
				/*STOP all further processing in this phase
				and move on to next phase*/
				break;
			}

			/*We will be here when activePoint is in middle of
			the edge being traversed and current character
			being processed is not on the edge (we fall off
			the tree). In this case, we add a new internal node
			and a new leaf edge going out of that new node. This
			is Extension Rule 2, where a new leaf edge and a new
			internal node get created*/
			splitEnd = (int*)malloc(sizeof(int));
			*splitEnd = next->start + activeLength - 1;

			//New internal node 
			Node *split = newNode(next->start, splitEnd);
			activeNode->children[lookup[text[activeEdge]] - 1] = split;

			//New leaf coming out of new internal node 
			split->children[lookup[text[pos]] - 1] = newNode(pos, &leafEnd);
			next->start += activeLength;
			split->children[lookup[text[next->start]] - 1] = next;

			/*We got a new internal node here. If there is any
			internal node created in last extensions of same
			phase which is still waiting for it's suffix link
			reset, do it now.*/
			if (lastNewNode != NULL)
			{
				/*suffixLink of lastNewNode points to current newly
				created internal node*/
				lastNewNode->suffixLink = split;
			}

			/*Make the current newly created internal node waiting
			for it's suffix link reset (which is pointing to root
			at present). If we come across any other internal node
			(existing or newly created) in next extension of same
			phase, when a new leaf edge gets added (i.e. when
			Extension Rule 2 applies is any of the next extension
			of same phase) at that point, suffixLink of this node
			will point to that internal node.*/
			lastNewNode = split;
		}

		/* One suffix got added in tree, decrement the count of
		suffixes yet to be added.*/
		remainingSuffixCount--;
		if (activeNode == root && activeLength > 0) //APCFER2C1 
		{
			activeLength--;
			activeEdge = pos - remainingSuffixCount + 1; 
		}
		else if (activeNode != root) //APCFER2C2 
		{
			activeNode = activeNode->suffixLink;
		}
	}
}

void print(int i, int j, char* text)
{
	int k;
	for (k = i; k <= j; k++)
		printf("%c", text[k]);
}

//Print the suffix tree as well along with setting suffix index 
//So tree will be printed in DFS manner 
//Each edge along with it's suffix index will be printed 
Node * setSuffixIndexByDFS(Node *n, int labelHeight, Node * lastSeenLeaf)
{
	if (n == NULL) return;
	/* if (n->start != -1) //A non-root node 
	{
		//Print the label on edge from parent to current node 
		print(n->start, *(n->end), text);
	} */
	int leaf = 1;
	if (n->suffixIndex == -2)
		n->suffixIndex = -1;
	else if (n->suffixIndex == -1)
		n->suffixIndex = lastSeenLeaf->suffixIndex;
	int i;
	for (i = 0; i < MAX_CHAR; i++)
	{
		if (n->children[i] != NULL)
		{

			//Current node is not a leaf as it has outgoing 
			//edges from it. 
			leaf = 0;
			lastSeenLeaf = setSuffixIndexByDFS(n->children[i], labelHeight +
				edgeLength(n->children[i]), lastSeenLeaf);
			if (n->suffixIndex == -2)
				n->suffixIndex = -1;
			else if (n->suffixIndex == -1)
				n->suffixIndex = lastSeenLeaf->suffixIndex;
		}
	}
	if (leaf == 1) {

		n->suffixIndex = size - labelHeight;
		lastSeenLeaf = n;
	}
	return lastSeenLeaf; 
}

void freeSuffixTreeByPostOrder(Node *n)
{
	if (n == NULL)
		return;
	int i;
	for (i = 0; i < MAX_CHAR; i++)
	{
		if (n->children[i] != NULL)
		{
			freeSuffixTreeByPostOrder(n->children[i]);
		}
	}
	if (n->suffixIndex == -1)
		free(n->end);
	free(n);
}

void printSuffixTreeByPostOrder(Node *n)
{
	if (n == NULL)
		return;
	int i;
	for (i = 0; i < MAX_CHAR; i++)
	{
		if (n->children[i] != NULL)
		{
			printSuffixTreeByPostOrder(n->children[i]);
		}
	}
	printf("%d\n",n->suffixIndex);
}

int countNodesSuffixTree(Node *n, int counter)
{
	if (n == NULL)
		return counter;
	int i;
	for (i = 0; i < MAX_CHAR; i++)
	{
		if (n->children[i] != NULL)
		{
			counter++;
			int partial_count = countNodesSuffixTree(n->children[i], 0);
			counter += partial_count; 
		}
	}
	return counter; 
}

/*Build the suffix tree and print the edge labels along with
suffixIndex. suffixIndex for leaf edges will be >= 0 and
for non-leaf edges will be -1*/
Node * buildSuffixTree(char* text)
{
	size = strlen(text);
	int i;
	rootEnd = (int*)malloc(sizeof(int));
	*rootEnd = -1;

	/*Root is a special node with start and end indices as -1,
	as it has no parent from where an edge comes to root*/
	root = newNode(-1, rootEnd);

	activeNode = root; //First activeNode will be root 
	for (i = 0; i < size; i++)
		extendSuffixTree(i, text);
	int labelHeight = 0;
	setSuffixIndexByDFS(root, labelHeight, NULL);
	setSuffixIndexByDFS(root, labelHeight, NULL);
	return root;
}

