' Code to load level data from disk
' Requires: cmap

CONST MAPTILESIZE=16
CONST MAPOFFSET_X=6
CONST MAPOFFSET_Y=6
CONST TILESPERROW=40
CONST NUMTILES=TILESPERROW*30

CONST MAP_OBJECTSMAX=20

DIM Integer Map.W=37
DIM Integer Map.H=21
DIM Integer Map.map(Map.W,Map.H)
DIM Integer Map.Masks(NUMTILES)

DIM Float   Map.Obj(MAP_OBJECTSMAX,3)
DIM Integer Map.Obj.next

DIM Integer Map.Damage(NUMTILES)
DIM Integer Map.DamageM1(20,5)
DIM Integer Map.DamageP1(20,5)


sub Map.init()
  Map.readDamageData
end sub

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

'Load the tileset masks (Map.Masks())
function Map.loadMasks(name$) as Integer
  LOCAL String char$,tmp$
  LOCAL Integer idx

  on error skip
  open name$ for input as #1
  if MM.ERRNO>0 then exit function

  do
    char$=input$(1,#1)
    if char$="," then
       Map.Masks(idx)=val(tmp$)
       inc idx : tmp$=""
    elseif char$=chr$(10) then
       Map.Masks(idx)=val(tmp$)
       inc idx : tmp$=""
    elseif char$=chr$(13) then
      'nop
    else
      tmp$=tmp$+char$
    endif
  loop while EOF(#1)=0
  close #1

  if tmp$<>"" then Map.Masks(idx)=val(tmp$)
  Map.loadMasks=1
end function

sub Map.saveMasks(name$)
  LOCAL Integer n,m
  
  on error abort
  open name$ for output as #1
  if MM.ERRNO>0 then exit sub

  for n=0 to NUMTILES-1
    print #1,str$(Map.Masks(n));
    inc m
    if m=10 then
      m=0
      print #1,""
    else
      print #1,",";
    endif
  next
  print #1,""
  close #1
end sub

sub Map.readDamageData()
  LOCAL Integer n,m,idx,value
  LOCAL Integer n1,p1

  'Prepare Damage Data
  '     -1=ignore
  '0..1199=replace current tile with this tile
  for n=0 to NUMTILES-1
    Map.Damage(n)=-1
  next

  restore MAPDamage
  for n=0 to 299
    read idx,value
    Map.Damage(idx)=value
  next

  for n=0 to 19  'MAPNeighbours
    for m=0 to 4
      read n1,p1
      Map.DamageM1(n,m)=n1
      Map.DamageP1(n,m)=p1
    next  
  next
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
      if tile% >= 0 then
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

sub Map.putTile(tile%,c%,r%)
  LOCAL Integer sx,sy,tx,ty

  PAGE write PAGE_PLAYFIELD
  if tile% >= 0 then
    Map.map(c%,r%)=tile% 'replace tile in map
    sx=(tile% mod TILESPERROW)*MAPTILESIZE
    sy=int(tile%/TILESPERROW)*MAPTILESIZE
    tx=Map.toScrX(c%*MAPTILESIZE) : ty=Map.toScrY(r%*MAPTILESIZE)
    blit tx,ty,tx,ty,MAPTILESIZE,MAPTILESIZE,PAGE_BACKGND,&B100
    blit sx,sy,tx,ty,MAPTILESIZE,MAPTILESIZE,PAGE_TILESET,&B100
  endif
  PAGE write PAGE_BUFFER
end sub

function Map.putDamageTile(tile1%,tx1%,ty1%,tile2%,tx2%,ty2%) as Integer
  if tile1%=-1 then exit function

  Map.putTile tile1%,tx1%,ty1%
  Map.putTile tile2%,tx2%,ty2%
  Map.putDamageTile=1
end function

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
  LOCAL Integer idx,n,m1,p1,ex
  LOCAL Integer tile_M,tile_C,tile_P,dir
  LOCAL Float _x,_y

  _x=Map.toMapX(x!)      : _y=Map.toMapY(y!)
  tx=int(_x/MAPTILESIZE) : if tx<0 or tx>MAP.W then exit function
  ty=int(_y/MAPTILESIZE) : if ty<0 or ty>MAP.H then exit function
  tile=Map.map(tx,ty)    : if tile<=0 then exit function
      
  mask=Map.calcMask(tx,ty,_x,_y,4,0)
  if mask AND Map.Masks(tile) then
    Map.hitByBullet=1

    select case tile
    case 1160 to 1169
      idx=tile-1160 : tile_M=-1 : tile_C=-1 : tile_P=-1
      m1=Map.map(tx-1,ty) : p1=Map.map(tx+1,ty)

      for n=0 to 5
        if Map.DamageM1(idx,n)=m1 then tile_M=Map.Damage(m1) : tile_C=tile_M+1
        if Map.DamageP1(idx,n)=p1 then tile_P=Map.Damage(p1) : tile_C=tile_P-1
        if tile_C<>-1 then exit for
      next

      _x=tx*MAPTILESIZE+MAPTILESIZE/2
      dir=choice(tile AND 1,-1,1)
      ex=EX_TYPE_SMALL

      if tile_C=-1 then tile_C=390-40*dir+idx\2
      if Map.putDamageTile(tile_M,tx-1,ty,tile_M+dir*40,tx-1,ty+dir) then inc _x,-MAPTILESIZE/2 : ex=EX_TYPE_BIG
      n= Map.putDamageTile(tile_C,tx  ,ty,tile_C+dir*40,tx  ,ty+dir)
      if Map.putDamageTile(tile_P,tx+1,ty,tile_P+dir*40,tx+1,ty+dir) then inc _x,MAPTILESIZE/2 : ex=EX_TYPE_BIG

      _x=Map.toScrX(_x) : _y=Map.toScrY((ty+choice(dir<0,0,1))*MAPTILESIZE)
      Explosion.add _x,_y,0,ex
      Map.hitByBullet=2

    case 1170 to 1179
      idx=tile-1160 : tile_M=-1 : tile_C=-1 : tile_P=-1
      m1=Map.map(tx,ty-1) : p1=Map.map(tx,ty+1)

      for n=0 to 5
        if Map.DamageM1(idx,n)=m1 then tile_M=Map.Damage(m1) : tile_C=tile_M+40
        if Map.DamageP1(idx,n)=p1 then tile_P=Map.Damage(p1) : tile_C=tile_P-40
        if tile_C<>-1 then exit for
      next

      _y=ty*MAPTILESIZE+MAPTILESIZE/2
      dir=choice(tile AND 1,-1,1)      
      ex=EX_TYPE_SMALL

      idx=(tile-1170)\2
      if tile_C=-1 then tile_C=197-dir+idx*40
      if Map.putDamageTile(tile_M,tx,ty-1,tile_M+dir,tx+dir,ty-1) then inc _y,-MAPTILESIZE/2 : ex=EX_TYPE_BIG
      n= Map.putDamageTile(tile_C,tx,ty,  tile_C+dir,tx+dir,ty)
      if Map.putDamageTile(tile_P,tx,ty+1,tile_P+dir,tx+dir,ty+1) then inc _y,MAPTILESIZE/2 : ex=EX_TYPE_BIG

      _x=Map.toScrX((tx+choice(dir<0,0,1))*MAPTILESIZE) : _y=Map.toScrY(_y)
      Explosion.add _x,_y,0,ex
      Map.hitByBullet=2

    case else
      if Map.Damage(tile)>=0 then
        Map.putTile Map.Damage(tile),tx,ty
        _x=Map.toScrX(tx*MAPTILESIZE+MAPTILESIZE/2)
        _y=Map.toScrY(ty*MAPTILESIZE+MAPTILESIZE/2)
        Explosion.add _x,_y,0,EX_TYPE_SMALL
        Map.hitByBullet=2
      endif
    end select
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

MAPDamage:
DATA   1,1160,  4,1160,  7,1160, 10,1160, 13,1160
DATA  81,1161, 84,1161, 87,1161, 90,1161, 93,1161
DATA 121,1162,124,1162,127,1162,130,1162,133,1162
DATA 201,1163,204,1163,207,1163,210,1163,213,1163
DATA 241,1164,244,1164,247,1164,250,1164,253,1164
DATA 321,1165,324,1165,327,1165,330,1165,333,1165
DATA 361,1166,364,1166,367,1166,370,1166,373,1166
DATA 441,1167,444,1167,447,1167,450,1167,453,1167
DATA 481,1168,484,1168,487,1168,490,1168,493,1168
DATA 561,1169,564,1169,567,1169,570,1169,573,1169
DATA  55,1170,175,1170,295,1170,415,1170,535,1170
DATA  57,1171,177,1171,297,1171,417,1171,537,1171
DATA  58,1172,178,1172,298,1172,418,1172,538,1172
DATA  60,1173,180,1173,300,1173,420,1173,540,1173
DATA  61,1174,181,1174,301,1174,421,1174,541,1174
DATA  63,1175,183,1175,303,1175,423,1175,543,1175
DATA  64,1176,184,1176,304,1176,424,1176,544,1176
DATA  66,1177,186,1177,306,1177,426,1177,546,1177
DATA  67,1178,187,1178,307,1178,427,1178,547,1178
DATA  69,1179,189,1179,309,1179,429,1179,549,1179
DATA   0, 600,  2, 602,  3, 603,  5, 605,  6, 606 '1160
DATA   8, 608,  9, 609, 11, 611, 12, 612, 14, 614
DATA  80, 720, 82, 722, 83, 273, 85, 725, 86, 726 '1161
DATA  88, 728, 89, 729, 91, 731, 92, 732, 94, 734
DATA 120, 760,122, 762,123, 763,125, 765,126, 766 '1162
DATA 128, 768,129, 769,131, 771,132, 772,134, 774
DATA 200, 880,202, 882,203, 883,205, 885,206, 886 '1163
DATA 208, 888,209, 889,211, 891,212, 892,214, 894
DATA 240, 920,242, 922,243, 923,245, 925,246, 926 '1164
DATA 248, 928,249, 929,251, 931,252, 932,254, 934
DATA 320,1040,322,1042,323,1043,325,1045,326,1046 '1165
DATA 328,1048,329,1049,331,1051,332,1052,334,1054
DATA 360,1080,362,1082,363,1083,365,1085,366,1086 '1166
DATA 368,1088,369,1089,371,1091,372,1092,374,1094
DATA 440, 655,442, 657,443, 735,445, 737,446, 815 '1167
DATA 448, 817,449, 895,451, 897,452, 975,454, 977
DATA 480,  30,482,  32,483,  33,485,  35,486,  36 '1168
DATA 488,  38,489, 190,491, 192,492, 193,494, 195
DATA 560, 150,562, 152,563, 153,565, 155,566, 156 '1169
DATA 568, 158,569, 310,571, 312,572, 313,574, 315
DATA  15, 620, 95, 700,135, 740,215, 820,255, 860 '1170
DATA 335, 940,375, 980,455,1060,495,1100,575,1180
DATA  17, 623, 97, 703,137, 743,217, 823,257, 863 '1171
DATA 337, 943,377, 983,457,1063,497,1103,577,1183
DATA  18, 624, 98, 704,138, 744,218, 824,258, 864 '1172
DATA 338, 944,378, 984,458,1064,498,1104,578,1184
DATA  20, 627,100, 707,140, 747,220, 827,260, 867 '1173
DATA 340, 947,380, 987,460,1067,500,1107,580,1187
DATA  21, 628,101, 708,141, 748,221, 828,261, 868 '1174
DATA 341, 948,381, 988,461,1068,501,1108,581,1188
DATA  23, 631,103, 711,143, 751,223, 831,263, 871 '1175
DATA 343, 951,383, 991,463,1071,503,1111,583,1191
DATA  24, 632,104, 712,144, 752,224, 832,264, 872 '1176
DATA 344, 952,384, 992,464,1072,504,1112,584,1192
DATA  26, 635,106, 715,146, 755,226, 835,266, 875 '1177
DATA 346, 955,386, 995,466,1075,506,1115,586,1195
DATA  27, 636,107, 716,147, 756,227, 836,267, 876 '1178
DATA 347, 956,387, 996,467,1076,507,1116,587,1196
DATA  29, 639,109, 719,149, 759,229, 839,269, 879 '1179
DATA 349, 959,389, 999,469,1079,509,1119,589,1199

MAPNeighbours:
DATA   0,  2,  3,  5,  6,  8,  9, 11, 12, 14 '1160
DATA  80, 82, 83, 85, 86, 88, 89, 91, 92, 94 '1161
DATA 120,122,123,125,126,128,129,131,132,134 '1162
DATA 200,202,203,205,206,208,209,211,212,214 '1163
DATA 240,242,243,245,246,248,249,251,252,254 '1164
DATA 320,322,323,325,326,328,329,331,332,334 '1165
DATA 360,362,363,365,366,368,369,371,372,374 '1166
DATA 440,442,443,445,446,448,449,451,452,454 '1167
DATA 480,482,483,485,486,488,489,491,492,494 '1168
DATA 560,562,563,565,566,568,569,571,572,574 '1169
DATA  15, 95,135,215,255,335,375,455,495,575 '1170
DATA  17, 97,137,217,257,337,377,457,497,577 '1171
DATA  18, 98,138,218,258,338,378,458,498,578 '1172
DATA  20,100,140,220,260,340,380,460,500,580 '1173
DATA  21,101,141,221,261,341,381,461,501,581 '1174
DATA  23,103,143,223,263,343,383,463,503,583 '1175
DATA  24,104,144,224,264,344,384,464,504,584 '1176
DATA  26,106,146,226,266,346,386,466,506,586 '1177
DATA  27,107,147,227,267,347,387,467,507,587 '1178
DATA  29,109,149,229,269,349,389,469,509,589 '1179

