CONST AI.REPAIRDELAY=8000
CONST AI.PATHBUFFER=3
CONST AI.PATHMINDIST=5

CONST AISTATE_OFF=0
CONST AISTATE_WAIT=1
CONST AISTATE_MOVE=2
CONST AISTATE_DESTROYED=3
CONST AISTATE_GOHOME=4
CONST AISTATE_ATHOME=5

CONST AIFLG_TARGET_DEAD=0
CONST AIFLG_HOLD=1
CONST AIFLG_ATTACK=2

CONST AISIG_AI_DEAD=0
CONST AISIG_AI_ALIVE=1
CONST AISIG_TARGET_DEAD=2
CONST AISIG_TARGET_ALIVE=3
CONST AISIG_HOLD=4
CONST AISIG_RELEASE=5

CONST ASTAR_NOTREADY=0
CONST ASTAR_READY=1
CONST ASTAR_RUNNING=2
CONST ASTAR_COMPLETE=3
CONST ASTAR_NOPATH=4
CONST ASTAR_PAUSED=5
CONST ASTAR_SLICES=5


DIM Integer AI.State
DIM Integer AI.Panzer
DIM Integer AI.Panzer.homeX
DIM Integer AI.Panzer.homeY
DIM Float   AI.timRepair
DIM Integer AI.Flags
DIM Integer AI.Target.lastX
DIM Integer AI.Target.lastY

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
DIM Float   AI.Path.X(Map.W+MAP.H)
DIM Float   AI.Path.Y(Map.W+MAP.H)


sub AI.off()
  AI.State=AISTATE_OFF
end sub

sub AI.install(idx%)
  AI.Panzer=idx%
  AI.Panzer.homeX=Panzer.X(idx%)
  AI.Panzer.homeY=Panzer.Y(idx%)
  AI.State=AISTATE_WAIT
  AI.AStar.State=ASTAR_NOTREADY
  AI.Flags=1<<AIFLG_HOLD
end sub

sub AI.reset()
  if AI.State=AISTATE_OFF then exit sub

  AI.State=AISTATE_WAIT
  AI.AStar.State=ASTAR_NOTREADY
  AI.Path.next=0
  AI.Path.length=0
  AI.Flags=0
end sub

sub AI.sendSignal(signal%)
  LOCAL Integer flag

  if AI.State=AISTATE_OFF then exit sub

  select case signal%
  case AISIG_AI_DEAD
    AI.State=AISTATE_DESTROYED
    AI.timRepair=TIMER

  case AISIG_AI_ALIVE
    flag=tst(AI.Flags,AIFLG_TARGET_DEAD)
    AI.State=choice(flag,AISTATE_GOHOME,AISTATE_MOVE)

  case AISIG_TARGET_DEAD
    set AI.Flags,AIFLG_TARGET_DEAD
    if AI.State<>AISTATE_DESTROYED then AI.State=AISTATE_GOHOME

  case AISIG_TARGET_ALIVE
    clr AI.Flags,AIFLG_TARGET_DEAD
    if AI.State<>AISTATE_DESTROYED then AI.AStar.State=ASTAR_NOTREADY  'trigger new path

  case AISIG_HOLD
    set AI.Flags,AIFLG_HOLD

  case AISIG_RELEASE
    clr AI.Flags,AIFLG_HOLD

  end select
end sub

'x2/y2=target
sub AI.update(x2!,y2!)
  STATIC Float tim
  LOCAL Float x1,y1,px,py,gx,gy
  LOCAL Integer n

[AI] LOCAL String aistates(5)=("off","wait","move","destroyed","go home","at home")
[AI] LOCAL String astarstates(5)=("notready","ready","running","complete","nopath","paused")
[AI] for n=0 to AI.Path.length-1
[AI]   circle AI.Path.X(n),AI.Path.Y(n),2,0,1.0,map(2),map(2)
[AI] next
[AI] text 5, 5,"State      : "+aistates(AI.State),"L",7,,map(7)
[AI] text 5,13,"AStar State: "+astarstates(AI.AStar.State),"L",7,,map(7)

  if AI.State=AISTATE_OFF then exit sub

  AI.findPath(x2!,y2!) 'find a path to target
  x1=Panzer.X(AI.Panzer)     : y1=Panzer.Y(AI.Panzer)
  px=AI.Path.X(AI.Path.next) : py=AI.Path.Y(AI.Path.next)

