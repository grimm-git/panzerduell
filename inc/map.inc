' Code to load level data from disk
' Requires: cmap

CONST MAPTILESIZE=16
CONST MAPOFFSET_X=6
CONST MAPOFFSET_Y=6
CONST TILESPERROW=40
CONST NUMTILES=TILESPERROW*15

DIM Integer Map.W=37
DIM Integer Map.H=21
DIM Integer Map.map(Map.W,Map.H)
DIM Integer Map.Masks(NUMTILES)

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

' x,y = playfield coordinates
function Map.objectHit(x!,y!,w%,h%,px!,py!) as Integer
  LOCAL Integer tx,_tx,ty,_ty,tile,mask
  LOCAL Float _x,_y
  LOCAL Integer z,rc

  _x=Map.toMapX(x!-w%/2)
  tx=int(_x/MAPTILESIZE) : if tx<0 then tx=0

  _y=mAP.toMapY(y!-h%/2)
  ty=int(_y/MAPTILESIZE) : if ty<0 then ty=0

[MAP] LOCAL Float dx,dy,hx,hy
[MAP] frame (x!-w%/2,y!-h%/2,w%,h%)
[MAP] hy=Map.toScrY(ty*MAPTILESIZE)-12
[MAP] hx=Map.toScrX(tx*MAPTILESIZE)-12

  _ty=ty  
  do
    _tx=tx
    do
      tile=Map.map(_tx,_ty)

[MAP] dx=Map.toScrX(_tx*MAPTILESIZE)
[MAP] dy=Map.toScrY(_ty*MAPTILESIZE)
[MAP] frame (dx,dy,MAPTILESIZE,MAPTILESIZE)
[MAP] text dx+8,dy+6,str$(tile),"C",7
[MAP] text hx,dy+6,str$(_ty),"C",7
[MAP] text dx+8,hy,str$(_tx),"C",7

      if tile>0 then
        mask=Map.calcMask(_tx,_ty,_x,_y,choice(w%=0,4,w%),h%)
        if mask AND Map.Masks(tile) then Map.objectHit=1 : exit function
      endif

      inc _ty : if _ty>=MAP.H then exit do
    loop while _y+h%>_ty*MAPTILESIZE
    _ty=ty
    inc tx : if tx>=MAP.W then exit do
  loop while _x+w%>tx*MAPTILESIZE
  Map.objectHit=0
end function

' x,y = playfield coordinates
function Map.bulletHit(x!,y!) as Integer
  LOCAL Integer tx,ty,tile,mask
  LOCAL Float _x,_y

  _x=Map.toMapX(x!)      : _y=Map.toMapY(y!)
  tx=int(_x/MAPTILESIZE) : if tx<0 or tx>MAP.W then exit function
  ty=int(_y/MAPTILESIZE) : if ty<0 or ty>MAP.H then exit function
  tile=Map.map(tx,ty)    : if tile<=0 then exit function

[MAP] LOCAL Float dx,dy
[MAP] frame (x!,y!,4,4)
[MAP] dx=Map.toScrX(tx*MAPTILESIZE)
[MAP] dy=Map.toScrY(ty*MAPTILESIZE)
[MAP] frame (dx,dy,MAPTILESIZE,MAPTILESIZE)
[MAP] text dx+8,dy+6,str$(tile),"C",7
      
  mask=Map.calcMask(tx,ty,_x,_y,4,0)
  if mask AND Map.Masks(tile) then
    Map.bulletHit=1
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




