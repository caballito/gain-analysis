

//#define  READ2_CC_DEBUG

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TString.h"

#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <cstdio>

//using namespace std ;

#include "GridPix.hpp"
#include "Waveform.hpp"
#include "LecroyWFDecoder.hpp"

/* Define which data is valid (Now for 1 chip, not 4!) */

inline bool datavalid(int x, int y, int t)
{
  return ( (x>=0) && (x<255) && (y>=0) && (y<255)
	   && (t>=0) && (t<12000) );
}

/* Define root tree header (additional info) */

class EventHeader
{
public:
  bool isvalid;
  struct _databuf
  {
    int evcount;
    int evid;
    double date;
    double frame;
    unsigned int errors;
  };

  struct _databuf databuf;

public:
  EventHeader (int firstcount = 0) : isvalid(false)
  {
    databuf.evcount = firstcount;
  }

  void Reset(int firstcount = 0)
  {
    databuf.evcount = firstcount;
  }
  
  /* If header is invalid filled -1, if isvalid is true return databuf.evcount, (compressed if else statement -> ?) */
  int GetCount() const { return (isvalid ? databuf.evcount : -1); }
  /* Function that retrieves pointer to the data buffer */
  void* GetDataBuffer() { return &(databuf.evcount); }
  /* Tells how the structure is defined */
  const char* GetDataDescription() const { return "evcount/I:evid/I:date/D:frame/D:errors/i"; }
  
  void Update(char* headerstr);
};

void EventHeader::Update(char* headerstr)
{
  isvalid = false;
  databuf.evcount++;
  databuf.errors = 0;
 
  /* Read in the HEADER assume it wrong, till everything went fine and isvalid becomes true otherwise return */

  const char* keyword[3] = {"Eventnr ", "Start time : ", "Acq time : "}; 
  size_t keywordsize[3] = {8, 13, 11};

  //Eventnr
  char* pos = std::strstr(headerstr, keyword[0]);
  if (!pos || 
      std::sscanf( &(pos[keywordsize[0]]), "%d", &databuf.evid) != 1)
    {
      std::cout << "Problems finding the pattern '" << keyword[0] 
		<< "' in the header string:\n"
		<< headerstr << std::endl;
      return;
    }

  //Start time
  pos = std::strstr(headerstr, keyword[1]);  
  if (!pos || 
      std::sscanf( &(pos[keywordsize[1]]), "%lg", &databuf.date) != 1)
    {
      std::cout << "Problems finding the pattern '" << keyword[1] 
		<< "' in the header string:\n"
		<< headerstr << std::endl;
      return;
    }
  //Acq time
  pos = std::strstr(headerstr, keyword[2]);
  if (!pos || 
      std::sscanf( &(pos[keywordsize[2]]), "%lg", &databuf.frame) != 1)
    {
      std::cout << "Problems finding the pattern '" << keyword[2]
		<< "' in the header string:\n"
		<< headerstr << std::endl;
      return;
    }


  isvalid = true;
}

/* MAIN FUNCTION */

/* Directory + filename ; output name ; number of events in filename */

