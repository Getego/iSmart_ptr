/*----------------------------------------------------------------------------------------------*/
/*      Autor: Georgios Terzoglou                                                               */
/*      Copyright (©) 2013 by Georgios Terzoglou                                                */
/*      Bug-reports and suggestions to georgios.terzoglou@googlemail.com                        */
/*                                                                                              */
/*                                                                                              */
/*      GENERAL NOTE: I assume no liability for any damages with this header.                   */
/*      Using this header at your own risk.                                                     */
/*                                                                                              */
/*                                                                                              */
/*      Template class: iSmart_ptr                                                              */
/*                                                                                              */
/*                                                                                              */
/*      State:		Release candidate [RC]                                                  */
/*      Date:		03.08.2013 [Last changes]                                               */
/*                                                                                              */
/*----------------------------------------------------------------------------------------------*/

#ifndef TEST_TESTHELPER_ISMART_PTR_H_
#define TEST_TESTHELPER_ISMART_PTR_H_

#include <ostream>

namespace GT
{
  template<class Base, class Derived> class is
  {
    private:
      struct yes{char nyes;};
      struct no{char _n[2];};

      static yes test_derived(Base);
      static no test_derived(...);

    public:
      static const bool derived = sizeof(test_derived(Derived())) == sizeof(yes);
  };

  template<class T> class Hub
  {
    public:
      size_t digit_; T *ptr_;
      inline Hub(T *ptr): digit_(1), ptr_(ptr){}
      inline Hub(const Hub &copy)
      {
        this->ptr_ = copy.ptr_;
        this->digit_ = copy.digit_;
      }
      ~Hub(){delete this->ptr_;}
  };

  template<typename type, size_t index> struct array_exp
  {
      type *ref_data;
      array_exp(type *ref, const type &value): ref_data(ref)
      {
        ref_data[index] = value;
      }
      array_exp<type, index + 1> operator,(const type& value)
      {
        return array_exp<type, index + 1>
            (ref_data , value);
      }
  };

  template <class T> class iSmart_ptr
  {
    private: Hub<T> *husk; bool excep;
    public: // ctor of iSmart_ptr(T *p) [p != NULL]
      inline explicit iSmart_ptr(): husk(new Hub<T>(new T)), excep(true){}
      inline explicit iSmart_ptr(const T &s): husk(new Hub<T>(new T(s))), excep(true){}
      inline explicit iSmart_ptr(T *p): husk(new Hub<T>(p = (p != NULL) ? p:new T)), excep(true){}

      template<class U>
      inline iSmart_ptr(const iSmart_ptr<U> &rhs): husk(nullptr), excep(false)
      {
        if(is<T, U>::derived) // Check if U is a derivation of T
        {
          // Assigns a new value to the iSmart_ptr
          this->assign(new T(*rhs));
        }
      }

      inline iSmart_ptr(iSmart_ptr<T> &rhs): husk(new Hub<T>(new T)), excep(true)
      {
        delete this->husk->ptr_;
        this->husk->ptr_ = rhs.husk->ptr_;
        this->husk->digit_ = rhs.husk->digit_+1;
      }

      ~iSmart_ptr()
      {
        if(--this->husk->digit_ == 0)
          if(this->excep) delete this->husk;
      }

      inline void swap(iSmart_ptr<T> &rhs)
      {
        T *temp_ptr = rhs.husk->ptr_;
        size_t temp_value = rhs.husk->digit_;
        rhs.husk->digit_ = this->husk->digit_;
        rhs.husk->ptr_ = this->husk->ptr_;
        this->husk->digit_ = temp_value;
        this->husk->ptr_ = temp_ptr;
      }

      inline void assign(T *p)
      {
        if(this->excep)
        {
          delete this->husk->ptr_;
          this->husk->ptr_ = p;
        }
        else this->husk = new Hub<T>(p);
        this->excep = true;
      }

      inline void free(void)
      {
        if(this->excep) delete this->husk;
        this->excep = false;
      }

      inline size_t size(void)
      {
        return size_t(sizeof(*this));
      }

      iSmart_ptr<T> inline &operator=(const iSmart_ptr<T> &rhs)
      {
        ++rhs.husk->digit_;
        if (--this->husk->digit_ == 0) delete this->husk;
        this->husk = rhs.husk;
        return *this;
      }

      iSmart_ptr<T> inline &operator=(iSmart_ptr<T> &rhs)
      {
        ++rhs.husk->digit_;
        if (--this->husk->digit_ == 0) delete this->husk;
        this->husk = rhs.husk;
        return *this;
      }

      friend std::ostream inline &operator<<(std::ostream &lhs, iSmart_ptr<T> const &rhs)
      {
        return lhs << rhs.husk->ptr_;
      }

      inline bool operator==(iSmart_ptr<T> &rhs)
      {
        if(*this != rhs)
          return false;
        else return true;
      }

      inline bool operator!=(iSmart_ptr<T> &rhs)
      {
        if(*this->husk->ptr_ == *rhs.husk->ptr_)
          return false;
        else return true;
      }

      inline operator T() const
      {
        return *this->husk->ptr_;
      }

      inline operator T*() const
      {
        return this->husk->ptr_;
      }

      template<class Y> inline operator Y() const
      {
        if(is<T, Y>::derived) // Check if Y is a derivation of T
          return Y(*this->husk->ptr_);
        else
        {
          // std::cerr << "IN FUNCTION: template<class Y> inline operator Y() const;" << std::endl;
          // std::cerr << "is<T, Y>::derived == false -> DATATYPE IS NOT COMPATIBLE!" << std::endl;
          iSmart_ptr<Y> tmp;
          return *tmp;
        }
      }

      template<class Y> inline operator Y*() const
      {
        if(is<T, Y>::derived) // Check if Y is a derivation of T
          return (Y *)(this->husk->ptr_);
        else
        {
          // std::cerr << "IN FUNCTION: template<class Y> inline operator Y*() const;" << std::endl;
          // std::cerr << "is<T, Y>::derived == false -> DATATYPE IS NOT COMPATIBLE!" << std::endl;
          return NULL;
        }
      }

      inline T **operator&() const{return &this->husk->ptr_;}
      inline T *operator->() const{return this->husk->ptr_;}
      inline T &operator*() const{return *this->husk->ptr_;}
  };
}
#endif // TEST_TESTHELPER_ISMART_PTR_H_
