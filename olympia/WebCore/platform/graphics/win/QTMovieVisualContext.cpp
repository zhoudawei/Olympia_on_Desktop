/*
 * Copyright (C) 2007, 2008, 2009, 2010 Apple, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
#include "config.h"

#include "QTMovieVisualContext.h"

#include "QTMovieTask.h"
#include <CVBase.h>
#include <CVHostTime.h>
#include <ImageCompression.h>
#include <Movies.h>
#include <windows.h>

struct QTCVTimeStamp {
    CVTimeStamp t;
};

class QTMovieVisualContextPriv {
public:
    QTMovieVisualContextPriv(QTMovieVisualContext* parent, QTMovieVisualContextClient* client, CFDictionaryRef options);
    ~QTMovieVisualContextPriv();

    bool isImageAvailableForTime(const QTCVTimeStamp*) const;
    QTPixelBuffer imageForTime(const QTCVTimeStamp*);
    void task();

    QTVisualContextRef visualContextRef();

    void setMovie(PassRefPtr<QTMovie>);
    QTMovie* movie() const;
    
    static void imageAvailableCallback(QTVisualContextRef visualContext, const CVTimeStamp *timeStamp, void *refCon);

private:
    QTMovieVisualContext* m_parent;
    QTMovieVisualContextClient* m_client;
    QTVisualContextRef m_visualContext;
    RefPtr<QTMovie> m_movie;

};

QTMovieVisualContextPriv::QTMovieVisualContextPriv(QTMovieVisualContext* parent, QTMovieVisualContextClient* client, CFDictionaryRef options) 
        : m_parent(parent)
        , m_client(client)
        , m_visualContext(0)
{
    typedef OSStatus ( __cdecl *pfnQTPixelBufferContextCreate)(CFAllocatorRef, CFDictionaryRef, QTVisualContextRef*);
    static pfnQTPixelBufferContextCreate pPixelBufferContextCreate = 0;
    if (!pPixelBufferContextCreate) {
        HMODULE qtmlDLL = ::LoadLibraryW(L"QTMLClient.dll");
        if (!qtmlDLL)
            return;
        pPixelBufferContextCreate = reinterpret_cast<pfnQTPixelBufferContextCreate>(GetProcAddress(qtmlDLL, "QTPixelBufferContextCreate"));
        if (!pPixelBufferContextCreate) 
            return;
    }

    OSStatus status = pPixelBufferContextCreate(kCFAllocatorDefault, options, &m_visualContext);
    if (status == noErr && m_visualContext)
        QTVisualContextSetImageAvailableCallback(m_visualContext, &QTMovieVisualContextPriv::imageAvailableCallback, static_cast<void*>(this));
}

QTMovieVisualContextPriv::~QTMovieVisualContextPriv()
{
}

bool QTMovieVisualContextPriv::isImageAvailableForTime(const QTCVTimeStamp* timeStamp) const
{
    if (!m_visualContext)
        return false;

    return QTVisualContextIsNewImageAvailable(m_visualContext, reinterpret_cast<const CVTimeStamp*>(timeStamp)); 
}

QTPixelBuffer QTMovieVisualContextPriv::imageForTime(const QTCVTimeStamp* timeStamp)
{
    QTPixelBuffer pixelBuffer;
    if (m_visualContext) {
        CVImageBufferRef newImage = 0;
        OSStatus status = QTVisualContextCopyImageForTime(m_visualContext, kCFAllocatorDefault, reinterpret_cast<const CVTimeStamp*>(timeStamp), &newImage);
        if (status == noErr)
            pixelBuffer.adopt(newImage);
    }
    return pixelBuffer;
}

void QTMovieVisualContextPriv::task()
{
    if (m_visualContext)
        QTVisualContextTask(m_visualContext);
}

QTVisualContextRef QTMovieVisualContextPriv::visualContextRef() 
{
    return m_visualContext;
}

void QTMovieVisualContextPriv::setMovie(PassRefPtr<QTMovie> movie)
{
    if (movie == m_movie)
        return;

    if (m_movie) {
        SetMovieVisualContext(m_movie->getMovieHandle(), 0);
        m_movie = 0;
    }

    if (movie)
        OSStatus status = SetMovieVisualContext(movie->getMovieHandle(), m_visualContext);
    
    m_movie = movie;
}

QTMovie* QTMovieVisualContextPriv::movie() const
{
    return m_movie.get();
}

void QTMovieVisualContextPriv::imageAvailableCallback(QTVisualContextRef visualContext, const CVTimeStamp *timeStamp, void *refCon) 
{
    if (!refCon)
        return;

    QTMovieVisualContextPriv* vc = static_cast<QTMovieVisualContextPriv*>(refCon);
    if (!vc->m_client)
        return;

    vc->m_client->imageAvailableForTime(reinterpret_cast<const QTCVTimeStamp*>(timeStamp));
}

static OSStatus SetNumberValue(CFMutableDictionaryRef inDict, CFStringRef inKey, SInt32 inValue)
{
    CFNumberRef number;
 
    number = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &inValue);
    if (!number) 
        return coreFoundationUnknownErr;
 
    CFDictionarySetValue(inDict, inKey, number);
 
    CFRelease(number);
 
    return noErr;
}

CFDictionaryRef QTMovieVisualContext::getCGImageOptions()
{
    static CFDictionaryRef options = 0;

    if (!options) {
        CFMutableDictionaryRef  pixelBufferOptions = 0;
        CFMutableDictionaryRef  visualContextOptions = 0;
        OSStatus                status = noErr;
        
        // Pixel Buffer attributes
        pixelBufferOptions = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                       &kCFTypeDictionaryKeyCallBacks,
                                                       &kCFTypeDictionaryValueCallBacks);

        // Use the k32BGRAPixelFormat, as QuartzCore will be able to use the pixels directly,
        // without needing an aditional copy or rendering pass:s
        SetNumberValue(pixelBufferOptions, kCVPixelBufferPixelFormatTypeKey, k32BGRAPixelFormat);
            
        // alignment
        SetNumberValue(pixelBufferOptions, kCVPixelBufferBytesPerRowAlignmentKey, 16);

        // compatability
        SetNumberValue(pixelBufferOptions, kCVPixelBufferCGImageCompatibilityKey, 1);
     
        // QT Visual Context attributes
        visualContextOptions = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                         &kCFTypeDictionaryKeyCallBacks,
                                                         &kCFTypeDictionaryValueCallBacks);
     
        // set the pixel buffer attributes for the visual context
        CFDictionarySetValue(visualContextOptions,
                             kQTVisualContextPixelBufferAttributesKey,
                             pixelBufferOptions);

        if (pixelBufferOptions)
            CFRelease(pixelBufferOptions);
    
        options = visualContextOptions;
    }

    return options;
}

QTMovieVisualContext::QTMovieVisualContext(QTMovieVisualContextClient* client, CFDictionaryRef options) 
    : m_private(new QTMovieVisualContextPriv(this, client, options))
{
}

QTMovieVisualContext::~QTMovieVisualContext()
{
}

bool QTMovieVisualContext::isImageAvailableForTime(const QTCVTimeStamp* timeStamp) const
{
    return m_private->isImageAvailableForTime(timeStamp);
}

QTPixelBuffer QTMovieVisualContext::imageForTime(const QTCVTimeStamp* timeStamp)
{
    return m_private->imageForTime(timeStamp);
}

void QTMovieVisualContext::task()
{
    m_private->task();
}

QTVisualContextRef QTMovieVisualContext::visualContextRef() 
{
    return m_private->visualContextRef();
}

void QTMovieVisualContext::setMovie(PassRefPtr<QTMovie> movie)
{
    m_private->setMovie(movie);
}

QTMovie* QTMovieVisualContext::movie() const
{
    return m_private->movie();
}

double QTMovieVisualContext::currentHostTime()
{
    return CVGetCurrentHostTime() / CVGetHostClockFrequency();
}
