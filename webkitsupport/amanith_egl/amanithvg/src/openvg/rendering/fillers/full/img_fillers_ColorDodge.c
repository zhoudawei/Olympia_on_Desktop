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

/*!
	\file img_fillers_ColorDodge.c
	\brief VG_BLEND_COLOR_DODGE_MZT image fillers, implementation ( \b generated \b file ).
	\author Matteo Muratori
	\author Michele Fabbri
*/

#if defined (RIM_VG_SRC)
#pragma push
#pragma arm
#endif

#if defined(AM_SRE) && !defined(AM_LITE_PROFILE)

#include "fillers.h"
#include "vggradients.h"
#include "pixel_utils.h"

#if defined(VG_MZT_advanced_blend_modes)

/*!
	\brief Image filler: blend mode VG_BLEND_COLOR_DODGE_MZT, image mode VG_DRAW_IMAGE_NORMAL, affine transformation ( \b generated \b function ).
	\param _surface pointer to a AMDrawingSurface structure, defining the destination drawing surface.
	\param _paintGen pointer to a AMPaintGen structure, containing paint information and derived values.
	\param y y coordinate of the scanline to fill.
	\param x0 x coordinate of the left-most pixel to fill.
	\param x1 x coordinate of the right-most pixel to fill.
	\note coverage line deltas are cleared (set to 0) during the loop, if needed.
*/
void amFilImage_NormalAffineColorDodge(void *_surface, void *_paintGen, AMint32 y, AMint32 x0, AMint32 x1) {

	AMDrawingSurface *surface = (AMDrawingSurface *)_surface;
	AMPaintGen *paintGen = (AMPaintGen *)_paintGen;
	AMuint32 *scrPixels;
	AMint32 *covLine;
	AMuint32 i, j, ofs0;
	AMint32 cov;
	AMPixel32 DcaDa, ScaSa, tmp;
	AMint32 invCov;
	AMint32 tmpCov;
	AMuint32 invSa;
	AMuint32 invDa;
	AMPixel32 t0, t1, t3, t4, t5, t6;
	AMint32 R, G, B;
	AMint32 r, g, b;
	AMuint8 *alphaPixels;
	AMuint32 mask;
	AMbool masking;

	AM_ASSERT(surface);
	AM_ASSERT(paintGen);
	AM_ASSERT(paintGen->coverageLineDeltas);
	AM_ASSERT(y >= 0 && y < amSrfHeightGet(surface));
	AM_ASSERT(x0 <= x1);
	AM_ASSERT(x0 >= 0 && x0 < amSrfWidthGet(surface));
	AM_ASSERT(x1 >= 0 && x1 < amSrfWidthGet(surface));
	ofs0 = (amSrfHeightGet(surface) - y - 1) * (AMuint32)amSrfWidthGet(surface) + x0;
	// go to the start screen pixel
	scrPixels = amSrfPixelsGet(surface) + ofs0;
	// extract coverage line
	covLine = paintGen->coverageLineDeltas + x0;
	// go to the start alpha pixel
	alphaPixels = surface->alphaMaskPixels + ofs0;
	masking = paintGen->paintDesc->masking;
	// initialize image paint generation
	amPaintImageInit(paintGen, x0 - 1, y);
	j = 0;
	i = x1 - x0 + 1;
	cov = 0;

loop_NormalAffineColorDodge:
	// update coverage and potential mask (according to alpha mask)
	cov += covLine[j];
	if (covLine[j] != 0)
		covLine[j] = 0;
	if (masking) {
		mask = alphaPixels[j];
		tmpCov = ((cov >> AM_RAS_COVERAGE_PRECISION) * mask) >> 8;
	}
	else
		tmpCov = cov >> AM_RAS_COVERAGE_PRECISION;
	// update image paint generation
	paintGen->iUx += paintGen->iDUx;
	paintGen->iVx += paintGen->iDVx;
	paintGen->imageSamplerParams.x = paintGen->iUx;
	paintGen->imageSamplerParams.y = paintGen->iVx;
	ScaSa.value = paintGen->imageSampler(&paintGen->imageSamplerParams);
	// get the pixel on the buffer, if blendMode is different than Src
	DcaDa.value = scrPixels[j];

		// apply blend equation
		invSa = ScaSa.c.a ^ 0xFF;
		invDa = DcaDa.c.a ^ 0xFF;
		t0.value = amPxlScl255(invDa, ScaSa.value);      // Sca.(1 - Da)
		t1.value = amPxlScl255(invSa, DcaDa.value);      // Dca.(1 - Sa)
		t3.value = t0.value + t1.value;              // Sca.(1 - Da) + Dca.(1 - Sa)
		t4.value = amPxlScl255(ScaSa.c.a, DcaDa.value);  // Dca.Sa
		t5 = ScaSa;
		t6 = DcaDa;

		b = (AMint32)t5.c.a - t5.c.b;
		B = (AMint32)t4.c.b - ((t6.c.a * b) >> 8);
		B = (B >= 0) ? (((AMint32)t5.c.a * t6.c.a) >> 8) + t3.c.b : ((AMint32)t4.c.b * t5.c.a) / b + t3.c.b;
		g = (AMint32)t5.c.a - t5.c.g;
		G = (AMint32)t4.c.g - ((t6.c.a * g) >> 8);
		G = (G >= 0) ? (((AMint32)t5.c.a * t6.c.a) >> 8) + t3.c.g : ((AMint32)t4.c.g * t5.c.a) / g + t3.c.g;
		r = (AMint32)t5.c.a - t5.c.r;
		R = (AMint32)t4.c.r - ((t6.c.a * r) >> 8);
		R = (R >= 0) ? (((AMint32)t5.c.a * t6.c.a) >> 8) + t3.c.r : ((AMint32)t4.c.r * t5.c.a) / r + t3.c.r;
		tmp.c.a = t5.c.a + t1.c.a;
		tmp.c.b = AM_CLAMP(B, 0, 255);
		tmp.c.g = AM_CLAMP(G, 0, 255);
		tmp.c.r = AM_CLAMP(R, 0, 255);


	// apply coverage
	invCov = 256 - tmpCov;
	if (invCov == 0) {
		// write pixel
		scrPixels[j++] = tmp.value;
		if (--i != 0)
			goto loop_NormalAffineColorDodge;
		else {
			covLine[j] = 0;
			return;
		}
	}
	else {
		// write pixel
		scrPixels[j++] = amPxlLerp((AMuint32)tmpCov, DcaDa.value, tmp.value);
		if (--i != 0)
			goto loop_NormalAffineColorDodge;
		else {
			covLine[j] = 0;
			return;
		}
	}
}

