/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *           Copyright (c) 2001-2015, RWTH-Aachen University                 *
 *           Department of Computer Graphics and Multimedia                  *
 *                          All rights reserved.                             *
 *                            www.openflipper.org                            *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of OpenFlipper.                                         *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS QtFileDialog
//
//=============================================================================


#ifndef ACG_QTFILEDIALOG_HH
#define ACG_QTFILEDIALOG_HH


//== INCLUDES =================================================================

#include "../Config/ACGDefines.hh"
#include <QString>
#include <QWidget>

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================

ACGDLLEXPORT	      
QString
getOpenFileName(QWidget*        _parent  = 0, 
		const QString&  _caption = QString::null,
		const QString&  _filter  = QString::null,
		const QString&  _start   = QString::null);

ACGDLLEXPORT
QString
getOpenMeshName(QWidget*        _parent  = 0, 
		const QString&  _caption = QString::null,
		const QString&  _start   = QString::null);

ACGDLLEXPORT
QStringList
getOpenFileNames(QWidget*        _parent  = 0,
		 const QString&  _caption = QString::null,
		 const QString&  _filter  = QString::null,
		 const QString&  _start   = QString::null);

ACGDLLEXPORT
QStringList
getOpenMeshNames(QWidget*        _parent  = 0,
		 const QString&  _caption = QString::null,
		 const QString&  _start   = QString::null);

ACGDLLEXPORT
QString
getSaveFileName(QWidget*        _parent  = 0,
		const QString&  _caption = QString::null,
		const QString&  _filter  = QString::null,
		bool            _askOverwrite=false,
		const QString&  _start   = QString::null);

ACGDLLEXPORT
QString
getSaveMeshName(QWidget*        _parent  = 0,
		const QString&  _caption = QString::null,
		bool            _askOverwrite=false,
		const QString&  _start   = QString::null);


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_QTFILEDIALOG_HH defined
//=============================================================================