[AI] circle px,py,2,0,1.0,map(2),map(4)

  select case AI.State
  case AISTATE_WAIT
    if NOT tst(AI.Flags,AIFLG_HOLD) then
      if tst(AI.Flags,AIFLG_ATTACK) then
        clr AI.Flags,AIFLG_ATTACK
        AI.State=AISTATE_MOVE
      endif
    endif

  case AISTATE_MOVE
    if NOT tst(AI.Flags,AIFLG_TARGET_DEAD) then
      if AI.findTarget(x2!,y2!) then Panzer.fire(AI.Panzer)
      gx=int(Map.toMapX(x2!)/MAPTILESIZE) : gy=int(Map.toMapY(y2!)/MAPTILESIZE)
      if AI.Target.lastX=0 then AI.Target.lastX=gx : AI.Target.lastY=gy
      if Math.distance(AI.Target.lastX,AI.Target.lastY,gx,gy)>AI.PATHMINDIST then
        AI.Target.lastX=gx : AI.Target.lastY=gy
        AI.AStar.State=ASTAR_NOTREADY  'trigger new path
      endif
    else
      n=AI.turnTurret0()
    endif

    if AI.turnPanzer(px,py) then
      Panzer.move(AI.Panzer,0.5)
      if AI.Path.length=AI.Path.next then
        if tst(AI.Flags,AIFLG_TARGET_DEAD) then
          AI.State=AISTATE_ATHOME
        else
          AI.State=AISTATE_WAIT
          AI.AStar.State=ASTAR_NOTREADY
        endif
      else if Math.distance(x1,y1,px,py)<16 then
        inc AI.Path.next
      endif
    endif
  
  case AISTATE_DESTROYED
    if TIMER-AI.timRepair>AI.REPAIRDELAY then n=Panzer.repair(AI.Panzer)

  case AISTATE_GOHOME
    AI.prepareAStar(Panzer.X(AI.Panzer),Panzer.Y(AI.Panzer),AI.Panzer.homeX,AI.Panzer.homeY)
    AI.runAStar()
    AI.State=AISTATE_MOVE

  case AISTATE_ATHOME
    if AI.turnPanzer(x2!,py) then
      clr AI.Flags,AIFLG_ATTACK
      AI.State=AISTATE_WAIT
    endif

  end select
end sub

function AI.findTarget(x2!,y2!) as Integer
  LOCAL Integer n,blocked,locked
  LOCAL Float rx,ry,rw,rh
  LOCAL Float x1,y1

  x1=Panzer.X(AI.Panzer) : y1=Panzer.Y(AI.Panzer)

  for n=0 to Map.Obj.next-1
    rx=Map.Obj(n,0) : ry=Map.Obj(n,1)
    rw=Map.Obj(n,2) : rh=Map.Obj(n,3)

    if Math.intersect(x1,y1,x2!,y2!,rx   ,ry   ,rx+rw,ry)    then blocked=1 : exit for
    if Math.intersect(x1,y1,x2!,y2!,rx+rw,ry   ,rx+rw,ry+rh) then blocked=1 : exit for
    if Math.intersect(x1,y1,x2!,y2!,rx+rw,ry+rh,rx   ,ry+rh) then blocked=1 : exit for
    if Math.intersect(x1,y1,x2!,y2!,rx   ,ry+rh,rx   ,ry)    then blocked=1 : exit for
  next
[AI] line x1,y1,x2!,y2!,1,map(choice(blocked,1,2))

  locked=AI.turnTurret(x2!,y2!)
  AI.findTarget=NOT blocked AND locked
end function

'x2,y2=target position
sub AI.findPath(x2!,y2!)
  LOCAL Float x1,y1

  if AI.AStar.State=ASTAR_PAUSED then exit sub
  x1=Panzer.X(AI.Panzer) : y1=Panzer.Y(AI.Panzer)

  select case AI.AStar.State
  case ASTAR_NOTREADY
    AI.prepareAStar(x1,y1,x2!,y2!)
  case ASTAR_READY,ASTAR_RUNNING
    AI.runAStar()
  case ASTAR_COMPLETE
    AI.extractPath()
    set AI.Flags,AIFLG_ATTACK
    AI.AStar.State=ASTAR_PAUSED
  case ASTAR_NOPATH
    AI.AStar.State=ASTAR_NOTREADY
  end select
end sub

