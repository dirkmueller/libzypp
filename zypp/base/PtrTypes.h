/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp/base/PtrTypes.h
 *  \ingroup ZYPP_SMART_PTR
 *  \see ZYPP_SMART_PTR
*/
#ifndef ZYPP_BASE_PTRTYPES_H
#define ZYPP_BASE_PTRTYPES_H

#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////

    /** \defgroup ZYPP_SMART_PTR Smart pointer types
     *  Smart pointer types.
     *
     * Namespace zypp provides 3 smart pointer types \b using the
     * boost smart pointer library.
     *
     * \li \c scoped_ptr Simple sole ownership of single objects. Noncopyable.
     *
     * \li \c shared_ptr Object ownership shared among multiple pointers
     *
     * \li \c weak_ptr Non-owning observers of an object owned by shared_ptr.
     *
     * And \ref zypp::RW_pointer, as wrapper around a smart pointer,
     * poviding \c const correct read/write access to the object it refers.
    */
    /*@{*/

    /** \class scoped_ptr scoped_ptr */
    using boost::scoped_ptr;

    /** \class shared_ptr */
    using boost::shared_ptr;

    /** \class weak_ptr */
    using boost::weak_ptr;

    /** \class intrusive_ptr */
    using boost::intrusive_ptr;

    /**  */
    using boost::static_pointer_cast;
    /**  */
    using boost::const_pointer_cast;
    /**  */
    using boost::dynamic_pointer_cast;

    ///////////////////////////////////////////////////////////////////
    //
    //	RW_pointer traits
    //
    ///////////////////////////////////////////////////////////////////
    /**
     * Don't forgett to provide versions for _Ptr and _constPtr,
     * esp. if creation a of temporary is not acceptable (eg. when
     * checking the ref count value).
    */
    namespace rw_pointer {

      template<class _D>
        struct Shared
        {
          typedef shared_ptr<_D>       _Ptr;
          typedef shared_ptr<const _D> _constPtr;
          /** Check whether pointer is not shared. */
          bool unique( const _constPtr & ptr_r )
          { return ptr_r.unique(); }
          bool unique( const _Ptr & ptr_r )
          { return ptr_r.unique(); }
          /** Return number of references. */
          long use_count( const _constPtr & ptr_r ) const
          { return ptr_r.use_count(); }
          long use_count( const _Ptr & ptr_r ) const
          { return ptr_r.use_count(); }
        };

      template<class _D>
        struct Intrusive
        {
          typedef intrusive_ptr<_D>       _Ptr;
          typedef intrusive_ptr<const _D> _constPtr;
          /** Check whether pointer is not shared. */
          bool unique( const _constPtr & ptr_r )
          { return !ptr_r || (ptr_r->refCount() <= 1); }
          bool unique( const _Ptr & ptr_r )
          { return !ptr_r || (ptr_r->refCount() <= 1); }
          /** Return number of references. */
          long use_count( const _constPtr & ptr_r ) const
          { return ptr_r ? ptr_r->refCount() : 0; }
          long use_count( const _Ptr & ptr_r ) const
          { return ptr_r ? ptr_r->refCount() : 0; }
        };
    }
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : RW_pointer
    //
    /** Wrapper for \c const correct access via \ref ZYPP_SMART_PTR.
     *
     * zypp::RW_pointer<tt>\<_D,_Traits></tt> stores a \ref ZYPP_SMART_PTR
     * of type \c _Traits::_Ptr, which must be convertible into a <tt>_D *</tt>.
     * Pointer style access (via \c -> and \c *) offers a <tt>const _D *</tt> in const
     * a context, otherwise a <tt>_D *</tt>. Thus \em RW_ means \em read/write,
     * as you get a different type, dependent on whether you're allowed to
     * read or write.
     *
     * Forwarding access from an interface to an implemantation class, an
     * RW_pointer prevents const interface methods from accidentally calling
     * nonconst implementation methods.
     *
     * The second template argument defaults to
     * <tt>_Traits = rw_pointer::Shared<_D></tt> thus wraping a
     * <tt>shared_ptr<_D></tt>. To wrap an <tt>intrusive_ptr<_D></tt>
     * use <tt>rw_pointer::Intrusive<_D></tt>.
     *
     * \see zypp::RWCOW_pointer for 'copy on write' functionality.
     *
     * \code
     * #include "zypp/base/PtrTypes.h"
     *
     * class Foo
     * {
     *   ...
     *   private:
     *     // Implementation class
     *     struct Impl;
     *     // Pointer to implementation; actually a shared_ptr<Impl>
     *     RW_pointer<Impl> _pimpl;
     *
     *     void baa()       { _pimpl->... } // is Impl *
     *     void baa() const { _pimpl->... } // is Impl const *
     * };
     * \endcode
    */
    template<class _D, class _Traits = rw_pointer::Shared<_D> >
      struct RW_pointer
      {
        typedef typename _Traits::_Ptr               _Ptr;
        typedef typename _Traits::_constPtr          _constPtr;
        typedef typename _Ptr::unspecified_bool_type unspecified_bool_type;

        explicit
        RW_pointer( typename _Ptr::element_type * dptr = 0 )
        : _dptr( dptr )
        {}

        explicit
        RW_pointer( _Ptr dptr )
        : _dptr( dptr )
        {}

        void reset()
        { _dptr.reset(); }

        void reset( typename _Ptr::element_type * dptr )
        { _dptr.reset( dptr ); }

        void swap( RW_pointer & rhs )
        { _dptr.swap( rhs._dptr ); }

        void swap( _Ptr & rhs )
        { _dptr.swap( rhs ); }

        operator unspecified_bool_type() const
        { return _dptr; }

        const _D & operator*() const
        { return *_dptr; };

        const _D * operator->() const
        { return _dptr.get(); }

        const _D * get() const
        { return _dptr.get(); }

        _D & operator*()
        { return *_dptr; }

        _D * operator->()
        { return _dptr.get(); }

        _D * get()
        { return _dptr.get(); }

      public:
        bool unique() const
	{ return _Traits().unique( _dptr ); }

	long use_count() const
	{ return _Traits().use_count( _dptr ); }

        _constPtr getPtr() const
        { return _dptr; }

        _Ptr getPtr()
        { return _dptr; }

      private:
        _Ptr _dptr;
      };
    ///////////////////////////////////////////////////////////////////

    /** \relates RW_pointer Stream output.
     *
     * Print the \c _D object the RW_pointer refers, or \c "NULL"
     * if the pointer is \c NULL.
    */
    template<class _D, class _Ptr>
      inline std::ostream &
      operator<<( std::ostream & str, const RW_pointer<_D, _Ptr> & obj )
      {
        if ( obj.get() )
          return str << *obj.get();
        return str << std::string("NULL");
      }

    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : RWCOW_pointer
    //
    /** \ref RW_pointer supporting 'copy on write' functionality.
     *
     * \em Write access to the underlying object creates a copy, iff
     * the object is shared.
     *
     * See \ref RW_pointer.
    */
    template<class _D, class _Traits = rw_pointer::Shared<_D> >
      struct RWCOW_pointer
      {
        typedef typename _Traits::_Ptr               _Ptr;
        typedef typename _Traits::_constPtr          _constPtr;
        typedef typename _Ptr::unspecified_bool_type unspecified_bool_type;

        explicit
        RWCOW_pointer( typename _Ptr::element_type * dptr = 0 )
        : _dptr( dptr )
        {}

        explicit
        RWCOW_pointer( _Ptr dptr )
        : _dptr( dptr )
        {}

        void reset()
        { _dptr.reset(); }

        void reset( typename _Ptr::element_type * dptr )
        { _dptr.reset( dptr ); }

        void swap( RWCOW_pointer & rhs )
        { _dptr.swap( rhs._dptr ); }

        void swap( _Ptr & rhs )
        { _dptr.swap( rhs ); }

        operator unspecified_bool_type() const
        { return _dptr; }

        const _D & operator*() const
        { return *_dptr; };

        const _D * operator->() const
        { return _dptr.get(); }

        const _D * get() const
        { return _dptr.get(); }

        _D & operator*()
        { assertUnshared(); return *_dptr; }

        _D * operator->()
        { assertUnshared(); return _dptr.get(); }

        _D * get()
        { assertUnshared(); return _dptr.get(); }

      public:
        bool unique() const
	{ return _Traits().unique( _dptr ); }

	long use_count() const
	{ return _Traits().use_count( _dptr ); }

        _constPtr getPtr() const
        { return _dptr; }

        _Ptr getPtr()
        { assertUnshared(); return _dptr; }

      private:

        void assertUnshared()
        {
          if ( !unique() )
            _dptr.reset( rwcowClone( _dptr.get() ) );
        }

      private:
        _Ptr _dptr;
      };
    ///////////////////////////////////////////////////////////////////

    /** \relates RWCOW_pointer Clone the underlying object.
     * Calls \a rhs <tt>-\>clone()</tt>. Being defined as a
     * function outside \ref RWCOW_pointer allows to overload
     * it, in case a specific \a _D does not have <tt>clone()</tt>.
    */
    template<class _D>
      inline _D * rwcowClone( const _D * rhs )
      { return rhs->clone(); }

    ///////////////////////////////////////////////////////////////////

    /** \relates RWCOW_pointer Stream output.
     *
     * Print the \c _D object the RWCOW_pointer refers, or \c "NULL"
     * if the pointer is \c NULL.
    */
    template<class _D, class _Ptr>
      inline std::ostream &
      operator<<( std::ostream & str, const RWCOW_pointer<_D, _Ptr> & obj )
      {
        if ( obj.get() )
          return str << *obj.get();
        return str << std::string("NULL");
      }

    ///////////////////////////////////////////////////////////////////

    /*@}*/

  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////

/** Forward declaration of Ptr types */
#define DEFINE_PTR_TYPE(NAME) \
class NAME;                                                      \
extern void intrusive_ptr_add_ref( const NAME * );               \
extern void intrusive_ptr_release( const NAME * );               \
typedef zypp::intrusive_ptr<NAME>       NAME##_Ptr;        \
typedef zypp::intrusive_ptr<const NAME> NAME##_constPtr;

///////////////////////////////////////////////////////////////////
#endif // ZYPP_BASE_PTRTYPES_H
