
//#define GRIDPIX_STILL_DEBUGGING

#ifdef GRIDPIX_STILL_DEBUGGING
#include <iostream>
#endif

#include "GridPix.hpp"

#include "TClass.h"

using namespace std ;

/////////////////////////////////////////
//
// class GridPix
//
////////////////////////////////////////

//
// Constructors
//

GridPix::GridPix(int fillnhits, 
		 pixel_t* xdata, pixel_t* ydata, 
		 value_t* tdata)
  : nhits(0), x(0), y(0), t(0)
  , n_allocated(0), n_datachunks(0)
  , firstchunk (0)
{
#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "New GridPix object at " << this << std::endl;
#endif

  GridPixClassInit();

  if (fillnhits<=0) return;

  n_allocated = fillnhits + addspace;
  x = new pixel_t[n_allocated];
  y = new pixel_t[n_allocated];
  t = new value_t[n_allocated];
#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "\tData arrays with " << n_allocated 
	    << " allocated elements are at\n\t" 
	    << (void*)x << "\t" << (void*)y 
	    << "\t" << (void*)t << std::endl;
#endif
  firstchunk = new datachunk(0, n_allocated, 0, x, y, t, 0);
  n_datachunks = 1;

  if (xdata && ydata && tdata)
    for (int i=0; i < fillnhits; ++i)
      Insert(xdata[i],ydata[i],tdata[i]);
}

GridPix::~GridPix()
{ 
#ifdef GRIDPIX_STILL_DEBUGGING
      std::cout << "Deleting GridPix object at " << this 
		<< "\n\tData arrays with at\n\t" 
		<< (void*)x << "\t" << (void*)y 
		<< "\t" << (void*)t 
		<< "\n\t\tare going to be deleted" << std::endl;
#endif
  if (x) delete [] x;
  if (y) delete [] y;
  if (t) delete [] t;
  if (firstchunk) delete firstchunk;
}

//
// Only-once class initializer
//
bool GridPix::classinitdone = false;
void GridPix::GridPixClassInit()
{
  if (!classinitdone)
    {
      classinitdone = true;
      TClass::GetClass("GridPix")->IgnoreTObjectStreamer();
    }
}


//
// declared within the ClassDef macro, it must be customized!
//

void GridPix::Streamer(TBuffer &R__b)
{
  // Stream an object of class GridPix.
  
  if (R__b.IsReading()) {
    R__b.ReadClassBuffer(GridPix::Class(),this);
  } else {
    RepackData(); //  <--- customization!!!
    R__b.WriteClassBuffer(GridPix::Class(),this);
  }
}
#ifdef __MAKECINT__
#pragma link C++ class GridPix-;
#endif

//
// Methods of class GridPix
//

