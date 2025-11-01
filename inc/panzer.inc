'Panzer.inc

CONST PANZER_MAX=4
CONST PANZER_RELOADTIME=1000
CONST PANZER_REPAIRTIME=100
CONST PANZER_REPAIRSTEP=10

CONST PSTATE_NONE=0
CONST PSTATE_ACTIVE=1
CONST PSTATE_DESTROYED=2
CONST PSTATE_REPAIR=3


DIM Integer Panzer.W=32
DIM Integer Panzer.Core.W=Panzer.W-8
DIM Integer Panzer.H=32
DIM Integer Panzer.Core.H=Panzer.H-4
DIM Integer Panzer.Sound

DIM Integer Panzer.State(PANZER_MAX)
DIM Float Panzer.X(PANZER_MAX)
DIM Float Panzer.Y(PANZER_MAX)
DIM Float Panzer.phi(PANZER_MAX)
DIM Float Panzer.turm(PANZER_MAX)
DIM Float Panzer.tiReload(PANZER_MAX)
DIM Float Panzer.tiRepair(PANZER_MAX)
DIM Float Panzer.tiStill(PANZER_MAX)
DIM Float Panzer.cycle(PANZER_MAX)
DIM Float Panzer.robot(PANZER_MAX)


'Sets the object into initial state
sub Panzer.init()
  LOCAL Integer idx

  for idx=0 to PANZER_MAX-1
    Panzer.State(idx)=PSTATE_NONE
    Panzer.X(idx)=0
    Panzer.Y(idx)=0
    Panzer.phi(idx)=0
    Panzer.turm(idx)=0
    Panzer.tiReload(idx)=0
    Panzer.tiRepair(idx)=0
    Panzer.tiStill(idx)=0
    Panzer.cycle(idx)=1
    Panzer.robot(idx)=0
  next
end sub

sub Panzer.set(panzer%,x!,y!,phi!)
  Panzer.State(panzer%)=PSTATE_ACTIVE
  Panzer.X(panzer%)=x!
  Panzer.Y(panzer%)=y!
  Panzer.phi(panzer%)=choice(phi!<0,phi!+360,phi!)
  Panzer.turm(panzer%)=0
  Panzer.tiStill(panzer%)=TIMER
  Panzer.robot(panzer%)=0
end sub

sub Panzer.turn(panzer%,angle!)
  LOCAL float phi=Panzer.phi(panzer%)

  inc phi,angle!
  if phi>360 then inc phi,-360
  if phi<0 then inc phi,360
  Panzer.phi(panzer%)=phi
end sub

sub Panzer.move(panzer%,delta!)
  LOCAL Float rect1(7),rect2(7)
  LOCAL Float x,y,px,py,cw,ch
  LOCAL Integer idx
  LOCAL Float pd,nd

  x=Panzer.X(panzer%) : y=Panzer.Y(panzer%)
  cw=Panzer.Core.W : ch=Panzer.Core.H
  px=x : py=y

  inc x,cfr(delta!*sin(RAD(Panzer.phi(panzer%))))
  inc y,-cfr(delta!*cos(RAD(Panzer.phi(panzer%))))

  'Stop, if playfield borders reached
  if x<=Playfield.X+Panzer.W/2 or x>Playfield.X+Playfield.W-Panzer.W/2 then exit sub
  if y<=Playfield.Y+Panzer.H/2 or y>Playfield.Y+Playfield.H-Panzer.H/2 then exit sub

  _makeRectangle rect1(),x,y,cw,ch,RAD(Panzer.phi(panzer%))

  'Stop, if we bump into a house
  if Panzer.robot(panzer%)=0 AND Map.hitByPanzer(rect1(),x,y,px,py) then exit sub

  'Stop, if we bump into another tank
  for idx=0 to PANZER_MAX-1
    if Panzer.State(idx)=PSTATE_NONE then continue for
    if idx=panzer% then continue for

    pd=_distance(px,py,Panzer.X(idx),Panzer.Y(idx))
    nd=_distance( x, y,Panzer.X(idx),Panzer.Y(idx))
    if nd<pd then if Panzer.hitByPanzer(rect1(),idx) then exit sub
  next

  Panzer.tiStill(panzer%)=TIMER
  Panzer.X(panzer%)=x : Panzer.Y(panzer%)=y
