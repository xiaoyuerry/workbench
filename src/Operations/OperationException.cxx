/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/


#include "OperationException.h"

#include <cstring>

using namespace caret;

/**
 * Constructor.
 *
 */
OperationException::OperationException()
: CaretException()
{
    this->initializeMembersOperationException();
}

/**
 * Constructor that uses stack trace from the exception
 * passed in as a parameter.
 * 
 * @param e Any exception whose stack trace becomes
 * this exception's stack trace.
 *
 */
OperationException::OperationException(
                    const CaretException& e)
: CaretException(e)
{
    this->initializeMembersOperationException();
}

/**
 * Constructor.
 *
 * @param  s  Description of the exception.
 *
 */
OperationException::OperationException(const AString& s)
: CaretException(s)
{
    this->initializeMembersOperationException();
}

/**
 * Copy Constructor.
 * @param e
 *     Exception that is copied.
 */
OperationException::OperationException(const OperationException& e)
: CaretException(e)
{
}

/**
 * Assignment operator.
 * @param e
 *     Exception that is copied.
 * @return 
 *     Copy of the exception.
 */
OperationException& 
OperationException::operator=(const OperationException& e)
{
    if (this != &e) {
        CaretException::operator=(e);
    }
    
    return *this;
}

/**
 * Destructor
 */
OperationException::~OperationException() throw()
{
}

void
OperationException::initializeMembersOperationException()
{
}

