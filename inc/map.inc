' Code to load level data from disk
' Requires: cmap

CONST MAPTILESIZE=16
CONST MAPOFFSET_X=6
CONST MAPOFFSET_Y=6
CONST TILESPERROW=40
CONST NUMTILES=TILESPERROW*15

CONST MAP_OBJECTSMAX=20

DIM Integer Map.W=37
DIM Integer Map.H=21
DIM Integer Map.map(Map.W,Map.H)
DIM Integer Map.Masks(NUMTILES)

DIM Float   Map.Obj(MAP_OBJECTSMAX,3)
DIM Integer Map.Obj.next

'Load the level map and stores it in Level.Map
sub Map.load(name$)
  LOCAL char$,tmp$
  LOCAL r%,c%

  on error abort
  open name$ for input as #1
  if MM.ERRNO>0 then exit sub

  do
    char$=input$(1,#1)
    if char$="," then
       Map.map(c%,r%)=val(tmp$)
       inc c%:tmp$=""
    elseif char$=chr$(10) then
       Map.map(c%,r%)=val(tmp$)
       inc r%:c%=0:tmp$=""
    elseif char$=chr$(13) then
      'nop
    else
      tmp$=tmp$+char$
    endif
  loop while EOF(#1)=0
  close #1

  if tmp$<>"" then Map.map(c%,r%)=val(tmp$)
  Map.Obj.next=0
end sub

sub Map.addObject(x!,y!,w%,h%)
  if Map.Obj.next<MAP_OBJECTSMAX-1 then
    Map.Obj(Map.Obj.next,0)=Map.toScrX(x!)
    Map.Obj(Map.Obj.next,1)=Map.toScrY(y!)
    Map.Obj(Map.Obj.next,2)=w%
    Map.Obj(Map.Obj.next,3)=h%
    inc Map.Obj.next
  endif
end sub

sub Map.draw()
  LOCAL c%,r%,tile%
  LOCAL Integer sx,sy,tx,ty

  PAGE write PAGE_PLAYFIELD
  for r%=0 to Map.H-1
    for c%=0 to Map.W-1
      tile%=Map.map(c%,r%)
      if tile%>=0 then
        sx=(tile% mod TILESPERROW)*MAPTILESIZE
        sy=int(tile%/TILESPERROW)*MAPTILESIZE
        tx=Map.toScrX(c%*MAPTILESIZE) : ty=Map.toScrY(r%*MAPTILESIZE)
        blit sx,sy,tx,ty,MAPTILESIZE,MAPTILESIZE,PAGE_TILESET,&B100
      endif
    next
  next

[MAP] frame(Playfield.X+MAPOFFSET_X,Playfield.Y+MAPOFFSET_Y,MAP.W*MAPTILESIZE,MAP.H*MAPTILESIZE)
[MAP] LOCAL Integer n
[MAP] for n=0 to Map.Obj.next-1
[MAP]  frame(Map.Obj(n,0),Map.Obj(n,1),Map.Obj(n,2),Map.Obj(n,3))
[MAP] next

  PAGE write PAGE_BUFFER
end sub

sub Map.putTile(idx%,c%,r%)
  LOCAL Integer sx,sy,tx,ty

  PAGE write PAGE_PLAYFIELD
  if idx%>=0 then
    sx=(idx% mod TILESPERROW)*MAPTILESIZE
    sy=int(idx%/TILESPERROW)*MAPTILESIZE
    tx=Map.toScrX(c%*MAPTILESIZE) : ty=Map.toScrY(r%*MAPTILESIZE)
    blit sx,sy,tx,ty,MAPTILESIZE,MAPTILESIZE,PAGE_TILESET,&B100
  endif
  PAGE write PAGE_BUFFER
end sub

function Map.hitByPanzer(rect1!(),nx!,ny!,px!,py!) as Integer
  LOCAL Float rect2!(7),pd,nd,onx,ony
  LOCAL Integer n

  for n=0 to Map.Obj.next-1
    onx=Map.Obj(n,0)+Map.Obj(n,2)/2
    ony=Map.Obj(n,1)+Map.Obj(n,3)/2
    pd=_distance(px!,py!,onx,ony)
    nd=_distance(nx!,ny!,onx,ony)
    if pd>nd then
      _makeRectangle(rect2!(),onx,ony,Map.Obj(n,2),Map.Obj(n,3),0)
[MAP] _drawRectangle rect2!(),map(4)
      if _doOverlap(rect1!(),rect2!()) then Map.hitByPanzer=1 : exit for
    endif
  next
end function

' x,y = playfield coordinates
function Map.hitByBullet(x!,y!) as Integer
  LOCAL Integer tx,ty,tile,mask
  LOCAL Float _x,_y

  _x=Map.toMapX(x!)      : _y=Map.toMapY(y!)
  tx=int(_x/MAPTILESIZE) : if tx<0 or tx>MAP.W then exit function
  ty=int(_y/MAPTILESIZE) : if ty<0 or ty>MAP.H then exit function
  tile=Map.map(tx,ty)    : if tile<=0 then exit function
      
  mask=Map.calcMask(tx,ty,_x,_y,4,0)
  if mask AND Map.Masks(tile) then
    Map.hitByBullet=1
    exit function
  endif
end function

function Map.calcMask(tx%,ty%,x!,y!,w%,h%) as Integer
  LOCAL Float x1=x!-tx%*MAPTILESIZE,x2=x1+w%
  LOCAL Float y1=y!-ty%*MAPTILESIZE,y2=y1+h%
  LOCAL Integer mask,row,n

  if x1<0 then x1=0:if y1<0 then y1=0
  if x2>MAPTILESIZE then x2=MAPTILESIZE
  if y2>MAPTILESIZE then y2=MAPTILESIZE
  row=2^(4-fix(x1/4))-1 AND MAPTILESIZE-2^(4-fix(x2/4))
    
  for n=0 to 3
    mask=mask<<4
    if n*4<=y2 AND (n+1)*4>y1 then mask=mask OR row
  next
  Map.calcMask=mask
end function

function Map.calcTileMask(tile%) as Integer
  LOCAL Integer addr=MM.INFO(Page Address PAGE_TILESET)
  LOCAL Integer col=tile% mod TILESPERROW
  LOCAL Integer row=fix(tile%/TILESPERROW)
  LOCAL Integer mask,part,n1,n2,n3

  inc addr,col*MAPTILESIZE+row*640*MAPTILESIZE

  for n3=0 to 3 ' 4 rows = 16 bit
    for n2=0 to 3 ' row = 4 bit
      part=0
      for n1=0 to 3  'One group 4x4 = 1 bit
        inc part,peek(BYTE addr+n1+0*640+4*n2)
        inc part,peek(BYTE addr+n1+1*640+4*n2)
        inc part,peek(BYTE addr+n1+2*640+4*n2)
        inc part,peek(BYTE addr+n1+3*640+4*n2)
      next
      mask=mask<<1
      if part>0 then mask=mask OR 1  
    next
    inc addr,4*640
  next
  Map.calcTileMask=mask
end function

function Map.toMapX(x!) as Float
  Map.toMapX=x!-Playfield.X-MAPOFFSET_X
end function

function Map.toScrX(x!) as Float
  Map.toScrX=x!+Playfield.X+MAPOFFSET_X
end function

function Map.toMapY(y!) as Float
  Map.toMapY=y!-Playfield.Y-MAPOFFSET_Y
end function

function Map.toScrY(y!) as Float
  Map.toScrY=y!+Playfield.Y+MAPOFFSET_Y
end function




