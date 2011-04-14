/****************************************************************************
** Copyright (C) 2004-2009 Mazatech S.r.l. All rights reserved.
**
** This file is part of AmanithVG software, an OpenVG implementation.
** This file is strictly confidential under the signed Mazatech Software
** Non-disclosure agreement and it's provided according to the signed
** Mazatech Software licensing agreement.
**
** Khronos and OpenVG are trademarks of The Khronos Group Inc.
** OpenGL is a registered trademark and OpenGL ES is a trademark of
** Silicon Graphics, Inc.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** For any information, please contact info@mazatech.com
**
****************************************************************************/

#ifndef _SREPRIMITIVES_H
#define _SREPRIMITIVES_H

/*!
	\file sreprimitives.h
	\brief Path and image drawing functions entry point (SRE), header.
	\author Matteo Muratori
	\author Michele Fabbri
*/

#if defined(AM_SRE)

#include "vg_priv.h"
#include "vgcontext.h"
#include "vgmatrix.h"

// Entry point for path drawing function (SRE).
AMbool amSrePathDraw(AMContext *context,
					 AMDrawingSurface *surface,
					 AMUserToSurfaceDesc *userToSurfaceDesc,
					 AMPath *path,
					 const VGbitfield paintModes);
// Entry point for image drawing function (SRE).
AMbool amSreImageDraw(AMContext *context,
					  AMDrawingSurface *surface,
					  AMUserToSurfaceDesc *userToSurfaceDesc,
					  AMImage *image);

#if (AM_OPENVG_VERSION >= 110)
AMbool amSreImageGlyphDraw(AMContext *context,
						   AMDrawingSurface *surface,
						   AMUserToSurfaceDesc *userToSurfaceDesc,
						   AMImage *image);
#endif

#endif
#endif
