/*
 * PDFedit - free program for PDF document manipulation.
 * Copyright (C) 2006-2009  PDFedit team: Michal Hocko,
 *                                        Jozef Misutka,
 *                                        Martin Petricek
 *                   Former team members: Miroslav Jahoda
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in doc/LICENSE.GPL); if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 * MA  02111-1307  USA
 *
 * Project is hosted on http://sourceforge.net/projects/pdfedit
 */
#ifndef __COUTLINE_H__
#define __COUTLINE_H__


// all basic includes
#include "kernel/static.h"


//=====================================================================================
namespace pdfobjects {
//=====================================================================================

/**
 * Checks whether the object is an ouline according to pdf specification.
 *
 * @param ip This iproperty is checked whether it is an outline.
 */
bool isOutline (boost::shared_ptr<IProperty> ip);

		
/**
 * Get text from an outline.
 *
 * @param ip Outline dictionary.
 */
std::string getOutlineText (boost::shared_ptr<IProperty> ip);
		

// not needed yet
//class COutline {};


//=====================================================================================
} /* namespace pdfobjects */
//=====================================================================================


#endif // __COUTLINE_H__
