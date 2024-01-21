// ExAvi.cpp
//
// Copyright © 1998-2000 Microsoft Corporation, All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#include "VisWin.h"
#include <VisCore.h>
#if VIS_INTERNAL_RELEASE
    #include <VisImageProc.h>
#endif // VIS_INTERNAL_RELEASE
#include "VisMemoryChecks.h"

class FindFiles 
{
public:
    FindFiles()  { m_hFound = 0;};
    ~FindFiles() { Close();};

    BOOL Open( const char* pImageNames )
    {    
        m_szPrefix[0] = '\0';

        m_hFound = FindFirstFile(pImageNames, &m_FindFileData);
        if (m_hFound == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }

        // If we are reading files from a directory other than the current one,
        // save the directory name.
        if (strchr(pImageNames, '\\') != NULL)
        {
            strcpy(m_szPrefix, pImageNames);
            char* ptr = strrchr(m_szPrefix,'\\');
            ptr[1] = '\0';
        }

        sprintf(m_szFull, "%s%s", m_szPrefix, m_FindFileData.cFileName);
        return TRUE;
    }

    void FileName( char** ppName, char** ppExt )
    {
        *ppName = m_szFull;
        *ppExt  = strrchr( m_szFull, '.' );
    }

    BOOL Next()
    {
        if ( !FindNextFile(m_hFound, &m_FindFileData) )
            return FALSE;

        sprintf(m_szFull, "%s%s", m_szPrefix, m_FindFileData.cFileName);
        return TRUE;
    }

    void Close()
    {
        if ( m_hFound )
        {
            FindClose(m_hFound);
            m_hFound = 0;
        }
    }

private:

    WIN32_FIND_DATA  m_FindFileData;
    HANDLE           m_hFound;
    char             m_szPrefix[MAX_PATH];
    char             m_szFull[MAX_PATH];
};

class FrameSequencer
{
public:
    FrameSequencer()  { m_pStream = NULL;};
    ~FrameSequencer() { Close();};

    BOOL Initialize( const char* pImageNames)
    {   
        Close();
        return m_ff.Open(pImageNames);
    }

    void Close()
    {
        m_curframe = 0;
        m_bDone    = FALSE;
        if ( m_pStream )
        {
            m_pStream->Release();
            m_pStream = NULL;
        }
        m_ff.Close();
    }

    CVisRGBAByteImage& Next()
    {
        if ( !m_bDone )
        {
            readagain:
            if ( m_pStream )
            {
                if ( m_curframe < m_pStream->FrameCount() )
                {
                    m_curimage.SetStreamHandler(m_pStream,m_curframe);
                    m_curimage.ReadFile();
                    m_curframe++;
                    return m_curimage;
                }
                else if ( !m_ff.Next() )
                {
                    m_bDone = TRUE;
                    return m_curimage;
                }
            }

            char *pname, *pext;
            m_ff.FileName(&pname, &pext);

            if ( pext && stricmp(pext,".avi") == 0 )
            {
                if ( m_pStream )
                    m_pStream->Release();
                m_pStream = OpenStreamHandler(pname, OF_READ);
                m_curframe = 0;
                goto readagain;
            }
            else
            {
                m_curimage.ReadFile(pname);
            }
        }

        if ( !m_ff.Next() )
            m_bDone = TRUE;

        return m_curimage;
    }

private:
    FindFiles          m_ff;
    CVisStreamHandler* m_pStream;
    UINT               m_curframe;
    CVisRGBAByteImage  m_curimage;
    BOOL               m_bDone;
};

static BOOL AddImages(CVisRGBAByteSequence& seq, const char* pImageNames)
{
    FindFiles ff;

    if ( !ff.Open(pImageNames) )
        return FALSE;

    UINT frstpos = seq.Length();
    while (1)
    {
        char *pname, *pext;
        ff.FileName(&pname, &pext);
        if ( pext && stricmp(pext,".avi") == 0 )
        {
            seq.AppendStream(pname);
        }
        else
        {
            seq.AddFile(pname);
        }

        if ( !ff.Next() )
            break;
    }
    ff.Close();

    // XXX mattu need to do this:
    //SortFileNames( seq, frstpos, seq.Length()-frstpos );

    return TRUE;
}

