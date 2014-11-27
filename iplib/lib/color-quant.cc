
/*
    $Id: color-quant.cc,v 1.1 1995/11/23 04:30:57 jaw Exp jaw $

    ee492 compression project
    color quantization -- a tree structured VQ approach
*/

#include <color-quant.h>
#include <iostream.h>

void quant(const ColorImage& img, int k, Bimage& nimg){
	// quantize color image to k colors
	// k must be <=256 
	// (so that it fits into nimg, it is not a limitation of 
	// the algorithm (which could handle an arbitrary number))
	
	if(k>256) return;
	
	Node tree(0);
	Cell cell;
	int i;
	u_int rot;

	cerr << "Building tree..." << endl;
	for(i=0; i< img.red.width*img.red.height; i++){
		// iterate over the image
		// adding colors (vectors) to the tree

		cell.a = int(img.red.value(i));
		cell.b = int(img.grn.value(i));
		cell.c = int(img.blu.value(i));
		rot = mkindx( cell );
		
		// add it to the tree
		tree.add(cell, rot);
	}

	// reduce the tree down to the desired size
	cerr << "Reducing tree..." << endl;
	while( tree.totalleaves > k)
		tree.reduce();

	// create indexes and CLUT
	cerr << "Indexing..." << endl;
	if(! nimg.cmap) nimg.cmap = new Cmap;
	int indx=0;
	tree.assgnindx( indx, nimg.cmap );

	// create the new image
	cerr << "creating new image..." << endl;
	nimg.width = img.red.width;
	nimg.height =img.red.height;
	nimg.resize();

	// assign our new values
	for(i=0; i< img.red.width*img.red.height; i++){
		cell.a = int(img.red.value(i));
		cell.b = int(img.grn.value(i));
		cell.c = int(img.blu.value(i));
		rot = mkindx( cell );

		// find the cell in the tree and get the index
		nimg.put(i, tree.indxof(rot));
	}
	

}

void Node::add(Cell& pcell, u_int rotcolor){
	// add a color to the tree
	
	int i, foo;
	
	// CSIZE**NVEC is # of total possible values of the vector
	if( leafp || depth >= CSIZE){ 
		// add vector to this node
		leafp = 1;
		totalleaves = 1;	/* since I am a leaf */
		cell = pcell;
	}else{
		// add it to approp branch
		
		i = rotcolor % CSIZE;					// which branch?
		if( ! branch[i]){					// if no branch there
			branch[i] = new Node(depth+1);			// make one
		}
		foo = totalleaves - branch[i]->totalleaves;		// # leaves on this nodes other branches
		branch[i]->add(pcell, rotcolor/CSIZE);			// add to the proper branch (recurse)
		
		maxdepth = MAX( branch[i]->maxdepth +1, maxdepth);	// did we get deeper?
		totalleaves = foo + branch[i]->totalleaves;		// update # of leaves below here
	}
	count ++;							// update our count
}

void Node::reduce(void){
	// reduce the tree
	int i, n=0;
	int omd=maxdepth;
	int omt=metric();
	
	if(maxdepth <= 1){
		// I'm it -- prune off my branches

		maxdepth = 0;
		totalleaves = 1;
		leafp = 1;
		
		for(i=0; i<NVEC; i++){
			// need make my cell = mean of branch cells
			if( branch[i]){
				n++;
				cell.a += branch[i]->cell.a;
				cell.b += branch[i]->cell.b;
				cell.c += branch[i]->cell.c;
				delete branch[i];		// snip-snip!
			}
		}
		cell.a /=n;
		cell.b /=n;
		cell.c /=n;
	}else{
		// recurse until we notice a reduction
		do{
			i = bestchoice();
			totalleaves -= branch[i]->totalleaves;
			branch[i]->reduce();
			totalleaves += branch[i]->totalleaves;

			// update maxdepth
			maxdepth = 1;
			for(i=0; i<NVEC; i++)
				if(branch[i])
					maxdepth = MAX(maxdepth, branch[i]->maxdepth+1);
		}while( omt==metric() && omd==maxdepth );
	}
}

int Node::bestchoice(void){
	int i;
	int metr;
	int bestd=0, bestm=-1, besti=-1;

	for(i=0; i<NVEC; i++)
		if( branch[i])
			if( branch[i]->maxdepth > bestd ){
				// chose the deepest path
				bestd = branch[i]->maxdepth;
				bestm = branch[i]->metric();
				besti = i;
			}else if( branch[i]->maxdepth == bestd ){
				// of 2 equally deep paths
				// chose one with the best `metric'

				metr = branch[i]->metric();
				if( metr > bestm || besti==-1 ){
					bestd = branch[i]->maxdepth;
					bestm = metr;
					besti = i;
				}
			}
	return besti;
}

int mkindx(Cell& cell){
	int rot = 0;
	// interleave the bits of the vectors
	// and reverse the order (lsb...msb)
	
	for(int j=0; j< CSIZE; j++){
		rot <<=1;
		rot |= cell.a & B(j) ?1:0;
		rot <<=1;
		rot |= cell.b & B(j) ?1:0;
		rot <<=1;
		rot |= cell.c & B(j) ?1:0;
	}
	return rot;
}

void Node::assgnindx( int& indx, Cmap* cmap){
	int i;
	
	if( leafp ){
		// I am the index
		index = indx;
		cmap->r[ index ] = cell.a;
		cmap->g[ index ] = cell.b;
		cmap->b[ index ] = cell.c;
	}else
		// recurse
		for(i=0; i<NVEC; i++)
			if( branch[i] ){
				indx++;
				branch[i]->assgnindx( indx, cmap );
			}
}

int Node::indxof(int rot){

	if( leafp )
		return index;
	else
		// recurse
		return branch[ rot % CSIZE ]->indxof( rot / CSIZE );
}


void mse(ColorImage& cimg, Bimage& nimg){
	int i, ni;
	int dr, dg, db;
	float err=0;

	for(i=0; i<nimg.width*nimg.height; i++){
		ni = int( nimg.value(i));
		dr = int( cimg.red.value(i) - nimg.cmap->r[ni]);
		db = int( cimg.blu.value(i) - nimg.cmap->b[ni]);
		dg = int( cimg.grn.value(i) - nimg.cmap->g[ni]);

		err += dr*dr + db*db + dg*dg;
	}
	err /= nimg.width*nimg.height;

	cout << "MSE:\t" << err << endl;

	cout << "#Colors:\t" << nimg.countcolors() << endl;

}

int Node::metric(void){
        return -count;          // least number of colors
}
 




	
