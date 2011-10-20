#ifndef MATRIXOPERATOR_H
#define MATRIXOPERATOR_H
#pragma once

#include "vector_td_utilities.h"

template <class REAL, class ARRAY_TYPE> class matrixOperator
{
 public:

  matrixOperator() { weight_ = get_one<REAL>(); }
  virtual ~matrixOperator() {}

  inline void set_weight( REAL weight ){ weight_ = weight; }
  inline REAL get_weight(){ return weight_; }

  virtual int mult_M( ARRAY_TYPE* in, ARRAY_TYPE* out, bool accumulate = false) = 0;
  virtual int mult_MH( ARRAY_TYPE* in, ARRAY_TYPE* out, bool accumulate = false) = 0;
  virtual int mult_MH_M( ARRAY_TYPE* in, ARRAY_TYPE* out, bool accumulate = false) = 0;
  
  void* operator new (size_t bytes) { return ::new char[bytes]; }
  void operator delete (void *ptr) { delete [] static_cast <char *> (ptr); } 
  void * operator new(size_t s, void * p) { return p; }

private:
  REAL weight_;
};

#endif //MATRIXOPERATOR_H