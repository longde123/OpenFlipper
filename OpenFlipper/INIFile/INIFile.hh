/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




#ifndef TOOLS_INIFILE_HH
#define TOOLS_INIFILE_HH

#include <OpenFlipper/common/GlobalDefines.hh>
#include <QString>
#include <QFile>
#include <map>
#include <vector>


//! Class for the handling of simple configuration files.
/*! The file format used by this class is similar to that of MS
  Windows INI files. It is possible to divide a file into sections
  which are denoted by brackets (e.g. <tt>[Section]</tt>). All values
  are stored in the form <tt>\<key\> = \<value\></tt> and are assigned
  to a section. The INIFile class is able to store values of the types
  string, double, signed and unsigned int and bool.

  The usual way to work with this class consists of the following steps:

  - Open an existing or create a new INI file with a call to connect()

  - Check for the existence of entries (entry_exists()), access stored
    values (get_entry()) or add new entries (add_entry()). Note that
    the function add_section() is obsolete and only kept for backward
    compatibility. A new section is now implicitly generated by
    specifying it in a call to add_entry().

  - disconnect() from the INI file. Only now are all entries and
    sections written to the file.

  To be able to use the INIFile class, an object is created. After
  that the object is connected to a given filename. This means that,
  if the file exists, it is opened and the content is parsed and
  stored in memory. The second (boolean) parameter to the connect()
  function indicates that the file should be created if it does not
  exists.  \until }

  Now a new section named <tt>Main</tt> is generated and several
  entries are added. Note that the INI file is not written to disc
  until the disconnect() function is called. All additions and
  modifications are only performed on the stored data in memory.
  \until iniFile.disconnect();

  In the second part of this example the just created INI file is
  opened (connected) again and the created entries are read and printed
  to <tt>stdout</tt>.
  \until }
  \until }
  \until }

*/
class DLLEXPORT INIFile
{
public :

    //! Default constructor
    INIFile();

    //! Destructor
    ~INIFile();

    //! Connect INIFile object with given filename
    bool connect( const QString& name,
		  const bool create );

    //! Remove connection of this object to a file
    void disconnect();

    //! Check if object is connected to  file
    bool is_connected() const { return mf_isConnected; }

    //! Access to name of connected file
    const QString& name() const { return m_filename; }


    // ---------------------------------------- check if something is there

    /*! \name Existence checking */
    //@{

    //! Check if given section exists in the current INI file
    bool section_exists(const QString & _section) const;

    //! Check if given entry esists in the current INI file
    bool entry_exists(const QString & _section, const QString & _key) const;

    //! Same as entry_exists() (for backward compatibility)
    bool entry_in_section(const QString & _section,
			  const QString & _key) const
	{ return entry_exists( _section, _key ); }

    // ---------------------------------------- section manipulation

    /*! \name Addition of entities */
    //@{

    //! Addition of a section
    /* \deprecated This function is not needed any more. New sections are
       implicitly created upon calls to add_entry().
    */
    void add_section(const QString & _sectionname);

    //! Addition / modification of a string entry
    void add_entry( const QString & _section,
		    const QString & _key,
		    const QString & _value );

    //! Addition / modification of a string entry, given as char array
    void add_entry( const QString & _section,
		  const QString & _key,
		  const char * _value )
	{ add_entry( _section, _key, QString(_value) ); }

    //! Addition / modification of a double entry
    void add_entry( const QString & _section,
		    const QString & _key,
		    const double & _value);

    //! Addition / modification of a double entry
    void add_entry( const QString & _section,
		    const QString & _key,
		    const float & _value);

    //! Addition / modification of an int entry
    void add_entry( const QString & _section,
		    const QString & _key ,
		    const int & _value);

    //! Addition / modification of an int entry
    void add_entry( const QString & _section,
		    const QString & _key ,
		    const unsigned int & _value);

    //! Addition / modification of a boolean entry
    void add_entry( const QString & _section,
		    const QString & _key ,
		    const bool & _value);

    //! Addition / modification of a float vector
    void add_entry( const QString & _section,
		    const QString & _key ,
		    const std::vector<float> & _value);

    //! Addition / modification of a double vector
    void add_entry( const QString & _section,
                    const QString & _key ,
                    const std::vector<double> & _value);

    //! Addition of a Vec_n_something
    template < typename VectorT >
    void add_entryVec ( const QString & _section,
                        const QString & _key,
                        const VectorT  & _value);

