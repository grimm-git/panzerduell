
CONST ASTAR_NOTREADY=0
CONST ASTAR_READY   =1
CONST ASTAR_RUNNING =2
CONST ASTAR_COMPLETE=3
CONST ASTAR_NOPATH  =4
CONST ASTAR_EXECUTE =5
CONST ASTAR_SLICES=150

CONST PATH_TURN=0
CONST PATH_MOVE=1

DIM Integer AI.Panzer
DIM Integer AI.Active

DIM Integer AI.AStar.State
DIM Integer AI.AStar.spx
DIM Integer AI.AStar.spy
DIM Integer AI.AStar.gpx
DIM Integer AI.AStar.gpy
DIM Integer AI.AStar.oc
DIM Integer AI.AStar.openList(MAP.W*MAP.H)
DIM Integer AI.AStar.closedList(MAP.W*MAP.H)
DIM Float   AI.AStar.gScore(MAP.W,MAP.H)
DIM Float   AI.AStar.fScore(MAP.W,MAP.H)
DIM Integer AI.AStar.predX(MAP.W,MAP.H)
DIM Integer AI.AStar.predY(MAP.W,MAP.H)

DIM Integer AI.Path.next
DIM Integer AI.Path.length
DIM Integer AI.Path.State
DIM Integer AI.Path.X(Map.W+MAP.H)
DIM Integer AI.Path.Y(Map.W+MAP.H)


sub AI.install(idx%)
  AI.Panzer=idx%
  AI.Active=1
  AI.AStar.State=ASTAR_NOTREADY
  AI.Path.State=PATH_TURN
end sub

'x2/y2=target
sub AI.update(x2!,y2!)
  STATIC Integer mc=0
  LOCAL Integer n,intersect=0
  LOCAL Float rx,ry,rw,rh
  LOCAL Float x1,y1,px,py

  if AI.Active=0 then exit sub

  x1=Panzer.X(AI.Panzer) : y1=Panzer.Y(AI.Panzer)

  select case AI.AStar.State
  case ASTAR_NOTREADY
    AI.prepareAStar(x1,y1,x2!,y2!)
  case ASTAR_READY,ASTAR_RUNNING
    AI.findPath()
  case ASTAR_COMPLETE
    AI.extractPath()
    AI.AStar.State=ASTAR_EXECUTE
  case ASTAR_EXECUTE
    px=Map.toScrX(AI.Path.X(AI.Path.next)*MAPTILESIZE+MAPTILESIZE/2)
    py=Map.toScrY(AI.Path.Y(AI.Path.next)*MAPTILESIZE+MAPTILESIZE/2)

    select case AI.Path.State
    case PATH_TURN
      if AI.adjPanzer(px,py,0) then AI.Path.State=PATH_MOVE
    case PATH_MOVE
      rx=x1 : ry=y1
      Panzer.move(AI.Panzer,0.5)
      if rx=Panzer.X(AI.Panzer) AND ry=Panzer.Y(AI.Panzer) then 
        n=AI.adjPanzer(px,py,1)
        mc=0
      else
        if _distance(x1,y1,px,py) < 16 then inc AI.Path.next
        if mc>0 then inc mc,-1 else AI.Path.State=PATH_TURN
      endif
    end select

    text 50,30,"px="+str$(px)+", py="+str$(py),"L",7,,,-1
    text 50,40,"rot="+str$(n),"L",7,,,-1
    circle px,py,2,0,1.0,map(2),map(2)
  end select

  text 50,10,"status="+str$(AI.AStar.State),"L",7,,,-1
  text 50,20,"x="+str$(int(x1))+", y="+str$(int(y1)),"L",7,,,-1
  circle x1,y1,2,0,1.0,map(3),map(3)

  for n=0 to Map.Obj.next-1
    rx=Map.Obj(n,0) : ry=Map.Obj(n,1)
    rw=Map.Obj(n,2) : rh=Map.Obj(n,3)

    if Math.intersect(x1,y1,x2!,y2!,rx   ,ry   ,rx+rw,ry)    then intersect=1 : exit for
    if Math.intersect(x1,y1,x2!,y2!,rx+rw,ry   ,rx+rw,ry+rh) then intersect=1 : exit for
    if Math.intersect(x1,y1,x2!,y2!,rx+rw,ry+rh,rx   ,ry+rh) then intersect=1 : exit for
    if Math.intersect(x1,y1,x2!,y2!,rx   ,ry+rh,rx   ,ry)    then intersect=1 : exit for
  next