static BOOL GrowShape( CVisShape& shape, UINT& framecnt,
                       const char* pImageNames, BOOL bHorizontal)
{
    FindFiles ff;

    if ( !ff.Open(pImageNames) )
        return FALSE;

    char *pname, *pext;
    ff.FileName(&pname, &pext);

    CVisRGBAByteImage img;

    if ( pext && stricmp(pext,".avi") == 0 )
    {
        CVisStreamHandler* pStream = OpenStreamHandler(pname, OF_READ);
        img.SetStreamHandler(pStream,0);

        if ( pStream->FrameCount() > framecnt )
            framecnt = pStream->FrameCount();

        shape.SetRight(shape.Right() + pStream->Width());
        if ( shape.Height() < pStream->Height() )
            shape.SetBottom(shape.Top() + pStream->Height());

        pStream->Release();
    }
    else
    {
        img.ReadFile(pname);

        if ( framecnt==0 )
            framecnt = 1;
        shape.SetRight(shape.Right() + img.Width());
        if ( shape.Height() < img.Height() )
            shape.SetBottom(shape.Top() + img.Height());
    }

    ff.Close();

    return TRUE;
}

static BOOL ConCatImages(CVisRGBAByteSequence& seq, const CVisShape& shape, 
                         POINT& pointDest, FrameSequencer& sequencer, int idx,
                         BOOL bHorizontal)
{
    if ( idx >= seq.Length() )
    {
        seq.PushBack(shape);
    }

    CVisImageBase& imgdst = seq.At(idx);
    assert( imgdst.Shape() == shape );

    CVisRGBAByteImage& imgsrc = sequencer.Next();
    imgsrc.CopyPixelsTo(imgdst, pointDest);

    pointDest.x += imgsrc.Width();

    // XXX mattu need to do this:
    //SortFileNames( seq, frstpos, seq.Length()-frstpos );

    return TRUE;
}

#if VIS_INTERNAL_RELEASE
static void
Warp(CVisRGBAByteImage& src, CVisRGBAByteImage& dst, float k1, float k2 )
{
    CVisFloatImage uv = VisCylindricalUV(src, dst, 1.0, 0.0, 0.0, 
                                         VIS_WARP_NONE, false, 1, k1, k2, true);
    VisInverseWarp(src, dst, uv, VIS_INTERP_CUBIC, 1, 1, 0);
}
#endif // VIS_INTERNAL_RELEASE


static void
WarpedShape( CVisShape& shape, float fk1, float fk2 )
{
    float fh = (float)shape.Height() / 2.;
    float fw = (float)shape.Width()  / 2.;
    float rdist2  = fw*fw + fh*fh;

    float correction = 1. + fk1 * rdist2 + fk2 * rdist2 * rdist2;
    float xundist = fw * correction;
    float yundist = fh * correction;

    shape.InflateRect( int(xundist - fw),
                       int(yundist - fh) );
}

void Usage(const char* progname)
{
#if VIS_INTERNAL_RELEASE
    fprintf(stderr,"%s: [-range first last] [-bpp {24|32}] [-warp k1 k2] [-aud] [-cath] input1 [input2 input3 ...] output.avi|output%%d.{bmp,ppm,...}\n",progname);
#else // VIS_INTERNAL_RELEASE
    fprintf(stderr,"%s: [-range first last] [-bpp {24|32}] [-aud] [-cath] input1 [input2 input3 ...] output.avi|output%%d.{bmp,ppm,...}\n",progname);
#endif // VIS_INTERNAL_RELEASE
    exit(-1);
}

