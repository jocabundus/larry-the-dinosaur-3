#include "modules/module-video/inc/palette256.bi"
#include "modules/module-video/inc/videosprites.bi"

const PAL_DIR = "../gfx/palettes/"
const BSV_DIR = "../gfx/"
const BMP_DIR = "../gfx/bmp/"

dim as Video videoHandle
dim as VideoSprites sprites
dim as string window_title
dim as integer scrn_w, scrn_h, fullscreen, zoom

window_title = "PUT-to-BMP Converter"
scrn_w       = 320
scrn_h       = 240
fullscreen   = 0
zoom         = 1

if VideoHandle.init(window_title, scrn_w, scrn_h, fullscreen, zoom) <> 0 then
    print VideoHandle.getErrorMsg()
    end
end if
    
dim files(80, 1) as string = {_
    {"aldog"   , "_____.pal"},_
    {"badmark" , "_____.pal"},_
    {"bigboss1", "grada.pal"},_ '// grada.pal
    {"bigboss3", "_____.pal"},_
    {"blank10" , "_____.pal"},_
    {"blobmine", "_____.pal"},_
    {"bones"   , "_____.pal"},_
    {"bones2"  , "grad0.pal"},_   '// grad0.pal
    {"chopper" , "_____.pal"},_
    {"gblob"   , "_____.pal"},_
    {"general" , "_____.pal"},_
    {"lary"    , "grad0.pal"},_     '// grad0.pal
    {"lary2"   , "grad0.pal"},_    '// grad0.pal
    {"larysnow", "_____.pal"},_
    {"laryswim", "_____.pal"},_
    {"larywgun", "_____.pal"},_
    {"lastbss1", "_____.pal"},_
    {"lastbss2", "_____.pal"},_
    {"ld3boss1", "_____.pal"},_
    {"ld3boss3", "_____.pal"},_
    {"ld3boss4", "grada.pal"},_ '// grada.pal
    {"ld3doors", "_____.pal"},_
    {"ld3exp"  , "_____.pal"},_
    {"ld3fire" , "_____.pal"},_
    {"ld3fly"  , "_____.pal"},_
    {"ld3font" , "_____.pal"},_
    {"ld3gplts", "_____.pal"},_
    {"ld3icons", "_____.pal"},_
    {"ld3jello", "_____.pal"},_
    {"ld3objs" , "_____.pal"},_
    {"ld3objs0", "grad0.pal"},_ '// grad0.pal
    {"ld3objs1", "grada.pal"},_ '// grada.pal
    {"ld3objs2", "grada.pal"},_ '// grada.pal
    {"ld3objs3", "grado.pal"},_ '// grado.pal
    {"ld3objsc", "_____.pal"},_
    {"ld3objsf", "grada.pal"},_ '// grada.pal
    {"ld3objsg", "grada.pal"},_ '// grada.pal
    {"ld3objsr", "_____.pal"},_
    {"ld3objss", "gradd.pal"},_ '// gradd.pal
    {"ld3objsz", "grada.pal"},_ '// grada.pal
    {"ld3plats", "_____.pal"},_
    {"ld3rat"  , "_____.pal"},_
    {"ld3rock" , "_____.pal"},_
    {"ld3rockb", "_____.pal"},_
    {"ld3scrpt", "_____.pal"},_
    {"ld3scrts", "_____.pal"},_
    {"ld3talk" , "_____.pal"},_
    {"ld3tg2"  , "_____.pal"},_
    {"ld3tildb", "_____.pal"},_
    {"ld3tile0", "grad0.pal"},_ '// grad0.pal
    {"ld3tile1", "grada.pal"},_ '// grada.pal
    {"ld3tile3", "_____.pal"},_
    {"ld3tile4", "_____.pal"},_
    {"ld3tile6", "grada.pal"},_ '// grada.pal
    {"ld3tileb", "grad0.pal"},_ '// grad0.pal
    {"ld3tilec", "_____.pal"},_
    {"ld3tiled", "gradd.pal"},_ '// gradd.pal
    {"ld3tilef", "grada.pal"},_ '// grada.pal
    {"ld3tileg", "_____.pal"},_
    {"ld3tileh", "grada.pal"},_ '// grada.pal
    {"ld3tileo", "grado.pal"},_ '// grado.pal
    {"ld3tilep", "grado.pal"},_ '// grado.pal
    {"ld3tiler", "_____.pal"},_
    {"ld3tiley", "_____.pal"},_
    {"ld3tilez", "grada.pal"},_ '// grada.pal
    {"ld3troop", "_____.pal"},_
    {"lstbss1f", "_____.pal"},_
    {"lstbss2f", "_____.pal"},_
    {"mark"    , "_____.pal"},_
    {"megafly" , "_____.pal"},_
    {"msonic"  , "_____.pal"},_
    {"msonic2" , "_____.pal"},_
    {"newrock" , "_____.pal"},_
    {"rusty"   , "_____.pal"},_
    {"rusty2"  , "grad0.pal"},_   '// grad0.pal
    {"rustyhrt", "_____.pal"},_
    {"spider"  , "_____.pal"},_
    {"spider2" , "_____.pal"},_
    {"spider3" , "_____.pal"},_
    {"spkehead", "_____.pal"},_
    {"xahn"    , "_____.pal"}_
}

dim pal_obj as Palette256
dim as string pal_file, bsv_file, bmp_file
dim as integer sprite_w, sprite_h

dim i as integer
for i = 0 to 81
    if i < 81 then
        pal_file = PAL_DIR+iif(files(i, 1) <> "_____.pal", files(i, 1), "grada.pal")
        bsv_file = BSV_DIR+files(i, 0)+".put"
        bmp_file = BMP_DIR+files(i, 0)+".bmp"
        sprite_w = 20
        sprite_h = 20
    else
        pal_file = PAL_DIR+"grada.pal"
        bsv_file = BSV_DIR+"font1.put"
        bmp_file = BMP_DIR+"font1.bmp"
        sprite_w   = 6
        sprite_h   = 5
    end if
    pal_obj.init(256)
    pal_obj.loadRGB256 pal_file
    sprites.init( @videoHandle )
    sprites.setPalette @pal_obj
    sprites.loadBsv bsv_file, sprite_w, sprite_w
    sprites.saveBmp8bit bmp_file
    sprites.release
    pal_obj.release
next i

videoHandle.release
end