[AI] line x1,y1,x2!,y2!,1,map(choice(intersect,1,2))

  AI.adjTurret x2!,y2!
  if intersect=0 then Panzer.fire(AI.Panzer)
end sub

'x2,y2=target
sub AI.adjTurret(x2!,y2!)
  LOCAL Float x1,y1,alpha,beta
  LOCAL Integer dir

  x1=Panzer.X(AI.Panzer) : y1=Panzer.Y(AI.Panzer)
  alpha=DEG(atan2(x2!-x1,y1-y2!))
  if alpha<0 then inc alpha,360
  beta =Panzer.phi(AI.Panzer)+Panzer.turm(AI.Panzer)
  if beta>360 then inc beta,-360

  if abs(alpha-beta)>1 then
    dir=choice(abs(alpha-beta)>180,1,-1)
    Panzer.cannon AI.Panzer,choice(beta>alpha,dir,-dir)
  endif
end sub

'x2,y2=target
function AI.adjPanzer(x2!,y2!,blk%) as Integer
  STATIC Integer dir
  LOCAL Float x1,y1,alpha,beta

  if blk%=1 then
    Panzer.turn AI.Panzer,dir
  else
    x1=Panzer.X(AI.Panzer) : y1=Panzer.Y(AI.Panzer)
    alpha=DEG(atan2(x2!-x1,y1-y2!))
    if alpha<0 then inc alpha,360
    beta =Panzer.phi(AI.Panzer)
    if beta>360 then inc beta,-360

    if abs(alpha-beta)<1 then
      AI.adjPanzer=1
    else
      dir=choice(abs(alpha-beta)>180,1,-1)
      dir=choice(beta>alpha,dir,-dir)
      Panzer.turn AI.Panzer,dir
      AI.adjPanzer=0
    endif
  endif
end function

sub AI.prepareAStar(sx%,sy%,gx%,gy%)
  LOCAL Integer _sx,_sy,_gx,_gy
  LOCAL Integer x,y

  if AI.Astar.State=ASTAR_RUNNING then exit sub

  _sx=int(Map.toMapX(sx%)/MAPTILESIZE)
  AI.AStar.spx=_sx
  _sy=int(Map.toMapY(sy%)/MAPTILESIZE)
  AI.AStar.spy=_sy
  _gx=int(Map.toMapX(gx%)/MAPTILESIZE)
  AI.AStar.gpx=_gx
  _gy=int(Map.toMapY(gy%)/MAPTILESIZE)
  AI.AStar.gpy=_gy

  for x=0 to MAP.W-1  ' Initialize scores
    for y=0 to MAP.H-1
      AI.AStar.gScore(x,y)=9999
      AI.AStar.fScore(x,y)=9999
    next
  next

  ' Setup start node
  AI.AStar.gScore(_sx,_sy)=0
  AI.AStar.fScore(_sx,_sy)=ABS(_gx-_sx) + ABS(_gy-_sy)
  AI.AStar.oc=1
  AI.AStar.openList(AI.AStar.oc)=_sx+_sy*Map.W ' encode coordinates
  AI.AStar.state=ASTAR_READY
end sub

