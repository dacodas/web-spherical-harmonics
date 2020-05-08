#include <string>
#include <filesystem>

#ifdef UBUNTU
#include <wand/MagickWand.h>
#include <wand/method-attribute.h>
#include <wand/drawing-wand.h>
#include <wand/pixel-wand.h>
#else
#include <MagickWand/MagickWand.h>
#include <MagickWand/method-attribute.h>
#include <MagickWand/wandcli.h>
#include <MagickWand/drawing-wand.h>
#include <MagickWand/pixel-wand.h>
#endif

void convert(const std::filesystem::path& path)
{
    MagickWand *mw = NULL;
    PixelWand *pw = NULL;

    MagickWandGenesis();

    mw = NewMagickWand();
    pw = NewPixelWand();

    MagickReadImage(mw, path.c_str());

    PixelSetColor(pw, "rgb(0,0,255)");
    MagickRotateImage(mw, pw, -90.0);
    MagickSetImageAlphaChannel(mw, SetAlphaChannel);

    std::string new_filename {
        path.parent_path().string() 
        + "/"
        + path.stem().string() 
        + ".png"
    };
    MagickWriteImage(mw, new_filename.c_str());

    if(mw) mw = DestroyMagickWand(mw);

    MagickWandTerminus();
}