int readrawdata(char * relaxdfilepath, char* WFfilespattern, char * outrootfile, int readmaxevent )
{
  //Parameters
  const int averagehits = 30000;

  //Opening files..

  std::ifstream relaxddata(relaxdfilepath);

  if (!relaxddata.is_open())
    {
      std::cout << "\nThe relaxd data file does not exists!" << std::endl ;
      return 0;
    }
  else
    {
      std::cout << "\nReading data from the file " 
		<< relaxdfilepath << "\n";
    }

  TFile rootf(outrootfile,"RECREATE");

  if (!rootf.IsOpen())
    {
      std::cout << "The output root file cannot be opened! (Check path and writing rights!)\n" 
		<< std::endl;
      return 0;
    }
  else
    {
      std::cout << "The tree will be saved in " 
		<< outrootfile << "\n" << std::endl;
    }

  TH2F hpixcount("hpixcount","Integral of counts on pixels", 256, -0.5, 255.5, 256, -0.5, 255.5);
  TH2F hbadpix("hbadpix","Occurrences of invalid values on pixels", 256, -0.5, 255.5, 256, -0.5, 255.5);
 
/* Define tree */
  TTree tree("tree","GridPix data tree");

/* Create class Eventheader */
  class EventHeader eventheader;
  //EventHeader eventheader;

/* GridPix and Waveform classes, pointers to them because they are going to be in the tree */
  GridPix* det1 = new GridPix(averagehits);
  Waveform* wf = new Waveform();

/* Create tree's branches */
  tree.Branch("Header",eventheader.GetDataBuffer(), eventheader.GetDataDescription());
  tree.Branch("det1",&det1);
  tree.Branch("wf",&wf);

/* Waveform decoder */
  LecroyWFDecoder lecroydec;


  /////////////////////////////////////////
  //
  // MAIN LOOP OVER EVENTS
  //
  //////////////////////////////////////////
	
	TH1D* hTime= new TH1D("hTime","time spectrum;time (ToA counts);entries",11811,0,11811);

  char c = 0;
  while ( (relaxddata.get(c)).good() )
    {
      //EACH CYCLE THE FIRST CHARACTER OF THE LINE IS TESTED (WHITESPACE -> SKIP, '#' -> HEADER, DIGIT -> DATA)

      if (std::isspace(c)) continue; //NEXT CYCLE

      //IS IT THE HEADER?
      if (c == '#' && relaxddata.get(c) && c == '-')
	{
	  char dummy[100];
	  if ( !relaxddata.getline(&(dummy[0]),100).good()) { break ; } ;
	  char dataline[10000];
	  size_t npos = 0 ; // WHY WAS `relaxddata.gcount()-1 ;` ?!?!?
	  for (int i=1; i<10; i++)
	    {
	      relaxddata.getline(&(dataline[npos]),1000) ;
	      npos += relaxddata.gcount()-1 ;
	      
	    }

	  // Fill the previous event
	  if (eventheader.GetCount() >= 0)
	    {
	      /*Readout the WF of the previous event*/
	      char filenameWF[1000]; sprintf(filenameWF, WFfilespattern, eventheader.GetCount());
	      lecroydec.ChangeFile(filenameWF);
	      if ( ! wf->ReadEncodedData(lecroydec) ) {
		std::cout << "ERROR: Cannot access waveform file " << filenameWF << std::endl;
		break;
	      }
	  
	      /* Repack gridpix data, due to usaged of SPLIT */
	      det1->RepackData();

	      /*Now that every information on the previous event is ready, fill the tree*/
	      tree.Fill();
	    }
	  
	  // Some printout every bunch of event
	  if (eventheader.GetCount() % 10 == 0)
	    {
	      /* \r to rewrite the same line (instead of \n. On mac this is different */
	      std::cout << "Processed " << eventheader.GetCount() << " events so far\r" << std::flush ;
	    }
	  
	  // Prepare for the next event
	  eventheader.Update(dataline);
	  det1->ClearData();
	  continue; //NEXT CYCLE
	}
      
      // IS IT A DIGIT OF DATA? LET'S ASSUME THAT MANY DATA ARE
      // FOLLOWING AND LOOP UNTIL A NON-DIGIT CHARACTER IS FOUND
      if (std::isdigit(c))
	{
	  /* Putback the already extracted data point */	
	  relaxddata.putback(c); //the digit is part of the data!!
	  
	  int x,y,t;
	  while ( ( relaxddata >> x >> y >> t ).good() )  
	    {
#ifdef READ2_CC_DEBUG
	      std::cout << "\nRead a hit: " << x << "\t" << y 
			<< "\t" << t << std::flush;
	      //sleep(1);
#endif
	      /* IF VALID insert data into gridpix object (data is saved when a new header is found) */
	      if (datavalid(x,y,t))
		{
		  det1->Insert(x,y,t);
		  if (t<11000) hTime->Fill(t);	// Already some simple cut on early events.
		  hpixcount.Fill(x,y);
		}
	      else  hbadpix.Fill(x,y);
	    }
	  
	  // can be !good() because a '#' (i.e. a new header) is found
	  // instead of a digit(), so the error must be reset and the
	  // main cycle will check it again
	  relaxddata.clear();
	}
      
      //AN ESCAPE LANE IN CASE OF BUGS..
      if (eventheader.GetCount() >= readmaxevent) break;
    }
  
//! EDIT Timespectra Histogram Stuff
//		Double_t max=hTime->GetMaximumBin();
//		TF1 *fit=new TF1("fit","gaus",max-50.,max+50.);
//		hTime->Fit("fit", "R&&Q");
//
//		ofstream cuts;
//		cuts.open("/localstore/detrd/data/ConvRelaxdData/Root/Timespectra/cuts.txt", ios::app);
//		
//		cuts<<"\nPressure/Voltage:\t"<<timespname<<"\n*****************\n"<<std::endl;
//		cuts<<"Mean:\t\t"<<fit->GetParameter(1)<<std::endl;
//		cuts<<"Sigma:\t\t"<<fit->GetParameter(2)<<std::endl;
//		cuts<<"\n\ttHigh:\t\t"<<fit->GetParameter(1)+5*fit->GetParameter(2)<<std::endl;
//		cuts<<"\ttlow:\t\t"<<fit->GetParameter(1)-5*fit->GetParameter(2)<<std::endl;
//		
//		cuts.close();
  //SAVE LAST EVENT (if valid)
  if (eventheader.GetCount() >= 0) 
    {
      /*Readout the WF of the previous event*/
      char filenameWF[1000]; sprintf(filenameWF, WFfilespattern, eventheader.GetCount());
      lecroydec.ChangeFile(filenameWF);
      wf->ReadEncodedData(lecroydec);
      det1->RepackData();
      tree.Fill();
    }
  

  std::cout << "DONE! Processed " << eventheader.GetCount() << " events\n" << std::endl;

  tree.Print();
  tree.Write();
  hbadpix.Write();
  hpixcount.Write();

  Double_t max=hTime->GetMaximumBin();
  TF1 *fit=new TF1("fit","gaus",max-50.,max+50.);
  hTime->Fit(fit, "R&&Q");
  hTime->Write();

  rootf.Save();

  return eventheader.GetCount();
}


  /************************************************************** 
     YOU CAN THEN REOPEN THE FILE AND TRY THE FOLLOWING IN THE COMMAND
     LINE:


     tree1.Print()
     tree1.Scan("det1_nhit:det1_x:det1_y:det1_t")
     tree1.Scan("n:x:y:t")
     tree1.Draw("x:y:t")
     tree1.Draw("x:y","t","lego2")
     tree1.Draw("x:y","t*(X>50)","lego2")
     tree1.Draw("x:y","t*(x>50)","lego2")
     tree1.Draw("x:y","","box")

     tree2.Print()
     tree2.Scan("det1")
     tree2.Scan("det1.first:det1.second")
     tree2.Scan("int(det1.first/256):det1.first%256:det1.second")
     tree2->SetAlias("det1x","det1.first%256")
     tree2->SetAlias("det1y","int(det1.first/256)")
     tree2.Scan("det1y:det1x:det1.second")
     tree2.Scan("det1y:det1x:det1.second","det1y==15")
     tree2.Scan("det1y:det1x:det1.second","det1x==11")

  ***************************************************************/