end sub

function Panzer.hitByPanzer(rect1!(),idx%) as Integer
  LOCAL Float rect2!(7)

  _makeRectangle rect2!(),Panzer.X(idx%),Panzer.Y(idx%),Panzer.Core.W,Panzer.Core.H,RAD(Panzer.phi(idx%))
  Panzer.hitByPanzer=choice(_doOverlap(rect1!(),rect2!()),1,0)
end function

sub Panzer.cannon(panzer%,angle!)
  LOCAL float phi
 
  phi=Panzer.turm(panzer%)+angle!
  if phi>360 then inc phi,-360
  if phi<0 then inc phi,360
  Panzer.turm(panzer%)=phi
end sub

sub Panzer.fire(idx%)
  if TIMER-Panzer.tiReload(idx%)>PANZER_RELOADTIME then
    Shots.fire(idx%,Panzer.X(idx%),Panzer.Y(idx%),Panzer.phi(idx%)+Panzer.turm(idx%))
    Panzer.tiReload(idx%)=TIMER
  endif  
end sub

function Panzer.repair(idx%) as Integer
  if Panzer.State(idx%)=PSTATE_DESTROYED then
    Panzer.State(idx%)=PSTATE_REPAIR
    Panzer.tiRepair(idx%)=TIMER
    Panzer.cycle(idx%)=1
    Panzer.repair=1
  endif
end function

sub Panzer.draw()
  LOCAL Integer idx,img,fk,n
  LOCAL Float x,y,w,h,phi

  for idx=0 to PANZER_MAX-1
    if Panzer.State(idx)=PSTATE_NONE then continue for

    select case Panzer.State(idx)
    case PSTATE_ACTIVE
      img=0
      if TIMER-Panzer.tiStill(idx)>60000 then Drohne.start Panzer.X(idx),Panzer.Y(idx)
    case PSTATE_DESTROYED
      img=1
    case PSTATE_REPAIR
      img=1

      if TIMER-Panzer.tiRepair(idx)>PANZER_REPAIRTIME then
        Panzer.tiRepair(idx)=TIMER
        inc Panzer.cycle(idx)

        if Panzer.cycle(idx)>PANZER_REPAIRTIME/PANZER_REPAIRSTEP then
          Panzer.State(idx)=PSTATE_ACTIVE
          Panzer.tiStill(idx)=TIMER
          for n=0 to 7
            Shots.fire(idx,Panzer.X(idx),Panzer.Y(idx),RND()*20+n*45-10)
          next
        endif
      endif
      if TIMER-Panzer.tiRepair(idx)<PANZER_REPAIRSTEP*Panzer.cycle(idx) then img=0
    end select

    w=PANZER.W : h=PANZER.H : x=Panzer.X(idx)-w/2 : y=Panzer.Y(idx)-h/2 : phi=Panzer.phi(idx)

    select case img
    case 0
      image rotate_fast w*idx,0,w,h,x,y,phi,PAGE_SPRITES,1
      inc phi,Panzer.turm(idx)
      image rotate_fast w*idx,32,w,h,x,y,phi,PAGE_SPRITES,1
    case 1
      image rotate_fast w*idx,64,w,h,x,y,phi,PAGE_SPRITES,1
    end select
  next
  Panzer.playSound Panzer.Sound
end sub

sub Panzer.destroy(idx%)
  if Panzer.State(idx%)=PSTATE_DESTROYED then
    Explosion.add Panzer.X(idx%),Panzer.Y(idx%),,EX_TYPE_SMALL
  else
    Explosion.add Panzer.X(idx%),Panzer.Y(idx%),,EX_TYPE_BIG
    Panzer.State(idx%)=PSTATE_DESTROYED
  endif