sub AI.extractPath()
  LOCAL Integer path(2*(MAP.W+MAP.H))
  LOCAL Integer idx,px,py,nx,ny
  LOCAL Float offsX,offsY

  px=AI.AStar.gpx : py=AI.AStar.gpy : idx=0
  do while NOT (px=AI.AStar.spx AND py=AI.AStar.spy)
    nx=AI.AStar.predX(px,py) : ny=AI.AStar.predY(px,py)
    path(idx  )=nx : px=nx
    path(idx+1)=ny : py=ny
    inc idx,2
  loop
  AI.Path.length=idx/2-2

  for idx=0 to AI.Path.length
    px=path(2*(AI.Path.length-idx))
    py=path(2*(AI.Path.length-idx)+1)
    offsX=MAPTILESIZE/2 : offsY=MAPTILESIZE/2

    if px-1>=0 and px+1<MAP.W and py-1>=0 and py+1<MAP.H then
      if Map.map(px+1,py)>0 then
        inc offsX,-4
        if Map.map(px+1,py+1)=-1 then inc offsY,4
        if Map.map(px+1,py-1)=-1 then inc offsY,-4
      endif
      if Map.map(px-1,py)>0 then
        inc offsX,4
        if Map.map(px-1,py+1)=-1 then inc offsY,4
        if Map.map(px-1,py-1)=-1 then inc offsY,-4
      endif
      if Map.map(px,py+1)>0 then
        inc offsY,-4
        if Map.map(px-1,py+1)=-1 then inc offsX,-4
        if Map.map(px+1,py+1)=-1 then inc offsX,4
      endif
      if Map.map(px,py-1)>0 then
        inc offsY,4
        if Map.map(px-1,py-1)=-1 then inc offsX,-4
        if Map.map(px+1,py-1)=-1 then inc offsX,4
      endif
    endif

    AI.Path.X(idx)=Map.toScrX(px*MAPTILESIZE+offsX)
    AI.Path.Y(idx)=Map.toScrY(py*MAPTILESIZE+offsY)
  next
  AI.Path.next=0
end sub

'x2,y2=target
function AI.turnTurret(x2!,y2!) as Integer
  LOCAL Float x1,y1,alpha,beta
  LOCAL Integer dir

  x1=Panzer.X(AI.Panzer) : y1=Panzer.Y(AI.Panzer)

  alpha=DEG(atan2(x2!-x1,y1-y2!))
  if alpha<0 then inc alpha,360
  beta =Panzer.phi(AI.Panzer)+Panzer.turm(AI.Panzer)
  if beta>360 then inc beta,-360

  if abs(alpha-beta)>1 then
    dir=cfr(choice(abs(alpha-beta)>180,1,-1))
    Panzer.cannon AI.Panzer,choice(beta>alpha,dir,-dir)
  else
    AI.turnTurret=1
  endif
end function

function AI.turnTurret0() as Integer
  if Panzer.turm(AI.Panzer)<>0 then
    Panzer.cannon AI.Panzer,cfr(choice(Panzer.turm(AI.Panzer)>180,1,-1))
  else
    AI.turnTurret0=1
  endif
end function

'x2,y2=target
function AI.turnPanzer(x2!,y2!) as Integer
  LOCAL Integer dir
  LOCAL Float x1,y1,alpha,beta

  x1=Panzer.X(AI.Panzer) : y1=Panzer.Y(AI.Panzer)

  alpha=DEG(atan2(x2!-x1,y1-y2!))
  if alpha<0 then inc alpha,360
  beta =Panzer.phi(AI.Panzer)
  if beta>360 then inc beta,-360

  if abs(alpha-beta)>1 then
    dir=choice(abs(alpha-beta)>180,1,-1)
    dir=choice(beta>alpha,dir,-dir)
    Panzer.turn AI.Panzer,dir
  else
    AI.turnPanzer=1
  endif
end function

sub AI.prepareAStar(sx%,sy%,gx%,gy%)
  LOCAL Integer _sx,_sy,_gx,_gy
  LOCAL Integer x,y

  _sx=int(Map.toMapX(sx%)/MAPTILESIZE) : AI.AStar.spx=_sx
  _sy=int(Map.toMapY(sy%)/MAPTILESIZE) : AI.AStar.spy=_sy
  _gx=int(Map.toMapX(gx%)/MAPTILESIZE) : AI.AStar.gpx=_gx
  _gy=int(Map.toMapY(gy%)/MAPTILESIZE) : AI.AStar.gpy=_gy

  for x=0 to MAP.W-1  ' Initialize scores
    for y=0 to MAP.H-1
      AI.AStar.gScore(x,y)=9999
      AI.AStar.fScore(x,y)=9999
    next
  next
  for x=0 to MAP.W*MAP.H-1
    AI.AStar.closedList(x)=0
  next

  ' Setup start node
  AI.AStar.gScore(_sx,_sy)=0
  AI.AStar.fScore(_sx,_sy)=ABS(_gx-_sx) + ABS(_gy-_sy)
  AI.AStar.oc=1
  AI.AStar.openList(AI.AStar.oc)=_sx+_sy*Map.W ' encode coordinates
  AI.AStar.state=ASTAR_READY
end sub

sub AI.runAStar()
  LOCAL Integer lowest,moveCost,tentativeG
  LOCAL Integer current,cx,cy,dx,dy,nx,ny,lx,ly
  LOCAL Integer n,found,slice

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
            if AI.AStar.openList(n)=nx+ny*MAP.W then found = 1 : exit for
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