#endif
#if defined(VG_MZT_advanced_blend_modes)

/*!
	\brief Image filler: blend mode VG_BLEND_COLOR_DODGE_MZT, image mode VG_DRAW_IMAGE_NORMAL, perspective transformation ( \b generated \b function ).
	\param _surface pointer to a AMDrawingSurface structure, defining the destination drawing surface.
	\param _paintGen pointer to a AMPaintGen structure, containing paint information and derived values.
	\param y y coordinate of the scanline to fill.
	\param x0 x coordinate of the left-most pixel to fill.
	\param x1 x coordinate of the right-most pixel to fill.
	\note coverage line deltas are cleared (set to 0) during the loop, if needed.
*/
void amFilImage_NormalProjectiveColorDodge(void *_surface, void *_paintGen, AMint32 y, AMint32 x0, AMint32 x1) {

	AMDrawingSurface *surface = (AMDrawingSurface *)_surface;
	AMPaintGen *paintGen = (AMPaintGen *)_paintGen;
	AMuint32 *scrPixels;
	AMint32 *covLine;
	AMuint32 i, j, ofs0;
	AMint32 cov;
	AMPixel32 DcaDa, ScaSa, tmp;
	AMint32 invCov;
	AMint32 tmpCov;
	AMuint32 invSa;
	AMuint32 invDa;
	AMPixel32 t0, t1, t3, t4, t5, t6;
	AMint32 R, G, B;
	AMint32 r, g, b;
	AMuint8 *alphaPixels;
	AMuint32 mask;
	AMbool masking;
	AMint32 u, v;

	AM_ASSERT(surface);
	AM_ASSERT(paintGen);
	AM_ASSERT(paintGen->coverageLineDeltas);
	AM_ASSERT(y >= 0 && y < amSrfHeightGet(surface));
	AM_ASSERT(x0 <= x1);
	AM_ASSERT(x0 >= 0 && x0 < amSrfWidthGet(surface));
	AM_ASSERT(x1 >= 0 && x1 < amSrfWidthGet(surface));
	ofs0 = (amSrfHeightGet(surface) - y - 1) * (AMuint32)amSrfWidthGet(surface) + x0;
	// go to the start screen pixel
	scrPixels = amSrfPixelsGet(surface) + ofs0;
	// extract coverage line
	covLine = paintGen->coverageLineDeltas + x0;
	// go to the start alpha pixel
	alphaPixels = surface->alphaMaskPixels + ofs0;
	masking = paintGen->paintDesc->masking;
	// initialize image paint generation
	amPaintImageInit(paintGen, x0 - 1, y);
	j = 0;
	i = x1 - x0 + 1;
	cov = 0;

loop_NormalProjectiveColorDodge:
	// update coverage and potential mask (according to alpha mask)
	cov += covLine[j];
	if (covLine[j] != 0)
		covLine[j] = 0;
	if (masking) {
		mask = alphaPixels[j];
		tmpCov = ((cov >> AM_RAS_COVERAGE_PRECISION) * mask) >> 8;
	}
	else
		tmpCov = cov >> AM_RAS_COVERAGE_PRECISION;
	// update image paint generation
	paintGen->iUf += paintGen->iDUf;
	paintGen->iVf += paintGen->iDVf;
	paintGen->iWf += paintGen->iDWf;
	u = (AMint32)(paintGen->iUf / paintGen->iWf);
	v = (AMint32)(paintGen->iVf / paintGen->iWf);
	paintGen->imageSamplerParams.x = u;
	paintGen->imageSamplerParams.y = v;
	ScaSa.value = paintGen->imageSampler(&paintGen->imageSamplerParams);
	// get the pixel on the buffer, if blendMode is different than Src
	DcaDa.value = scrPixels[j];

		// apply blend equation
		invSa = ScaSa.c.a ^ 0xFF;
		invDa = DcaDa.c.a ^ 0xFF;
		t0.value = amPxlScl255(invDa, ScaSa.value);      // Sca.(1 - Da)
		t1.value = amPxlScl255(invSa, DcaDa.value);      // Dca.(1 - Sa)
		t3.value = t0.value + t1.value;              // Sca.(1 - Da) + Dca.(1 - Sa)
		t4.value = amPxlScl255(ScaSa.c.a, DcaDa.value);  // Dca.Sa
		t5 = ScaSa;
		t6 = DcaDa;

		b = (AMint32)t5.c.a - t5.c.b;
		B = (AMint32)t4.c.b - ((t6.c.a * b) >> 8);
		B = (B >= 0) ? (((AMint32)t5.c.a * t6.c.a) >> 8) + t3.c.b : ((AMint32)t4.c.b * t5.c.a) / b + t3.c.b;
		g = (AMint32)t5.c.a - t5.c.g;
		G = (AMint32)t4.c.g - ((t6.c.a * g) >> 8);
		G = (G >= 0) ? (((AMint32)t5.c.a * t6.c.a) >> 8) + t3.c.g : ((AMint32)t4.c.g * t5.c.a) / g + t3.c.g;
		r = (AMint32)t5.c.a - t5.c.r;
		R = (AMint32)t4.c.r - ((t6.c.a * r) >> 8);
		R = (R >= 0) ? (((AMint32)t5.c.a * t6.c.a) >> 8) + t3.c.r : ((AMint32)t4.c.r * t5.c.a) / r + t3.c.r;
		tmp.c.a = t5.c.a + t1.c.a;
		tmp.c.b = AM_CLAMP(B, 0, 255);
		tmp.c.g = AM_CLAMP(G, 0, 255);
		tmp.c.r = AM_CLAMP(R, 0, 255);


	// apply coverage
	invCov = 256 - tmpCov;
	if (invCov == 0) {
		// write pixel
		scrPixels[j++] = tmp.value;
		if (--i != 0)
			goto loop_NormalProjectiveColorDodge;
		else {
			covLine[j] = 0;
			return;
		}
	}
	else {
		// write pixel
		scrPixels[j++] = amPxlLerp((AMuint32)tmpCov, DcaDa.value, tmp.value);
		if (--i != 0)
			goto loop_NormalProjectiveColorDodge;
		else {
			covLine[j] = 0;
			return;
		}
	}
}

