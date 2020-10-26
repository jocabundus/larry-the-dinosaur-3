#pragma once
#inclib "font"

#include once "video.bi"

declare sub Font_Init(videoptr as Video ptr)
declare sub Font_Release()
declare sub Font_Load(filename as string, fontw as integer, fonth as integer, transparentColor as integer = &hff00ff)
declare sub Font_ModSize(sprite_id as integer, w as integer, h as integer)

declare sub Font_centerText(text as string, y as integer)
declare sub Font_put(sprite_id as integer, x as integer, y as integer)
declare sub Font_putCenter(sprite_id as integer, y as integer)
declare sub Font_putText(text as string, x as integer, y as integer)
declare sub Font_putColorText(text as string, x as integer, y as integer, colr as integer)

declare sub Font_Metrics(byval sprite_id as integer, byref x as integer, byref y as integer, byref w as integer, byref h as integer)
declare sub Font_SetColor(fontColor as integer)
declare sub Font_SetAlpha(a as double)