int main(int argc, const char *argv[])
{
    if ( argc < 3 )
        Usage(argv[0]);

    try
    {
        CVisImageBase::SetIsAlphaWritten(false); // don't write alpha
        int   frstimg = 0;
        int   lastimg = -1;
        bool  bunwarp = false;
        bool  bconcath   = false;
        bool  bAddSilentAudio = false;
        float fk1, fk2;
        int bpp      = 32;
        const char** pargs = argv + 1;
        argc--;
        while (1)
        {
            if (strcmp(pargs[0], "-range") == 0 )
            {
                if ( argc < 5 ) Usage( argv[0] );

                frstimg = atoi( pargs[1] );
                lastimg = atoi( pargs[2] );

                argc -= 2; pargs += 2;
            }
            else if ( strcmp(pargs[0], "-bpp") == 0 )
            {
                if ( argc < 4 ) Usage( argv[0] );

                bpp = atoi( pargs[1] );
                if ( bpp != 24 && bpp != 32 ) Usage( argv[0] );

                argc--; pargs++;
            }
            else if ( strcmp(pargs[0], "-warp") == 0 )
            {
                if ( argc < 5 ) Usage( argv[0] );

                fk1 = atof( pargs[1] );
                fk2 = atof( pargs[2] );
                bunwarp = true;

                argc -= 2; pargs += 2;

            }
            else if ( strcmp(pargs[0], "-cath") == 0 )
            {
                if ( argc < 3 ) Usage( argv[0] );

                bconcath = true;
            }
            else if ( strcmp(pargs[0], "-aud") == 0 )
            {
                if ( argc < 3 ) Usage( argv[0] );

                bAddSilentAudio = true;
            }
            else
            {
                break;
            }
            argc--;
            pargs++;
        }

        if ( argc < 2 )
            Usage( argv[0] );

        char* pext = strrchr( pargs[argc-1], '.' );

        if ( !pext )
            Usage(argv[0]);

        // load the images
        CVisRGBAByteSequence seq;
        int eopts = seq.SequenceOptions();
        eopts &= ~(evissequenceMaskDelayRead|evissequenceMaskLimitMemoryUsage);
        eopts |=  evissequenceDelayReadUntilNeeded|evissequenceLimitMemoryUsage;
        seq.SetSequenceOptions(eopts);

        // open a stream handler
        CVisStreamHandler* pStrmHandler = NULL;
        if ( stricmp(pext,".avi") == 0 )
        {
            pStrmHandler = OpenStreamHandler(pargs[argc-1], 
                                             OF_READWRITE | OF_CREATE,
                                             bpp, TRUE );
        }

        UINT uFrameCount = 0;

        if ( bconcath )
        {
            CVisShape shape(0,0,0,0);
            FrameSequencer *psequencers = new FrameSequencer[argc-1];

            for ( int i=0; i<argc-1; i++ )
            {
                GrowShape( shape, uFrameCount, pargs[i], TRUE );
                psequencers[i].Initialize(pargs[i]);
            }

            for ( i = 0; i < uFrameCount; i++ )
            {
                POINT pointDest = {0,0};

                for ( int j=0; j<argc-1; j++ )
                {
                    ConCatImages( seq, shape, pointDest, psequencers[j], 
                                  i, TRUE );
                }

                CVisRGBAByteImage& refimage = seq.At(i);
                if ( pStrmHandler )
                {
                    refimage.SetStreamHandler(pStrmHandler, i);
                    refimage.WriteFile();
                }
                else
                {
                    char szFileName[MAX_PATH];

                    sprintf(szFileName,pargs[argc-1],i+frstimg);
                    refimage.WriteFile(szFileName);
                }

                if ( (i%64) == 63 )
                    fprintf(stderr,"Fininshed frame %d\n",i);
            }

            delete [] psequencers;
        }
        else
        {
            for ( UINT i=0; i<argc-1; i++ )
            {
                AddImages( seq, pargs[i] );
            }

            // optionally edit the seq
            if ( lastimg != -1 && lastimg < seq.Length() )
            {
                seq.Erase( lastimg, seq.Length() );
            }
            if ( frstimg > 0 )
            {
                seq.Erase( 0, frstimg - 1 );
            }

#if VIS_INTERNAL_RELEASE
            CVisShape OutShape = seq[0].Shape();
            if ( bunwarp )
            {
                WarpedShape( OutShape, fk1, fk2 );
            }
#endif // VIS_INTERNAL_RELEASE

            char szFileName[MAX_PATH];
            uFrameCount = seq.Length(); 
            for ( i=0; i<uFrameCount; i++ )
            {
                CVisRGBAByteImage outimg;
#if VIS_INTERNAL_RELEASE
                CVisRGBAByteImage warped(OutShape);
                if ( bunwarp )
                {
                    Warp(seq[i], warped, fk1, fk2);
                    outimg = warped;
                }
                else
#endif // VIS_INTERNAL_RELEASE
                {
                    outimg = seq[i];
                }

                if ( pStrmHandler )
                {
                    pStrmHandler->WriteFrame( outimg, i );
                }
                else
                {
                    sprintf(szFileName,pargs[argc-1],i+frstimg);
                    outimg.WriteFile(szFileName);
                }

                if ( (i&0xf)==0xf )
                {
                    fprintf(stderr,"done writing ");
                    if ( pStrmHandler )
                    {
                        fprintf(stderr," frame # %d\n",i+frstimg);
                    }
                    else
                    {
                        fprintf(stderr,"%s\n",szFileName);
                    }
                }
            }

        }

        if ( bAddSilentAudio )
        {
            INT64 auddur = pStrmHandler->TimeFromFrameCount( uFrameCount );

            CVisAudioChannel mono(8000);
            UINT uNumSamp;
            float* pAudio = mono.StartWrite(0, auddur, &uNumSamp);
            memset( pAudio, 0, sizeof(float)*uNumSamp );
            mono.EndWrite(0, auddur);

            CVisAudioData audio;
            audio.InitializeChannel( 0, &mono, 0, auddur );
            pStrmHandler->WriteAudio( audio, 0 );
        }

        if ( pStrmHandler )
        {
            pStrmHandler->Release();
        }

    }
    catch (CVisError &err)
    {
        fprintf(stderr, "%s error: %s\n", argv[0], err.FullMessage());
        exit(-1);
    }
    catch (...)
    {
        Usage(argv[0]);
        exit(-1);
    }

    return 0;
}
