
/*
    $Id: color-quant.h,v 1.1 1995/11/23 04:31:12 jaw Exp jaw $

*/

#include <images.h>
#include <handy.h>

#define B(x)		(1<<(x))
typedef int		Boolean;
typedef unsigned int	u_int;

#define VSIZE	3		/* dimensionality of the vector */
#define CSIZE	8		/* each component of the vector is 8 bits */
// 	The code below assumes on VSIZE*CSIZE being <= 32 (the number of bits in a int)
            
#define NVEC	B(VSIZE)	/* 2**VSIZE -- number of branches on tree */


struct Cell {
	int a,b,c;
};

extern int mkindx(Cell& cell);
void quant(ColorImage& img, int k, Bimage& nimg);
void mse(ColorImage& cimg, Bimage& nimg);

class Node {
  public:
	Cell cell;			// the vector of this node
	u_int count;			// number of pixels at this node (and below)
	u_int index;			// index representing this vector (our final result)
	int depth;			// depth of this node
	Boolean leafp;			// am I a leaf?
	int maxdepth;			// how much deeper does it get below me?
	u_int totalleaves;		// how many leaves below me?
	Node *branch[ NVEC ];		// das kinder
	
	Node(int d){
		count = index = 0;
		totalleaves = 0;
		depth = d;
		maxdepth = 0;
		leafp = 0;
		for(int i=0;i<NVEC;i++)branch[i]=0;
	}
	
	void add(Cell&, u_int);		// (vector, index)  add a cell to the tree
	void reduce(void);		// reduce tree
	void assgnindx(int&, Cmap*);	// assign indices
	int indxof(int);		// find the index of our vector
	int bestchoice(void);		// pick the best branch to reduce
	int metric(void);		// the metric used to determine the best choice
	
};

