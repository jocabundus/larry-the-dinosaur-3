#include once "inc/font.bi"
#include once "inc/video.bi"
#include once "inc/videosprites.bi"

declare function fontVal(ch as string) as integer

dim shared VideoHandle as Video ptr
dim shared Sprites as VideoSprites
dim shared FONT_W as integer
dim shared FONT_H as integer
dim shared FONT_SAVE_COLOR as integer
dim shared FONT_SAVE_ALPHA as integer
dim shared CANVAS_W as integer
dim shared CANVAS_H as integer

sub Font_Init(videoptr as Video ptr)
    
    VideoHandle = videoptr
    Sprites.init( VideoHandle )
    VideoHandle->getScreenSize( CANVAS_W, CANVAS_H )
    
end sub

sub Font_Release
    
    Sprites.release
    
end sub

sub Font_Load(filename as string, fontw as integer, fonth as integer, transparentColor as integer = &hff00ff)
    
    FONT_W = fontw
    FONT_H = fonth
    
    Sprites.loadBMP(filename, FONT_W, FONT_H)
    
end sub

sub Font_ModSize(sprite_id as integer, w as integer, h as integer)
    Sprites.modSize( sprite_id, w, h )
end sub

sub Font_SetCanvasSize(w as integer, h as integer)
end sub

sub Font_centerText(text as string, y as integer)
    
    dim as integer x, w
    
    w = FONT_W * len(text)
    x = int((CANVAS_W - w) * 0.5)
    Font_putText text, x, y
    
end sub

sub Font_put(sprite_id as integer, x as integer, y as integer)
    Sprites.putToScreen( sprite_id, x, y )
end sub

sub Font_putCenter(sprite_id as integer, y as integer)
    
    dim as integer x, w
    
    Sprites.getMetrics( sprite_id, 0, 0, w, 0 )
    x = int((CANVAS_W - w) * 0.5)
    Font_put sprite_id, x, y
    
end sub

sub Font_putText(text as string, x as integer, y as integer)
    
    dim n as integer
    
    for n = 1 to len(text)
        if mid(text, n, 1) <> " " then
            Sprites.putToScreen( fontVal(mid(text, n, 1)), (n * FONT_W - FONT_W) + x, y )
        end if
    next n
    
end sub

sub Font_putColorText(text as string, x as integer, y as integer, colr as integer)
    
    dim n as integer
    
    for n = 1 to len(text)
        if mid(text, n, 1) <> " " then
            Sprites.putToScreen( fontVal(mid(text, n, 1)), (n * FONT_W - FONT_W) + x, y )
        end if
    next n
    
end sub

sub Font_Metrics(byval sprite_id as integer, byref x as integer, byref y as integer, byref w as integer, byref h as integer)
    Sprites.getMetrics sprite_id, x, y, w, h
end sub

sub Font_SetColor(colr as integer)
    if colr >= 0 then
        Sprites.setColorMod(colr)
    else
        if colr = -2 then FONT_SAVE_COLOR = Sprites.getColorMod()
        if colr = -1 then Sprites.setColorMod(FONT_SAVE_COLOR)
    end if
end sub

sub Font_SetAlpha(a as double)
    if a >= 0 then
        Sprites.setAlphaMod(int(a * 255))
    else
        if a = -2 then FONT_SAVE_ALPHA = Sprites.getAlphaMod()
        if a = -1 then Sprites.setAlphaMod(FONT_SAVE_ALPHA)
    end if
end sub

private function fontVal(ch as string) as integer
    
    dim v as integer
    
    v = asc(ch)-32
    
    return v
    
end function
