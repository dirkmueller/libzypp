/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/**
 * \file zypp/url/UrlException.h
 */
#ifndef   ZYPP_URL_URLEXCEPTION_H
#define   ZYPP_URL_URLEXCEPTION_H

#include <zypp-core/base/Exception.h>


//////////////////////////////////////////////////////////////////////
namespace zypp
{ ////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////
  namespace url
  { //////////////////////////////////////////////////////////////////


    // ---------------------------------------------------------------
    /**
     * Base class for all URL exceptions.
     */
    class ZYPP_API UrlException: public zypp::Exception
    {
    public:
      UrlException()
        : zypp::Exception("Url exception")
      {}

      UrlException(const std::string &msg)
        : zypp::Exception(msg)
      {}

      ~UrlException() throw() override {};
    };

    // ---------------------------------------------------------------
    /**
     * Thrown if the encoded string contains a NUL byte (%00).
     */
    class ZYPP_API UrlDecodingException: public UrlException
    {
    public:
      UrlDecodingException()
        : UrlException("Url NUL decoding exception")
      {}

      UrlDecodingException(const std::string &msg)
        : UrlException(msg)
      {}

      ~UrlDecodingException() throw() override {};
    };

    // ---------------------------------------------------------------
    /**
     * Thrown if the url or a component can't be parsed at all.
     */
    class ZYPP_API UrlParsingException: public UrlException
    {
    public:
      UrlParsingException()
        : UrlException("Url parsing failure exception")
      {}

      UrlParsingException(const std::string &msg)
        : UrlException(msg)
      {}

      ~UrlParsingException() throw() override {};
    };

    // ---------------------------------------------------------------
    /**
     * Thrown if a url component is invalid.
     */
    class ZYPP_API UrlBadComponentException: public UrlException
    {
    public:
      UrlBadComponentException()
        : UrlException("Url bad component exception")
      {}

      UrlBadComponentException(const std::string &msg)
        : UrlException(msg)
      {}

      ~UrlBadComponentException() throw() override {};
    };


    // ---------------------------------------------------------------
    /**
     * Thrown if scheme does not allow a component.
     */
    class ZYPP_API UrlNotAllowedException: public UrlException
    {
    public:
      UrlNotAllowedException()
        : UrlException("Url not allowed component exception")
      {}

      UrlNotAllowedException(const std::string &msg)
        : UrlException(msg)
      {}

      ~UrlNotAllowedException() throw() override {};
    };


    // ---------------------------------------------------------------
    /**
     * Thrown if a feature e.g. parsing of a component
     * is not supported for the url/scheme.
     */
    class ZYPP_API UrlNotSupportedException: public UrlException
    {
    public:
      UrlNotSupportedException()
        : UrlException("Url parsing unsupported exception")
      {}

      UrlNotSupportedException(const std::string &msg)
        : UrlException(msg)
      {}

      ~UrlNotSupportedException() throw() override {};
    };


    //////////////////////////////////////////////////////////////////
  } // namespace url
  ////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////
} // namespace zypp
//////////////////////////////////////////////////////////////////////

#endif /* ZYPP_URL_URLEXCEPTION_H */
/*
** vim: set ts=2 sts=2 sw=2 ai et:
*/