int GridPix::Insert(pixel_t newx, pixel_t newy, value_t newt)
{
#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "Insert() method of object at " << this  
	    << ". At the moment\n\t n_allocated = " 
	    << n_allocated << "\t nhits = " << nhits
	    << "\t firstchunk = " << firstchunk 
	    << "\t n_datachunks = " << n_datachunks << std::endl;
#endif
  //
  // CASE 1: Totally empty!
  //
  if (!nhits){
    if (n_allocated > 0){
			//is it possible that for some weird reason it is bad?!?!?
			if (!firstchunk) return errbaddata ;

			firstchunk->x[0] = newx;
			firstchunk->y[0] = newy;
			firstchunk->t[0] = newt;
			firstchunk->nhitshere = 1;
			nhits = 1;
			return 1;
		}
    else {
			//is it possible that for some weird reason it exists?!?!?
			if (firstchunk) return errbaddata;

			n_allocated = addspace;
			x = new pixel_t[n_allocated];
			y = new pixel_t[n_allocated];
			t = new value_t[n_allocated];
			x[0] = newx; y[0] = newy; t[0] = newt;
			nhits++;
			firstchunk = new datachunk(nhits, n_allocated, false,x, y, t, 0);
			n_datachunks = 1;
			return 1;
		} ;
	}
  
  //
  // CASE 2: Looking for a position in a specific chunk
  //
  SortingTools::sortindex_t newindex 
    = GridPix::SortingTools::GetSortIndex(newx,newy);
  
#ifdef GRIDPIX_STILL_DEBUGGING
  //at this point firstchunk must exist!
  if (!firstchunk){
		std::cout << "firstchunk does not exist.. How is it possible?!?!"
			<< std::endl;
		return errbaddata;
	}
#endif

  datachunk* prevchunk = firstchunk;
  datachunk* nextchunk = firstchunk->next;

  SortingTools::pos_t prevpos 
    = GridPix::SortingTools::FindThePlace(newindex,
					  firstchunk->nhitshere,
					  firstchunk->x, 
					  firstchunk->y);
  if (prevpos == SortingTools::alreadyexists) return noop;
      
  SortingTools::pos_t nextpos = SortingTools::undefinedpos;

  while (nextchunk && prevpos < SortingTools::firstgoodpos)
    {
      nextpos = GridPix::SortingTools::FindThePlace(newindex,
						    nextchunk->nhitshere,
						    nextchunk->x, 
						    nextchunk->y);
      if (nextpos == SortingTools::alreadyexists) return noop;

      if (nextpos == SortingTools::emptylist 
	  && prevpos == SortingTools::emptylist)
	{
	  //a further chunk must be checked, the previous stay the same
	  nextchunk = nextchunk->next;
	  continue; //and iterate again!
	}
      //otherwise shift prev and next and try again
      prevchunk = nextchunk;
      prevpos = nextpos;
      nextchunk = nextchunk->next;
    };

#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "Place in the chunk at " << prevchunk 
	    << " at position " << (int)prevpos << std::endl;
#endif

  //
  // sub-CASE 2A: to be placed at the beginning of the chunk
  //
  if (prevpos == SortingTools::emptylist){
		if (prevchunk->n_slots > 0){
			prevchunk->x[0] = newx; prevchunk->y[0] = newy;
			prevchunk->t[0] = newt;
			prevchunk->nhitshere = 1;
			nhits++;
			return 1;
		}
		else{
			// This should never happen.
#ifdef GRIDPIX_STILL_DEBUGGING
			std::cout << "THIS SHOULD NEVER HAPPEN. Chunk object at "
				<< prevchunk << " has 0 allocated space!" << std::endl;
#endif	
			return errbaddata;
		};
	}

  //
  // sub-CASE 2B: to be placed at the end of the chunk or, in case it
  // is full, a new chunk must be inserted
  //
  if (prevpos == SortingTools::afteranyother){
		if (prevchunk->n_slots > prevchunk->nhitshere){
			prevchunk->x[prevchunk->nhitshere] = newx;
			prevchunk->y[prevchunk->nhitshere] = newy;
			prevchunk->t[prevchunk->nhitshere] = newt;
			prevchunk->nhitshere++;
			nhits++;
			return 1;
		}
		else{
			prevchunk->next = new datachunk(1, addspace, true,
			   &newx, &newy, &newt, prevchunk->next);
			n_datachunks ++;
			nhits++;
			return 1;
		} ;
	}

  //
  // sub-CASE 2C: if it is possible and does not cost too much
  // shifting, make room and place the new element. Otherwise split
  // the chunk in two, and insert a third chunk with the new element
  // between them
  //
  if (prevpos >= SortingTools::firstgoodpos){
		//if possible and not much shift implied, do it! 
    if ( prevchunk->n_slots > prevchunk->nhitshere 
				&& (prevchunk->nhitshere - prevpos <= thrshift) ){
			prevchunk->ShiftAndPlace(prevpos, newx, newy, newt);
			nhits++;
			return 1;
		}
    //otherwise, if in pos 0, insert a new chunk
		else if (prevpos == 0){
			prevchunk->next = new datachunk(1, addspace, true,
					&newx, &newy, &newt, prevchunk->next);
			n_datachunks++;
			nhits++;
			return 1;
		}
    //otherwise split in two chunks and insert a new chunk between
    else{
			datachunk* secondpiece
				= new datachunk(prevchunk->nhitshere - prevpos,
						prevchunk->n_slots - prevpos, false,
						&(prevchunk->x[prevpos]), &(prevchunk->y[prevpos]),
						&(prevchunk->t[prevpos]), prevchunk->next);
			prevchunk->nhitshere = prevpos;
			prevchunk->n_slots = prevpos;
			prevchunk->next = new datachunk(1, addspace, true,
					&newx, &newy, &newt, secondpiece);
			n_datachunks += 2;
			nhits++;
			return 1;
		}
	}

  return -5;
}


