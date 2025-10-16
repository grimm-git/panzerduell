

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