    //! Addition of a vector of Vec_n_something
    template < typename VectorT >
    void add_entryVec ( const QString & _section,
                        const QString & _key,
                        const std::vector< VectorT > & _value);

    //! Addition / modification of an int vector
    void add_entry( const QString & _section,
          const QString & _key ,
          const std::vector<int> & _value);

    //! Addition / modification of an int vector
    void add_entry( const QString & _section,
          const QString & _key ,
          const std::vector<QString> & _value);

    //! Addition / modification of an int vector
    void add_entry( const QString & _section,
          const QString & _key ,
          const QStringList & _value);


    //@}


    // -------------------- delete

    /*! \name Deletion of entities */
    //@{

    //! Deletion of an entry
    void delete_entry( const QString & _section, const QString & _key);

    //! Deletion of an entire section
    void delete_section( const QString & _sectionname );

    //@}



    // -------------------- retrieval

    /*! \name Access to stored entries */
    //@{

    //! Access to a string entry
    bool get_entry( QString & _val,
		    const QString & _section,
		    const QString & _key ) const;

    //! Access to a floating point entry, stored as double value
    bool get_entry( double & _val,
		    const QString & _section,
		    const QString & _key ) const;

    //! Access to a floating point entry, stored as float value
    bool get_entry( float & _val,
		    const QString & _section,
		    const QString & _key ) const;

    //! Access to an int entry
    bool get_entry( int & _val,
		    const QString & _section,
		    const QString & _key ) const;

    //! Access to an unsigned int entry (which is actually stored as int!!!)
    bool get_entry( unsigned int & _val,
		    const QString & _section,
		    const QString & _key ) const;

    //! Access to a boolean entry
    bool get_entry( bool & _val,
		    const QString & _section,
		    const QString & _key) const;

    //! Access to a float vector
    bool get_entry( std::vector<float> & _val,
		    const QString & _section,
		    const QString & _key) const;

    //! Access to a double vector
    bool get_entry( std::vector<double> & _val,
                    const QString & _section,
                    const QString & _key) const;

    //! Access to an int vector
    bool get_entry( std::vector<int> & _val,
          const QString & _section,
          const QString & _key) const;

    //! Access to a string vector
    bool get_entry( std::vector<QString> & _val,
          const QString & _section,
          const QString & _key ) const;

    //! Access to a stringList
    bool get_entry( QStringList & _val,
          const QString & _section,
          const QString & _key ) const;

    //! Get a Vec_n_d (double)
    template < typename VectorT >
    bool get_entryVecd ( VectorT & _val ,
                         const QString & _section,
                         const QString & _key ) const;

        //! Get a Vec_n_i (int)
    template < typename VectorT >
    bool get_entryVecf ( VectorT & _val ,
                         const QString & _section,
                         const QString & _key ) const;

    //! Get a Vec_n_i (int)
    template < typename VectorT >
    bool get_entryVeci ( VectorT & _val ,
                         const QString & _section,
                         const QString & _key ) const;

    //! Get a Vec_n_d
    template < typename VectorT >
    bool get_entryVecd (std::vector< VectorT > & _val ,
                       const QString & _section,
                       const QString & _key ) const;

    //! Get a Vec_n_f
    template < typename VectorT >
        bool get_entryVecf (std::vector< VectorT > & _val ,
                           const QString & _section,
                           const QString & _key ) const;

    //! Get a Vec_n_i
    template < typename VectorT >
    bool get_entryVeci (std::vector< VectorT > & _val ,
                        const QString & _section,
                        const QString & _key ) const;


private: // data


    //! Type for map of contained entries
    typedef std::map< QString, QString > EntryMap;


    //! Type for map of contained sections
    typedef std::map< QString, EntryMap > SectionMap;


    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    //! Read content of an INI file
    bool parseFile( QFile & _inputStream );

    //! Write data to file we are currently connected to
    bool writeFile( void );

    //! Name of current INI file
    QString m_filename;

    //! Flag: this object is connected to an INI file
    bool mf_isConnected;


    //! Stored data of an INI file
    SectionMap m_iniData;
};


#if defined(INCLUDE_TEMPLATES) && !defined(INIFILE_C)
#define INIFILE_TEMPLATES
#include "INIFileT.cc"
#endif

#endif
