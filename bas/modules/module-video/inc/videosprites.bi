#pragma once
#inclib "videosprites"
#include once "palette256.bi"
#include once "video.bi"

type VideoSpritesMetrics
    
    top as integer
    btm as integer
    lft as integer
    rgt as integer
    
end type

type VideoSprites
private:
    redim _metrics(0) as VideoSpritesMetrics
    redim _sprites(0) as SDL_Rect
    _texture   as SDL_Texture ptr
    _pixel_format as SDL_PixelFormat ptr
    _surface   as SDL_Surface ptr
    _pixels    as any ptr
    _renderer  as SDL_Renderer ptr
    _palette   as Palette256 ptr
    _sdl_palette as SDL_Palette ptr
    _sdl_colors(255) as SDL_Color
    _canvas_w as integer
    _canvas_h as integer
    _w as integer
    _h as integer
    _center_x as integer
    _center_y as integer
    _transparent_index as integer
    _colormod as integer
    _alphamod as integer
    _count as integer
    declare sub _reset
    declare sub _buildMetrics(crop as integer = 0)
    declare sub _erase(sprite_id as integer, quad as integer = 0)
    declare sub _textureToPixels(byval texture as SDL_Texture ptr, byref pixels as any ptr, byref size_in_bytes as uinteger, byref pitch as integer)
    declare sub _textureToSurface(byval texture as SDL_Texture ptr, byref surface as SDL_Surface ptr)
    declare function _getPixel(x as integer, y as integer) as uinteger
    declare sub _putPixel(x as integer, y as integer, colr as integer)
    declare function _getRenderTarget() as SDL_Texture ptr
public:
    declare sub init(v as Video ptr)
    declare sub release()
    declare sub setCenter(x as integer, y as integer)
    declare sub resetCenter()
    declare sub setPalette(p as Palette256 ptr)
    declare function setAsTarget(renderTarget as SDL_Texture ptr = 0) as SDL_Texture ptr
    declare sub loadBsv(filename as string, w as integer, h as integer, crop as integer = 0)
    declare sub loadBmp(filename as string, w as integer = 0, h as integer = 0, crop as integer = 0)
    declare sub saveBmp(filename as string, xscale as double = 1.0, yscale as double = 1.0)
    declare sub saveBmp8bit(filename as string, xscale as double = 1.0, yscale as double = 1.0)
    declare sub dice(w as integer = 0, h as integer = 0)
    declare sub setTransparentColor(index as integer)
    declare sub putToScreen(sprite_id as integer, x as integer, y as integer)
    declare sub putToScreenEx(sprite_id as integer, x as integer, y as integer, flip_horizontal as integer = 0, rotation_angle as double = 0, crop as SDL_RECT ptr = 0, dest as SDL_RECT ptr = 0)
    declare sub setColorMod(colr as integer)
    declare sub setAlphaMod(a as integer)
    declare sub getMetrics(byval sprite_id as integer, byref x as integer, byref y as integer, byref w as integer, byref h as integer)
    declare sub convertPalette(p as Palette256 ptr)
    declare sub setBlendMode(blendmode_id as integer)
    declare sub modSize(sprite_id as integer, w as integer, h as integer)
    declare function getColorMod() as integer
    declare function getAlphaMod() as integer
    declare function getCount() as integer
end type