#endif
#if defined(VG_MZT_advanced_blend_modes)

/*!
	\brief Image filler: blend mode VG_BLEND_COLOR_DODGE_MZT, image mode VG_DRAW_IMAGE_MULTIPLY, affine transformation ( \b generated \b function ).
	\param _surface pointer to a AMDrawingSurface structure, defining the destination drawing surface.
	\param _paintGen pointer to a AMPaintGen structure, containing paint information and derived values.
	\param y y coordinate of the scanline to fill.
	\param x0 x coordinate of the left-most pixel to fill.
	\param x1 x coordinate of the right-most pixel to fill.
	\note coverage line deltas are cleared (set to 0) during the loop, if needed.
*/
void amFilImage_MultiplyAffineColorDodge(void *_surface, void *_paintGen, AMint32 y, AMint32 x0, AMint32 x1) {

	AMDrawingSurface *surface = (AMDrawingSurface *)_surface;
	AMuint32 srfIdx = AM_FMT_GET_INDEX(amSrfFormat32Get(surface));
	AMPaintGen *paintGen = (AMPaintGen *)_paintGen;
	AMuint32 *scrPixels;
	AMint32 *covLine;
	AMuint32 i, j, ofs0;
	AMint32 cov;
	AMPixel32 DcaDa, ScaSa, tmp;
	AMint32 invCov;
	AMint32 tmpCov;
	AMuint32 invSa;
	AMuint32 invDa;
	AMPixel32 t0, t1, t3, t4, t5, t6;
	AMint32 R, G, B;
	AMint32 r, g, b;
	AMint32 fixedU;
#if defined RIM_VG_SRC
    VGColorRampSpreadMode spreadMode = 0;
    const AMuint32 *gradientPixels = NULL;
#else
	VGColorRampSpreadMode spreadMode;
	const AMuint32 *gradientPixels;
#endif
	AMPixel32 paint;
	AMfloat s, u;
	AMuint8 *alphaPixels;
	AMuint32 mask;
	AMbool masking;

	AM_ASSERT(surface);
	AM_ASSERT(paintGen);
	AM_ASSERT(paintGen->coverageLineDeltas);
	AM_ASSERT(y >= 0 && y < amSrfHeightGet(surface));
	AM_ASSERT(x0 <= x1);
	AM_ASSERT(x0 >= 0 && x0 < amSrfWidthGet(surface));
	AM_ASSERT(x1 >= 0 && x1 < amSrfWidthGet(surface));
#if defined RIM_VG_SRC
    paint.value = 0;
#endif

	ofs0 = (amSrfHeightGet(surface) - y - 1) * (AMuint32)amSrfWidthGet(surface) + x0;
	// go to the start screen pixel
	scrPixels = amSrfPixelsGet(surface) + ofs0;
	// extract coverage line
	covLine = paintGen->coverageLineDeltas + x0;
	// go to the start alpha pixel
	alphaPixels = surface->alphaMaskPixels + ofs0;
	masking = paintGen->paintDesc->masking;
	// initialize image paint generation
	amPaintImageInit(paintGen, x0 - 1, y);
	// initialize paint generation
	switch (paintGen->paintDesc->paintType) {
		case VG_PAINT_TYPE_COLOR:
			// for color paint, we can extract it once outside of loop
			paint.value = paintGen->paintColor32ImgMultiply;
			break;
		case VG_PAINT_TYPE_LINEAR_GRADIENT:
			amPaintLinGradInit(paintGen, x0 - 1, y);
			spreadMode = paintGen->paintDesc->paint->colorRampSpreadMode;
			gradientPixels = paintGen->gradTextureImgMultiply;
			break;
		case VG_PAINT_TYPE_RADIAL_GRADIENT:
			amPaintRadGradInit(paintGen, x0 - 1, y);
			spreadMode = paintGen->paintDesc->paint->colorRampSpreadMode;
			gradientPixels = paintGen->gradTextureImgMultiply;
			break;
		case VG_PAINT_TYPE_PATTERN:
			amPaintPatternInit(paintGen, x0 - 1, y);
			break;
	#if defined(VG_MZT_conical_gradient)
		case VG_PAINT_TYPE_CONICAL_GRADIENT_MZT:
			amPaintConGradInit(paintGen, x0 - 1, y);
			spreadMode = paintGen->paintDesc->paint->colorRampSpreadMode;
			gradientPixels = paintGen->gradTextureImgMultiply;
			break;
	#endif
		default:
			break;
	}
	j = 0;
	i = x1 - x0 + 1;
	cov = 0;

loop_MultiplyAffineColorDodge:
	// update coverage and potential mask (according to alpha mask)
	cov += covLine[j];
	if (covLine[j] != 0)
		covLine[j] = 0;
	if (masking) {
		mask = alphaPixels[j];
		tmpCov = ((cov >> AM_RAS_COVERAGE_PRECISION) * mask) >> 8;
	}
	else
		tmpCov = cov >> AM_RAS_COVERAGE_PRECISION;
	// update image paint generation
	paintGen->iUx += paintGen->iDUx;
	paintGen->iVx += paintGen->iDVx;
	paintGen->imageSamplerParams.x = paintGen->iUx;
	paintGen->imageSamplerParams.y = paintGen->iVx;
	ScaSa.value = paintGen->imageSampler(&paintGen->imageSamplerParams);
	// update paint generation
	switch (paintGen->paintDesc->paintType) {
		case VG_PAINT_TYPE_LINEAR_GRADIENT:
			paintGen->Ux += paintGen->DUx;
			fixedU = paintGen->Ux;
			AM_GRADIENTS_LINEAR_PIXEL_GET(paint.value, fixedU)
			break;
		case VG_PAINT_TYPE_RADIAL_GRADIENT:
			paintGen->Uf += paintGen->DUf;
			paintGen->Vf += paintGen->DVf;
			paintGen->A += paintGen->dA;
			paintGen->B += paintGen->dB;
			s = paintGen->radiusSqr * (paintGen->Uf * paintGen->Uf + paintGen->Vf * paintGen->Vf);
			u = paintGen->A + amFastSqrtf(s - (paintGen->B *paintGen->B));
			fixedU = (AMint32)(u * paintGen->K);
			AM_GRADIENTS_RADIAL_PIXEL_GET(paint.value, fixedU)
			break;
		case VG_PAINT_TYPE_PATTERN:
			paintGen->Ux += paintGen->DUx;
			paintGen->Vx += paintGen->DVx;
			paintGen->patternSamplerParams.x = paintGen->Ux;
			paintGen->patternSamplerParams.y = paintGen->Vx;
			paint.value = paintGen->patternSampler(&paintGen->patternSamplerParams);
			break;
	#if defined(VG_MZT_conical_gradient)
		case VG_PAINT_TYPE_CONICAL_GRADIENT_MZT: {
			AMfloat xf, yf, l;
			AMint32 fixedV;
			AMuint32 atan2Val;
			AMuint32 atan2U;
			paintGen->Uf += paintGen->DUf;
			paintGen->Vf += paintGen->DVf;
			xf = paintGen->Uf;
			yf = paintGen->Vf;
			l = xf * xf + yf * yf;
			if (l != 0) {
				AMfloat xhalf = 0.5f * l;
				AMint32 f = *(AMint32 *)&l;
				f = 0x5f3759df - (f >> 1);
				l = *(AMfloat*)&f;
				l = l * (1.5f - xhalf * l * l);
				xf *= l;
				yf *= l;
			}
			xf = xf * AM_ATAN2_TABLE_SIZE_K + AM_ATAN2_TABLE_SIZE_K;
			yf = yf * AM_ATAN2_TABLE_SIZE_K + AM_ATAN2_TABLE_SIZE_K;
			fixedU = (AMint32)xf;
			fixedV = (AMint32)yf;
			AM_ASSERT(fixedU >= 0 && fixedU < AM_GRADIENTS_CONICAL_TEXTURE_WIDTH);
			AM_ASSERT(fixedV >= 0 && fixedV < AM_GRADIENTS_CONICAL_TEXTURE_WIDTH);
			AM_GRADIENTS_CONICAL_PIXEL_GET(paint.value, fixedU, fixedV)
			}
			break;
	#endif
		default:
			break;
	}
	ScaSa.value = amPxlMul(ScaSa.value, paint.value);
	ScaSa.value = amImgSampleToSurface(ScaSa.value, paintGen->imageSamplerParams.dstIdx, paintGen->colorTransformationImgMultiply, srfIdx);
	// get the pixel on the buffer, if blendMode is different than Src
	DcaDa.value = scrPixels[j];

		// apply blend equation
		invSa = ScaSa.c.a ^ 0xFF;
		invDa = DcaDa.c.a ^ 0xFF;
		t0.value = amPxlScl255(invDa, ScaSa.value);      // Sca.(1 - Da)
		t1.value = amPxlScl255(invSa, DcaDa.value);      // Dca.(1 - Sa)
		t3.value = t0.value + t1.value;              // Sca.(1 - Da) + Dca.(1 - Sa)
		t4.value = amPxlScl255(ScaSa.c.a, DcaDa.value);  // Dca.Sa
		t5 = ScaSa;
		t6 = DcaDa;

		b = (AMint32)t5.c.a - t5.c.b;
		B = (AMint32)t4.c.b - ((t6.c.a * b) >> 8);
		B = (B >= 0) ? (((AMint32)t5.c.a * t6.c.a) >> 8) + t3.c.b : ((AMint32)t4.c.b * t5.c.a) / b + t3.c.b;
		g = (AMint32)t5.c.a - t5.c.g;
		G = (AMint32)t4.c.g - ((t6.c.a * g) >> 8);
		G = (G >= 0) ? (((AMint32)t5.c.a * t6.c.a) >> 8) + t3.c.g : ((AMint32)t4.c.g * t5.c.a) / g + t3.c.g;
		r = (AMint32)t5.c.a - t5.c.r;
		R = (AMint32)t4.c.r - ((t6.c.a * r) >> 8);
		R = (R >= 0) ? (((AMint32)t5.c.a * t6.c.a) >> 8) + t3.c.r : ((AMint32)t4.c.r * t5.c.a) / r + t3.c.r;
		tmp.c.a = t5.c.a + t1.c.a;
		tmp.c.b = AM_CLAMP(B, 0, 255);
		tmp.c.g = AM_CLAMP(G, 0, 255);
		tmp.c.r = AM_CLAMP(R, 0, 255);


	// apply coverage
	invCov = 256 - tmpCov;
	if (invCov == 0) {
		// write pixel
		scrPixels[j++] = tmp.value;
		if (--i != 0)
			goto loop_MultiplyAffineColorDodge;
		else {
			covLine[j] = 0;
			return;
		}
	}
	else {
		// write pixel
		scrPixels[j++] = amPxlLerp((AMuint32)tmpCov, DcaDa.value, tmp.value);
		if (--i != 0)
			goto loop_MultiplyAffineColorDodge;
		else {
			covLine[j] = 0;
			return;
		}
	}
}

