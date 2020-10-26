#pragma once
#inclib "video"
#include once "palette256.bi"
#include once "SDL2/SDL.bi"

enum VideoBlendModes
    None  = 0
    Blend = 1
    Add   = 2
    Modulate = 3
end enum

type Video
private:
    
    _window as SDL_Window ptr
    _renderer as SDL_Renderer ptr
    _palette as Palette256 ptr

    _fullscreen as integer
    _screen_w as integer
    _screen_h as integer
    
    _error_msg as string
    
    declare sub _reset()
    declare function _getRenderTarget() as SDL_Texture ptr

public:

    declare function getErrorMsg() as string
    declare function init(window_title as string, screen_w as integer, screen_h as integer, is_fullscreen as integer, zoom as double=1.0) as integer
    declare sub release()
    declare sub getScreenSize(byref w as integer, byref h as integer)
    declare sub getWindowSize(byref w as integer, byref h as integer)
    declare sub setPalette(p as Palette256 ptr)
    declare function getRenderer() as SDL_Renderer ptr
    declare sub clearScreen(col as integer)
    declare sub loadBmp(filename as string)
    declare sub saveBmp(filename as string, xscale as double = 1.0, yscale as double = 1.0)
    declare sub putPixel(x as integer, y as integer, colr as integer)
    declare sub fillScreen(colr as integer, a255 as integer = &hff)
    declare sub fill(x as integer, y as integer, w as integer, h as integer, col as integer, a255 as integer = &hff)
    declare sub outline(x as integer, y as integer, w as integer, h as integer, col as integer, size as integer = 1, a255 as integer = &hff)
    declare function setAsTarget(renderTarget as SDL_Texture ptr = 0) as SDL_Texture ptr
    declare sub setBlendMode(blendmode_id as integer)
    declare sub refresh()
    
end type

