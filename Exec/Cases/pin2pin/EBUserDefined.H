#ifndef _EBUSERDEFINED_H_
#define _EBUSERDEFINED_H_

using namespace amrex;

#ifdef AMREX_USE_EB
#include <AMReX_ParmParse.H>
#include <AMReX_EB2.H>
#include <AMReX_EB2_IF.H>


std::string convertIntGG(int number);
std::string
convertIntGG(int number)
{
  std::stringstream ss; // create a stringstream
  ss << number;         // add number to the stream
  return ss.str();      // return a string with the contents of the stream
}

void
EBUserDefined(const Geometry& geom,
              const int required_coarsening_level,
              const int max_coarsening_level)
{
    // ParmParse your geometry parameters
    // --> fixed geometry of the challenge problem
    //
  
      // FIXME : UPDATE MAX COARSENING LEVEL FOR THIS CASE
      //setting some constants
      //we can only do a maximum of 2 pins (change if needed)
      const int max_pin=2;
      
      //number of user defined pins
      int num_pin;
      
      amrex::ParmParse pp("pins");
      amrex::Vector<amrex::Array<amrex::Real,AMREX_SPACEDIM>> allpin(max_pin);

      //initalize pins with some dummy values
      //that fall outside of the domain
      //
      const amrex::Real *problo,*probhi;
      amrex::Real maxlen;

      problo=geom.ProbLo();
      probhi=geom.ProbHi();

      maxlen=std::max(std::max(geom.ProbLength(0),geom.ProbLength(1)),geom.ProbLength(2));

      //setting pins to be way outside the domain initially
      for(int ipin=0;ipin<max_pin;ipin++)
      {
         allpin[ipin][0] = problo[0]-100.0*maxlen;
         allpin[ipin][1] = problo[1]-100.0*maxlen;
         allpin[ipin][2] = problo[2]-100.0*maxlen;
      }
            
      //get user defined number of pins
      pp.get("num_pin", num_pin);

      amrex::Vector <std::unique_ptr<amrex::EB2::PinIF>> impfunc_pins(max_pin);
      for(int ipin = 0; ipin < num_pin; ipin++)
      {
          amrex::Array<amrex::Real,AMREX_SPACEDIM> tip{0.0,0.0,0.0};

          std::string  tipstr = "pin_" + convertIntGG(ipin) + "_tip"; 
          std::string  cstr = "pin_" + convertIntGG(ipin) + "_c"; 
          std::string  dirstr = "pin_" + convertIntGG(ipin) + "_dir"; 
          std::string  rstr = "pin_rmax";
          amrex::Vector<amrex::Real> vectip;
          amrex::Real  pinc;
          amrex::Real  rmax;
          int  pindir;
          pp.getarr(tipstr.c_str(), vectip,  0, AMREX_SPACEDIM);
          pp.get(cstr.c_str(), pinc);
          pp.get(dirstr.c_str(), pindir);
          pp.get(rstr.c_str(), rmax);
          for(int idir = 0; idir < AMREX_SPACEDIM; idir++)
          {
              tip[idir] = vectip[idir] ;
          }
          allpin[ipin] = tip;

          impfunc_pins[ipin] = std::unique_ptr<amrex::EB2::PinIF>
                              (new amrex::EB2::PinIF(pinc, allpin[ipin], pindir, rmax, false));
      }

      auto allpin_IF = amrex::EB2::makeUnion(*impfunc_pins[0],*impfunc_pins[1]);

      auto gshop = amrex::EB2::makeShop(allpin_IF);
      //amrex::EB2::Build(gshop, geom, max_level, max_level);
      amrex::EB2::Build(gshop, geom, required_coarsening_level, max_coarsening_level);
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void
setEBState(const amrex::Real * /*xEBface[AMREX_SPACEDIM]*/,
           amrex::Real s_ext[NVAR],
           const amrex::Real /*time*/,
           amrex::GeometryData const& /*geomdata*/,
           ProbParm const& prob_parm)
{
  s_ext[TEMP] = prob_parm.Twall;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void
setEBType(const amrex::Real xEBface[AMREX_SPACEDIM],
          amrex::Real & EBflagType,
          amrex::GeometryData const& /*geomdata*/,
          ProbParm const& /*prob_parm*/)
{
//  if ( xEBface[0] > 0.0098 ) {
    EBflagType = 1.0;
//  } else {
//    EBflagType = 0.0;
//  }
}



#endif
#endif