end sub

function Panzer.isHit(x!,y!) as Integer
  LOCAL Float x1,y1,x2,y2,W,H
  LOCAL Integer idx

  W=Panzer.W-2 : H=Panzer.H-2

  for idx=0 to PANZER_MAX-1
    if Panzer.State(idx)=PSTATE_NONE then continue for
    
    x1=Panzer.X(idx)-W/2 : x2=x1+W
    y1=Panzer.Y(idx)-H/2 : y2=y1+H

    if x!>x1 and x!<x2 and y!>y1 and y!<y2 then
       Panzer.isHit=idx
       exit function
    endif
  next 
  Panzer.isHit=-1
end function

function Panzer.isActive(idx%) as Integer
  Panzer.isActive=(Panzer.State(idx%)=PSTATE_ACTIVE)
end function

function Panzer.isRepair(idx%) as Integer
  Panzer.isRepair=(Panzer.State(idx%)=PSTATE_REPAIR)
end function

function Panzer.active() as Integer
  LOCAL Integer idx,n

  for idx=0 to PANZER_MAX-1
    if Panzer.State(idx)=PSTATE_ACTIVE then inc n
  next
  Panzer.active=n
end function

sub _makeRectangle(rect!(),x!,y!,w!,h!,phi!)
  LOCAL Float ca,sa,dx,dy

  ca=cos(phi!) : sa=sin(phi!) : dx=w!/2 : dy=h!/2

  rect!(0)=x! + (ca * -dx - sa * -dy)
  rect!(1)=y! + (sa * -dx + ca * -dy)
  rect!(2)=x! + (ca * dx - sa * -dy)
  rect!(3)=y! + (sa * dx + ca * -dy)
  rect!(4)=x! + (ca * dx - sa * dy)
  rect!(5)=y! + (sa * dx + ca * dy)
  rect!(6)=x! + (ca * -dx - sa * dy)
  rect!(7)=y! + (sa * -dx + ca * dy) 
end sub

function _doOverlap(rect1!(),rect2!()) as Integer
  LOCAL Float axes(7), axisX, axisY
  LOCAL Float r1min,r1max,r2min,r2max
  LOCAL Float dot, overlap
  LOCAL Integer i,j

  for i=0 to 6 step 2
    axisX=rect1!((i+2) MOD 8) - rect1!(i)
    axisY=rect1!((i+3) MOD 8) - rect1!(i+1)
    if i<4 then axes(i)=-axisY : axes(i+1)=axisX

    axisX=rect2!((i+2) MOD 8) - rect2!(i)
    axisY=rect2!((i+3) MOD 8) - rect2!(i+1)
    if i<4 then axes(4+i)=-axisY : axes(5+i)=axisX
  next

  for i=0 to 6 step 2
    axisX=axes(i) : axisY=axes(i+1)

    r1min=1E9 : r1max=-1E9
    r2min=1E9 : r2max=-1E9
    for j=0 to 6 step 2
      dot=rect1!(j)*axisX + rect1!(j+1)*axisY
      r1min=MIN(r1min,dot)
      r1max=MAX(r1max,dot)

      dot=rect2!(j)*axisX + rect2!(j+1)*axisY
      r2min=MIN(r2min,dot)
      r2max=MAX(r2max,dot)
    next

    overlap=NOT (r1max<r2min or r2max<r1min)
    if overlap=0 then exit for
  next

  _doOverlap=overlap
end function

sub Panzer.playSound arg%
  STATIC Float Interval
  STATIC Integer state=0 '=off

  if (state=0 and arg%=1) then
    state=1 '=on
    playSample 5,11025,1
    Interval=TIMER

  elseif (state=1 and arg%=0) then
    state=0 '=off
    playSample 14,22050,1

  else if (state=1 and TIMER-Interval>5000) then
    playSample 5,11025,1
    Interval=TIMER
  endif
end sub