#endif
#if defined(VG_MZT_advanced_blend_modes)

/*!
	\brief Image filler: blend mode VG_BLEND_COLOR_DODGE_MZT, image mode VG_DRAW_IMAGE_STENCIL, affine transformation ( \b generated \b function ).
	\param _surface pointer to a AMDrawingSurface structure, defining the destination drawing surface.
	\param _paintGen pointer to a AMPaintGen structure, containing paint information and derived values.
	\param y y coordinate of the scanline to fill.
	\param x0 x coordinate of the left-most pixel to fill.
	\param x1 x coordinate of the right-most pixel to fill.
	\note coverage line deltas are cleared (set to 0) during the loop, if needed.
*/
void amFilImage_StencilAffineColorDodge(void *_surface, void *_paintGen, AMint32 y, AMint32 x0, AMint32 x1) {

	AMDrawingSurface *surface = (AMDrawingSurface *)_surface;
	AMPaintGen *paintGen = (AMPaintGen *)_paintGen;
	AMuint32 *scrPixels;
	AMint32 *covLine;
	AMuint32 i, j, ofs0;
	AMint32 cov;
	AMPixel32 DcaDa, ScaSa, tmp;
	AMint32 invCov;
	AMint32 tmpCov;
	AMPixel32 Sa, invSa;
	AMuint32 invDa;
	AMPixel32 t0, t1, t3, t4, t5, t6;
	AMint32 R, G, B;
	AMint32 r, g, b;
	AMint32 fixedU;
#if defined RIM_VG_SRC
    VGColorRampSpreadMode spreadMode = 0;
    const AMuint32 *gradientPixels = NULL;
#else
	VGColorRampSpreadMode spreadMode;
	const AMuint32 *gradientPixels;
#endif
	AMPixel32 paint;
	AMfloat s, u;
	AMuint8 *alphaPixels;
	AMuint32 mask;
	AMbool masking;

	AM_ASSERT(surface);
	AM_ASSERT(paintGen);
	AM_ASSERT(paintGen->coverageLineDeltas);
	AM_ASSERT(y >= 0 && y < amSrfHeightGet(surface));
	AM_ASSERT(x0 <= x1);
	AM_ASSERT(x0 >= 0 && x0 < amSrfWidthGet(surface));
	AM_ASSERT(x1 >= 0 && x1 < amSrfWidthGet(surface));
	ofs0 = (amSrfHeightGet(surface) - y - 1) * (AMuint32)amSrfWidthGet(surface) + x0;
	// go to the start screen pixel
	scrPixels = amSrfPixelsGet(surface) + ofs0;
	// extract coverage line
	covLine = paintGen->coverageLineDeltas + x0;
	// go to the start alpha pixel
	alphaPixels = surface->alphaMaskPixels + ofs0;
	masking = paintGen->paintDesc->masking;
	// initialize image paint generation
	amPaintImageInit(paintGen, x0 - 1, y);
	// initialize paint generation
	switch (paintGen->paintDesc->paintType) {
		case VG_PAINT_TYPE_COLOR:
			// for color paint, we can extract it once outside of loop
			paint.value = paintGen->paintColor32;
			break;
		case VG_PAINT_TYPE_LINEAR_GRADIENT:
			amPaintLinGradInit(paintGen, x0 - 1, y);
			spreadMode = paintGen->paintDesc->paint->colorRampSpreadMode;
			gradientPixels = paintGen->gradTexture;
			break;
		case VG_PAINT_TYPE_RADIAL_GRADIENT:
			amPaintRadGradInit(paintGen, x0 - 1, y);
			spreadMode = paintGen->paintDesc->paint->colorRampSpreadMode;
			gradientPixels = paintGen->gradTexture;
			break;
		case VG_PAINT_TYPE_PATTERN:
			amPaintPatternInit(paintGen, x0 - 1, y);
			break;
	#if defined(VG_MZT_conical_gradient)
		case VG_PAINT_TYPE_CONICAL_GRADIENT_MZT:
			amPaintConGradInit(paintGen, x0 - 1, y);
			spreadMode = paintGen->paintDesc->paint->colorRampSpreadMode;
			gradientPixels = paintGen->gradTexture;
			break;
	#endif
		default:
			break;
	}
	j = 0;
	i = x1 - x0 + 1;
	cov = 0;

loop_StencilAffineColorDodge:
	// update coverage and potential mask (according to alpha mask)
	cov += covLine[j];
	if (covLine[j] != 0)
		covLine[j] = 0;
	if (masking) {
		mask = alphaPixels[j];
		tmpCov = ((cov >> AM_RAS_COVERAGE_PRECISION) * mask) >> 8;
	}
	else
		tmpCov = cov >> AM_RAS_COVERAGE_PRECISION;
	// update image paint generation
	paintGen->iUx += paintGen->iDUx;
	paintGen->iVx += paintGen->iDVx;
	paintGen->imageSamplerParams.x = paintGen->iUx;
	paintGen->imageSamplerParams.y = paintGen->iVx;
	ScaSa.value = paintGen->imageSampler(&paintGen->imageSamplerParams);
	// update paint generation
	switch (paintGen->paintDesc->paintType) {
		case VG_PAINT_TYPE_LINEAR_GRADIENT:
			paintGen->Ux += paintGen->DUx;
			fixedU = paintGen->Ux;
			AM_GRADIENTS_LINEAR_PIXEL_GET(paint.value, fixedU)
			break;
		case VG_PAINT_TYPE_RADIAL_GRADIENT:
			paintGen->Uf += paintGen->DUf;
			paintGen->Vf += paintGen->DVf;
			paintGen->A += paintGen->dA;
			paintGen->B += paintGen->dB;
			s = paintGen->radiusSqr * (paintGen->Uf * paintGen->Uf + paintGen->Vf * paintGen->Vf);
			u = paintGen->A + amFastSqrtf(s - (paintGen->B *paintGen->B));
			fixedU = (AMint32)(u * paintGen->K);
			AM_GRADIENTS_RADIAL_PIXEL_GET(paint.value, fixedU)
			break;
		case VG_PAINT_TYPE_PATTERN:
			paintGen->Ux += paintGen->DUx;
			paintGen->Vx += paintGen->DVx;
			paintGen->patternSamplerParams.x = paintGen->Ux;
			paintGen->patternSamplerParams.y = paintGen->Vx;
			paint.value = paintGen->patternSampler(&paintGen->patternSamplerParams);
			break;
	#if defined(VG_MZT_conical_gradient)
		case VG_PAINT_TYPE_CONICAL_GRADIENT_MZT: {
			AMfloat xf, yf, l;
			AMint32 fixedV;
			AMuint32 atan2Val;
			AMuint32 atan2U;
			paintGen->Uf += paintGen->DUf;
			paintGen->Vf += paintGen->DVf;
			xf = paintGen->Uf;
			yf = paintGen->Vf;
			l = xf * xf + yf * yf;
			if (l != 0) {
				AMfloat xhalf = 0.5f * l;
				AMint32 f = *(AMint32 *)&l;
				f = 0x5f3759df - (f >> 1);
				l = *(AMfloat*)&f;
				l = l * (1.5f - xhalf * l * l);
				xf *= l;
				yf *= l;
			}
			xf = xf * AM_ATAN2_TABLE_SIZE_K + AM_ATAN2_TABLE_SIZE_K;
			yf = yf * AM_ATAN2_TABLE_SIZE_K + AM_ATAN2_TABLE_SIZE_K;
			fixedU = (AMint32)xf;
			fixedV = (AMint32)yf;
			AM_ASSERT(fixedU >= 0 && fixedU < AM_GRADIENTS_CONICAL_TEXTURE_WIDTH);
			AM_ASSERT(fixedV >= 0 && fixedV < AM_GRADIENTS_CONICAL_TEXTURE_WIDTH);
			AM_GRADIENTS_CONICAL_PIXEL_GET(paint.value, fixedU, fixedV)
			}
			break;
	#endif
		default:
			break;
	}
	Sa.value = amPxlScl255(paint.c.a, ScaSa.value);
	ScaSa.value = amPxlMul(ScaSa.value, paint.value);
	// get the pixel on the buffer, if blendMode is different than Src
	DcaDa.value = scrPixels[j];

		// apply blend equation
		invSa.value = amPxlInv(Sa.value);
		invDa = DcaDa.c.a ^ 0xFF;
		t0.value = amPxlScl255(invDa, ScaSa.value);         // Sca.(1 - Da)
		t1.value = amPxlMul(invSa.value, DcaDa.value);    // Dca.(1 - Sa)
		t3.value = t0.value + t1.value;                 // Sca.(1 - Da) + Dca.(1 - Sa)
		t4.value = amPxlMulNoA(Sa.value, DcaDa.value);    // Dca.Sa
		t5 = ScaSa;
		t6 = DcaDa;

		b = (AMint32)Sa.c.b - t5.c.b;
		B = (AMint32)t4.c.b - ((t6.c.a * b) >> 8);
		if (B >= 0)
			B = (((AMint32)Sa.c.b * t6.c.a) >> 8) + t3.c.b;
		else
			B = ((AMint32)t4.c.b * Sa.c.b) / b + t3.c.b;

		g = (AMint32)Sa.c.g - t5.c.g;
		G = (AMint32)t4.c.g - ((t6.c.a * g) >> 8);
		if (G >= 0)
			G = (((AMint32)Sa.c.g * t6.c.a) >> 8) + t3.c.g;
		else
			G = ((AMint32)t4.c.g * Sa.c.g) / g + t3.c.g;

		r = (AMint32)Sa.c.r - t5.c.r;
		R = (AMint32)t4.c.r - ((t6.c.a * r) >> 8);
		if (R >= 0)
			R = (((AMint32)Sa.c.r * t6.c.a) >> 8) + t3.c.r;
		else
			R = ((AMint32)t4.c.r * Sa.c.r) / r + t3.c.r;

		tmp.c.a = t5.c.a + t1.c.a;
		tmp.c.b = AM_CLAMP(B, 0, 255);
		tmp.c.g = AM_CLAMP(G, 0, 255);
		tmp.c.r = AM_CLAMP(R, 0, 255);

	// apply coverage
	invCov = 256 - tmpCov;
	if (invCov == 0) {
		// write pixel
		scrPixels[j++] = tmp.value;
		if (--i != 0)
			goto loop_StencilAffineColorDodge;
		else {
			covLine[j] = 0;
			return;
		}
	}
	else {
		// write pixel
		scrPixels[j++] = amPxlLerp((AMuint32)tmpCov, DcaDa.value, tmp.value);
		if (--i != 0)
			goto loop_StencilAffineColorDodge;
		else {
			covLine[j] = 0;
			return;
		}
	}
}

#endif
#endif

#if defined (RIM_VG_SRC)
#pragma pop
#endif

