
#include "TObject.h"

/////////////////////////////////////////
//
// GridPix class.
//
////////////////////////////////////////



class GridPix : public TObject
{
  //class parameters.. maybe they should be private..
public:
  const static unsigned int addspace = 8;
  const static unsigned int thrshift = 4;
  const static unsigned int pixelspercolumn = 256;

  //typedef and forward declarations.. will they (or part of..) become private?
public:
  typedef unsigned char pixel_t;
  typedef short value_t;
  enum opresult 
    { errgeneric = -2000000000,
      errbaddata = -10,
      errduplicate = -1, 
      noop = 0, 
      maxop  = 2000000000 /*numeric_limits<int>::max()*/ 
    };
  //  class datachunk; //forward declaration
  class SortingTools; //forward declaration

  //Only-once class initializier (must be private):
public:
  static bool classinitdone; //!
  static void GridPixClassInit();

  //will they (or part of..) become private?
public: 
  int      nhits; // it is signed because it seems that ROOT
		  // persistency prefers that
  pixel_t* x; //[nhits]
  pixel_t* y; //[nhits]
  value_t* t; //[nhits]

  //transient members.. they should be private..
public:
  size_t      n_allocated;   //!
  int         n_datachunks; //!

  /////////////////////////////////////////
  //
  // internal class GridPix::datachunk
  // (unluckily I cannot just forward declare it because the GridPix
  // class destructor wants to call the datachunk class destructor,
  // that must be defined here.. it seems a C++ Standard Holy Rule..)
  //
  ////////////////////////////////////////
  struct datachunk
  {
    friend class Gridpix;
    
    //all of this class must be private
  public:
    size_t   nhitshere;
    pixel_t* x;
    pixel_t* y;
    value_t* t;

    datachunk* next;
    size_t     n_slots;
    bool       mustdelete;
    
    //NO DEFAULT CONSTRUCTOR
    explicit datachunk(size_t nhits, 
		       size_t n_alloc, bool do_alloc,
		       pixel_t* xlink, pixel_t* ylink, 
		       value_t* tlink,
		       datachunk* linkafter = 0);
    ~datachunk();

    void ShiftAndPlace(size_t thepos, 
		       pixel_t newx, pixel_t newy, value_t newt);
  };

  struct datachunk *firstchunk;         //!
  //
  /////////////////////////////////////////
  //


  //Constructors and Destructors
public:
  GridPix() : nhits(0), x(0), y(0), t(0)
	    , n_allocated(0), n_datachunks(0)
	    , firstchunk (0) {}
  explicit GridPix(int nhits, 
		   pixel_t* xdata = 0, pixel_t* ydata = 0, 
		   value_t* tdata = 0);

  virtual ~GridPix();

  //Methods
public:

  virtual int Insert(pixel_t newx, pixel_t newy, value_t newt);
  virtual int ClearData();
  virtual int RepackData();

  // Add member functions that manipulate the data
	
  double CountIf(pixel_t xmin, pixel_t xmax,
		 pixel_t ymin, pixel_t ymax,
		 value_t tmin, value_t tmax);

  // -----
	
  //internal methods (will become private)
public:

  //ROOT Dictionary and I/O
  ClassDef(GridPix,1);

};



/////////////////////////////////////////////////////////
//
// internal class SortingTools
//
// a collection of static functions for sorting
//
//
/////////////////////////////////////////////////////////


class GridPix::SortingTools
{
public:
  typedef int sortindex_t;
  
  static sortindex_t GetSortIndex(pixel_t x, pixel_t y) 
  { return y * pixelspercolumn + x; } //Sorting criterium  

  typedef int pos_t;
  static const pos_t afteranyother = -10, 
    undefinedpos = -3, alreadyexists = -2, 
    emptylist = -1, firstgoodpos = 0;

  static pos_t FindThePlace(sortindex_t newindex, 
			    size_t n, pixel_t* x, pixel_t* y);
};



/* Some test command lines:

GridPix::pixel_t x[10] = {3,5,1,100,180,11,0,100,255,250}, y[10]={0,0,1,2,2,10,35,120,200,255}; GridPix::value_t t[10]={10,11,12,13,14,15,16,17,18,19}
GridPix a(10,x,y,t)
a.Insert(0,0,222)
GridPix::SortingTools::FindThePlace(613,a.nhits, a.x, a.y)

*/