int GridPix::ClearData()
{
#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "Clear() method of object at " << this  
	    << ". At the moment\n\t n_allocated = " 
	    << n_allocated << "\t nhits = " << nhits
	    << "\t firstchunk = " << firstchunk 
	    << "\t n_datachunks = " << n_datachunks << std::endl;
#endif

  if (firstchunk) delete firstchunk;
  nhits = 0;
  n_datachunks = 0;

  if (n_allocated)
    {
      firstchunk = new datachunk(0, n_allocated, 0, x, y, t, 0);
      n_datachunks = 1;
    }

  return 1;
}


int GridPix::RepackData()
{
#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "RepackData() method of object at " << this  
	    << ". At the moment\n\t n_allocated = " 
	    << n_allocated << "\t nhits = " << nhits
	    << "\t firstchunk = " << firstchunk 
	    << "\t n_datachunks = " << n_datachunks << std::endl;
#endif

  if (nhits && (!n_allocated) && (!firstchunk) ) 
    return errbaddata;
  
  if (nhits < 0)
    return errbaddata;

  if (!(firstchunk->next)) return nhits; //no repacking is needed!  

  size_t tmpn = nhits + addspace;
  pixel_t* tmpx = new pixel_t[tmpn];
  pixel_t* tmpy = new pixel_t[tmpn];
  value_t* tmpt = new value_t[tmpn];

  datachunk* nextchunk = firstchunk;
  int pos = 0;
  int nchunks = 0;
  while (nextchunk)
    {
      for (size_t i = 0; i < nextchunk->nhitshere; ++i)
	{
	  tmpx[pos] = nextchunk->x[i];
	  tmpy[pos] = nextchunk->y[i];
	  tmpt[pos] = nextchunk->t[i];
	  pos++;
	}
      nchunks++;
      nextchunk = nextchunk->next;
    }

#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "\t-----> final pos = " << pos 
	    << "\tnchunks = " << nchunks << std::endl;
#endif

  if (pos != nhits || nchunks != n_datachunks)
      return errbaddata;


#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "\n\tData arrays with at\n\t" 
	    << (void*)x << "\t" << (void*)y 
	    << "\t" << (void*)t 
	    << "\n\t\tare going to be deleted" << std::endl;
#endif
  delete firstchunk;
  delete [] x; delete [] y; delete [] t;

  n_allocated = tmpn;
  x = tmpx; y = tmpy; t = tmpt;
#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "\tNew data arrays with " << n_allocated 
	    << " allocated elements are at\n\t" 
	    << (void*)x << "\t" << (void*)y 
	    << "\t" << (void*)t << std::endl;
#endif
  firstchunk = new datachunk(nhits, n_allocated, false,
			     x, y, t, 0);
  n_datachunks = 1;

  return nhits;
}





////////////////////////////////////////////////////////////
//
// internal struct GridPix::datachunk
//
////////////////////////////////////////////////////////////

//
// Constructors
//

GridPix::datachunk::datachunk(size_t nhits, 
			      size_t n_alloc, bool do_alloc,
			      pixel_t* xlink, pixel_t* ylink, 
			      value_t* tlink,
			      datachunk* linkafter)
  : nhitshere(nhits)
  , x(xlink), y(ylink), t(tlink)
  , next(linkafter), n_slots(n_alloc), mustdelete(do_alloc)
{
  // a new allocation is requested rather than a link
#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "New GridPix::datachunk object at " << this; 
#endif
  if (mustdelete)
    {
      x = new pixel_t[n_slots];
      y = new pixel_t[n_slots];
      t = new value_t[n_slots];      
      for (size_t i=0; i < nhitshere; ++i) //NO CHECK ON n_slots BOUNDARY!!!
	{
	  x[i] = xlink[i];
	  y[i] = ylink[i];
	  t[i] = tlink[i];
	}
    }
#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "\n\tData arrays with " << n_slots 
	    << " slots are at\n\t" 
	    << (void*)x << "\t" << (void*)y << "\t" << (void*)t 
	    << "\n\tThey will ";
  if (!mustdelete) std::cout << "NOT ";
  std::cout << "be deleted by this object" << std::endl;
#endif
}

