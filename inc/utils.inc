

' Gets the parent directory of 'f$', or the empty string if it does not have one.
Function getParent$(f$) as String
  LOCAL i%

  For i% = Len(f$) To 1 Step -1
    If InStr("/\", Mid$(f$, i%, 1)) > 0 Then Exit For
  Next i%

  If i% = 0 Then
    getParent$=""
  Else
    getParent$=Left$(f$, i% - 1)
  EndIf
End Function

function isVisible(x!,y!) as Integer
  isVisible=0
  if x!<Screen.VPx then exit function
  if x!>Screen.VPx+VP.W then exit function
  if y!<Screen.VPy then exit function
  if y!>Screen.VPy+VP.H then exit function
  isVisible=1
end function

sub set(arg%,bit%)
  arg%=arg% OR 1<<bit%
end sub

sub clr(arg%,bit%)
  arg%=arg% AND INV(1<<bit%)
end sub

function tst(arg%,bit%) AS Integer
  tst=(arg% AND 1<<bit%)
end function

function cfr(value!) as Float
  cfr=choice(Game.frameTime<13.3,value!,Game.frameTime/13.3*value!)
end function

sub _drawRectangle(rect!(),color%)
  line rect!(0),rect!(1),rect!(2),rect!(3),1,color%
  line rect!(2),rect!(3),rect!(4),rect!(5),1,color%
  line rect!(4),rect!(5),rect!(6),rect!(7),1,color%
  line rect!(6),rect!(7),rect!(0),rect!(1),1,color%
end sub

function _distance(x1!,y1!,x2!,y2!) as float
  LOCAL float dx,dy

  dx=x2!-x1! : dy=y2!-y1!
  _distance=SQR(dx*dx+dy*dy)
end function

sub frame(x%,y%,w%,h%)
  line x%,y%,x%+w%-1,y%
  line x%+w%-1,y%,x%+w%-1,y%+h%-1
  line x%,y%,x%,y%+h%-1
  line x%,y%+h%-1,x%+w%-1,y%+h%-1
end sub





