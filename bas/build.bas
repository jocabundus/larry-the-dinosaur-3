dim pathToFbc as string = "fbc"
dim args(4) as string

#include once "dir.bi"
if dir("lib", fbDirectory) <> "lib" then
    mkdir "lib"
end if

args(0) = "modules/module-video/palette256.bas     -lib -x lib/libpalette256.a"
args(1) = "modules/module-video/video.bas          -lib -x lib/libvideo.a"
args(2) = "modules/module-video/videobuffer.bas    -lib -x lib/libvideobuffer.a"
args(3) = "modules/module-video/videosprites.bas   -lib -x lib/libvideosprites.a"

print "Building modules..."
dim i as integer
dim percent as double
dim s as string
for i = 0 to 3
    if shell(pathToFbc+" "+args(i)) = -1 then
        print "ERROR while running fbc with: "+args(i)
    else
        percent = (i/3)
        s = "["
        s += string(int(percent*25), "=")
        s += string(25-int(percent*25), " ")
        s += "] "
        s += str(int(percent*100))+"%"
        print s+chr(13);
    end if
next i

args(0) = "-w all convert.bas -p lib/ -exx"

print ""
print "Compiling convert.bas..."
if shell(pathToFbc+" "+args(0)) = -1 then
    print "ERROR while running fbc with: " + args(0)
end if

print "Done!"