GridPix::datachunk::~datachunk() 
{ 
#ifdef GRIDPIX_STILL_DEBUGGING
      std::cout << "Deleting GridPix::datachunk object at " 
		<< this << std::endl;
#endif
  if (mustdelete) 
    { 
#ifdef GRIDPIX_STILL_DEBUGGING
      std::cout << "\n\tData arrays with at\n\t" 
		<< (void*)x << "\t" << (void*)y << "\t" << (void*)t 
		<< "\n\t are going to be deleted" << std::endl;
#endif
      delete [] x; delete [] y; delete [] t; 
    }
  if (next) delete next; 
};

void GridPix::datachunk::ShiftAndPlace(size_t thepos, 
				       pixel_t newx, pixel_t newy,
				       value_t newt)
{
#ifdef GRIDPIX_STILL_DEBUGGING
  std::cout << "Calling GridPix::datachunk::ShiftAndPlace of object at " 
	    << this << "\n\tnhitshere,n_slots=" << nhitshere << "," 
	    << n_slots << "\tpos=" << thepos << std::endl;
#endif

  // NO CHECK FOR BOUNDARIES!!! IT IS ASSUMED TO BE DONE BY THE CALLER!!!
  for (int i = nhitshere-1; i >= static_cast<int>(thepos); --i)
    {
      x[i+1] = x[i];
      y[i+1] = y[i];
      t[i+1] = t[i];
    }
  x[thepos] = newx;
  y[thepos] = newy;
  t[thepos] = newt;
  nhitshere++;

  return;
}





/////////////////////////////////////////////////////////
//
// internal class SortingTools
//
// a collection of static functions for sorting
//
//
/////////////////////////////////////////////////////////

//
// Methods of internal class GridPix::SortingTools
//

GridPix::SortingTools::pos_t 
GridPix::SortingTools::FindThePlace(sortindex_t newindex,
				    size_t n, 
				    pixel_t* x, pixel_t* y)
{
  //Unfortunately it can happen that the chunk is empty, because of
  //weird construction or removal of all elements  
  if (n && x && y)
    {
      size_t lower = 0, higher = n-1, middlepos = 0;
      if ( newindex == GridPix::SortingTools::GetSortIndex(x[0],y[0]) 
	   || newindex ==  GridPix::SortingTools::GetSortIndex(x[higher],y[higher]))
	return alreadyexists; //it is a duplicated entry!!!

      if ( newindex <
	   GridPix::SortingTools::GetSortIndex(x[0],y[0]) )
	return 0; //the new one before any other here
      if ( newindex >
	   GridPix::SortingTools::GetSortIndex(x[higher],y[higher]) )
	return afteranyother; //the new one after any other here

      while (higher - lower > 1)
	{
	  middlepos = (higher - lower)/2 + lower; //integer
	                                          //division!!!
	  if ( newindex ==
	       GridPix::SortingTools::GetSortIndex(x[middlepos],y[middlepos]) )
	    {
	      return alreadyexists;
	    }
	  else if ( newindex >
		    GridPix::SortingTools::GetSortIndex(x[middlepos],y[middlepos]) )
	    {
	      lower = middlepos;
	    }
	  else higher = middlepos;

//	  std::cout << (int)lower << "\t" << (int)middlepos 
//		    << "\t" << (int)higher << std::endl;
	}  
      //what happened in the last iteration?
      return higher;
    }

  //all other cases are errors
  return emptylist; 
}


double GridPix::CountIf(GridPix::pixel_t xmin, GridPix::pixel_t xmax,
			GridPix::pixel_t ymin, GridPix::pixel_t ymax,
			GridPix::value_t tmin, GridPix::value_t tmax)
{
  double sum = 0;

  for (int i = 0; i < nhits; i++)
    if ( x[i] > xmin && x[i] < xmax 
	 && y[i] > ymin && y[i] < ymax 
	 && t[i] > tmin && t[i] < tmax )
      sum++;

  return sum;
}