sub AI.findPath()
  LOCAL Integer lowest,moveCost,tentativeG
  LOCAL Integer current,cx,cy,dx,dy,nx,ny,lx,ly
  LOCAL Integer n,found,slice

  if AI.AStar.State=ASTAR_NOTREADY then exit sub
  if AI.AStar.State=ASTAR_COMPLETE then exit sub
  AI.AStar.State=ASTAR_RUNNING

  do while AI.AStar.oc > 0
    inc slice : if slice=ASTAR_SLICES then exit sub

    ' Find node in openList with lowest fScore
    lowest = 1
    for n=2 to AI.AStar.oc
      nx = AI.AStar.openList(n) MOD MAP.W
      ny = int(AI.AStar.openList(n) / MAP.W)
      lx = AI.AStar.openList(lowest) MOD MAP.W
      ly = int(AI.AStar.openList(lowest) / MAP.W)
      if AI.AStar.fScore(nx,ny) < AI.AStar.fScore(lx,ly) then lowest=n
    next

    current = AI.AStar.openList(lowest)
    cx = current MOD MAP.W : cy = int(current / MAP.W)

    ' Goal reached?
    if cx=AI.AStar.gpx AND cy=AI.AStar.gpy then
      AI.AStar.State=ASTAR_COMPLETE
      exit sub
    endif

    ' Remove current from openList
    for n=lowest to AI.AStar.oc-1 : AI.AStar.openList(n)=AI.AStar.openList(n+1) : next
    AI.AStar.closedList(cx+cy*MAP.W)=1
    inc AI.AStar.oc,-1

    ' Explore 8 neighbors
    for dx=-1 to 1
      for dy=-1 to 1
        if dx=0 AND dy=0 then continue for

        ' Check bounds
        nx=cx+dx : ny=cy+dy
        if nx<0 OR nx>=MAP.W OR ny<0 OR ny>=MAP.H then continue for

        ' Check obstacles
        if Map.map(nx,ny)<>-1 OR AI.AStar.closedList(nx+ny*MAP.W)<>0 then continue for

        ' Movement cost: 1 for straight, 1.4 for diagonal
        moveCost=choice(ABS(dx)+ABS(dy)=2,1.4,1)

        tentativeG=AI.AStar.gScore(cx,cy)+moveCost
        if tentativeG < AI.AStar.gScore(nx,ny) then
          AI.AStar.predX(nx,ny)=cx : AI.AStar.predY(nx,ny)=cy
          AI.AStar.gScore(nx,ny)=tentativeG

          ' Heuristic: Euclidean approximation
          AI.AStar.fScore(nx,ny)=AI.AStar.gScore(nx,ny)+SQR((AI.AStar.gpx-nx)^2+(AI.AStar.gpy-ny)^2)

          ' Add to openList if not already there
          found=0
          for n=1 to AI.AStar.oc
            if AI.AStar.openList(n) = nx+ny*MAP.W then found = 1 : exit for
          next
          if found=0 then
            inc AI.AStar.oc
            AI.AStar.openList(AI.AStar.oc)=nx+ny*MAP.W 
          endif
        endif
      next
    next
  loop
  AI.AStar.State=ASTAR_NOPATH
end sub

sub AI.extractPath()
  LOCAL Integer path(2*(MAP.W+MAP.H))
  LOCAL Integer idx,px,py,nx,ny

  px=AI.AStar.gpx : py=AI.AStar.gpy : idx=0
  do while NOT (px=AI.AStar.spx AND py=AI.AStar.spy)
    nx=AI.AStar.predX(px,py) : ny=AI.AStar.predY(px,py)
    path(idx  )=nx : px=nx
    path(idx+1)=ny : py=ny
    inc idx,2
  loop
  AI.Path.length=idx/2-2

  for idx=0 to AI.Path.length
    AI.Path.X(idx)=path(2*(AI.Path.length-idx))
    AI.Path.Y(idx)=path(2*(AI.Path.length-idx)+1)
  next
  AI.Path.next=0
end sub


'(x1/y1),(x2,y2)=One line segment
'(x3/y3),(x4,y4)=Second line segment
function Math.intersect(x1!,y1!,x2!,y2!,x3!,y3!,x4!,y4!) as Integer
  LOCAL Float den,ua,ub,numA,numB

  den=(y4!-y3!)*(x2!-x1!) - (x4!-x3!)*(y2!-y1!)
  if den=0 then exit function
  
  numA=(x4!-x3!)*(y1!-y3!) - (y4!-y3!)*(x1!-x3!)
  numB=(x2!-x1!)*(y1!-y3!) - (y2!-y1!)*(x1!-x3!)
  ua=numA/den : ub=numB/den

  if ua>=0 and ua<=1 and ub>=0 and ub<=1 then Math.intersect=1
[AI] if Math.intersect=1 then line x3!,y3!,x4!,y4!,1,map(7)
end function




